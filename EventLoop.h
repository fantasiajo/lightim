#pragma once

#include "util.h"
#include <vector>
#include <functional>
#include <mutex>
#include <thread>
#include "MsgCache.h"

typedef std::function<void()> Task;

class DB;
class Epoller;
class IOEventManager;

class EventLoop {
public:
	EventLoop();
	~EventLoop(){}

	EventLoop(const EventLoop &) = delete;
	EventLoop &operator=(const EventLoop &) = delete;
	EventLoop(EventLoop &&) = delete;
	EventLoop &operator=(EventLoop &&) = delete;

	void loop();

	void addIOEM(IOEventManager *pIOEM);
	void updateIOEM(IOEventManager *pIOEM);
	void deleteIOEM(IOEventManager *pIOEM);

	void runInLoop(const Task &task);
	bool isInLoopThread();
	void queueInLoop(const Task &task);

	DB *getDb() {
		return pDb.get();
	}

	std::weak_ptr<MsgCache> getPMsgCache(){
		return pMsgCache;
	}
private:

	//成员变量
	std::thread::id tid;

	std::shared_ptr<DB> pDb;

	std::shared_ptr<Epoller> pEpoller;
	std::vector<Task> tasks;
	std::mutex mtx_tasks;

	int event_fd;
	std::shared_ptr<IOEventManager> event_fd_ioem;
	
	std::shared_ptr<MsgCache> pMsgCache;

	void readEventFd();
	void writeEventFd();

	//成员函数
	void wakeup();

	void do_pending_task();

	std::vector<IOEventManager *> activeIOEM;
};
