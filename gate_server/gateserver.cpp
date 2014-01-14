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

#include "gateserver.h"

GateServer::GateServer()
{
}

GateServer::~GateServer()
{
}

bool GateServer::InitServer()
{
	bool return_code = false;
	FILE * conf_fp;	
	conf_fp = fopen("init.conf","r");
	if(NULL == conf_fp)
	{
		std::cout<<"open configuration file failed!"<<std::endl;
		return false;
	}
	char line[0x200] = {0};
	char name[0x20] = {0};
	char value[0x20] = {0};

	while(fgets(line,0x200,conf_fp))
	{
		if(line[0] == '#' || strcmp(line,"") == 0)
			continue;
		memset(name,0,sizeof(name));
		memset(value,0,sizeof(value));
		sscanf(line,"%s %s",name,value);
		if(strcmp(name,"port") == 0){
			m_conf.m_server_port = atoi(value);
		}else if(strcmp(name,"ip") == 0){
			memcpy(m_conf.m_server_ip,value,strlen(value))
		}else if(strcmp(name,"worker_num") == 0){
			m_conf.m_count_worker_thread = atoi(value);
		}else if(strcmp(name,"time_out") == 0){
			m_conf.m_timeout = atoi(value);
		}else if(strcmp(name,"conn_num") == 0){
			m_conf.m_conn_num = atoi(value);
		}else if(strcmp(name,"count_user") == 0){
			m_conf.m_count_user = atoi(value);
		}else if(strcmp(name,"file_num") == 0){
			m_conf.m_file_num = atoi(value);
		}else 
			continue;
	}
	if(m_conf.m_server_port != 0)
	{
		return_code = true;
	}
	fclose(conf_fp);
	return return_code;
}

bool GateServer::StartServer()
{
	if(!InitServer())
		return false;
	return true;
}

void GateServer::StopServer()
{

}
