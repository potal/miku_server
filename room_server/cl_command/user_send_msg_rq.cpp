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
#include "../packet/packet_analysis.h"
#include "../room_server.h"

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

	if(!tmp_room->GetUserInfoList()->GetUserInfo(tmp_send_msg_rq.user_id()))
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
		if(!tmp_room->GetUserInfoList()->GetUserInfo(tmp_send_msg_rq.dst_user_id()))
		{
			std::cout<<"the one you talk with is not online!"<<std::endl;
			tmp_send_msg_rs.set_result(0);
			return ;
		}
	}
}
