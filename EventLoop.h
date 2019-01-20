#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "Epoller.h"
#include "IOEventManager.h"
#include "util.h"
#include <vector>
#include <functional>
#include <mutex>

typedef std::function<void()> Task;

class EventLoop {
public:
	EventLoop();
	~EventLoop();

	void loop();

	void updateIOEM(IOEventManager *pIOEM);
private:

	//成员变量
	std::shared_ptr<Epoller> pEpoller;
	std::vector<Task> tasks;
	std::mutex mtx_tasks;

	int event_fd;
	IOEventManager event_fd_ioem;
	void readEventFd();

	//成员函数
	inline void add_task(Task &task);
	void do_pending_task();

	std::vector<IOEventManager *> activeIOEM;
	
};
#endif
