#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <thread>
#include <vector>
#include <memory>

class ThreadManager {
public:
	typedef std::function<void()> Task;

	ThreadManager();
	~ThreadManager();

	void newThread(int num, Task task);

private:
	std::vector<std::shared_ptr<std::thread>> pthreads;
};

#endif
