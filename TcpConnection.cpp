#include "TcpConnection.h"
#include "EventLoop.h"
#include "IOEventManager.h"
#include "TcpConnection.h"
#include "TcpSession.h"
#include <string>
#include <iostream>
TcpConnection::TcpConnection()
{
}

TcpConnection::TcpConnection(EventLoop *_ploop, Socket _fd)
	:ploop(_ploop),
	confd(_fd)
{
	pIOEM.reset(new IOEventManager(_ploop, confd.getSockfd()));
	pIOEM->type = "TcpConnection";
	pIOEM->ip = confd.getPeerAddr();
	pIOEM->port = confd.getPeerPort();
	pIOEM->setReadCallBack(std::bind(&TcpConnection::handleRead,this));
	pIOEM->setWriteCallBack(std::bind(&TcpConnection::handleWrite, this));
}

void TcpConnection::connectionEstablished(std::shared_ptr<TcpConnection> pTcpCon)
{
	pTcpSession.reset(new TcpSession(pTcpCon));
	setMsgCallBack(std::bind(&TcpSession::handleMsg,
		pTcpSession.get(), &inbuffer));
	pIOEM->enableReading();
}

void TcpConnection::setMsgCallBack(const std::function<void()> &cb)
{
	msgCallBack = cb;
}

void TcpConnection::setCloseCallBack(const std::function<void(int)>& cb)
{
	closeCallBack = cb;
}

void TcpConnection::sendInLoop(const char *buf, int len) {
	if (ploop->isInLoopThread()) {
		send(buf, len);
	}
	else {
		ploop->queueInLoop(std::bind(&TcpConnection::send, this, buf, len));
	}
}

void TcpConnection::send(const char * buf, int len)
{
	outbuffer.in(buf, len);
	pIOEM->enableWriting();
}

void TcpConnection::handleRead()
{
	int cnt = inbuffer.readin(confd, confd.readAbleNum());
	if (cnt > 0) {
		msgCallBack();
	}
	else if (cnt == 0) {
		handleClose();
	}
	else {
		handleError();
	}
}

void TcpConnection::handleWrite()
{
	int cnt = outbuffer.writeout(confd);
	if (cnt < 0) {
		if (errno == EPIPE || errno == ECONNRESET) {
			handleClose();
		}
	}
	if (outbuffer.empty()) {
		pIOEM->disableWriting();
	}
}

void TcpConnection::handleError()
{
}

void TcpConnection::handleClose()
{
	confd.close();
	std::cerr << confd.getPeerAddr() + ":" << confd.getPeerPort() << " leaves."<< std::endl;
	closeCallBack(confd.getSockfd());
}
