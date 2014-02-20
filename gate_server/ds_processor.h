#ifndef DS_PROCESSOR_H_
#define DS_PROCESSOR_H_

#include "../common/base_processor.h"

class DirectorServerProcessor : public BaseProcessor
{
private:
	bool is_started_;
	int thread_count_;
	pthread_t *thread_id_ptr_;
	void *parent_ptr_;
public:
	DirectorServerProcessor();
	~DirectorServerProcessor();

	bool InitProcessor(int max_list_size,void *caller_ptr,void *server_ptr);
	bool StartProcessor(int thread_count);
	void StopProcessor();
	void *GetParent();
	inline bool GetStatus(){return is_started_;}
	void SendConnectDsRQ();
	void SendQueryRoom(int room_id);

	static void* DealWithDataThread(void *arg);
};

#endif
