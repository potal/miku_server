#ifndef CONNECTOR_H_
#define CONNECTOR_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

class Connector
{
private:
	std::string server_ip_;
	short server_port_;
	int client_fd_;

public:
	Connector();
	virtual ~Connector();

public:
	void InitConnectionInfo(const char *server_ip,const short server_port);
	bool ConnectServer();
	void Disconnect();
};

#endif
