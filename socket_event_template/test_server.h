#ifndef TEST_SERVER_H_
#define TEST_SERVER_H_

#include "../common/base_user_info.h"
#include "../common/server_listenner.h"
#include "../common/command_chain.h"
#include "user_info_list.h"
#include "room_manager.h"
#include "rs_connector.h"
#include "ds_connector.h"
#include "ds_processor.h"

class TestServer
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
	RoomServerConnector rs_connector_;

	DirectorServerConnector ds_connector_;
	DirectorProcessor ds_processor_;

public:
	TestServer();
	~TestServer();

	bool GetConfig(std::string file_name);
	bool InitServer();
	bool StartServer();
	
	RoomManager *GetRoomManager();
	DirectorServerConnector *GetDSConnector();

	void StopServer();
};

#endif
