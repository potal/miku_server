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

UserInfoEx::UserInfoEx()
{
	
}

UserInfoEx::~UserInfoEx()
{
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
	std::cout<<hash_key<<":DealWithData"<<std::endl;
	char tmp_read_buff[0x1000] = {0};
	int tmp_len = bufferevent_read(buffev,tmp_read_buff,0x1000);
	std::cout<<"Read:"<<tmp_read_buff<<std::endl;
}

/////////////////////////////////////////////////

UserInfoList::UserInfoList()
{

}

UserInfoList::~UserInfoList()
{

}

BaseUserInfo *UserInfoList::GetUserInfo()
{
	UserInfoEx *tmp_user = unused_user_list_.Get();
	return tmp_user;
}

void UserInfoList::ReleaseUserInfo(BaseUserInfo * user)
{
	UserInfoEx *tmp_user = (UserInfoEx *)user;
	if(NULL != user_list_[tmp_user->hash_key])
	{
		user_list_.erase(tmp_user->hash_key);
	}
	tmp_user->Clear();
	unused_user_list_.Put(tmp_user);
}

BaseUserInfo *UserInfoList::GetUserByHashkey(int user_hashkey)
{
	return (BaseUserInfo *)user_list_[user_hashkey];
}

int UserInfoList::Init(int max_user)
{
	return unused_user_list_.Init(max_user);
}

bool UserInfoList::AddUserInfo(int user_hashkey,BaseUserInfo *user)
{
	if(NULL != user_list_[user_hashkey])
		return false;
	UserInfoEx *tmp_user = (UserInfoEx *)user;
	tmp_user->hash_key = user_hashkey;
	//copy other members
	user_list_[user_hashkey] = tmp_user;
	return true;
}


