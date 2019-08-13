#include "TcpSession.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Msg.h"
#include "DataManager.h"
#include "easylogging++.h"
#include "Singleton.h"
#include "TcpServer.h"
#include "LogManager.h"
#include <string>
#include "EventLoopThreadManager.h"

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
			{
				std::ostringstream oss;
				oss << "SIGN_UP from" << pTcpConnection->getfd()->getPeerAddr()
					<< ": " << pTcpConnection->getfd()->getPeerPort();
				Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::DEBUG_LEVEL, oss.str());
			}
			handleSignUp(pBuffer);
			break;
		case Msg::MSG_TYPE::LOGIN_IN:
			{
				std::ostringstream oss;
				oss << "LOGIN_IN from" << pTcpConnection->getfd()->getPeerAddr()
					<< ": " << pTcpConnection->getfd()->getPeerPort();
				Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::DEBUG_LEVEL, oss.str());
			}
			handleLoginIn(pBuffer);
			return;//必须
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
			{
				std::ostringstream oss;
				oss << "GET_FRIENDS from" << pTcpConnection->getfd()->getPeerAddr()
						<< ": " << pTcpConnection->getfd()->getPeerPort() << " " << pTcpConnection->getid();
				Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::DEBUG_LEVEL, oss.str());
			}
			handleGetFriends(pBuffer);
			break;
		case Msg::MSG_TYPE::HEART_BEAT:
			handleHeartBeat();
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
	switch(type){
		case Msg::MSG_TYPE::FROM_SB:
		case Msg::MSG_TYPE::ADD_FROM_SB:
		case Msg::MSG_TYPE::AGREE_FROM_SB:
			lastRecvMsgId=pBuffer->getUint64();
			break;
		default:
			break;
	};
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
	if (ploop->getpDM().lock()->addUser(nickname, password,id)) {
		//Singleton<TcpServer>

		std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 5, Msg::MSG_TYPE::SIGN_UP_ANS));
		pMsg->writeUint8(SUCCESS);
		pMsg->writeUint32(id);

		std::ostringstream oss;
		oss << "SIGN_UP from" << pTcpConnection->getfd()->getPeerAddr()
			<< ": " << pTcpConnection->getfd()->getPeerPort() << "success." << nickname << ":" << password;
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::DEBUG_LEVEL, oss.str());
		pTcpConnection->sendMsg(pMsg);
	}
	else {
		std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::SIGN_UP_ANS));
		pMsg->writeUint8(FAIL);
		std::ostringstream oss;
		oss << "SIGN_UP from" << pTcpConnection->getfd()->getPeerAddr()
			<< ": " << pTcpConnection->getfd()->getPeerPort() << "fail." << nickname << ":" << password;
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::DEBUG_LEVEL, oss.str());
		pTcpConnection->sendMsg(pMsg);
	}
}

void TcpSession::handleLoginIn(Buffer *pBuffer)
{
	uint32_t id = pBuffer->peekUint32();
	EventLoop *ownPloop=Singleton<EventLoopThreadManager>::instance().getEventLoopById(id);
	if(ownPloop!=ploop){//转移TcpConnection的所有权
		ploop->deleteIOEM(pTcpConnection->getPIOEM());
		std::vector<Task> tasks_;
		tasks_.push_back(std::bind(&EventLoop::addIOEM,ownPloop,pTcpConnection->getPIOEM()));
		tasks_.push_back(std::bind(&EventLoop::addTcpConn,ownPloop,ploop->pullTcpConn(pTcpConnection->shared_from_this())));
		tasks_.push_back(std::bind(&TcpSession::handleLoginIn,this,pBuffer));
		ownPloop->runInLoop(tasks_);
		return;
	}
	
	//回复确认收到包
	std::shared_ptr<Msg> pConMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::CONFIRM));
	pConMsg->writeUint8(Msg::MSG_TYPE::LOGIN_IN);
	pTcpConnection->sendMsg(pConMsg);
	
	//登录逻辑
	id = pBuffer->getUint32();
	std::string pwd = pBuffer->getString(32);

	Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::DEBUG_LEVEL,std::string("login:")+std::to_string(id)+" "+pwd);

	if (ploop->getpDM().lock()->exists(id, pwd)) {//存在该用户
		auto weakPTcpConn=ploop->getConnById(id);
		if (!(weakPTcpConn.expired())) {//当前已登录//待改进
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
			pTcpConnection->setid(id);
			ploop->setConnById(id,pTcpConnection->shared_from_this());
			pTcpConnection->sendMsg(pMsg);
			ploop->getpDM().lock()->getLastMsgId(id,lastRecvMsgId);
			Singleton<LogManager>::instance().logInQueue(LogManager::DEBUG_LEVEL,
				std::string("Get lastmsgid of ")+std::to_string(id)+"="+std::to_string(lastRecvMsgId));
			loadCache(id);
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
	auto targetid = pBuffer->getUint32();
	//交给server处理
	auto targetPLoop = Singleton<EventLoopThreadManager>::instance().getEventLoopById(targetid);
	std::shared_ptr<Msg> pConMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::CONFIRM));
	pConMsg->writeUint8(Msg::MSG_TYPE::ADD_SB);
	targetPLoop->runInLoop(
		std::bind(&TcpServer::addFriend,&Singleton<TcpServer>::instance(),id,targetid,
			static_cast<Task>(std::bind(&TcpConnection::sendMsgWithCheck,pTcpConnection,pConMsg,std::weak_ptr<TcpConnection>(pTcpConnection->shared_from_this())))
		)
	);
}


void TcpSession::handleAgreeSb(Buffer * pBuffer)
{
	auto agreeid = pBuffer->getUint32();
	//交给server处理
	auto targetPLoop = Singleton<EventLoopThreadManager>::instance().getEventLoopById(agreeid);
	std::shared_ptr<Msg> pConMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::CONFIRM));
	pConMsg->writeUint8(Msg::MSG_TYPE::AGREE_SB);
	targetPLoop->runInLoop(
		std::bind(&TcpServer::agreeFriend,&Singleton<TcpServer>::instance(),id,agreeid,
			static_cast<Task>(std::bind(&TcpConnection::sendMsgWithCheck,pTcpConnection,pConMsg,std::weak_ptr<TcpConnection>(pTcpConnection->shared_from_this())))
		)
	);
}

void TcpSession::handleDeleteSb()
{}

void TcpSession::handleToSb(Buffer *pBuffer,int msglen)
{
	auto toid = pBuffer->getUint32();
	auto content = pBuffer->getString(msglen - Msg::headerLen - 4);
	//交给server处理
	auto targetPLoop = Singleton<EventLoopThreadManager>::instance().getEventLoopById(toid);
	std::shared_ptr<Msg> pConMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::CONFIRM));
	pConMsg->writeUint8(Msg::MSG_TYPE::TO_SB);
	targetPLoop->runInLoop(
		std::bind(&TcpServer::chat,&Singleton<TcpServer>::instance(),id,toid,content,
			static_cast<Task>(std::bind(&TcpConnection::sendMsgWithCheck,pTcpConnection,pConMsg,std::weak_ptr<TcpConnection>(pTcpConnection->shared_from_this())))
		)
	);
}

void TcpSession::handleGetFriends(Buffer * pBuffer)
{
	// 回复确认收到包
	std::shared_ptr<Msg> pConMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::CONFIRM));
	pConMsg->writeUint8(Msg::MSG_TYPE::GET_FRIENDS);
	pTcpConnection->sendMsg(pConMsg);

	//auto id = pBuffer->getUint32();
	std::vector<std::pair<uint32_t, std::string>> idname;
	ploop->getpDM().lock()->getFriends(id, idname);

	std::shared_ptr<Msg> pMsg(new Msg(Msg::headerLen + idname.size()*(4 + 32),Msg::MSG_TYPE::GET_FRIENDS_ANS));
	for (int i = 0; i < idname.size(); ++i) {
		pMsg->writeUint32(idname[i].first);
		idname[i].second.resize(32);
		pMsg->writeString(idname[i].second.c_str(), idname[i].second.length());
	}
	pTcpConnection->sendMsg(pMsg);
}

void TcpSession::handleHeartBeat()
{
	std::ostringstream oss;
	oss << std::this_thread::get_id() << " HeartBeat " << heartBeatcnt++;
	Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::DEBUG_LEVEL, oss.str());
	std::shared_ptr<Msg> pConMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::CONFIRM));
	pConMsg->writeUint8(Msg::MSG_TYPE::HEART_BEAT);
	pTcpConnection->sendMsg(pConMsg);
}

void TcpSession::loadCache(uint32_t id){
	std::vector<std::shared_ptr<Msg>> pMsgs;
	//从sql中读取未发送且未缓存的消息到buffer中
	if(ploop->getPMsgCache().lock()->size(id) > 0){
		uint64_t msgid;
		ploop->getPMsgCache().lock()->peekMsgid(id,msgid);
		Singleton<LogManager>::instance().logInQueue(LogManager::DEBUG_LEVEL,
			std::string("Peek lastRecvMsgId of")+std::to_string(id)+":"+std::to_string(msgid));
		ploop->getpDM().lock()->getMsgsById(id,pMsgs,lastRecvMsgId,msgid);
	}
	else{
		ploop->getpDM().lock()->getMsgsById(id,pMsgs,lastRecvMsgId,0);
	}
	for(const std::shared_ptr<Msg> &pMsg:pMsgs){
		pTcpConnection->sendMsg(pMsg);
	}

	//从redis缓存读取缓存消息并加入buffer中
	if(ploop->getPMsgCache().lock()->size(id) > 0){
		std::vector<std::string> msgs;
		ploop->getPMsgCache().lock()->content(id,msgs);
		for(const std::string &msg:msgs){
			pTcpConnection->send(msg.c_str(),msg.length());
		}
	}
}

void TcpSession::leave(){
	ploop->getpDM().lock()->updateLastRecvMsgId(id,lastRecvMsgId);
}
