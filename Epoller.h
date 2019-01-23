#pragma once

#include <vector>
#include <unordered_map>
#include <sys/epoll.h>

class IOEventManager;

class Epoller {
private:
	static int waittimeout;
public:
	Epoller();
	~Epoller();

	int wait(std::vector<IOEventManager *> &activeIOEM, int timeout = waittimeout);
	void updateFdIOEM(IOEventManager *);
	void deleteFdIOEM(IOEventManager *);
private:

	int epoll_fd;

	struct epoll_event tmpev;
	std::vector<struct epoll_event> evvec;
	int numEvents;//wait 检测到的事件数目

	std::unordered_map<int, IOEventManager *> FdIOEM;
};

