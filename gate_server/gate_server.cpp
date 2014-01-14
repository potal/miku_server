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

GateServer::GateServer()
{
}

GateServer::~GateServer()
{
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
		if(tmp_name_str == "ip"){
			tmp_ss>>m_server_ip;
		}else{
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

	if(m_server_port != 0)
	{
		return false;
	}
	return true;
}

bool GateServer::StartServer()
{
	if(!InitServer())
		return false;
	m_base = event_init();
	return true;
}

void GateServer::SetNonblock()
{
	int tmp_flags = fcntl(m_server_listen_sock,F_GETFL,0);
	tmp_flags |= O_NONBLOCK;
	fcntl(m_server_listen_sock,F_SETFL,flags);
}

bool Gate::OpenServerSocket()
{
	m_server_listen_sock = socket(AF_INET,SOCK_STREAM,NULL);
	if(m_server_listen_sock == -1)
		return false;
	struct sockaddr_in tmp_server_addr;
	memset(&tmp_server_addr,0,sizeof(tmp_server_addr));
	tmp_server_addr.sin_family = AF_INET;
	tmp_server_addr.sin_port = htons(m_server_port);
	tmp_server_addr.sin_addr.s_addr = inet_atoi(m_server_ip);
	return true;
}

void GateServer::StopServer()
{

}
