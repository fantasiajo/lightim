#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <vector>
#include <memory>
#include <unordered_set>
#include "TcpConnection.h"
#include "Acceptor.h"
#include "EventLoop.h"

class TcpServer {
public:
	TcpServer();
	TcpServer(EventLoop *_ploop,const char *ip, short port);
	~TcpServer();

	void start();

private:
	EventLoop *ploop;

	std::unordered_set<std::shared_ptr<TcpConnection>> conSet;
	std::shared_ptr<Acceptor> pAcceptor;

};

#endif
