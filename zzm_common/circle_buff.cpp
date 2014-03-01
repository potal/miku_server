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

// ��ն���
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
	if(miCountBuff == 0 && miHeadPos == miTailPos)										// ����Ϊ��
	{
		mszTCPData[miHeadPos].miLen = aiLength;
		memcpy(mszTCPData[miHeadPos].mszBuff,apBuffer,aiLength);
		miLastTailPos = miTailPos;
		miTailPos = (miTailPos + 1) % miBuffQueueSize;									// ʹ������ѭ������
	}
	else																				// ���зǿ�
	{
		// ������ݵ���β
		mszTCPData[miTailPos].miLen = aiLength;
		memcpy(mszTCPData[miTailPos].mszBuff,apBuffer,aiLength);
		miLastTailPos = miTailPos;
		miTailPos = (miTailPos + 1) % miBuffQueueSize;
		if(miHeadPos == miTailPos)// ������
		{
			WRITEERRORLOG(CCircleBuff::PushTCPData,"��������\n");
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
	if(miCountBuff <= 0)																// ����Ϊ�գ�ȡ��������
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