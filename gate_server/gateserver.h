#ifndef GATE_SERVER_H_
#define GATE_SERVER_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include <iostream>
#include <string>
#include <stdlib.h>

#include <event.h>
#include <pthread.h>

#include "chatroom.h"

const int MAX_IP_LEN = 0x21;
#pragma pack(1)
struct ConfigInfo
{
	char m_server_ip[MAX_IP_LEN];
	int m_server_port;
	int m_count_worker_thread;
	int m_timeout;
	int m_conn_num;
	int m_count_user;
	int m_file_num;
};
#pragma pack()

class GateServer
{
private:
	ConfigInfo m_conf;
	struct event_base *m_base;
	struct event m_listen_event;
public:
	GateServer();
	~GateServer();

public:
	bool InitServer();
	bool StartServer();
	void StopServer();
};

#endif
