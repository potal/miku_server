#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_

#include <iostream>
#include <map>
#include <list>
#include <pthread.h>
//用户资源管理，主要用于事先分配好资源，以便后面使用
template <typename T>
class UserResourceManager
{
private:
	int max_num_;
	std::map<T*,T*> using_user_list_;
	std::list<T*> unused_user_list_;
	T * resource_block_ptr;
	pthread_mutex_t list_lock_;

public:
	UserResourceManager();
	~UserResourceManager();
	
	int Init(int max_num);
	T *Get();
	bool Put(T *user);
};


template <typename T>
UserResourceManager<T>::UserResourceManager()
{
	pthread_mutex_init(&list_lock_,NULL);
}

template <typename T>
UserResourceManager<T>::~UserResourceManager()
{
	if(resource_block_ptr)
		delete []resource_block_ptr;
	unused_user_list_.clear();

	pthread_mutex_destroy(&list_lock_);
}

template <typename T>
int UserResourceManager<T>::Init(int max_num)
{
	max_num_ = max_num;
	int tmp_count = 0;
	try{
		resource_block_ptr = new T[max_num];
		while(tmp_count < max_num)
		{
			unused_user_list_.push_back(resource_block_ptr+tmp_count);
			tmp_count++;
		}
	}
	catch(...)
	{
		return unused_user_list_.size();
	}
	return unused_user_list_.size();
}

template<typename T>
T *UserResourceManager<T>::Get()
{
	pthread_mutex_lock(&list_lock_);
	if(unused_user_list_.size() <= 0)
	{
		pthread_mutex_unlock(&list_lock_);
		return NULL;
	}
	T *tmp_user = unused_user_list_.front();
	unused_user_list_.pop_front();
	using_user_list_[tmp_user] = tmp_user;
	pthread_mutex_unlock(&list_lock_);
	return tmp_user;
}

template<typename T>
bool UserResourceManager<T>::Put(T *user)
{
	pthread_mutex_lock(&list_lock_);
	if(NULL == using_user_list_[user])
	{
		pthread_mutex_unlock(&list_lock_);
		return false;
	}
	using_user_list_.erase(user);
	unused_user_list_.push_back(user);
	pthread_mutex_unlock(&list_lock_);
	return true;
}

#endif
