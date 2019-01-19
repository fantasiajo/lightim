#ifndef BUFFER_H
#define BUFFER_H
#include <cstring>
#include <cmath>
#include <iostream>
#include <algorithm>
class Buffer {
public:
	Buffer();
	~Buffer();

	bool empty();
	int length();
	int inbuffer(const char *buf, int len);
	int outbuffer(char *buf, int len);
	int getCapacity();
	int getLeftSpace();

private:
	const static int STRIP_LEN = 10;
	typedef struct BufferStrip {
		char data[STRIP_LEN];
		struct BufferStrip *next;
	}BufferStrip;
	typedef struct {
		BufferStrip *pstrip;
		int offset;
	}Pos;
	//有效内容为[begin.offset,end.offset)，包括左边界，不包括右边界
	Pos begin;
	Pos end;
	BufferStrip *head;
	BufferStrip *tail;
	int stripnum;
	//begin.pstrip始终等于head，end.pstrip可为null。

	void headToTail();
	int expandSpace();

};

#endif
