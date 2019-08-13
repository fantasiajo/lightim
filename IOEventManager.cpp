#include "IOEventManager.h"
#include <sys/epoll.h>
#include "EventLoop.h"
#include <iostream>

IOEventManager::IOEventManager(EventLoop *_ploop, int _fd)
	:ploop(_ploop),
	fd(_fd),
	events(0)
{
	ploop->addIOEM(this);
}

IOEventManager::~IOEventManager() {
	if(type != "TcpConnection"){//因为TcpConnection在close套接字时，会自动从epoll中删除该套接字
		ploop->deleteIOEM(this);
	}
}

int IOEventManager::getfd() {
	return fd;
}

uint32_t IOEventManager::getEvents() {
	return events;
}

void IOEventManager::setRecvEvents(uint32_t events)
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
	events |= EPOLLOUT;
	update();
}

void IOEventManager::disableReading() {
	events &= ~(EPOLLIN | EPOLLPRI);
	update();
}

void IOEventManager::disableWriting() {
	events &= (~EPOLLOUT);
	update();
}

void IOEventManager::disableall()
{
	events = 0;
	update();
}

void IOEventManager::update() {
	ploop->updateIOEM(this);
}

void IOEventManager::handleEvent() {
	//一次只处理一个事件,防止因为read到0，从而触发handleclose导致TcpConnection被析构
	//但是有可能导致一直有数据要读而无法写数据
	if (recvEvents & (EPOLLIN | EPOLLPRI)) {
		//std::cout << type + " " + ip + ":" << port << std::endl;
		readCallBack();
	}
	else if (recvEvents & EPOLLOUT) {
		writeCallBack();
	}
	/*
	if ((recvEvents & EPOLLHUP) && !(recvEvents & EPOLLIN)) {
		closeCallBack();
	}
	if (recvEvents & EPOLLERR) {
		errorCallBack();
	}
	*/
}
