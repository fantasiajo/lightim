#pragma once

#include "Buffer.h"
#include <functional>
#include <memory>
#include "MsgCache.h"

class TcpConnection;
class EventLoop;
class Msg;

class TcpSession {
public:
	TcpSession(EventLoop *_ploop,TcpConnection *_pTcpConn);
	~TcpSession(){}

	TcpSession(const TcpSession &) = delete;
	TcpSession &operator=(const TcpSession &) = delete;
	TcpSession(TcpSession &&) = delete;
	TcpSession &operator=(TcpSession &&) = delete;

	void handleMsg(Buffer *pBuffer);

	void setLoginCallback(const std::function<void(uint32_t)> &cb);
	void setConfirmCallback(const std::function<void()> &);
private:
	EventLoop *ploop;

	bool login;
	uint32_t id;

	TcpConnection *pTcpConnection;

	std::function<void(uint32_t)> loginCallback;
	std::function<void()> confirmCallback;

	int heartBeatcnt = 0;

	std::weak_ptr<MsgCache> weakPMsgCache;

	void handleConfirm(Buffer *pBuffer);
	void handleSignUp(Buffer *pBuffer);//|nickname 32 bytes|md5passwd 32 bytes|
	void handleLoginIn(Buffer *pBuffer);
	void handleShowSb();
	void handleAddSb(Buffer *pBuffer);
	void handleAgreeSb(Buffer *pBuffer);
	void handleDeleteSb();
	void handleToSb(Buffer *pBuffer,int len);
	void handleGetFriends(Buffer *pBuffer);
	void handleHeartBeat();

	void sendMsg(uint32_t targetid,bool addInCache, std::shared_ptr<Msg> pMsg);
};
