#ifndef CONNECTOR_H_
#define CONNECTOR_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <fcntl.h>
#include "circle_list.h"

class Connector
{
private:
	std::string server_ip_;
	short server_port_;
	int client_fd_;
	bool is_started_;
	CircleList send_list_;

public:
	Connector();
	virtual ~Connector();

public:
	void InitConnectionInfo(const char *server_ip,const short server_port);
	bool StartConnect();
	bool ConnectServer();	
	void Disconnect();
	bool SendData(char *buff,int len);
	int GetSocket();

	static void * SendDataThread(void *arg);
	static void * RecvDataThread(void *arg);
	static void * ReconnectThread(void *arg);

	virtual void DealWithData(char *buff,int len,int fd) = 0;
};

#endif
