#ifndef EventLoopThreadMANAGER_H
#define EventLoopThreadMANAGER_H

#include <vector>
#include <memory>
#include "ThreadManager.h"
#include "Singleton.h"
#include "EventLoop.h"

class EventLoopThreadManager {
public:
	EventLoopThreadManager();
	~EventLoopThreadManager();

	void newEventLoopThread(int num);
	std::shared_ptr<EventLoop> getNextEventLoop();

private:
	std::vector<std::shared_ptr<EventLoop>> ploops;
	static int next;
};

#endif
