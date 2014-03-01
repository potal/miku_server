// ���λ�����
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
	//��ʼ��
	bool Init(int aiDataBuffLen);

	//ɾ��
	void Close();

	//ʹ���п�
	void Clear();

	//��������
	void PushTCPData(char *apBuffer,int aiLength);

	//��ȡ����
	bool ReadTCPData(BuffStruc &aoSocketDataInfo);
private:

	CMLock	m_lock;

	BuffStruc *	mszTCPData;									// �����б�

	int		miBuffQueueSize;								// �������г���
	int		miCountBuff;									// ��Ч���ݸ�������

	int     miHeadPos;										// ��ͷλ��
	int     miTailPos;										// ��βλ��
	int     miLastTailPos;									// ��һ�εĶ�βλ��
};
