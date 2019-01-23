#include "ThreadManager.h"

ThreadManager::ThreadManager() {

}

ThreadManager::~ThreadManager() {

}

void ThreadManager::newThread(int num, Task task) {
	for (int i = 0; i < num; ++i) {
		pthreads.emplace_back(new std::thread(task));
	}
}
