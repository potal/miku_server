#include "query_chat_room_rs.h"
#include "../../packet/ds_server.pb.h"
#include "../gate_server.h"

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
	std::cout<<tmp_get_room_rs.server_id()<<std::endl;
	std::cout<<tmp_get_room_rs.room_id()<<std::endl;
	std::cout<<tmp_get_room_rs.rs_server_id()<<std::endl;
	std::cout<<tmp_get_room_rs.server_ip()<<std::endl;
	std::cout<<tmp_get_room_rs.server_port()<<std::endl;

	GateServer *tmp_server = reinterpret_cast<GateServer *>(server_ptr_);
	if(!tmp_server)
		return ;
	ChatRoom *tmp_room = NULL;
	tmp_room = tmp_server->GetRoomManager()->GetChatRoom(tmp_get_room_rs.room_id());
	if(tmp_room)
		return;
	RoomServerConnector * tmp_rs_conn = NULL;
	std::map<int,RoomServerConnector *> *tmp_conn_list = tmp_server->GetRSConnectionList();
	std::map<int,RoomServerConnector *>::iterator tmp_iter = tmp_conn_list->find(tmp_get_room_rs.rs_server_id());
	if(tmp_iter == tmp_conn_list->end())
	{
		tmp_rs_conn = new RoomServerConnector;
		tmp_rs_conn->InitConnectionInfo(tmp_get_room_rs.server_ip().c_str(),tmp_get_room_rs.server_port());
		tmp_return = tmp_rs_conn->StartConnect();
		if(!tmp_return)
		{
			delete tmp_rs_conn;
			return;
		}
		tmp_return = tmp_rs_conn->StartRSProcessor(1000,1,server_ptr_);
		if(!tmp_return)
		{
			tmp_rs_conn->Disconnect();
			delete tmp_rs_conn;
			return;
		}
	}
	else
	{
		tmp_rs_conn = tmp_iter->second;
	}
	tmp_server->GetRoomManager()->AddRoom(tmp_get_room_rs.room_id(),tmp_rs_conn->GetSocket());
	tmp_server->GetCLProcessor()->DeleteRoom(tmp_get_room_rs.room_id());
}
