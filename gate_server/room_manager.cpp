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
	m_room_list.clear();
	pthread_mutex_init(&m_list_mutex,NULL);
	pthread_cond_init(&m_list_cond,NULL);
}

RoomManager::~RoomManager()
{
	std::map<long,ChatRoom*>::iterator tmp_iter;
	for(tmp_iter = m_room_list.begin();tmp_iter != m_room_list.end();tmp_iter++)
	{
		ChatRoom *tmp_room = tmp_iter->second;
		if(NULL != tmp_room)
		{
			tmp_room->Stop();
			delete tmp_room;
		}
	}
	m_room_list.clear();
}


bool RoomManager::AddRoom(long room_id,ChatRoom *chat_room)
{
	if(NULL == chat_room)
		return false;

	if(NULL != m_room_list[room_id])
	{
		//the room adding exists,modify room information
		return true;
	}

	ChatRoom * tmp_room = new ChatRoom;
	if(NULL == tmp_room)
		return false;

	tmp_room->SetRoom(chat_room);
	tmp_room->Start();

	m_room_list[room_id] = tmp_room;
}

bool RoomManager::DelRoom(long room_id)
{
	std::map<long,ChatRoom*>::iterator tmp_iter = m_room_list.find(room_id);
	if(tmp_iter == m_room_list.end())
	{
		return false;
	}

	ChatRoom *tmp_room = tmp_iter->second;
	tmp_room->Stop();
	m_room_list.erase(tmp_iter);
	return true;
}
