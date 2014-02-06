#include "command_chain.h"

CommandChain::CommandChain():caller_ptr_(NULL)
{
	command_chain_.clear();
	pthread_mutex_init(&chain_lock_,NULL);
}

CommandChain::~CommandChain()
{
	ReleaseAllCommand();
	pthread_mutex_destroy(&chain_lock_);
}

void CommandChain::Init(void *caller_ptr)
{
	caller_ptr_ = caller_ptr;
}

void *CommandChain::GetCaller()
{
	return caller_ptr_;
}

bool CommandChain::RegisterCommand(int command_index,BaseCommand *command_class)
{
	AutoLock lock(&chain_lock_);
	BaseCommand * tmp_command = command_chain_[command_index];
	if(tmp_command)
	{
		std::cout<<"Command ("<<command_index<<") already exists"<<std::endl;
		return false;
	}
	command_chain_[command_index] = command_class;
	return true;
}

BaseCommand *CommandChain::GetCommand(int command_index)
{
	AutoLock lock(&chain_lock_);
	BaseCommand * tmp_command = command_chain_[command_index];
	return tmp_command;
}

void CommandChain::ReleaseCommand(int command_index)
{
	AutoLock lock(&chain_lock_);
	BaseCommand * tmp_command = command_chain_[command_index];
	if(!tmp_command)
	{
		delete tmp_command;
		tmp_command = NULL;
	}
	command_chain_.erase(command_index);
}

void CommandChain::ReleaseAllCommand()
{
	AutoLock lock(&chain_lock_);
	std::map<int,BaseCommand *>::iterator tmp_iter;
	for(tmp_iter = command_chain_.begin();tmp_iter != command_chain_.end();tmp_iter ++)
	{
		BaseCommand * tmp_command = tmp_iter->second;
		if(!tmp_command)
		{
			delete tmp_command;
			tmp_command = NULL;
		}
	}
	command_chain_.clear();
}
