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
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <event.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "gate_server.h"

//void set_non_block()
//{
//	int flags = fcntl(listen_fd,F_GETFL,0);
//	flags |= O_NONBLOCK;
//	fcntl(listen_fd,F_SETFL,flags);
//}

//
//void on_accept()
//{
//}
//
//int main()
//{
//	struct event ev_accept;
//	event_init();
//	struct sockaddr_in listen_addr;
//	int listen_port = 5555;
//	int listen_fd = socket(AF_INET,SOCK_STREAM,0);
//	memset(&listen_addr,0,sizeof(listen_addr));
//	listen_addr.sin_family = AF_INET;
//	listen_addr.sin_addr.s_addr = INADDR_ANY;
//	listen_addr.sin_port = htons(listen_port);
//	
//	int resue_addr_on = 1;
//	setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&resue_addr_on,sizeof(resue_addr_on));
//	
//	bind(listen_fd,(struct sockaddr*)&listen_addr,sizeof(listen_addr));
//	listen(listen_fd,1024);
//
//	set_non_block();
//
//	event_set(&ev_accept,listen_fd,EV_READ|EV_PERSIST,on_accept,NULL);
//	event_add(&ev_accept,NULL);
//
//	event_dispatch();
//
//	return 0;
//}

int main()
{
	GateServer gate_server;
	gate_server.StartServer();

	gate_server.StopServer();

	return 0;
}
