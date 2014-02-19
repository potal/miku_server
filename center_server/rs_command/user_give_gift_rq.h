#ifndef USER_GIVE_GIFT_RQ_
#define USER_GIVE_GIFT_RQ_

#include "../../common/base_command.h"

class UserGiveGiftRQ : public BaseCommand
{
private:

public:
	UserGiveGiftRQ(void *server_ptr);
	~UserGiveGiftRQ();

	void Execute(char *buff,int len,void *caller_ptr);
};

#endif
