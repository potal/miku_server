/*
 * =====================================================================================
 *
 *       Filename:  user_exit_room_id.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2014 11:40:24 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "user_exit_room_id.h"
#include "../../packet/packet_analysis.h"
#include "../room_server.h"
#include "../client_info_list.h"

UserExitRoomID::UserExitRoomID(void *server_ptr)
{
	server_ptr_ = server_ptr;
}

UserExitRoomID::~UserExitRoomID()
{

}

void UserExitRoomID::Execute(char *buff,int len,void *caller_ptr)
{
	if(!server_ptr_ || !caller_ptr)
		return;
	ClientInfoEx *tmp_gs = reinterpret_cast<ClientInfoEx *>(caller_ptr);
	RoomServer *tmp_server = reinterpret_cast<RoomServer *>(server_ptr_);
	
	GateRoomServerPack tmp_package;
	StruCytPacket tmp_cyt_pack;
	StruUserExitRoomID tmp_exit_room_id;
	bool tmp_return = UnpackAndAnalysePackFromGSRS(buff,len,tmp_package,tmp_cyt_pack,tmp_exit_room_id);
	if(!tmp_return)
	{
		std::cout<<"Unpack StruUserExitRoomID Error"<<std::endl;
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","Unpack Error");
		return ;
	}
	
	ChatRoom *tmp_room = tmp_server->GetRoomManager()->GetRoom(tmp_exit_room_id.room_id());
	if(!tmp_room)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","There is on room:%d\n",tmp_exit_room_id.room_id());
		return ;
	}

	UserInfo *tmp_user = tmp_room->GetUserInfo(tmp_exit_room_id.user_id());
	if(!tmp_user)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"Execute","There is on user:%d\n",tmp_exit_room_id.user_id());
		return ;
	}
	tmp_room->RemoveUser(tmp_exit_room_id.user_id());
	//send user_exit_room to all user
	tmp_server->SendDataToRoomAllUser(tmp_exit_room_id,E_USER_EXIT_ROOM_ID,tmp_room->GetRoomID());
}
