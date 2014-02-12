#ifndef CL_PROCESSOR_H_
#define CL_PROCESSOR_H_

#include "../common/base_processor.h"
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

	static void* DealWithDataThread(void *arg);
};

#endif
