/*
 * =====================================================================================
 *
 *       Filename:  cl_processor.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/20/2014 02:59:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "cl_processor.h"
#include "gate_server.h"

ClientProcessor::ClientProcessor():gate_server_(NULL),deal_thread_id_(0),is_started_(false)
{
	room_list_.clear();
	pthread_mutex_init(&list_lock_,NULL);
}
ClientProcessor::~ClientProcessor()
{
	room_list_.clear();
	pthread_mutex_destroy(&list_lock_);
}

bool ClientProcessor::InitProcessor(int max_list_size,void *caller_ptr,void *server_ptr)
{
	BaseProcessor::InitProcessor(max_list_size,server_ptr);
	gate_server_ = caller_ptr;
	return true;
}

bool ClientProcessor::StartProcessor()
{
	is_started_ = true;
	int tmp_ret = pthread_create(&deal_thread_id_,NULL,DealWithDataThread,this);
	if(tmp_ret != 0)
	{
		is_started_ = false;
		return false;
	}
	return true;
}

void ClientProcessor::StopProcessor()
{
	is_started_ = false;
	pthread_join(deal_thread_id_,NULL);
}

bool ClientProcessor::AddOneUndealPack(char *buff,int len,int room_id,int user_hashkey)
{
	AutoLock tmp_lock(&list_lock_);
	if(!room_list_[room_id])
		room_list_[room_id] = room_id;
	char tmp_in_buff[0x2000] = {0};
	if(len < 0x2000-4)
	{
		memcpy(tmp_in_buff,buff,len);
		memcpy(tmp_in_buff+len,&user_hashkey,4);
		GetCircleList()->AddBuffer(tmp_in_buff,len+4);
	}
	return true;
}

void ClientProcessor::DeleteRoom(int room_id)
{
	AutoLock tmp_lock(&list_lock_);
	room_list_.erase(room_id);
}

void *ClientProcessor::DealWithDataThread(void *arg)
{
	if(!arg)
		return NULL;
	ClientProcessor *tmp_processor = reinterpret_cast<ClientProcessor *>(arg);
	GateServer *tmp_server = reinterpret_cast<GateServer *>(tmp_processor->gate_server_);
	if(!tmp_server)
		return NULL;
	char tmp_out_buff[0x2000] = {0};
	int tmp_out_len = 0;
	bool tmp_return = false;
	while(tmp_processor->is_started_)
	{
		if(!tmp_processor->GetCircleList()->GetBuffer(tmp_out_buff,0x2000,tmp_out_len))
		{
			usleep(5000);
			continue;
		}
		int tmp_user_hashkey = 0;
		memcpy(&tmp_user_hashkey,tmp_out_buff+tmp_out_len-4,4);
		StruCytPacket tmp_cyt_pack;
		tmp_return = tmp_cyt_pack.ParseFromArray(tmp_out_buff,tmp_out_len-4);
		if(!tmp_return)
		{
			std::cout<<"Unpack package error!"<<std::endl;
			continue;
		}
		int tmp_room_id = tmp_cyt_pack.room_id();
		bool tmp_is_deal = false;
		pthread_mutex_lock(&(tmp_processor->list_lock_));
		if(tmp_processor->room_list_[tmp_room_id])
			tmp_is_deal = false;
		else
			tmp_is_deal = true;
		pthread_mutex_unlock(&(tmp_processor->list_lock_));
		if(!tmp_is_deal)
		{
			tmp_processor->GetCircleList()->AddBuffer(tmp_out_buff,tmp_out_len);
			continue;
		}
		ChatRoom *tmp_room = tmp_server->GetRoomManager()->GetChatRoom(tmp_room_id);
		if(tmp_room)
		{
			GateRoomServerPack tmp_package;
			tmp_package.set_str_head("123");
			tmp_package.set_user_hashkey(tmp_user_hashkey);
			tmp_package.set_gs_hashkey(0);
			tmp_package.set_data_len(tmp_out_len-4);
			tmp_package.set_data_msg(tmp_out_buff,tmp_out_len-4);
			tmp_package.set_str_tail("456");

			char tmp_send_buff[0x2000] = {0};
			int tmp_pack_len = tmp_package.ByteSize();
			tmp_return = tmp_package.SerializeToArray(tmp_send_buff,tmp_pack_len);
			if(!tmp_return)
				continue;
			if(write(tmp_room->GetSocket(),tmp_send_buff,tmp_pack_len) != tmp_pack_len)
			{
				std::cout<<"Write error!"<<std::endl;
				continue;
			}
		}
	}
}
