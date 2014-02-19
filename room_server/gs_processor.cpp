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
#include "../packet/cyt_packet.pb.h"
#include "../packet/package_define.pb.h"
#include "gs_processor.h"
#include "client_info_list.h"
#include <iostream>

#include "cl_command/client_login_rq.h"
#include "cl_command/cancle_user_mic_rq.h"
#include "cl_command/put_user_on_mic_rq.h"
#include "cl_command/user_apply_mic_rq.h"
#include "cl_command/user_exit_room_id.h"
#include "cl_command/user_give_gift_rq.h"
#include "cl_command/user_off_mic_rq.h"
#include "cl_command/user_send_msg_rq.h"

GateServerProcessor::GateServerProcessor():is_started_(false),thread_id_ptr_(NULL),thread_count_(0),parent_ptr_(NULL),send_thread_id_(0)
{

}

GateServerProcessor::~GateServerProcessor()
{
	for(int i = 0;i < thread_count_;i++)
	{
		pthread_join(thread_id_ptr_[i],NULL);
	}
	pthread_join(send_thread_id_,NULL);
	if(thread_id_ptr_)
	{
		delete []thread_id_ptr_;
		thread_id_ptr_ = NULL;
	}
}

bool GateServerProcessor::InitProcessor(int max_list_size,void *caller_ptr,void *server_ptr)
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
	tmp_ret = tmp_cmd_chain->RegisterCommand(E_CANCLE_USER_MIC_RQ,new CancleUserMicRQ(server_ptr));
	if(!tmp_ret)
	{
		std::cout<<"Register CancleUserMicRQ Error"<<std::endl;
		return false;
	}
	tmp_ret = tmp_cmd_chain->RegisterCommand(E_PUT_USER_ON_MIC_RQ,new PutUserOnMicRQ(server_ptr));
	if(!tmp_ret)
	{
		std::cout<<"Register PutUserOnMicRQ Error"<<std::endl;
		return false;
	}
	tmp_ret = tmp_cmd_chain->RegisterCommand(E_USER_APPLY_MIC_RQ,new UserApplyMicRQ(server_ptr));
	if(!tmp_ret)
	{
		std::cout<<"Register UserApplyMicRQ Error"<<std::endl;
		return false;
	}
	tmp_ret = tmp_cmd_chain->RegisterCommand(E_USER_EXIT_ROOM_ID,new UserExitRoomID(server_ptr));
	if(!tmp_ret)
	{
		std::cout<<"Register UserExitRoomID Error"<<std::endl;
		return false;
	}
	tmp_ret = tmp_cmd_chain->RegisterCommand(E_USER_GIVE_GIFT_RQ,new UserGiveGiftRQ(server_ptr));
	if(!tmp_ret)
	{
		std::cout<<"Register UserGiveGiftRQ Error"<<std::endl;
		return false;
	}
	tmp_ret = tmp_cmd_chain->RegisterCommand(E_USER_OFF_MIC_RQ,new UserOffMicRQ(server_ptr));
	if(!tmp_ret)
	{
		std::cout<<"Register UserOffMicRQ Error"<<std::endl;
		return false;
	}
	tmp_ret = tmp_cmd_chain->RegisterCommand(E_USER_MSG_RQ,new UserSendMsgRQ(server_ptr));
	if(!tmp_ret)
	{
		std::cout<<"Register UserSendMsgRQ Error"<<std::endl;
		return false;
	}
	return true;
}

bool GateServerProcessor::StartProcessor(int thread_count)
{
	//starting the thread of dealing with package
	if(thread_count <= 0)
		return false;
	thread_count_ = thread_count;
	is_started_ = true;
	int tmp_ret = -1;
	thread_id_ptr_ = new pthread_t[thread_count];
	int i = 0;
	for(i = 0;i < thread_count;i ++)
	{
		tmp_ret = pthread_create(&thread_id_ptr_[i],NULL,DealWithDataThread,this);
		if(tmp_ret != 0)
		{
			break;
		}
	}
	std::cout<<"Create "<<i+1<<" deal reced data thread(s) ok!"<<std::endl;
	tmp_ret = pthread_create(&send_thread_id_,NULL,SendDataThread,this);
	if(tmp_ret != 0)
	{
		std::cout<<"create send thread error!"<<std::endl;
	}
	return true;
}

void GateServerProcessor::StopProcessor()
{
	is_started_ = false;
	for(int i = 0;i < thread_count_;i++)
	{
		pthread_join(thread_id_ptr_[i],NULL);
	}
	GetCmdChain()->ReleaseAllCommand();
}

void* GateServerProcessor::DealWithDataThread(void *arg)
{
	std::cout<<"GateServerProcessor::DealWithDataThread starts"<<std::endl;
	GateServerProcessor *tmp_processor = reinterpret_cast<GateServerProcessor *>(arg);
	if(!tmp_processor)
	{
		std::cout<<"tmp_processor NULL"<<std::endl;
		return NULL;
	}
	bool tmp_return = false;
	char tmp_out_buff[0x2000] = {0};
	int tmp_out_len = 0;
	while(tmp_processor->is_started_)
	{
		tmp_out_len = 0;
		if(!tmp_processor->GetCircleList()->GetBuffer(tmp_out_buff,0x2000,tmp_out_len))
		{
			usleep(5000);
			continue;
		}
		std::cout<<"GetBuffer ok! buff_len:"<<tmp_out_len<<std::endl;
		GateRoomServerPack tmp_gr_pack;
		tmp_return = tmp_gr_pack.ParseFromArray(tmp_out_buff,tmp_out_len);
		if(!tmp_return)
		{
			std::cout<<"Unpack package error!"<<std::endl;
			continue;
		}

		StruCytPacket tmp_pack_cyt_pack;
		tmp_return = tmp_pack_cyt_pack.ParseFromArray(tmp_gr_pack.data_msg().c_str(),tmp_gr_pack.data_len());
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
		tmp_cmd->Execute(tmp_out_buff,tmp_out_len,arg);
	}
	std::cout<<"GateServerProcessor::DealWithDataThread ends"<<std::endl;
	return NULL;
}

void * GateServerProcessor::SendDataThread(void *arg)
{
	GateServerProcessor *tmp_processor = reinterpret_cast<GateServerProcessor *>(arg);
	if(!tmp_processor)
	{
		std::cout<<"Create SendDataThread Error"<<std::endl;
		return NULL;
	}
	int tmp_gs_fd = reinterpret_cast<ClientInfoEx *>(tmp_processor->parent_ptr_)->user_sock;
	bool tmp_return = false;
	char tmp_out_buff[0x2000] = {0};
	int tmp_out_len = 0;
	while(tmp_processor->is_started_)
	{
		tmp_out_len = 0;
		if(!tmp_processor->GetCircleList()->GetBuffer(tmp_out_buff,0x2000,tmp_out_len))
		{
			usleep(5000);
			continue;
		}
		write(tmp_gs_fd,tmp_out_buff,tmp_out_len);
	}

	return NULL;
}
