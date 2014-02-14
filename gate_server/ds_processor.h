#ifndef DS_PROCESSOR_H_
#define DS_PROCESSOR_H_

#include "../common/base_processor.h"
#include "ds_connector.h"

class DirectorProcessor : public BaseProcessor
{
private:
	bool is_started_;
public:
	DirectorProcessor();
	~DirectorProcessor();

	bool InitProcessor(int max_list_size,void *caller_ptr);
	bool StartProcessor(int thread_count);

	static void* DealWithDataThread(void *arg);
};

#endif
