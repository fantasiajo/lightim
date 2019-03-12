#include <iostream>
#include "Socket.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "easylogging++.h"
#include "error.h"


Socket::Socket()
	:sockfd(socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0))
{
	if (sockfd == -1) {
		LOG(FATAL) << "socket create failed.";
		exit(1);
	}
}

Socket::Socket(std::string _ip, unsigned short _port)
	:sockfd(socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK,0))
{
	if (sockfd == -1) {
		LOG(FATAL) << "socket create failed.";
		exit(1);
	}

	//开启地址复用来解决time_wait问题
	int optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		LOG(FATAL) << "reuseaddr failed.";
		exit(1);
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) == -1) {
		LOG(FATAL) << "reuseaddr failed.";
		exit(1);
	}
	int status = inet_pton(AF_INET, _ip.c_str(), &addr);
	if (status == -1) {
		LOG(FATAL) << "inet_pton:" << strerror(errno);
		exit(1);
	}
	if (status == 0) {
		LOG(FATAL) << "inet_pton:" << strerror(errno);
		exit(1);
	}
	port = htons(_port);
}

Socket::~Socket()
{
}

void Socket::bind() {
	struct sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = port;
	sockaddr.sin_addr = addr;
	if (::bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1) {
		LOG(FATAL) << "bind:" << strerror(errno);
		exit(1);
	}
}

void Socket::listen()
{
	type = LISTENING_SOCKET;
	int cnt;
	if (cnt = ::listen(sockfd, backlog) == -1) {
		LOG(FATAL) << "bind:" << strerror(errno);
		exit(1);
	}
}

std::shared_ptr<Socket> Socket::accept() {
	int tmpfd = ::accept4(sockfd,NULL,0, SOCK_NONBLOCK);
	if (tmpfd == -1) {
		LOG(FATAL) << "accept4:" << strerror(errno);
		return nullptr;
	}

	struct sockaddr_in sockaddr;
	socklen_t len=sizeof(sockaddr_in);

	if (getpeername(tmpfd, (struct sockaddr *)&sockaddr, &len) == -1) {
		LOG(FATAL) << "getpeername:" << strerror(errno);
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


int Socket::getSockfd() const{
	return sockfd;
}

std::string Socket::getAddr() const{
	char ip[INET_ADDRSTRLEN];
	const char *ans = inet_ntop(AF_INET, &addr, ip, sizeof(ip));
	if (!ans) {
		LOG(FATAL) << "inet_ntop:" << strerror(errno);
		exit(1);
	}
	return std::string(ans);
}

unsigned short Socket::getPort() const{
	return ntohs(port);
}

std::string Socket::getPeerAddr() const
{
	char ip[INET_ADDRSTRLEN];
	const char *ans = inet_ntop(AF_INET, &peerAddr, ip, sizeof(ip));
	if (!ans) {
		LOG(FATAL) << "inet_ntop:" << strerror(errno);
		exit(1);
	}
	return std::string(ans);
}

unsigned short Socket::getPeerPort() const
{
	return ntohs(peerPort);
}

int Socket::getBacklog() const{
	return backlog;
}

int Socket::read(char * buffer, int length)
{
	return ::recv(sockfd, buffer, length, 0);
}

int Socket::readAbleNum() {
	int num;
	if (ioctl(sockfd, FIONREAD, &num) == -1) {
		LOG(FATAL) << "ioctl:" << strerror(errno);
		return 0;
	}
	return num;
}

int Socket::write(const char *buffer, int length) {
	return ::send(sockfd, buffer, length, MSG_NOSIGNAL);
}

int Socket::writen(const char * buffer, int length)
{
	int nwrote = 0,cnt;
	while (nwrote < length) {
		cnt = write(buffer + nwrote, length - nwrote);
		if (cnt == -1) {
			return cnt;
		}
		nwrote += cnt;
	}
	return nwrote;
}

