#pragma once

#include <thread>
#include <vector>
#include <memory>
#include <functional>

class ThreadManager {
public:
	typedef std::function<void()> Task;

	ThreadManager(){}
	~ThreadManager(){}
	ThreadManager(const ThreadManager &) = delete;
	ThreadManager & operator=(const ThreadManager &) = delete;
	ThreadManager(ThreadManager &&) = delete;
	ThreadManager & operator=(ThreadManager &&) = delete;

	void newThread(int num, Task task);

private:
	std::vector<std::shared_ptr<std::thread>> pthreads;
};

