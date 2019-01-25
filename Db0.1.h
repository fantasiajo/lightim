#pragma once
#include <mysql/mysql.h>
#include <string>
#include <vector>
#include <unordered_map>

class DB {
public:
	typedef std::unordered_map<std::string, std::vector<std::string>> QUERY_RESULT;
	DB(std::string host, std::string user, std::string pwd, std::string db_name);
	~DB();

	bool exeSQL(std::string sql, QUERY_RESULT &queryResult);
		
private:
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
};

