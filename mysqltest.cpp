#include <mysql.h>
#include <my_global.h>

int main() {
	std::cout << mysql_get_client_info();
	return 0;
}
