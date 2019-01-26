#pragma once

#include <memory>
#include "Socket.h"
#include "Buffer.h"
#include <functional>

class TcpSession;
class IOEventManager;
class EventLoop;
class Msg;

class TcpConnection {
public:
	TcpConnection();
	TcpConnection(EventLoop *_ploop, std::shared_ptr<Socket> pSocket);
	~TcpConnection();
	//通知负责该连接的epoller，delete ioem

	Socket *getfd() {
		return pconfd.get();
	}

	void connectionEstablished(std::shared_ptr<TcpConnection>);

	void setMsgCallBack(const std::function<void()>&);
	void setCloseCallBack(const std::function<void(int)> &);

	void sendInLoop(const char *buf, int len);
	void send(const char*buf, int len);
	void sendInLoop(std::shared_ptr<Msg> pMsg);
	void sendMsg(std::shared_ptr<Msg> pMsg);
	

private:
	EventLoop *ploop;

	std::shared_ptr<Socket> pconfd;

	std::shared_ptr<IOEventManager> pIOEM;
	std::shared_ptr<TcpSession> pTcpSession;//需要改动，tcpconnection不能主导tcpsession的生命周期

	Buffer inbuffer;
	Buffer outbuffer;

	void handleRead();
	void handleWrite();
	void handleError();
	void handleClose();

	std::function<void()> msgCallBack;
	std::function<void(int)> closeCallBack;

	
};
