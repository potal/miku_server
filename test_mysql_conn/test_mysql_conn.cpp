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
	int tmp_user_id = 92002;
	MysqlConnPool *g_pool = MysqlConnPool::GetInstance(); 
	g_pool->Init(DBHOST,USER,PASSWORD,"mmikuapp",10);
	sql::Connection *tmp_conn = g_pool->GetConnection();
	try{
		std::auto_ptr<sql::PreparedStatement> tmp_pre_stmt(tmp_conn->prepareStatement("call user_login(?,@user_psw,@user_red_d,@user_blue_d)"));
		tmp_pre_stmt->setInt(1,tmp_user_id);
		tmp_pre_stmt->execute();

		std::auto_ptr<sql::PreparedStatement> tmp_pre_select_stmt(tmp_conn->prepareStatement("select @user_psw as user_psw,@user_red_d as user_red_d,@user_blue_d as user_blue_d"));
		std::auto_ptr<sql::ResultSet> tmp_result(tmp_pre_select_stmt->executeQuery());
		if(!tmp_result.get())
		{
			std::cout<<"execute error!"<<std::endl;
		}
		else
		{
			if(tmp_result->next())
			{
				std::string tmp_user_psw = tmp_result->getString(1);
				int tmp_red_d = tmp_result->getInt(2);
				int tmp_blue_d = tmp_result->getInt(3);
				std::cout<<"psw:"<<tmp_user_psw<<" red:"<<tmp_red_d<<" blue:"<<tmp_blue_d<<std::endl;
			}
		}
	}
	catch(sql::SQLException &e)
	{
		std::cout<<"Exception:"<<e.what()<<std::endl;
	}
	g_pool->ReleaseConnection(tmp_conn);
	return 0;
} 
