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

ChatRoom::ChatRoom():room_id_(0),server_fd_(-1)
{
}

ChatRoom::~ChatRoom()
{
}

bool ChatRoom::SetRoom(int room_id,int server_fd)
{
	room_id_ = room_id;
	server_fd_ = server_fd;
	return true;
}

