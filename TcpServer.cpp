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
	loadUserBuffer();
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
	std::shared_lock<std::shared_mutex> sharelck(mtxUserMap);
	if (userMap.find(id) == userMap.end()) {
		sharelck.unlock();
		{
			std::unique_lock<std::shared_mutex> lck(mtxUserMap);
			userMap[id].pSendBuf = std::make_shared<Buffer>(true);
			userMap[id].pSendBuf->setMsgWritenCallback(std::bind(&TcpServer::forwardNotify, this, id));
		}
		sharelck.lock();
	}
	UserInfo &tmpUserInfo = userMap[id];//将值提出来操作，减小临界区
	sharelck.unlock();

	tmpUserInfo.pSendBuf->setLoop(pTcpConn.lock()->getloop());
	tmpUserInfo.pSendBuf->reset();//前移confirmindex到最近的非confirm消息

	std::shared_ptr<TcpConnection> tmpTcpConn = pTcpConn.lock();
	if (tmpTcpConn) {
		tmpUserInfo.tmpPTcpConn = tmpTcpConn;
		tmpTcpConn->setSendBuf(tmpUserInfo.pSendBuf);
		tmpTcpConn->setid(id);
		//每次登录时就开启写监听来接收不在线状态时接收的消息
		tmpTcpConn->getloop()->queueInLoop(std::bind(&TcpServer::forwardNotify,this,id));
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

void TcpServer::forwardNotify(uint32_t id)//只有id所在线程会调用此函数
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

void TcpServer::loadUserBuffer() {
	std::vector<uint32_t> ids;
	if (UserManager::getAllUsers(ploop, ids)) {
		for (const auto &id : ids) {
			userMap[id].pSendBuf = std::make_shared<Buffer>(true);
			userMap[id].pSendBuf->setLoop(Singleton<EventLoopThreadManager>::instance().getNextEventLoop());
			userMap[id].pSendBuf->setMsgWritenCallback(std::bind(&TcpServer::forwardNotify, this, id));
		}
	}
	else {
		//LOG(ERROR) << "loadUserBuffer failed.";
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::ERROR_LEVEL, "loadUserBuffer failed.");
	}
}

std::weak_ptr<TcpConnection> TcpServer::getConnById(uint32_t id){
	std::shared_lock<std::shared_mutex> lck(mtxUserMap);
	return userMap[id].tmpPTcpConn;
}
