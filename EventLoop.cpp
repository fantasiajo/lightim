#include "EventLoop.h"

EventLoop::EventLoop()
	:event_fd(createEventFd()),
	event_fd_ioem(this,event_fd)
{
	event_fd_ioem.enableReading();
	event_fd_ioem.setReadCallBack(readEventFd);
}

EventLoop::~EventLoop() {}

void EventLoop::loop() {
	while (true) {
		int numEvents = pEpoller->wait(activeIOEM);

		for (int i = 0; i < numEvents; ++i) {
			activeIOEM[i]->handleEvent();
		}
		//epoller.wait();
		//handle_result();
		do_pending_task();
	}
}

void EventLoop::updateIOEM(IOEventManager * pIOEM)
{
	pEpoller->updateFdIOEM(pIOEM);
}

void EventLoop::readEventFd()
{
	uint64_t n;
	if (read(event_fd, &n, sizeof(n) != sizeof(n)) {
		//log.err
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
