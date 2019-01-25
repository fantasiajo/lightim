#pragma once

#include <vector>
#include "unistd.h"
#include "Socket.h"

class Buffer {
public:
	Buffer();
	~Buffer();

	bool empty();
	int length();
	int leftSpace();

	void in(const char* buf,int len);
	void out(char* buf, int len);
	//void recycleSpace();

	uint16_t peekUint16();
	uint16_t getUint16();

	uint8_t getUint8();

	std::string getString(int len);

	int readin(Socket &socket, int len);

	int writeout(Socket socket);

private:
	std::vector<char> str;
	int readIndex;
	int writeIndex;
};
