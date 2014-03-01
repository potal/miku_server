/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年01月07日 14时38分25秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#include "room_server.h"
#include "../common/base_log.h"
#include "../common/munit.h"

#include <iostream>
#include <string>
#include <sstream>

int main()
{
	char tmp_path[256]="";
	GetProgramDir(tmp_path,sizeof(tmp_path)-1);
	//初始化
	char tmp_log_path[256]="";

#ifdef _WIN32
	sprintf(tmp_log_path,"%s\\log",tmp_path);
#else
	sprintf(tmp_log_path,"%s/log",tmp_path);
	std::stringstream tmp_ss;
	tmp_ss<<tmp_path<<"/server.conf";
	std::string tmp_conf_file_name = tmp_ss.str();
#endif

	CBaseLog::GetInstance()->Init(tmp_log_path,NULL);
	
	RoomServer tmp_server;
	if(tmp_server.InitServer(tmp_conf_file_name))
		tmp_server.StartServer();
	std::cout<<"server quit"<<std::endl;
	return 0;
}
