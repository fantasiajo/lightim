#include "EventLoop.h"
#include "IOEventManager.h"
#include "Epoller.h"
#include "unistd.h"
#include "DB.h"
#include "easylogging++.h"
#include <mutex>
#include <cerrno>
#include "LogManager.h"
#include "Singleton.h"
#include "DataManager.h"
#include "TcpConnection.h"

EventLoop::EventLoop()
	:tid(std::this_thread::get_id()),
	pEpoller(new Epoller()),
	event_fd(createEventFd()),
	event_fd_ioem(new IOEventManager(this, event_fd)),
	pDM(new DataManager(this)),
	pMsgCache(new MsgCache(this))
{
	event_fd_ioem->enableReading();
	event_fd_ioem->setReadCallBack(std::bind(&EventLoop::readEventFd,this));
}

void EventLoop::loop() {
	while (true) {
		int numEvents = pEpoller->wait(activeIOEM);
		for (int i = 0; i < numEvents; ++i) {
			activeIOEM[i]->handleEvent();
		}
		do_pending_task();
	}
}



void EventLoop::addIOEM(IOEventManager * pIOEM)
{
	if (isInLoopThread()) {
		pEpoller->addIOEM(pIOEM);
	}
	else {
		queueInLoop(std::bind(&Epoller::addIOEM, pEpoller.get(), pIOEM));
	}
}

void EventLoop::updateIOEM(IOEventManager * pIOEM)
{
	if (isInLoopThread()) {
		pEpoller->updateIOEM(pIOEM);
	}
	else {
		queueInLoop(std::bind(&Epoller::updateIOEM, pEpoller.get(), pIOEM));
	}
}

void EventLoop::deleteIOEM(IOEventManager * pIOEM)
{
	if (isInLoopThread()) {
		pEpoller->deleteIOEM(pIOEM);
	}
	else {
		queueInLoop(std::bind(&Epoller::deleteIOEM, pEpoller.get(), pIOEM));
	}
}

void EventLoop::readEventFd()
{
	uint64_t n;
	if (read(event_fd, &n, sizeof(n)) != sizeof(n)) {
		//LOG(FATAL) << "read eventfd fail:" << strerror(errno);
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL,
			std::string("read eventfd fail: ") + strerror(errno));
		//std::cerr << "read eventfd fail" << std::endl;
		//std::cerr << errno << std::endl;
	}
}

void EventLoop::writeEventFd()
{
	uint64_t n = 1;
	if (write(event_fd, &n, sizeof(n)) != sizeof(n)) {
		//LOG(FATAL) << "write eventfd fail:" << strerror(errno);
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL,
			std::string("write eventfd fail: ") + strerror(errno));
		//std::cerr << "write eventfd fail" << std::endl;
		//std::cerr << errno << std::endl;
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
	std::unique_lock<std::mutex> lck(mtx_tasks);
	tasks.push_back(task);
	wakeup();
}

void EventLoop::runInLoop(std::vector<Task> &tasks_) {
	if (isInLoopThread()) {
		for(auto &task:tasks_){
			task();
		}
	}
	else {
		queueInLoop(tasks_);
	}
}

void EventLoop::queueInLoop(std::vector<Task> &tasks_)
{
	std::unique_lock<std::mutex> lck(mtx_tasks);
	tasks.insert(tasks.end(),tasks_.begin(),tasks_.end());
	wakeup();
}

void EventLoop::wakeup() {
	writeEventFd();
}

void EventLoop::do_pending_task() {
	std::vector<Task> tmptasks;
	{
		std::unique_lock<std::mutex> lck(mtx_tasks);
		std::swap(tmptasks, tasks);
	}
	for (const auto &task : tmptasks) {
		task();
	}
}

void EventLoop::addTcpConn(std::shared_ptr<TcpConnection> pTcpConn){
	pTcpConn->setLoop(this);
	pTcpConns.insert(pTcpConn);
}

std::shared_ptr<TcpConnection> EventLoop::pullTcpConn(std::shared_ptr<TcpConnection> pTcpConn){
	pTcpConns.erase(pTcpConn);
	return pTcpConn;
}
