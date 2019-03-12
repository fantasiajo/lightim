#pragma once
#include "Buffer.h"

#define FAIL 0
#define SUCCESS 1

#define USERORPWDNOTCORR 0
#define LOGINED 2

//Msg:|len 2 bytes|MSG_TYPE 1 bytes|data len-3 bytes|
class Msg {
public:
	enum MSG_TYPE {
		CONFIRM,//server <-> client
		//|type|
		SIGN_UP,//server <- client
		//|nickname 32 bytes|pwdmd5 32 bytes|
		SIGN_UP_ANS,//server -> client
		//|successorfail 1 byte|
		LOGIN_IN,//server <- client
		//|id 4 bytes|pwdmd5 32bytes|
		LOGIN_IN_ANS,//server -> client
		//|successorfail 1 byte|fialtype 1 byte|
		SHOW_SB,
		SHOW_SB_ANS,
		ADD_SB,//server <- client
		//|id 4 bytes|
		ADD_FROM_SB,//server -> client
		//|id 4 bytes|
		AGREE_SB,//server <- client
		//|id 4 bytes|
		AGREE_FROM_SB,//server -> client
		//|id 4 bytes|
		DELETE_SB,
		DELETE_SB_ANS,
		TO_SB,
		//|targetid 4 bytes|content len - 3 - 4 bytes|
		FROM_SB,
		//|targetid 4 bytes|content len - 3 - 4 bytes|
		GET_FRIENDS,
		//||
		GET_FRIENDS_ANS
		//|id 4 + nickname 32|...
	};

	Msg(uint16_t _len, MSG_TYPE _type);
	~Msg();

	Msg(const Msg &) = delete;
	Msg &operator=(const Msg &) = delete;
	Msg(Msg &&) = delete;
	Msg &operator=(Msg &&) = delete;

	char *getBuf() {
		return buf;
	}
	uint16_t getLen() {
		return len;
	}

	void writeUint8(uint8_t n);
	void writeUint16(uint16_t n);
	void writeUint32(uint32_t n);
	void writeString(const char* str, int len);

	const static uint16_t headerLen = 3;
private:
	MSG_TYPE type;
	uint16_t len;
	uint16_t curr;
	char *buf;
};

