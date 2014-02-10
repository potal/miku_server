#ifndef BASE_USER_INFO_H_
#define BASE_USER_INFO_H_

#include <iostream>
#include <event.h>

//用户资料，可扩展
#pragma pack(1)
class BaseUserInfo
{
public:
	int user_sock;
	int hash_key;
	int user_uip;
	int user_uport;

	struct bufferevent * buffev;
	void *server_ptr;

public:
	BaseUserInfo():user_sock(-1),hash_key(0),user_uip(0),user_uport(0),server_ptr(NULL)
	{
		buffev = bufferevent_new(-1,NULL,NULL,NULL,NULL);
		//std::cout<<buffev<<std::endl;
	};
	virtual ~BaseUserInfo()
	{
		//bufferevent_free(buffev);
	};

	virtual void Clear(){
		user_sock = 0;
		hash_key = 0;
		user_uip = 0;
		user_uport = 0;
		server_ptr = NULL;
	};
	virtual void DealWithData(struct bufferevent *buff_ev,void *arg) = 0;
};
#pragma pack()

#endif
