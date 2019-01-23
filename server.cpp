#include <iostream>
#include "Singleton.h"
#include "EventLoopThreadManager.h"
#include "EventLoop.h"
#include "TcpServer.h"

int main() {
	std::cout << "Welcome to lightim!" << std::endl;
	EventLoop loop;
	Singleton<EventLoopThreadManager>::instance().newEventLoopThread(4);
	Singleton<TcpServer>::instance().init(&loop, "0.0.0.0", 4399);
	Singleton<TcpServer>::instance().start();
	TcpServer *pServer = &Singleton<TcpServer>::instance();
	loop.loop();
	return 0;
}
