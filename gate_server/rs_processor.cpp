#include "rs_processor.h"
#include "../packet/package_define.pb.h"
#include "../packet/gs_rs_packet.pb.h"
#include "../common/base_log.h"
#include "rs_connector.h"
#include "gate_server.h"

RoomServerProcessor::RoomServerProcessor():is_started_(false),thread_count_(0),thread_id_ptr_(NULL),parent_ptr_(NULL),valid_buff_len_(0)
{
	deal_buff_ = new char[MaxDealBuffLen];
}

RoomServerProcessor::~RoomServerProcessor()
{
	if(thread_id_ptr_)
	{
		delete []thread_id_ptr_;
		thread_id_ptr_ = NULL;
	}
	if(deal_buff_)
	{
		delete []deal_buff_;
		deal_buff_ = NULL;
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
	GateServer *tmp_gs = reinterpret_cast<GateServer *>(tmp_processor->GetCaller());
	bool tmp_return = false;
	while(tmp_processor->is_started_)
	{
		int tmp_out_len = 0;
		if(!tmp_processor->GetCircleList()->GetBuffer(tmp_processor->deal_buff_+tmp_processor->valid_buff_len_,MaxDealBuffLen-tmp_processor->valid_buff_len_,tmp_out_len))
		{
			usleep(5000);
			continue;
		}
		std::cout<<"RSP:DealWithDataThread: Buff_len:"<<tmp_out_len<<std::endl;
		tmp_processor->valid_buff_len_ += tmp_out_len;

		GateRoomServerPack tmp_rg_pack;
		while(true)
		{
			if(tmp_processor->valid_buff_len_ <= 18)
				break;

			tmp_return = tmp_rg_pack.ParseFromArray(tmp_processor->deal_buff_,tmp_processor->valid_buff_len_);
			if(!tmp_return)
			{
				break;
			}
			int tmp_this_pack_len = tmp_rg_pack.ByteSize();
			tmp_processor->valid_buff_len_ -= tmp_this_pack_len;
			memmove(tmp_processor->deal_buff_,tmp_processor->deal_buff_+tmp_this_pack_len,tmp_processor->valid_buff_len_);

			UserInfoEx *tmp_user = reinterpret_cast<UserInfoEx *>(tmp_gs->GetClientInfoList()->GetUserByHashkey(tmp_rg_pack.user_hashkey()));
			if(tmp_user)
			{
				write(tmp_user->user_sock,tmp_rg_pack.data_msg().c_str(),tmp_rg_pack.data_len());
			}
			else
			{
				std::cout<<"RSP:DealWithDataThread:not find user:"<<tmp_rg_pack.user_hashkey()<<std::endl;
			}
		}
	}
	std::cout<<"RoomServerProcessor::DealWithDataThread ends"<<std::endl;
	return NULL;
}
