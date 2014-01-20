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

GateServer::GateServer():server_port_(0)
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

bool GateServer::StartServer()
{
	if(!InitServer())
	{
		std::cout<<"Init server error"<<std::endl;
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
