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

Connector::Connector():server_port_(0),client_fd_(-1),is_started_(false)
{

}

Connector::~Connector()
{
	is_started_ = false;
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
	send_list_.InitBuffer(1000);
}

bool Connector::StartConnect()
{
	is_started_ = true;
	bool tmp_return = false;
	int tmp_create_ret = -1;
	tmp_return = ConnectServer();
	if(!tmp_return)
	{
		is_started_ = false;
		return false;
	}
	pthread_t tmp_thread_id = 0;
	tmp_create_ret = pthread_create(&tmp_thread_id,NULL,ReconnectThread,this);
	if(tmp_create_ret != 0)
	{
		is_started_ = false;
		return false;
	}
	tmp_create_ret = pthread_create(&tmp_thread_id,NULL,RecvDataThread,this);
	if(tmp_create_ret != 0)
	{
		is_started_ = false;
		return false;
	}
	tmp_create_ret = pthread_create(&tmp_thread_id,NULL,SendDataThread,this);
	if(tmp_create_ret != 0)
	{
		is_started_ = false;
		return false;
	}
	return true;
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
	//server_ip_ = "";//test
	tmp_svr_addr.sin_addr.s_addr = (server_ip_ == "" ? INADDR_ANY : inet_addr(server_ip_.c_str()));
	while(true)
	{
		if(connect(client_fd_,(struct sockaddr *)&tmp_svr_addr,sizeof(struct sockaddr))==-1)
		{
			sleep(3);
			continue;
		}
		else
		{
			int tmp_flags = fcntl(client_fd_,F_GETFL,0);
			tmp_flags |= O_NONBLOCK;
			fcntl(client_fd_,F_SETFL,tmp_flags);
			break;
		}
	}
	return true;
}

void Connector::Disconnect()
{
	is_started_ = false;
	close(client_fd_);
	client_fd_ = -1;
}

bool Connector::SendData(char *buff,int len)
{
	return send_list_.AddBuffer(buff,len);
}

int Connector::GetSocket()
{
	return client_fd_;
}

void *Connector::SendDataThread(void *arg)
{
	Connector *tmp_connector = reinterpret_cast<Connector *>(arg);
	if(!tmp_connector)
		return NULL;
	char tmp_out_buff[0x2000];
	int tmp_out_len = 0;
	bool tmp_get_ret = false;
	int tmp_send_ret = -1;
	while(tmp_connector->is_started_)
	{
		memset(tmp_out_buff,0,sizeof(tmp_out_buff));
		tmp_out_len = 0;
		if(tmp_connector->client_fd_ == -1)
		{
			usleep(5000);
			continue;
		}
		tmp_get_ret = tmp_connector->send_list_.GetBuffer(tmp_out_buff,sizeof(tmp_out_buff),tmp_out_len);
		if(tmp_get_ret)
		{
			tmp_send_ret = write(tmp_connector->client_fd_,tmp_out_buff,tmp_out_len);
			if(tmp_send_ret < 0)
			{
				close(tmp_connector->client_fd_);
				tmp_connector->client_fd_ = -1;
			}
		}
		else
		{
			usleep(5000);
			continue;
		}
	}
	return NULL;
}

// how about bufferevent?
void *Connector::RecvDataThread(void *arg)
{
	Connector *tmp_connector = reinterpret_cast<Connector *>(arg);
	if(!tmp_connector)
		return NULL;
	char tmp_read_buff[0x2000];
	int tmp_read_len = 0;
	while(tmp_connector->is_started_)
	{
		memset(tmp_read_buff,0,sizeof(tmp_read_buff));
		tmp_read_len = 0;
		if(tmp_connector->client_fd_ == -1)
		{
			usleep(5000);
			continue;
		}
		tmp_read_len = read(tmp_connector->client_fd_,tmp_read_buff,sizeof(tmp_read_buff));
		if(tmp_read_len > 0)
		{
			tmp_connector->DealWithData(tmp_read_buff,tmp_read_len,tmp_connector->client_fd_);
		}
		else
		{
			usleep(5000);
		}
	}
	return NULL;
}

void *Connector::ReconnectThread(void *arg)
{
	Connector *tmp_connector = reinterpret_cast<Connector *>(arg);
	while(tmp_connector->is_started_)
	{
		if(tmp_connector->client_fd_ != -1)
		{
			sleep(3);
			continue;
		}
		tmp_connector->ConnectServer();
	}
	return NULL;
}
