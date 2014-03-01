#ifndef GATE_SERVER_H_
#define GATE_SERVER_H_

#include "../common/server_listenner.h"
#include "user_info_list.h"
#include "room_manager.h"
#include "rs_connector.h"
#include "ds_connector.h"
#include "cl_processor.h"
#include <map>

class GateServer
{
public:
	int server_id_;
	std::string server_ip_;
	int server_port_;
	int count_worker_;
	int count_user_per_worker_;
	int read_timeout_;
	int write_timeout_;
	std::string ds_ip_;
	int ds_port_;

private:
	UserInfoList user_list_;
	ServerListenner server_listenner_;
	RoomManager room_manager_;
	RoomServerConnector rs_connector_;

	DirectorServerConnector ds_connector_;
	std::map<int,RoomServerConnector *> rs_conn_list_;
	
	ClientProcessor cl_processor_;

public:
	GateServer();
	~GateServer();

	bool InitServer(std::string conf_file_name);
	bool StartServer();
	void StopServer();

	UserInfoList *GetClientInfoList();
	RoomManager *GetRoomManager();
	DirectorServerConnector *GetDSConnector();
	std::map<int,RoomServerConnector *> *GetRSConnectionList();
	ClientProcessor *GetCLProcessor();

protected:
	bool GetConfig(std::string file_name);
};

#endif
