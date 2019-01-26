#pragma once
#include <string>



class EventLoop;

class UserManager {
public:
	UserManager(EventLoop *_ploop);

	static bool exists(EventLoop *ploop, uint32_t id,std::string pwd);
	static bool addUser(EventLoop *ploop,std::string nickname, std::string password);

private:
	EventLoop *ploop;
};
