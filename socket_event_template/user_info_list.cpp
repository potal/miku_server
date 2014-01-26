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

UserInfoEx::UserInfoEx()
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
	StruCytPacket tmp_pack_cyt_pack;
	tmp_pack_cyt_pack.ParseFromArray(tmp_read_buff,tmp_len);
	std::cout<<tmp_pack_cyt_pack.str_head()<<std::endl;
	std::cout<<tmp_pack_cyt_pack.room_id()<<std::endl;
	std::cout<<tmp_pack_cyt_pack.msg_len()<<std::endl;
	std::cout<<tmp_pack_cyt_pack.msg_type()<<std::endl;
	std::cout<<tmp_pack_cyt_pack.msg_data()<<std::endl;
	std::cout<<tmp_pack_cyt_pack.str_tail()<<std::endl;
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

int UserInfoList::Init(int max_user)
{
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
	user_list_[user_hashkey] = tmp_user;
	pthread_mutex_unlock(&list_lock_);
	return true;
}


