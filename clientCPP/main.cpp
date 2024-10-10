#include "Client.h"
#pragma comment(lib, "ws2_32.lib")
int main() {
	Client client;
	if (!client.init("127.0.0.1", 2345)) {
		return 1;
	}

	client.start();
	system("pause");
	return 0;
}