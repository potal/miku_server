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

PutUserOnMicRQ::PutUserOnMicRQ(void *server_ptr)
{
	server_ptr_ = server_ptr;
}

PutUserOnMicRQ::~PutUserOnMicRQ()
{

}

void PutUserOnMicRQ::Execute(char *buff,int len,void *caller_ptr)
{
	
}
