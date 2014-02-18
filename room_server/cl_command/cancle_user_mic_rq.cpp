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

CancleUserMicRQ::CancleUserMicRQ(void *server_ptr)
{
	server_ptr_ = server_ptr;
}

CancleUserMicRQ::~CancleUserMicRQ()
{

}

void CancleUserMicRQ::Execute(char *buff,int len,void *caller_ptr)
{
	
}
