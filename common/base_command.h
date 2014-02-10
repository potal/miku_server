#ifndef BASE_COMMAND_H_
#define BASE_COMMAND_H_

#include <iostream>

class BaseCommand
{
public:
	void * server_ptr_;
public:
	BaseCommand():server_ptr_(NULL)
	{
	};
	virtual ~BaseCommand()
	{
	};
	virtual void Execute(char * buff,int len,void *caller_ptr) = 0;
};


#endif
