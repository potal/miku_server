#ifndef ROOM_MANAGER_H_
#define ROOM_MANAGER_H_

#include "chat_room.h"
#include "common/auto_lock.h"
#include <iostream>
#include <map>
#include <pthread.h>
//lock

class RoomManager
{
private:
	std::map<int,ChatRoom *> room_list_;
	pthread_mutex_t list_mutex_;

public:
	RoomManager();
	~RoomManager();

	bool AddRoom(int room_id,ChatRoom * chat_room);
	bool DelRoom(int room_id);
	ChatRoom *GetRoom(int room_id);
};

#endif
