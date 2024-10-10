#pragma once
#include "Client.h"
#include "message.hpp"
#include <map>
#include <vector>
#include<mutex>
#include "MessageInterface.h"
class RecvServer
{
public:
	RecvServer(MessageInterface* pMessageInterface_);
	~RecvServer();
	void close();
	void start();
	void addClientToBufVec(Client* pClient);
	unsigned getClientNum();
private:
	bool coreFunc();
	bool recvMsg(Client* pClient);
	bool conductMsg(Header* pHeader, Client* pClient);
	std::vector<Client*> clntBufVec;
	std::map<SOCKET, Client*> clntMap;
	char szBuf[RECV_BUF_SIZE]{};
	std::mutex _mutex;

	MessageInterface* pMessageInterface;
};
