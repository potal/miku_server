#ifndef USER_INFO_LIST_H_
#define USER_INFO_LIST_H_

#include <iostream>
#include <map>
#include <list>
#include <string>
#include <pthread.h>
#include <string.h>
#include "../common/auto_lock.h"

using std::map;

const int MaxNickNameLen = 0x20;
#pragma pack(1)
struct UserInfo
{
	int gs_hashkey_user_on;
	int user_id;
	int user_hash_key;
	char user_nickname[MaxNickNameLen];
	int user_red_d;
	int user_blue_d;
	int user_level;
	int buyer_riches;
	int buyer_grow_up;
	int buyer_honour;
	int buyer_estimate;
	int seller_income;
	int seller_exp;
	int seller_pop;
	int seller_estimate;

	UserInfo():gs_hashkey_user_on(0),user_id(0),user_hash_key(0),user_red_d(0),user_blue_d(0),buyer_riches(0),
		buyer_grow_up(0),buyer_honour(0),buyer_estimate(0),seller_income(0),seller_exp(0),seller_pop(0),seller_estimate(0)
	{
		memset(user_nickname,0,MaxNickNameLen);
	}
	void Clear()
	{
		gs_hashkey_user_on = 0;
		user_id = 0;
		user_hash_key = 0;
		memset(user_nickname,0,MaxNickNameLen);
		user_red_d = 0;
		user_blue_d = 0;
		user_level = 0;
		buyer_estimate = 0;
		buyer_honour = 0;
		buyer_grow_up = 0;
		buyer_riches = 0;
		seller_estimate = 0;
		seller_exp = 0;
		seller_pop = 0;
		seller_income = 0;
	}
};
#pragma pack()

class UserInfoList
{
private:
	pthread_mutex_t list_lock_;
	int max_user_count_;
	std::list<UserInfo *> unused_user_info_list_;
	std::map<int,UserInfo *> user_info_list_;

public:
	UserInfoList();
	~UserInfoList();

	bool InitUserList(int max_user_count);

	int GetUnusedUser(UserInfo *&user_info);
	bool PushUserInUnusedList(UserInfo *user_info);

	bool AddUserInfo(int user_id,UserInfo *user_info);
	void RemoveUser(int user_id);
	UserInfo *GetUserInfo(int user_id);

	void ClearAllUserInfo();
};

#endif
