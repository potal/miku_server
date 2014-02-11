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

#include "chat_room.h"

ChatRoom::ChatRoom():m_sock(-1),m_room_id(0)
{
	user_list_.InitUserList(100000);
	pre_user_list_.InitUserList(1000);
}

ChatRoom::~ChatRoom()
{
}

bool ChatRoom::SetRoom(ChatRoom* chat_room)
{
	return true;
}

UserInfoList *ChatRoom::GetUserInfoList()
{
	return &user_list_;
}

UserInfoList *ChatRoom::GetPreUserInfoList()
{
	return &pre_user_list_;
}

