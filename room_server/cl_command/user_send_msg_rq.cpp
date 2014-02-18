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
		return;
	
	StruUserMsgRQ tmp_send_msg_rq;
	bool tmp_return = UnpackAndAnalysePackFromGSRS(tmp_send_msg_rq,buff,len);
	if(!tmp_return)
	{
		std::cout<<"UserSendMsgRQ::Execute Unpack StruUserMsgRQ Error"<<std::endl;
		return;
	}
}
