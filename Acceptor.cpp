#include "Acceptor.h"

Acceptor::Acceptor()
{
}

Acceptor::Acceptor(EventLoop *_ploop,const char * ip, short port)
	:ploop(_ploop),
	pIOEM(new IOEventManager(_ploop)),
	lisfd(ip,port)
{
	pIOEM->setReadCallBack(accept());
}

Acceptor::~Acceptor()
{
}

void Acceptor::listen()
{
	lisfd.listen();
	pIOEM->enableReading();
}

void Acceptor::accept()
{

}

