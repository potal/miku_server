//user login result from cs

#include "client_login_rs.h"
#include "../room_server.h"
#include "../cs_processor.h"

ClientLoginRS::ClientLoginRS()
{
}

ClientLoginRS::ClientLoginRS(void *server_ptr)
{
	server_ptr_ = server_ptr;
}

ClientLoginRS::~ClientLoginRS()
{

}

void ClientLoginRS::Execute(char *buff,int len,void *caller_ptr)
{
	if(!server_ptr_ || !caller_ptr)
		return ;
	CenterServerProcessor *tmp_processor = reinterpret_cast<CenterServerProcessor *>(caller_ptr);
	RoomServer *tmp_server = reinterpret_cast<RoomServer *>(server_ptr_);

	StruUserLoginRS tmp_user_login_rs;
	bool tmp_return = false;
	tmp_return = tmp_user_login_rs.ParseFromArray(buff,len);
	if(!tmp_return)
		return ;
	std::cout<<tmp_user_login_rs.msg_id()<<std::endl;
	std::cout<<tmp_user_login_rs.user_id()<<std::endl;
	std::cout<<tmp_user_login_rs.room_id()<<std::endl;
	std::cout<<tmp_user_login_rs.result()<<std::endl;

	int tmp_login_result = tmp_user_login_rs.result();
	int tmp_room_id = tmp_user_login_rs.room_id();
	int tmp_user_id = tmp_user_login_rs.user_id();
	
	ChatRoom *tmp_room = tmp_server->GetRoomManager()->GetRoom(tmp_room_id);
	if(!tmp_room)
	{
		std::cout<<"Room "<<tmp_room_id<<" does not exist"<<std::endl;
		return;
	}	
	UserInfo *tmp_user = tmp_room->GetPreUserInfoList()->GetUserInfo(tmp_user_id);
	if(!tmp_user)
	{
		std::cout<<"User "<<tmp_user_id<<" have not logined"<<std::endl;
		return ;
	}

	if(tmp_login_result == 1)
	{
		UserInfo *tmp_current_user = NULL;
		int tmp_get_user = tmp_room->GetUserInfoList()->GetUnusedUser(tmp_current_user);
		if(tmp_get_user < 0)
		{
			std::cout<<"Get UserInfo failed"<<std::endl;
		}
		else
		{
			memcpy(tmp_current_user,tmp_user,sizeof(UserInfo));
			tmp_room->GetUserInfoList()->AddUserInfo(tmp_user_id,tmp_current_user);
		}
	}
	ClientInfoEx *tmp_gs = reinterpret_cast<ClientInfoEx *>(tmp_server->GetClientList()->GetUserByHashkey(tmp_user->gs_hashkey_user_on));
	if(tmp_gs)
	{
		tmp_processor->SendDataToGS(tmp_user_login_rs,tmp_user_login_rs.msg_id(),tmp_gs->user_sock,tmp_user->user_hash_key);
	}
	tmp_room->GetPreUserInfoList()->PushUserInUnusedList(tmp_user);
}
