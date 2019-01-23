#include "EventLoopThreadManager.h"
#include "EventLoop.h"

int EventLoopThreadManager::next;
void EventLoopThreadManager::newEventLoopThread(int num) {
	if (num != 0 && ploops.empty()) {
		next = 0;
	}
	//ploops.push_back(new EventLoop());
	Singleton<ThreadManager>::instance().newThread(num,
		std::bind(&EventLoopThreadManager::threadFun,this));
}

EventLoop *
EventLoopThreadManager::getNextEventLoop() {
	if (ploops.empty()) {
		return nullptr;
	}
	if (next == ploops.size() - 1) {
		next = 0;
		return ploops.back().get();
	}
	return ploops[next++].get();
}

void EventLoopThreadManager::threadFun() {
	EventLoop *ploop = nullptr;
	mtx_ploops.lock();
	ploops.emplace_back(new EventLoop());
	ploop = ploops.back().get();
	mtx_ploops.unlock();

	ploop->loop();
	//ploops.erase()
}
