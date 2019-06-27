#include "TcpServer.h"
#include "Singleton.h"
#include <memory>
#include "TcpConnection.h"
#include "Acceptor.h"
#include "EventLoopThreadManager.h"
#include "EventLoop.h"
#include "IOEventManager.h"
#include <iostream>
#include "Buffer.h"
#include <unordered_set>
#include "UserManager.h"
#include "easylogging++.h"
#include "Singleton.h"
#include "LogManager.h"

void TcpServer::init(EventLoop *_ploop,std::string ip, unsigned short port)
{
	ploop = _ploop;
	pAcceptor.reset(new Acceptor(ploop, ip, port));
	pAcceptor->setNewConnectionCallBack(std::bind(&TcpServer::newConnection,this,std::placeholders::_1));
}

void TcpServer::start()
{
	pAcceptor->listen();
}

void TcpServer::newConnection(std::shared_ptr<Socket> pSocket) {
	EventLoop *pIoLoop = Singleton<EventLoopThreadManager>::instance().getNextEventLoop();
	std::shared_ptr<TcpConnection> pTcpCon(new TcpConnection(pIoLoop, pSocket));
	tcpConnSet.insert(pTcpCon);
	pTcpCon->setCloseCallBack(std::bind(&EventLoop::queueInLoop, ploop,
		static_cast<Task>(std::bind(&TcpServer::closeConnection, this, std::weak_ptr<TcpConnection>(pTcpCon)))
	));
	pIoLoop->queueInLoop(std::bind(&TcpConnection::connectionEstablished, pTcpCon.get()));
}

void TcpServer::closeConnection(std::weak_ptr<TcpConnection> pTcpConn)
{
	tcpConnSet.erase(pTcpConn.lock());
}

void TcpServer::login(uint32_t id, std::weak_ptr<TcpConnection> pTcpConn)
{
	std::unique_lock<std::shared_mutex> uniquelck(mtxUserMap);
	userMap[id].tmpPTcpConn = pTcpConn;
}

std::weak_ptr<TcpConnection> TcpServer::getConnById(uint32_t id){
	std::shared_lock<std::shared_mutex> lck(mtxUserMap);
	auto it = userMap.find(id);
	if(it == userMap.end()){
		return std::weak_ptr<TcpConnection>();
	}
	else{
		return it->second.tmpPTcpConn;
	}
}
