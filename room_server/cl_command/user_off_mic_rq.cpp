/*
 * =====================================================================================
 *
 *       Filename:  user_off_mic_rq.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2014 11:39:15 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "user_off_mic_rq.h"
#include "../../packet/packet_analysis.h"
#include "../room_server.h"
#include "../client_info_list.h"

UserOffMicRQ::UserOffMicRQ(void *server_ptr)
{
	server_ptr_ = server_ptr;
}

UserOffMicRQ::~UserOffMicRQ()
{

}

void UserOffMicRQ::Execute(char *buff,int len,void *caller_ptr)
{
if(!server_ptr_ || !caller_ptr)
		return ;
	ClientInfoEx *tmp_gs = reinterpret_cast<ClientInfoEx *>(caller_ptr);

	GateRoomServerPack tmp_package;
	StruCytPacket tmp_cyt_pack;
	StruUserOffMicRQ tmp_off_mic_rq;
	bool tmp_return = UnpackAndAnalysePackFromGSRS(buff,len,tmp_package,tmp_cyt_pack,tmp_off_mic_rq);
	if(!tmp_return)
	{
		std::cout<<"Unpack StruUserOffMicRQ Error"<<std::endl;
		return ;
	}

	RoomServer *tmp_server = reinterpret_cast<RoomServer *>(server_ptr_);
	if(!tmp_server)
		return ;
	ChatRoom *tmp_room = tmp_server->GetRoomManager()->GetRoom(tmp_off_mic_rq.room_id());
	if(!tmp_room)
		return ;

	UserInfo *tmp_user = tmp_room->GetUserInfo(tmp_off_mic_rq.user_id());
	if(!tmp_user)
		return ;
	tmp_room->UserGetOffVideo(tmp_off_mic_rq.user_id());
	StruUserOffMicRS tmp_off_mic_rs;
	tmp_off_mic_rs.set_msg_id(E_USER_OFF_MIC_RS);
	tmp_off_mic_rs.set_room_id(tmp_off_mic_rq.room_id());
	tmp_off_mic_rs.set_user_id(tmp_off_mic_rq.user_id());
	tmp_off_mic_rs.set_result(1);
	tmp_server->SendDataToRoomAllUser(tmp_off_mic_rs,E_USER_OFF_MIC_RS,tmp_room->GetRoomID());
}
