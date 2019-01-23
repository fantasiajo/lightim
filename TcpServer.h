#pragma once

#include <vector>
#include <memory>
#include <unordered_set>

class EventLoop;
class TcpConnection;
class Acceptor;
class Socket;

class TcpServer {
public:
	TcpServer();
	
	~TcpServer();

	void init(EventLoop *_ploop, const char *ip, unsigned short port);
	void start();

	void newConnection(Socket &socket);

private:
	EventLoop *ploop;

	std::unordered_set<std::shared_ptr<TcpConnection>> conSet;
	std::shared_ptr<Acceptor> pAcceptor;

};

