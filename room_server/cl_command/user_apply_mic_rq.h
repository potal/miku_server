#ifndef USER_APPLY_MIC_RQ_H_
#define USER_APPLY_MIC_RQ_H_

#include "../../common/base_command.h"

class UserApplyMicRQ : public BaseCommand
{
private:

public:
	UserApplyMicRQ(void *server_ptr);
	~UserApplyMicRQ();

	void Execute(char *buff,int len,void *caller_ptr);
};



#endif
