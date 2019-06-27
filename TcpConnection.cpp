#include "TcpConnection.h"
#include "EventLoop.h"
#include "IOEventManager.h"
#include "TcpConnection.h"
#include "TcpSession.h"
#include "Msg.h"
#include "TcpServer.h"
#include "Singleton.h"
#include "easylogging++.h"
#include <string>
#include <iostream>
#include "LogManager.h"

TcpConnection::TcpConnection(EventLoop *_ploop, std::shared_ptr<Socket> pSocket)
	:ploop(_ploop),
	pconfd(pSocket),
	pSendBuf(new Buffer()),
	pRecvBuf(new Buffer())
{
	pIOEM.reset(new IOEventManager(_ploop, pconfd->getSockfd()));
	pIOEM->type = "TcpConnection";
	pIOEM->ip = pconfd->getPeerAddr();
	pIOEM->port = pconfd->getPeerPort();
	pIOEM->setReadCallBack(std::bind(&TcpConnection::handleRead,this));
	pIOEM->setWriteCallBack(std::bind(&TcpConnection::handleWrite, this));
}

void TcpConnection::connectionEstablished()
{
	pTcpSession.reset(new TcpSession(ploop,this));
	pTcpSession->setLoginCallback(std::bind(&TcpServer::login,&Singleton<TcpServer>::instance(), std::placeholders::_1,std::weak_ptr<TcpConnection>(this->shared_from_this())));
	setMsgCallBack(std::bind(&TcpSession::handleMsg, pTcpSession.get(), pRecvBuf.get()));
	pIOEM->enableReading();
}

void TcpConnection::setMsgCallBack(const std::function<void()> &cb)
{
	msgCallBack = cb;
}

void TcpConnection::setCloseCallBack(const std::function<void()>& cb)
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
	pSendBuf->in(buf, len);
	pIOEM->enableWriting();
}

void TcpConnection::sendMsg(std::shared_ptr<Msg> pMsg,std::weak_ptr<TcpConnection> weakPTcpConn) {
	if(weakPTcpConn.expired()){//防止因为TcpConnection被销毁后又调用此函数
		return;
	}
	send(pMsg->getBuf(), pMsg->getLen());
}

TcpConnection::~TcpConnection()
{
	ploop->deleteIOEM(pIOEM.get());
}

void TcpConnection::handleRead()
{
	auto n = pconfd->readAbleNum();
	if (n == 0) n = 1;//防止因为n==0导致返回值为0
	int cnt = pRecvBuf->readin(pconfd.get(), n);
	if (cnt > 0) {
		msgCallBack();
	}
	else if (cnt == 0) {
		handleClose();
	}
	else {
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
			return;
		}
		handleClose();
	}
}

void TcpConnection::handleWrite()
{
	int cnt = pSendBuf->writeout(pconfd.get());
	if (cnt < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
			return;
		}
		if (errno == EPIPE || errno == ECONNRESET) {
			handleClose();
		}
	}
	else {
		if (pSendBuf->empty()) {
			pIOEM->disableWriting();
		}
	}
}

void TcpConnection::handleError()
{
}

void TcpConnection::handleClose()
{
	pconfd->close();
	std::ostringstream oss;
	oss << pconfd->getPeerAddr() + ":" << pconfd->getPeerPort() << " leaves.";
	Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::INFO_LEVEL, oss.str());
	closeCallBack();
}
