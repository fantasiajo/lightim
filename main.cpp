#include <iostream>
#include "Singleton.h"
#include "EventLoopThreadManager.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include "easylogging++.h"
#include "Server.h"

#define ELPP_THREAD_SAFE

INITIALIZE_EASYLOGGINGPP

int main() {
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%datetime %thread %level: %msg");

	LOG(INFO) << "Welcome to lightim!";
	EventLoop loop;
	Singleton<EventLoopThreadManager>::instance().newEventLoopThread(4);
	Singleton<Server>::instance().init(&loop, "0.0.0.0", 4399);
	Singleton<Server>::instance().start();
	//TcpServer *pServer = &Singleton<TcpServer>::instance();
	loop.loop();
	return 0;
}
