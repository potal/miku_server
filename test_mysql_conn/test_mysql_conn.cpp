#include <iostream> 
#include <sstream> 
#include <memory> 
#include <string> 
#include <stdexcept> 
 
using namespace std; 
 
#include <mysql_connection.h> 
#include <mysql_driver.h> 
#include <cppconn/driver.h> 
 
using namespace sql; 
 
#define DBHOST "tcp://127.0.0.1:3306" 
#define USER "root" 
#define PASSWORD "13637848712" 
#include "mysql_conn_pool.h"
MysqlConnPool *g_pool = MysqlConnPool::GetInstance(); 
int main() 
{ 
//  sql::mysql::MySQL_Driver *driver; 
//  sql::Connection *conn; 
//  driver = sql::mysql::get_mysql_driver_instance(); 
//  conn = driver->connect("tcp://127.0.0.1:3306","root","13637848712"); 
//  conn->setAutoCommit(0); 
//  cout<<"DataBase connection autocommit mode = "<<conn->getAutoCommit()<<endl; 
//  delete conn; 
//  driver = NULL; 
//  conn = NULL; 
//  return 0; 
	
	
	sql::Connection *tmp_conn = NULL;
	sql::Statement *tmp_state;
	sql::ResultSet *tmp_result;
	tmp_conn = g_pool->GetConnection();
	tmp_state = tmp_conn->createStatement();

	tmp_result = tmp_state->executeQuery("select test_id,test_name from testTable where test_id < 1002");

	while (tmp_result->next()) 
	{
		int tmp_id = tmp_result->getInt("test_id");
		string tmp_name = tmp_result->getString("test_name");
		cout << tmp_id << " : " << tmp_name << endl;
	}
	delete tmp_state;
	g_pool->ReleaseConnection(tmp_conn);
	return 0;
} 
