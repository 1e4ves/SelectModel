#include "recvServer.h"
#include<thread>
#include<iostream>

RecvServer::RecvServer(MessageInterface* pMessageInterface_) : pMessageInterface(pMessageInterface_){
}
RecvServer::~RecvServer() {
	close();
}
void RecvServer::close() {
	if (!(clntBufVec.empty() && clntMap.empty())) {
		for (auto pClient : clntBufVec) {
			closesocket(pClient->getSocket());
			delete pClient;
		}
		for (auto clntMapPair : clntMap) {
			closesocket(clntMapPair.first);
			delete(clntMapPair.second);
		}
		clntBufVec.clear();
		clntMap.clear();
	}
}
void RecvServer::start() {
	std::thread myThread(&RecvServer::coreFunc, this);
	myThread.detach();
}
void RecvServer::addClientToBufVec(Client* pClient) {
	_mutex.lock();
	clntBufVec.push_back(pClient);
	_mutex.unlock();
}
bool RecvServer::coreFunc() {
	while (1) {
		fd_set fdRead;
		FD_ZERO(&fdRead);
		{
			_mutex.lock();
			if (!clntBufVec.empty()) {
				for (auto pClient : clntBufVec) {
					clntMap.insert(std::make_pair(pClient->getSocket(), pClient));
				}
				clntBufVec.clear();
			}
			_mutex.unlock();
		}
		for (auto clnPair : clntMap) {
			FD_SET(clnPair.first, &fdRead);
		}
		if (clntMap.empty()) {
			continue;
		}
		timeval tv{ 1, 0 };
		int selectRes = select(0, &fdRead, nullptr, nullptr, &tv);

		if (selectRes > 0) {
			for (unsigned i = 0; i < fdRead.fd_count; i++) {
				if (!recvMsg(clntMap[fdRead.fd_array[i]])) {
					std::cout << "recvMsg error!" << std::endl;
				}
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
bool RecvServer::recvMsg(Client* pClient) {
	int recvRes = recv(pClient->getSocket(), szBuf, RECV_BUF_SIZE, 0);
	pMessageInterface->recvMessage();
	if (recvRes <= 0) {
		std::cout << "recv func error, the error num is " << WSAGetLastError() << std::endl;
		/*		closesocket(servSock);
				for (auto client : clientList) {
					closesocket(client);
				}
				WSACleanup()*/;
				return false;
	}
	memcpy_s(pClient->getMsgBuf() + pClient->getMsgBufCount(),
		RECV_BUF_SIZE * 10 - pClient->getMsgBufCount(), szBuf, (unsigned)recvRes);
	pClient->setMsgBufCount(pClient->getMsgBufCount() + (unsigned)recvRes);

	while (pClient->getMsgBufCount() >= sizeof Header) {
		Header* pHeader = (Header*)pClient->getMsgBuf();
		if (pClient->getMsgBufCount() >= pHeader->dataLength) {
			unsigned tmpMsgBufLength = pClient->getMsgBufCount() - pHeader->dataLength;
			conductMsg(pHeader, pClient);
			memcpy(pClient->getMsgBuf(), pClient->getMsgBuf() + pHeader->dataLength,
				tmpMsgBufLength);
			pClient->setMsgBufCount(tmpMsgBufLength);
		}
		else {
			break;
		}
	}
	return true;
}
bool RecvServer::conductMsg(Header* pHeader, Client* pClient) {
	switch (pHeader->cmd) {
	case CMD::LOGIN: {
		LoginResult loginResult(10);
		if (send(pClient->getSocket(), (char*)&loginResult, sizeof LoginResult, 0) < 0) {
			std::cout << "send func error, the error num is " << WSAGetLastError() << std::endl;

			return false;
		}
		break;
	}
	case CMD::LOGOUT: {
		LogoutResult logoutResult(10);
		if (send(pClient->getSocket(), (char*)&logoutResult, sizeof LogoutResult, 0) < 0) {
			std::cout << "send func error, the error num is " << WSAGetLastError() << std::endl;
			return false;
		}
		break;
	}
	default:
		break;
	}

	return true;
}

unsigned RecvServer::getClientNum() {
	_mutex.lock();
	unsigned clientNum = clntBufVec.size() + clntMap.size();
	_mutex.unlock();
	return clientNum;
}