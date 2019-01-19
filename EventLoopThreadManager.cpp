#include "EventLoopThreadManager.h"

int EventLoopThreadManager::next;
void EventLoopThreadManager::newEventLoopThread(int num) {
	if (num != 0 && ploops.empty()) {
		next = 0;
	}
	ploops.push_back(new EventLoop());
	Singleton<ThreadManager>::instance().newThread(num,
		std::bind(EventLoop::loop, ploops.back()));
}

std::shared_ptr<EventLoop>
EventLoopThreadManager::getNextEventLoop() {
	if (ploops.empty()) {
		return nullptr;
	}
	if (next == ploops.size() - 1) {
		next = 0;
		return ploops.back();
	}
	return ploops[next++];
}
