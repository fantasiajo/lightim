#include "TcpServer.h"
#include "Singleton.h"
#include <memory>
#include "TcpConnection.h"
#include "Acceptor.h"
#include "EventLoopThreadManager.h"
#include "EventLoop.h"
#include <iostream>

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

void TcpServer::newConnection(Socket &socket) {
	EventLoop *pIoLoop = Singleton<EventLoopThreadManager>::instance().getNextEventLoop();
	std::shared_ptr<TcpConnection> pTcpCon(new TcpConnection(pIoLoop, socket));
	fd2TcpConn[socket.getSockfd()] = pTcpCon;
	pTcpCon->setCloseCallBack(std::bind(&TcpServer::closeConnection, this, std::placeholders::_1));
	pIoLoop->runInLoop(std::bind(&TcpConnection::connectionEstablished, pTcpCon.get(), pTcpCon));
}

void TcpServer::closeConnection(int fd)
{
	fd2TcpConn.erase(fd);
}


