#ifndef ROOM_MANAGER_H_
#define ROOM_MANAGER_H_

#include "chatroom.h"
#include <iostream>
#include <map>

class RoomManager
{
private:
	std::map<long,ChatRoom*> m_room_list;
public:
	RoomManager();
	~RoomManager();

	bool AddRoom(ChatRoom * chat_room);
	bool DelRoom(long room_id);
}

#endif
