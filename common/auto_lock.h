#ifndef AUTO_LOCK_H_
#define AUTO_LOCK_H_

#ifdef _WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

class AutoLock
{
private:

#ifdef _WIN32
	CRITICAL_SECTION *auto_lock_ptr_;
#else
	pthread_mutex_t *auto_lock_ptr_;
#endif

public:
#ifdef _WIN32
	AutoLock(CRITICAL_SECTION * lock_ptr)
	{
		auto_lock_ptr_ = lock_ptr;
		EnterCriticalSection(auto_lock_ptr_);
	}

	~AutoLock()
	{
		LeaveCriticalSection(auto_lock_ptr_);
	}
#else
	AutoLock(pthread_mutex_t * mutex_t)
	{
		auto_lock_ptr_ = mutex_t;
		pthread_mutex_lock(auto_lock_ptr_);
	}

	~AutoLock()
	{
		pthread_mutex_unlock(auto_lock_ptr_);
	}
#endif
};

#endif
