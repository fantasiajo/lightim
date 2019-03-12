#include "Msg.h"
#include "easylogging++.h"
#include <cstring>

Msg::Msg(uint16_t _len, MSG_TYPE _type)
	:type(_type),
	len(_len),
	curr(0),
	buf(nullptr)
{
	buf = (char *)malloc(len);
	if (!buf) {
		LOG(FATAL) << "malloc failed";
		exit(1);
	}
	writeUint16(len);
	writeUint8(type);
}

Msg::~Msg()
{
	if (buf) {
		free(buf);
	}
}

void Msg::writeUint8(uint8_t n)
{
	*(buf + curr) = n;
	++curr;
}

void Msg::writeUint16(uint16_t n)
{
	*((uint16_t *)(buf + curr)) = ::htons(n);
	curr += 2;
}

void Msg::writeUint32(uint32_t n)
{
	*((uint32_t *)(buf + curr)) = ::htonl(n);
	curr += 4;
}

void Msg::writeString(const char * str, int len)
{
	strncpy(buf + curr, str, len);
	curr += len;
}
