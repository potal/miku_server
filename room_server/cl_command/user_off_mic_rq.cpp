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

UserOffMicRQ::UserOffMicRQ(void *server_ptr)
{
	server_ptr_ = server_ptr;
}

UserOffMicRQ::~UserOffMicRQ()
{

}

void UserOffMicRQ::Execute(char *buff,int len,void *caller_ptr)
{
	
}
