/*
 * =====================================================================================
 *
 *       Filename:  ds_connector.cpp
 *
 *    Description:  director server connector
 *
 *        Version:  1.0
 *        Created:  02/10/2014 06:16:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (potal), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ds_connector.h"

DirectorServerConnector::DirectorServerConnector():server_ptr_(NULL)
{
	recv_list_.InitBuffer(1000);
}

DirectorServerConnector::~DirectorServerConnector()
{
	recv_list_.ReleaseBuffer();
	ds_processor_.StopProcessor();
}

bool DirectorServerConnector::StartDSProcessor(int circle_list_size,int thread_count,void *server_ptr)
{
	server_ptr_ = server_ptr;
	bool tmp_return = false;
	tmp_return = ds_processor_.InitProcessor(circle_list_size,this,server_ptr);
	if(!tmp_return)
		return false;
	tmp_return = ds_processor_.StartProcessor(thread_count);
	if(!tmp_return)
		return false;
	return true;
}

void DirectorServerConnector::DealWithData(char *buff,int len,int fd)
{
	if(ds_processor_.GetStatus())
	{
		ds_processor_.GetCircleList()->AddBuffer(buff,len);
	}
}
