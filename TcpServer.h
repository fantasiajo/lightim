#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <shared_mutex>
#include "Singleton.h"
#include "util.h"

class EventLoopManager;
class EventLoop;
class TcpConnection;
class Acceptor;
class Socket;
class Buffer;
class Msg;

class TcpServer {
public:

	TcpServer() = default;
	TcpServer(EventLoop *_ploop,std::string _ip, unsigned short _port);
	~TcpServer(){}
	TcpServer(const TcpServer &) = delete;
	TcpServer & operator=(const TcpServer &) = delete;
	TcpServer(TcpServer &&) = delete;
	TcpServer & operator=(TcpServer &&) = delete;

	void start();

	void newConnection(std::shared_ptr<Socket> pSocket);

	void login(uint32_t id, std::weak_ptr<TcpConnection> pTcpConn);

	void forwardMsg(uint32_t, std::shared_ptr<Msg> pMsg);

	void forwardNotify(uint32_t id);

	// bool isOnLine(uint32_t id) {
	// 	std::shared_lock<std::shared_mutex> lck(mtxUserMap);
	// 	if (userMap.find(id) == userMap.end()) return false;
	// 	return !userMap[id].tmpPTcpConn.expired();
	// }

	void addFriend(uint32_t fromid, uint32_t toid, const Task &confirmCallback);
	void agreeFriend(uint32_t fromid, uint32_t toid, const Task &confirmCallback);
	void chat(uint32_t fromid, uint32_t toid, std::string content, const Task &confirmCallback);

private:
	EventLoop *ploop;

	std::shared_ptr<Acceptor> pAcceptor;
	//std::unordered_set<std::shared_ptr<TcpConnection>> tcpConnSet;
	//std::unordered_map<uint32_t, UserInfo> userMap;//用户id-（发送buffer，负责的连接）
	//std::shared_mutex mtxUserMap;

	void loadUserBuffer();
};

