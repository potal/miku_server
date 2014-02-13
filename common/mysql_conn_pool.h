/*
 *usage:manage mysql connection
 *consult:http://www.51testing.com/html/45/n-853945.html
 *auther:potal
 */
#ifndef MYSQL_CONN_POOL_H_
#define MYSQL_CONN_POOL_H_

#include <iostream> 
#include <sstream> 
#include <memory> 
#include <string> 
#include <stdexcept> 
#include <list>
#include <pthread.h>

#include <mysql_connection.h> 
#include <mysql_driver.h> 
#include <cppconn/driver.h> 
#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

using namespace std;

class MysqlConnPool
{
private:
	std::string sql_addr_;
	std::string user_name_;
	std::string user_psw_;
	std::string db_name_;

	int cur_size_;
	int max_size_;

	std::list<sql::Connection *> sql_conn_list_;
	pthread_mutex_t list_lock_;

	sql::mysql::MySQL_Driver *sql_driver_;

	static MysqlConnPool *conn_pool_;
	
	MysqlConnPool(string sql_addr,string user_name,string user_psw,string db_name,int max_conn_size);//constructor
	sql::Connection * CreateConnection();//create one conncection
	void InitConnection(int init_size);//initialize connections
	void DestoryConnection(sql::Connection *sql_conn);//destory connections
	void DestoryConnPool();//destory connection pool
	
public:
	~MysqlConnPool();
	
	static MysqlConnPool *GetInstance();
	sql::Connection *GetConnection();
	bool ReleaseConnection(sql::Connection *sql_conn);
};

#endif
