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
#include <string>
#include <time.h>
#include "../packet/package_define.pb.h"
#include "../packet/cyt_packet.pb.h"

std::string g_pack_header = "123";
std::string g_pack_tail = "456";

#define PORT 1501
#define MAXDATASIZE 5000
int main(int argc,char **argv)
{
	int sockfd,nbytes;
	struct hostent *he;
	struct sockaddr_in srvaddr;

	/*函数gethostbyname获得指定域名地址所对应的ip地址*/
	if((he=gethostbyname("localhost"))==NULL)
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
	StruUserLoginRQ tmp_user_login;
	tmp_user_login.set_msg_id(E_USER_LOGIN_RQ);
	tmp_user_login.set_user_id(92002);
	tmp_user_login.set_room_id(16000);
	tmp_user_login.set_user_psw("e10adc3949ba59abbe56e057f20f883e");
	tmp_user_login.set_user_account_name("Test");

	int tmp_pack_size = tmp_user_login.ByteSize();
	char tmp_buff[0x1000];
	tmp_user_login.SerializeToArray(tmp_buff,tmp_pack_size);

	StruCytPacket tmp_packet;
	tmp_packet.set_str_head(const_cast<char *>(g_pack_header.c_str()));
	tmp_packet.set_room_id(16000);
	tmp_packet.set_msg_len(tmp_pack_size);
	tmp_packet.set_msg_type(E_USER_LOGIN_RQ);
	tmp_packet.set_msg_data(tmp_buff,tmp_pack_size);
	tmp_packet.set_str_tail(const_cast<char *>(g_pack_tail.c_str()));

	char tmp_send_buff[0x1000] = {0};
	int tmp_send_buff_len = tmp_packet.ByteSize();
	tmp_packet.SerializeToArray(tmp_send_buff,tmp_send_buff_len);

//	StruCytPacket tmp_packet1;
//	tmp_packet1.set_str_head(const_cast<char *>(g_pack_header.c_str()));
//	tmp_packet1.set_room_id(16001);
//	tmp_packet1.set_msg_len(tmp_byte_size);
//	tmp_packet1.set_msg_type(E_USER_LOGIN_RQ);
//	tmp_packet1.set_msg_data(tmp_send_buff,tmp_byte_size);
//	tmp_packet1.set_str_tail(const_cast<char *>(g_pack_tail.c_str()));
//
//	tmp_packet1.SerializeToArray(tmp_buff+tmp_send_buff_len,tmp_packet1.ByteSize());
//	int tmp_all_len = tmp_send_buff_len+tmp_packet1.ByteSize();
//	std::cout<<"tmp_all_len:"<<tmp_all_len<<std::endl;
//
//	char *tmp_buff_start = tmp_buff;
//	int tmp_remain_len = tmp_all_len;
//	do
//	{
//		StruCytPacket tmp_unpack;
//		if(tmp_unpack.ParseFromArray(tmp_buff_start,20))
//		{
//			std::cout<<tmp_unpack.room_id()<<std::endl;
//			tmp_remain_len = tmp_remain_len-tmp_unpack.ByteSize();
//			tmp_buff_start = tmp_buff_start+tmp_unpack.ByteSize();
//		}
//		else
//		{
//			std::cout<<"parse error!"<<std::endl;
//		}
//	}while(tmp_remain_len>0);

///	std::cin.get();
///	close(sockfd);
///	return 0;
///	

//	std::cout<<"Total length:"<<tmp_send_buff_len<<std::endl;

//	if(write(sockfd,send_buff,strlen(send_buff)) > 0)
//	{
//		std::cout<<"write ok1"<<std::endl;
//	}
//	int tmp_first_send_len = 10;

//	if(write(sockfd,tmp_buff,tmp_send_buff_len) > 0)
//	{
//		std::cout<<"1write ok!length:"<<tmp_send_buff_len<<std::endl;
//	}

	int tmp_real_send_len = write(sockfd,tmp_send_buff,tmp_send_buff_len);
	if(tmp_real_send_len > 0)
	{
		std::cout<<"write ok!length:"<<tmp_real_send_len<<std::endl;
	}
	time_t tmp_current_time;
	time(&tmp_current_time);
	std::cout<<"Send OK Time:"<<tmp_current_time<<std::endl;

	char tmp_read_buff[0x100];
	int tmp_read_len = 0;
	tmp_read_len = read(sockfd,tmp_read_buff,0x100);
	if(tmp_read_len > 0)
	{
		std::cout<<"Read Len:"<<tmp_read_len<<std::endl;
		std::cout<<"Read Buff:"<<tmp_read_buff<<std::endl;
		time(&tmp_current_time);
		std::cout<<"Read Time:"<<tmp_current_time<<std::endl;
	}
	else
		std::cout<<"Read error!"<<std::endl;

	std::cin.get();

	StruUserExitRoomID tmp_user_exit;
	tmp_user_exit.set_msg_id(E_USER_EXIT_ROOM_ID);
	tmp_user_exit.set_user_id(92002);
	tmp_user_exit.set_room_id(16000);

	tmp_pack_size = tmp_user_exit.ByteSize();
	tmp_user_exit.SerializeToArray(tmp_buff,tmp_pack_size);

	tmp_packet.set_str_head(const_cast<char *>(g_pack_header.c_str()));
	tmp_packet.set_room_id(16000);
	tmp_packet.set_msg_len(tmp_pack_size);
	tmp_packet.set_msg_type(E_USER_EXIT_ROOM_ID);
	tmp_packet.set_msg_data(tmp_buff,tmp_pack_size);
	tmp_packet.set_str_tail(const_cast<char *>(g_pack_tail.c_str()));

	tmp_send_buff_len = tmp_packet.ByteSize();
	tmp_packet.SerializeToArray(tmp_send_buff,tmp_send_buff_len);

	write(sockfd,tmp_send_buff,tmp_send_buff_len);
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


