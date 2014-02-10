#ifndef CIRCLE_BUFF_H_
#define CIRCLE_BUFF_H_

#include <string.h>
#include <pthread.h>
#include "auto_lock.h"

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

#endif
