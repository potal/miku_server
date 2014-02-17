#ifndef QUERY_CHAT_ROOM_RS_H_
#define QUERY_CHAT_ROOM_RS_H_

#include "../../common/base_command.h"

class QueryChatRoomRS : public BaseCommand
{
private:

public:
	QueryChatRoomRS(void *server_ptr);
	~QueryChatRoomRS();

	void Execute(char *buff,int len,void *caller_ptr);
};

#endif
