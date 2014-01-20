#ifndef USER_LIST_H_
#define USER_LIST_H_

#include "server_types.h"
#include <iostream>
#include <map>
#include <pthread.h>

#pragma pack(1)
struct UserInfo
{
	int user_id;
	int user_sock;
	long hash_key;
};
#pragma pack()

typedef std::map<int,UserInfo*> USERLISTMAP;

class UserInfoList
{
private:
	USERLISTMAP user_list_;
	pthread_mutex_t list_lock_;
	pthread_cond_t list_cond_;

public:
	UserInfoList();
	~UserInfoList();
	
	inline USERLISTMAP * GetUserList()
	{
		return &user_list_;
	}

	bool AddUserInfo(int user_id,UserInfo & user);
	bool DelUserInfo(int user_id);
	void DeleteAllUser();
};

#endif
