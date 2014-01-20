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

bool UserInfoList::AddUserInfo(int user_id,UserInfo & user)
{
	pthread_mutex_lock(&list_lock_);
	// do something
	pthread_mutex_unlock(&list_lock_);
	return true;
}

bool UserInfoList::DelUserInfo(int user_id)
{
	return true;
}

void UserInfoList::DeleteAllUser()
{
	USERLISTMAP::iterator tmp_iter;
	for(tmp_iter = user_list_.begin();tmp_iter != user_list_.end();tmp_iter++)
	{
		UserInfo *tmp_user = tmp_iter->second;
		if(NULL != tmp_user)
		{
			delete tmp_user;
		}
	}
	user_list_.clear();
}
