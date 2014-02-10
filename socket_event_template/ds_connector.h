#ifndef DS_CONNECTOR_H_
#define DS_CONNECTOR_H_

#include "../common/connector.h"

class DirectorServerConnector : public Connector
{
private:
	CircleList recv_list_;
public:
	DirectorServerConnector();
	~DirectorServerConnector();

	void DealWithData(char *buff,int len,int fd);
};

#endif
