#include "Client.h"
#include<iostream>
#include<WS2tcpip.h>
#include<thread>
Client::Client() : servSock(INVALID_SOCKET) {}
Client::~Client() {
	close();
}
void Client::close() {
	if (servSock != INVALID_SOCKET) {
		closesocket(servSock);
	}
	WSACleanup();
}
bool Client::init(const char* ip, unsigned int port) {
	WSADATA data{};
	if (WSAStartup(MAKEWORD(2, 2), &data) == SOCKET_ERROR) {
		std::cout << "WSAStartup func error, error num is " << WSAGetLastError() << std::endl;
		return false;
	}

	servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (servSock == INVALID_SOCKET) {
		std::cout << "socket func error, error num is: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return false;;
	}
	sockaddr_in servAddr{};
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &servAddr.sin_addr.S_un.S_addr);
	if (connect(servSock, (sockaddr*)&servAddr, sizeof sockaddr) == SOCKET_ERROR) {
		std::cout << "connect fun error, error num is: " << WSAGetLastError() << std::endl;
		closesocket(servSock);
		servSock = INVALID_SOCKET;
		WSACleanup();
		return false;
	}
	std::cout << "connection success" << std::endl;
}
bool Client::start() {
	std::thread recvThread(&Client::recvMsg, this);
	recvThread.detach();
	while (1) {
		Login login("zhj", "123");
		if (send(servSock, (char*)&login, sizeof Login, 0) < 0) {
			std::cout << "send func error, error num is: " << WSAGetLastError() << std::endl;
			return false;
			break;
		}

		Sleep(100);
	}
	return true;
}

void Client::recvMsg() {
	while (1) {
		int recvRet = recv(servSock, szBuf, RECV_BUF_SIZE, 0);
		if (recvRet <= 0) {
			std::cout << "recv func error, error num is" << WSAGetLastError() << std::endl;
			return;
		}

		memcpy_s(szMsgBuf + msgBufLength, RECV_BUF_SIZE * 10 - msgBufLength,
			szBuf, (unsigned)recvRet);
		msgBufLength += (unsigned)recvRet;


		while (msgBufLength >= sizeof Header) {
			Header* pHeader = (Header*)szMsgBuf;
			if (pHeader->dataLength <= msgBufLength) {
				unsigned tmpLength = msgBufLength - pHeader->dataLength;
				if (!conductMsg(pHeader)) {
					return;
				}
				memcpy(szMsgBuf, szMsgBuf + pHeader->dataLength, tmpLength);
				msgBufLength = tmpLength;
			}
			else {
				break;
			}
		}
	}
}

bool Client::conductMsg(Header* pHeader) {
	switch (pHeader->cmd) {
	case CMD::LOGIN_RESULT: {
		LoginResult* pLoginResult = (LoginResult*)pHeader;
		std::cout << pLoginResult->loginResultNum << std::endl;
		break;
	}
	case CMD::LOGOUT_RESULT: {
		LogoutResult* pLogoutResult = (LogoutResult*)pHeader;
		std::cout << pLogoutResult->logoutResultNum << std::endl;
		break;
	}
	defaut: {
		break;
	}
	}
	return true;
}