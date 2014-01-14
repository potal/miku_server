#ifndef ROOM_MANAGER_H_
#define ROOM_MANAGER_H_

#include "chat_room.h"
#include <iostream>
#include <map>
#include <pthread.h>
//lock

class RoomManager
{
private:
	std::map<long,ChatRoom*> m_room_list;
	pthread_mutex_t m_list_mutex;
	pthread_cond_t m_list_cond;
public:
	RoomManager();
	~RoomManager();

	bool AddRoom(long room_id,ChatRoom * chat_room);
	bool DelRoom(long room_id);
};

#endif
