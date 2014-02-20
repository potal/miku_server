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
#include "../room_server.h"
#include "../../packet/ds_server.pb.h"

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
	std::cout<<"rs connect result:"<<tmp_conn_rs.result()<<std::endl;
	if(tmp_conn_rs.result() == 1)
	{
		RoomServer *tmp_server = reinterpret_cast<RoomServer *>(server_ptr_);
		if(!tmp_server)
			return;
		StruDsRSGetCsInfoRq tmp_get_cs_rq;
		tmp_get_cs_rq.set_server_id(tmp_server->server_id_);
		
		DirectorServerConnector *tmp_ds_conn = tmp_server->GetDSConnector();
		if(!tmp_ds_conn)
			return;
		tmp_ds_conn->SendPackage(tmp_get_cs_rq,DEF_DS_RS_GET_CS_RQ);
	}
}

