/*
 * =====================================================================================
 *
 *       Filename:  connector.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/15/2014 08:32:19 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "connector.h"

Connector::Connector():server_port_(0),client_fd_(-1)
{

}

Connector::~Connector()
{
	if(-1 != client_fd_)
	{
		close(client_fd_);
		client_fd_ = -1;
	}
}

void Connector::InitConnectionInfo(const char *server_ip,const short server_port)
{
	server_ip_ = server_ip;
	server_port_ = server_port;
}

bool Connector::ConnectServer()
{
	if(server_port_ <= 0)
		return false;
	client_fd_ = socket(AF_INET,SOCK_STREAM,0);
	if(client_fd_ == -1)
		return false;
	struct sockaddr_in tmp_svr_addr;
	bzero(&tmp_svr_addr,sizeof(tmp_svr_addr));
	tmp_svr_addr.sin_family = AF_INET;
	tmp_svr_addr.sin_port = htons(server_port_);
	tmp_svr_addr.sin_addr.s_addr = inet_addr(server_ip_.c_str());
	if(connect(client_fd_,(struct sockaddr *)&tmp_svr_addr,sizeof(struct sockaddr))==-1)
	{
		return false;
	}

	return true;
}

void Connector::Disconnect()
{
	close(client_fd_);
	client_fd_ = -1;
}
