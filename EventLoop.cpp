#include "EventLoop.h"

EventLoop::EventLoop() {}

EventLoop::~EventLoop() {}

void EventLoop::loop() {
	while (true) {
		//epoller.wait();
		//handle_result();
		do_pending_task();
	}
}

void EventLoop::add_task(Task &task) {
	mtx_tasks.lock();
	tasks.push_back(task);
	mtx_tasks.unlock();
}

void EventLoop::do_pending_task() {
	std::vector<Task> tmptasks;

	mtx_tasks.lock();
	std::swap(tmpstasks, tasks);
	mtx_tasks.unlock();

	for (const auto &task : tmptasks) {
		task();
	}
}
