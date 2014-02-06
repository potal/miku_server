/*
 * =====================================================================================
 *
 *       Filename:  base_processor.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/07/2014 12:45:20 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "base_processor.h"

BaseProcessor::BaseProcessor():caller_ptr_(NULL)
{

}

BaseProcessor::~BaseProcessor()
{

}

void BaseProcessor::Init(int max_list_size)
{
	//data_list_.Init(max_list_size);
}

void *BaseProcessor::GetCaller()
{
	return caller_ptr_;
}

CommandChain *BaseProcessor::GetCmdChain()
{
	return &cmd_chain_;
}

CircleList *BaseProcessor::GetCircleList()
{
	return &data_list_;
}

