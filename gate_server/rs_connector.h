#ifndef RS_CONNECTOR_H_
#define RS_CONNECTOR_H_

#include "../common/connector.h"
#include "rs_processor.h"

class RoomServerConnector : public Connector
{
private:
	void *server_ptr_;
	CircleList recv_list_;
	RoomServerProcessor rs_processor_;
public:
	RoomServerConnector();
	~RoomServerConnector();

	bool StartRSProcessor(int max_list_size,int thread_count,void *server_ptr);
	void DealWithData(char *buff,int len,int fd);
};

#endif
