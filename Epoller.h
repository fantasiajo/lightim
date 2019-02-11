#pragma once

#include <vector>
#include <unordered_set>
#include <sys/epoll.h>

class IOEventManager;

class Epoller {
private:
	static int waittimeout;
public:
	Epoller();
	~Epoller();

	Epoller(const Epoller &) = delete;
	Epoller &operator=(const Epoller &) = delete;
	Epoller(Epoller &&) = delete;
	Epoller &operator=(Epoller &&) = delete;

	int wait(std::vector<IOEventManager *> &activeIOEM, int timeout = waittimeout);

	void addIOEM(IOEventManager *);
	void updateIOEM(IOEventManager *);
	void deleteIOEM(IOEventManager *);
private:
	int epoll_fd;

	struct epoll_event tmpev;
	std::vector<struct epoll_event> evvec;
	int numEvents;//wait 检测到的事件数目

	std::unordered_set<IOEventManager *> IOEMset;
};

