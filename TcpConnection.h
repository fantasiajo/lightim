#pragma once

#include <memory>
#include "Socket.h"
#include "Buffer.h"
#include <functional>

class TcpSession;
class IOEventManager;
class EventLoop;
class Msg;

class TcpConnection :public std::enable_shared_from_this<TcpConnection> {
public:
	TcpConnection(EventLoop *_ploop, std::shared_ptr<Socket> pSocket);
	~TcpConnection();
	TcpConnection(const TcpConnection &) = delete;
	TcpConnection &operator=(const TcpConnection &) = delete;
	TcpConnection(TcpConnection &&) = delete;
	TcpConnection &operator=(TcpConnection &&) = delete;

	Socket *getfd() {
		return pconfd.get();
	}

	EventLoop *getloop() {
		return ploop;
	}

	uint32_t getid() {
		return id;
	}

	void setid(uint32_t _id) {
		id = _id;
	}

	void connectionEstablished();

	void setMsgCallBack(const std::function<void()>&);
	void setCloseCallBack(const std::function<void()> &);

	void sendInLoop(const char *buf, int len);
	void send(const char*buf, int len);
	void sendInLoop(std::shared_ptr<Msg> pMsg);
	void sendMsg(std::shared_ptr<Msg> pMsg, std::weak_ptr<TcpConnection> weakPTcpConn);

	std::shared_ptr<IOEventManager> getPIOEM() {
		return pIOEM;
	}

private:
	EventLoop *ploop;

	uint32_t id;

	std::shared_ptr<Socket> pconfd;

	std::shared_ptr<IOEventManager> pIOEM;
	std::shared_ptr<TcpSession> pTcpSession;

	std::shared_ptr<Buffer> pRecvBuf;
	std::shared_ptr<Buffer> pSendBuf;

	void handleRead();
	void handleWrite();
	void handleError();
	void handleClose();

	std::function<void()> msgCallBack;
	std::function<void()> closeCallBack;
};
