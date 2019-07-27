#include "Epoller.h"
#include "unistd.h"
#include "util.h"
#include "IOEventManager.h"
#include "easylogging++.h"
#include "LogManager.h"
#include "Singleton.h"
#include <thread>

int Epoller::waittimeout = 1;

Epoller::Epoller()
	:epoll_fd(createEpollFd())
{
	evvec.resize(10);
}

Epoller::~Epoller() {
	if (close(epoll_fd) == -1) {
		//LOG(FATAL) << "close epoll_fd" << strerror(errno);
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, std::string("close epoll_fd")+strerror(errno));
	}
}

int Epoller::wait(std::vector<IOEventManager *> &activeIOEM, int timeout /*= waittimeout*/){
	numEvents = ::epoll_wait(epoll_fd, &evvec[0], static_cast<int>(evvec.size()), timeout);
	if (numEvents == -1) {
		if (errno == EBADF) {
			//LOG(ERROR) << "epfd not valid.";
			Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::ERROR_LEVEL, "epfd not valid.");
		}
		if (errno == EFAULT) {
			//LOG(ERROR) << "Write forbidden.";
			Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::ERROR_LEVEL, "write forbidden.");
		}
		if (errno == EINTR) {
			//LOG(ERROR) << "Interrupted.";
			Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::ERROR_LEVEL, "Interrupted.");
		}
		if (errno == EINVAL) {
			//LOG(ERROR) << "hehe.";
			Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::ERROR_LEVEL, "EINVAL");
		}
		//LOG(ERROR) << "epoll_wait failed.";
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::ERROR_LEVEL, "epoll_wait failed.");
		return -1;
	}

	activeIOEM.clear();
	bool eventhappen = false;
	if (numEvents > 0) {
		//std::cout << std::this_thread::get_id() << " " << getpid() << " " << numEvents << " happen.\n";
	}
	for (int i = 0; i < numEvents; ++i) {
		activeIOEM.push_back(static_cast<IOEventManager *>(evvec[i].data.ptr));
		activeIOEM.back()->setRecvEvents(evvec[i].events);
		//std::cout << activeIOEM.back()->getfd() << " " << activeIOEM.back()->getEvents() << std::endl;
	}
	if (numEvents == evvec.size()) {
		evvec.resize(evvec.size() * 2);
	}
	return numEvents;
}

void Epoller::addIOEM(IOEventManager * pIOEM)
{
	tmpev.events = pIOEM->getEvents();
	tmpev.data.ptr = pIOEM;
	if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pIOEM->getfd(), &tmpev) == -1) {
		//LOG(ERROR) << "epoll_ctl add " << pIOEM->getfd() << "failed.";
		std::ostringstream oss;
		oss << "epoll_ctl add" << pIOEM->getfd() << "failed:" << strerror(errno);
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::ERROR_LEVEL, oss.str());
		return;
	}
	IOEMset.insert(pIOEM);
}

void Epoller::updateIOEM(IOEventManager* pIOEM) {
	if (IOEMset.find(pIOEM) == IOEMset.end()) return;
	tmpev.events = pIOEM->getEvents();
	tmpev.data.ptr = pIOEM;
	if (::epoll_ctl(epoll_fd, EPOLL_CTL_MOD, pIOEM->getfd(), &tmpev) == -1) {
		//LOG(ERROR) << "epoll_ctl mod " << pIOEM->getfd() << "failed.";
		std::ostringstream oss;
		oss << pIOEM << "epoll_ctl mod" << pIOEM->getfd() << "failed:" << strerror(errno);
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::ERROR_LEVEL, oss.str());
		return;
	}
}

void Epoller::deleteIOEM(IOEventManager* pIOEM) {
	if(::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, pIOEM->getfd(), NULL)==-1){
		std::ostringstream oss;
		oss << pIOEM << "epoll_ctl del" << pIOEM->getfd() << "failed:" << errno << strerror(errno) << epoll_fd;
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::ERROR_LEVEL, oss.str());
		return;
	}
	IOEMset.erase(pIOEM);
}

