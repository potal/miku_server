#ifndef CHAT_ROOM_H_
#define CHAT_ROOM_H_

#include "user_list.h"
#include <event.h>
#include <iostream>
#include <map>

class ChatRoom
{
private:
	bool m_is_start;
	long m_room_id;
	int m_sock;
	struct event m_room_event;

	UserInfoList m_user_list;
public:
	ChatRoom();
	~ChatRoom();

	bool Start();
	bool SetRoom(ChatRoom *chat_room);
	void Stop();
};


#endif
