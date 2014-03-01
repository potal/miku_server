#pragma once
#include "dispatcher.h"
#include "mlock.h"

class CDispatchers
{
public:
	CDispatchers(void);
	virtual ~CDispatchers(void);

public:
	//��ʼ��
	bool Initialize(unsigned int uiCount);

	//�����������߳�
	bool Active();

	//ֹͣ����
	bool Terminate();

	//���ػ᳡
	bool LoadObject(CDispatchClient* pClient,char* pValue);

	//ж�ػ᳡
	bool UnLoadObject(CDispatchClient* pClient,char* pValue);

private:
	//�ַ��߳���
	vector<CDispatcher> m_Dispatchers;
	
	CMLock m_lock;

};
