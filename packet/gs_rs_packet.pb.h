#pragma once
#include <string>
using namespace std;

class GateRoomServerPack
{
public:
	GateRoomServerPack(void);
	virtual ~GateRoomServerPack(void);

public:
	void set_str_head(char* head);
	void set_user_hashkey(int user_hashkey);
	void set_gs_hashkey(int gs_hashkey);
	void set_data_len(int data_len);
	void set_data_msg(const void* data,int data_len);
	void set_str_tail(char* tail);

public:
	const string& str_head();
	int user_hashkey();
	int gs_hashkey();
	int data_len();
	const string& data_msg();
	const string& str_tail();

public:
	bool ParseFromArray(const void* data,int data_len);
	bool SerializeToArray(void* data,int data_len);
	int ByteSize();
	
private:
	string str_head_;
	int user_hashkey_;
	int gs_hashkey_;
	int data_len_;
	string data_msg_;
	string str_tail_;
};
