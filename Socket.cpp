#include <iostream>
#include "Socket.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "arpa/inet.h"
#include "unistd.h"

Socket::Socket()
{
}

Socket::Socket(const char * ip, unsigned short port)
	:sockfd(socket(AF_INET,SOCK_STREAM | O_NONBLOCK,0))
{
	if (sockfd == -1) {
		//log.err
	}

	int optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		//log.err
	}
	int status = inet_pton(AF_INET, ip, &addr);
	if (status == -1) {
		//log.err
	}
	if (status == 0) {
		//log.err.ipcharformat
	}
	sin_port = port;
}

Socket::Socket(const Socket &socket)
	:sockfd(socket.getSockfd()),
	addr(socket.getAddr()),
	sin_port(socket.getPort()),
	backlog(socket.getBacklog())
{

}

Socket::~Socket()
{
}

void Socket::bind() {
	struct sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(sin_port);
	sockaddr.sin_addr = addr;
	if (::bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1) {
		std::cerr << "bind failed.\n";
		perror("bind");
		abort();
	}
}

void Socket::listen()
{
	int cnt=999;
	if (cnt = ::listen(sockfd, backlog) == -1) {
		perror("listen");
		abort();
	}
}

Socket Socket::accept() {
	struct sockaddr_in tmpsockaddr;
	int tmpfd = ::accept4(sockfd, (struct sockaddr *)&tmpsockaddr,nullptr,SOCK_NONBLOCK);
	if (tmpfd == -1) {
		//log.err
	}
	Socket socket;
	socket.sockfd = tmpfd;
	socket.addr = tmpsockaddr.sin_addr;
	socket.sin_port = tmpsockaddr.sin_port;
	return socket;
}

void Socket::close()
{
	::close(sockfd);
}

int Socket::getSockfd() const{
	return sockfd;
}

struct in_addr Socket::getAddr() const{
	return addr;
}

in_port_t Socket::getPort() const{
	return sin_port;
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

