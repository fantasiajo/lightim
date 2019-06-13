#include <iostream>
#include <string>
#include "Buffer.h"
using namespace std;

#define MAX_BUFFER_SIZE 100
char buf[MAX_BUFFER_SIZE];
int main() {
	Buffer b;
	char action;
	string str;
	int len;
	while (cin >> action) {
		switch (action) {
		case 'i':
			cin >> str;
			b.inbuffer(str.c_str(), str.length());
			break;
		case 'o':
			cin >> len;
			b.outbuffer(buf, len);
			buf[len] = '\0';
			cout << buf << endl;
			break;
		default:
			break;
		}
	}
	return 0;
}
