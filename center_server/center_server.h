#ifndef TEST_SERVER_H_
#define TEST_SERVER_H_

#include "common/base_user_info.h"
#include "common/server_listenner.h"
#include "common/command_chain.h"
#include "user_info_list.h"
#include "room_manager.h"
#include "cs_connector.h"
#include "cl_processor.h"

class CenterServer
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
	ClientProcessor cl_processor_;

public:
	CenterServer();
	~CenterServer();

	bool GetConfig(std::string file_name);
	bool InitServer();
	bool StartServer();
	void StopServer();

	UserInfoList *GetUserList();
	RoomManager *GetRoomManager();
	CenterServerConnector *GetCSConnector();
	ClientProcessor *GetClientProcessor();
};

#endif
