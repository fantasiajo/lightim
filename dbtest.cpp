#include "DB.h"
#include <iostream>
#include <string>
#include "easylogging++.h"
using namespace std;

INITIALIZE_EASYLOGGINGPP


int main() {
	DB db("localhost", "root", "root", "lightim");
	string line;
	QUERY_RESULT queryResult;
	while (getline(cin, line)) {
		if (db.exeSQL(line, queryResult)) {
			for (const auto &field : queryResult.fields) {
				cout << field.first << "\t";
			}
			cout << endl;

			for (int i = 0; i < queryResult.dataMatrix.size(); ++i) {
				for (const auto &field : queryResult.fields) {
					cout << queryResult.get(i, field.first) << "\t";
				}
				cout << endl;
			}
			cout << endl;
		}
	}
}
