#pragma once
#include <mysql/mysql.h>
#include <string>
#include <vector>
#include <unordered_map>

#define HOST "localhost"
#define USER "root"
#define PWD "root"
#define DB_NAME "lightim"

class QUERY_RESULT {
public:
	std::string get(int row, std::string field) {
		if (row >= dataMatrix.size() || fields.find(field) == fields.end()) {
			return "";
		}
		return dataMatrix[row][fields[field]];
	}

	void print(std::ostream &os);

	void clear() {
		fields.clear();
		dataMatrix.clear();
	}
	std::unordered_map<std::string, int> fields;
	std::vector<std::vector<std::string>> dataMatrix;
};

class DB {
public:
	DB(std::string host, std::string user, std::string pwd, std::string db_name);
	~DB();

	bool exeSQL(std::string sql, QUERY_RESULT &queryResult);
		
private:
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
};


