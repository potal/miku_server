#ifndef GATE_SERVER_H_
#define GATE_SERVER_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#include <iostream>
#include <string>
#include <stdlib.h>
#include <errno.h>

#include <event.h>
#include <pthread.h>

#include "room_manager.h"

class GateServer
{
private:	
	std::string server_ip_;
	int server_port_;
	int count_worker_thread_;
	int timeout_;
	int conn_num_;
	int count_user_;
	int file_num_;

	int server_listen_sock_;

	struct event_base *base_;
	struct event listen_event_;

	struct event_base *worker_base_;
	struct event worker_event_;
	int pipe_read_fd_;
	int pipe_write_fd_;
	struct event read_event_;
	struct event write_event_;

	struct event test_time_ev_;
	struct timeval test_time_val_;

	RoomManager room_manager_;
	UserInfoList user_info_list_;
	long base_hash_key_;

public:
	GateServer();
	~GateServer();
public:
	UserInfoList *GetUserInfoList(){return &user_info_list_;}
public:
	bool InitServer();
	bool StartServer();
	bool InitWorker();
	void SetNonblock();
	bool OpenServerSocket();
	void SafeCloseSocket(int sock);
	bool StartWorker();

	static void ReadAction(int sock,short event_flag,void *action_class);
	static void AcceptAction(int sock,short event_flag,void *action_class);
	//bool RegisterFunc();
	static void *WorkerThread(void *arg);
	
	void StopServer();

	void TestTime();
	static void TimeCallback(int sock,short event_flag,void *argc);
};

#endif
