#include "Epoller.h"

Epoller::EPoller()
	:epoll_fd(epoll_create(0)),event_fd(eventfd(0)) {
	if (epoll_fd == -1) {
		//log.err
	}
	if (event_fd == -1) {
		//log.err
	}
	tmpev.events = EPOLLIN;
	tmpev.data.fd = event_fd;
	if ((epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_fd, &tmpev)) == -1) {
		//log.err
	}

	evvec.resize(10);
}

Epoller::~Epoller() {
	close(epoll_fd);
}

int Epoller::wait(std::vector<std::shard_ptr<IOEventManager>> &activeIOEM, int timeout = waittimeout){
	numEvents = ::epoll_wait(epoll_fd, &*evvec.begin(),
		static_cast<int>(evvec.size()), timeout);
	//if (numEvents == 1 && evvec[0].data.fd == event_fd) return 0;
	activeIOEM.clear();
	bool eventhappen = false;
	for (int i = 0; i < numEvents; ++i) {
		if (evvec[i].data.fd == event_fd()) {
			eventhappen = true;
			continue;
		}
		activeIOEM.push_back(evvec[i].data.ptr);
	}
	if (eventhappen) return numEvents - 1;
	else return numEvents;
}

void Epoller::updateFdIOEM(std::shared_ptr<IOEventManager> pIOEM) {
	if (FdIOEM.find(pIOEM->getfd()) == FdIOEM.end()) {
		tmpev.events = pIOEM->getEvents();
		tmpev.data.ptr = pIOEM.get();
		::epoll_ctl(epoll_fd,EPOLL_CTL_ADD,pIOEM->getfd(),&tmpev);
		FdIOEM[pIOEM->getfd()] = pIOEM;
	}
	else {
		tempev.events = pIOEM->getfd();
		tempev.data.ptr = pIOEM.get();
		::epoll_ctl(epoll_fd,EPOLL_CTL_MOD,pIOEM->getfd(),&tmpev);
	}
}

void Epoller::deleteFdIOEM(std::shared_ptr<IOEventManager> pIOEM) {
	if (FdIOEM.find(pIOEM->getfd()) == FdIOEM.end()) {
		//log.err << no pioem;
	}
	else {
		::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, pIOEM->getfd(), NULL);
		FdIOEM.erase(pIOEM->getfd());
	}
}

