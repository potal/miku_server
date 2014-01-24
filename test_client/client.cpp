/*
 * =====================================================================================
 *
 *       Filename:  client.cpp
 *
 *    Description:  Test Client
 *
 *        Version:  1.0
 *        Created:  2014年01月07日 12时34分24秒
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
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>
#include <strings.h>
#include <netinet/in.h>

#define PORT 5555
#define MAXDATASIZE 5000
int main(int argc,char **argv)
{
	int sockfd,nbytes;
	char buf[1024];
	struct hostent *he;
	struct sockaddr_in srvaddr;
	if(argc!=2)
	{
	perror("Usage:client hostname\n");
	exit(1);
	}
	/*函数gethostbyname获得指定域名地址所对应的ip地址*/
	if((he=gethostbyname(argv[1]))==NULL)
	{
	perror("gethostbyname");
	exit(1);
	}
	/*创建套接字，返回套接字描述符*/
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
	perror("create socket error");
	exit(1);
	}
	bzero(&srvaddr,sizeof(srvaddr));
	/*用获得的远程服务器进程的ip地址和端口号来填充一个internet套接字地址结构*/
	srvaddr.sin_family=AF_INET;
	srvaddr.sin_port=htons(PORT);
	srvaddr.sin_addr=*((struct in_addr *)he->h_addr);
	/*用connect于这个远程服务器建立一个internet连接*/
	if(connect(sockfd,(struct sockaddr *)&srvaddr,sizeof(struct sockaddr))==-1)
	{
	perror("connect error");
	exit(1);
	}
	const char * send_buff = "send buff!";
	if(write(sockfd,send_buff,strlen(send_buff)) > 0)
	{
	std::cout<<"write ok"<<std::endl;
	}
	const char *tmp_send_buff = "asfdasdfasdfas";
	if(write(sockfd,tmp_send_buff,strlen(tmp_send_buff)) > 0)
	{
	std::cout<<"write 1 ok"<<std::endl;
	}
	std::cin.get();
	/*调用read函数读取服务器write过来的信息*/
	//  if((nbytes=read(sockfd,buf,MAXDATASIZE))==-1)
	//  {
	//    perror("read error");
	//    exit(1);
	//  }
	//  buf[nbytes]='\0';
	//  printf("read: %s\n",buf);
	close(sockfd);
	return 0;
}

