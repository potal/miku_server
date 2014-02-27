/*
 * =====================================================================================
 *
 *       Filename:  user_info_list.cpp
 *
 *    Description:  store all user information
 *
 *        Version:  1.0
 *        Created:  02/08/2014 08:46:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (potal), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "user_info_list.h"

UserInfoList::UserInfoList():max_user_count_(0)
{
	pthread_mutex_init(&list_lock_,NULL);
	unused_user_info_list_.clear();
	user_info_list_.clear();
}

UserInfoList::~UserInfoList()
{
	std::list<UserInfo *>::iterator tmp_list_iter;
	for(tmp_list_iter = unused_user_info_list_.begin();tmp_list_iter != unused_user_info_list_.end();tmp_list_iter++)
	{
		UserInfo *tmp_user = *tmp_list_iter;
		if(tmp_user)
		{
			delete tmp_user;
			tmp_user = NULL;
		}
	}
	unused_user_info_list_.clear();

	std::map<int,UserInfo *>::iterator tmp_map_iter;
	for(tmp_map_iter = user_info_list_.begin();tmp_map_iter != user_info_list_.end();tmp_map_iter++)
	{
		UserInfo *tmp_user = tmp_map_iter->second;
		if(tmp_user)
		{
			delete tmp_user;
			tmp_user = NULL;
		}
	}
	user_info_list_.clear();
	
	pthread_mutex_destroy(&list_lock_);
}

bool UserInfoList::InitUserList(int max_user_count)
{
	AutoLock tmp_lock(&list_lock_);
	max_user_count_ = max_user_count;
	try
	{
		for(int i = 0;i < max_user_count;i++)
		{
			UserInfo *tmp_user = new UserInfo;
			if(tmp_user)
			{
				unused_user_info_list_.push_back(tmp_user);
			}
		}
	}
	catch(...)
	{
		std::cout<<"new error"<<std::endl;
	}
	user_info_list_.clear();
	return true;
}

int UserInfoList::GetUnusedUser(UserInfo *&user_info)
{
	AutoLock tmp_lock(&list_lock_);
	UserInfo *tmp_user = unused_user_info_list_.front();
	if(tmp_user)
	{
		tmp_user->Clear();
		user_info = tmp_user;
		unused_user_info_list_.pop_front();
		return 1;
	}
	else if(unused_user_info_list_.size() <= 0)
	{
		try
		{
			tmp_user = new UserInfo;
			if(tmp_user)
			{
				user_info = tmp_user;
			}
		}
		catch(...)
		{
			std::cout<<"new error"<<std::endl;
			return -2;
		}
		return 0;
	}
	else
		return -1;
}

bool UserInfoList::PushUserInUnusedList(UserInfo *user_info)
{
	AutoLock tmp_lock(&list_lock_);
	unused_user_info_list_.push_back(user_info);
	return true;
}

bool UserInfoList::AddUserInfo(int user_id,UserInfo *user_info)
{
	AutoLock tmp_lock(&list_lock_);
	if(user_info_list_[user_id])
	{
		std::cout<<"User "<<user_id<<" in list"<<std::endl;
		return false;
	}
	user_info_list_[user_id] = user_info;
	return true;
}

void UserInfoList::RemoveUser(int user_id)
{
	AutoLock tmp_lock(&list_lock_);
	user_info_list_.erase(user_id);
}

UserInfo *UserInfoList::GetUserInfo(int user_id)
{
	AutoLock tmp_lock(&list_lock_);
	UserInfo *tmp_user = user_info_list_[user_id];
	return tmp_user;
}

UserInfo *UserInfoList::GetFirstUser()
{
	pthread_mutex_lock(&list_lock_);
	if(user_info_list_.size() <= 0)
	{
		pthread_mutex_unlock(&list_lock_);
		return NULL;
	}
	list_iter_ = user_info_list_.begin();
	return list_iter_->second;
}
UserInfo *UserInfoList::GetNextUser()
{
	list_iter_++;
	if(list_iter_ == user_info_list_.end())
	{
		pthread_mutex_unlock(&list_lock_);
		return NULL;
	}
	return list_iter_->second;
}

void UserInfoList::ClearAllUserInfo()
{
	AutoLock tmp_lock(&list_lock_);
	std::map<int ,UserInfo *>::iterator tmp_map_iter;
	for(tmp_map_iter = user_info_list_.begin();tmp_map_iter != user_info_list_.end();tmp_map_iter++)
	{
		UserInfo *tmp_user = tmp_map_iter->second;
		PushUserInUnusedList(tmp_user);
	}
	user_info_list_.clear();
}
