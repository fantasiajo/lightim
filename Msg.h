#pragma once
#include "Buffer.h"

//Msg:|len 2 bytes|MSG_TYPE 1 bytes|data len-3 bytes|
class Msg {
public:
	enum MSG_TYPE {
		SIGN_UP,
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
	const static int headerLen = 3;
private:
	MSG_TYPE type;
	uint16_t len;
	
};

