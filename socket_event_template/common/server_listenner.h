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
#include <map>
#include <list>

//基础用户信息
#include "base_user_info.h"
//基础用户列表信息，也就是基础资源列表
#include "base_user_info_resource.h"

struct WorkerInfo
{
	struct event_base *worker_base;
	struct event worker_event;
	int pipe_read_fd;
	int pipe_write_fd;
	void *server_ptr;
	pthread_t worker_thread_id;
};

class ServerListenner
{
private:	
	std::string server_ip_;
	int server_port_;
	int count_worker_;
	int count_user_per_worker_;
	int read_timeout_;
	int write_timeout_;

	int server_listen_sock_;

	struct event_base *base_;
	struct event listen_event_;

	WorkerInfo *workers_ptr_;
//	struct event_base *worker_base_;
//	struct event worker_event_;
//	int pipe_read_fd_;
//	int pipe_write_fd_;
//	struct event read_event_;
//	struct event write_event_;

	int base_hash_key_;
	
	BaseUserInfoResource* user_info_resource_;
	std::map<int,BaseUserInfo *> using_user_map_;

public:
	ServerListenner();
	~ServerListenner();
	
public:
	bool InitServer(std::string server_ip,int server_port,int worker_count,int count_user_per_worker,int read_timeout,int write_timeout);
	bool InitWorker();
	bool StartWorker();
	bool StartServer(BaseUserInfoResource *user_resource);

	void SetNonblock();
	bool OpenServerSocket();
	void SafeCloseSocket(int sock);
	BaseUserInfo *GetUserInfo();
	void ReleaseUserInfo(int hash_key,BaseUserInfo *user);
	
	static void ReadAction(int sock,short event_flag,void *action_class);
	static void AcceptAction(int sock,short event_flag,void *action_class);
	static void DealWithReadData(struct bufferevent *buffev,void *arg);
	static void ErrorRead(struct bufferevent *buffev,short event_flag,void *arg);
	static void *WorkerThread(void *arg);
	
	void StopServer();

};

#endif
