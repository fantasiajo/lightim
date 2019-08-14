#include "TcpServer.h"
#include "Singleton.h"
#include <memory>
#include "TcpConnection.h"
#include "Acceptor.h"
#include "EventLoopThreadManager.h"
#include "EventLoop.h"
#include "IOEventManager.h"
#include <iostream>
#include "Buffer.h"
#include <unordered_set>
#include "DataManager.h"
#include "easylogging++.h"
#include "Singleton.h"
#include "LogManager.h"

TcpServer::TcpServer(EventLoop *_ploop,std::string ip, unsigned short port)
	:ploop(_ploop),
	pAcceptor(new Acceptor(_ploop,ip,port)){
		pAcceptor->setNewConnectionCallBack(std::bind(&TcpServer::newConnection,this,std::placeholders::_1));
}

void TcpServer::start()
{
	pAcceptor->listen();
}

void TcpServer::newConnection(std::shared_ptr<Socket> pSocket) {
	EventLoop *pIoLoop = Singleton<EventLoopThreadManager>::instance().getNextEventLoop();
	std::shared_ptr<TcpConnection> pTcpCon(new TcpConnection(pIoLoop, pSocket));
	pTcpCon->setCloseCallBack(std::bind(&EventLoop::closeConnection, pIoLoop,std::weak_ptr<TcpConnection>(pTcpCon)));
	pIoLoop->queueInLoop(std::bind(&TcpConnection::connectionEstablished, pTcpCon.get(),pTcpCon));
}

void TcpServer::addFriend(uint32_t fromid, uint32_t toid,const Task &confirmCallback)
{ //只有toid所在的线程会调用此函数
	EventLoop *ploopOfToid = Singleton<EventLoopThreadManager>::instance().getEventLoopById(toid);
	if (!(ploopOfToid->getpDM().lock()->addMsg(fromid, toid, Msg::MSG_TYPE::ADD_SB, "")))
	{
		return;
	}

	std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 12, Msg::MSG_TYPE::ADD_FROM_SB));
	pMsg->writeUint64(0);
	pMsg->writeUint32(fromid);

	auto msgid = ploopOfToid->getpDM().lock()->addSendMsg(toid, std::string(pMsg->getBuf(), pMsg->getLen()));
	pMsg->writeUint64(msgid, Msg::headerLen);

	auto pTcpConn = ploopOfToid->getConnById(toid).lock();
	if (pTcpConn)
	{
		pTcpConn->sendMsg(pMsg);
	}
	else
	{
		ploopOfToid->getPMsgCache().lock()->push(toid, pMsg);
	}

	//返回confirm
	confirmCallback();
}

void TcpServer::agreeFriend(uint32_t fromid, uint32_t toid,const Task &confirmCallback)
{ //只有toid所在的线程会调用此函数
	EventLoop *ploopOfToid = Singleton<EventLoopThreadManager>::instance().getEventLoopById(toid);
	if (!(ploopOfToid->getpDM().lock()->addMsg(fromid, toid, Msg::MSG_TYPE::AGREE_SB, "")))
	{
		return;
	}

	std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 12, Msg::MSG_TYPE::AGREE_FROM_SB));
	pMsg->writeUint64(0);
	pMsg->writeUint32(fromid);

	auto msgid = ploopOfToid->getpDM().lock()->addSendMsg(toid, std::string(pMsg->getBuf(), pMsg->getLen()));
	pMsg->writeUint64(msgid, Msg::headerLen);

	auto pTcpConn = ploopOfToid->getConnById(toid).lock();
	if (pTcpConn && pTcpConn->getid()>0)
	{
		pTcpConn->sendMsg(pMsg);
	}
	else
	{
		ploopOfToid->getPMsgCache().lock()->push(toid, pMsg);
	}

	//返回confirm
	confirmCallback();
}

void TcpServer::chat(uint32_t fromid, uint32_t toid, std::string content, const Task &confirmCallback)
{ //只有toid所在的线程会调用此函数
	EventLoop *ploopOfToid = Singleton<EventLoopThreadManager>::instance().getEventLoopById(toid);
	if (!(ploopOfToid->getpDM().lock()->addMsg(fromid, toid, Msg::MSG_TYPE::TO_SB, content)))
	{
		return;
	}

	std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 8 + 4 + content.length(), Msg::MSG_TYPE::FROM_SB));
	pMsg->writeUint64(0);
	pMsg->writeUint32(fromid);
	pMsg->writeString(content.c_str(),content.length());

	auto msgid = ploopOfToid->getpDM().lock()->addSendMsg(toid, std::string(pMsg->getBuf(), pMsg->getLen()));
	pMsg->writeUint64(msgid, Msg::headerLen);

	auto pTcpConn = ploopOfToid->getConnById(toid).lock();
	if (pTcpConn)
	{
		pTcpConn->sendMsg(pMsg);
	}
	else
	{
		ploopOfToid->getPMsgCache().lock()->push(toid, pMsg);
	}

	//返回confirm
	confirmCallback();
}