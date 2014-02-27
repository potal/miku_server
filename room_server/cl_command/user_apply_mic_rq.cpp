/*
 * =====================================================================================
 *
 *       Filename:  user_apply_mic_rq.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2014 11:38:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "user_apply_mic_rq.h"
#include "../../packet/packet_analysis.h"
#include "../room_server.h"
#include "../client_info_list.h"

UserApplyMicRQ::UserApplyMicRQ(void *server_ptr)
{
	server_ptr_ = server_ptr;
}

UserApplyMicRQ::~UserApplyMicRQ()
{

}

void UserApplyMicRQ::Execute(char *buff,int len,void *caller_ptr)
{
	if(!server_ptr_ || !caller_ptr)
		return ;
	ClientInfoEx *tmp_gs = reinterpret_cast<ClientInfoEx *>(caller_ptr);

	GateRoomServerPack tmp_package;
	StruCytPacket tmp_cyt_pack;
	StruUserApplyMicRQ tmp_apply_mic_rq;
	bool tmp_return = UnpackAndAnalysePackFromGSRS(buff,len,tmp_package,tmp_cyt_pack,tmp_apply_mic_rq);
	if(!tmp_return)
	{
		std::cout<<"Unpack StruUserApplyMicRQ Error"<<std::endl;
		return ;
	}

	RoomServer *tmp_server = reinterpret_cast<RoomServer *>(server_ptr_);
	if(!tmp_server)
		return ;
	ChatRoom *tmp_room = tmp_server->GetRoomManager()->GetRoom(tmp_apply_mic_rq.room_id());
	if(!tmp_room)
		return ;

	UserInfo *tmp_user = tmp_room->GetUserInfo(tmp_apply_mic_rq.user_id());
	if(!tmp_user)
		return ;

	StruUserApplyMicRS tmp_apply_mic_rs;
	tmp_apply_mic_rs.set_msg_id(E_USER_APPLY_MIC_RS);
	tmp_apply_mic_rs.set_room_id(tmp_room->GetRoomID());
	tmp_apply_mic_rs.set_user_id(tmp_apply_mic_rq.user_id());
	std::string tmp_channel = tmp_room->UserApplyOneNewVideo(tmp_apply_mic_rq.user_id());
	if(tmp_channel == "")
	{
		tmp_apply_mic_rs.set_result(0);
		tmp_apply_mic_rs.set_mic_channel_str("");
		tmp_gs->GetGSProcessor()->SendData(tmp_apply_mic_rs,E_USER_APPLY_MIC_RS,tmp_user->user_hash_key);
	}
	else
	{
		tmp_apply_mic_rs.set_result(1);
		tmp_apply_mic_rs.set_mic_channel_str(tmp_channel);
		tmp_server->SendDataToRoomAllUser(tmp_apply_mic_rs,E_USER_APPLY_MIC_RS,tmp_room->GetRoomID());
	}
}
