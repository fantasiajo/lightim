#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "IOEventManager.h"
#include <memory>

class TcpConnection {
public:
	TcpConnection();
	~TcpConnection() {
		//֪ͨ��������ӵ�epoller��delete ioem
	}
private:
	std::shared_ptr<IOEventManager> pIOEM;
};

#endif
