#include "IOEventManager.h"

IOEventManager::IOEventManager() {

}

IOEventManager::IOEventManager(EventLoop *_ploop, int _fd)
	:ploop(_ploop),
	fd(_fd)
{

}

IOEventManager::~IOEventManager() {

}

int IOEventManager::getfd() {
	return fd;
}

uint32_t IOEventManager::getEvents() {
	return events;
}

inline void IOEventManager::setRecvEvents(uint32_t events)
{
	recvEvents = events;
}

void IOEventManager::setReadCallBack(const EventCallBack & cb)
{
	readCallBack = cb;
}

void IOEventManager::setWriteCallBack(const EventCallBack & cb)
{
	writeCallBack = cb;
}

void IOEventManager::setCloseCallBack(const EventCallBack & cb)
{
	closeCallBack = cb;
}

void IOEventManager::setErrorCallBack(const EventCallBack & cb)
{
	errorCallBack = cb;
}

void IOEventManager::enableReading() {
	events |= (EPOLLIN | EPOLLPRI);
	update();
}

void IOEventManager::enableWriting() {

}

void IOEventManager::disableReading() {

}

void IOEventManager::disableWriting() {

}

void IOEventManager::update() {
	ploop->updateIOEM(this);
}

void IOEventManager::handleEvent() {
	if ((recvEvents & EPOLLHUP) && !(recvEvents & EPOLLIN)) {
		closeCallBack();
	}
	if (recvEvents & EPOLLERR) {
		errorCallBack();
	}
	if (recvEvents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
		readCallBack();
	}
	if (recvEvents & EPOLLOUT) {
		writeCallBack();
	}
}
