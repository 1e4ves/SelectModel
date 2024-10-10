#include "Server.h"
#include<assert.h>
#include<iostream>
#include<WS2tcpip.h>
Server::Server() :servSock(INVALID_SOCKET) {};
Server::~Server() {
	close();
}
void Server::close() {
	if (servSock != INVALID_SOCKET) {
		closesocket(servSock);
	}
	for (auto pRecvServer : recvServerVec) {
		delete pRecvServer;
	}
	WSACleanup();
}
bool Server::init(const char* ip, unsigned short port) {
	WSADATA data{};
	if (WSAStartup(MAKEWORD(2, 2), &data) == SOCKET_ERROR) {
		std::cout << "WSAStartup func error, error num is " << WSAGetLastError() << std::endl;
		return -1;
	}

	//TCP 协议族
	servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (servSock == INVALID_SOCKET) {
		std::cout << "socket func error, error num is " << WSAGetLastError() << std::endl;
		return -1;
	}
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);

	inet_pton(AF_INET, ip, &servAddr.sin_addr.S_un.S_addr);
	if (bind(servSock, (sockaddr*)&servAddr, sizeof(sockaddr)) == SOCKET_ERROR) {
		std::cout << "bind func error, error num is " << WSAGetLastError() << std::endl;
		closesocket(servSock);
		servSock = INVALID_SOCKET;
		WSACleanup();
		return -1;
	}
	if (listen(servSock, 128) == -1) {
		std::cout << "listen func error, error num is " << WSAGetLastError() << std::endl;
		closesocket(servSock);
		servSock = INVALID_SOCKET;
		WSACleanup();
		return -1;
	}
	return true;
}
bool Server::start() {
	for (unsigned i = 0; i < RECV_SERVER_SIZE; ++i) {
		RecvServer* pRecvServer = new RecvServer(this);
		pRecvServer->start();
		recvServerVec.push_back(pRecvServer);
	}
	if (!coreFunc()) {
		return false;
	}
	return true;
}
bool Server::coreFunc() {
	assert(servSock != INVALID_SOCKET);
	while (1) {
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(servSock, &fdRead);
		timeval tv{ 1, 0 };
		int selectRes = select(0, &fdRead, nullptr, nullptr, &tv);

		if (selectRes > 0) {
			for (unsigned i = 0; i < fdRead.fd_count; i++) {
				SOCKET clntSock = accept(servSock, nullptr, nullptr);
				if (clntSock == INVALID_SOCKET) {
					std::cout << "accept func error, error num is: " << WSAGetLastError() << std::endl;
					//??
					break;
				}
				addClientToRecvServer(new Client(clntSock));
			}
		}
		//等待时间超过一秒钟
		else if (selectRes == 0) {
			std::cout << "do sth else" << std::endl;
			continue;
		}
		else {
			std::cout << "select func error, error name is " << WSAGetLastError() << std::endl;
			return false;
			continue;
		}
	}
	return true;
}

void Server::addClientToRecvServer(Client* pClient) {
	RecvServer* minClientRecvServer = recvServerVec[0];
	for (unsigned i = 0; i < RECV_SERVER_SIZE; i++) {
		if (recvServerVec[i]->getClientNum() < minClientRecvServer->getClientNum()) {
			minClientRecvServer = recvServerVec[i];
		}
	}
	minClientRecvServer->addClientToBufVec(pClient);
}

void Server::recvMessage() {
	std::cout << "recvServer recv message" << std::endl;
}

