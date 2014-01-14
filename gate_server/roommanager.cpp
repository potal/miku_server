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

#include "roommanager.h"

RoomManager::RoomManager()
{
	
}

RoomManager::~RoomManager()
{
	std::map<ChatRoom*>::iterator tmp_iter;
	for(tmp_iter = m_room_list.begin();tmp_iter != m_room_list.end();tmp_iter++)
	{
		ChatRoom *tmp_room = tmp_iter->second;
		if(NULL != tmp_room)
		{
			tmp_room->Stop();
			delete tmp_room;
		}

	}
}


bool RoomManager::AddRoom(ChatRoom *chat_room_)
