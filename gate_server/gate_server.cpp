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

GateServer::GateServer():m_server_port(0)
{
}

GateServer::~GateServer()
{
	SafeCloseSocket(m_server_listen_sock);
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
			tmp_ss>>m_server_ip;
		}
		else
		{
			int tmp_value;
			tmp_ss>>tmp_value;
			if(tmp_name_str == "port")
				m_server_port = tmp_value;
			else if(tmp_name_str == "worker_num")
				m_count_worker_thread = tmp_value;
			else if(tmp_name_str == "time_out")
				m_timeout = tmp_value;
			else if(tmp_name_str ==  "conn_num")
				m_conn_num = tmp_value;
			else if(tmp_name_str == "count_user")
				m_count_user = tmp_value;
			else if(tmp_name_str == "file_num")
				m_file_num = tmp_value;
		}
	}

	if(m_server_port == 0)
	{
		std::cout<<"port:"<<m_server_port<<std::endl;
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
	m_base = event_init();
	std::cout<<"GateServer::StartServer"<<std::endl;

	m_test_time_val.tv_sec = 10;
	m_test_time_val.tv_usec = 0;
	evtimer_set(&m_test_time_ev,TestTime,this);
	event_add(&m_test_time_ev,&m_test_time_val);
	event_base_dispatch(m_base);
//	event_set(&m_listen_event,m_server_listen_sock,EV_READ|EV_PERSIST,AcceptAction,this);
//	event_base_set(m_base,&m_listen_event);
//	if(event_add(&m_listen_event,0) < 0)
//	{
//		std::cout<<"event_add error"<<std::endl;
//		return false;
//	}
//	event_base_dispatch(m_base);
	return true;
}

void GateServer::SetNonblock()
{
	int tmp_flags = fcntl(m_server_listen_sock,F_GETFL,0);
	tmp_flags |= O_NONBLOCK;
	fcntl(m_server_listen_sock,F_SETFL,tmp_flags);
}

bool GateServer::OpenServerSocket()
{
	m_server_listen_sock = socket(AF_INET,SOCK_STREAM,NULL);
	if(m_server_listen_sock == -1)
		return false;
	struct sockaddr_in tmp_server_addr;
	memset(&tmp_server_addr,0,sizeof(tmp_server_addr));
	tmp_server_addr.sin_family = AF_INET;
	tmp_server_addr.sin_port = htons(m_server_port);
	tmp_server_addr.sin_addr.s_addr = (m_server_ip == "" ? INADDR_ANY : inet_addr(m_server_ip.c_str()));
	int tmp_resue_addr_on = 1;
	int tmp_ret_code = 0;
	setsockopt(m_server_listen_sock,SOL_SOCKET,SO_REUSEADDR,&tmp_resue_addr_on,sizeof(int));	
	tmp_ret_code = bind(m_server_listen_sock,(struct sockaddr*)&tmp_server_addr,sizeof(tmp_server_addr));
	if(tmp_ret_code == -1)
	{
		return false;
	}
	tmp_ret_code = listen(m_server_listen_sock,1024);
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
	do
	{
		tmp_client_sock = accept(sock,(struct sockaddr*)&tmp_client_addr,&tmp_len);
		if(tmp_client_sock < 0)
		{
			if(errno == EINTR)
				continue;
			break;
		}
		unsigned long tmp_non_blocking = 1;
		if(ioctl(tmp_client_sock,FIONBIO,&tmp_non_blocking) < 0)
		{
			tmp_server->SafeCloseSocket(tmp_client_sock);
			tmp_client_sock = -1;
			break;
		}
		break;
	}while(-1);

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

void GateServer::TestTime(int sock,short event_flag,void *argc)
{
	GateServer * tmp_server = (GateServer*)argc;
	std::cout<<"timer wakeup!"<<std::endl;
	event_add(&(tmp_server->m_test_time_ev),&(tmp_server->m_test_time_val));
}
