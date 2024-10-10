#pragma once
#include<WinSock2.h>
const unsigned  RECV_BUF_SIZE = 1024;

class Client {
public:
	Client(SOCKET servSock_);
	void setMsgBufCount(unsigned msgBufCount_);
	unsigned getMsgBufCount();
	char* getMsgBuf();
	SOCKET getSocket();
private:
	//Ӧ����clntSock���ø���
	SOCKET servSock;

	char szMsgBuf[RECV_BUF_SIZE * 10]{};

	unsigned msgBufCount = 0;
};
