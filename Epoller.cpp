#include "Epoller.h"
#include "unistd.h"
#include "util.h"
#include "IOEventManager.h"
#include "easylogging++.h"
#include <thread>

int Epoller::waittimeout = 1;

Epoller::Epoller()
	:epoll_fd(createEpollFd())
{
	evvec.resize(10);
}

Epoller::~Epoller() {
	close(epoll_fd);
}

int Epoller::wait(std::vector<IOEventManager *> &activeIOEM, int timeout /*= waittimeout*/){
	numEvents = ::epoll_wait(epoll_fd, &evvec[0], static_cast<int>(evvec.size()), timeout);
	if (numEvents == -1) {
		if (errno == EBADF) {
			LOG(ERROR) << "epfd not valid.";
		}
		if (errno == EFAULT) {
			LOG(ERROR) << "Write forbidden.";
		}
		if (errno == EINTR) {
			LOG(ERROR) << "Interrupted.";
		}
		if (errno == EINVAL) {
			LOG(ERROR) << "hehe.";
		}
		LOG(ERROR) << "epoll_wait failed.";
	}
	activeIOEM.clear();
	bool eventhappen = false;
	for (int i = 0; i < numEvents; ++i) {
		std::cout << std::this_thread::get_id() << " " << getpid() << " " << numEvents << " happen.\n";
		activeIOEM.push_back(static_cast<IOEventManager *>(evvec[i].data.ptr));
		activeIOEM.back()->setRecvEvents(evvec[i].events);
	}
	if (numEvents == evvec.size()) {
		evvec.resize(evvec.size() * 2);
	}
	return numEvents;
}

void Epoller::updateFdIOEM(IOEventManager* pIOEM) {
	if (FdIOEM.find(pIOEM->getfd()) == FdIOEM.end()) {
		tmpev.events = pIOEM->getEvents();
		tmpev.data.ptr = pIOEM;
		if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pIOEM->getfd(), &tmpev) == -1) {
			LOG(ERROR) << "epoll_ctl add " << pIOEM->getfd() << "failed.";
			exit(1);
		}
		FdIOEM[pIOEM->getfd()] = pIOEM;
	}
	else {
		tmpev.events = pIOEM->getEvents();
		tmpev.data.ptr = pIOEM;
		if (::epoll_ctl(epoll_fd, EPOLL_CTL_MOD, pIOEM->getfd(), &tmpev) == -1) {
			LOG(ERROR) << "epoll_ctl add " << pIOEM->getfd() << "failed.";
			exit(1);
		}
	}
}

void Epoller::deleteFdIOEM(IOEventManager* pIOEM) {
	if (FdIOEM.find(pIOEM->getfd()) == FdIOEM.end()) {
		//log.err << no pioem;
	}
	else {
		::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, pIOEM->getfd(), NULL);
		FdIOEM.erase(pIOEM->getfd());
	}
}

