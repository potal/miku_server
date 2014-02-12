#ifndef GS_PROCESSOR_H_
#define GS_PROCESSOR_H_

#include "../common/base_processor.h"
#include <pthread.h>

class GateServerProcessor : public BaseProcessor
{
private:
	int thread_count_;
	pthread_t *thread_id_ptr_;
	bool is_started_;
	void *parent_ptr_;		//client_info
public:
	GateServerProcessor();
	~GateServerProcessor();

	bool InitProcessor(int max_list_size,void *caller_ptr,void *server_ptr);
	bool StartProcessor(int thread_count);
	void StopProcessor();

	static void* DealWithDataThread(void *arg);
};

#endif
