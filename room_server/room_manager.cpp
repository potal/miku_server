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
	unused_room_list_.clear();
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

ChatRoom *RoomManager::GetNewRoom()
{
	AutoLock tmp_lock(&list_mutex_);
	ChatRoom *tmp_room = NULL;
	if(unused_room_list_.size() <= 0)
	{
		try
		{
			tmp_room = new ChatRoom;
		}
		catch(...)
		{
			tmp_room = NULL;
		}
		return tmp_room;
	}
	else
	{
		tmp_room = unused_room_list_.front();
		tmp_room->ResetRoom(0,-1);
		unused_room_list_.pop_front();
		return tmp_room;
	}
}

bool RoomManager::AddRoom(int room_id,ChatRoom *chat_room)
{
	AutoLock tmp_lock(&list_mutex_);
	if(NULL == chat_room)
		return false;

	if(NULL != room_list_[room_id])
	{
		return true;
	}
	room_list_[room_id] = chat_room;
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
	if(!tmp_room)
	{
		unused_room_list_.push_back(tmp_room);
	}

	room_list_.erase(tmp_iter);
	return true;
}

ChatRoom *RoomManager::GetRoom(int room_id)
{
	AutoLock tmp_lock(&list_mutex_);
	ChatRoom *tmp_room = room_list_[room_id];
	return tmp_room;
}
