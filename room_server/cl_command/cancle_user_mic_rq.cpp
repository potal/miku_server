/*
 * =====================================================================================
 *
 *       Filename:  cancle_user_mic_rq.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2014 11:40:09 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "cancle_user_mic_rq.h"
#include "../../packet/packet_analysis.h"
#include "../room_server.h"
#include "../client_info_list.h"

CancleUserMicRQ::CancleUserMicRQ(void *server_ptr)
{
	server_ptr_ = server_ptr;
}

CancleUserMicRQ::~CancleUserMicRQ()
{

}

void CancleUserMicRQ::Execute(char *buff,int len,void *caller_ptr)
{
	if(!server_ptr_ || !caller_ptr)
		return ;
	ClientInfoEx *tmp_gs = reinterpret_cast<ClientInfoEx *>(caller_ptr);
	RoomServer *tmp_server = reinterpret_cast<RoomServer *>(server_ptr_);

	GateRoomServerPack tmp_package;
	StruCytPacket tmp_cyt_pack;
	StruCancleUserMicRQ tmp_cancle_mic_rq;
	bool tmp_return = UnpackAndAnalysePackFromGSRS(buff,len,tmp_package,tmp_cyt_pack,tmp_cancle_mic_rq);
	if(!tmp_return)
	{
		std::cout<<"Unpack StruCancleUserMicRQ Error"<<std::endl;
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","Unpack StruCancleUserMicRQ Error");
		return ;
	}

	ChatRoom *tmp_room = tmp_server->GetRoomManager()->GetRoom(tmp_cancle_mic_rq.room_id());
	if(!tmp_room)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","There is on room:%d\n",tmp_cancle_mic_rq.room_id());
		return ;
	}

	UserInfo *tmp_user = tmp_room->GetUserInfo(tmp_cancle_mic_rq.user_id());
	if(!tmp_user)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","There is on user:%d\n",tmp_cancle_mic_rq.user_id());
		return ;
	}
}
