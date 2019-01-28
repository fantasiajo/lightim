#include "UserManager.h"
#include "DB.h"
#include "EventLoop.h"
#include <string>

UserManager::UserManager(EventLoop * _ploop)
	:ploop(_ploop)
{
}

//bool UserManager::exists(std::string nickname)
//{
//	if (ploop->getDb()->exeSQL(std::string("select nickname from user where nickname='")+nickname+"'", res)) {
//		return res.size() != 0;
//	}
//	else {
//		//todo,出现错误
//	}
//	return false;
//}

bool UserManager::exists(EventLoop * ploop, uint32_t id, std::string pwd)
{
	QUERY_RESULT res;
	ploop->getDb()->exeSQL(
		std::string("select nickname from user where user_id = '") + std::to_string(id) + "' and password = '" + pwd + "'",
		res
	);
	return !res.dataMatrix.empty();
}

bool UserManager::addUser(EventLoop *ploop,std::string nickname, std::string password,uint32_t &id)
{
	QUERY_RESULT res;

	if (!ploop->getDb()->exeSQL(
		std::string("insert into user(nickname,password) values('") + nickname + "','" + password + "')",
		res)) {
		return false;
	}
	if (!ploop->getDb()->exeSQL(
		std::string("select last_insert_id()"),
		res
	)) {
		return false;
	}
	id = std::stoul(res.dataMatrix[0][0]);
	return true;
}

bool UserManager::addChat(EventLoop * ploop, int fromid, int toid, std::string content)
{

	return false;
}
