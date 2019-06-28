#include "util.h"
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include "easylogging++.h"
#include "LogManager.h"
#include "Singleton.h"

int createEventFd() {
	int fd = eventfd(0, 0);
	if (fd == -1) {
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, "create eventfd failed.");
	}
	return fd;
}

int createEpollFd() {
	int fd = epoll_create(1);
	if (fd == -1) {
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, "create epollfd failed.");
	}
	return fd;
}
