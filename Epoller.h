#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unordered_map>
#include <memory>
#include "IOEventManager.h"

class Epoller {
public:
	EPoller();
	~Epoller();

	int wait(std::vector<IOEventManager *> &activeIOEM, int timeout);
	void updateFdIOEM(IOEventManager *);
	void deleteFdIOEM(IOEventManager *);
private:
	static int waittimeout;

	int epoll_fd;
	int event_fd;

	struct epoll_event tmpev;
	std::vector<struct epoll_event> evvec;
	int numEvents;//wait 检测到的事件数目

	std::unordered_map<int, IOEventManager *> FdIOEM;
};

#endif
