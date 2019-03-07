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
	{
		std::unique_lock<std::shared_mutex> lck(mtxUserMap);
		auto iter = userMap.find(id);
		if (iter == userMap.end()) {
			userMap[id].tmpPTcpConn = pTcpConn;
		}
	}
	std::shared_lock<std::shared_mutex> sharelck(mtxUserMap);
	UserInfo &tmpUserInfo = userMap[id];//将值提出来操作，减小临界区
	sharelck.unlock();

	tmpUserInfo.pSendBuf = std::make_shared<Buffer>(true);
	tmpUserInfo.pSendBuf->setMsgWritenCallback(std::bind(&TcpServer::forwardNotify, this, id));
	tmpUserInfo.pSendBuf->setLoop(pTcpConn.lock()->getloop());
	tmpUserInfo.pSendBuf->reset();//前移confirmindex到最近的readindex

	std::shared_ptr<TcpConnection> tmpTcpConn = pTcpConn.lock();
	if (tmpTcpConn) {
		tmpTcpConn->setSendBuf(userMap[id].pSendBuf);//启动监听？
		tmpTcpConn->setid(id);
	}
}

void TcpServer::forwardMsg(uint32_t id, std::shared_ptr<Msg> pMsg)
{
	std::shared_lock<std::shared_mutex> lck(mtxUserMap);
	auto iter = userMap.find(id);
	if (iter != userMap.end()) {
		iter->second.pSendBuf->pushMsgInLoop(pMsg);
	}
}

void TcpServer::forwardNotify(uint32_t id)
{
	std::shared_ptr<TcpConnection> pTcpConn;
	{
		std::shared_lock<std::shared_mutex> lck(mtxUserMap);
		pTcpConn = userMap[id].tmpPTcpConn.lock();
	}
	if (pTcpConn) {//如果id目前在线，则启动写监听
		pTcpConn->getPIOEM()->enableWriting();
	}
}

