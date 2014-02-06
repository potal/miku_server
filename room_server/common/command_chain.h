#ifndef COMMAND_CHAIN_H_
#define COMMAND_CHAIN_H_

#include <iostream>
#include <map>
#include <pthread.h>
#include "base_command.h"
#include "auto_lock.h"

class CommandChain
{
private:
	std::map<int ,BaseCommand *> command_chain_;
	void *caller_ptr_;
	pthread_mutex_t chain_lock_;

public:
	CommandChain();
	~CommandChain();

	void Init(void *caller_ptr);
	void *GetCaller();
	bool RegisterCommand(int command_index,BaseCommand * command_class);
	BaseCommand *GetCommand(int command_index);
	void ReleaseCommand(int command_index);
	void ReleaseAllCommand();
};

#endif
