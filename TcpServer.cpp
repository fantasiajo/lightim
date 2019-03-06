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
	pTcpCon->setCloseCallBack(std::bind(&TcpServer::closeConnection, this, std::weak_ptr<TcpConnection>(pTcpCon)));
	pIoLoop->queueInLoop(std::bind(&TcpConnection::connectionEstablished, pTcpCon.get()));
}

void TcpServer::closeConnection(std::weak_ptr<TcpConnection> pTcpConn)
{
	ploop->queueInLoop(std::bind(&TcpServer::closeConnection_, this, pTcpConn));
}

void TcpServer::closeConnection_(std::weak_ptr<TcpConnection> pTcpConn)
{
	tcpConnSet.erase(pTcpConn.lock());
}

void TcpServer::loginInLoop(uint32_t id, std::weak_ptr<TcpConnection> pTcpConn)
{
	ploop->queueInLoop(std::bind(&TcpServer::login, this, id, pTcpConn));
}

void TcpServer::login(uint32_t id, std::weak_ptr<TcpConnection> pTcpConn)
{
	auto iter = userMap.find(id);
	if (iter == userMap.end()) {
		userMap[id].pSendBuf = std::make_shared<Buffer>(true);
		userMap[id].tmpPTcpConn = pTcpConn;
	}
	
	userMap[id].pSendBuf->setLoop(pTcpConn.lock()->getloop());
	userMap[id].pSendBuf->reset();//前移confirmindex到最近的readindex
	userMap[id].pSendBuf->setMsgWritenCallback(std::bind(&TcpServer::forwardNotify,this,id));

	pTcpConn.lock()->setSendBuf(userMap[id].pSendBuf);//启动监听？
	pTcpConn.lock()->setid(id);
}

void TcpServer::forwardMsg(uint32_t id, std::shared_ptr<Msg> pMsg)
{
	//如果没有在usermap中找到id，说明从来没有登陆过，视为没有此用户。从而不用加锁。
	auto iter = userMap.find(id);
	if (iter != userMap.end()) {
		userMap[id].pSendBuf->pushMsgInLoop(pMsg);
	}
}

void TcpServer::forwardNotify(uint32_t id)
{
	if (!userMap[id].tmpPTcpConn.expired()) {//如果id目前在线，则启动写监听
		userMap[id].tmpPTcpConn.lock()->getPIOEM()->enableWriting();
	}
}

