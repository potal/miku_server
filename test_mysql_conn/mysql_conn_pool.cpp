#include "mysql_conn_pool.h"
using namespace std;

MysqlConnPool *MysqlConnPool::conn_pool_ = NULL;

MysqlConnPool::MysqlConnPool():cur_size_(0),max_size_(0),sql_driver_(NULL)
{
	sql_conn_list_.clear();
	pthread_mutex_init(&list_lock_,NULL);
	sql_driver_ = sql::mysql::get_mysql_driver_instance();
}

sql::Connection * MysqlConnPool::CreateConnection()
{
	sql::Connection *tmp_conn = NULL;
	try
	{
		tmp_conn = sql_driver_->connect(sql_addr_,user_name_,user_psw_);
		if(tmp_conn)
		{
			tmp_conn->setSchema(db_name_);
		}
		return tmp_conn;
	}
	catch(...)
	{
		return NULL;
	}
}

void MysqlConnPool::InitConnection(int init_size)
{
	sql::Connection *tmp_conn = NULL;
	pthread_mutex_lock(&list_lock_);
	for(int i = 0;i < init_size;i ++)
	{
		tmp_conn = this->CreateConnection();
		if(tmp_conn)
		{
			sql_conn_list_.push_back(tmp_conn);
			++(this->cur_size_);
		}
	}
	std::cout<<"connect db ok,count:"<<cur_size_<<std::endl;
	pthread_mutex_unlock(&list_lock_);
}

void MysqlConnPool::DestoryConnection(sql::Connection *sql_conn)
{
	if(sql_conn)
	{
		try{
			sql_conn->close();
		}
		catch(...)
		{
			;
		}
		delete sql_conn;
	}
}

void MysqlConnPool::DestoryConnPool()
{
	std::list<sql::Connection *>::iterator tmp_iter;
	pthread_mutex_lock(&list_lock_);
	for(tmp_iter = sql_conn_list_.begin();tmp_iter != sql_conn_list_.end();tmp_iter ++)
	{
		this->DestoryConnection(*tmp_iter);
	}
	cur_size_ = 0;
	sql_conn_list_.clear();
	pthread_mutex_unlock(&list_lock_);
}

MysqlConnPool::~MysqlConnPool()
{
	this->DestoryConnPool();
}

void MysqlConnPool::Init(string sql_addr,string user_name,string user_psw,string db_name,int max_conn_size)
{
	sql_addr_ = sql_addr;
	user_name_ = user_name;
	user_psw_ = user_psw;
	db_name_ = db_name;
	max_size_ = max_conn_size;

	InitConnection(max_size_/2);
}

MysqlConnPool *MysqlConnPool::GetInstance()
{
	if(NULL == conn_pool_)
	{
		try
		{
			conn_pool_ = new MysqlConnPool();
		}
		catch(...)
		{
			conn_pool_ = NULL;
		}
	}
	return conn_pool_;
}

sql::Connection *MysqlConnPool::GetConnection()
{
	sql::Connection *tmp_conn = NULL;
	pthread_mutex_lock(&list_lock_);
	if(sql_conn_list_.size() > 0)
	{
		tmp_conn = sql_conn_list_.front();
		sql_conn_list_.pop_front();
		if(tmp_conn->isClosed())
		{
			delete tmp_conn;
			tmp_conn = this->CreateConnection();
		}
		if(tmp_conn == NULL)
		{
			--cur_size_;
		}
		pthread_mutex_unlock(&list_lock_);
		return tmp_conn;
	}
	else
	{
		if(cur_size_ < max_size_)
		{
			tmp_conn = this->CreateConnection();
			if(tmp_conn)
			{
				++cur_size_;
			}
		}
		pthread_mutex_unlock(&list_lock_);
		return tmp_conn;
	}
}

bool MysqlConnPool::ReleaseConnection(sql::Connection *sql_conn)
{
	if(sql_conn)
	{
		pthread_mutex_lock(&list_lock_);
		sql_conn_list_.push_back(sql_conn);
		pthread_mutex_unlock(&list_lock_);
	}
	return true;
}

