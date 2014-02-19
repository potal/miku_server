#include "rs_processor.h"
#include "../packet/package_define.pb.h"
#include "rs_connector.h"
#include "gate_server.h"

RoomServerProcessor::RoomServerProcessor():is_started_(false),thread_count_(0),thread_id_ptr_(NULL),parent_ptr_(NULL)
{

}

RoomServerProcessor::~RoomServerProcessor()
{
	if(thread_id_ptr_)
	{
		delete []thread_id_ptr_;
		thread_id_ptr_ = NULL;
	}
}

bool RoomServerProcessor::InitProcessor(int max_list_size,void *caller_ptr,void *server_ptr)
{
	BaseProcessor::InitProcessor(max_list_size,server_ptr);
	parent_ptr_ = caller_ptr;
	return true;
}

bool RoomServerProcessor::StartProcessor(int thread_count)
{
	if(thread_count <= 0)
		return false;
	thread_count_ = thread_count;
	is_started_ = true;
	bool tmp_ret = false;
	thread_id_ptr_ = new pthread_t[thread_count];
	int i = 0;
	for(i = 0;i < thread_count;i ++)
	{
		tmp_ret = pthread_create(&thread_id_ptr_[i],NULL,DealWithDataThread,this);
		if(!tmp_ret)
		{
			break;
		}
	}
	std::cout<<"Create "<<i+1<<" RoomServerProcessor thread(s) ok!"<<std::endl;
	return true;
}

void RoomServerProcessor::StopProcessor()
{
	is_started_ = false;
	for(int i = 0;i < thread_count_;i++)
	{
		pthread_join(thread_id_ptr_[i],NULL);
	}
	GetCmdChain()->ReleaseAllCommand();
}

void *RoomServerProcessor::GetParent()
{
	return parent_ptr_;
}
void* RoomServerProcessor::DealWithDataThread(void *arg)
{
	std::cout<<"RoomServerProcessor::DealWithDataThread starts"<<std::endl;
	RoomServerProcessor *tmp_processor = reinterpret_cast<RoomServerProcessor *>(arg);
	if(!tmp_processor)
	{
		std::cout<<"tmp_processor NULL"<<std::endl;
		return NULL;
	}
	bool tmp_return = false;
	while(tmp_processor->is_started_)
	{
		char tmp_out_buff[0x2000] = {0};
		int tmp_out_len = 0;
		if(!tmp_processor->GetCircleList()->GetBuffer(tmp_out_buff,0x2000,tmp_out_len))
		{
			usleep(5000);
			continue;
		}
		std::cout<<"Buff:"<<tmp_out_buff<<std::endl;

		GateRoomServerPack tmp_rg_pack;
		tmp_return = tmp_rg_pack.ParseFromArray(tmp_out_buff,tmp_out_len);
		if(!tmp_return)
		{
			std::cout<<"Unpack package error!"<<std::endl;
			continue;
		}
		GateServer *tmp_gs = reinterpret_cast<GateServer *>(tmp_processor->GetCaller());
		if(tmp_gs)
		{
			UserInfoEx *tmp_user = reinterpret_cast<UserInfoEx *>(tmp_gs->GetClientInfoList()->GetUserByHashkey(tmp_rg_pack.user_hashkey()));
			if(tmp_user)
			{
				write(tmp_user->user_sock,tmp_rg_pack.data_msg().c_str(),tmp_rg_pack.data_len());
			}
		}
	}
	std::cout<<"RoomServerProcessor::DealWithDataThread ends"<<std::endl;
	return NULL;
}
