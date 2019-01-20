#include "Epoller.h"

Epoller::EPoller()
	:epoll_fd(epoll_create(0)) {
	if (epoll_fd == -1) {
		//log.err
	}

	evvec.resize(10);
}

Epoller::~Epoller() {
	close(epoll_fd);
	close(event_fd);
}

int Epoller::wait(std::vector<IOEventManager *> &activeIOEM, int timeout = waittimeout){
	numEvents = ::epoll_wait(epoll_fd, &*evvec.begin(), static_cast<int>(evvec.size()), timeout);
	if (numEvents == -1) {
		//log.err << epoll_wait
	}
	activeIOEM.clear();
	bool eventhappen = false;
	for (int i = 0; i < numEvents; ++i) {
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
			//log.err << epoll_ctl 
		}
		FdIOEM[pIOEM->getfd()] = pIOEM;
	}
	else {
		tempev.events = pIOEM->getfd();
		tempev.data.ptr = pIOEM.get();
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

