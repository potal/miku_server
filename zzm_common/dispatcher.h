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
	//检查输出
	void CheckOutput();

public:
	//启动分发线程
	bool Active();

	//停止分发线程
	bool Terminate();

	// 加载一个新
	bool LoadSession(CDispatchClient* pClient,char* pValue);

	// 卸载一个
	bool UnloadSession(CDispatchClient* plClient,char* pValue);

	// 设置本分发线程的编号
	void SetDispatcherID(unsigned int uiDispatcherID);

	// 得到拥有的会场个数
	unsigned int size();

private:
	vector<StuDispatchItem> m_outputvect;

	// 输出线程锁
	CMLock m_lockoutput;

	//组ID
	unsigned int m_uiDispatcherID;

	//是否停止线程
	bool m_bRunning;	

	//输出检查线程
	MThreadID m_OutputThread;
};
