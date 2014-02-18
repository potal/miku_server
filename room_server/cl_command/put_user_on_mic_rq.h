#ifndef PUT_USER_ON_MIC_RQ_H_
#define PUT_USER_ON_MIC_RQ_H_

#include "../../common/base_command.h"

class PutUserOnMicRQ : public BaseCommand
{
private:

public:
	PutUserOnMicRQ(void *server_ptr);
	~PutUserOnMicRQ();

	void Execute(char *buff,int len,void *caller_ptr);
};

#endif
