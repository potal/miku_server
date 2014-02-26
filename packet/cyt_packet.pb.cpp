#include "cyt_packet.pb.h"
#include <string.h>

StruCytPacket::StruCytPacket(void)
{

}

StruCytPacket::~StruCytPacket(void)
{

}


void StruCytPacket::set_str_head(char* head)
{
	if (NULL != head)
	{
		m_str_head_ = head;
	}
}

void StruCytPacket::set_room_id(int id)
{
	m_room_id_ = id;
}

void StruCytPacket::set_msg_len(int len)
{
	m_msg_len_ = len;
}

void StruCytPacket::set_msg_type(int type)
{
	m_msg_type_ = type;
}

void StruCytPacket::set_str_tail(char* tail)
{
	if (NULL != tail)
	{
		m_str_tail_ = tail;
	}
}

void StruCytPacket::set_msg_data(const void* data,int data_len)
{
	if (NULL != data && data_len >0)
	{
		m_msg_data_ ="";
		m_msg_data_.append((const char*)data,data_len);
	}
}

const string& StruCytPacket::str_head()
{
	return m_str_head_;
}

int StruCytPacket::room_id()
{
	return m_room_id_;
}

int StruCytPacket::msg_len()
{
	return m_msg_len_;
}

int StruCytPacket::msg_type()
{
	return m_msg_type_;
}

const string& StruCytPacket::str_tail()
{
	return m_str_tail_;
}

const string& StruCytPacket::msg_data()
{
	return m_msg_data_;
}


bool StruCytPacket::ParseFromArray(const void* data,int data_len)
{
	if (NULL == data || data_len < 18)
	{
		return false;
	}

	m_str_head_= "";
	m_str_head_.append((char*)data,3);
	m_room_id_ = *(int*)((char*)data+3);
	m_msg_len_ = *(int*)((char*)data+3+4);
	m_msg_type_ = *(int*)((char*)data+3+4+4);
	if (m_msg_len_ < 0 || data_len < ByteSize())
	{
		return false;
	}
	m_msg_data_="";
	m_msg_data_.append(((char*)data+3+4+4+4),m_msg_len_);
	m_str_tail_ = "";
	m_str_tail_.append(((char*)data+3+4+4+4+m_msg_len_),3);

	return true;
}

bool StruCytPacket::SerializeToArray(void* data,int data_len)
{
	if (NULL == data || data_len < ByteSize())
	{
		return false;
	}

	char* start_data = (char*)data;
	memcpy(start_data,m_str_head_.data(),3);
	memcpy(start_data+3,&m_room_id_,4);
	memcpy(start_data+3+4,&m_msg_len_,4);
	memcpy(start_data+3+4+4,&m_msg_type_,4);
	memcpy(start_data+3+4+4+4,m_msg_data_.data(),m_msg_data_.length());
	memcpy(start_data+3+4+4+4+m_msg_data_.length(),m_str_tail_.data(),3);

	return true;
}

int StruCytPacket::ByteSize()
{
	return 3+4+4+4+m_msg_len_+3;
}
