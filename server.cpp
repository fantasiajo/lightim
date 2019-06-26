#include <iostream>
#include "Singleton.h"
#include "EventLoopThreadManager.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include "easylogging++.h"
#include "LogManager.h"

INITIALIZE_EASYLOGGINGPP//初始化easylogging
int main() {
	//配置日志格式
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%datetime %thread %level: %msg");
	//LOG(INFO) << "Welcome to lightim!";
	EventLoop loop;
	Singleton<ThreadManager>::instance().newThread(1,
		std::bind(&LogManager::loop, &Singleton<LogManager>::instance()));
	Singleton<EventLoopThreadManager>::instance().newEventLoopThread(3);
	Singleton<TcpServer>::instance().init(&loop, "0.0.0.0", 4399);
	Singleton<TcpServer>::instance().start();
	Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::INFO_LEVEL, "Welcome to lightim!");
	loop.loop();
	return 0;
}
