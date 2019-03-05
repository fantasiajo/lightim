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


void TcpSession::handleMsg(Buffer *pBuffer) {
	//首先判断接收到的消息是否可以组成一条完整的msg
	while (true) {
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
			handleAddSb(pBuffer);
			break;
		case Msg::MSG_TYPE::AGREE_SB:
			handleAgreeSb(pBuffer);
			break;
		case Msg::MSG_TYPE::DELETE_SB:
			handleDeleteSb();
			break;
		case Msg::MSG_TYPE::TO_SB:
			handleToSb(pBuffer, msglen);
			break;
		case Msg::MSG_TYPE::GET_FRIENDS:
			LOG(INFO) << "GET_FRIENDS from" << pTcpConnection->getfd()->getPeerAddr()
				<< ": " << pTcpConnection->getfd()->getPeerPort() << " " << pTcpConnection->getid();
			handleGetFriends(pBuffer);
			break;
		default:
			break;
		}
	}
}

void TcpSession::setLoginCallback(const std::function<void(uint32_t)> &cb)
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
			loginCallback(id);
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
		if (Singleton<TcpServer>::instance().isOnLine(id)) {//当前已登录
			std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 2, Msg::MSG_TYPE::LOGIN_IN_ANS));
			pMsg->writeUint8(FAIL);
			pMsg->writeUint8(LOGINED);
			pTcpConnection->sendMsg(pMsg);
		}
		else {//当前未登录
			std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::LOGIN_IN_ANS));
			pMsg->writeUint8(SUCCESS);
			login = true;
			this->id = id;
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

void TcpSession::handleAddSb(Buffer *pBuffer)
{
	//回复确认收到包
	std::shared_ptr<Msg> pConMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::CONFIRM));
	pConMsg->writeUint8(Msg::MSG_TYPE::ADD_SB);
	pTcpConnection->sendMsg(pConMsg);

	auto targetid = pBuffer->getUint32();
	std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 4, Msg::MSG_TYPE::ADD_FROM_SB));
	pMsg->writeUint32(id);
	Singleton<TcpServer>::instance().forwardMsg(targetid, pMsg);
}

void TcpSession::handleAgreeSb(Buffer * pBuffer)
{
	//回复确认收到包
	std::shared_ptr<Msg> pConMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::CONFIRM));
	pConMsg->writeUint8(Msg::MSG_TYPE::AGREE_SB);
	pTcpConnection->sendMsg(pConMsg);
	//逻辑有些问题，可能没有插入成功，但同意方已更新好友列表
	auto agreeid = pBuffer->getUint32();
	if (UserManager::addFriend(ploop,id,agreeid) && UserManager::addFriend(ploop, agreeid, id)) {
		std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 4, Msg::MSG_TYPE::AGREE_FROM_SB));
		pMsg->writeUint32(id);
		Singleton<TcpServer>::instance().forwardMsg(agreeid, pMsg);
	}
}

void TcpSession::handleDeleteSb()
{}

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

void TcpSession::handleGetFriends(Buffer * pBuffer)
{
	// 回复确认收到包
	std::shared_ptr<Msg> pConMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::CONFIRM));
	pConMsg->writeUint8(Msg::MSG_TYPE::GET_FRIENDS);
	pTcpConnection->sendMsg(pConMsg);

	//auto id = pBuffer->getUint32();
	std::vector<std::pair<uint32_t, std::string>> idname;
	UserManager::getFriends(ploop, id, idname);

	std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + idname.size()*(4 + 32),Msg::MSG_TYPE::GET_FRIENDS_ANS));
	for (int i = 0; i < idname.size(); ++i) {
		pMsg->writeUint32(idname[i].first);
		idname[i].second.resize(32);
		pMsg->writeString(idname[i].second.c_str(), idname[i].second.length());
	}
	pTcpConnection->sendMsg(pMsg);
}

