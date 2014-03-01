#ifndef ROOM_SERVER_H_
#define ROOM_SERVER_H_

#include "../common/base_user_info.h"
#include "../common/server_listenner.h"
#include "../common/command_chain.h"
#include "../common/base_log.h"
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

	bool InitServer(std::string conf_file_name);
	bool StartServer();
	void StopServer();

	ClientInfoList *GetClientList();
	RoomManager *GetRoomManager();
	DirectorServerConnector *GetDSConnector();
	std::map<int,CenterServerConnector *> *GetCSConnectionList();

	CenterServerConnector *GetCSConnector();

protected:
	bool GetConfig(std::string file_name);

public:
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
		UserInfo *tmp_current_user = tmp_room->GetUserInfoList()->GetFirstUser();
		if(!tmp_current_user)
			return false;
		do
		{
			ClientInfoEx *tmp_gs = reinterpret_cast<ClientInfoEx *>(client_list_.GetUserByHashkey(tmp_current_user->gs_hashkey_user_on));
			if(tmp_gs)
			{
				tmp_gs->GetGSProcessor()->SendData(pack,msg_id,tmp_current_user->user_hash_key);
			}
			tmp_current_user = tmp_room->GetUserInfoList()->GetNextUser();
		}while(tmp_current_user);
	}
	return true;
}

#endif
