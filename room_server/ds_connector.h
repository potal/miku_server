#ifndef DS_CONNECTOR_H_
#define DS_CONNECTOR_H_

#include "../common/connector.h"
#include "ds_processor.h"

#include "packet/cyt_packet.pb.h"
#include "packet/ds_server.pb.h"

class DirectorServerConnector : public Connector
{
private:
	void *server_ptr_;
	CircleList recv_list_;
	DirectorServerProcessor ds_processor_;
public:
	DirectorServerConnector();
	~DirectorServerConnector();

	bool StartDSProcessor(int circle_list_size,int thread_count,void *server_ptr);
	void DealWithData(char *buff,int len,int fd);

	template<class T>
	bool SendPackage(T &pack,int msg_id,int optional_fd = 0);
};

template<class T>
bool DirectorServerConnector::SendPackage(T &pack,int msg_id,int optional_fd)
{
	bool tmp_return = false;
	char tmp_pack_buff[0x1000] = {0};
	int tmp_pack_len = pack.ByteSize();
	tmp_return = pack.SerializeToArray(tmp_pack_buff,tmp_pack_len);
	if(!tmp_return)
	{
		std::cout<<"pack error!msg_id:"<<msg_id<<std::endl;
		return false;
	}
	StruCytPacket tmp_cyt_pack;
	tmp_cyt_pack.set_str_head("123");
	tmp_cyt_pack.set_msg_type(msg_id);
	tmp_cyt_pack.set_room_id(0);
	tmp_cyt_pack.set_msg_len(tmp_pack_len);
	tmp_cyt_pack.set_msg_data(tmp_pack_buff,tmp_pack_len);
	tmp_cyt_pack.set_str_tail("456");

	tmp_pack_len = tmp_cyt_pack.ByteSize();
	tmp_return = tmp_cyt_pack.SerializeToArray(tmp_pack_buff,tmp_pack_len);
	if(!tmp_return)
	{
		std::cout<<"pack strucytpacket error"<<std::endl;
		return false;
	}

	if(optional_fd == 0)
	{
		return SendData(tmp_pack_buff,tmp_pack_len);
	}
	else
	{
		int tmp_ret_len = write(optional_fd,tmp_pack_buff,tmp_pack_len);
		if(tmp_ret_len == tmp_pack_len)
			return true;
		else
			return false;
	}
}

#endif
