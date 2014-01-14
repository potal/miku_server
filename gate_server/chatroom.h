#ifndef CHAT_ROOM_H_
#define CHAT_ROOM_H_

#include <event.h>
#include <iostream>
#include <map>

#pragma pack(1)
struct ConnectedUser
{
	int m_user_id;
	int m_user_sock;
};
#pragma pack()

class ChatRoom
{
private:
	bool m_is_start;
	long m_room_id;
	int m_sock;
	struct event m_room_event;
	std::map<int,ConnectedUser*> m_user_list;

public:
	ChatRoom();
	~ChatRoom();

	bool Start();
	void Stop();
};


#endif
