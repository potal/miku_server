/*
 * =====================================================================================
 *
 *       Filename:  user_send_msg_rq.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2014 11:37:41 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "user_send_msg_rq.h"
#include "../../packet/packet_analysis.h"
#include "../room_server.h"
#include "../client_info_list.h"

UserSendMsgRQ::UserSendMsgRQ(void *server_ptr)
{
	server_ptr_ = server_ptr;
}

UserSendMsgRQ::~UserSendMsgRQ()
{

}

void UserSendMsgRQ::Execute(char *buff,int len,void *caller_ptr)
{
	if(!server_ptr_ || !caller_ptr)
		return ;
	
	ClientInfoEx *tmp_gs = reinterpret_cast<ClientInfoEx *>(caller_ptr);

	GateRoomServerPack tmp_package;
	StruCytPacket tmp_cyt_pack;
	StruUserMsgRQ tmp_send_msg_rq;
	bool tmp_return = UnpackAndAnalysePackFromGSRS(buff,len,tmp_package,tmp_cyt_pack,tmp_send_msg_rq);
	if(!tmp_return)
	{
		std::cout<<"UserSendMsgRQ::Execute Unpack StruUserMsgRQ Error"<<std::endl;
		return ;
	}
	
	RoomServer *tmp_server = reinterpret_cast<RoomServer *>(server_ptr_);
	if(!tmp_server)
		return ;
	ChatRoom *tmp_room = tmp_server->GetRoomManager()->GetRoom(tmp_send_msg_rq.room_id());
	if(!tmp_room)
	{
		std::cout<<"there is no room you find"<<std::endl;
		return ;
	}

	UserInfo *tmp_user = tmp_room->GetUserInfoList()->GetUserInfo(tmp_send_msg_rq.user_id());
	if(!tmp_user)
	{
		std::cout<<"there is no user you find,user_id:"<<tmp_send_msg_rq.user_id()<<std::endl;
		return ;
	}

	StruUserMsgRS tmp_send_msg_rs;
	tmp_send_msg_rs.set_msg_id(E_USER_MSG_RS);
	tmp_send_msg_rs.set_room_id(tmp_send_msg_rq.room_id());
	tmp_send_msg_rs.set_user_id(tmp_send_msg_rq.user_id());

	if(tmp_send_msg_rq.is_private())//private msg
	{
		UserInfo *tmp_dst_user = tmp_room->GetUserInfoList()->GetUserInfo(tmp_send_msg_rq.dst_user_id());
		if(!tmp_dst_user)
		{
			std::cout<<"the one you talk with is not online!"<<std::endl;
			tmp_send_msg_rs.set_result(0);
		}
		else
		{
			tmp_send_msg_rs.set_result(1);
			StruUserMsgTextID tmp_msg_text;
			tmp_msg_text.set_msg_id(E_USER_MSG_TEXT_ID);
			tmp_msg_text.set_is_private(true);
			tmp_msg_text.set_room_id(tmp_send_msg_rq.room_id());
			tmp_msg_text.set_user_id(tmp_send_msg_rq.user_id());
			tmp_msg_text.set_dst_user_id(tmp_send_msg_rq.dst_user_id());
			tmp_msg_text.set_msg_text(tmp_send_msg_rq.msg_text());

			ClientInfoEx *tmp_dst_gs = reinterpret_cast<ClientInfoEx *>(tmp_server->GetClientList()->GetUserByHashkey(tmp_dst_user->gs_hashkey_user_on));
			if(tmp_dst_gs)
			{
				tmp_dst_gs->GetGSProcessor()->SendData(tmp_msg_text,E_USER_MSG_TEXT_ID,tmp_dst_user->user_hash_key);
			}
		}
		tmp_gs->GetGSProcessor()->SendData(tmp_send_msg_rs,E_USER_MSG_RS,tmp_user->user_hash_key);
		return;
	}
	else//send to all user
	{
		;
	}
}
