#pragma once

#include "Buffer.h"
#include <memory>

class TcpConnection;
class EventLoop;
class Msg;

class TcpSession {
public:
	TcpSession(EventLoop *_ploop,std::shared_ptr<TcpConnection>);
	~TcpSession();

	void handleMsg(Buffer *pBuffer);
private:
	EventLoop *ploop;

	std::weak_ptr<TcpConnection> tmpPTcpConnection;

	void handleSignUp(Buffer *pBuffer);//|nickname 32 bytes|md5passwd 32 bytes|
	void handleLoginIn();
	void handleShowSb();
	void handleAddSb();
	void handleDeleteSb();
	void handleToSb();
};
