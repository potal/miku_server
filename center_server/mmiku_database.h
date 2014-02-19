#ifndef MMIKU_DATABASE_H_
#define MMIKU_DATABASE_H_

#include "../common/mysql_conn_pool.h"
#include "../packet/cyt_packet.pb.h"
#include "../packet/package_define.pb.h"

class MikuDatabase
{
private:
	MysqlConnPool *conn_pool_ptr_;
public:
	MikuDatabase();
	~MikuDatabase();

public:
	int UserLogin(int user_id,std::string user_psw,int &result);
};

#endif
