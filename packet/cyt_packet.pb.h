#pragma once
#include <string>
using namespace std;

class StruCytPacket
{
public:
	StruCytPacket(void);
	virtual ~StruCytPacket(void);

public:
	void set_str_head(char* head);
	void set_room_id(int id);
	void set_msg_len(int len);
	void set_msg_type(int type);
	void set_str_tail(char* tail);
	void set_msg_data(const void* data,int data_len);

public:
	const string& str_head();
	int room_id();
	int msg_len();
	int msg_type();
	const string& str_tail();
	const string& msg_data();

public:
	bool ParseFromArray(const void* data,int data_len);
	bool SerializeToArray(void* data,int data_len);
	int ByteSize();
	
private:
	string m_str_head_;
	int m_room_id_;
	int m_msg_len_;
	int m_msg_type_;
	string m_msg_data_;
	string m_str_tail_;
};
