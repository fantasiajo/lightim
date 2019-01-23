#include "Buffer.h"

Buffer::Buffer()
	:readIndex(0),writeIndex(0)
{
	str.resize(10);
}

Buffer::~Buffer() {

}

bool Buffer::empty() {
	return readIndex == writeIndex;
}

int Buffer::length() {
	return writeIndex - readIndex;
}

int Buffer::leftSpace() {
	return str.size() - writeIndex;
}

void Buffer::in(const char *buf, int len) {
	while (leftSpace() < len) {
		str.resize(str.size() * 2);
	}
	while (len--) {
		str[writeIndex++] = *(buf++);
	}
}

void Buffer::out(char *buf, int len) {
	while (len--) {
		*(buf++) = str[readIndex++];
	}
}

int Buffer::readin(Socket &socket, int len) {
	while (leftSpace() < len) {
		str.resize(str.size() * 2);
	}
	int cnt = socket.read(&str[writeIndex], len);
	if (cnt > 0) {
		writeIndex += cnt;
	}
	return cnt;
}

int Buffer::writeout(Socket socket)
{
	int cnt = socket.write(&str[readIndex], length());
	if (cnt > 0) {
		readIndex += cnt;
	}
	return cnt;
}
