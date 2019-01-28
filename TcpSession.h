#pragma once

#include "Buffer.h"
#include <functional>
#include <memory>

class TcpConnection;
class EventLoop;
class Msg;

class TcpSession {
public:
	TcpSession(EventLoop *_ploop,TcpConnection *_pTcpConn);
	~TcpSession();

	void handleMsg(Buffer *pBuffer);

	void setLoginCallback(const std::function<void(uint32_t, TcpConnection *)>&);
	void setConfirmCallback(const std::function<void()> &);
private:
	EventLoop *ploop;

	bool login;
	uint32_t id;

	TcpConnection *pTcpConnection;

	void handleConfirm(Buffer *pBuffer);
	void handleSignUp(Buffer *pBuffer);//|nickname 32 bytes|md5passwd 32 bytes|
	void handleLoginIn(Buffer *pBuffer);
	void handleShowSb();
	void handleAddSb();
	void handleDeleteSb();
	void handleToSb(Buffer *pBuffer,int len);

	std::function<void(uint32_t, TcpConnection *)> loginCallback;
	std::function<void()> confirmCallback;
};
