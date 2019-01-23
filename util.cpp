#include "util.h"
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <iostream>

int createEventFd() {
	int fd = eventfd(0, 0);
	if (fd == -1) {
		std::cerr << "Create eventfd failed." << std::endl;
		abort();
	}
	return fd;
}

int createEpollFd() {
	int fd = epoll_create(1);
	if (fd == -1) {
		std::cerr << "Create epollfd failed." << std::endl;
		abort();
	}
	return fd;
}
