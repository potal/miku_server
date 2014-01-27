#ifndef CHAT_ROOM_H_
#define CHAT_ROOM_H_

#include <iostream>
#include <map>

class ChatRoom
{
private:
	long m_room_id;
	int m_sock;

public:
	ChatRoom();
	~ChatRoom();

	bool SetRoom(ChatRoom *chat_room);
};


#endif
