#ifndef CONNECTOR_H_
#define CONNECTOR_H_

class Connector
{
private:
	std::string server_ip_;
	short server_port_;

public:
	Connector();
	virtual ~Connector() = 0;

public:
	void InitConnectionInfo(const char *server_ip,const short server_port);
	bool ConnectServer();
	void Disconnect();
};

#endif
