/*
 * =====================================================================================
 *
 *       Filename:  ds_processor.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/11/2014 12:31:29 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ds_processor.h"
#include "gate_server.h"

DirectorProcessor::DirectorProcessor():is_started_(false)
{
	
}

DirectorProcessor::~DirectorProcessor()
{

}

bool DirectorProcessor::InitProcessor(int max_list_size,void *caller_ptr)
{
	BaseProcessor::InitProcessor(max_list_size,caller_ptr);
	CommandChain *tmp_cmd_chain = GetCmdChain();
	bool tmp_ret = false;
//	tmp_ret = tmp_cmd_chain->RegisterCommand(E_USER_LOGIN_RQ,new ClientLoginRQ(caller_ptr));
//	if(!tmp_ret)
//	{
//		std::cout<<"Register UserLoginRQ Error"<<std::endl;
//		return false;
//	}

	return true;
}

bool DirectorProcessor::StartProcessor(int thread_count)
{
	is_started_ = true;
	pthread_t tmp_thread_id = 0;
	bool tmp_create_ret = false;
	int tmp_thread_num = 0;
	for(int i = 0;i < thread_count;i++)
	{
		tmp_create_ret = pthread_create(&tmp_thread_id,NULL,DealWithDataThread,this);
		if(!tmp_create_ret)
		{
			continue;
		}
		tmp_thread_num++;
	}
	std::cout<<"Create "<<tmp_thread_num<<" thread(s) OK!"<<std::endl;
	return true;
}

void *DirectorProcessor::DealWithDataThread(void *arg)
{
	DirectorProcessor *tmp_processor = reinterpret_cast<DirectorProcessor *>(arg);
	if(!tmp_processor)
		return NULL;
	CircleList *tmp_recv_list = &(reinterpret_cast<GateServer *>(tmp_processor->GetCaller())->GetDSConnector()->recv_list_);
	while (tmp_processor->is_started_)
	{
		sleep(3);
	}
	return NULL;
}
