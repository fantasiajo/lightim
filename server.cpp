#include <iostream>
#include "Singleton.h"
#include "EventLoopThreadManager.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include "easylogging++.h"
#include "LogManager.h"
#include <sys/sysinfo.h>

INITIALIZE_EASYLOGGINGPP//初始化easylogging

int main() {
	EventLoop loop;
	Singleton<ThreadManager>::instance().newThread(1,
		std::bind(&LogManager::loop, &Singleton<LogManager>::instance()));
	Singleton<EventLoopThreadManager>::instance().newEventLoopThread(2*get_nprocs()+1);
	Singleton<TcpServer>::instance(&loop, "0.0.0.0", 4399).start();
	Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::INFO_LEVEL, "Welcome to lightim!");
	loop.loop();
	return 0;
}
