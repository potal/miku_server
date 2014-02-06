#ifndef OPEN_CHAT_ROOM_H_
#define OPEN_CHAT_ROOM_H_

#include "../common/base_command.h"
class OpenChatRoom : public BaseCommand
{
private:

public:
	OpenChatRoom(void *server_ptr);
	~OpenChatRoom();

	void Execute(char *buff,int len,void *server_ptr);
};

#endif
