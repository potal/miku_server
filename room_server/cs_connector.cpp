#include "cs_connector.h"

CenterServerConnector::CenterServerConnector():server_ptr_(NULL)
{
}

CenterServerConnector::~CenterServerConnector()
{
	cs_processor_.StopProcessor();
}

bool CenterServerConnector::StartCSProcessor(int circle_list_size,int proc_thread_count,void *server_ptr)
{
	server_ptr_ = server_ptr;
	bool tmp_return = false;
	tmp_return = cs_processor_.InitProcessor(circle_list_size,this,server_ptr);
	if(!tmp_return)
		return false;
	tmp_return = cs_processor_.StartProcessor(proc_thread_count);
	if(!tmp_return)
		return false;
	return true;
}

void CenterServerConnector::DealWithData(char *buff,int len,int fd)
{
	if(cs_processor_.GetStatus())
	{
		cs_processor_.GetCircleList()->AddBuffer(buff,len);
	}
}
