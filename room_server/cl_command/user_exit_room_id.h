#ifndef USER_EXIT_ROOM_ID_H_
#define USER_EXIT_ROOM_ID_H_

#include "../../common/base_command.h"

class UserExitRoomID : public BaseCommand
{
private:

public:
	UserExitRoomID(void *server_ptr);
	~UserExitRoomID();

	void Execute(char *buff,int len,void *caller_ptr);
};


#endif
