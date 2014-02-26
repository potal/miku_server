#ifndef ROOM_SERVER_H_
#define ROOM_SERVER_H_

#include "../common/base_user_info.h"
#include "../common/server_listenner.h"
#include "../common/command_chain.h"
#include "client_info_list.h"
#include "room_manager.h"
#include "ds_connector.h"
#include "cs_connector.h"
#include <time.h>
#include <map>

class RoomServer
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
	ClientInfoList client_list_;
	ServerListenner server_listenner_;
	RoomManager room_manager_;
	CenterServerConnector cs_connector_;

	DirectorServerConnector ds_connector_;
	std::map<int,CenterServerConnector *> cs_conn_list_;

public:
	RoomServer();
	~RoomServer();

	bool GetConfig(std::string file_name);
	bool InitServer();
	bool StartServer();
	void StopServer();

	ClientInfoList *GetClientList();
	RoomManager *GetRoomManager();
	DirectorServerConnector *GetDSConnector();
	std::map<int,CenterServerConnector *> *GetCSConnectionList();

	CenterServerConnector *GetCSConnector();

	template<class T>
	bool SendDataToRoomAllUser(T &pack,int msg_id,int room_id = 0);
};

template<class T>
bool RoomServer::SendDataToRoomAllUser(T &pack,int msg_id,int room_id)
{
	if(room_id == 0)//send to all room
	{
		;
	}
	else
	{
		ChatRoom *tmp_room = room_manager_.GetRoom(room_id);
		if(!tmp_room)
			return false;
	}
	return false;
}

#endif
