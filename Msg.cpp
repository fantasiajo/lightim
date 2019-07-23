#include "Msg.h"
#include "easylogging++.h"
#include <cstring>
#include "Singleton.h"
#include "LogManager.h"
#include "util.h"

const std::unordered_map<Msg::MSG_TYPE, std::string> Msg::typeMetaData{
	{MSG_TYPE::ADD_SB, "ADD FRIEND"},
	{MSG_TYPE::AGREE_SB, "AGREE FRIEND"},
	{MSG_TYPE::TO_SB, "CHAT"}
};

Msg::Msg(const std::string &content)
	:len(content.length()),
	curr(0)
{
	buf = (char *)malloc(len);
	if (!buf) {
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, "malloc failed.");
	}
	writeString(content.c_str(),content.length());
}

Msg::Msg(uint16_t _len, MSG_TYPE _type)
	:type(_type),
	len(_len),
	curr(0),
	buf(nullptr)
{
	buf = (char *)malloc(len);
	if (!buf) {
		Singleton<LogManager>::instance().logInQueue(LogManager::LOG_TYPE::FATAL_LEVEL, "malloc failed.");
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

void Msg::writeUint64(uint64_t n){
	*((uint64_t *)(buf + curr)) = ::htonll(n);
	curr += 8;
}

void Msg::writeUint64(uint64_t n, uint16_t start){
	*((uint64_t *)(buf + start)) = ::htonll(n);
}

void Msg::writeString(const char * str, int len)
{
	//strncpy(buf + curr, str, len);
	while (len--) {
		*(buf + (curr++)) = *(str++);
	}
	//curr += len;
}
