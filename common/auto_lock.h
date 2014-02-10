#ifndef AUTO_LOCK_H_
#define AUTO_LOCK_H_

#include <pthread.h>

class AutoLock
{
private:
	pthread_mutex_t *auto_lock_ptr_;

public:
	AutoLock(pthread_mutex_t * mutex_t)
	{
		auto_lock_ptr_ = mutex_t;
		pthread_mutex_lock(auto_lock_ptr_);
	}

	~AutoLock()
	{
		pthread_mutex_unlock(auto_lock_ptr_);
	}
};

#endif
