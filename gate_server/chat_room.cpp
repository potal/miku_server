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

ChatRoom::ChatRoom():m_sock(0),m_room_id(0),m_is_start(false)
{
}

ChatRoom::~ChatRoom()
{
}

bool ChatRoom::Start()
{
	m_is_start = true;
	return m_is_start;
}

bool ChatRoom::SetRoom(ChatRoom* chat_room)
{
	return true;
}

void ChatRoom::Stop()
{
	m_is_start = false;
}
