#ifndef USER_OFF_MIC_RQ_H_
#define USER_OFF_MIC_RQ_H_

#include "../../common/base_command.h"

class UserOffMicRQ : public BaseCommand
{
private:

public:
	UserOffMicRQ(void *server_ptr);
	~UserOffMicRQ();

	void Execute(char *buff,int len,void *caller_ptr);
};

#endif
