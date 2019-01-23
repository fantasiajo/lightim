#include "Acceptor.h"
#include "IOEventManager.h"

Acceptor::Acceptor(EventLoop *_ploop,std::string ip, unsigned short port)
	:ploop(_ploop),
	lisfd(ip,port),
	pIOEM()
{
	pIOEM.reset(new IOEventManager(_ploop, lisfd.getSockfd()));
	pIOEM->type = "Acceptor";
	pIOEM->setReadCallBack(std::bind(&Acceptor::accept,this));
}

Acceptor::~Acceptor()
{
}

void Acceptor::listen()
{
	lisfd.bind();
	lisfd.listen();
	pIOEM->enableReading();
}

void Acceptor::accept()
{
	Socket consocket = lisfd.accept();
	newConnectionCallBack(consocket);
}

void Acceptor::setNewConnectionCallBack(std::function<void(Socket&)> cb) {
	newConnectionCallBack = cb;
}

