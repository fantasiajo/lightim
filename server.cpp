#include <iostream>
#include "Singleton.h"
#include "EventLoopThreadManager.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include "easylogging++.h"

#define ELPP_THREAD_SAFE

INITIALIZE_EASYLOGGINGPP

int main() {
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%datetime %thread %level: %msg");

	LOG(INFO) << "Welcome to lightim!";
	EventLoop loop;
	Singleton<EventLoopThreadManager>::instance().newEventLoopThread(4);
	Singleton<TcpServer>::instance().init(&loop, "0.0.0.0", 4399);
	Singleton<TcpServer>::instance().start();
	//TcpServer *pServer = &Singleton<TcpServer>::instance();
	loop.loop();
	return 0;
}
