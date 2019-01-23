#include "Epoller.h"
#include "unistd.h"
#include "util.h"
#include "IOEventManager.h"
#include <iostream>
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
			std::cerr << "epfd not valid.\n";
		}
		if (errno == EFAULT) {
			std::cerr << "Write forbidden.\n";
		}
		if (errno == EINTR) {
			std::cerr << "Interrupted.\n";
		}
		if (errno == EINVAL) {
			std::cerr << "hehe.\n";
		}
		std::cerr << "epoll_wait failed." << std::endl;
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
			std::cerr << "Add epoll fd failed.\n";
			exit(1);
		}
		FdIOEM[pIOEM->getfd()] = pIOEM;
	}
	else {
		tmpev.events = pIOEM->getEvents();
		tmpev.data.ptr = pIOEM;
		if (::epoll_ctl(epoll_fd, EPOLL_CTL_MOD, pIOEM->getfd(), &tmpev) == -1) {
			//log.err << epoll_ctl
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

