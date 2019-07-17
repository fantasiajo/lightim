#pragma once
#include <string>
#include <vector>
#include "DB.h"
#include "Msg.h"

class EventLoop;

class DataManager {
public:
	DataManager(EventLoop*);

	DataManager(const DataManager &) = delete;
	DataManager &operator=(const DataManager &) = delete;
	DataManager(DataManager &&) = delete;
	DataManager &operator=(DataManager &&) = delete;

	bool exists(uint32_t id,std::string pwd);
	bool getLastMsgId(uint32_t id,uint64_t &lastmsgid);
	bool addUser(std::string nickname, std::string password,uint32_t &id);
	bool addMsg(uint32_t fromid, uint32_t toid, Msg::MSG_TYPE, const std::string &content);
	uint64_t addSendMsg(uint32_t id, const std::string &content);
	bool addFriend(uint32_t id1,uint32_t id2);
	bool getFriends(uint32_t id, std::vector<std::pair<uint32_t, std::string>> &idname);
	bool getAllUsers(std::vector<uint32_t> &ids);
	//获取id中msgid在(startid,endid)中的数据到msgs中
	bool getMsgsById(uint32_t id, std::vector<std::shared_ptr<Msg>> &pMsgs, uint32_t startid, uint32_t endid);
private:
	std::shared_ptr<DB> pDB;
};
