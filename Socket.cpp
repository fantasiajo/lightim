#include "Socket.h"

Socket::Socket()
{
}

Socket::Socket(const char * ip, short port)
	:sockfd(socket(AF_INET,SOCK_STREAM | O_NONBLOCK,0))
{
	if (sockfd == -1) {
		//log.err
	}

	int optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval) == -1) {
		//log.err
	}
	int status = inet_pton(AF_INET, ip, &addr);
	if (status == -1) {
		//log.err
	}
	if (status == 0) {
		//log.err.ipcharformat
	}
	bind();
}

Socket::~Socket()
{
}

void Socket::bind() {
	struct sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = port;
	sockaddr.sin_addr = addr;
	if (::bind(sockfd, &sockaddr, sizeof(sockaddr) == -1) {
		//log.err
	}
}

void Socket::listen()
{
	if (::listen(sockfd, backlog) == -1) {
		//log.err
	}
}
