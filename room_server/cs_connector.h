#ifndef RS_CONNECTOR_H_
#define RS_CONNECTOR_H_

#include "common/connector.h"

class CenterServerConnector : public Connector
{
private:
	CircleList recv_list_;
public:
	CenterServerConnector();
	~CenterServerConnector();

	void DealWithData(char *buff,int len,int fd);
};

#endif
