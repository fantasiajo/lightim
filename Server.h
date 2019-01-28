#pragma once

#include <memory>
#include <unordered_map>

class TcpServer;
class TcpConnection;

class Server {
public:
	Server();
	~Server();

	void init(EventLoop *_ploop, std::string ip, unsigned short port);
	void start();
private:
	std::shared_ptr<TcpServer> pTcpServer;

	std::unordered_map<uint32_t, std::weak_ptr<TcpConnection>> onlineUsers;
};
