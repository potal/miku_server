#include "stdafx.h"
#include "dispatchers.h"

CDispatchers::CDispatchers(void)
{
}

CDispatchers::~CDispatchers(void)
{

}

//初始化
bool CDispatchers::Initialize(unsigned int uiCount)
{
	//确保至少有一个分发线程
	unsigned int uiDispatcherCount = 0 == uiCount ? 1 : uiCount; 

	//初始化分发线程
	m_Dispatchers.reserve(uiDispatcherCount);	
	for (int i = 0; i < uiDispatcherCount; i++)
	{		
		CDispatcher dispatcher(i);
		m_Dispatchers.push_back(dispatcher);		
	}

	//启动分发线程
	return Active();	
}

//激活，开启相关线程
bool CDispatchers::Active()
{
	for (unsigned int i = 0; i < m_Dispatchers.size(); i++)
	{
		m_Dispatchers[i].Active();
	}

	return true;
}

//停止处理
bool CDispatchers::Terminate()
{
	for (unsigned int i = 0; i < m_Dispatchers.size(); i++)
	{
		m_Dispatchers[i].Terminate();
	}

	return true;
}

//加载流
bool CDispatchers::LoadObject(CDispatchClient* pClient,char* pValue)
{
	CMAutoLock autolock(&m_lock);
	if (NULL == pClient)
	{
		return false;
	}

	//选择加载的分发线程(得到拥有流最少的分发线程ID)
	unsigned int uiLoadDispatcherID = 0;
	for (unsigned int i = 1; i < m_Dispatchers.size(); i++)
	{
		if (m_Dispatchers[uiLoadDispatcherID].size() > m_Dispatchers[i].size())
		{
			uiLoadDispatcherID = i;
		}
	}

	//将会场加载到分发线程中
	CDispatcher& dispatcher = m_Dispatchers[uiLoadDispatcherID];

	return dispatcher.LoadSession(pClient,pValue);
}

//卸载流
bool CDispatchers::UnLoadObject(CDispatchClient* pClient,char* pValue)
{
	CMAutoLock autolock(&m_lock);

	if (NULL == pClient)
	{
		return false;
	}

	//找到该会场对应的分发线程
	int iDispatcherID = pClient->GetDispatcherID();	

	//检查是否越界
	if(m_Dispatchers.size() <= iDispatcherID || iDispatcherID < 0)
	{
		return false;
	}

	CDispatcher& dispatcher = m_Dispatchers[iDispatcherID];

	//包含该会场的线程卸载该会场
	return dispatcher.UnloadSession(pClient,pValue);
}