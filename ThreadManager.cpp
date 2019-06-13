#include "ThreadManager.h"

void ThreadManager::newThread(int num, Task task) {
	for (int i = 0; i < num; ++i) {
		pthreads.emplace_back(new std::thread(task));
	}
}
