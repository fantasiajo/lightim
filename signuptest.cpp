#include "Socket.h"
#include "Msg.h"
#include "easylogging++.h"
#include <string>
#include <iostream>
using namespace std;

INITIALIZE_EASYLOGGINGPP

int main() {
	Socket socket;
	if (socket.connect("192.168.254.71", 4399) == -1) {
		exit(1);
	}
	string nickname, password;
	while (cin >> nickname >> password) {
		Msg msg(Msg::headerLen + 64, Msg::MSG_TYPE::SIGN_UP);
		msg.writeString(nickname.c_str(), 32);
		msg.writeString(password.c_str(), 32);
		socket.writen(msg.getBuf(), msg.getLen());
	}
	return 0;
}
