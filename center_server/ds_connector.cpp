#include "ds_connector.h"

DirectorServerConnector::DirectorServerConnector()
{
	recv_list_.InitBuffer(1000);
}

DirectorServerConnector::~DirectorServerConnector()
{
	recv_list_.ReleaseBuffer();
}

void DirectorServerConnector::DealWithData(char *buff,int len,int fd)
{

}
