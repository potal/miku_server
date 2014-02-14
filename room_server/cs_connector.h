#ifndef RS_CONNECTOR_H_
#define RS_CONNECTOR_H_

#include "../common/connector.h"
#include "cs_processor.h"

class CenterServerConnector : public Connector
{
private:
	void *server_ptr_;
	CenterServerProcessor cs_processor_;
public:
	CenterServerConnector();
	~CenterServerConnector();

	bool StartCSProcessor(int circle_list_size,int proc_thread_count,void *server_ptr);
	void DealWithData(char *buff,int len,int fd);
};

#endif
