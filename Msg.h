#pragma once

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
private:
	MSG_TYPE type;
	uint16_t len;
};
