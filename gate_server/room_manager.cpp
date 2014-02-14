/*
 * =====================================================================================
 *
 *       Filename:  roommanger.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/14/2014 02:01:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "room_manager.h"

RoomManager::RoomManager()
{
	room_list_.clear();
	pthread_mutex_init(&list_mutex_,NULL);
}

RoomManager::~RoomManager()
{
	std::map<int,ChatRoom*>::iterator tmp_iter;
	for(tmp_iter = room_list_.begin();tmp_iter != room_list_.end();tmp_iter++)
	{
		ChatRoom *tmp_room = tmp_iter->second;
		if(NULL != tmp_room)
		{
			delete tmp_room;
		}
	}
	room_list_.clear();
	pthread_mutex_destroy(&list_mutex_);
}


bool RoomManager::AddRoom(int room_id,int server_fd)
{
	AutoLock tmp_lock(&list_mutex_);
	if(NULL != room_list_[room_id])
	{
		//the room adding exists,modify room information
		return true;
	}

	ChatRoom * tmp_room = new ChatRoom;
	if(NULL == tmp_room)
		return false;

	tmp_room->SetRoom(room_id,server_fd);

	room_list_[room_id] = tmp_room;
	return true;
}

ChatRoom * RoomManager::GetChatRoom(int room_id)
{
	AutoLock tmp_lock(&list_mutex_);
	ChatRoom *tmp_chat_room = room_list_[room_id];
	return tmp_chat_room;
}

bool RoomManager::DelRoom(int room_id)
{
	AutoLock tmp_lock(&list_mutex_);
	std::map<int,ChatRoom*>::iterator tmp_iter = room_list_.find(room_id);
	if(tmp_iter == room_list_.end())
	{
		return false;
	}

	ChatRoom *tmp_room = tmp_iter->second;
	room_list_.erase(tmp_iter);
	return true;
}
