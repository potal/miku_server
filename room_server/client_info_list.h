#ifndef CLIENT_INFO_LIST_H_
#define CLIENT_INFO_LIST_H_

#include "../common/base_user_info.h"
#include "../common/base_user_info_resource.h"
#include "../common/resource_manager.h"
#include "gs_processor.h"
#include <iostream>
#include <map>

const int MaxRecvBuffLen = 0x4000;

class ClientInfoEx : public BaseUserInfo
{
public:
	int valid_buff_len_;
	char *recved_buff_;
	void *room_server_;

	GateServerProcessor gs_processor_;//gs processor
public:
	ClientInfoEx();
	~ClientInfoEx();

	void Clear();
	void DealWithData(struct bufferevent *buff_ev,void *arg);
	inline GateServerProcessor *GetGSProcessor(){
		return &gs_processor_;
	}
};

class ClientInfoList : public BaseUserInfoResource
{
private:
	UserResourceManager<ClientInfoEx> unused_user_list_;
	std::map<int,ClientInfoEx*> user_list_;
	void *room_server_;
	pthread_mutex_t list_lock_;

public:
	virtual BaseUserInfo *GetUserInfo();
	virtual void ReleaseUserInfo(BaseUserInfo * user);
	virtual BaseUserInfo *GetUserByHashkey(int user_hashkey);
	virtual bool AddUserInfo(int user_hashkey,BaseUserInfo *user);

public:
	ClientInfoList();
	~ClientInfoList();

	int Init(int max_user,void *server_ptr);
};
#endif
