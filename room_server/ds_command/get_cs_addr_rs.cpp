/*
 * =====================================================================================
 *
 *       Filename:  get_cs_addr_rs.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/17/2014 08:51:20 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "get_cs_addr_rs.h"
#include "../room_server.h"
#include "../../packet/ds_server.pb.h"

GetCenterServerAddrRS::GetCenterServerAddrRS()
{
}

GetCenterServerAddrRS::GetCenterServerAddrRS(void *server_ptr)
{
	server_ptr_ = server_ptr;
}

GetCenterServerAddrRS::~GetCenterServerAddrRS()
{

}

void GetCenterServerAddrRS::Execute(char *buff ,int len,void *caller_ptr)
{
	if(!server_ptr_ || !caller_ptr)
		return ;
	
	RoomServer *tmp_server = reinterpret_cast<RoomServer *>(server_ptr_);

	SStruDsRSGetCsInfoRs tmp_get_cs_rs;
	bool tmp_return = false;
	tmp_return = tmp_get_cs_rs.ParseFromArray(buff,len);
	if(!tmp_return)
	{
		std::cout<<"Unpack SStruDsRSGetCsInfoRs Error!"<<std::endl;
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","Unpack Error");
		return ;
	}

	if(tmp_get_cs_rs.server_ip() == "" || tmp_get_cs_rs.server_port() == 0)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","Get cs addr error!");
		return ;
	}

	tmp_server->GetCSConnector()->InitConnectionInfo(tmp_get_cs_rs.server_ip().c_str(),tmp_get_cs_rs.server_port());

	tmp_return = tmp_server->GetCSConnector()->StartConnect();
	if(!tmp_return)
	{
		std::cout<<"Connect cs error!"<<std::endl;
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","Connect cs error!");
		return;
	}

	tmp_return = tmp_server->GetCSConnector()->StartCSProcessor(1000,1,server_ptr_);
	if(!tmp_return)
	{
		std::cout<<"Start cs processor error"<<std::endl;
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","Start cs processor error!");
		tmp_server->GetCSConnector()->Disconnect();
	}
}
