/*
 * =====================================================================================
 *
 *       Filename:  connector.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/15/2014 08:32:19 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

Connector::Connector()
{

}

void Connector::InitConnectionInfo(const char *server_ip,const short server_port)
{
	m_server_ip = server_ip;
	m_server_port = server_port;
}

bool Connector::ConnectServer()
{
	return true;
}

void Connnector::Disconnect()
{

}
