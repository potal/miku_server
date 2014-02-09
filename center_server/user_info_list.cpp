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

#include "user_info_list.h"
#include "packet/cyt_packet.pb.h"
#include "packet/package_define.pb.h"
#include "room_server.h"

UserInfoEx::UserInfoEx():room_server_(NULL)
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
	std::cout<<hash_key<<"UserInfoEx::DealWithData()"<<std::endl;
	char tmp_read_buff[0x1000] = {0};
	int tmp_len = bufferevent_read(buffev,tmp_read_buff,0x1000);
	//analyse received buffer ...
	
	StruCytPacket tmp_pack_cyt_pack;
	bool tmp_ret = tmp_pack_cyt_pack.ParseFromArray(tmp_read_buff,tmp_len);
	if(tmp_ret)
	{
		RoomServer *tmp_server = reinterpret_cast<RoomServer *>(server_ptr);
		if(!tmp_server)
		{
			std::cout<<"tmp_server NULL"<<std::endl;
			return;
		}
		tmp_ret = tmp_server->GetClientProcessor()->GetCircleList()->AddBuffer(tmp_read_buff,tmp_len);
		if(!tmp_ret)
			std::cout<<"Add buffer error!"<<std::endl;
	}
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
	room_server_ = server_ptr;
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
	tmp_user->room_server_ = room_server_;
	tmp_user->server_ptr = room_server_;
	user_list_[user_hashkey] = tmp_user;
	pthread_mutex_unlock(&list_lock_);
	return true;
}


