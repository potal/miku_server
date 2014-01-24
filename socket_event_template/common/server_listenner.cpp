/*
 * =====================================================================================
 *
 *       Filename:  server_listenner.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/13/2014 06:35:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "server_listenner.h"
#include <sstream>
#include <fstream>
#include <fcntl.h>

ServerListenner::ServerListenner():server_listen_sock_(-1),base_hash_key_(0)
{
}

ServerListenner::~ServerListenner()
{
	SafeCloseSocket(server_listen_sock_);
}

bool ServerListenner::InitServer(std::string server_ip,int server_port,int worker_count,int count_user_per_worker,int read_timeout,int write_timeout)
{
	server_ip_ = server_ip;
	server_port_ = server_port;
	count_worker_ = worker_count;
	count_user_per_worker_ = count_user_per_worker;
	read_timeout_ = read_timeout;
	write_timeout_ = write_timeout;
	return true;
}

bool ServerListenner::InitWorker()
{
	int tmp_fds[2];
	if(pipe(tmp_fds))
	{
		std::cout<<"Error InitWorker pipe"<<std::endl;
		return false;
	}

	pipe_read_fd_ = tmp_fds[0];
	pipe_write_fd_ = tmp_fds[1];

	worker_base_ = event_init();
	event_set(&worker_event_,pipe_read_fd_,EV_READ|EV_PERSIST,ReadAction,this);
	event_base_set(worker_base_,&worker_event_);
	if(event_add(&worker_event_,0) < 0)
	{
		std::cout<<"Init worker error"<<std::endl;
		return false;
	}
	return true;
}

bool ServerListenner::StartWorker()
{
	pthread_t tmp_thread_id;
	int tmp_ret = pthread_create(&tmp_thread_id,NULL,WorkerThread,this);
	if(tmp_ret == 0)
	{
		std::cout<<"Create worker thread ok! ThreadID:"<<tmp_thread_id<<std::endl;
		return true;
	}
	return false;
}

bool ServerListenner::StartServer(BaseUserInfoResource *user_resource)
{
	user_info_resource_ = user_resource;

	if(!InitWorker())
	{
		std::cout<<"Init worker thread error"<<std::endl;
		return false;
	}

	if(!StartWorker())
	{
		std::cout<<"Start worker error"<<std::endl;
		return false;
	}

	if(!OpenServerSocket())
	{
		std::cout<<"Open socket error"<<std::endl;
		return false;
	}
	base_ = event_init();

	event_set(&listen_event_,server_listen_sock_,EV_READ|EV_PERSIST,AcceptAction,this);
	event_base_set(base_,&listen_event_);
	if(event_add(&listen_event_,0) < 0)
	{
		std::cout<<"event_add error"<<std::endl;
		return false;
	}
	event_base_dispatch(base_);
	return true;
}

void ServerListenner::SetNonblock()
{
	int tmp_flags = fcntl(server_listen_sock_,F_GETFL,0);
	tmp_flags |= O_NONBLOCK;
	fcntl(server_listen_sock_,F_SETFL,tmp_flags);
}

bool ServerListenner::OpenServerSocket()
{
	server_listen_sock_ = socket(AF_INET,SOCK_STREAM,NULL);
	if(server_listen_sock_ == -1)
		return false;
	struct sockaddr_in tmp_server_addr;
	memset(&tmp_server_addr,0,sizeof(tmp_server_addr));
	tmp_server_addr.sin_family = AF_INET;
	tmp_server_addr.sin_port = htons(server_port_);
	server_ip_ = "";
	tmp_server_addr.sin_addr.s_addr = (server_ip_ == "" ? INADDR_ANY : inet_addr(server_ip_.c_str()));
	int tmp_resue_addr_on = 1;
	int tmp_ret_code = 0;
	setsockopt(server_listen_sock_,SOL_SOCKET,SO_REUSEADDR,&tmp_resue_addr_on,sizeof(int));	
	tmp_ret_code = bind(server_listen_sock_,(struct sockaddr*)&tmp_server_addr,sizeof(tmp_server_addr));
	if(tmp_ret_code == -1)
	{
		return false;
	}
	tmp_ret_code = listen(server_listen_sock_,1024);
	if(tmp_ret_code == -1)
	{
		return false;
	}
	return true;
}

void ServerListenner::SafeCloseSocket(int sock)
{
	if(-1 != sock)
	{
		int tmp_return = close(sock);
		while(tmp_return != 0)
		{
			if(errno != EINTR || errno == EBADF)
				break;
			tmp_return = close(sock);
		}
	}
}


void *ServerListenner::WorkerThread(void *arg)
{
	ServerListenner *tmp_server = (ServerListenner *)arg;
	event_base_dispatch(tmp_server->worker_base_);
}

BaseUserInfo *ServerListenner::GetUserInfo()
{
	if(!user_info_resource_)
		return NULL;
	BaseUserInfo *tmp_user_info = user_info_resource_->GetUserInfo();

	return tmp_user_info;
}

void ServerListenner::ReadAction(int sock,short event_flag,void *action_class)
{
	ServerListenner *tmp_server = (ServerListenner*)action_class;
	int tmp_user_hash_key;
	int tmp_ret = read(sock,&tmp_user_hash_key,4);
	if(tmp_ret <= 0)
	{
		std::cout<<"Read buff error"<<std::endl;
		return ;
	}
	std::cout<<"ReadAction:"<<tmp_user_hash_key<<std::endl;
	if(!tmp_server->user_info_resource_)
		return;
	BaseUserInfo *tmp_user = tmp_server->user_info_resource_->GetUserByHashkey(tmp_user_hash_key);
	if(NULL != tmp_user)
	{
		bufferevent_setcb(tmp_user->buffev,DealWithReadData,NULL,ErrorRead,tmp_user);
		//std::cout<<"tmp_ret:"<<tmp_ret<<std::endl;
	}
	else
	{
		std::cout<<"Found no one"<<std::endl;
	}
}

void ServerListenner::AcceptAction(int sock,short event_flag,void *action_class)
{
	ServerListenner *tmp_server = (ServerListenner *)action_class;
	int tmp_client_sock = -1;
	struct sockaddr_in tmp_client_addr;
	socklen_t tmp_len = sizeof(struct sockaddr_in);

	tmp_client_sock = accept(sock,(struct sockaddr*)&tmp_client_addr,&tmp_len);
	if(tmp_client_sock < 0)
	{
		std::cout<<"accept error"<<std::endl;
	}
	unsigned long tmp_non_blocking = 1;
	if(ioctl(tmp_client_sock,FIONBIO,&tmp_non_blocking) < 0)
	{
		tmp_server->SafeCloseSocket(tmp_client_sock);
		tmp_client_sock = -1;
	}

	if(tmp_client_sock > 0)
	{
		std::cout<<"client connecting ip:"<<*(int *)&tmp_client_addr.sin_addr
			<<" port:"<<tmp_client_addr.sin_port<<std::endl;
		tmp_server->base_hash_key_++;
		BaseUserInfo * tmp_user = tmp_server->user_info_resource_->GetUserInfo();
		if(tmp_user)
		{
			tmp_user->user_sock = tmp_client_sock;
			tmp_user->hash_key = tmp_server->base_hash_key_;
			tmp_user->server_ptr = tmp_server;
			tmp_user->user_uip = *(int *)&tmp_client_addr.sin_addr;
			tmp_user->user_uport = tmp_client_addr.sin_port;
			tmp_user->server_ptr = tmp_server;
			bufferevent_base_set(tmp_server->worker_base_,tmp_user->buffev);
			bufferevent_settimeout(tmp_user->buffev,0,0);
			bufferevent_setfd(tmp_user->buffev,tmp_user->user_sock);
			bufferevent_enable(tmp_user->buffev,EV_READ|EV_WRITE);
			using_user_map_[tmp_user->hash_key] = tmp_user;
			tmp_server->user_info_resource_->AddUserInfo(tmp_server->base_hash_key_,tmp_user);
			write(tmp_server->pipe_write_fd_,&(tmp_server->base_hash_key_),4);
		}
	}
}

void ServerListenner::DealWithReadData(struct bufferevent *buffev,void *arg)
{
	BaseUserInfo * tmp_user = (BaseUserInfo *)arg;
	//std::cout<<tmp_user->hash_key<<":DealWithReadData"<<std::endl;
	tmp_user->DealWithData(buffev,arg);
//	char tmp_read_buff[0x1000] = {0};
//	int tmp_len = bufferevent_read(buffev,tmp_read_buff,0x1000);
//	std::cout<<"Read:"<<tmp_read_buff<<std::endl;
}

void ServerListenner::ErrorRead(struct bufferevent * buffev,short event_flag,void *arg)
{
	BaseUserInfo *tmp_user = (BaseUserInfo *)arg;
	std::cout<<"ErrorRead:"<<tmp_user->hash_key<<std::endl;
	ServerListenner *tmp_server = (ServerListenner *)tmp_user->server_ptr;
	if(tmp_server)
	{
		tmp_server->ReleaseUserInfo(tmp_user->hash_key,tmp_user);
	}
}

void ServerListenner::ReleaseUserInfo(int user_hashkey,BaseUserInfo *user)
{
	using_user_map_.erase(user_hashkey);
	user_info_resource_->ReleaseUserInfo(user);
}

void ServerListenner::StopServer()
{
}

