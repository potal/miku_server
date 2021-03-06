#include "cs_processor.h"
#include "../packet/package_define.pb.h"
#include "../common/base_log.h"

#include "cs_command/client_login_rs.h"
#include "cs_command/user_give_gift_rs.h"

CenterServerProcessor::CenterServerProcessor():is_started_(false),thread_count_(0),thread_id_ptr_(NULL),parent_ptr_(NULL)
{

}

CenterServerProcessor::~CenterServerProcessor()
{
	if(thread_id_ptr_)
	{
		delete []thread_id_ptr_;
		thread_id_ptr_ = NULL;
	}
}

bool CenterServerProcessor::InitProcessor(int max_list_size,void *caller_ptr,void *server_ptr)
{
	BaseProcessor::InitProcessor(max_list_size,server_ptr);
	parent_ptr_ = caller_ptr;
	CommandChain *tmp_cmd_chain = GetCmdChain();
	bool tmp_ret = false;
	tmp_ret = tmp_cmd_chain->RegisterCommand(E_USER_LOGIN_RS,new ClientLoginRS(server_ptr));
	if(!tmp_ret)
	{
		std::cout<<"Register UserLoginRS Error"<<std::endl;
		return false;
	}
	tmp_ret = tmp_cmd_chain->RegisterCommand(E_USER_GIVE_GIFT_RS,new UserGiveGiftRS(server_ptr));
	if(!tmp_ret)
	{
		std::cout<<"Register UserGiveGiftRS Error"<<std::endl;
		return false;
	}

	return true;
}

bool CenterServerProcessor::StartProcessor(int thread_count)
{
	//starting the thread of dealing with package
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
	std::cout<<"Create "<<i+1<<" CenterServerProcessor thread(s) ok!"<<std::endl;
	return true;
}

void CenterServerProcessor::StopProcessor()
{
	is_started_ = false;
	for(int i = 0;i < thread_count_;i++)
	{
		pthread_join(thread_id_ptr_[i],NULL);
	}
	GetCmdChain()->ReleaseAllCommand();
}

void *CenterServerProcessor::GetParent()
{
	return parent_ptr_;
}
void* CenterServerProcessor::DealWithDataThread(void *arg)
{
	std::cout<<"CenterServerProcessor::DealWithDataThread starts"<<std::endl;
	CenterServerProcessor *tmp_processor = reinterpret_cast<CenterServerProcessor *>(arg);
	if(!tmp_processor)
	{
		std::cout<<"tmp_processor NULL"<<std::endl;
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"DealWithDataThread","tmp_processor NULL");
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

		StruCytPacket tmp_pack_cyt_pack;
		tmp_return = tmp_pack_cyt_pack.ParseFromArray(tmp_out_buff,tmp_out_len);
		if(!tmp_return)
		{
			std::cout<<"Unpack package error!"<<std::endl;
			WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"DealWithDataThread","Unpack Error");
			continue;
		}

		int tmp_msg_type = tmp_pack_cyt_pack.msg_type();

		BaseCommand *tmp_cmd = tmp_processor->GetCmdChain()->GetCommand(tmp_msg_type);
		if(!tmp_cmd)
		{
			std::cout<<"Get cmd error!"<<std::endl;
			WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"DealWithDataThread","Get cmd Error");
			usleep(5000);
			continue;
		}
		tmp_cmd->Execute(const_cast<char *>(tmp_pack_cyt_pack.msg_data().c_str()),tmp_pack_cyt_pack.msg_len(),tmp_processor);
	}
	std::cout<<"CenterServerProcessor::DealWithDataThread ends"<<std::endl;
	return NULL;
}
