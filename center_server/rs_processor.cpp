/*
 * =====================================================================================
 *
 *       Filename:  rs_processor.cpp
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
#include "../packet/cyt_packet.pb.h"
#include "../packet/package_define.pb.h"
#include "rs_processor.h"
#include <iostream>

#include "rs_command/client_login_rq.h"

RoomServerProcessor::RoomServerProcessor():is_started_(false),thread_count_(0),thread_id_ptr_(NULL),parent_ptr_(NULL)
{

}

RoomServerProcessor::~RoomServerProcessor()
{

}

bool RoomServerProcessor::InitProcessor(int max_list_size,void *caller_ptr,void *server_ptr)
{
	BaseProcessor::InitProcessor(max_list_size,server_ptr);
	parent_ptr_ = caller_ptr;
	CommandChain *tmp_cmd_chain = GetCmdChain();
	bool tmp_ret = false;
	tmp_ret = tmp_cmd_chain->RegisterCommand(E_USER_LOGIN_RQ,new ClientLoginRQ(server_ptr));
	if(!tmp_ret)
	{
		std::cout<<"Register UserLoginRQ Error"<<std::endl;
		return false;
	}

	return true;
}

bool RoomServerProcessor::StartProcessor(int thread_count)
{
	//starting the thread of dealing with package
	if(thread_count <= 0)
		return false;
	thread_count_ = thread_count;
	is_started_ = true;
	bool tmp_ret = false;
	thread_id_ptr_ = new pthread_t[thread_count];
	int i = 0;
	for(i = 0;i < thread_count;i ++)
	{
		tmp_ret = pthread_create(&thread_id_ptr_[i],NULL,DealWithDataThread,this);
		if(!tmp_ret)
		{
			break;
		}
	}
	std::cout<<"Create "<<i+1<<" thread(s) ok!"<<std::endl;
	return true;
}

void RoomServerProcessor::StopProcessor()
{
	is_started_ = false;
	for(int i = 0;i < thread_count_;i++)
	{
		pthread_join(thread_id_ptr_[i],NULL);
	}
	GetCmdChain()->ReleaseAllCommand();
}

void *RoomServerProcessor::GetParent()
{
	return parent_ptr_;
}

void* RoomServerProcessor::DealWithDataThread(void *arg)
{
	std::cout<<"RoomServerProcessor::DealWithDataThread starts"<<std::endl;
	RoomServerProcessor *tmp_processor = reinterpret_cast<RoomServerProcessor *>(arg);
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
			usleep(5000);
			continue;
		}
		std::cout<<"GetBuffer ok! buff_len:"<<tmp_out_len<<std::endl;
		std::cout<<"Buff:"<<tmp_out_buff<<std::endl;

		StruCytPacket tmp_pack_cyt_pack;
		tmp_return = tmp_pack_cyt_pack.ParseFromArray(tmp_out_buff,tmp_out_len);
		if(!tmp_return)
		{
			std::cout<<"Unpack package error!"<<std::endl;
			continue;
		}

		int tmp_msg_type = tmp_pack_cyt_pack.msg_type();

		BaseCommand *tmp_cmd = tmp_processor->GetCmdChain()->GetCommand(tmp_msg_type);
		if(!tmp_cmd)
		{
			std::cout<<"Get cmd error!"<<std::endl;
			usleep(5000);
			continue;
		}
		tmp_cmd->Execute(const_cast<char *>(tmp_pack_cyt_pack.msg_data().c_str()),tmp_pack_cyt_pack.msg_len(),tmp_processor);
	}
	std::cout<<"RoomServerProcessor::DealWithDataThread ends"<<std::endl;
	return NULL;
}
