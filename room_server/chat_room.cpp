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

ChatRoom::ChatRoom():sock_(-1),room_id_(0),start_channel_id_(1000)
{
	user_list_.InitUserList(100000);
	pre_user_list_.InitUserList(1000);
	pthread_mutex_init(&channel_list_lock_,NULL);
}

ChatRoom::~ChatRoom()
{
	video_channel_list_.clear();
	pthread_mutex_destroy(&channel_list_lock_);
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

std::string ChatRoom::UserApplyOneNewVideo(int user_id)
{
	AutoLock tmp_lock(&channel_list_lock_);
	std::string tmp_new_channel = video_channel_list_[user_id];
	if(tmp_new_channel != "")
		return "";
	std::stringstream tmp_ss;
	tmp_ss>>room_id_;
	start_channel_id_++;
	tmp_ss>>start_channel_id_;
	tmp_new_channel = tmp_ss.str();
	video_channel_list_[user_id] = tmp_new_channel;
	return tmp_new_channel;
}

void ChatRoom::UserGetOffVideo(int user_id)
{
	AutoLock tmp_lock(&channel_list_lock_);
	video_channel_list_.erase(user_id);
}

void ChatRoom::GetMicUserList(std::map<int,std::string> &mic_list)
{
	AutoLock tmp_lock(&channel_list_lock_);
	std::map<int ,std::string>::iterator tmp_iter;
	for(tmp_iter = video_channel_list_.begin();tmp_iter != video_channel_list_.end();tmp_iter++)
	{
		mic_list[tmp_iter->first] = tmp_iter->second;
	}
}
