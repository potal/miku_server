#include "stdafx.h"
#include "dispatchers.h"

CDispatchers::CDispatchers(void)
{
}

CDispatchers::~CDispatchers(void)
{

}

//��ʼ��
bool CDispatchers::Initialize(unsigned int uiCount)
{
	//ȷ��������һ���ַ��߳�
	unsigned int uiDispatcherCount = 0 == uiCount ? 1 : uiCount; 

	//��ʼ���ַ��߳�
	m_Dispatchers.reserve(uiDispatcherCount);	
	for (int i = 0; i < uiDispatcherCount; i++)
	{		
		CDispatcher dispatcher(i);
		m_Dispatchers.push_back(dispatcher);		
	}

	//�����ַ��߳�
	return Active();	
}

//�����������߳�
bool CDispatchers::Active()
{
	for (unsigned int i = 0; i < m_Dispatchers.size(); i++)
	{
		m_Dispatchers[i].Active();
	}

	return true;
}

//ֹͣ����
bool CDispatchers::Terminate()
{
	for (unsigned int i = 0; i < m_Dispatchers.size(); i++)
	{
		m_Dispatchers[i].Terminate();
	}

	return true;
}

//������
bool CDispatchers::LoadObject(CDispatchClient* pClient,char* pValue)
{
	CMAutoLock autolock(&m_lock);
	if (NULL == pClient)
	{
		return false;
	}

	//ѡ����صķַ��߳�(�õ�ӵ�������ٵķַ��߳�ID)
	unsigned int uiLoadDispatcherID = 0;
	for (unsigned int i = 1; i < m_Dispatchers.size(); i++)
	{
		if (m_Dispatchers[uiLoadDispatcherID].size() > m_Dispatchers[i].size())
		{
			uiLoadDispatcherID = i;
		}
	}

	//���᳡���ص��ַ��߳���
	CDispatcher& dispatcher = m_Dispatchers[uiLoadDispatcherID];

	return dispatcher.LoadSession(pClient,pValue);
}

//ж����
bool CDispatchers::UnLoadObject(CDispatchClient* pClient,char* pValue)
{
	CMAutoLock autolock(&m_lock);

	if (NULL == pClient)
	{
		return false;
	}

	//�ҵ��û᳡��Ӧ�ķַ��߳�
	int iDispatcherID = pClient->GetDispatcherID();	

	//����Ƿ�Խ��
	if(m_Dispatchers.size() <= iDispatcherID || iDispatcherID < 0)
	{
		return false;
	}

	CDispatcher& dispatcher = m_Dispatchers[iDispatcherID];

	//�����û᳡���߳�ж�ظû᳡
	return dispatcher.UnloadSession(pClient,pValue);
}