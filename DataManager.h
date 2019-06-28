#pragma once
#include <string>
#include <vector>

class EventLoop;

class DataManager {
public:
	DataManager(EventLoop *_ploop);

	DataManager(const DataManager &) = delete;
	DataManager &operator=(const DataManager &) = delete;
	DataManager(DataManager &&) = delete;
	DataManager &operator=(DataManager &&) = delete;

	static bool exists(EventLoop *ploop, uint32_t id,std::string pwd);
	static bool addUser(EventLoop *ploop, std::string nickname, std::string password,uint32_t &id);
	static bool addChat(EventLoop *ploop, int fromid, int toid, std::string content);
	static bool addFriend(EventLoop *ploop, uint32_t id1,uint32_t id2);
	static bool getFriends(EventLoop *ploop, uint32_t id, std::vector<std::pair<uint32_t, std::string>> &idname);
	static bool getAllUsers(EventLoop *ploop, std::vector<uint32_t> &ids);
private:
	EventLoop *ploop;
};
