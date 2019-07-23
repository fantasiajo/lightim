#pragma once
#include "Buffer.h"
#include <unordered_map>

#define FAIL 0
#define SUCCESS 1

#define USERORPWDNOTCORR 0
#define LOGINED 2

//Msg:|len 2 bytes|MSG_TYPE 1 bytes|data len-3 bytes|
class Msg {
public:
	enum MSG_TYPE {
		CONFIRM,//server <-> client				0
		//|type|	
		SIGN_UP,//server <- client				1
		//|nickname 32 bytes|pwdmd5 32 bytes|	
		SIGN_UP_ANS,//server -> client			2
		//|successorfail 1 byte|
		LOGIN_IN,//server <- client				3
		//|id 4 bytes|pwdmd5 32bytes|
		LOGIN_IN_ANS,//server -> client			4
		//|successorfail 1 byte|fialtype 1 byte|
		SHOW_SB,//	server <- client			5
		SHOW_SB_ANS,//	server -> client		6
		ADD_SB,//server <- client				7
		//|id 4 bytes|
		ADD_FROM_SB,//server -> client			8
		//|id 4 bytes|
		AGREE_SB,//server <- client				9
		//|id 4 bytes|
		AGREE_FROM_SB,//server -> client		a
		//|id 4 bytes|
		DELETE_SB,//	server <- client		b
		DELETE_SB_ANS,//server -> client		c
		TO_SB,//		server <- client		d
		//|targetid 4 bytes|content len - 3 - 4 bytes|
		FROM_SB,//		server -> client		e
		//|fromid 4 bytes|msgid 8 bytes|content len - 3 - 4 - 8bytes|
		GET_FRIENDS,//	server <- client		f
		//||
		GET_FRIENDS_ANS,//	sever -> client		10
		//|id 4 + nickname 32|...
		HEART_BEAT//	server <-> client		11
	};

	Msg(const std::string&);
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
	void writeUint64(uint64_t n);
	void writeUint64(uint64_t n,uint16_t start);
	void writeString(const char* str, int len);

	const static uint16_t headerLen = 3;

	const static std::unordered_map<MSG_TYPE,std::string> typeMetaData;
private:
	MSG_TYPE type;
	uint16_t len;
	uint16_t curr;
	char *buf;
};

