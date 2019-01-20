#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <vector>
#include <memory>
#include <unordered_set>
#include "TcpConnection.h"

class TcpServer {
public:
	TcpServer();
	~TcpServer();

private:
	std::unordered_set<std::shared_ptr<TcpConnection>> conSet;
};

#endif
