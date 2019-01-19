#ifndef IOEVENTMANAGER_H
#define IOEVENTMANAGER_H

#include <functional>

class IOEventManager {
public:
	IOEventManager();
	~IOEventManager();

	inline int getfd();
	inline uint32_t getEvents();

	void enableReading();
	void enableWriting();
	void disableReading();
	void disableWriting();

	void handleEvent();

private:
	typedef std::function<void()> EventCallBack;

	const int fd;
	EventCallBack readCallBack;
	EventCallBack writeCallBack;
	EventCallBack closeCallBack;
	EventCallBack errorCallBack;

	uint32_t events;
	uint32_t recvEvents;

};

#endif
