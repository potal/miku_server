#ifndef CL_PROCESSOR_H_
#define CL_PROCESSOR_H_

#include "../common/base_processor.h"
#include "../common/auto_lock.h"
#include "pthread.h"
#include <map>

class ClientProcessor : public BaseProcessor
{
private:
	void *gate_server_;
	pthread_t deal_thread_id_;
	bool is_started_;
	std::map<int,int> room_list_;//uncreated room list
	pthread_mutex_t list_lock_;
public:
	ClientProcessor();
	~ClientProcessor();

	bool InitProcessor(int max_list_size,void *caller_ptr,void *server_ptr);
	bool StartProcessor();
	void StopProcessor();

	bool AddOneUndealPack(char *buff,int len,int room_id,int user_hashkey);
	void DeleteRoom(int room_id);

	static void * DealWithDataThread(void *arg);
};

#endif
