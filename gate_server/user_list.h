#ifndef USER_LIST_H_
#define USER_LIST_H_

#include "server_types.h"
#include <iostream>
#include <map>
#include <list>
#include <pthread.h>
#include <event.h>

#pragma pack(1)
struct UserInfo
{
	int user_id;
	int user_sock;
	long hash_key;
	struct bufferevent * buffev;
	void *server_ptr;
};
#pragma pack()

typedef std::map<int,UserInfo*> USERLISTMAP;

class UserInfoList
{
private:
	USERLISTMAP user_list_;
	std::list<UserInfo*> unused_user_list_;
	pthread_mutex_t list_lock_;
	pthread_cond_t list_cond_;

public:
	UserInfoList();
	~UserInfoList();
	
	int Init(int num,struct event_base * base);
	inline USERLISTMAP * GetUserList()
	{
		return &user_list_;
	}

	bool AddUserInfo(int user_id,UserInfo & user);
	UserInfo *GetUserInfo(int user_id);
	bool DelUserInfo(int user_id);
	void DeleteAllUser();
};

#endif
