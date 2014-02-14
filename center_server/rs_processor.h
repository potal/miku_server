#ifndef CL_PROCESSOR_H_
#define CL_PROCESSOR_H_

#include "../common/base_processor.h"
#include "packet/cyt_packet.pb.h"
#include <pthread.h>

class RoomServerProcessor : public BaseProcessor
{
private:
	int thread_count_;
	pthread_t *thread_id_ptr_;
	bool is_started_;
	void *parent_ptr_;
public:
	RoomServerProcessor();
	~RoomServerProcessor();

	bool InitProcessor(int max_list_size,void *caller_ptr,void *server_ptr);
	bool StartProcessor(int thread_count);
	void StopProcessor();
	void *GetParent();

	static void* DealWithDataThread(void *arg);

	template<class T>
	bool SendDataToRS(T &pack,int msg_id,int fd);
};

template<class T>
bool RoomServerProcessor::SendDataToRS(T &pack,int msg_id,int fd)
{
	char tmp_msg_data_buff[0x2000] = {0};
	int tmp_pack_len = pack.ByteSize();
	bool tmp_return = pack.SerializeToArray(tmp_msg_data_buff,tmp_pack_len);
	if(!tmp_return)
	{
		std::cout<<"MsgID:"<<msg_id<<" pack error"<<std::endl;
		return false;
	}
	char tmp_send_buff[0x2000] = {0};
	int tmp_send_len = 0;
	StruCytPacket tmp_pack_cyt_pack;
	tmp_pack_cyt_pack.set_str_head("123");
	tmp_pack_cyt_pack.set_room_id(0);
	tmp_pack_cyt_pack.set_msg_type(msg_id);
	tmp_pack_cyt_pack.set_msg_len(tmp_pack_len);
	tmp_pack_cyt_pack.set_msg_data(tmp_msg_data_buff,tmp_pack_len);
	tmp_pack_cyt_pack.set_str_tail("456");
	tmp_send_len = tmp_pack_cyt_pack.ByteSize();
	tmp_return  = tmp_pack_cyt_pack.SerializeToArray(tmp_send_buff,tmp_send_len);
	if(!tmp_return)
	{
		std::cout<<"pack strucytpacket error"<<std::endl;
		return false;
	}
	if(write(fd,tmp_send_buff,tmp_send_len) <= 0)
	{
		std::cout<<"write error"<<std::endl;
		return false;
	}
	return true;
};

#endif
