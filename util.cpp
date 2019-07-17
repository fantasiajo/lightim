#include "util.h"
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include "easylogging++.h"
#include "LogManager.h"
#include "Singleton.h"
#include <arpa/inet.h>

int createEventFd()
{
	int fd = eventfd(0, 0);
	if (fd == -1)
	{
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, "create eventfd failed.");
	}
	return fd;
}

int createEpollFd()
{
	int fd = epoll_create(1);
	if (fd == -1)
	{
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, "create epollfd failed.");
	}
	return fd;
}

uint64_t htonll(uint64_t n)
{
	if (htonl(0x1234) == 0x1234)
	{
		return n;
	}
	else
	{
		return ((uint64_t)htonl(n) << 32) + htonl(n >> 32);
	}
}

uint64_t ntohll(uint64_t n)
{
	if (ntohl(0x1234) == 0x1234)
	{
		return n;
	}
	else
	{
		return ((uint64_t)ntohl(n) << 32) + ntohl(n >> 32);
	}
}
