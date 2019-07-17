#include "DataManager.h"
#include "DB.h"
#include "EventLoop.h"
#include <string>
#include "Msg.h"
#include <sstream>

DataManager::DataManager(EventLoop * _ploop)
	:pDB(new DB())
{
}

//bool DataManager::exists(std::string nickname)
//{
//	if (pDB->exeSQL(std::string("select nickname from user where nickname='")+nickname+"'", res)) {
//		return res.size() != 0;
//	}
//	else {
//		//todo,出现错误
//	}
//	return false;
//}

bool DataManager::exists(uint32_t id, std::string pwd)
{
	QUERY_RESULT res;
	pDB->exeSQL(
		std::string("select nickname from user where user_id = '") + std::to_string(id) + "' and password = '" + pwd + "'",
		res
	);
	return !res.dataMatrix.empty();
}

bool DataManager::getLastMsgId(uint32_t id,uint64_t &lastmsgid){
	QUERY_RESULT res;
	pDB->exeSQL(std::string("select lastmsgid from user where user_id = '") + std::to_string(id) + "'",res);
	if(res.dataMatrix.empty()){
		return false;
	}
	else{
		lastmsgid=std::stoull(res.dataMatrix[0][0]);
		return true;
	}
}

bool DataManager::addUser(std::string nickname, std::string password,uint32_t &id)
{
	QUERY_RESULT res;

	if (!pDB->exeSQL(
		std::string("insert into user(nickname,password) values('") + nickname + "','" + password + "')",
		res)) {
		return false;
	}
	if (!pDB->exeSQL(
		std::string("select last_insert_id()"),
		res
	)) {
		return false;
	}
	id = std::stoul(res.dataMatrix[0][0]);
	return true;
}

bool DataManager::addMsg(uint32_t fromid, uint32_t toid, Msg::MSG_TYPE type, const std::string &content)
{
	QUERY_RESULT res;
	return pDB->exeSQL(
		std::string("insert into msg(from_id,to_id,msg_type,content) values ('") + std::to_string(fromid) + "','"
		+ std::to_string(toid) + "','" + Msg::typeMetaData.at(type) + "','" + pDB->escape(content) + "')",
		res
	);
}

bool DataManager::addFriend(uint32_t id1, uint32_t id2) {
	QUERY_RESULT res;
	return pDB->exeSQL(
		std::string("insert into friends(id1,id2) values ('")+std::to_string(id1)+"','"+
		std::to_string(id2)+"')",
		res
	);
}

bool DataManager::getFriends(uint32_t id, std::vector<std::pair<uint32_t, std::string>>& idname)
{
	QUERY_RESULT res;
	if (pDB->exeSQL(
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

bool DataManager::getAllUsers(std::vector<uint32_t> &ids) {
	QUERY_RESULT res;
	std::string sql("select user_id from user");
	if (pDB->exeSQL(sql, res)) {
		ids.resize(res.dataMatrix.size());
		for (int i = 0; i < res.dataMatrix.size(); ++i) {
			ids[i] = std::stoul(res.dataMatrix[i][0]);
		}
		return true;
	}
	return false;
}

bool DataManager::getMsgsById(uint32_t id, std::vector<std::shared_ptr<Msg>> &pMsgs, uint32_t startid, uint32_t endid){
	QUERY_RESULT res;
	std::ostringstream oss;
	if(endid==0){
		oss << "select id,content from send_msg where to_id = " << id 
			<< " and id > " << startid;
	}
	else{
		oss << "select id,content from msg where to_id = " << id 
			<< " and id > " << startid << " and id < " << endid;
	}
	if(pDB->exeSQL(oss.str(),res)){
		pMsgs.resize(res.dataMatrix.size());
		for (int i = 0; i < res.dataMatrix.size(); ++i) {
			//msgs[i] = res.dataMatrix[i][0];
			pMsgs[i].reset(new Msg(res.dataMatrix[i][1]));
			pMsgs[i]->writeUint64(std::stoull(res.dataMatrix[i][0]),Msg::headerLen);
		}
		return true;
	}
	return false;
}

uint64_t DataManager::addSendMsg(uint32_t id, const std::string &content){
	QUERY_RESULT res;	
	std::ostringstream oss;
	oss << "insert into send_msg(to_id,content) values(" << id << "," << pDB->escape(content) << ")";
	if(pDB->exeSQL(oss.str(),res)){
		if(pDB->exeSQL("SELECT LAST_INSERT_ID()",
			res
		)){
			return std::stoull(res.dataMatrix[0][0]);
		}
		return 0;
	}
	return 0;
}