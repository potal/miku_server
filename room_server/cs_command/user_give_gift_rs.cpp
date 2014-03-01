/*
 * =====================================================================================
 *
 *       Filename:  user_give_gift_rs.cpp
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
#include "user_give_gift_rs.h"
#include "../../packet/packet_analysis.h"
#include "../room_server.h"

UserGiveGiftRS::UserGiveGiftRS(void *server_ptr)
{
	server_ptr_ = server_ptr;
}

UserGiveGiftRS::~UserGiveGiftRS()
{

}

void UserGiveGiftRS::Execute(char *buff,int len,void *caller_ptr)
{
	if(!server_ptr_ || !caller_ptr)
		return;
	CenterServerProcessor *tmp_processor = reinterpret_cast<CenterServerProcessor *>(caller_ptr);
	RoomServer *tmp_server = reinterpret_cast<RoomServer *>(server_ptr_);

	StruUserGiveGiftRS tmp_give_gift_rs;
	bool tmp_return = tmp_give_gift_rs.ParseFromArray(buff,len);
	if(!tmp_return)
	{
		std::cout<<"Unpack StruUserGiveGiftRS Error"<<std::endl;
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","Unpack Error");
		return ;
	}
	
	ChatRoom *tmp_room = tmp_server->GetRoomManager()->GetRoom(tmp_give_gift_rs.room_id());
	if(!tmp_room)
	{
		std::cout<<"there is no room you find"<<std::endl;
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","There is on room:%d\n",tmp_give_gift_rs.room_id());
		return ;
	}

	UserInfo *tmp_user = tmp_room->GetUserInfo(tmp_give_gift_rs.user_id());
	if(!tmp_user)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","There is on user:%d\n",tmp_give_gift_rs.user_id());
		return ;
	}
	UserInfo *tmp_dst_user = tmp_room->GetUserInfo(tmp_give_gift_rs.dst_user_id());
	if(!tmp_dst_user)
	{
		return;
	}
	ClientInfoEx *tmp_gs = reinterpret_cast<ClientInfoEx *>(tmp_server->GetClientList()->GetUserByHashkey(tmp_user->gs_hashkey_user_on));
	if(tmp_give_gift_rs.result() == 0)
	{
		tmp_processor->SendDataToGS(tmp_give_gift_rs,tmp_give_gift_rs.msg_id(),tmp_gs->user_sock,tmp_user->user_hash_key);
		return;
	}
	else//send to all
	{
		tmp_user->user_red_d = tmp_give_gift_rs.sender_red_d();
		tmp_server->SendDataToRoomAllUser(tmp_give_gift_rs,tmp_give_gift_rs.msg_id(),tmp_give_gift_rs.room_id());
	}
}
