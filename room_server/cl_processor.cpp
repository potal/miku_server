/*
 * =====================================================================================
 *
 *       Filename:  cl_processor.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/07/2014 12:21:20 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "packet/cyt_packet.pb.h"
#include "packet/package_define.pb.h"
#include "cl_processor.h"
#include <iostream>
#include <pthread.h>

#include "cl_command/client_login_rq.h"

ClientProcessor::ClientProcessor():is_started_(false)
{

}

ClientProcessor::~ClientProcessor()
{

}

bool ClientProcessor::InitProcessor(int max_list_size,void *caller_ptr)
{
	BaseProcessor::InitProcessor(max_list_size,caller_ptr);
	CommandChain *tmp_cmd_chain = GetCmdChain();
	bool tmp_ret = false;
	tmp_ret = tmp_cmd_chain->RegisterCommand(E_USER_LOGIN_RQ,new ClientLoginRQ(caller_ptr));
	if(!tmp_ret)
	{
		std::cout<<"Register UserLoginRQ Error"<<std::endl;
		return false;
	}

	return true;
}

bool ClientProcessor::StartProcessor(int thread_count)
{
	//starting the thread of dealing with package
	is_started_ = true;
	bool tmp_ret = false;
	pthread_t tmp_thread_id = 0;
	int i = 0;
	for(i = 0;i < thread_count;i ++)
	{
		tmp_ret = pthread_create(&tmp_thread_id,NULL,DealWithDataThread,this);
		if(!tmp_ret)
		{
			break;
		}
	}
	std::cout<<"Create "<<i<<" thread(s) ok!"<<std::endl;
	return true;
}

void* ClientProcessor::DealWithDataThread(void *arg)
{
	std::cout<<"ClientProcessor::DealWithDataThread starts"<<std::endl;
	ClientProcessor *tmp_processor = reinterpret_cast<ClientProcessor *>(arg);
	if(!tmp_processor)
	{
		std::cout<<"tmp_processor NULL"<<std::endl;
		return NULL;
	}
	bool tmp_return = false;
	while(tmp_processor->is_started_)
	{
		char tmp_out_buff[0x2000] = {0};
		int tmp_out_len = 0;
		if(!tmp_processor->GetCircleList()->GetBuffer(tmp_out_buff,0x2000,tmp_out_len))
		{
			usleep(1);
			continue;
		}
		std::cout<<"GetBuffer ok! buff_len:"<<tmp_out_len<<std::endl;
		StruCytPacket tmp_pack_cyt_pack;
		tmp_return = tmp_pack_cyt_pack.ParseFromArray(tmp_out_buff,tmp_out_len);
		if(!tmp_return)
		{
			std::cout<<"Unpack package error!"<<std::endl;
			continue;
		}

		int tmp_msg_type = tmp_pack_cyt_pack.msg_type();
		std::string tmp_msg_data = tmp_pack_cyt_pack.msg_data();

		BaseCommand *tmp_cmd = tmp_processor->GetCmdChain()->GetCommand(tmp_msg_type);
		if(!tmp_cmd)
		{
			std::cout<<"Get cmd error!"<<std::endl;
			usleep(1);
			continue;
		}
		tmp_cmd->Execute(const_cast<char*>(tmp_msg_data.c_str()),tmp_pack_cyt_pack.msg_len(),arg);
	}
	std::cout<<"ClientProcessor::DealWithDataThread ends"<<std::endl;
	return NULL;
}
