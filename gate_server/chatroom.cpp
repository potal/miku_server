/*
 * =====================================================================================
 *
 *       Filename:  chatroom.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/14/2014 01:49:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <chatroom.h>

ChatRoom::ChatRoom():m_sock(0),m_room_id(0),m_is_start(false)
{
	m_user_list.clear();
}

ChatRoom::~ChatRoom()
{
	std::map<int,ConnectedUser*>::iterator tmp_iter;
	for(tmp_iter = m_user_list.begin();tmp_iter != m_user_list.end();tmp++)
	{
		ConnectedUser *tmp_user = tmp_iter->second;
		if(NULL != tmp_user)
		{
			delete tmp_user;
		}
	}
	m_user_list.clear();
}

bool ChatRoom::Start()
{
	m_is_start = true;
	return m_is_start;
}

void ChatRoom::Stop()
{
	m_is_start = false;
}
