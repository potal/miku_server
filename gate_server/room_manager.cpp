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
	pthread_cond_init(&list_cond_,NULL);
}

RoomManager::~RoomManager()
{
	std::map<long,ChatRoom*>::iterator tmp_iter;
	for(tmp_iter = room_list_.begin();tmp_iter != room_list_.end();tmp_iter++)
	{
		ChatRoom *tmp_room = tmp_iter->second;
		if(NULL != tmp_room)
		{
			tmp_room->Stop();
			delete tmp_room;
		}
	}
	room_list_.clear();
}


bool RoomManager::AddRoom(long room_id,ChatRoom *chat_room)
{
	if(NULL == chat_room)
		return false;

	if(NULL != room_list_[room_id])
	{
		//the room adding exists,modify room information
		return true;
	}

	ChatRoom * tmp_room = new ChatRoom;
	if(NULL == tmp_room)
		return false;

	tmp_room->SetRoom(chat_room);
	tmp_room->Start();

	room_list_[room_id] = tmp_room;
}

bool RoomManager::DelRoom(long room_id)
{
	std::map<long,ChatRoom*>::iterator tmp_iter = room_list_.find(room_id);
	if(tmp_iter == room_list_.end())
	{
		return false;
	}

	ChatRoom *tmp_room = tmp_iter->second;
	tmp_room->Stop();
	room_list_.erase(tmp_iter);
	return true;
}
