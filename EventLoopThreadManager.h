#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include "ThreadManager.h"
#include "Singleton.h"

class EventLoop;

class EventLoopThreadManager {
public:
	EventLoopThreadManager(){}
	~EventLoopThreadManager(){}
	EventLoopThreadManager(const EventLoopThreadManager &) = delete;
	EventLoopThreadManager & operator=(const EventLoopThreadManager &) = delete;
	EventLoopThreadManager(EventLoopThreadManager &&) = delete;
	EventLoopThreadManager & operator=(EventLoopThreadManager &&) = delete;

	void newEventLoopThread(int num);
	EventLoop *getNextEventLoop();

private:
	std::vector<std::shared_ptr<EventLoop>> ploops;
	std::mutex mtx_ploops;
	static int next;

	void threadFun();
};
