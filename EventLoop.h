#pragma once

#include "util.h"
#include <vector>
#include <functional>
#include <mutex>
#include <thread>
#include "MsgCache.h"
#include <unordered_map>
#include <unordered_set>

typedef std::function<void()> Task;

class DB;
class Epoller;
class IOEventManager;
class DataManager;
class TcpConnection;

class EventLoop
{
public:
	EventLoop();
	~EventLoop() {}

	EventLoop(const EventLoop &) = delete;
	EventLoop &operator=(const EventLoop &) = delete;
	EventLoop(EventLoop &&) = delete;
	EventLoop &operator=(EventLoop &&) = delete;

	void loop();

	void addIOEM(IOEventManager *pIOEM);
	void updateIOEM(IOEventManager *pIOEM);
	void deleteIOEM(IOEventManager *pIOEM);

	void runInLoop(const Task &task);
	void runInLoop(std::vector<Task> &);
	bool isInLoopThread();
	void queueInLoop(const Task &task);
	void queueInLoop(std::vector<Task> &);

	std::weak_ptr<MsgCache> getPMsgCache()
	{
		return pMsgCache;
	}

	std::weak_ptr<TcpConnection> getConnById(uint32_t id)
	{
		auto it = id2conn.find(id);
		if (it == id2conn.end())
		{
			return std::shared_ptr<TcpConnection>();
		}
		return it->second;
	}

	void setConnById(uint32_t id, std::shared_ptr<TcpConnection> pTcpConn)
	{
		id2conn[id] = pTcpConn;
	}

	std::weak_ptr<DataManager> getpDM()
	{
		return pDM;
	}

	void closeConnection(std::weak_ptr<TcpConnection> pTcpConn)
	{
		pTcpConns.erase(pTcpConn.lock());
	}

	void addTcpConn(std::shared_ptr<TcpConnection>);
	std::shared_ptr<TcpConnection> pullTcpConn(std::shared_ptr<TcpConnection>);

private:
	//成员变量
	std::thread::id tid;

	std::shared_ptr<DataManager> pDM;

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

	std::unordered_set<std::shared_ptr<TcpConnection>> pTcpConns;
	std::unordered_map<uint32_t, std::weak_ptr<TcpConnection>> id2conn;
};
