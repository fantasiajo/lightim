#pragma once

#include <vector>
#include "unistd.h"
#include "Socket.h"

class EventLoop;
class Msg;

class Buffer {
public:
	Buffer(bool isSend = false);
	~Buffer();

	bool allConfirm();

	bool empty();
	int length();
	int leftSpace();

	void in(const char* buf,int len);
	void out(char* buf, int len);
	//void recycleSpace();

	uint16_t peekUint16();
	
	uint8_t getUint8();
	uint16_t getUint16();
	uint32_t getUint32();

	std::string getString(int len);

	int readin(Socket *socket, int len);

	int writeout(Socket *socket);

	void setLoop(EventLoop *_ploop) {
		ploop = _ploop;
	}


	void pushMsg(std::shared_ptr<Msg> pMsg);
	void pushMsgInLoop(std::shared_ptr<Msg> pMsg);

	void confirm();

private:
	EventLoop *ploop;

	std::vector<char> str;
	int readIndex;
	int writeIndex;
	int confirmIndex;

	bool isSend;
};
