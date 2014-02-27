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
#include "../../packet/package_define.pb.h"
#include "../center_server.h"

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
	RoomServerProcessor *tmp_processor = reinterpret_cast<RoomServerProcessor *>(caller_ptr);
	CenterServer *tmp_server = reinterpret_cast<CenterServer *>(server_ptr_);
	
	ClientInfoEx *tmp_user = reinterpret_cast<ClientInfoEx *>(tmp_processor->GetParent());
	if(!tmp_user)
		return ;
	
	StruUserGiveGiftRQ tmp_give_gift_rq;
	bool tmp_return = tmp_give_gift_rq.ParseFromArray(buff,len);
	if(!tmp_return)
	{
		std::cout<<"UserGiveGiftRQ::Execute Unpack StruUserGiveGiftRQ Error"<<std::endl;
		return ;
	}
	
	StruUserGiveGiftRS tmp_give_gift_rs;
	tmp_give_gift_rs.set_msg_id(E_USER_GIVE_GIFT_RS);
	tmp_give_gift_rs.set_room_id(tmp_give_gift_rq.room_id());
	tmp_give_gift_rs.set_user_id(tmp_give_gift_rq.user_id());
	tmp_give_gift_rs.set_dst_user_id(tmp_give_gift_rq.dst_user_id());
	tmp_give_gift_rs.set_gift_code(tmp_give_gift_rq.gift_code());

	int tmp_result = 0;
	int tmp_sender_red_d = 0;
	int tmp_db_ret = tmp_server->GetMikuDB()->UserGiveGift(tmp_give_gift_rq.user_id(),tmp_give_gift_rq.dst_user_id(),tmp_give_gift_rq.count_gift(),tmp_give_gift_rq.gift_code(),tmp_sender_red_d,tmp_result);
	tmp_give_gift_rs.set_result(tmp_result);
	tmp_give_gift_rs.set_sender_red_d(tmp_sender_red_d);

	tmp_processor->SendDataToRS(tmp_give_gift_rs,E_USER_GIVE_GIFT_RS,tmp_user->user_sock);
}
