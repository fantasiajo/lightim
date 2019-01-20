#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "IOEventManager.h"
#include "Socket.h"
#include "EventLoop.h"
#include <memory>

class Acceptor {
public:
	Acceptor();
	Acceptor(EventLoop *_ploop,const char *ip, short port);

	~Acceptor();

	void listen();

	void accept();

private:
	EventLoop *ploop;

	std::shared_ptr<IOEventManager> pIOEM;
	Socket lisfd;
};

#endif
