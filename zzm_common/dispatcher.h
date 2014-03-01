#pragma once
#include <vector>
#include "munit.h"
#include "mlock.h"
using namespace std;


class CDispatchClient
{
public:
	CDispatchClient(void)
	{
		m_iDispatchID = -1;
	}
	virtual ~CDispatchClient(void)
	{

	}

public:
	virtual bool CheckBusiness(void* pValue)=0;

	void SetDispatcherID(int iDispatchID){m_iDispatchID = iDispatchID;};
	int GetDispatcherID(){return m_iDispatchID;};
private:
	int m_iDispatchID;
};


struct StuDispatchItem
{
	CDispatchClient* m_pClient;
	void* m_pValue;

	StuDispatchItem()
	{
		m_pClient = NULL;
		m_pValue = NULL;
	}

	bool CheckBusiness()
	{
		if (NULL == m_pClient)
		{
			return false;
		}

		return m_pClient->CheckBusiness(m_pValue);
	}
};

class CDispatcher
{
public:
	CDispatcher(unsigned int uiDispatcherID);

	CDispatcher(const CDispatcher& src);

	CDispatcher& operator=(const CDispatcher& src);

	virtual ~CDispatcher(void);

public:
	//������
	void CheckOutput();

public:
	//�����ַ��߳�
	bool Active();

	//ֹͣ�ַ��߳�
	bool Terminate();

	// ����һ����
	bool LoadSession(CDispatchClient* pClient,char* pValue);

	// ж��һ��
	bool UnloadSession(CDispatchClient* plClient,char* pValue);

	// ���ñ��ַ��̵߳ı��
	void SetDispatcherID(unsigned int uiDispatcherID);

	// �õ�ӵ�еĻ᳡����
	unsigned int size();

private:
	vector<StuDispatchItem> m_outputvect;

	// ����߳���
	CMLock m_lockoutput;

	//��ID
	unsigned int m_uiDispatcherID;

	//�Ƿ�ֹͣ�߳�
	bool m_bRunning;	

	//�������߳�
	MThreadID m_OutputThread;
};
