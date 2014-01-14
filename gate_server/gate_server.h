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

#include "room_manager.h"

class GateServer
{
private:	
	std::string m_server_ip;
	int m_server_port;
	int m_count_worker_thread;
	int m_timeout;
	int m_conn_num;
	int m_count_user;
	int m_file_num;

	int m_server_listen_sock;

	RoomManager m_room_manager;
	struct event_base *m_base;
	struct event m_listen_event;
public:
	GateServer();
	~GateServer();

public:
	bool InitServer();
	bool StartServer();
	void SetNonblock();
	bool OpenServerSocket();
	//bool RegisterFunc();
	void StopServer();
};

#endif
