#ifndef CL_PROCESSOR_H_
#define CL_PROCESSOR_H_

#include "../common/base_processor.h"
class ClientProcessor : public BaseProcessor
{
private:
	bool is_started_;
public:
	ClientProcessor();
	~ClientProcessor();

	bool InitProcessor(int max_list_size,void *caller_ptr);
	bool StartProcessor(int thread_count);

	static void* DealWithDataThread(void *arg);
};

#endif
