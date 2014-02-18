#ifndef CANCLE_USER_MIC_RQ_H_
#define CANCLE_USER_MIC_RQ_H_

#include "../../common/base_command.h"

class CancleUserMicRQ : public BaseCommand
{
private:

public:
	CancleUserMicRQ(void *server_ptr);
	~CancleUserMicRQ();

	void Execute(char *buff,int len,void *caller_ptr);
};
#endif
