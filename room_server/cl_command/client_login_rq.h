#ifndef CLIENT_LOGIN_RQ_H_
#define CLIENT_LOGIN_RQ_H_

#include "../../common/base_command.h"

class ClientLoginRQ : public BaseCommand
{
private:

public:
	ClientLoginRQ();
	ClientLoginRQ(void *server_ptr);
	~ClientLoginRQ();

	void Execute(char *buff,int len,void *caller_ptr);
};

#endif
