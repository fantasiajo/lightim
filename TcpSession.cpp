#include "TcpSession.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Msg.h"
#include "UserManager.h"
#include "easylogging++.h"
#include "Singleton.h"
#include "TcpServer.h"

TcpSession::TcpSession(EventLoop *_ploop,std::shared_ptr<TcpConnection> pTcpCon)
	:ploop(_ploop),
	login(false),
	tmpPTcpConnection(pTcpCon)
{
}

TcpSession::~TcpSession()
{
}

//void TcpSession::handleMsg(Buffer * pBuffer)
//{
//	int len = pBuffer->length();
//	char buf[len];
//	pBuffer->out(buf, len);
//	if (!tmpPTcpConnection.expired()) {
//		tmpPTcpConnection.lock()->sendInLoop(buf, len);
//	}
//}

void TcpSession::handleMsg(Buffer *pBuffer) {
	int msglen;
	if (pBuffer->length() >= Msg::headerLen) {
		msglen = pBuffer->peekUint16();
		if (pBuffer->length() < msglen) {
			return;
		}
	}
	else {
		return;
	}
	msglen = pBuffer->getUint16();
	int type = pBuffer->getUint8();

	switch (type) {
	case Msg::MSG_TYPE::SIGN_UP:
		LOG(INFO) << "SIGN_UP from" << tmpPTcpConnection.lock()->getfd()->getPeerAddr()
			<< ": " << tmpPTcpConnection.lock()->getfd()->getPeerPort();
		handleSignUp(pBuffer);
		break;
	case Msg::MSG_TYPE::LOGIN_IN:
		LOG(INFO) << "LOGIN_IN from" << tmpPTcpConnection.lock()->getfd()->getPeerAddr()
			<< ": " << tmpPTcpConnection.lock()->getfd()->getPeerPort();
		handleLoginIn(pBuffer);
		break;
	case Msg::MSG_TYPE::SHOW_SB:
		handleShowSb();
		break;
	case Msg::MSG_TYPE::ADD_SB:
		handleAddSb();
		break;
	case Msg::MSG_TYPE::DELETE_SB:
		handleDeleteSb();
		break;
	case Msg::MSG_TYPE::TO_SB:
		handleToSb();
		break;
	default:
		break;
	};
}


void TcpSession::handleSignUp(Buffer *pBuffer)
{
	std::string nickname = pBuffer->getString(32);
	std::string password = pBuffer->getString(32);
	nickname.erase(nickname.begin()+nickname.find_first_of('\0'), nickname.end());
	if (UserManager::addUser(ploop,nickname, password)) {
		std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::SIGN_UP_ANS));
		pMsg->writeUint8(SUCCESS);
		LOG(INFO) << "SIGN_UP from" << tmpPTcpConnection.lock()->getfd()->getPeerAddr()
			<< ": " << tmpPTcpConnection.lock()->getfd()->getPeerPort() << "success." << nickname << ":" << password;
		if (!tmpPTcpConnection.expired()) {
			tmpPTcpConnection.lock()->sendInLoop(pMsg);
		}
	}
	else {
		std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::SIGN_UP_ANS));
		pMsg->writeUint8(FAIL);
		LOG(INFO) << "SIGN_UP from" << tmpPTcpConnection.lock()->getfd()->getPeerAddr()
			<< ": " << tmpPTcpConnection.lock()->getfd()->getPeerPort() << "fail." << nickname << ":" << password;
		if (!tmpPTcpConnection.expired()) {
			tmpPTcpConnection.lock()->sendInLoop(pMsg);
		}
	}
	//if (UserManager::exists(nickname)) {
	//	std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen+1,Msg::MSG_TYPE::SIGN_UP_ANS));
	//	pMsg->writeUint8(FAIL);
	//	if (!tmpPTcpConnection.expired()) {
	//		tmpPTcpConnection.lock()->sendInLoop(pMsg);
	//	}
	//}
	//else {
	//	std::string password = pBuffer->getString(32);
	//	UserManager::addUser(nickname, password);
	//}
	
}

void TcpSession::handleLoginIn(Buffer *pBuffer)
{
	uint32_t id = pBuffer->getUint32();
	std::string pwd = pBuffer->getString(32);
	if (UserManager::exists(ploop,id, pwd)) {//存在该用户
		if (login) {//当前已登录
			std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 2, Msg::MSG_TYPE::LOGIN_IN_ANS));
			pMsg->writeUint8(FAIL);
			pMsg->writeUint16(LOGINED);
			if (!tmpPTcpConnection.expired()) {
				tmpPTcpConnection.lock()->sendInLoop(pMsg);
			}
		}
		else {//当前未登录
			login = true;
			//Singleton<TcpServer>::instance().loginInLoop(id, tmpPTcpConnection.lock().get());
			std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::LOGIN_IN_ANS));
			pMsg->writeUint8(SUCCESS);
			if (!tmpPTcpConnection.expired()) {
				tmpPTcpConnection.lock()->sendInLoop(pMsg);
			}
		}
	}
	else {//不存在该用户
		std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 2, Msg::MSG_TYPE::LOGIN_IN_ANS));
		pMsg->writeUint8(FAIL);
		pMsg->writeUint16(USERORPWDNOTCORR);
		if (!tmpPTcpConnection.expired()) {
			tmpPTcpConnection.lock()->sendInLoop(pMsg);
		}
	}
}

void TcpSession::handleShowSb()
{
}

void TcpSession::handleAddSb()
{
}

void TcpSession::handleDeleteSb()
{
}

void TcpSession::handleToSb()
{
}

