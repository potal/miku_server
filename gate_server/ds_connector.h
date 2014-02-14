#ifndef DS_CONNECTOR_H_
#define DS_CONNECTOR_H_

#include "../common/connector.h"

class DirectorProcessor;
class DirectorServerConnector : public Connector
{
private:
	CircleList recv_list_;
public:
	friend class DirectorProcessor;	//ds_processor can read recv_list_;
	DirectorServerConnector();
	~DirectorServerConnector();

	void DealWithData(char *buff,int len,int fd);
};

#endif
