#include "TcpServer.h"
#include "Singleton.h"
#include <memory>
#include "TcpConnection.h"
#include "Acceptor.h"
#include "EventLoopThreadManager.h"
#include "EventLoop.h"
#include <iostream>
#include "Buffer.h"

TcpServer::TcpServer() {

}

void TcpServer::init(EventLoop *_ploop,std::string ip, unsigned short port)
{
	ploop = _ploop;
	pAcceptor.reset(new Acceptor(ploop, ip, port));
	pAcceptor->setNewConnectionCallBack(std::bind(&TcpServer::newConnection,this,std::placeholders::_1));
}

TcpServer::~TcpServer() {

}

void TcpServer::start()
{
	pAcceptor->listen();
}

void TcpServer::newConnection(std::shared_ptr<Socket> pSocket) {
	EventLoop *pIoLoop = Singleton<EventLoopThreadManager>::instance().getNextEventLoop();
	std::shared_ptr<TcpConnection> pTcpCon(new TcpConnection(pIoLoop, pSocket));
	fd2TcpConn[pSocket->getSockfd()] = pTcpCon;
	pTcpCon->setCloseCallBack(std::bind(&TcpServer::closeConnection, this, std::placeholders::_1));
	pIoLoop->runInLoop(std::bind(&TcpConnection::connectionEstablished, pTcpCon.get()));
}

void TcpServer::closeConnection(int fd)
{
	fd2TcpConn.erase(fd);
}

void TcpServer::login(uint32_t id, TcpConnection *pTcpcon)
{
	auto iter = id2SBuf.find(id);
	if (iter == id2SBuf.end()) {
		id2SBuf[id] = std::make_shared<Buffer>(true);
	}
	pTcpcon->setSendBuf(id2SBuf[id]);
	id2SBuf[id]->setLoop(pTcpcon->getloop());
}

void TcpServer::forwardMsg(uint32_t id, std::shared_ptr<Msg> pMsg)
{
	//if (ploop->isInLoopThread()) {
		id2SBuf[id]->pushMsgInLoop(pMsg);//?是否需要加锁？
	//}
	//else {
	//	ploop->queueInLoop(std::bind(&TcpServer::forwardMsg, this, id, pMsg));
	//}
}

