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
#include "../packet/ds_server.pb.h"

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
	SStruDsRSGetCsInfoRs tmp_get_cs_rs;
	bool tmp_return = false;
	tmp_return = tmp_get_cs_rs.ParseFromArray(buff,len);
	if(!tmp_return)
	{
		std::cout<<"Unpack SStruDsRSGetCsInfoRs Error!"<<std::endl;
		return ;
	}
}
