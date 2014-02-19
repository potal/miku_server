/*
 * =====================================================================================
 *
 *       Filename:  user_info_list.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/23/2014 11:51:13 AM
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
#include "room_manager.h"
#include "gate_server.h"
#include "user_info_list.h"

UserInfoEx::UserInfoEx():remain_buff_len_(0)
{
	recved_buff_ = new char[0x1000];
}

UserInfoEx::~UserInfoEx()
{
	if(recved_buff_)
	{
		delete []recved_buff_;
		recved_buff_ = NULL;
	}
}

void UserInfoEx::Clear()
{
	user_sock = 0;
	hash_key = 0;
	user_uip = 0;
	user_uport = 0;
	server_ptr = NULL;
}

void UserInfoEx::DealWithData(struct bufferevent *buff_ev,void *arg)
{
	char tmp_read_buff[0x1000] = {0};
	int tmp_len = bufferevent_read(buffev,tmp_read_buff,0x1000);
	std::cout<<"UserInfoEx::DealWithData() user("<<hash_key<<") recv "<<tmp_len<<" bytes"<<std::endl;
	if(remain_buff_len_+tmp_len < 0x1000)
	{
		memcpy(recved_buff_+remain_buff_len_,tmp_read_buff,tmp_len);
		remain_buff_len_ += tmp_len;
	}
	else
	{
		memset(recved_buff_,0,0x1000);
		memcpy(recved_buff_,tmp_read_buff,tmp_len);
		remain_buff_len_ = tmp_len;
	}
	//analyse received buffer ...
	std::cout<<"remain_len:"<<remain_buff_len_<<" Buff:"<<recved_buff_<<std::endl;
	bool tmp_result = false;
	StruCytPacket tmp_pack_cyt_pack;
	char tmp_pack_data[0x1000] = {0};
	char tmp_send_buff[0x2000] = {0};
	do
	{
		if(remain_buff_len_ <= 0)
			break;
		tmp_result = tmp_pack_cyt_pack.ParseFromArray(recved_buff_,remain_buff_len_);
		if(tmp_result)
		{
			int tmp_current_pack_len = tmp_pack_cyt_pack.ByteSize();
			memcpy(tmp_pack_data,recved_buff_,tmp_current_pack_len);
			remain_buff_len_ -= tmp_current_pack_len;
			memmove(recved_buff_,recved_buff_+tmp_current_pack_len,0x1000-tmp_current_pack_len);
			long tmp_room_id = tmp_pack_cyt_pack.room_id();
			std::cout<<tmp_room_id<<std::endl;
			ChatRoom *tmp_chat_room = reinterpret_cast<GateServer *>(gate_server_)->GetRoomManager()->GetChatRoom(tmp_room_id);
			if(NULL == tmp_chat_room)
			{
				std::cout<<"There is no room you find"<<std::endl;
				//here create a package of Querying a chat room 
				continue ;
			}
			GateRoomServerPack tmp_package;
			tmp_package.set_str_head("123");
			tmp_package.set_user_hashkey(hash_key);
			tmp_package.set_gs_hashkey(0);
			tmp_package.set_data_len(tmp_current_pack_len);
			tmp_package.set_data_msg(tmp_pack_data,tmp_current_pack_len);
			tmp_package.set_str_tail("456");
			tmp_current_pack_len = tmp_package.ByteSize();
			bool tmp_return = tmp_package.SerializeToArray(tmp_send_buff,tmp_current_pack_len);
			if(!tmp_return)
				continue;
			if(write(tmp_chat_room->GetSocket(),tmp_send_buff,tmp_current_pack_len) != tmp_current_pack_len)
			{
				std::cout<<"Write error!"<<std::endl;
				continue;
			}
		}
		else if(tmp_pack_cyt_pack.ParsePartialFromArray(recved_buff_,remain_buff_len_))
		{
			std::cout<<"uncompleted data"<<std::endl;//uncompleted data
			break;
		}
		else
		{
			std::cout<<"bad data"<<std::endl;
			memset(recved_buff_,0,0x1000);
			remain_buff_len_ = 0;
			break;
		}
	}while (tmp_result);
}

/////////////////////////////////////////////////

UserInfoList::UserInfoList()
{
	pthread_mutex_init(&list_lock_,NULL);
}

UserInfoList::~UserInfoList()
{
	pthread_mutex_destroy(&list_lock_);
}

BaseUserInfo *UserInfoList::GetUserInfo()
{
	pthread_mutex_lock(&list_lock_);
	UserInfoEx *tmp_user = unused_user_list_.Get();
	pthread_mutex_unlock(&list_lock_);
	return tmp_user;
}

void UserInfoList::ReleaseUserInfo(BaseUserInfo * user)
{
	pthread_mutex_lock(&list_lock_);
	UserInfoEx *tmp_user = (UserInfoEx *)user;
	if(NULL != user_list_[tmp_user->hash_key])
	{
		user_list_.erase(tmp_user->hash_key);
	}
	tmp_user->Clear();
	unused_user_list_.Put(tmp_user);
	pthread_mutex_unlock(&list_lock_);
}

BaseUserInfo *UserInfoList::GetUserByHashkey(int user_hashkey)
{
	pthread_mutex_lock(&list_lock_);
	BaseUserInfo *tmp_user = (BaseUserInfo *)user_list_[user_hashkey];
	pthread_mutex_unlock(&list_lock_);
	return tmp_user;
}

int UserInfoList::Init(int max_user,void *server_ptr)
{
	gate_server_ = server_ptr;
	return unused_user_list_.Init(max_user);
}

bool UserInfoList::AddUserInfo(int user_hashkey,BaseUserInfo *user)
{
	pthread_mutex_lock(&list_lock_);
	if(NULL != user_list_[user_hashkey])
	{
		pthread_mutex_unlock(&list_lock_);
		return false;
	}
	UserInfoEx *tmp_user = (UserInfoEx *)user;
	tmp_user->hash_key = user_hashkey;
	//copy other members
	tmp_user->gate_server_ = gate_server_;
	user_list_[user_hashkey] = tmp_user;
	pthread_mutex_unlock(&list_lock_);
	return true;
}


