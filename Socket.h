#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string>

class Socket {
public:
	enum SOCKET_TYPE{LISTENING_SOCKET,CONNECTION_SOCKET};
	typedef in_addr IP;
	typedef in_port_t Port;

	Socket();
	Socket(std::string _ip, unsigned short _port);
	~Socket();

	void bind();
	void listen();
	Socket accept();
	void close();

	int connect(std::string ip, unsigned short _port);

	int getSockfd () const;
	std::string getAddr() const;
	unsigned short getPort() const;
	std::string getPeerAddr() const;
	unsigned short getPeerPort() const;

	int getBacklog() const;
	int read(char *buffer, int length);
	int readAbleNum();
	int write(const char *buffer, int length);
	int writen(const char *buffer, int length);

private:
	int sockfd;
	SOCKET_TYPE type;
	IP addr;
	Port port;
	IP peerAddr;
	Port peerPort;
	int backlog = 10;
};
