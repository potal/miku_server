#ifndef BASE_USER_INFO_RESOURCE_H_
#define BASE_USER_INFO_RESOURCE_H_
// 用户列表资源基础类
#include "base_user_info.h"
class BaseUserInfoResource
{
public:
	virtual BaseUserInfo *GetUserInfo()=0;
	virtual void ReleaseUserInfo(BaseUserInfo *user)=0;
	virtual BaseUserInfo *GetUserByHashkey(int user_hashkey)=0;
	virtual bool AddUserInfo(int user_hashkey,BaseUserInfo *user)=0;
};

#endif
