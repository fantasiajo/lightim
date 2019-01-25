#include "EventLoop.h"
#include "IOEventManager.h"
#include "Epoller.h"
#include "unistd.h"
#include <iostream>

EventLoop::EventLoop()
	:tid(std::this_thread::get_id()),
	pEpoller(new Epoller()),
	event_fd(createEventFd()),
	event_fd_ioem(new IOEventManager(this, event_fd))
{
	event_fd_ioem->enableReading();
	event_fd_ioem->setReadCallBack(std::bind(&EventLoop::readEventFd,this));
}

EventLoop::~EventLoop() {}

void EventLoop::loop() {
	while (true) {
		int numEvents = pEpoller->wait(activeIOEM);
		for (int i = 0; i < numEvents; ++i) {
			activeIOEM[i]->handleEvent();
		}
		do_pending_task();
	}
}

void EventLoop::updateIOEM(IOEventManager * pIOEM)
{
	pEpoller->updateFdIOEM(pIOEM);
}

void EventLoop::deleteIOEM(IOEventManager * pIOEM)
{
	pEpoller->deleteFdIOEM(pIOEM);
}

void EventLoop::readEventFd()
{
	uint64_t n;
	if (read(event_fd, &n, sizeof(n)) != sizeof(n)) {
		std::cerr << "read eventfd fail" << std::endl;
		std::cerr << errno << std::endl;
	}
}

void EventLoop::writeEventFd()
{
	uint64_t n = 1;
	if (write(event_fd, &n, sizeof(n)) != sizeof(n)) {
		std::cerr << "write eventfd fail" << std::endl;
		std::cerr << errno << std::endl;
	}
}

bool EventLoop::isInLoopThread()
{
	return tid == std::this_thread::get_id();
}

void EventLoop::runInLoop(const Task &task) {
	if (isInLoopThread()) {
		if (task)
			task();
	}
	else {
		queueInLoop(task);
	}
}

void EventLoop::queueInLoop(const Task & task)
{
	mtx_tasks.lock();
	tasks.push_back(task);
	mtx_tasks.unlock();
	wakeup();
}

void EventLoop::wakeup() {
	writeEventFd();
}

void EventLoop::do_pending_task() {
	std::vector<Task> tmptasks;

	mtx_tasks.lock();
	std::swap(tmptasks, tasks);
	mtx_tasks.unlock();

	for (const auto &task : tmptasks) {
		task();
	}
}
