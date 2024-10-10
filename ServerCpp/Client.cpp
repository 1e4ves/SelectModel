#include "Client.h"
Client::Client(SOCKET servSock_) : servSock(servSock_){

}
void Client::setMsgBufCount(unsigned msgBufCount_) {
	msgBufCount = msgBufCount_;
}

unsigned Client::getMsgBufCount() {
	return msgBufCount;
}
char* Client::getMsgBuf() {
	return szMsgBuf;
}
SOCKET Client::getSocket() {
	return servSock;
}