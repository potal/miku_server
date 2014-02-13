/*
 * =====================================================================================
 *
 *       Filename:  mmiku_database.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2014 10:48:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "mmiku_database.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

MikuDatabase::MikuDatabase()
{
	conn_pool_ptr_ = MysqlConnPool::GetInstance();
}

MikuDatabase::~MikuDatabase()
{

}

int MikuDatabase::UserLogin(int user_id,std::string user_psw,int &result)
{
	int tmp_return = 0;
	sql::Connection *tmp_conn = NULL;
	sql::Statement *tmp_state = NULL;
	try
	{
		tmp_conn = conn_pool_ptr_->GetConnection();
		if(!tmp_conn)
			return 0;
		tmp_state = tmp_conn->createStatement();
		if(!tmp_state)
		{
			std::cout<<"tmp_state NULL"<<std::endl;
			conn_pool_ptr_->ReleaseConnection(tmp_conn);
			return 0;
		}
		std::stringstream tmp_sql_state;
		std::string tmp_sql;
		tmp_sql_state<<"select user_psw from userInfo where user_id="<<user_id;
		tmp_sql = tmp_sql_state.str();
		std::cout<<tmp_sql<<std::endl;

		sql::ResultSet *tmp_result = tmp_state->executeQuery(tmp_sql);
		if(!tmp_result)
		{
			std::cout<<"execute error!"<<std::endl;
			tmp_return = 0;
		}
		else
		{
			if(tmp_result->next())
			{
				std::string tmp_user_psw = tmp_result->getString("user_psw");
				std::cout<<tmp_user_psw<<std::endl;
				if(tmp_user_psw == user_psw)
				{
					tmp_return = 1;
				}
				else
				{
					tmp_return = -1;
				}
			}
			else
				tmp_return = 0;
		}
	}
	catch(...)
	{
		std::cout<<"exception"<<std::endl;
		tmp_return = 0;
	}
	conn_pool_ptr_->ReleaseConnection(tmp_conn);
	if(tmp_state)
		delete tmp_state;
	return tmp_return;
}
