#include "util.h"
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include "easylogging++.h"
#include "LogManager.h"
#include "Singleton.h"

int createEventFd() {
	int fd = eventfd(0, 0);
	if (fd == -1) {
		//LOG(FATAL) << "Create eventfd failed.";
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, "create eventfd failed.");
		//todo sleep
		exit(1);
	}
	return fd;
}

int createEpollFd() {
	int fd = epoll_create(1);
	if (fd == -1) {
		//LOG(FATAL) << "Create epollfd failed.";
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, "create epollfd failed.");
		//todo sleep
		exit(1);
	}
	return fd;
}
