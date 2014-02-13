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
int main() 
{ 
//	sql::mysql::MySQL_Driver *driver; 
//	sql::Connection *conn; 
//	driver = sql::mysql::get_mysql_driver_instance(); 
//	conn = driver->connect("tcp://127.0.0.1:3306","root","13637848712"); 
//	conn->setAutoCommit(0); 
//	cout<<"DataBase connection autocommit mode = "<<conn->getAutoCommit()<<endl; 
//	delete conn; 
//	driver = NULL; 
//	conn = NULL; 
//	return 0; 
	
	MysqlConnPool *g_pool = MysqlConnPool::GetInstance(); 
	
	sql::Connection *tmp_conn = NULL;
	sql::Statement *tmp_state;
	sql::ResultSet *tmp_result;
	tmp_conn = g_pool->GetConnection();
	tmp_state = tmp_conn->createStatement();
	std::stringstream tmp_sql_state;
	std::string tmp_sql;
	int tmp_user_id = 92002;
	tmp_sql_state<<"select user_id,user_psw,user_nickname from userInfo where user_id="<<tmp_user_id;
	tmp_sql = tmp_sql_state.str();
	std::cout<<tmp_sql<<std::endl;

	tmp_result = tmp_state->executeQuery(tmp_sql);

	while (tmp_result->next()) 
	{
		int tmp_id = tmp_result->getInt("user_id");
		string tmp_psw = tmp_result->getString("user_psw");
		string tmp_name = tmp_result->getString("user_nickname");
		cout << tmp_id << " : " << tmp_name << " : " << tmp_psw << endl;
	}
	delete tmp_state;
	g_pool->ReleaseConnection(tmp_conn);
	return 0;
} 
