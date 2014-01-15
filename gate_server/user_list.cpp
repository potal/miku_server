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
	pthread_mutex_init(&m_list_lock,NULL);
	pthread_cond_init(&m_list_cond,NULL);
}

UserInfoList::~UserInfoList()
{
	DeleteAllUser();
	pthread_mutex_destroy(&m_list_lock);
	pthread_cond_destroy(&m_list_cond);
}

bool UserInfoList::AddUserInfo(int user_id,UserInfo & user)
{
	pthread_mutex_lock(&m_list_lock);
	// do something
	pthread_mutex_unlock(&m_list_lock);
	return true;
}

bool UserInfoList::DelUserInfo(int user_id)
{
	return true;
}

void UserInfoList::DeleteAllUser()
{
	USERLISTMAP::iterator tmp_iter;
	for(tmp_iter = m_user_list.begin();tmp_iter != m_user_list.end();tmp_iter++)
	{
		UserInfo *tmp_user = tmp_iter->second;
		if(NULL != tmp_user)
		{
			delete tmp_user;
		}
	}
	m_user_list.clear();
}
