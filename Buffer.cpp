#include "Buffer.h"
#include "EventLoop.h"
#include "Msg.h"

Buffer::Buffer(bool _isSend)
	:readIndex(0),writeIndex(0),isSend(_isSend)
{
	str.resize(MINSIZE);
	if (isSend) {
		confirmIndex = 0;
	}
}

bool Buffer::allConfirm() {
	return confirmIndex == writeIndex;
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

void Buffer::recycleSpace()
{
	if (isSend) {
		//sendbuf
		if (str.size() > MINSIZE && (writeIndex - confirmIndex) * 5 < str.size()) {
			std::copy(str.begin() + confirmIndex, str.begin() + writeIndex, str.begin());
			if ((writeIndex-confirmIndex) > MINSIZE) {
				str.resize(writeIndex - confirmIndex);
			}
			else {
				str.resize(MINSIZE);
			}
			str.shrink_to_fit();

			//下面不能调换顺序
			writeIndex -= confirmIndex;
			readIndex -= confirmIndex;
			confirmIndex = 0;
		}
	}
	else {
		//recvbuf
		if (str.size() > MINSIZE && length() * 5 < str.size()) {
			std::copy(str.begin() + readIndex, str.begin() + writeIndex, str.begin());
			if (length() > MINSIZE) {
				str.resize(length());
			}
			else {
				str.resize(MINSIZE);
			}
			str.shrink_to_fit();

			//下面两行不能调换顺序
			writeIndex -= readIndex;
			readIndex = 0;
		}
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

uint32_t Buffer::getUint32()
{
	auto tmp = readIndex;
	readIndex += sizeof(uint32_t);
	return ::ntohl(*((uint32_t *)(&str[tmp])));
}

uint8_t Buffer::getUint8()
{
	return *((uint8_t*)(&str[readIndex++]));
}

std::string Buffer::getString(int len)
{
	std::string res(&str[readIndex], len);
	readIndex += len;
	recycleSpace();
	return res;
}

int Buffer::readin(Socket *psocket, int len) {//只有recvbuf会调用
	while (leftSpace() < len) {
		str.resize(str.size() * 2);//类似vector的策略
	}
	int cnt = psocket->read(&str[writeIndex], len);
	if (cnt > 0) {
		writeIndex += cnt;
	}
	return cnt;
}

int Buffer::writeout(Socket *psocket)//只有sendbuf会调用
{
	int cnt = psocket->write(&str[readIndex], length());
	if (cnt > 0) {
		readIndex += cnt;
	}
	return cnt;
}

void Buffer::reset()
{
	if (confirmIndex == readIndex) return;
	auto len = ::ntohs(*((uint16_t *)(&str[confirmIndex])));
	auto type = *((uint8_t *)(&str[confirmIndex + 2]));
	while (confirmIndex!=readIndex && type == Msg::MSG_TYPE::CONFIRM) {
		confirmIndex += len;
		len = ::ntohs(*((uint16_t *)(&str[confirmIndex])));
		type = *((uint8_t *)(&str[confirmIndex + 2]));
	}
	readIndex = confirmIndex;
}

void Buffer::pushMsg(std::shared_ptr<Msg> pMsg)
{
	in(pMsg->getBuf(), pMsg->getLen());
	msgWritenCallback();
}

void Buffer::pushMsgInLoop(std::shared_ptr<Msg> pMsg)
{
	if (ploop->isInLoopThread()) {
		pushMsg(pMsg);
	}
	else {
		ploop->queueInLoop(std::bind(&Buffer::pushMsgInLoop, this, pMsg));//连续递归，直到到达负责该buffer的loop所在的线程
	}
}

void Buffer::confirm() {
	auto len = ::ntohs(*((uint16_t *)(&str[confirmIndex])));
	auto type = *(uint8_t *)(&str[confirmIndex + 2]);
	while (type == Msg::MSG_TYPE::CONFIRM) {
		confirmIndex += len;
		len = ::ntohs(*((uint16_t *)(&str[confirmIndex])));
		type = *(uint8_t *)(&str[confirmIndex + 2]);
	}
	confirmIndex += len;
	recycleSpace();
}

void Buffer::setMsgWritenCallback(const std::function<void()>& cb)
{
	msgWritenCallback = cb;
}
