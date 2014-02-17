#ifndef CONNECT_DS_RS_H_
#define CONNECT_DS_RS_H_

#include "../../common/base_command.h"

class ConnectDirectorServerRS : public BaseCommand
{
private:

public:
	ConnectDirectorServerRS();
	ConnectDirectorServerRS(void *server_ptr);
	~ConnectDirectorServerRS();

	void Execute(char *buff,int len,void *caller_ptr);
};

#endif
