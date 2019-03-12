#include "DB.h"

DB::DB(std::string host, std::string user, std::string pwd, std::string db_name)
{
	conn = mysql_init(NULL);
	if (!conn) {
		LOG(FATAL) << "mysql_init failed:";
		exit(1);
	}
	conn = mysql_real_connect(conn, host.c_str(), user.c_str(), pwd.c_str(),
		db_name.c_str(), 0, NULL, 0);
	if (!conn) {
		LOG(FATAL) << "mysql_real_connect failed.";
		exit(1);
	}
}

DB::~DB()
{
	if (!conn) {
		mysql_close(conn);
	}
}

bool DB::exeSQL(std::string sql, QUERY_RESULT &queryResult)
{
	if (mysql_query(conn, sql.c_str())) {
		LOG(ERROR) << "mysql_query failed: " << sql;
		return false;
	}
	else {
		queryResult.clear();
		res = mysql_store_result(conn);
		auto numRows = mysql_num_rows(res);
		auto numCols = mysql_num_fields(res);
		auto fields = mysql_fetch_fields(res);
		std::string fieldName;
		std::vector<std::vector<std::string>> resbak(numRows);
		for (int i = 0; i < numRows; ++i) {
			resbak[i].resize(numCols);
			row = mysql_fetch_row(res);
			for (int j = 0; j < numCols; ++j) {
				resbak[i][j] = std::string(row[j]);
			}
		}

		for (int i = 0; i < numCols; ++i) {
			fieldName = std::string(fields[i].name);
			queryResult[fieldName] = std::vector<std::string>(numRows);
			for (int j = 0; j < numRows; ++j) {
				queryResult[fieldName][j] = resbak[j][i];
			}
		}

		mysql_free_result(res);
		return true;
	}
}
