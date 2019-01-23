#pragma once

#include <memory>
#include "Socket.h"
#include "Buffer.h"
#include <functional>

class TcpSession;
class IOEventManager;
class EventLoop;

class TcpConnection {
public:
	TcpConnection();
	TcpConnection(EventLoop *_ploop, Socket _fd);

	void connectionEstablished(std::shared_ptr<TcpConnection>);

	void setMsgCallBack(const std::function<void()>&);
	void setCloseCallBack(const std::function<void(int)> &);

	void sendInLoop(const char *buf, int len);
	void send(const char*buf, int len);
	~TcpConnection() {
		//通知负责该连接的epoller，delete ioem
	}
private:
	EventLoop *ploop;

	Socket confd;

	std::shared_ptr<IOEventManager> pIOEM;
	std::shared_ptr<TcpSession> pTcpSession;

	Buffer inbuffer;
	Buffer outbuffer;

	void handleRead();
	void handleWrite();
	void handleError();
	void handleClose();

	std::function<void()> msgCallBack;
	std::function<void(int)> closeCallBack;

	
};
