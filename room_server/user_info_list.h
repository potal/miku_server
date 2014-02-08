#ifndef USER_INFO_LIST_H_
#define USER_INFO_LIST_H_

#include "common/base_user_info.h"
#include "common/base_user_info_resource.h"
#include "common/resource_manager.h"
#include <iostream>
#include <map>


class UserInfoEx : public BaseUserInfo
{
public:
	char *recved_buff_;
	void *room_server_;
public:
	UserInfoEx();
	~UserInfoEx();

	void Clear();
	void DealWithData(struct bufferevent *buff_ev,void *arg);
};

class UserInfoList : public BaseUserInfoResource
{
private:
	UserResourceManager<UserInfoEx> unused_user_list_;
	std::map<int,UserInfoEx*> user_list_;
	void *room_server_;
	pthread_mutex_t list_lock_;

public:
	virtual BaseUserInfo *GetUserInfo();
	virtual void ReleaseUserInfo(BaseUserInfo * user);
	virtual BaseUserInfo *GetUserByHashkey(int user_hashkey);
	virtual bool AddUserInfo(int user_hashkey,BaseUserInfo *user);

public:
	UserInfoList();
	~UserInfoList();

	int Init(int max_user,void *server_ptr);
};
#endif
