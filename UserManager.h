#pragma once
#include <string>

class EventLoop;

class UserManager {
public:
	UserManager(EventLoop *_ploop);

	static bool exists(EventLoop *ploop, uint32_t id,std::string pwd);
	static bool addUser(EventLoop *ploop, std::string nickname, std::string password,uint32_t &id);
	static bool addChat(EventLoop *ploop, int fromid, int toid, std::string content);
private:
	EventLoop *ploop;
};
