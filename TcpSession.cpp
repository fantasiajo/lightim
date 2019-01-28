#include "TcpSession.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Msg.h"
#include "UserManager.h"
#include "easylogging++.h"
#include "Singleton.h"
#include "TcpServer.h"

TcpSession::TcpSession(EventLoop *_ploop,TcpConnection *_pTcpCon)
	:ploop(_ploop),
	login(false),
	pTcpConnection(_pTcpCon)
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
	auto type = pBuffer->getUint8();

	switch (type) {
	case Msg::MSG_TYPE::CONFIRM:
		handleConfirm(pBuffer);
		break;
	case Msg::MSG_TYPE::SIGN_UP:
		LOG(INFO) << "SIGN_UP from" << pTcpConnection->getfd()->getPeerAddr()
			<< ": " << pTcpConnection->getfd()->getPeerPort();
		handleSignUp(pBuffer);
		break;
	case Msg::MSG_TYPE::LOGIN_IN:
		LOG(INFO) << "LOGIN_IN from" << pTcpConnection->getfd()->getPeerAddr()
			<< ": " << pTcpConnection->getfd()->getPeerPort();
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
		handleToSb(pBuffer,msglen);
		break;
	default:
		break;
	};
}

void TcpSession::setLoginCallback(const std::function<void(uint32_t, TcpConnection*)> &cb)
{
	loginCallback = cb;
}

void TcpSession::setConfirmCallback(const std::function<void()>& cb)
{
	confirmCallback = cb;
}


void TcpSession::handleConfirm(Buffer * pBuffer)
{
	auto type = pBuffer->getUint8();
	if (type == Msg::MSG_TYPE::LOGIN_IN_ANS) {
		if (login) {
			loginCallback(id, pTcpConnection);
		}
	}
	else {
		confirmCallback();
	}
}

void TcpSession::handleSignUp(Buffer *pBuffer)
{
	//回复确认收到包
	std::shared_ptr<Msg> pConMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::CONFIRM));
	pConMsg->writeUint8(Msg::MSG_TYPE::SIGN_UP);
	pTcpConnection->sendMsg(pConMsg);

	//处理注册逻辑,因为主键是id是自增的，所以几乎任何注册都会成功。
	std::string nickname = pBuffer->getString(32);
	std::string password = pBuffer->getString(32);
	nickname.erase(nickname.begin()+nickname.find_first_of('\0'), nickname.end());
	uint32_t id;
	if (UserManager::addUser(ploop,nickname, password,id)) {
		std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 5, Msg::MSG_TYPE::SIGN_UP_ANS));
		pMsg->writeUint8(SUCCESS);
		pMsg->writeUint32(id);
		LOG(INFO) << "SIGN_UP from" << pTcpConnection->getfd()->getPeerAddr()
			<< ": " << pTcpConnection->getfd()->getPeerPort() << "success." << nickname << ":" << password;
		pTcpConnection->sendMsg(pMsg);
	}
	else {
		std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::SIGN_UP_ANS));
		pMsg->writeUint8(FAIL);
		LOG(INFO) << "SIGN_UP from" << pTcpConnection->getfd()->getPeerAddr()
			<< ": " << pTcpConnection->getfd()->getPeerPort() << "fail." << nickname << ":" << password;
		pTcpConnection->sendMsg(pMsg);
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
	//回复确认收到包
	std::shared_ptr<Msg> pConMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::CONFIRM));
	pConMsg->writeUint8(Msg::MSG_TYPE::LOGIN_IN);
	pTcpConnection->sendMsg(pConMsg);

	id = pBuffer->getUint32();
	std::string pwd = pBuffer->getString(32);
	if (UserManager::exists(ploop,id, pwd)) {//存在该用户
		if (login) {//当前已登录
			std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 2, Msg::MSG_TYPE::LOGIN_IN_ANS));
			pMsg->writeUint8(FAIL);
			pMsg->writeUint8(LOGINED);
			pTcpConnection->sendMsg(pMsg);
		}
		else {//当前未登录
			std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::LOGIN_IN_ANS));
			pMsg->writeUint8(SUCCESS);
			login = true;
			pTcpConnection->sendMsg(pMsg);
		}
	}
	else {//不存在该用户
		std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 2, Msg::MSG_TYPE::LOGIN_IN_ANS));
		pMsg->writeUint8(FAIL);
		pMsg->writeUint8(USERORPWDNOTCORR);
		pTcpConnection->sendMsg(pMsg);
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

void TcpSession::handleToSb(Buffer *pBuffer,int msglen)
{
	//回复确认收到包
	std::shared_ptr<Msg> pConMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::CONFIRM));
	pConMsg->writeUint8(Msg::MSG_TYPE::TO_SB);
	pTcpConnection->sendMsg(pConMsg);

	std::shared_ptr<Msg> pMsg(new Msg(msglen, Msg::MSG_TYPE::FROM_SB));
	auto targetid = pBuffer->getUint32();
	pMsg->writeUint32(id);
	auto content = pBuffer->getString(msglen - Msg::headerLen - 4);
	UserManager::addChat(ploop,id,targetid,content);
	pMsg->writeString(content.c_str(), content.length());

	Singleton<TcpServer>::instance().forwardMsg(targetid, pMsg);

}

