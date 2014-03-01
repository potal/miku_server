#include "stdafx.h"
#include "dispatcher.h"

CDispatcher::CDispatcher(unsigned int uiDispatcherID)
{
	m_uiDispatcherID = uiDispatcherID;
	m_bRunning = false;
	m_OutputThread = NULL;
}

CDispatcher::CDispatcher(const CDispatcher& src)
{
	m_outputvect = src.m_outputvect;
	m_uiDispatcherID = src.m_uiDispatcherID;
	m_bRunning = src.m_bRunning;
	m_OutputThread = src.m_OutputThread;
}

CDispatcher& CDispatcher::operator=(const CDispatcher& src)
{
	m_outputvect = src.m_outputvect;
	m_uiDispatcherID = src.m_uiDispatcherID;
	m_bRunning = src.m_bRunning;
	m_OutputThread = src.m_OutputThread;
	return *this;
}

CDispatcher::~CDispatcher(void)
{
	Terminate();
}

TTHREAD_FUNC_RESULT DispatchThread(TTHREAD_FUNC_PARAMETER lpParam)
{
	CDispatcher* pSer = (CDispatcher*)lpParam;
	printf("DispatchThread Thread Runing\r\n");
	if (NULL != pSer)
	{
		pSer->CheckOutput();
	}
	printf("DispatchThread Thread stop\r\n");
	return 0;
}

//启动分发线程
bool CDispatcher::Active()
{
	if (m_bRunning)
	{
		return true;
	}
	
	m_bRunning = true;

	m_OutputThread = MCreateThread(DispatchThread,(void *)this);
	if (NULL == m_OutputThread)
	{
		printf("DispatchOutputThread CreateThread Failed\r\n");

		m_bRunning = false;
		return false;
	}

	return true;
}

//停止分发线程
bool CDispatcher::Terminate()
{
	m_bRunning = false;
	
	if (NULL != m_OutputThread)
	{
		MWaitThread(m_OutputThread);
		m_OutputThread = NULL;
	}

	return true;
}

// 加载
bool CDispatcher::LoadSession(CDispatchClient* pClient,char* pValue)
{
	if (NULL == pClient)
	{
		return false;
	}

	m_lockoutput.Lock();

	pClient->SetDispatcherID(m_uiDispatcherID);

	StuDispatchItem dispatchitem;
	dispatchitem.m_pClient = pClient;
	dispatchitem.m_pValue = pValue;

	m_outputvect.push_back(dispatchitem);

	m_lockoutput.UnLock();
	
	return true;
}

// 卸载
bool CDispatcher::UnloadSession(CDispatchClient* pClient,char* pValue)
{
	m_lockoutput.Lock();
	
 	vector<StuDispatchItem>::iterator iter;
 	for (iter = m_outputvect.begin();iter!=m_outputvect.end();iter++)
 	{
 		if (iter->m_pClient == pClient && iter->m_pValue == pValue)
 		{
 			m_outputvect.erase(iter);
 			break;
 		}
 	}

	m_lockoutput.UnLock();
	
	return true;
}

// 设置本分发线程的编号
void CDispatcher::SetDispatcherID(unsigned int uiDispatcherID)
{
	m_uiDispatcherID = uiDispatcherID;
}

// 得到对象个数
unsigned int CDispatcher::size()
{
	CMAutoLock autolock(&m_lockoutput);
	return m_outputvect.size();
}

//检查输出
void CDispatcher::CheckOutput()
{
	bool bSleep = false;;
	while(m_bRunning)
	{
		bSleep = true;
		m_lockoutput.Lock();

		vector<StuDispatchItem>::iterator iter;
		for (iter = m_outputvect.begin();iter != m_outputvect.end();iter++)
		{
			if(iter->CheckBusiness())
			{
				bSleep = false;
			}
		}

		m_lockoutput.UnLock();

		if (bSleep)
		{
			MSleep(1);
		}
	}
}