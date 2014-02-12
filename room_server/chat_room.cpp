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

ChatRoom::ChatRoom():sock_(-1),room_id_(0)
{
	user_list_.InitUserList(100000);
	pre_user_list_.InitUserList(1000);
}

ChatRoom::~ChatRoom()
{
}

bool ChatRoom::SetRoom(int room_id,int room_fd)
{
	room_id_ = room_id;
	sock_ = room_fd;
	return true;
}

void ChatRoom::ResetRoom(int room_id,int room_fd)
{
	room_id_ = room_id;
	sock_ = room_fd;
	user_list_.ClearAllUserInfo();
	pre_user_list_.ClearAllUserInfo();
}

UserInfoList *ChatRoom::GetUserInfoList()
{
	return &user_list_;
}

UserInfoList *ChatRoom::GetPreUserInfoList()
{
	return &pre_user_list_;
}

