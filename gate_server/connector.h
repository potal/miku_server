#ifndef CONNECTOR_H_
#define CONNECTOR_H_

class Connector
{
private:
	std::string m_server_ip;
	short m_server_port;

public:
	Connector();
	virtual ~Connector() = 0;

public:
	void InitConnectionInfo(const char *server_ip,const short server_port);
	bool ConnectServer();
	void Disconnect();
};

#endif
