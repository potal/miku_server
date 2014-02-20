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
#include "center_server.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

MikuDatabase::MikuDatabase():server_ptr_(NULL)
{
	conn_pool_ptr_ = MysqlConnPool::GetInstance();
}

MikuDatabase::~MikuDatabase()
{

}

void MikuDatabase::Init(void *server_ptr)
{
	server_ptr_ = server_ptr;
	CenterServer *tmp_server = reinterpret_cast<CenterServer *>(server_ptr_);
	if(!tmp_server)
		return;
	if(conn_pool_ptr_)
	{
		conn_pool_ptr_->Init(tmp_server->db_addr_,tmp_server->db_user_,tmp_server->db_psw_,tmp_server->db_name_,tmp_server->max_conn_size_);
	}
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
				if(tmp_user_psw == user_psw)
					result = 1;
				else
					result = -1;
				tmp_return = 1;
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

int MikuDatabase::UserGiveGift(int user_id,std::string gift_code,int &result)
{
	int tmp_db_ret = 0;
	sql::Connection *tmp_conn = NULL;
	sql::Statement *tmp_state = NULL;
	sql::ResultSet *tmp_result = NULL;
	std::stringstream tmp_sql_state;
	std::string tmp_sql;

	return tmp_db_ret;
}
