#include "TcpConnection.h"
#include "EventLoop.h"
#include "IOEventManager.h"
#include "TcpConnection.h"
#include "TcpSession.h"
#include "Msg.h"
#include <string>
#include <iostream>
TcpConnection::TcpConnection()
{
}

TcpConnection::TcpConnection(EventLoop *_ploop, std::shared_ptr<Socket> pSocket)
	:ploop(_ploop),
	pconfd(pSocket)
{
	pIOEM.reset(new IOEventManager(_ploop, pconfd->getSockfd()));
	pIOEM->type = "TcpConnection";
	pIOEM->ip = pconfd->getPeerAddr();
	pIOEM->port = pconfd->getPeerPort();
	pIOEM->setReadCallBack(std::bind(&TcpConnection::handleRead,this));
	pIOEM->setWriteCallBack(std::bind(&TcpConnection::handleWrite, this));
}

void TcpConnection::connectionEstablished(std::shared_ptr<TcpConnection> pTcpCon)
{
	pTcpSession.reset(new TcpSession(ploop,pTcpCon));
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

void TcpConnection::sendInLoop(std::shared_ptr<Msg> pMsg) {
	if (ploop->isInLoopThread()) {
		sendMsg(pMsg);
	}
	else {
		ploop->queueInLoop(std::bind(&TcpConnection::sendMsg, this, pMsg));
	}
}

void TcpConnection::sendMsg(std::shared_ptr<Msg> pMsg) {
	send(pMsg->getBuf(), pMsg->getLen());
}

TcpConnection::~TcpConnection()
{
	ploop->deleteIOEM(pIOEM.get());
}

void TcpConnection::handleRead()
{
	int cnt = inbuffer.readin(pconfd.get(), pconfd->readAbleNum());
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
	int cnt = outbuffer.writeout(pconfd.get());
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
	pconfd->close();
	std::cerr << pconfd->getPeerAddr() + ":" << pconfd->getPeerPort() << " leaves."<< std::endl;
	closeCallBack(pconfd->getSockfd());
}
