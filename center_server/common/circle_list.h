#ifndef CIRCLE_LIST_H_
#define CIRCLE_LIST_H_

#include <string.h>
//#include <deque>
#include "auto_lock.h"

const int MaxSizePerBuff = 0x1000;
const int DefaltMaxSize = 0;

#pragma pack(1)
struct UnitStruct
{
	char buff[MaxSizePerBuff];
	int len;
	bool has_child;			//is there any data after

	UnitStruct(){
		has_child = false;
		len = 0;
		memset(buff,0,MaxSizePerBuff);
	};
};

class CircleList
{
private:
	pthread_mutex_t list_lock_;
//	deque<UnitStruct *> using_list_;
	int max_stru_size_;
	int using_size_;
	UnitStruct * data_ptr_;
	UnitStruct * data_start_;
	UnitStruct * data_end_;
	UnitStruct * list_start_;
	UnitStruct * list_end_;
	int start_pos_index_;
	int end_pos_index_;

public:
	CircleList(int stru_size = DefaltMaxSize);
	~CircleList();

	void InitBuffer(int max_stru_size);
	bool AddBuffer(char *buff,int len);
	bool GetBuffer(char *out_buff,int max_buff_len,int &out_len);
	void ReleaseBuffer();
};

#pragma pack()
#endif
