#pragma once

#ifdef _WIN32

#include "Windows.h"

#else

#include <pthread.h>

#endif	

class CMLock
{
public:
	CMLock(void);
	virtual ~CMLock(void);

public:
	void Lock();
	void UnLock();

private:
	//ÊÂ¼þ
#ifdef _WIN32
	CRITICAL_SECTION m_section;
#else
	pthread_mutex_t lock;
#endif	
};


class CMAutoLock
{
public:
	CMAutoLock(CMLock* lock);
	~CMAutoLock(void);

private:
	CMLock* m_pLock;
};
