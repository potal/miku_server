#ifndef CENTER_SERVER_H_
#define CENTER_SERVER_H_

#include "../common/base_user_info.h"
#include "../common/server_listenner.h"
#include "../common/command_chain.h"
#include "client_info_list.h"
#include "ds_connector.h"
#include "mmiku_database.h"

class CenterServer
{
private:
	std::string server_ip_;
	int server_port_;
	int count_worker_;
	int count_user_per_worker_;
	int read_timeout_;
	int write_timeout_;

	ClientInfoList client_list_;
	ServerListenner server_listenner_;
	MikuDatabase miku_db_;

	//DirectorServerConnector ds_connector_;

public:
	CenterServer();
	~CenterServer();

	bool GetConfig(std::string file_name);
	bool InitServer();
	bool StartServer();
	void StopServer();

	ClientInfoList *GetClientList();
	ServerListenner *GetListenner();
	MikuDatabase *GetMikuDB();
	//DirectorServerConnector *GetCSConnector();
};

#endif
