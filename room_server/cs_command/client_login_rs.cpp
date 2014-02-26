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
	time_t tmp_current_time;
	time(&tmp_current_time);
	std::cout<<"ClientLoginRS:"<<tmp_current_time<<std::endl;
	if(!server_ptr_ || !caller_ptr)
		return ;
	CenterServerProcessor *tmp_processor = reinterpret_cast<CenterServerProcessor *>(caller_ptr);
	RoomServer *tmp_server = reinterpret_cast<RoomServer *>(server_ptr_);

	StruUserLoginRS tmp_user_login_rs;
	bool tmp_return = false;
	tmp_return = tmp_user_login_rs.ParseFromArray(buff,len);
	if(!tmp_return)
		return ;

	int tmp_login_result = tmp_user_login_rs.result();
	int tmp_room_id = tmp_user_login_rs.room_id();
	int tmp_user_id = tmp_user_login_rs.user_id();
	
	std::cout<<"tmp_user_id:"<<tmp_user_id<<std::endl;
	std::cout<<"tmp_room_id:"<<tmp_room_id<<std::endl;
	std::cout<<"tmp_login_result:"<<tmp_login_result<<std::endl;
	
	ChatRoom *tmp_room = tmp_server->GetRoomManager()->GetRoom(tmp_room_id);
	if(!tmp_room)
	{
		std::cout<<"Room "<<tmp_room_id<<" does not exist"<<std::endl;
		return;
	}	
	UserInfo *tmp_user = tmp_room->GetPreUserInfo(tmp_user_id);
	if(!tmp_user)
	{
		std::cout<<"User "<<tmp_user_id<<" have not logined"<<std::endl;
		return ;
	}

	UserInfo *tmp_current_user = NULL;
	if(tmp_login_result == 1)
	{
		tmp_current_user = tmp_room->NewUserInfo();
		if(!tmp_current_user)
		{
			std::cout<<"Get UserInfo failed"<<std::endl;
		}
		else
		{
			memcpy(tmp_current_user,tmp_user,sizeof(UserInfo));
			tmp_current_user->user_red_d = tmp_user_login_rs.user_red_d();
			tmp_current_user->user_blue_d = tmp_user_login_rs.user_blue_d();
			tmp_current_user->user_level = tmp_user_login_rs.user_level();
			tmp_current_user->buyer_riches = tmp_user_login_rs.buyer_riches();
			tmp_current_user->buyer_honour = tmp_user_login_rs.buyer_honour();
			tmp_current_user->buyer_grow_up = tmp_user_login_rs.buyer_grow_up();
			tmp_current_user->buyer_estimate = tmp_user_login_rs.buyer_estimate();
			tmp_current_user->seller_income = tmp_user_login_rs.seller_income();
			tmp_current_user->seller_exp = tmp_user_login_rs.seller_exp();
			tmp_current_user->seller_pop = tmp_user_login_rs.seller_pop();
			tmp_current_user->seller_estimate = tmp_user_login_rs.seller_estimate();
			tmp_room->AddUser(tmp_user_id,tmp_current_user);
		}
	}
	ClientInfoEx *tmp_gs = reinterpret_cast<ClientInfoEx *>(tmp_server->GetClientList()->GetUserByHashkey(tmp_user->gs_hashkey_user_on));
	if(tmp_gs)
	{
		tmp_processor->SendDataToGS(tmp_user_login_rs,tmp_user_login_rs.msg_id(),tmp_gs->user_sock,tmp_user->user_hash_key);
	}
	if(tmp_login_result == 1 && tmp_current_user)
	{
		StruUserInfo tmp_user_online;
		tmp_user_online.set_msg_id(E_USER_INFO);
		tmp_user_online.set_user_id(tmp_current_user->user_id);
		tmp_user_online.set_user_account_name(tmp_current_user->user_nickname);
		tmp_user_online.set_user_role_uid("");
		tmp_user_online.set_user_level(tmp_current_user->user_level);
		tmp_user_online.set_buyer_riches(tmp_current_user->buyer_riches);
		tmp_user_online.set_buyer_honour(tmp_current_user->buyer_honour);
		tmp_user_online.set_buyer_grow_up(tmp_current_user->buyer_grow_up);
		tmp_user_online.set_buyer_estimate(tmp_current_user->buyer_estimate);
		tmp_user_online.set_seller_income(tmp_current_user->seller_income);
		tmp_user_online.set_seller_exp(tmp_current_user->seller_exp);
		tmp_user_online.set_seller_pop(tmp_current_user->seller_pop);
		tmp_user_online.set_seller_estimate(tmp_current_user->seller_estimate);
		//tmp_processor->SendDataToAllUser(tmp_user_online,E_USER_INFO,tmp_room_id);
		//send user on line to all
	}
	tmp_room->RemovePreUser(tmp_user_id);
}
