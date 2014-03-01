#include "stdafx.h"
#include "base_log.h"
#include "circle_buff.h"

CCircleBuff::CCircleBuff(void)
{
	miHeadPos = 0;
	miTailPos = 0;
	miLastTailPos = 0;
	miBuffQueueSize = 0;	

	mszTCPData = NULL;
}

CCircleBuff::~CCircleBuff(void)
{
	if (mszTCPData)
	{
		delete []mszTCPData;
		mszTCPData = NULL;
	}
}

bool CCircleBuff::Init(int aiDataBuffLen)
{
	miHeadPos = 0;
	miTailPos = 0;
	miLastTailPos = 0;

	miBuffQueueSize = aiDataBuffLen;
	miCountBuff = 0;

	if (mszTCPData)
	{
		delete []mszTCPData;
		mszTCPData = NULL;
	}
	mszTCPData = new BuffStruc[miBuffQueueSize];
	if (!mszTCPData)
	{
		return false;
	}
	for (int i = 0; i< miBuffQueueSize; i++)
	{
		mszTCPData[i].miLen = 0;
		memset(&mszTCPData[i].mszBuff,0,MaxBuffLen);
	}    
	return true;
}

void CCircleBuff::Close()
{
	miHeadPos = 0;
	miTailPos = 0;
	miLastTailPos = 0;

	miCountBuff = 0;
	miBuffQueueSize = 0;

	if (mszTCPData)
	{
		delete []mszTCPData;
		mszTCPData = NULL;
	}
}

// 清空队列
void CCircleBuff::Clear()
{
	CMAutoLock autolcok(&m_lock);
	miHeadPos = miTailPos;
	miLastTailPos = miTailPos;
	miCountBuff = 0;
}

void CCircleBuff::PushTCPData(char *apBuffer,int aiLength)
{
	if(!apBuffer || aiLength <= 0 || !mszTCPData)
		return;
	CMAutoLock autolcok(&m_lock);
	if(miCountBuff == 0 && miHeadPos == miTailPos)										// 队列为空
	{
		mszTCPData[miHeadPos].miLen = aiLength;
		memcpy(mszTCPData[miHeadPos].mszBuff,apBuffer,aiLength);
		miLastTailPos = miTailPos;
		miTailPos = (miTailPos + 1) % miBuffQueueSize;									// 使缓冲区循环利用
	}
	else																				// 队列非空
	{
		// 添加数据到队尾
		mszTCPData[miTailPos].miLen = aiLength;
		memcpy(mszTCPData[miTailPos].mszBuff,apBuffer,aiLength);
		miLastTailPos = miTailPos;
		miTailPos = (miTailPos + 1) % miBuffQueueSize;
		if(miHeadPos == miTailPos)// 队列满
		{
			WRITEERRORLOG(CCircleBuff::PushTCPData,"队列已满\n");
			miHeadPos = (miHeadPos + 1) % miBuffQueueSize;
			miCountBuff --;
		}
	}
	miCountBuff ++;
}

bool CCircleBuff::ReadTCPData(BuffStruc &aoSocketDataInfo)
{
	if(!&aoSocketDataInfo || !mszTCPData) 
		return false;
	CMAutoLock autolcok(&m_lock);
	if(miCountBuff <= 0)																// 队列为空，取不到数据
	{
		miCountBuff = 0;
		return false;
	}
	aoSocketDataInfo.miLen = mszTCPData[miHeadPos].miLen;
	memcpy(aoSocketDataInfo.mszBuff,mszTCPData[miHeadPos].mszBuff,MaxBuffLen);
	miHeadPos = (miHeadPos + 1) % miBuffQueueSize;
	miCountBuff--;
	return true;
}