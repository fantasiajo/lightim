#include <iostream>
#include "Socket.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "easylogging++.h"


Socket::Socket()
	:sockfd(socket(AF_INET, SOCK_STREAM | O_NONBLOCK, 0))
{
	if (sockfd == -1) {
		LOG(FATAL) << "socket create failed.";
		exit(1);
	}
}

Socket::Socket(std::string _ip, unsigned short _port)
	:sockfd(socket(AF_INET,SOCK_STREAM | O_NONBLOCK,0))
{
	if (sockfd == -1) {
		std::cerr << "socket create failed.\n";
	}

	int optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		//log.err
		exit(1);
	}
	int status = inet_pton(AF_INET, _ip.c_str(), &addr);
	if (status == -1) {
		//log.err
		exit(1);
	}
	if (status == 0) {
		//log.err.ipcharformat
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
		std::cerr << "bind failed.\n";
		perror("bind");
		abort();
	}
}

void Socket::listen()
{
	type = LISTENING_SOCKET;
	int cnt=999;
	if (cnt = ::listen(sockfd, backlog) == -1) {
		perror("listen");
		abort();
	}
}

Socket Socket::accept() {
	int tmpfd = ::accept4(sockfd,NULL,0, SOCK_NONBLOCK);
	if (tmpfd == -1) {
		perror("accept4");
		fflush(stderr);
		exit(1);
	}

	struct sockaddr_in sockaddr;
	socklen_t len=sizeof(sockaddr_in);

	if (getpeername(tmpfd, (struct sockaddr *)&sockaddr, &len) == -1) {
		std::cout << "getpeername" << std::endl;
		exit(1);
	}

	Socket socket;
	socket.type = CONNECTION_SOCKET;
	socket.sockfd = tmpfd;
	socket.addr = addr;
	socket.port = port;
	socket.peerAddr = sockaddr.sin_addr;
	socket.peerPort = sockaddr.sin_port;
	
	return socket;
}

void Socket::close()
{
	::close(sockfd);
}

int Socket::connect(std::string ip, unsigned short _port)
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


int Socket::getSockfd() const{
	return sockfd;
}

std::string Socket::getAddr() const{
	char ip[INET_ADDRSTRLEN];
	const char *ans = inet_ntop(AF_INET, &addr, ip, sizeof(ip));
	if (!ans) {
		std::cout << "inet_ntop" << std::endl;
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
		std::cout << "inet_ntop" << std::endl;
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
		//log.err
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

