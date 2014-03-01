#include "mlock.h"

CMLock::CMLock(void)
{
#ifdef _WIN32
	InitializeCriticalSection(&m_section);
#else
	pthread_mutex_init(&lock, NULL);
#endif	
}

CMLock::~CMLock(void)
{
#ifdef _WIN32
	 
	 DeleteCriticalSection(&m_section);
#else
	pthread_mutex_destroy(&lock);
#endif
	
}

void CMLock::Lock()
{
#ifdef _WIN32
	EnterCriticalSection(&m_section);
#else
	pthread_mutex_lock(&lock);
#endif
}

void CMLock::UnLock()
{
#ifdef _WIN32
	LeaveCriticalSection(&m_section);
#else
	pthread_mutex_unlock(&lock);
#endif
}


CMAutoLock::CMAutoLock(CMLock* lock)
{
	m_pLock = lock;
	if (NULL != m_pLock)
	{
		m_pLock->Lock();
	}
}

CMAutoLock::~CMAutoLock(void)
{
	if (NULL != m_pLock)
	{
		m_pLock->UnLock();
	}
}
