#ifndef DS_CONNECTOR_H_
#define DS_CONNECTOR_H_

#include "../common/connector.h"
#include "ds_processor.h"

class DirectorServerConnector : public Connector
{
private:
	void *server_ptr_;
	CircleList recv_list_;
	DirectorServerProcessor ds_processor_;
public:
	DirectorServerConnector();
	~DirectorServerConnector();

	bool StartDSProcessor(int circle_list_size,int thread_count,void *server_ptr);
	void DealWithData(char *buff,int len,int fd);
};

#endif
