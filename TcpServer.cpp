#include "TcpServer.h"

TcpServer::TcpServer() {

}

TcpServer::TcpServer(EventLoop *_ploop,const char * ip, short port)
	:ploop(_ploop),
	pAcceptor(new Acceptor(_ploop,ip,port))
{
}

TcpServer::~TcpServer() {

}

void TcpServer::start()
{
	pAcceptor->listen();
}

