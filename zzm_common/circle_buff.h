// 环形缓冲区
#pragma once
#include "mlock.h"
const int MaxBuffLen = 0x2000;

struct BuffStruc
{
	char mszBuff[MaxBuffLen+1];
	int  miLen;

	BuffStruc()
	{
		memset(mszBuff,0,sizeof(mszBuff));
		miLen = 0;
	}
};

class CCircleBuff
{
public:
	CCircleBuff(void);
	~CCircleBuff(void);

public:
	//初始化
	bool Init(int aiDataBuffLen);

	//删除
	void Close();

	//使队列空
	void Clear();

	//保存数据
	void PushTCPData(char *apBuffer,int aiLength);

	//读取数据
	bool ReadTCPData(BuffStruc &aoSocketDataInfo);
private:

	CMLock	m_lock;

	BuffStruc *	mszTCPData;									// 数据列表

	int		miBuffQueueSize;								// 整个队列长度
	int		miCountBuff;									// 有效数据个数计数

	int     miHeadPos;										// 队头位置
	int     miTailPos;										// 队尾位置
	int     miLastTailPos;									// 上一次的队尾位置
};
