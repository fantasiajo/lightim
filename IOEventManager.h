#pragma once

#include <functional>

class EventLoop;

class IOEventManager {
public:
	typedef std::function<void()> EventCallBack;

	IOEventManager(EventLoop *_ploop, int _fd);
	~IOEventManager();

	int getfd();

	uint32_t getEvents();

	void setRecvEvents(uint32_t events);

	void setReadCallBack(const EventCallBack & cb);
	void setWriteCallBack(const EventCallBack & cb);
	void setCloseCallBack(const EventCallBack & cb);
	void setErrorCallBack(const EventCallBack & cb);

	void enableReading();
	void enableWriting();
	void disableReading();
	void disableWriting();
	void update();

	void handleEvent();

	std::string type;
	std::string ip;
	std::string port;

private:
	EventLoop *ploop;

	const int fd;
	EventCallBack readCallBack;
	EventCallBack writeCallBack;
	EventCallBack closeCallBack;
	EventCallBack errorCallBack;

	uint32_t events = 0;
	uint32_t recvEvents = 0;

	
};

