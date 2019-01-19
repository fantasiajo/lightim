#include "Epoller.h"

Epoller::EPoller()
	:epoll_fd(epoll_create(0)),event_fd(eventfd(0)) {
	if (epoll_fd == -1) {
		//log.err
	}
	if (event_fd == -1) {
		//log.err
	}
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = event_fd;
	if ((epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_fd, &ev)) == -1) {
		//log.err
	}
}

Epoller::~Epoller() {
	close(epoll_fd);
}

int Epoller::wait(int timeout){
	//todo
}

void Epoller::updateFdIOEM(std::shared_ptr<IOEventManager> pIOEM) {
	if (FdIOEM.find(pIOEM->getfd() == FdIOEM.end()) {
		tmpev.events = pIOEM->getEvents();
		tmpev.data.ptr = pIOEM.get();
		::epoll_ctl(epoll_fd,EPOLL_CTL_ADD,pIOEM->getfd(),&tmpev);
	}

}

