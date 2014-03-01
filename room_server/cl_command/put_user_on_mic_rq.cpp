/*
 * =====================================================================================
 *
 *       Filename:  put_user_on_mic_rq.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2014 11:39:39 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "put_user_on_mic_rq.h"
#include "../../packet/packet_analysis.h"
#include "../room_server.h"
#include "../client_info_list.h"

PutUserOnMicRQ::PutUserOnMicRQ(void *server_ptr)
{
	server_ptr_ = server_ptr;
}

PutUserOnMicRQ::~PutUserOnMicRQ()
{

}

void PutUserOnMicRQ::Execute(char *buff,int len,void *caller_ptr)
{
	if(!server_ptr_ || !caller_ptr)
		return ;
	ClientInfoEx *tmp_gs = reinterpret_cast<ClientInfoEx *>(caller_ptr);
	RoomServer *tmp_server = reinterpret_cast<RoomServer *>(server_ptr_);

	GateRoomServerPack tmp_package;
	StruCytPacket tmp_cyt_pack;
	StruPutUserOnMicRQ tmp_put_mic_rq;
	bool tmp_return = UnpackAndAnalysePackFromGSRS(buff,len,tmp_package,tmp_cyt_pack,tmp_put_mic_rq);
	if(!tmp_return)
	{
		std::cout<<"Unpack StruPutUserOnMicRQ Error"<<std::endl;
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","Unpack StruPutUserOnMicRQ Error");
		return ;
	}

	ChatRoom *tmp_room = tmp_server->GetRoomManager()->GetRoom(tmp_put_mic_rq.room_id());
	if(!tmp_room)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","There is on room:%d\n",tmp_put_mic_rq.room_id());
		return ;
	}

	UserInfo *tmp_user = tmp_room->GetUserInfo(tmp_put_mic_rq.user_id());
	if(!tmp_user)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","There is on user:%d\n",tmp_put_mic_rq.user_id());
		return ;
	}
}
