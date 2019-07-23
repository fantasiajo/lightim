#pragma once
#include <mysql/mysql.h>
#include <string>
#include <vector>
#include <unordered_map>

#define MYSQL_HOST "localhost"
#define MYSQL_USER "root"
#define MYSQL_PWD ""
#define MYSQL_DB_NAME "lightim"

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
	DB(std::string host = MYSQL_HOST, std::string user = MYSQL_USER, std::string pwd = MYSQL_PWD, std::string db_name = MYSQL_DB_NAME);
	~DB();

	DB(const DB &) = delete;
	DB &operator=(const DB &) = delete;
	DB(DB &&) = delete;
	DB &operator=(DB &&) = delete;

	bool exeSQL(std::string sql, QUERY_RESULT &queryResult);
	std::string escape(const std::string &content);
		
private:
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	unsigned long *lengths;
};


