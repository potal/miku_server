/*
 * =====================================================================================
 *
 *       Filename:  gateserver.cpp
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

#include "gate_server.h"
#include <sstream>
#include <fstream>
#include <fcntl.h>

GateServer::GateServer():server_port_(0),base_hash_key_(0)
{
}

GateServer::~GateServer()
{
	SafeCloseSocket(server_listen_sock_);
}

bool GateServer::InitServer()
{
	std::ifstream tmp_conf_file("init.conf");
	std::string tmp_str;
	while(std::getline(tmp_conf_file, tmp_str))
	{
		if (tmp_conf_file.eof())
			break;
		if(tmp_str.find_first_not_of(" #") == tmp_str.npos)//std::string::npos
			continue;

		std::stringstream tmp_ss;
		tmp_ss<<tmp_str;

		std::string tmp_name_str;
		tmp_ss>>tmp_name_str;
		if(tmp_name_str == "ip")
		{
			tmp_ss>>server_ip_;
		}
		else
		{
			int tmp_value;
			tmp_ss>>tmp_value;
			if(tmp_name_str == "port")
				server_port_ = tmp_value;
			else if(tmp_name_str == "worker_num")
				count_worker_thread_ = tmp_value;
			else if(tmp_name_str == "time_out")
				timeout_ = tmp_value;
			else if(tmp_name_str ==  "conn_num")
				conn_num_ = tmp_value;
			else if(tmp_name_str == "count_user")
				count_user_ = tmp_value;
			else if(tmp_name_str == "file_num")
				file_num_ = tmp_value;
		}
	}

	if(server_port_ == 0)
	{
		std::cout<<"port:"<<server_port_<<std::endl;
		return false;
	}
	return true;
}

bool GateServer::InitWorker()
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
	std::cout<<"Init Worker ok!"<<std::endl;
	return true;
}

bool GateServer::StartServer()
{
	if(!InitServer())
	{
		std::cout<<"Init server error"<<std::endl;
		return false;
	}

	// just open one worker thread here
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
	std::cout<<"GateServer::StartServer"<<std::endl;
//	TestTime();

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

void GateServer::SetNonblock()
{
	int tmp_flags = fcntl(server_listen_sock_,F_GETFL,0);
	tmp_flags |= O_NONBLOCK;
	fcntl(server_listen_sock_,F_SETFL,tmp_flags);
}

bool GateServer::OpenServerSocket()
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

void GateServer::SafeCloseSocket(int sock)
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

bool GateServer::StartWorker()
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

void *GateServer::WorkerThread(void *arg)
{
	GateServer *tmp_server = (GateServer *)arg;
	std::cout<<"WorkerThread"<<std::endl;
	event_base_dispatch(tmp_server->worker_base_);
}

void GateServer::ReadAction(int sock,short event_flag,void *action_class)
{
	GateServer *tmp_server = (GateServer*)action_class;
	std::cout<<"notify pipe read"<<std::endl;
	int tmp_user_hash_key;
	int tmp_ret = read(sock,&tmp_user_hash_key,4);
	if(tmp_ret <= 0)
	{
		std::cout<<"Read buff error"<<std::endl;
		return ;
	}
	std::cout<<tmp_user_hash_key<<std::endl;
}

void GateServer::AcceptAction(int sock,short event_flag,void *action_class)
{
	GateServer *tmp_server = (GateServer *)action_class;
	int tmp_client_sock = -1;
	struct sockaddr_in tmp_client_addr;
	socklen_t tmp_len = sizeof(struct sockaddr_in);

	std::cout<<"Call AcceptAction function"<<std::endl;

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
		UserInfo tmp_new_user;
		tmp_new_user.hash_key = tmp_server->base_hash_key_;
		tmp_new_user.user_sock = tmp_client_sock;
		tmp_new_user.user_id = tmp_server->base_hash_key_;
		tmp_server->GetUserInfoList()->AddUserInfo(tmp_server->base_hash_key_,tmp_new_user);
		// notify worker thread the new user is already
		write(tmp_server->pipe_write_fd_,&(tmp_server->base_hash_key_),4);
	}
	else
	{
		std::cout<<"error :"<<std::endl;
	}
}

void GateServer::StopServer()
{

}

void GateServer::TestTime()
{
	test_time_val_.tv_sec = 10;
	test_time_val_.tv_usec = 0;
	evtimer_set(&test_time_ev_,TimeCallback,this);
	event_add(&test_time_ev_,&test_time_val_);
	event_base_dispatch(base_);
}

void GateServer::TimeCallback(int sock,short event_flag,void *argc)
{
	GateServer * tmp_server = (GateServer*)argc;
	std::cout<<"timer wakeup!"<<std::endl;
	event_add(&(tmp_server->test_time_ev_),&(tmp_server->test_time_val_));
}
