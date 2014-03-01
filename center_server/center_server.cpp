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

bool CenterServer::InitServer(std::string conf_file_name)
{
	if(!GetConfig(conf_file_name))
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"InitServer","GetConfig error");	
		return false;
	}
	int tmp_max_user_count = client_list_.Init(50,this);
	std::cout<<"Max user:"<<tmp_max_user_count<<std::endl;
	std::cout<<"IP:"<<server_ip_<<"  port:"<<server_port_<<std::endl;
	bool tmp_return = server_listenner_.InitServer(server_ip_,server_port_,count_worker_,count_user_per_worker_,read_timeout_,write_timeout_);
	
	if(!tmp_return)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"InitServer","server_listenner_.InitServer error");
		return false;
	}
	miku_db_.Init(this);

	ds_connector_.InitConnectionInfo(ds_ip_.c_str(),ds_port_);
	return true;
}

bool CenterServer::StartServer()
{
	bool tmp_return = false;
	tmp_return = ds_connector_.StartConnect();
	if(!tmp_return)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"StartServer","ds_connector_.StartConnect error");
		return false;
	}
	tmp_return = ds_connector_.StartDSProcessor(100,1,this);
	if(!tmp_return)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"StartServer","ds_connector_.StartDSProcessor error");
		ds_connector_.Disconnect();
		return false;
	}
	std::cout<<"Connect DirectorServer OK!"<<std::endl;
	WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"StartServer","Connect DS OK!");

	tmp_return = server_listenner_.StartServer(&client_list_);
	if(!tmp_return)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"StartServer","server_listenner_.StartServer error");
		return false;
	}
	std::cout<<"quit loop!"<<std::endl;
	return true;
}

void CenterServer::StopServer()
{
	ds_connector_.Disconnect();
	std::cout<<"call stop before"<<std::endl;
	server_listenner_.StopServer();
	std::cout<<"call stop after"<<std::endl;
}

ClientInfoList *CenterServer::GetClientList()
{
	return &client_list_;
}

ServerListenner *CenterServer::GetListenner()
{
	return &server_listenner_;
}

MikuDatabase *CenterServer::GetMikuDB()
{
	return &miku_db_;
}

DirectorServerConnector *CenterServer::GetDSConnector()
{
	return &ds_connector_;
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
			tmp_ss>>server_ip_;
		else if(tmp_name_str == "ds_ip")
			tmp_ss>>ds_ip_;
		else if(tmp_name_str == "db_addr")
			tmp_ss>>db_addr_;
		else if(tmp_name_str == "db_user")
			tmp_ss>>db_user_;
		else if(tmp_name_str == "db_psw")
			tmp_ss>>db_psw_;
		else if(tmp_name_str == "db_name")
			tmp_ss>>db_name_;
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
			else if(tmp_name_str == "ds_port")
				ds_port_ = tmp_value;
			else if(tmp_name_str == "server_id")
				server_id_ = tmp_value;
			else if(tmp_name_str == "db_max_conn")
				max_conn_size_ = tmp_value;
		}
	}
	if(server_port_ == 0)
		return false;
	return true;
}

