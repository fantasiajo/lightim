#include "util.h"
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include "easylogging++.h"

int createEventFd() {
	int fd = eventfd(0, 0);
	if (fd == -1) {
		LOG(FATAL) << "Create eventfd failed.";
		exit(1);
	}
	return fd;
}

int createEpollFd() {
	int fd = epoll_create(1);
	if (fd == -1) {
		LOG(FATAL) << "Create epollfd failed.";
		exit(1);
	}
	return fd;
}
