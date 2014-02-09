#ifndef CHAT_ROOM_H_
#define CHAT_ROOM_H_

#include <iostream>
#include <map>
#include "user_info_list.h"

class ChatRoom
{
private:
	int m_room_id;
	int m_sock;
	UserInfoList user_list_;
	UserInfoList pre_user_list_;

public:
	ChatRoom();
	~ChatRoom();

	bool SetRoom(ChatRoom *chat_room);
	UserInfoList *GetUserInfoList();
	UserInfoList *GetPreUserInfoList();
};


#endif
