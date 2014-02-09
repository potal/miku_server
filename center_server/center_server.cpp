/*
 * =====================================================================================
 *
 *       Filename:  test_server.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/23/2014 02:26:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "center_server.h"
#include <sstream>
#include <fstream>

CenterServer::CenterServer()
{

}

CenterServer::~CenterServer()
{

}

bool CenterServer::GetConfig(std::string file_name)
{
	std::ifstream tmp_conf_file(file_name.c_str());
	std::string tmp_str;
	while(std::getline(tmp_conf_file,tmp_str))
	{
		if(tmp_conf_file.eof())
			break;
		if(tmp_str.find_first_not_of(" #") == tmp_str.npos)
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
			else if(tmp_name_str == "count_worker")
				count_worker_ = tmp_value;
			else if(tmp_name_str == "user_per_worker")
				count_user_per_worker_ = tmp_value;
			else if(tmp_name_str == "read_timeout")
				read_timeout_ = tmp_value;
			else if(tmp_name_str == "write_timeout")
				write_timeout_ = tmp_value;
		}
	}
	if(server_port_ == 0)
		return false;
	return true;
}

bool CenterServer::InitServer()
{
	if(!GetConfig("server.conf"))
		return false;
	cl_processor_.InitProcessor(10,this);//max circle list buffer count
	int tmp_max_user_count = user_list_.Init(10,this);
	std::cout<<"Max user:"<<tmp_max_user_count<<std::endl;
	std::cout<<"IP:"<<server_ip_<<"  port:"<<server_port_<<std::endl;
	bool tmp_return = server_listenner_.InitServer(server_ip_,server_port_,count_worker_,count_user_per_worker_,read_timeout_,write_timeout_);
	
	if(!tmp_return)
		return false;
	//cs_connector_.InitConnectionInfo("192.168.229.128",5556);
	return true;
}

bool CenterServer::StartServer()
{
	bool tmp_return = false;
	//tmp_return = cs_connector_.ConnectServer();
	//if(!tmp_return)
	//	return false;
	//std::cout<<"Connect CenterServer OK!"<<std::endl;
	tmp_return = cl_processor_.StartProcessor(1);
	if(!tmp_return)
	{
		return false;
	}
	tmp_return = server_listenner_.StartServer(&user_list_);
	if(!tmp_return)
	{
		return false;
	}
	return true;
}

void CenterServer::StopServer()
{

}

UserInfoList *CenterServer::GetUserList()
{
	return &user_list_;
}

RoomManager *CenterServer::GetRoomManager()
{
	return &room_manager_;
}

CenterServerConnector *CenterServer::GetCSConnector()
{
	return &cs_connector_;
}

ClientProcessor *CenterServer::GetClientProcessor()
{
	return &cl_processor_;
}


