#ifndef GET_CS_ADDR_RS_H_
#define GET_CS_ADDR_RS_H_

#include "../../common/base_command.h"

class GetCenterServerAddrRS : public BaseCommand
{
private:

public:
	GetCenterServerAddrRS();
	GetCenterServerAddrRS(void *server_ptr);
	~GetCenterServerAddrRS();

	void Execute(char *buff,int len,void *caller_ptr);
};
#endif
