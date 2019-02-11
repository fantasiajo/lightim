#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

class EventLoop;
class TcpConnection;
class Acceptor;
class Socket;
class Buffer;
class Msg;

class TcpServer {
public:
	typedef struct {
		std::shared_ptr<Buffer> pSendBuf;
		std::weak_ptr<TcpConnection> tmpPTcpConn;
	}UserInfo;

	TcpServer(){}
	~TcpServer(){}
	TcpServer(const TcpServer &) = delete;
	TcpServer & operator=(const TcpServer &) = delete;
	TcpServer(TcpServer &&) = delete;
	TcpServer & operator=(TcpServer &&) = delete;

	void init(EventLoop *_ploop, std::string , unsigned short port);
	void start();

	void newConnection(std::shared_ptr<Socket> pSocket);
	void closeConnection(std::shared_ptr<TcpConnection> pTcpConn);

	void login(uint32_t id, std::shared_ptr<TcpConnection> pTcpConn);

	void forwardMsg(uint32_t, std::shared_ptr<Msg> pMsg);

	void forwardNotify(uint32_t id);

private:
	EventLoop *ploop;

	std::shared_ptr<Acceptor> pAcceptor;
	std::unordered_set<std::shared_ptr<TcpConnection>> tcpConnSet;
	std::unordered_map<uint32_t, UserInfo> userMap;//用户id-（发送buffer，负责的连接）
};

