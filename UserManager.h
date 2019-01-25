#pragma once
#include <string>

#define FAIL 0
#define SUCCESS 1

class EventLoop;

class UserManager {
public:
	UserManager(EventLoop *_ploop);

	//static bool exists(std::string nickname);
	static bool addUser(EventLoop *ploop,std::string nickname, std::string password);

private:
	EventLoop *ploop;
};
