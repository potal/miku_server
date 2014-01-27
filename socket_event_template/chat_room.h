#ifndef CHAT_ROOM_H_
#define CHAT_ROOM_H_

#include <iostream>
#include <map>

class ChatRoom
{
private:
	long room_id_;
	int server_fd_;

public:
	ChatRoom();
	~ChatRoom();

	bool SetRoom(long room_id,int server_fd);
	int GetSocket(){
		return server_fd_;
	}
};


#endif
