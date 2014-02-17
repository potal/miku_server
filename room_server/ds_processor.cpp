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
#include "ds_connector.h"

#include "packet/ds_server.pb.h"
#include "packet/cyt_packet.pb.h"
#include "ds_command/query_chat_room_rs.h"
#include "ds_command/connect_director_server_rs.h"
#include "ds_command/get_cs_addr_rs.h"

DirectorServerProcessor::DirectorServerProcessor():is_started_(false),thread_count_(0),thread_id_ptr_(NULL),parent_ptr_(NULL)
{
	
}

DirectorServerProcessor::~DirectorServerProcessor()
{
	if(thread_id_ptr_)
	{
		delete []thread_id_ptr_;
		thread_id_ptr_ = NULL;
	}
}

bool DirectorServerProcessor::InitProcessor(int max_list_size,void *caller_ptr,void *server_ptr)
{
	BaseProcessor::InitProcessor(max_list_size,server_ptr);
	parent_ptr_ = caller_ptr;

	CommandChain *tmp_cmd_chain = GetCmdChain();
	bool tmp_ret = false;
	tmp_ret = tmp_cmd_chain->RegisterCommand(DEF_DS_SERVER_CONNECT_RS,new ConnectDirectorServerRS(server_ptr));
	if(!tmp_ret)
	{
		std::cout<<"Register ConnectDirectorServerRS Error"<<std::endl;
		return false;
	}
	tmp_ret = tmp_cmd_chain->RegisterCommand(DEF_DS_GS_GET_ROOMINFO_RS,new QueryChatRoomRS(server_ptr));
	if(!tmp_ret)
	{
		std::cout<<"Register QueryChatRoom Error!"<<std::endl;
		return false;
	}
	tmp_ret = tmp_cmd_chain->RegisterCommand(DEF_DS_RS_GET_CS_RS,new GetCenterServerAddrRS(server_ptr));
	if(!tmp_ret)
	{
		std::cout<<"Register GetCenterServerAddrRS Error!"<<std::endl;
		return false;
	}

	return true;
}

bool DirectorServerProcessor::StartProcessor(int thread_count)
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
	std::cout<<"Create "<<i+1<<" thread(s) ok!"<<std::endl;
	SendConnectDsRQ();
	return true;
}

void DirectorServerProcessor::StopProcessor()
{
	is_started_ = false;
	for(int i = 0;i < thread_count_;i++)
	{
		pthread_join(thread_id_ptr_[i],NULL);
	}
	GetCmdChain()->ReleaseAllCommand();
}

void *DirectorServerProcessor::GetParent()
{
	return parent_ptr_;
}

void DirectorServerProcessor::SendConnectDsRQ()
{
	StruDsServerConnectRq tmp_conn_rq;
	tmp_conn_rq.set_server_id(1);//here server id from configuration file
	tmp_conn_rq.set_server_ip("192.168.220.128");
	tmp_conn_rq.set_server_port(5555);
	tmp_conn_rq.set_server_type(SERVER_TYPE_RS);
	tmp_conn_rq.set_connect_type(StruDsServerConnectRq_ConnectType_CONNECT_TYPE_NEWCONNECT);
	tmp_conn_rq.set_server_detect_port(0);

	char tmp_pack_buff[0x1000] = {0};
	int tmp_pack_len = tmp_conn_rq.ByteSize();
	bool tmp_ret = tmp_conn_rq.SerializeToArray(tmp_pack_buff,tmp_pack_len);
	if(!tmp_ret)
	{
		std::cout<<"Pack connect ds error!"<<std::endl;
		return;
	}

	StruCytPacket tmp_cyt_pack;
	tmp_cyt_pack.set_str_head("123");
	tmp_cyt_pack.set_room_id(0);
	tmp_cyt_pack.set_msg_len(tmp_pack_len);
	tmp_cyt_pack.set_msg_type(DEF_DS_SERVER_CONNECT_RQ);
	tmp_cyt_pack.set_msg_data(tmp_pack_buff,tmp_pack_len);
	tmp_cyt_pack.set_str_tail("456");

	tmp_pack_len = tmp_cyt_pack.ByteSize();
	tmp_ret = tmp_cyt_pack.SerializeToArray(tmp_pack_buff,tmp_pack_len);
	if(!tmp_ret)
	{
		std::cout<<"Pack cyt_pack error!"<<std::endl;
		return;
	}
	DirectorServerConnector *tmp_ds_conn = reinterpret_cast<DirectorServerConnector *>(parent_ptr_);
	if(!tmp_ds_conn)
	{
		std::cout<<"DS connector error!"<<std::endl;
		return ;
	}
	tmp_ds_conn->SendData(tmp_pack_buff,tmp_pack_len);
}

void *DirectorServerProcessor::DealWithDataThread(void *arg)
{
	std::cout<<"DirectorServerProcessor::DealWithDataThread starts"<<std::endl;
	DirectorServerProcessor *tmp_processor = reinterpret_cast<DirectorServerProcessor *>(arg);
	if(!tmp_processor)
		return NULL;

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
			continue;
		}

		int tmp_msg_type = tmp_pack_cyt_pack.msg_type();

		BaseCommand *tmp_cmd = tmp_processor->GetCmdChain()->GetCommand(tmp_msg_type);
		if(!tmp_cmd)
		{
			std::cout<<"Get cmd error!"<<std::endl;
			usleep(5000);
			continue;
		}
		tmp_cmd->Execute(const_cast<char *>(tmp_pack_cyt_pack.msg_data().c_str()),tmp_pack_cyt_pack.msg_len(),tmp_processor);
	}
	std::cout<<"DirectorServerProcessor::DealWithDataThread ends"<<std::endl;
	return NULL;
}
