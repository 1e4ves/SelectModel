#include "Server.h"

#pragma comment(lib, "ws2_32.lib")

int main() {
	Server server;
	if (!server.init("127.0.0.1", 2345)) {
		return 1;
	}

	
	server.start();


	system("pause");
	return 0;
}