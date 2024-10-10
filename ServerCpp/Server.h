#pragma once
#include "recvServer.h"
#include "MessageInterface.h"
const unsigned RECV_SERVER_SIZE = 4;
class Server : public MessageInterface{
public:
	Server();
	~Server();
	void close();
	bool init(const char* ip, unsigned short port);
	bool start();
	void addClientToRecvServer(Client* pClient);
	void recvMessage();
private:
	bool coreFunc();
	SOCKET servSock;
	std::vector<RecvServer*> recvServerVec;
};
