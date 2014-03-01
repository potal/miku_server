/*
 * =====================================================================================
 *
 *       Filename:  connect_director_server_rs.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/17/2014 05:53:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "connect_director_server_rs.h"
#include "../../packet/ds_server.pb.h"
#include "../../common/base_log.h"
#include "../gate_server.h"

ConnectDirectorServerRS::ConnectDirectorServerRS()
{
}

ConnectDirectorServerRS::ConnectDirectorServerRS(void *server_ptr)
{
	server_ptr_ = server_ptr;
}

ConnectDirectorServerRS::~ConnectDirectorServerRS()
{

}

void ConnectDirectorServerRS::Execute(char *buff,int len,void *caller_ptr)
{
	if(!server_ptr_ || !caller_ptr)
		return ;
	StruDsServerConnectRs tmp_conn_rs;
	bool tmp_return = false;
	tmp_return = tmp_conn_rs.ParseFromArray(buff,len);
	if(!tmp_return)
	{
		std::cout<<"Unpack StruDsServerConnectRs Error!"<<std::endl;
		return;
	}
	std::cout<<"ds connect result:"<<tmp_conn_rs.result()<<std::endl;
	if(tmp_conn_rs.result() == 0)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","DS Verify GS error!");
		GateServer *tmp_server = reinterpret_cast<GateServer *>(server_ptr_);
		tmp_server->StopServer();
		return;
	}
	WRITEFORMATINFOLOG(__THREADID__,__FILE__,__LINE__,"Execute","DS Verify GS successfully!");
}

