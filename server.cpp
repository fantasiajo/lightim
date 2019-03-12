#include <iostream>
#include "Singleton.h"
#include "EventLoopThreadManager.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include "easylogging++.h"

#define ELPP_THREAD_SAFE//启用easylogging多线程支持

INITIALIZE_EASYLOGGINGPP//初始化easylogging

int main() {
	//配置日志格式
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%datetime %thread %level: %msg");
	LOG(INFO) << "Welcome to lightim!";
	EventLoop loop;
	Singleton<EventLoopThreadManager>::instance().newEventLoopThread(4);
	Singleton<TcpServer>::instance().init(&loop, "0.0.0.0", 4399);
	Singleton<TcpServer>::instance().start();
	loop.loop();
	return 0;
}
