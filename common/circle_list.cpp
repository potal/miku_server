/*
 * =====================================================================================
 *
 *       Filename:  circle_list.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/06/2014 07:01:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "circle_list.h"
#include <iostream>


CircleList::CircleList(int stru_size)
{
	pthread_mutex_init(&list_lock_,NULL);
	data_ptr_ = data_start_ = data_end_ = list_start_ = list_end_ = NULL;
	max_stru_size_ = stru_size;
	using_size_ = 0;
	start_pos_index_ = 0;
	end_pos_index_ = 0;
	if(stru_size > 0)
	{
		data_ptr_ = new UnitStruct[stru_size];
		list_start_ = list_end_ = data_ptr_;
		data_start_ = data_ptr_;
		data_end_ = data_ptr_ + stru_size - 1;
	}
}

CircleList::~CircleList()
{
	if(data_ptr_)
	{
		delete []data_ptr_;
		data_ptr_ = NULL;
		list_start_ = NULL;
		list_end_ = NULL;
		data_start_ = NULL;
		data_end_ = NULL;
	}
	pthread_mutex_destroy(&list_lock_);
}

void CircleList::InitBuffer(int max_stru_size)
{
	AutoLock tmp_lock(&list_lock_);
	if(data_ptr_)
	{
		delete []data_ptr_;
		data_ptr_ = NULL;
	}
	max_stru_size_ = max_stru_size;
	data_ptr_ = new UnitStruct[max_stru_size];
	list_start_ = list_end_ = data_start_ = data_ptr_;
	data_end_ = data_start_ + max_stru_size - 1;
	using_size_ = 0;
	start_pos_index_ = 0;
	end_pos_index_ = 0;
}

bool CircleList::AddBuffer(char *buff,int len)
{
	AutoLock tmp_lock(&list_lock_);

	if(!data_ptr_)
		return false;
	//if(data_start_ <= list_start_ && list_start_ <= list_end_)
	if(len > MaxSizePerBuff)//when one structure could not store the buffer over
	{
		int tmp_apart_pack_count = len/MaxSizePerBuff;
		int tmp_last_little_pack_len = len%MaxSizePerBuff;
		if(0 != tmp_last_little_pack_len)
		{
			tmp_apart_pack_count ++;
		}
		if(max_stru_size_ - using_size_ < tmp_apart_pack_count)
		{
			return false;
		}
		for(int i = 0;i < tmp_apart_pack_count;i++)
		{
			if(i < tmp_apart_pack_count - 1)
			{
				data_ptr_[end_pos_index_].len = MaxSizePerBuff;
				memcpy(data_ptr_[end_pos_index_].buff,buff,MaxSizePerBuff);
				data_ptr_[end_pos_index_].has_child = true;
			}
			else if(tmp_last_little_pack_len == 0)
			{
				data_ptr_[end_pos_index_].len = MaxSizePerBuff;
				memcpy(data_ptr_[end_pos_index_].buff,buff,MaxSizePerBuff);
				data_ptr_[end_pos_index_].has_child = false;
			}
			else
			{
				data_ptr_[end_pos_index_].len = tmp_last_little_pack_len;
				memcpy(data_ptr_[end_pos_index_].buff,buff,tmp_last_little_pack_len);
				data_ptr_[end_pos_index_].has_child = false;
			}
			end_pos_index_ ++;
			if(end_pos_index_ >= max_stru_size_)
			{
				end_pos_index_ = 0;
			}
			using_size_ ++;
		}
	}
	else
	{
		if((start_pos_index_ != end_pos_index_) || (using_size_ == 0))
		{
			data_ptr_[end_pos_index_].len = len;
			memcpy(data_ptr_[end_pos_index_].buff,buff,len);
			data_ptr_[end_pos_index_].has_child = false;
			end_pos_index_ ++;
			if(end_pos_index_ >= max_stru_size_)
			{
				end_pos_index_ = 0;
			}
			using_size_ ++;
			return true;
		}
		else if(start_pos_index_ == end_pos_index_ && using_size_ > 0)//buffer full
		{
			return false;
		}
	}
	return false;
}

bool CircleList::GetBuffer(char * out_buff,int max_buff_len,int &out_len)
{
	AutoLock tmp_lock(&list_lock_);

	if(!data_ptr_)
		return false;
	if(start_pos_index_ == end_pos_index_ && using_size_ == 0)//buffer empty
	{
		return false;
	}
	//check the out-buffer length
	int tmp_out_len = 0;
	int tmp_index = -1;
	int tmp_start_pos_index = start_pos_index_;
	do
	{
		tmp_index ++;
		if(tmp_start_pos_index + tmp_index == max_stru_size_)
		{
			tmp_start_pos_index = 0 - tmp_index;
		}
		if(data_ptr_[tmp_start_pos_index + tmp_index].len <= 0)
			break;
		tmp_out_len += data_ptr_[tmp_start_pos_index + tmp_index].len;
	}while(data_ptr_[tmp_start_pos_index + tmp_index].has_child);
	if(tmp_out_len >= max_buff_len)
		return false;
	int tmp_pack_count = tmp_index + 1;
	out_len = tmp_out_len;
	tmp_out_len = 0;
	tmp_index = -1;
	tmp_start_pos_index = start_pos_index_;
	do
	{
		tmp_index ++;
		if(tmp_start_pos_index + tmp_index == max_stru_size_)
		{
			tmp_start_pos_index = 0 - tmp_index;
		}
		if(data_ptr_[tmp_start_pos_index + tmp_index].len <= 0)
			break;
		memcpy(out_buff + tmp_out_len,data_ptr_[tmp_start_pos_index + tmp_index].buff,data_ptr_[tmp_start_pos_index + tmp_index].len);
		memset(data_ptr_[tmp_start_pos_index + tmp_index].buff,0,MaxSizePerBuff);
		tmp_out_len += data_ptr_[tmp_start_pos_index + tmp_index].len;
		data_ptr_[tmp_start_pos_index + tmp_index].len = 0;
	}while(data_ptr_[tmp_start_pos_index + tmp_index].has_child);
	start_pos_index_ = tmp_start_pos_index + tmp_index + 1;
	if(start_pos_index_ == max_stru_size_)
		start_pos_index_ = 0;
	using_size_ -= tmp_pack_count;
	return true;
}

void CircleList::ReleaseBuffer()
{
	AutoLock tmp_lock(&list_lock_);

	using_size_ = 0;
	data_start_ = list_start_ = list_end_ = data_ptr_;
	start_pos_index_ = end_pos_index_ = 0;
}


