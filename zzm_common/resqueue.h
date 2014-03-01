#pragma once

#include "munit.h"
#include "base_log.h"
#include <deque>
#include <map>
using namespace std;

template <class T> 
class CResQueue
{
public:
	CResQueue(void);
	virtual ~CResQueue(void);

public:
	bool Init(unsigned int iMinSize,unsigned int iIncrease=0,unsigned int iMaxSize=0);
	bool Release();
	T* Get();
	void Put(T* pData);

private:
	bool Increase(unsigned int iNum);
	bool Descend(unsigned int iNum);

private:
	CMLock m_lock;
	map<void*,void*> m_ResMap;
	deque<void*> m_ResList;
	unsigned int m_iMinSize;
	unsigned int m_iIncrease;
	unsigned int m_iMaxSize;
	DWORD m_dwMaxTime;
};


template <class T>
CResQueue<T>::CResQueue(void)
{
	m_iMaxSize = 0;
	m_iIncrease = 0;
	m_iMinSize = 0;
	m_dwMaxTime = 0;
}

template <class T>
CResQueue<T>::~CResQueue(void)
{
	Release();
}

template <class T>
bool CResQueue<T>::Init(unsigned int iMinSize,unsigned int iIncrease,unsigned int iMaxSize)
{
	if (iMinSize < 1)
	{
		return false;
	}

	m_ResList.clear();
	m_ResMap.clear();

	if(!Increase(iMinSize))
	{
		return false;
	}

	m_iMinSize = iMinSize;
	m_iIncrease = iIncrease;
	m_iMaxSize = iMaxSize;

	return true;
}


template <class T>
bool CResQueue<T>::Release()
{	
	m_lock.Lock();

	deque<void*>::iterator iter;
	
	T* pRes = NULL;
	for (iter = m_ResList.begin();iter!=m_ResList.end();iter++)
	{
		pRes = (T*)*iter;
		if (NULL != pRes)
		{
			delete pRes;
		}
	}

	map<void*,void*>::iterator itermap;
	for (itermap = m_ResMap.begin();itermap != m_ResMap.end();itermap++)
	{
		pRes = (T*)itermap->first;
		if (NULL != pRes)
		{
			delete pRes;
		}
	}

	m_ResList.clear();
	m_ResMap.clear();

	m_lock.UnLock();

	return true;
}

template <class T>
bool CResQueue<T>::Increase(unsigned int iNum)
{
	if (iNum < 0)
	{
		return false;
	}
	
	m_dwMaxTime = timeGetTime();

	T* pRes = NULL;
	for (unsigned int i=0;i<iNum;i++)
	{
		try
		{
			pRes = new T();
		}
		catch (...)
		{
			pRes = NULL;
			return false;
		}

		m_ResList.push_back(pRes);
	}

	WRITEFORMATDEBUGLOG(__THREADID__,__FILE__,__LINE__,"CResQueue<T>::Increase","m_iMinSize:%d,m_iIncrease:%d,m_iMaxSize:%d,m_ResList:%d,m_ResMap:%d",m_iMinSize,m_iIncrease,m_iMaxSize,m_ResList.size(),m_ResMap.size());

	return true;
}
template <class T>
bool CResQueue<T>::Descend(unsigned int iNum)
{
	if (iNum < 0)
	{
		return false;
	}

	if (m_ResList.size() < iNum)
	{
		return false;
	}
	
	T* pRes = NULL;
	while( iNum > 0)
	{
		pRes = NULL;
		pRes = (T*)m_ResList.front();
		m_ResList.pop_front();

		if (NULL != pRes)
		{
			delete pRes;
		}

		iNum--;
	}

	return false;
}

template <class T>
T* CResQueue<T>::Get()
{
	m_lock.Lock();

	T* pReturn = NULL;

	if (m_ResList.size() > 1)
	{
		deque<void*>::iterator iter;
		iter = m_ResList.begin();
		pReturn = (T*)*iter;
		m_ResList.erase(iter);
		m_ResMap[pReturn] = pReturn;
	}
	else 
	{
		unsigned int iCount = m_ResList.size() + m_ResMap.size();
		if (iCount < m_iMaxSize)
		{
			unsigned int iNum = m_iIncrease;
			if (iNum > (m_iMaxSize - iCount))
			{
				iNum = m_iMaxSize - iCount;
			}

			Increase(iNum);
		}

		if (m_ResList.size() > 1)
		{
			deque<void*>::iterator iter;
			iter = m_ResList.begin();
			pReturn = (T*)*iter;
			m_ResList.erase(iter);
			m_ResMap[pReturn] = pReturn;
		}
	}

	m_lock.UnLock();

	return pReturn;
}

template <class T>
void CResQueue<T>::Put(T* pData)
{
	if (NULL == pData)
	{
		return;
	}

	m_lock.Lock();

	map<void*,void*>::iterator iter;
	iter = m_ResMap.find(pData);
	if (iter != m_ResMap.end())
	{
		m_ResList.push_back(pData);
		m_ResMap.erase(pData);
	}
	
	if (m_iIncrease > 0)
	{
		DWORD dwCurrent = timeGetTime();
		unsigned int iCount = m_ResList.size();
		if (iCount >= m_iIncrease*2 && iCount >= (m_iMinSize + m_iIncrease))
		{
			DWORD dwTemp = dwCurrent - m_dwMaxTime;

			if (dwTemp > 60000)
			{
				Descend(m_iIncrease);
				m_dwMaxTime = dwCurrent;
			}
		}
		else
		{
			m_dwMaxTime = dwCurrent;
		}
	}

	m_lock.UnLock();
}

