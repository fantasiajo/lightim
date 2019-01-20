#ifndef IOEVENTMANAGER_H
#define IOEVENTMANAGER_H

#include <functional>
#include "EventLoop.h"

class IOEventManager {
public:
	IOEventManager();
	IOEventManager(EventLoop *_ploop);
	~IOEventManager();

	inline int getfd();

	inline uint32_t getEvents();

	inline void setRecvEvents(uint32_t events);

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

private:
	typedef std::function<void()> EventCallBack;

	EventLoop *ploop;

	const int fd;
	EventCallBack readCallBack;
	EventCallBack writeCallBack;
	EventCallBack closeCallBack;
	EventCallBack errorCallBack;

	uint32_t events = 0;
	uint32_t recvEvents = 0;

};

#endif
