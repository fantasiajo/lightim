#ifndef SOCKET_H
#define SOCKET_H

class Socket {
public:
	Socket();
	Socket(const char *ip, short port);
	~Socket();

	void bind();
	void listen();

private:
	int sockfd;
	struct in_addr addr;
	in_port_t sin_port;
	int backlog = 10;
};

#endif
