#pragma once

#include "Socket.h"
#include <memory>
#include <functional>

class EventLoop;
class IOEventManager;

class Acceptor {
public:
	Acceptor(EventLoop *_ploop,std::string ip, unsigned short port);

	~Acceptor();

	void listen();

	void accept();

	void setNewConnectionCallBack(std::function<void(Socket&)> cb);

private:
	EventLoop *ploop;

	Socket lisfd;

	std::shared_ptr<IOEventManager> pIOEM;
	

	std::function<void(Socket&)> newConnectionCallBack;
};

