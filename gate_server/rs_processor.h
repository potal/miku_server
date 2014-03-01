#ifndef CS_PROCESSOR_H_
#define CS_PROCESSOR_H_

#include "../common/base_processor.h"
#include "../packet/cyt_packet.pb.h"
#include "../packet/package_define.pb.h"
#include <pthread.h>

const int MaxDealBuffLen = 0x4000;

class RoomServerProcessor : public BaseProcessor
{
private:
	int thread_count_;
	pthread_t *thread_id_ptr_;
	bool is_started_;
	void *parent_ptr_;

	char *deal_buff_;
	int valid_buff_len_;
public:
	RoomServerProcessor();
	~RoomServerProcessor();

	bool InitProcessor(int max_list_size,void *caller_ptr,void *server_ptr);
	bool StartProcessor(int thread_count);
	void StopProcessor();
	void *GetParent();
	inline bool GetStatus(){return is_started_;}

	static void* DealWithDataThread(void *arg);

};

#endif
