#ifndef CHAT_ROOM_H_
#define CHAT_ROOM_H_

#include <iostream>
#include <map>
#include "user_info_list.h"

class ChatRoom
{
private:
	int room_id_;
	int sock_;
	UserInfoList user_list_;
	UserInfoList pre_user_list_;

public:
	ChatRoom();
	~ChatRoom();

	bool SetRoom(int room_id,int room_fd);
	void ResetRoom(int room_id,int room_fd);
	UserInfoList *GetUserInfoList();
	UserInfoList *GetPreUserInfoList();

	UserInfo *NewUserInfo();
	UserInfo *NewPreUserInfo();

	bool AddUser(int user_id,UserInfo *user);
	bool AddPreUser(int user_id,UserInfo *user);

	UserInfo *GetUserInfo(int user_id);
	UserInfo *GetPreUserInfo(int user_id);

	void RemoveUser(int user_id);
	void RemovePreUser(int user_id);
};


#endif
