#include "DB.h"
#include "easylogging++.h"

DB::DB(std::string host, std::string user, std::string pwd, std::string db_name)
{
	conn = mysql_init(NULL);
	if (!conn) {
		LOG(FATAL) << "mysql_init failed:" << mysql_error(conn);
		exit(1);
	}
	conn = mysql_real_connect(conn, host.c_str(), user.c_str(), pwd.c_str(),
		db_name.c_str(), 0, NULL, 0);
	if (!conn) {
		LOG(FATAL) << "mysql_real_connect failed." << mysql_error(conn);
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
		LOG(ERROR) << sql << ": " << mysql_error(conn);
		return false;
	}
	else {
		queryResult.clear();
		res = mysql_store_result(conn);
		if (!res) return true;
		auto numRows = mysql_num_rows(res);
		auto numCols = mysql_num_fields(res);
		auto fields = mysql_fetch_fields(res);

		for (int i = 0; i < numCols; ++i) {
			queryResult.fields[std::string(fields[i].name)] = i;
		}

		queryResult.dataMatrix.resize(numRows);
		for (int i = 0; i < numRows; ++i) {
			queryResult.dataMatrix[i].resize(numCols);
			row = mysql_fetch_row(res);
			for (int j = 0; j < numCols; ++j) {
				queryResult.dataMatrix[i][j] = std::string(row[j]);
			}
		}

		mysql_free_result(res);
		return true;
	}
}

std::string DB::escape(const std::string &content)
{
	char to[2 * content.length() + 1];
	mysql_real_escape_string(conn, to, content.c_str(), content.length());
	return to;
}

void QUERY_RESULT::print(std::ostream & os)
{
	for (const auto &field : fields) {
		os << field.first << "\t";
	}
	os << std::endl;

	for (int i = 0; i < dataMatrix.size(); ++i) {
		for (const auto &field : fields) {
			os << get(i, field.first) << "\t";
		}
		os << std::endl;
	}
	os << std::endl;
}
