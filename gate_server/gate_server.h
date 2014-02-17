#ifndef GATE_SERVER_H_
#define GATE_SERVER_H_

#include "../common/server_listenner.h"
#include "user_info_list.h"
#include "room_manager.h"
#include "rs_connector.h"
#include "ds_connector.h"
#include <map>

class GateServer
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
	std::map<int,RoomServerConnector *> rs_conn_list_;

public:
	GateServer();
	~GateServer();

	bool GetConfig(std::string file_name);
	bool InitServer();
	bool StartServer();
	
	UserInfoList *GetClientInfoList();
	RoomManager *GetRoomManager();
	DirectorServerConnector *GetDSConnector();
	std::map<int,RoomServerConnector *> *GetRSConnectionList();

	void StopServer();
};

#endif
