#include "easylogging++.h"
#include <thread>
#include "Socket.h"
#include "Msg.h"
#include "md5.h"
#include "Buffer.h"
#include "unistd.h"
#include <string>
#include <iostream>

#define ELPP_THREAD_SAFE

INITIALIZE_EASYLOGGINGPP

Socket sock;
Buffer recvBuf;

void handleConfirm() {
	auto type = recvBuf.getUint8();
	if (type == Msg::MSG_TYPE::SIGN_UP) {
		LOG(INFO) << "sign up sent.";
	}
	else if(type==Msg::MSG_TYPE::LOGIN_IN){
		LOG(INFO) << "login in sent.";
	}
	else if (type == Msg::MSG_TYPE::TO_SB) {
		LOG(INFO) << "to sb sent.";
	}
}

void handleSignUp() {
	uint8_t res = recvBuf.getUint8();
	if (res == SUCCESS) {
		uint32_t id = recvBuf.getUint32();
		LOG(INFO) << "sign up success." << id;
	}
	else {
		LOG(ERROR) << "sign up fail.";
	}

	std::shared_ptr<Msg> pConMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::CONFIRM));
	pConMsg->writeUint8(Msg::MSG_TYPE::SIGN_UP_ANS);
	sock.writen(pConMsg->getBuf(),pConMsg->getLen());
}

void handleLoginIn() {
	uint8_t res = recvBuf.getUint8();
	if (res == SUCCESS) {
		LOG(INFO) << "login in success";
	}
	else {
		uint8_t type = recvBuf.getUint8();
		if (type == LOGINED) {
			LOG(ERROR) << "you have logined.";
		}
		else if (type == USERORPWDNOTCORR) {
			LOG(ERROR) << "user or password not correct.";
		}
	}
	std::shared_ptr<Msg> pConMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::CONFIRM));
	pConMsg->writeUint8(Msg::MSG_TYPE::LOGIN_IN_ANS);
	sock.writen(pConMsg->getBuf(), pConMsg->getLen());
}

void handleFromSb(int msglen) {
	uint32_t fromid = recvBuf.getUint32();
	std::string str = recvBuf.getString(msglen - 3 - 4);
	LOG(INFO) << str;
	std::shared_ptr<Msg> pConMsg(new Msg(Msg::headerLen + 1, Msg::MSG_TYPE::CONFIRM));
	pConMsg->writeUint8(Msg::MSG_TYPE::TO_SB);
	sock.writen(pConMsg->getBuf(), pConMsg->getLen());
}

void handle() {
	while (true) {
		sleep(1);
		int cnt = recvBuf.readin(&sock, sock.readAbleNum());
		//std::cout << cnt << strerror(errno)<< errno << std::endl;
		if (cnt > 0) {
			std::cout << cnt << std::endl;
			while (true) {
				int msglen;
				std::cout << recvBuf.length() << std::endl;
				if (recvBuf.length() >= Msg::headerLen) {
					msglen = recvBuf.peekUint16();
					if (recvBuf.length() < msglen) {
						break;
					}
				}
				else {
					break;
				}
				msglen = recvBuf.getUint16();
				auto type = recvBuf.getUint8();

				switch (type) {
				case Msg::MSG_TYPE::CONFIRM:
					handleConfirm();
					break;
				case Msg::MSG_TYPE::SIGN_UP_ANS:
					//LOG(INFO) << "SIGN_UP from" << pTcpConnection->getfd()->getPeerAddr()
						//<< ": " << pTcpConnection->getfd()->getPeerPort();
					handleSignUp();
					break;
				case Msg::MSG_TYPE::LOGIN_IN_ANS:
					//LOG(INFO) << "LOGIN_IN from" << pTcpConnection->getfd()->getPeerAddr()
					//	<< ": " << pTcpConnection->getfd()->getPeerPort();
					handleLoginIn();
					break;
				case Msg::MSG_TYPE::SHOW_SB:
					//handleShowSb();
					break;
				case Msg::MSG_TYPE::ADD_SB:
					//handleAddSb();
					break;
				case Msg::MSG_TYPE::DELETE_SB:
					//handleDeleteSb();
					break;
				case Msg::MSG_TYPE::TO_SB:
					//handleToSb(pBuffer, msglen);
					break;
				case Msg::MSG_TYPE::FROM_SB:
					handleFromSb(msglen);
					break;
				default:
					break;
				}
			}
		}
		else {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				continue;
			}
			sock.close();
		}
		sleep(1);
	}
}

int main() {

	if (sock.connect("192.168.0.11", 4399, 10) == -1) {
		exit(1);
	}

	std::thread t(handle);

	std::string line;
	std::string comm;
	while (std::cin >> comm) {
		if (comm == "signup") {
			std::string nickname, password;
			std::cin >> nickname >> password;
			Msg msg(Msg::headerLen + 64, Msg::MSG_TYPE::SIGN_UP);
			nickname.resize(32);
			password = MD5(password).toString();
			msg.writeString(nickname.c_str(), 32);
			msg.writeString(password.c_str(), 32);
			sock.writen(msg.getBuf(), msg.getLen());
		}
		else if (comm == "login") {
			std::uint32_t id;
			std::string password;
			std::cin >> id >> password;
			password = MD5(password).toString();
			Msg msg(Msg::headerLen + 36, Msg::MSG_TYPE::LOGIN_IN);
			msg.writeUint32(id);
			msg.writeString(password.c_str(), 32);
			sock.writen(msg.getBuf(), msg.getLen());
		}
		else if (comm == "tell") {
			std::uint32_t id;
			std::cin >> id;
			std::string line;
			std::getline(std::cin, line);
			Msg msg(Msg::headerLen + 4 + line.length(), Msg::MSG_TYPE::TO_SB);
			msg.writeUint32(id);
			msg.writeString(line.c_str(), line.length());
			sock.writen(msg.getBuf(), msg.getLen());
		}
	}
}
