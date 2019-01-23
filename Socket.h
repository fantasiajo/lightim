#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

class Socket {
public:

	Socket();
	Socket(const Socket &);
	Socket(const char *ip, unsigned short port);
	~Socket();

	void bind();
	void listen();
	Socket accept();
	void close();

	int getSockfd () const;
	struct in_addr getAddr() const;
	in_port_t getPort() const;
	int getBacklog() const;
	int read(char *buffer, int length);
	int readAbleNum();
	int write(const char *buffer, int length);

private:
	int sockfd;
	struct in_addr addr;
	in_port_t sin_port;
	int backlog = 10;
};
