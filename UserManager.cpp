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
	QUERY_RESULT res;
	return ploop->getDb()->exeSQL(
		std::string("insert into msg(from_id,to_id,content) values ('") + std::to_string(fromid) + "','"
		+ std::to_string(toid) + "','" + ploop->getDb()->escape(content) + "')",
		res
	);
}

bool UserManager::addFriend(EventLoop * ploop,uint32_t id1, uint32_t id2) {
	QUERY_RESULT res;
	return ploop->getDb()->exeSQL(
		std::string("insert into friends(id1,id2) values ('")+std::to_string(id1)+"','"+
		std::to_string(id2)+"')",
		res
	);
}

bool UserManager::getFriends(EventLoop * ploop, uint32_t id, std::vector<std::pair<uint32_t, std::string>>& idname)
{
	QUERY_RESULT res;
	if (ploop->getDb()->exeSQL(
		std::string("select user_id, nickname from user where user_id in(select id2 from friends where id1 = '")
		+ std::to_string(id) + "')",
		res
	)) {
		idname.resize(res.dataMatrix.size());
		for (int i = 0; i < res.dataMatrix.size(); ++i) {
			idname[i].first = std::stoul(res.dataMatrix[i][0]);
			idname[i].second = res.dataMatrix[i][1];
		}
		return true;
	}
	return false;
}
bool UserManager::getAllUsers(EventLoop *ploop, std::vector<uint32_t> &ids) {
	QUERY_RESULT res;
	std::string sql("select user_id from user");
	if (ploop->getDb()->exeSQL(sql, res)) {
		ids.resize(res.dataMatrix.size());
		for (int i = 0; i < res.dataMatrix.size(); ++i) {
			ids[i] = std::stoul(res.dataMatrix[i][0]);
		}
	}
}
