#include "TcpSession.h"
#include "TcpConnection.h"

TcpSession::TcpSession(std::shared_ptr<TcpConnection> pTcpCon)
	:tmpPTcpConnection(pTcpCon)
{
}

TcpSession::~TcpSession()
{
}

void TcpSession::handleMsg(Buffer * pBuffer)
{
	int len = pBuffer->length();
	char buf[len];
	pBuffer->out(buf, len);
	if (!tmpPTcpConnection.expired()) {
		tmpPTcpConnection.lock()->sendInLoop(buf, len);
	}
}

