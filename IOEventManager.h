#pragma once

#include <functional>

class EventLoop;

class IOEventManager {
public:
	typedef std::function<void()> EventCallBack;

	IOEventManager(EventLoop *_ploop, int _fd);
	~IOEventManager();

	IOEventManager(const IOEventManager &) = delete;
	IOEventManager &operator=(const IOEventManager &) = delete;
	IOEventManager(IOEventManager &&) = delete;
	IOEventManager &operator=(IOEventManager &&) = delete;

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
	void disableall();
	void update();

	void handleEvent();

	std::string type;
	std::string ip;
	unsigned short port;

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

