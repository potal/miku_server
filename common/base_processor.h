#ifndef BASE_PROCESSOR_H_
#define BASE_PROCESSOR_H_

#include "command_chain.h"
#include "circle_list.h"

class BaseProcessor
{
private:
	void *caller_ptr_;
	CommandChain cmd_chain_;
	CircleList data_list_;
public:
	BaseProcessor();
	virtual ~BaseProcessor();

	virtual bool InitProcessor(int max_list_size,void *caller_ptr);
	void *GetCaller();
	CommandChain *GetCmdChain();
	CircleList *GetCircleList();
};


#endif
