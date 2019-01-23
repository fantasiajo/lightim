#pragma once

#include "util.h"
#include <vector>
#include <functional>
#include <mutex>
#include <thread>

typedef std::function<void()> Task;

class Epoller;
class IOEventManager;

class EventLoop {
public:
	EventLoop();
	~EventLoop();

	void loop();

	void updateIOEM(IOEventManager *pIOEM);
	void deleteIOEM(IOEventManager *pIOEM);

	void runInLoop(const Task &task);
	bool isInLoopThread();
	void queueInLoop(const Task &task);
private:

	//成员变量
	std::thread::id tid;

	std::shared_ptr<Epoller> pEpoller;
	std::vector<Task> tasks;
	std::mutex mtx_tasks;

	int event_fd;
	std::shared_ptr<IOEventManager> event_fd_ioem;
	void readEventFd();
	void writeEventFd();

	//成员函数
	

	
	
	void wakeup();

	void do_pending_task();

	std::vector<IOEventManager *> activeIOEM;
};
