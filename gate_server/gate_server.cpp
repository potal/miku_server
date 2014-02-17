/*
 * =====================================================================================
 *
 *       Filename:  gate_server.cpp
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

#include "gate_server.h"
#include <sstream>
#include <fstream>

GateServer::GateServer()
{
}

GateServer::~GateServer()
{
	std::map<int,RoomServerConnector *>::iterator tmp_iter = rs_conn_list_.begin();
	for(;tmp_iter != rs_conn_list_.end();tmp_iter++)
	{
		RoomServerConnector *tmp_rs_conn = tmp_iter->second;
		tmp_rs_conn->Disconnect();
		delete tmp_rs_conn;
	}
	rs_conn_list_.clear();
}

bool GateServer::GetConfig(std::string file_name)
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

bool GateServer::InitServer()
{
	if(!GetConfig("server.conf"))
		return false;
	int tmp_max_user_count = user_list_.Init(10,this);
	std::cout<<"Max user:"<<tmp_max_user_count<<std::endl;
	std::cout<<"IP:"<<server_ip_<<"  port:"<<server_port_<<std::endl;
	bool tmp_return = server_listenner_.InitServer(server_ip_,server_port_,count_worker_,count_user_per_worker_,read_timeout_,write_timeout_);
	
	if(!tmp_return)
		return false;
//	rs_connector_.InitConnectionInfo("192.168.220.142",5556);
	ds_connector_.InitConnectionInfo("192.168.229.128",5560);
	return true;
}

bool GateServer::StartServer()
{
	bool tmp_return = false;
//	tmp_return = rs_connector_.StartConnect();
//	if(!tmp_return)
//		return false;
//	tmp_return = rs_connector_.StartRSProcessor(1000,1,this);
//	if(tmp_return)
//		std::cout<<"Connect RoomServer OK!"<<std::endl;
//	else
//	{
//		std::cout<<"Start RS Processor error!"<<std::endl;
//		return false;
//	}
//	room_manager_.AddRoom(16000,rs_connector_.GetSocket());
	
	tmp_return = ds_connector_.StartConnect();
	if(!tmp_return)
		return false;
	tmp_return = ds_connector_.StartDSProcessor(1000,1,this);
	if(!tmp_return)
		return false;
	std::cout<<"Connect DS OK!"<<std::endl;

	tmp_return = server_listenner_.StartServer(&user_list_);
	if(!tmp_return)
		return false;
	return true;
}

UserInfoList *GateServer::GetClientInfoList()
{
	return &user_list_;
}

RoomManager *GateServer::GetRoomManager()
{
	return &room_manager_;
}

DirectorServerConnector *GateServer::GetDSConnector()
{
	return &ds_connector_;
}

std::map<int,RoomServerConnector *> *GateServer::GetRSConnectionList()
{
	return &rs_conn_list_;
}

void GateServer::StopServer()
{

}


