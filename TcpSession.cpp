#include "TcpSession.h"
#include "TcpConnection.h"
#include "Msg.h"

TcpSession::TcpSession(std::shared_ptr<TcpConnection> pTcpCon)
	:tmpPTcpConnection(pTcpCon)
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
		handleSignUp();
		break;
	case Msg::MSG_TYPE::LOGIN_IN:
		handleLoginIn();
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

void TcpSession::handleSignUp()
{
}

void TcpSession::handleLoginIn()
{
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

