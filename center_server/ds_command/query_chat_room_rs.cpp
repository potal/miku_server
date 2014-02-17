#include "query_chat_room_rs.h"
#include "../packet/ds_server.pb.h"

QueryChatRoomRS::QueryChatRoomRS(void *server_ptr)
{
	server_ptr_ = server_ptr;
}

QueryChatRoomRS::~QueryChatRoomRS()
{

}

void QueryChatRoomRS::Execute(char *buff,int len,void *caller_ptr)
{
	if(!server_ptr_ || !caller_ptr)
		return;
	StruDsGSGetRoomInfoRs tmp_get_room_rs;
	bool tmp_return = false;
	tmp_return = tmp_get_room_rs.ParseFromArray(buff,len);
	if(!tmp_return)
	{
		std::cout<<"Unpack StruDsGSGetRoomInfoRs Error!"<<std::endl;
		return;
	}
}
