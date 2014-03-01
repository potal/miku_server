#include "gs_rs_packet.pb.h"
#include <string.h>

GateRoomServerPack::GateRoomServerPack(void)
{

}

GateRoomServerPack::~GateRoomServerPack(void)
{

}


void GateRoomServerPack::set_str_head(char* head)
{
	if (NULL != head)
	{
		str_head_ = head;
	}
}

void GateRoomServerPack::set_user_hashkey(int user_hashkey)
{
	user_hashkey_ = user_hashkey;
}

void GateRoomServerPack::set_gs_hashkey(int gs_hashkey)
{
	gs_hashkey_ = gs_hashkey;
}

void GateRoomServerPack::set_data_len(int data_len)
{
	data_len_ = data_len;
}

void GateRoomServerPack::set_data_msg(const void* data,int data_len)
{
	if (NULL != data && data_len >0)
	{
		data_msg_ ="";
		data_msg_.append((const char*)data,data_len);
	}
}

void GateRoomServerPack::set_str_tail(char* tail)
{
	if (NULL != tail)
	{
		str_tail_ = tail;
	}
}

const string& GateRoomServerPack::str_head()
{
	return str_head_;
}

int GateRoomServerPack::user_hashkey()
{
	return user_hashkey_;
}

int GateRoomServerPack::gs_hashkey()
{
	return gs_hashkey_;
}

int GateRoomServerPack::data_len()
{
	return data_len_;
}

const string& GateRoomServerPack::data_msg()
{
	return data_msg_;
}

const string& GateRoomServerPack::str_tail()
{
	return str_tail_;
}

bool GateRoomServerPack::ParseFromArray(const void* data,int data_len)
{
	if (NULL == data || data_len < 18)
	{
		return false;
	}

	str_head_= "";
	str_head_.append((char*)data,3);
	user_hashkey_ = *(int*)((char*)data+3);
	gs_hashkey_ = *(int*)((char*)data+3+4);
	data_len_ = *(int*)((char*)data+3+4+4);
	if (data_len_ < 0 || data_len < ByteSize())
	{
		return false;
	}
	data_msg_="";
	data_msg_.append(((char*)data+3+4+4+4),data_len_);
	str_tail_ = "";
	str_tail_.append(((char*)data+3+4+4+4+data_len_),3);

	return true;
}

bool GateRoomServerPack::SerializeToArray(void* data,int data_len)
{
	if (NULL == data || data_len < ByteSize())
	{
		return false;
	}

	char* start_data = (char*)data;
	memcpy(start_data,str_head_.data(),3);
	memcpy(start_data+3,&user_hashkey_,4);
	memcpy(start_data+3+4,&gs_hashkey_,4);
	memcpy(start_data+3+4+4,&data_len_,4);
	memcpy(start_data+3+4+4+4,data_msg_.data(),data_msg_.length());
	memcpy(start_data+3+4+4+4+data_msg_.length(),str_tail_.data(),3);

	return true;
}

int GateRoomServerPack::ByteSize()
{
	return 3+4+4+4+data_len_+3;
}
