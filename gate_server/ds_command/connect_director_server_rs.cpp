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
#include "../packet/ds_server.pb.h"

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

}

