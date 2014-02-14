#include "rs_connector.h"

RoomServerConnector::RoomServerConnector()
{
	recv_list_.InitBuffer(1000);
}

RoomServerConnector::~RoomServerConnector()
{
	recv_list_.ReleaseBuffer();
}

void RoomServerConnector::DealWithData(char *buff,int len,int fd)
{

}
