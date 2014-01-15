#ifndef USER_LIST_H_
#define USER_LIST_H_

#include "server_types.h"
#include <iostream>
#include <map>
#include <pthread.h>

#pragma pack(1)
struct UserInfo
{
	int m_user_id;
	int m_user_sock;
	long m_hash_key;
};
#pragma pack()

typedef std::map<int,UserInfo*> USERLISTMAP;

class UserInfoList
{
private:
	USERLISTMAP m_user_list;
	pthread_mutex_t m_list_lock;
	pthread_cond_t m_list_cond;

public:
	UserInfoList();
	~UserInfoList();
	
	inline USERLISTMAP * GetUserList()
	{
		return &m_user_list;
	}

	bool AddUserInfo(int user_id,UserInfo & user);
	bool DelUserInfo(int user_id);
	void DeleteAllUser();
};

#endif
