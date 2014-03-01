#include "rs_connector.h"
#include "../common/base_log.h"

RoomServerConnector::RoomServerConnector():server_ptr_(NULL)
{
	recv_list_.InitBuffer(1000);
}

RoomServerConnector::~RoomServerConnector()
{
	recv_list_.ReleaseBuffer();
	rs_processor_.StopProcessor();
}

bool RoomServerConnector::StartRSProcessor(int max_list_size,int thread_count,void *server_ptr)
{
	server_ptr_ = server_ptr;
	bool tmp_return = false;
	tmp_return = rs_processor_.InitProcessor(max_list_size,this,server_ptr);
	if(!tmp_return)
		return false;
	tmp_return = rs_processor_.StartProcessor(thread_count);
	if(!tmp_return)
		return false;
	return true;
}

void RoomServerConnector::DealWithData(char *buff,int len,int fd)
{
	//std::cout<<"RoomServerConnector::DealWithData buff_len:"<<len<<std::endl;
	if(rs_processor_.GetStatus())
	{
		bool tmp_ret = rs_processor_.GetCircleList()->AddBuffer(buff,len);
		//std::cout<<"RoomServerConnector::DealWithData addbuffer ok"<<std::endl;
		if(!tmp_ret)
		{
			WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"DealWithData","add buffer error");
		}
	}
	else
	{
		std::cout<<"rs_processor_ is not started"<<std::endl;
	}
}
