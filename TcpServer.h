#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

class EventLoop;
class TcpConnection;
class Acceptor;
class Socket;

class TcpServer {
public:
	TcpServer();
	
	~TcpServer();

	void init(EventLoop *_ploop, std::string , unsigned short port);
	void start();

	void newConnection(std::shared_ptr<Socket> pSocket);
	void closeConnection(int fd);

	//void loginInLoop(uint32_t,TcpConnection *);

private:
	EventLoop *ploop;

	std::unordered_map<int,std::shared_ptr<TcpConnection>> fd2TcpConn;
	std::shared_ptr<Acceptor> pAcceptor;

	//std::unordered_map<uint32_t, TcpConnection*> id2TcpConn;
};

