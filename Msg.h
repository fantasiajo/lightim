#pragma once
#include "Buffer.h"

//Msg:|len 2 bytes|MSG_TYPE 1 bytes|data len-3 bytes|
class Msg {
public:
	enum MSG_TYPE {
		SIGN_UP,
		//|nickname 32 bytes|pwdmd5 32 bytes|
		SIGN_UP_ANS,
		LOGIN_IN,
		LOGIN_IN_ANS,
		SHOW_SB,
		SHOW_SB_ANS,
		ADD_SB,
		ADD_SB_ANS,
		DELETE_SB,
		DELETE_SB_ANS,
		TO_SB,
		FROM_SB
	};

	Msg(uint16_t _len, MSG_TYPE _type);
	~Msg();

	char *getBuf() {
		return buf;
	}
	uint16_t getLen() {
		return len;
	}

	void writeUint8(uint8_t n);
	void writeUint16(uint16_t n);
	void writeString(const char* str, int len);

	const static uint16_t headerLen = 3;
private:
	MSG_TYPE type;
	uint16_t len;
	uint16_t curr;
	char *buf;
};

