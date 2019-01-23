#pragma once

#include "Socket.h"
#include <memory>
#include <functional>

class EventLoop;
class IOEventManager;

class Acceptor {
public:
	Acceptor();
	Acceptor(EventLoop *_ploop,const char *ip, unsigned short port);

	~Acceptor();

	void listen();

	void accept();

	void setNewConnectionCallBack(std::function<void(Socket&)> cb);

private:
	EventLoop *ploop;

	std::shared_ptr<IOEventManager> pIOEM;
	Socket lisfd;

	std::function<void(Socket&)> newConnectionCallBack;
};

