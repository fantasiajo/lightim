#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "Epoller.h"
#include <vector>
#include <functional>
#include <mutex>

class Task;

class EventLoop {
public:
	EventLoop();
	~EventLoop();

	void loop();
private:

	//成员变量
	Epoller epoller;
	std::vector<Task> tasks;
	std::mutex mtx_tasks;

	//成员函数
	inline void add_task(Task &task);
	void do_pending_task();

	
};
#endif
