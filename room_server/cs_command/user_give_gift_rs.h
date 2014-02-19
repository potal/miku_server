#ifndef USER_GIVE_GIFT_RQ_
#define USER_GIVE_GIFT_RQ_

#include "../../common/base_command.h"

class UserGiveGiftRS : public BaseCommand
{
private:

public:
	UserGiveGiftRS(void *server_ptr);
	~UserGiveGiftRS();

	void Execute(char *buff,int len,void *caller_ptr);
};

#endif
