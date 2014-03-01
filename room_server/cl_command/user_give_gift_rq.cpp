/*
 * =====================================================================================
 *
 *       Filename:  user_give_gift_rq.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2014 11:37:56 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "user_give_gift_rq.h"
#include "../../packet/packet_analysis.h"
#include "../room_server.h"
#include "../client_info_list.h"

UserGiveGiftRQ::UserGiveGiftRQ(void *server_ptr)
{
	server_ptr_ = server_ptr;
}

UserGiveGiftRQ::~UserGiveGiftRQ()
{

}

void UserGiveGiftRQ::Execute(char *buff,int len,void *caller_ptr)
{
	if(!server_ptr_ || !caller_ptr)
		return;
	ClientInfoEx *tmp_gs = reinterpret_cast<ClientInfoEx *>(caller_ptr);
	RoomServer *tmp_server = reinterpret_cast<RoomServer *>(server_ptr_);
	
	GateRoomServerPack tmp_package;
	StruCytPacket tmp_cyt_pack;
	StruUserGiveGiftRQ tmp_give_gift_rq;
	bool tmp_return = UnpackAndAnalysePackFromGSRS(buff,len,tmp_package,tmp_cyt_pack,tmp_give_gift_rq);
	if(!tmp_return)
	{
		std::cout<<"Unpack StruUserGiveGiftRQ Error"<<std::endl;
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","Unpack Error");
		return ;
	}
	
	ChatRoom *tmp_room = tmp_server->GetRoomManager()->GetRoom(tmp_give_gift_rq.room_id());
	if(!tmp_room)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","There is on room:%d\n",tmp_give_gift_rq.room_id());
		return ;
	}

	UserInfo *tmp_user = tmp_room->GetUserInfo(tmp_give_gift_rq.user_id());
	if(!tmp_user)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","There is on user:%d\n",tmp_give_gift_rq.user_id());
		return ;
	}
	UserInfo *tmp_dst_user = tmp_room->GetUserInfo(tmp_give_gift_rq.dst_user_id());
	if(!tmp_dst_user)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","There is on dst_user:%d\n",tmp_give_gift_rq.dst_user_id());
		return;
	}
	tmp_server->GetCSConnector()->SendData(const_cast<char *>(tmp_package.data_msg().c_str()),tmp_package.data_len());
}
