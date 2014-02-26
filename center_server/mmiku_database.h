#ifndef MMIKU_DATABASE_H_
#define MMIKU_DATABASE_H_

#include "../common/mysql_conn_pool.h"
#include "../packet/cyt_packet.pb.h"
#include "../packet/package_define.pb.h"

class MikuDatabase
{
private:
	void *server_ptr_;
	MysqlConnPool *conn_pool_ptr_;
public:
	MikuDatabase();
	~MikuDatabase();

	void Init(void *server_ptr);
public:
	int UserLogin(int user_id,std::string user_psw,int &result,int &user_red_d,int &user_blue_d,int &user_level,int &buyer_riches,int &buyer_grow_up,int &buyer_honour,int &buyer_estimate,int &seller_income,int &seller_exp,int &seller_pop,int &seller_estimate);
	int UserGiveGift(int user_id,std::string gift_code,int &result);
};

#endif
