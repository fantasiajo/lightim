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

	int readin(Socket &socket, int len);

	int writeout(Socket socket);

private:
	std::vector<char> str;
	int readIndex;
	int writeIndex;
};
