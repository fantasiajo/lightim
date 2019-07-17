#include "EventLoopThreadManager.h"
#include "EventLoop.h"
#include <mutex>

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
	if (next == ploops.size()) {
		next = 0;
	}
	return ploops[next++].get();
}

EventLoop* EventLoopThreadManager::getEventLoopById(uint32_t id){
	return ploops[id%ploops.size()].get();
}

void EventLoopThreadManager::threadFun() {
	EventLoop *ploop = nullptr;
	{
		std::unique_lock<std::mutex> lck(mtx_ploops);
		ploops.emplace_back(new EventLoop());
		ploop = ploops.back().get();
		//std::cout << std::this_thread::get_id() << ": running in ploops " << ploops.size() - 1 << " " << ploops.back().get() << std::endl;
	}

	ploop->loop();
	//ploops.erase()
}
