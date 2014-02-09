#include "client_login_rq.h"
#include "../packet/package_define.pb.h"
#include "room_server.h"

ClientLoginRQ::ClientLoginRQ()
{

}

ClientLoginRQ::ClientLoginRQ(void *server_ptr)
{
	server_ptr_ = server_ptr;
}

ClientLoginRQ::~ClientLoginRQ()
{

}

void ClientLoginRQ::Execute(char *buff,int len,void *caller_ptr)
{
	if(!server_ptr_)
		return;
	StruUserLoginRQ tmp_user_login;
	bool tmp_return = false;
	tmp_return = tmp_user_login.ParseFromArray(buff,len);
	if(!tmp_return)
		return ;
	std::cout<<tmp_user_login.msg_id()<<std::endl;
	std::cout<<tmp_user_login.user_id()<<std::endl;
	std::cout<<tmp_user_login.room_id()<<std::endl;
	std::cout<<tmp_user_login.user_psw()<<std::endl;
	std::cout<<tmp_user_login.user_account_name()<<std::endl;

	RoomServer *tmp_server = reinterpret_cast<RoomServer *>(server_ptr_);
	if(!tmp_server)
		return ;
	ChatRoom *tmp_room = tmp_server->GetRoomManager()->GetRoom(tmp_user_login.room_id());
	if(!tmp_room)
		return ;
	UserInfo *tmp_user = NULL;
	int tmp_get_ret = tmp_room->GetPreUserInfoList()->GetUnusedUser(tmp_user);
	if(tmp_get_ret >= 0)
	{
		tmp_user->user_id = tmp_user_login.user_id();
		tmp_room->GetPreUserInfoList()->AddUserInfo(tmp_user_login.user_id(),tmp_user);
		//send login package to cs to comfirm
	}
}
