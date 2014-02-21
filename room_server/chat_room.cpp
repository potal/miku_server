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

UserInfo *ChatRoom::NewUserInfo()
{
	UserInfo *tmp_user = NULL;
	int tmp_get_user = user_list_.GetUnusedUser(tmp_user);
	if(tmp_get_user < 0)
		return NULL;
	else
		return tmp_user;
}

UserInfo *ChatRoom::NewPreUserInfo()
{
	UserInfo *tmp_user = NULL;
	int tmp_get_user = user_list_.GetUnusedUser(tmp_user);
	if(tmp_get_user < 0)
		return NULL;
	else
		return tmp_user;
}

bool ChatRoom::AddUser(int user_id,UserInfo *user)
{
	return user_list_.AddUserInfo(user_id,user);
}

bool ChatRoom::AddPreUser(int user_id,UserInfo *user)
{
	return pre_user_list_.AddUserInfo(user_id,user);
}

UserInfo *ChatRoom::GetUserInfo(int user_id)
{
	return user_list_.GetUserInfo(user_id);
}

UserInfo *ChatRoom::GetPreUserInfo(int user_id)
{
	return pre_user_list_.GetUserInfo(user_id);
}

void ChatRoom::RemoveUser(int user_id)
{
	UserInfo *tmp_user = user_list_.GetUserInfo(user_id);
	if(!tmp_user)
	{
		return;
	}
	user_list_.RemoveUser(user_id);
	user_list_.PushUserInUnusedList(tmp_user);
}

void ChatRoom::RemovePreUser(int user_id)
{
	UserInfo *tmp_user = pre_user_list_.GetUserInfo(user_id);
	if(!tmp_user)
		return;
	pre_user_list_.RemoveUser(user_id);
	pre_user_list_.PushUserInUnusedList(tmp_user);
}
