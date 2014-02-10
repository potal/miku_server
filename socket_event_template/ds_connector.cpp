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

DirectorServerConnector::DirectorServerConnector()
{
	recv_list_.InitBuffer(1000);
}

DirectorServerConnector::~DirectorServerConnector()
{
	recv_list_.ReleaseBuffer();
}

void DirectorServerConnector::DealWithData(char *buff,int len,int fd)
{

}
