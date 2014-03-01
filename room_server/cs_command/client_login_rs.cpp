//user login result from cs

#include "client_login_rs.h"
#include "../room_server.h"
#include "../cs_processor.h"
#include <map>
#include <string>

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
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","Unpack Error");
		return ;
	}

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
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","There is on room:%d\n",tmp_room_id);
		return;
	}	
	UserInfo *tmp_user = tmp_room->GetPreUserInfo(tmp_user_id);
	if(!tmp_user)
	{
		std::cout<<"User "<<tmp_user_id<<" have not logined"<<std::endl;
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","There is on user:%d\n",tmp_user_id);
		return ;
	}

	UserInfo *tmp_current_user = NULL;
	if(tmp_login_result == 1)
	{
		tmp_current_user = tmp_room->NewUserInfo();
		if(!tmp_current_user)
		{
			std::cout<<"Get UserInfo failed"<<std::endl;
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","Get new UserInfo failed");
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
		tmp_server->SendDataToRoomAllUser(tmp_user_online,E_USER_INFO,tmp_room_id);
		//send user on line to all
		//send user list to the current user
		
		int tmp_count_user = 0;
		StruUserInfoListID tmp_room_user_list;
		tmp_room_user_list.set_msg_id(E_USER_INFO_LIST_ID);
		tmp_room_user_list.set_room_id(tmp_room_id);
		tmp_room_user_list.set_user_id(tmp_user_id);
		UserInfo *tmp_list_user = tmp_room->GetUserInfoList()->GetFirstUser();
		if(tmp_list_user)
		{
			do
			{
				StruUserInfo *tmp_user_info = tmp_room_user_list.add_user_info_list();
				tmp_user_info->set_msg_id(E_USER_INFO);
				tmp_user_info->set_user_account_name(tmp_list_user->user_nickname);
				tmp_user_info->set_user_id(tmp_list_user->user_id);
				tmp_user_info->set_user_role_uid("");
				tmp_user_info->set_user_level(tmp_list_user->user_level);
				tmp_user_info->set_buyer_riches(tmp_list_user->buyer_riches);
				tmp_user_info->set_buyer_grow_up(tmp_list_user->buyer_grow_up);
				tmp_user_info->set_buyer_honour(tmp_list_user->buyer_honour);
				tmp_user_info->set_buyer_estimate(tmp_list_user->buyer_estimate);
				tmp_user_info->set_seller_income(tmp_list_user->seller_income);
				tmp_user_info->set_seller_exp(tmp_list_user->seller_exp);
				tmp_user_info->set_seller_pop(tmp_list_user->seller_pop);
				tmp_user_info->set_seller_estimate(tmp_list_user->seller_estimate);

				tmp_count_user++;
				if(15 <= tmp_count_user)
				{
					tmp_processor->SendDataToGS(tmp_room_user_list,E_USER_INFO_LIST_ID,tmp_gs->user_sock,tmp_current_user->user_hash_key);
					tmp_room_user_list.clear_user_info_list();
					tmp_count_user = 0;
				}
				tmp_list_user = tmp_room->GetUserInfoList()->GetNextUser();
			}while(tmp_list_user);
			if(tmp_count_user > 0)
			{
				tmp_processor->SendDataToGS(tmp_room_user_list,E_USER_INFO_LIST_ID,tmp_gs->user_sock,tmp_current_user->user_hash_key);
			}
		}
	}
	tmp_room->RemovePreUser(tmp_user_id);
	//send room mic list to user
	std::map<int,std::string> tmp_mic_list;
	tmp_room->GetMicUserList(tmp_mic_list);
	if(tmp_mic_list.size() > 0)
	{
		StruRoomMicInfoID tmp_room_mic_user_info_list;
		tmp_room_mic_user_info_list.set_msg_id(E_ROOM_MIC_INFO_ID);
		tmp_room_mic_user_info_list.set_room_id(tmp_room->GetRoomID());
		tmp_room_mic_user_info_list.set_user_id(tmp_current_user->user_id);
		
		std::map<int,std::string>::iterator tmp_iter;
		for(tmp_iter = tmp_mic_list.begin();tmp_iter != tmp_mic_list.end();tmp_iter++)
		{
			int tmp_mic_user_id = tmp_iter->first;
			UserInfo *tmp_mic_user = tmp_room->GetUserInfo(tmp_mic_user_id);
			if(tmp_mic_user)
			{
				StruMicInfo *tmp_mic_info = tmp_room_mic_user_info_list.add_mic_list();
				tmp_mic_info->set_msg_id(E_MIC_INFO);
				tmp_mic_info->set_user_id(tmp_mic_user->user_id);
				tmp_mic_info->set_user_account_name(tmp_mic_user->user_nickname);
				tmp_mic_info->set_mic_channel_str(tmp_iter->second);
			}
		}
		tmp_processor->SendDataToGS(tmp_room_mic_user_info_list,E_ROOM_MIC_INFO_ID,tmp_gs->user_sock,tmp_current_user->user_hash_key);
	}
}
