#pragma once

#include "Buffer.h"
#include <memory>

class TcpConnection;

class TcpSession {
public:
	TcpSession(std::shared_ptr<TcpConnection>);
	~TcpSession();

	void handleMsg(Buffer *pBuffer);
	//void send(const char *buf, int len);
private:
	std::weak_ptr<TcpConnection> tmpPTcpConnection;
};
