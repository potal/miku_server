#ifndef USER_SEND_MSG_RQ_H_
#define USER_SEND_MSG_RQ_H_

#include "../../common/base_command.h"

class UserSendMsgRQ : public BaseCommand
{
private:

public:
	UserSendMsgRQ(void *server_ptr);
	~UserSendMsgRQ();

	void Execute(char *buff,int len,void *caller_ptr);
};

#endif
