#ifndef TEST_SERVER_H_
#define TEST_SERVER_H_

#include "common/base_user_info.h"
#include "common/server_listenner.h"
#include "user_info_list.h"
#include "room_manager.h"
#include "cs_connector.h"

class RoomServer
{
private:
	std::string server_ip_;
	int server_port_;
	int count_worker_;
	int count_user_per_worker_;
	int read_timeout_;
	int write_timeout_;

	UserInfoList user_list_;
	ServerListenner server_listenner_;
	RoomManager room_manager_;
	CenterServerConnector cs_connector_;

public:
	RoomServer();
	~RoomServer();

	bool GetConfig(std::string file_name);
	bool InitServer();
	bool StartServer();
	void StopServer();
};

#endif
