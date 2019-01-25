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

uint16_t Buffer::peekUint16()
{
	return ::ntohs(*((uint16_t *)(&str[readIndex])));
}

uint16_t Buffer::getUint16()
{
	uint16_t n = peekUint16();
	readIndex += sizeof(uint16_t);
	return n;
}

uint8_t Buffer::getUint8()
{
	return *((uint8_t*)(&str[readIndex++]));
}

std::string Buffer::getString(int len)
{
	auto tmp = readIndex;
	readIndex += len;
	return std::string(&str[tmp],len);
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
