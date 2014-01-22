/*
 * =====================================================================================
 *
 *       Filename:  user_list.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/15/2014 08:06:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "user_list.h"

UserInfoList::UserInfoList()
{
	pthread_mutex_init(&list_lock_,NULL);
	pthread_cond_init(&list_cond_,NULL);
}

UserInfoList::~UserInfoList()
{
	DeleteAllUser();
	pthread_mutex_destroy(&list_lock_);
	pthread_cond_destroy(&list_cond_);
}

int UserInfoList::Init(int num,struct event_base * base)
{
	int count_num = num;
	try{
		while(true)
		{
			UserInfo * tmp_user = new UserInfo;
			if(tmp_user)
			{
				tmp_user->buffev = bufferevent_new(-1,NULL,NULL,NULL,NULL);
				bufferevent_base_set(base,tmp_user->buffev);
				bufferevent_settimeout(tmp_user->buffev,0,0);
				unused_user_list_.push_back(tmp_user);
				count_num--;
			}
			if(count_num <= 0)
				break;
		}
	}
	catch(...)
	{
		return unused_user_list_.size();
	}
	return unused_user_list_.size();
}

bool UserInfoList::AddUserInfo(int user_id,UserInfo & user)
{
	pthread_mutex_lock(&list_lock_);
	// if there is someone existing in the vector unused_user_list_,we should use
	// it first,if no,new one more and add it into user_info_list_
	if(unused_user_list_.size() > 0)
	{
		UserInfo *tmp_user = unused_user_list_.front();
		tmp_user->hash_key = user.hash_key;
		tmp_user->user_id = user.user_id;
		tmp_user->user_sock = user.user_sock;
		bufferevent_setfd(tmp_user->buffev,tmp_user->user_sock);
		bufferevent_enable(tmp_user->buffev,EV_READ|EV_WRITE);
		user_list_[user.user_id] = tmp_user;
		unused_user_list_.pop_front();
		std::cout<<"Add user successfully"<<std::endl;
	}
	pthread_mutex_unlock(&list_lock_);
	return true;
}

UserInfo *UserInfoList::GetUserInfo(int user_id)
{
	UserInfo *tmp_user = NULL;
	pthread_mutex_lock(&list_lock_);
	tmp_user = user_list_[user_id];
	pthread_mutex_unlock(&list_lock_);
	return tmp_user;
}

bool UserInfoList::DelUserInfo(int user_id)
{
	pthread_mutex_lock(&list_lock_);
	UserInfo *tmp_user = user_list_[user_id];
	if(NULL != tmp_user)
	{
		tmp_user->hash_key = 0;
		tmp_user->user_id = 0;
		tmp_user->user_sock = 0;
		user_list_.erase(user_id);
		unused_user_list_.push_back(tmp_user);
	}
	pthread_mutex_unlock(&list_lock_);
	return true;
}

void UserInfoList::DeleteAllUser()
{
	pthread_mutex_lock(&list_lock_);
	USERLISTMAP::iterator tmp_iter;
	for(tmp_iter = user_list_.begin();tmp_iter != user_list_.end();tmp_iter++)
	{
		UserInfo *tmp_user = tmp_iter->second;
		if(NULL != tmp_user)
		{
			delete tmp_user;
		}
	}
	std::list<UserInfo *>::iterator tmp_unused_list_iter;
	for(tmp_unused_list_iter = unused_user_list_.begin();tmp_unused_list_iter != unused_user_list_.end();tmp_unused_list_iter++)
	{
		UserInfo *tmp_user = unused_user_list_.front();
		if(tmp_user)
			delete tmp_user;
	}
	unused_user_list_.clear();
	user_list_.clear();
	pthread_mutex_unlock(&list_lock_);
}
