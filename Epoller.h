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

	int wait(int timeout);
	void updateFdIOEM(std::shared_ptr<IOEventManager>);
private:
	int epoll_fd;
	int event_fd;

	struct epoll_event tmpev;

	std::unordered_map<int, std::shared_ptr<IOEventManager>> FdIOEM;
};

#endif
