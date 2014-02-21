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

#include "client_info_list.h"
#include "../packet/cyt_packet.pb.h"
#include "../packet/package_define.pb.h"

ClientInfoEx::ClientInfoEx():room_server_(NULL),remain_buff_len_(0)
{
	recved_buff_ = new char[0x1000];
}

ClientInfoEx::~ClientInfoEx()
{
	if(recved_buff_)
	{
		delete []recved_buff_;
		recved_buff_ = NULL;
	}
}

void ClientInfoEx::Clear()
{
	user_sock = 0;
	hash_key = 0;
	user_uip = 0;
	user_uport = 0;
	server_ptr = NULL;
}

void ClientInfoEx::DealWithData(struct bufferevent *buff_ev,void *arg)
{
	char tmp_read_buff[0x1000] = {0};
	int tmp_len = bufferevent_read(buffev,tmp_read_buff,0x1000);
	std::cout<<hash_key<<" ClientInfoEx::DealWithData() Len:"<<tmp_len<<std::endl;
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
	bool tmp_result = false;
	std::cout<<"ClientInfoEx: remain_len="<<remain_buff_len_<<std::endl;
	do
	{
		if(remain_buff_len_ <= 0)
		{
			std::cout<<"remain_buff_len == 0"<<std::endl;
			break;
		}

		GateRoomServerPack tmp_package;
		tmp_result = tmp_package.ParseFromArray(recved_buff_,remain_buff_len_);
		if(tmp_result)
		{
			int tmp_current_pack_len = tmp_package.ByteSize();
			remain_buff_len_ -= tmp_current_pack_len;
			memmove(recved_buff_,recved_buff_+tmp_current_pack_len,0x1000-tmp_current_pack_len);

			tmp_package.set_gs_hashkey(hash_key);
			tmp_current_pack_len = tmp_package.ByteSize();
			tmp_result = tmp_package.SerializeToArray(tmp_read_buff,tmp_current_pack_len);
			if(!tmp_result)
			{
				std::cout<<"ClientInfoEx::DealWithData SerializeToArray Error!"<<std::endl;
				continue;
			}
			bool tmp_ret = gs_processor_.GetCircleList()->AddBuffer(tmp_read_buff,tmp_current_pack_len);
			if(!tmp_ret)
			{
				std::cout<<"ClientInfoEx::DealWithData Add buffer error!"<<std::endl;
			}
			else
			{
				std::cout<<"ClientInfoEx::DealwithData addbuffer ok!"<<std::endl;
			}
		}
		else if(tmp_package.ParsePartialFromArray(recved_buff_,remain_buff_len_))
		{
			std::cout<<"ClientInfoEx::DealWithData not the whole package"<<std::endl;
			break;
		}
		else
		{
			//bad data
			//memset(recved_buff_,0,0x1000);
			//remain_buff_len_ = 0;
			std::cout<<"ClientInfoEx::DealWithData bad data!"<<std::endl;
			break;
		}
	}while (tmp_result);
}

/////////////////////////////////////////////////

ClientInfoList::ClientInfoList()
{
	pthread_mutex_init(&list_lock_,NULL);
}

ClientInfoList::~ClientInfoList()
{
	pthread_mutex_destroy(&list_lock_);
}

BaseUserInfo *ClientInfoList::GetUserInfo()
{
	pthread_mutex_lock(&list_lock_);
	ClientInfoEx *tmp_user = unused_user_list_.Get();
	pthread_mutex_unlock(&list_lock_);
	return tmp_user;
}

void ClientInfoList::ReleaseUserInfo(BaseUserInfo * user)
{
	pthread_mutex_lock(&list_lock_);
	ClientInfoEx *tmp_user = (ClientInfoEx *)user;
	if(NULL != user_list_[tmp_user->hash_key])
	{
		user_list_.erase(tmp_user->hash_key);
	}
	tmp_user->gs_processor_.StopProcessor();
	tmp_user->Clear();
	unused_user_list_.Put(tmp_user);
	pthread_mutex_unlock(&list_lock_);
}

BaseUserInfo *ClientInfoList::GetUserByHashkey(int user_hashkey)
{
	pthread_mutex_lock(&list_lock_);
	BaseUserInfo *tmp_user = (BaseUserInfo *)user_list_[user_hashkey];
	pthread_mutex_unlock(&list_lock_);
	return tmp_user;
}

int ClientInfoList::Init(int max_user,void *server_ptr)
{
	room_server_ = server_ptr;
	return unused_user_list_.Init(max_user);
}

bool ClientInfoList::AddUserInfo(int user_hashkey,BaseUserInfo *user)
{
	pthread_mutex_lock(&list_lock_);
	if(NULL != user_list_[user_hashkey])
	{
		pthread_mutex_unlock(&list_lock_);
		return false;
	}
	ClientInfoEx *tmp_user = (ClientInfoEx *)user;
	tmp_user->hash_key = user_hashkey;
	//copy other members
	tmp_user->room_server_ = room_server_;
	tmp_user->gs_processor_.InitProcessor(1000,tmp_user,room_server_);
	tmp_user->gs_processor_.StartProcessor(1);
	user_list_[user_hashkey] = tmp_user;
	pthread_mutex_unlock(&list_lock_);
	return true;
}


