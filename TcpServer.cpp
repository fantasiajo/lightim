#include "TcpServer.h"
#include "Singleton.h"
#include <memory>
#include "TcpConnection.h"
#include "Acceptor.h"
#include "EventLoopThreadManager.h"
#include "EventLoop.h"

TcpServer::TcpServer() {

}

void TcpServer::init(EventLoop *_ploop,const char * ip, unsigned short port)
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
	conSet.insert(pTcpCon);

	pIoLoop->runInLoop(std::bind(&TcpConnection::connectionEstablished, pTcpCon.get(), pTcpCon));
}

