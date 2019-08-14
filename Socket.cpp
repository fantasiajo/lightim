#include <iostream>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <error.h>
#include "Socket.h"
#include "easylogging++.h"
#include "LogManager.h"
#include "Singleton.h"

Socket::Socket(std::string _ip, unsigned short _port)
	: sockfd(socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0))
{
	if (sockfd == -1)
	{
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, std::string("Socket create failed:") + strerror(errno));
	}

	//开启地址复用来解决time_wait问题
	int optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	{
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, std::string("Set SO_REUSEADDR failed:") + strerror(errno));
	}

	int status = inet_pton(AF_INET, _ip.c_str(), &addr);
	if (status == -1)
	{
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, strerror(errno));
	}
	if (status == 0)
	{
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, strerror(errno));
	}
	port = htons(_port);
}

void Socket::bind()
{
	struct sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = port;
	sockaddr.sin_addr = addr;
	if (::bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1)
	{
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, std::string("bind") + strerror(errno));
	}
}

void Socket::listen()
{
	type = LISTENING_SOCKET;
	int cnt;
	if (cnt = ::listen(sockfd, backlog) == -1)
	{
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, std::string("listen") + strerror(errno));
	}
}

std::shared_ptr<Socket> Socket::accept()
{
	int tmpfd = ::accept4(sockfd, NULL, 0, SOCK_NONBLOCK);
	if (tmpfd == -1)
	{
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, std::string("accept4") + strerror(errno));
		return nullptr;
	}

	//开启keepalive
	int keepAlive = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(keepAlive)) == -1)
	{
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, std::string("Set SO_KEEPALIVE failed:") + strerror(errno));
	}

	//设置没有数据传输多少秒后发送探测报文
	int keepIdle = 30;
	if (setsockopt(sockfd,SOL_TCP,TCP_KEEPIDLE,&keepIdle,sizeof(keepIdle))==-1){
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, std::string("Set TCP_KEEPIDLE failed:")+strerror(errno));
	}

	//设置探测发包间隔
	int keepInterval = 5;
	if (setsockopt(sockfd,SOL_TCP,TCP_KEEPINTVL,&keepInterval,sizeof(keepInterval))==-1){
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, std::string("Set TCP_KEEPINTVL failed:")+strerror(errno));
	}

	//设置探测的次数
	int keepCnt = 3;
	if (setsockopt(sockfd,SOL_TCP,TCP_KEEPCNT,&keepCnt,sizeof(keepCnt))==-1){
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, std::string("Set TCP_KEEPCNT failed:")+strerror(errno));
	}

	struct sockaddr_in sockaddr;
	socklen_t len = sizeof(sockaddr_in);

	if (getpeername(tmpfd, (struct sockaddr *)&sockaddr, &len) == -1)
	{
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, std::string("getpeername") + strerror(errno));
		return nullptr;
	}

	std::shared_ptr<Socket> psocket(new Socket());
	psocket->type = CONNECTION_SOCKET;
	psocket->sockfd = tmpfd;
	psocket->addr = addr;
	psocket->port = port;
	psocket->peerAddr = sockaddr.sin_addr;
	psocket->peerPort = sockaddr.sin_port;

	return psocket;
}

void Socket::close()
{
	::close(sockfd);
}
/*
int Socket::connect(std::string ip, unsigned short _port,int timeout)
{
	int status = inet_pton(AF_INET, ip.c_str(), &addr);
	if (status == -1) {
		//log.err
		exit(1);
	}
	if (status == 0) {
		//log.err.ipcharformat
		exit(1);
	}
	port = htons(_port);

	struct sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = port;
	sockaddr.sin_addr = addr;
	int res;
	int error = 0;
	if ((res = ::connect(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr))) < 0) {
		if (errno != EINPROGRESS) {
			return -1;
		}
	}
	if (res != 0) {
		fd_set rset, wset;
		FD_ZERO(&rset);
		FD_SET(sockfd, &rset);
		wset = rset;
		struct timeval tval;
		tval.tv_sec = timeout;
		tval.tv_usec = 0;
		if ((res = select(sockfd + 1, &rset, &wset, NULL, timeout ? &tval : NULL)) == 0) {
			::close(sockfd);
			errno == ETIMEDOUT;
			return -1;
		}
		if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
			socklen_t len = sizeof(error);
			if ((getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len)) < 0) {
				return -1;
			}
		}
		else {
			LOG(FATAL) << "select error: sockfd not set";
		}
	}
	if (error) {
		::close(sockfd);
		errno = error;
		return -1;
	}
	return 0;

	
	
	if (::connect(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1) {
		LOG(ERROR) << "connect." << strerror(errno);
		return -1;
	}

	//struct sockaddr_in sockaddr;
	socklen_t len = sizeof(sockaddr_in);

	if (getpeername(sockfd, (struct sockaddr *)&sockaddr, &len) == -1) {
		LOG(ERROR) << "getpeername.";
		return -1;
	}

	type = CONNECTION_SOCKET;
	peerAddr = sockaddr.sin_addr;
	peerPort = sockaddr.sin_port;
	return 0;
	
}
*/

int Socket::getSockfd() const
{
	return sockfd;
}

std::string Socket::getAddr() const
{
	char ip[INET_ADDRSTRLEN];
	const char *ans = inet_ntop(AF_INET, &addr, ip, sizeof(ip));
	if (!ans)
	{
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, "inet_ntop");
	}
	return std::string(ans);
}

unsigned short Socket::getPort() const
{
	return ntohs(port);
}

std::string Socket::getPeerAddr() const
{
	char ip[INET_ADDRSTRLEN];
	const char *ans = inet_ntop(AF_INET, &peerAddr, ip, sizeof(ip));
	if (!ans)
	{
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, "inet_ntop");
	}
	return std::string(ans);
}

unsigned short Socket::getPeerPort() const
{
	return ntohs(peerPort);
}

int Socket::getBacklog() const
{
	return backlog;
}

int Socket::read(char *buffer, int length)
{
	return ::recv(sockfd, buffer, length, 0);
}

int Socket::readAbleNum()
{
	int num;
	if (ioctl(sockfd, FIONREAD, &num) == -1)
	{
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, std::string("ioctl") + strerror(errno));
		return 0;
	}
	return num;
}

int Socket::write(const char *buffer, int length)
{
	return ::send(sockfd, buffer, length, MSG_NOSIGNAL);
}

int Socket::writen(const char *buffer, int length)
{
	int nwrote = 0, cnt;
	while (nwrote < length)
	{
		cnt = write(buffer + nwrote, length - nwrote);
		if (cnt == -1)
		{
			return cnt;
		}
		nwrote += cnt;
	}
	return nwrote;
}
