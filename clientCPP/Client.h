#pragma once
#include<WinSock2.h>
#include"message.hpp"
#include<WS2tcpip.h>
#include<iostream>
const unsigned RECV_BUF_SIZE = 1024;
class Client {
public:
	Client();
	~Client();
	void close();
	bool init(const char* ip, unsigned int port);
	bool start();
	void recvMsg();
	bool conductMsg(Header* pHeader);
private:
	SOCKET servSock;
	//第一缓冲区
	char szBuf[RECV_BUF_SIZE]{};
	//第二缓冲区
	char szMsgBuf[RECV_BUF_SIZE * 10]{};
	unsigned msgBufLength = 0;
};



