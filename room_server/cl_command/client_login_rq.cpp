#include "client_login_rq.h"
#include "../packet/package_define.pb.h"
#include "../packet/cyt_packet.pb.h"
#include "../room_server.h"
#include "../packet/packet_analysis.h"

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
	time_t tmp_current_time;
	time(&tmp_current_time);
	std::cout<<"ClientLoginRQ:"<<tmp_current_time<<std::endl;
	if(!server_ptr_)
		return;
	GateRoomServerPack tmp_package;
	StruCytPacket tmp_cyt_pack;
	StruUserLoginRQ tmp_login_rq;
	bool tmp_return = UnpackAndAnalysePackFromGSRS(buff,len,tmp_package,tmp_cyt_pack,tmp_login_rq);
	if(!tmp_return)
	{
		std::cout<<"Unpack StruUserLoginRQ Error"<<std::endl;
		return ;
	}
	std::cout<<tmp_login_rq.msg_id()<<std::endl;
	std::cout<<tmp_login_rq.user_id()<<std::endl;
	std::cout<<tmp_login_rq.room_id()<<std::endl;
	std::cout<<tmp_login_rq.user_psw()<<std::endl;
	std::cout<<tmp_login_rq.user_account_name()<<std::endl;

	RoomServer *tmp_server = reinterpret_cast<RoomServer *>(server_ptr_);
	if(!tmp_server)
		return ;
	ChatRoom *tmp_room = tmp_server->GetRoomManager()->GetRoom(tmp_login_rq.room_id());
	if(!tmp_room)
		return ;
	UserInfo *tmp_user = NULL;
	int tmp_get_ret = tmp_room->GetPreUserInfoList()->GetUnusedUser(tmp_user);
	if(tmp_get_ret >= 0)
	{
		tmp_user->gs_hashkey_user_on = tmp_package.gs_hashkey();
		tmp_user->user_hash_key = tmp_package.user_hashkey();
		tmp_user->user_id = tmp_login_rq.user_id();
		memcpy(tmp_user->user_nickname,tmp_login_rq.user_account_name().c_str(),tmp_login_rq.user_account_name().length());
		tmp_room->GetPreUserInfoList()->AddUserInfo(tmp_login_rq.user_id(),tmp_user);
		std::cout<<"Add User to pre-user-list OK.send_len:"<<tmp_package.data_len()<<std::endl;
		//send login package to cs to comfirm
		tmp_server->GetCSConnector()->SendData(const_cast<char *>(tmp_package.data_msg().c_str()),tmp_package.data_len());
	}
}
