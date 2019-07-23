#pragma once

#include "Socket.h"
#include <memory>
#include <functional>

class EventLoop;
class IOEventManager;

class Acceptor
{
public:
	Acceptor(EventLoop *_ploop, std::string ip, unsigned short port);
	~Acceptor() {}
	Acceptor(const Acceptor &) = delete;
	Acceptor &operator=(const Acceptor &) = delete;
	Acceptor(Acceptor &&) = delete;
	Acceptor &operator=(Acceptor &&) = delete;

	void listen();

	void accept();

	void setNewConnectionCallBack(std::function<void(std::shared_ptr<Socket>)> cb);

private:
	EventLoop *ploop;

	std::shared_ptr<Socket> plisfd;

	std::shared_ptr<IOEventManager> pIOEM;

	std::function<void(std::shared_ptr<Socket>)> newConnectionCallBack;
};
