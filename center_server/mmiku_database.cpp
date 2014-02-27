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
#include <algorithm>

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

int MikuDatabase::UserLogin(int user_id,std::string user_psw,int &result,int &user_red_d,int &user_blue_d,int &user_level,int &buyer_riches,int &buyer_grow_up,int &buyer_honour,int &buyer_estimate,int &seller_income,int &seller_exp,int &seller_pop,int &seller_estimate)
{
	int tmp_return = 0;
	sql::Connection *tmp_conn = NULL;
	try
	{
		tmp_conn = conn_pool_ptr_->GetConnection();
		if(!tmp_conn)
			return 0;

		std::auto_ptr<sql::PreparedStatement> tmp_pre_stmt(tmp_conn->prepareStatement("call user_login(?,@user_psw,@user_red_d,@user_blue_d,@user_level,@buyer_riches,@buyer_grow_up,@buyer_honour,@buyer_estimate,@seller_income,@seller_exp,@seller_popularity,@seller_estimate)"));
		tmp_pre_stmt->setInt(1,user_id);
		tmp_pre_stmt->execute();

		std::auto_ptr<sql::PreparedStatement> tmp_pre_select_stmt(tmp_conn->prepareStatement("select @user_psw as user_psw,@user_red_d as user_red_d,@user_blue_d as user_blue_d,@user_level as user_level,@buyer_riches as buyer_riches,@buyer_grow_up as buyer_grow_up,@buyer_honour as buyer_honour,@buyer_estimate as buyer_estimate,@seller_income as seller_income,@seller_exp as seller_exp,@seller_popularity as seller_popularity,@seller_estimate as seller_estimate"));
		std::auto_ptr<sql::ResultSet> tmp_result(tmp_pre_select_stmt->executeQuery());
		if(!tmp_result.get())
		{
			std::cout<<"execute error!"<<std::endl;
			tmp_return = 0;
		}
		else
		{
			if(tmp_result->next())
			{
				std::string tmp_user_psw = tmp_result->getString(1);
				user_red_d = tmp_result->getInt(2);
				user_blue_d = tmp_result->getInt(3);
				user_level = tmp_result->getInt(4);
				buyer_riches = tmp_result->getInt(5);
				buyer_grow_up = tmp_result->getInt(6);
				buyer_honour = tmp_result->getInt(7);
				buyer_estimate = tmp_result->getInt(8);
				seller_income = tmp_result->getInt(9);
				seller_exp = tmp_result->getInt(10);
				seller_pop = tmp_result->getInt(11);
				seller_estimate = tmp_result->getInt(12);
				std::cout<<"psw:"<<tmp_user_psw<<" red:"<<user_red_d<<" blue:"<<user_blue_d<<std::endl;

				std::string tmp_user_psw_arg = user_psw;
				transform(tmp_user_psw.begin(),tmp_user_psw.end(),tmp_user_psw.begin(),::tolower);//::toupper
				transform(tmp_user_psw_arg.begin(),tmp_user_psw_arg.end(),tmp_user_psw_arg.begin(),::tolower);
				result = (tmp_user_psw == tmp_user_psw_arg ? 1:0);
				tmp_return = 1;
			}
		}

//		std::auto_ptr<sql::Statement> tmp_stmt(tmp_conn->createStatement());
//		if(!tmp_stmt.get())
//		{
//			std::cout<<"tmp_stmt NULL"<<std::endl;
//			conn_pool_ptr_->ReleaseConnection(tmp_conn);
//			return 0;
//		}

		//std::stringstream tmp_sql_state;
		//std::string tmp_sql;
		//tmp_sql_state<<"select user_psw from userInfo where user_id="<<user_id;
		//tmp_sql = tmp_sql_state.str();

//		sql::ResultSet *tmp_result = tmp_stmt->executeQuery(tmp_sql);
//		if(!tmp_result)
//		{
//			std::cout<<"execute error!"<<std::endl;
//			tmp_return = 0;
//		}
//		else
//		{
//			if(tmp_result->next())
//			{
//				std::string tmp_user_psw = tmp_result->getString("user_psw");
//				std::string tmp_user_psw_arg = user_psw;
//				transform(tmp_user_psw.begin(),tmp_user_psw.end(),tmp_user_psw.begin(),::tolower);//::toupper
//				transform(tmp_user_psw_arg.begin(),tmp_user_psw_arg.end(),tmp_user_psw_arg.begin(),::tolower);
//				if(tmp_user_psw == user_psw)
//					result = 1;
//				else
//					result = -1;
//				tmp_return = 1;
//			}
//			else
//				tmp_return = 0;
//		}
	}
	catch(sql::SQLException &e)
	{
		std::cout<<"exception:"<<e.what()<<std::endl;
		tmp_return = 0;
	}
	conn_pool_ptr_->ReleaseConnection(tmp_conn);
	return tmp_return;
}

int MikuDatabase::UserGiveGift(int sender_id,int recver_id,int count_gift,std::string gift_code,int &result,int &sender_red_d)
{
	int tmp_db_ret = 0;
	sql::Connection *tmp_conn = NULL;
	try
	{
		tmp_conn = conn_pool_ptr_->GetConnection();
		if(!tmp_conn)
			return 0;

		std::auto_ptr<sql::PreparedStatement> tmp_pre_stmt(tmp_conn->prepareStatement("call give_gift(?,?,?,?,@sender_red_d,@result)"));
		tmp_pre_stmt->setInt(1,sender_id);
		tmp_pre_stmt->setInt(2,recver_id);
		tmp_pre_stmt->setInt(3,count_gift);
		tmp_pre_stmt->setString(4,gift_code);
		tmp_pre_stmt->execute();

		std::auto_ptr<sql::PreparedStatement> tmp_pre_select_stmt(tmp_conn->prepareStatement("select @sender_red_d as sender_red_d,@result as result"));
		std::auto_ptr<sql::ResultSet> tmp_result(tmp_pre_select_stmt->executeQuery());
		if(!tmp_result.get())
		{
			std::cout<<"execute error!"<<std::endl;
			tmp_db_ret = 0;
		}
		else
		{
			if(tmp_result->next())
			{
				sender_red_d = tmp_result->getInt(1);
				result = tmp_result->getInt(2);
				tmp_db_ret = 1;
			}
		}
	}
	catch(sql::SQLException &e)
	{
		std::cout<<"Error:"<<e.what()<<std::endl;
		tmp_db_ret = 0;
	}
	conn_pool_ptr_->ReleaseConnection(tmp_conn);
	return tmp_db_ret;
}
