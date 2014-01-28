#ifndef CIRCLE_BUFF_H_
#define CIRCLE_BUFF_H_

#include <pthread.h>
const int default_buff_len = 0x4000;
class CircleBuff
{
private:
	char * buff_;
	int buff_len_;
	char * buff_header_;
	char * buff_tail_;
	int remain_len_;

	pthread_mutex_t buff_lock_;

public:
	CircleBuff();
	CircleBuff(int buff_len);
	~CircleBuff();

public:
	bool AddBuff(char *buff,int buff_len);
	bool GetBuff(char *buff_out,int len_in,int &len_out);
	bool ResetBuff();
	void ReleaseBuff();

	/*
	 *add some functions which get each kind of data,like int,char,short and string.
	 *add those later
	 */
};

CircleBuff::CircleBuff()
{
	buff_len_ = default_buff_len;
	buff_ = new char[buff_len_];
	remain_len_ = buff_len_;
	buff_header_ = buff_;
	buff_tail_ = buff_;
	pthread_mutex_init(&buff_lock_,NULL);
}

CircleBuff::CircleBuff(int buff_len)
{
	buff_len_ = buff_len;
	buff_ = new char[buff_len_];
	remain_len_ = buff_len_;
	buff_header_ = buff_;
	buff_tail_ = buff_;
	pthread_mutex_init(&buff_lock_,NULL);
}

CircleBuff::~CircleBuff()
{
	if(buff_)
	{
		delete []buff_;
		buff_ = NULL;
	}
	pthread_mutex_destroy(&buff_lock_);
}

bool CircleBuff::AddBuff(char *buff,int buff_len)
{
	if(buff_len > buff_len_)
		return false;
	if(remain_buff_ < buff_len)
	{//if buffer fulls,option 1:return;option 2:clear first and then fill it.here is option 1
		return false;
	}
	if(buff_tail_ >= buff_header_)
	{
		int tmp_tail_end_len = static_cast<int>(buff_+buff_len_-buff_tail_);
		if(tmp_tail_end_len >= buff_len)
		{
			memcpy(buff_tail_,buff,buff_len);
			remain_len_ -= buff_len;
			buff_tail_ += buff_len;
			if(buff_tail_ == buff_+buff_len_)
			{
				buff_tail_ = buff_;
			}
			return true;
		}
		else
		{
			memcpy(buff_tail_,buff,tmp_tail_end_len);
			memcpy(buff_,buff+tmp_tail_end_len,buff_len-tmp_tail_end_len);
			buff_tail_ = buff_+buff_len-tmp_tail_end_len;
			remain_len_ -= buff_len;
			return true;
		}
	}
	else
	{
		memcpy(buff_tail_,buff,buff_len);
		buff_tail_ += buff_len;
		remain_len_ -= buff_len;
		return true;
	}
}

bool CircleBuff::GetBuff(char *buff_out,int len_in,int &len_out)
{
	if(buff_len_-remain_len_ > len_in)
	{
		len_out = len_in;
	}
	else
	{
		len_out = buff_len_-remain_len_;
	}
	return true;
}

bool CircleBuff::ResetBuff()
{
	remain_len_ = buff_len_;
	buff_header_ = buff_;
	buff_tail_ = buff_;
	return true;
}

void CircleBuff::ReleaseBuff()
{
	if(buff_)
	{
		delete []buff_;
		buff_ = NULL;
	}
}
#endif
