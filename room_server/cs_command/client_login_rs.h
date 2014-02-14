#ifndef CLIENT_LOGIN_RS_H_
#define CLIENT_LOGIN_RS_H_

#include "../../common/base_command.h"

class ClientLoginRS : public BaseCommand
{
private:

public:
	ClientLoginRS();
	ClientLoginRS(void *server_ptr);
	~ClientLoginRS();

	void Execute(char *buff,int len,void *caller_ptr);
};

#endif
