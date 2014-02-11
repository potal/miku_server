#include "cs_connector.h"

CenterServerConnector::CenterServerConnector()
{
	recv_list_.InitBuffer(1000);
}

CenterServerConnector::~CenterServerConnector()
{
	recv_list_.ReleaseBuffer();
}

void CenterServerConnector::DealWithData(char *buff,int len,int fd)
{

}
