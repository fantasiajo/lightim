#include "Acceptor.h"
#include "IOEventManager.h"

Acceptor::Acceptor(EventLoop *_ploop, std::string ip, unsigned short port)
	: ploop(_ploop),
	  plisfd(new Socket(ip, port))
{
	pIOEM.reset(new IOEventManager(_ploop, plisfd->getSockfd()));
	pIOEM->type = "Acceptor";
	pIOEM->setReadCallBack(std::bind(&Acceptor::accept, this));
}

void Acceptor::listen()
{
	plisfd->bind();
	plisfd->listen();
	pIOEM->enableReading();
}

void Acceptor::accept()
{
	std::shared_ptr<Socket> pconsocket = plisfd->accept();
	if (pconsocket)
	{
		newConnectionCallBack(pconsocket);
	}
}

void Acceptor::setNewConnectionCallBack(std::function<void(std::shared_ptr<Socket>)> cb)
{
	newConnectionCallBack = cb;
}
