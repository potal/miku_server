#include "stdafx.h"
#include "tcp_client.h"
#include "base_log.h"


CTCPClient::CTCPClient()
{
	Clear();
	moSendTcpList.Init(2000);
}

CTCPClient::~CTCPClient(void)
{
	
}

void CTCPClient::Clear()
{
	m_sock = INVALID_SOCKET;
	m_bRunning = false;
	m_pThreadSend = NULL;
	m_pCallBack = NULL;
}


TTHREAD_FUNC_RESULT TCPClientRecvThread(TTHREAD_FUNC_PARAMETER lpParam)
{
	CTCPClient* pSer = (CTCPClient*)lpParam;
	if (NULL != pSer)
	{
		pSer->RecvDataFun();
	}
	return 0;
}

TTHREAD_FUNC_RESULT TCPClientSendThread(TTHREAD_FUNC_PARAMETER lpParam)
{
	CTCPClient* pSer = (CTCPClient*)lpParam;
	if (NULL != pSer)
	{
		pSer->SendDataFun();
	}
	return 0;
}

TTHREAD_FUNC_RESULT TCPClientReconnectThread(TTHREAD_FUNC_PARAMETER lpParam)
{
	CTCPClient* pSer = (CTCPClient*)lpParam;
	if (NULL != pSer)
	{
		pSer->ReConnectFun();
	}
	return 0;
}

//��ʼ��
bool CTCPClient::Init(CTCPClientCallBack* pCallBack)
{
	if (NULL == pCallBack || NULL != m_pCallBack)
	{
		return false;
	}

	m_pCallBack = pCallBack;

	return true;
}

//����
bool CTCPClient::Connect(char* pDstIP, int iDstPort)
{
	if (NULL == pDstIP || iDstPort <= 0)
	{
		return false;
	}
	memcpy(m_szIP,pDstIP,0x1F);
	m_iPort = iDstPort;
	
	m_sock = socket(AF_INET, SOCK_STREAM, 0);
	
	if (INVALID_SOCKET ==m_sock)
	{
		return false;
	}

	sockaddr_in sockin;
	sockin.sin_family=AF_INET;
	sockin.sin_port=htons(iDstPort);                    //ָ�����ӵĶ˿ں�

#ifdef _WIN32
	sockin.sin_addr.S_un.S_addr= inet_addr(pDstIP);     //ָ����������IP��ַ
#else
	sockin.sin_addr.s_addr = inet_addr(pDstIP);
#endif

	int iRet = connect(m_sock,(sockaddr*)&sockin,sizeof(sockin));
	if (SOCKET_ERROR == iRet)
	{
		MCloseScoket(m_sock);
		m_sock = INVALID_SOCKET;
		return false;
	}
	
	m_bRunning = true;
	int liTimeOut = 1000;
	setsockopt(m_sock, IPPROTO_TCP, SO_SNDTIMEO, (char *) &liTimeOut, 4);
	// ���ýϴ�Ķ�����
	long llRcvBufLen = 128*1024;
	long llSndBufLen = 128*1024;

	char * lpOptVal;
	lpOptVal = (char *)&llRcvBufLen;
	setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, lpOptVal, 4);
	lpOptVal = (char *)&llSndBufLen;
	setsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, lpOptVal, 4); 

	m_pThreadRecv = MCreateThread(TCPClientRecvThread,this);
	m_pThreadSend = MCreateThread(TCPClientSendThread,this);
	m_pThreadReconnect = MCreateThread(TCPClientReconnectThread,this);
	return true;
}

//��������
void CTCPClient::RecvDataFun()
{
	char szRecvBuf[TCP_BUF_LENGTH] = "";
	while (m_bRunning)
	{
		if (INVALID_SOCKET == m_sock)
		{
			MSleep(10);
			continue;
		}

		int iRecvLen = recv(m_sock,szRecvBuf,TCP_BUF_LENGTH,0);
		if (iRecvLen > 0 && NULL != m_pCallBack)
		{
			m_pCallBack->OnRecvData(szRecvBuf,iRecvLen);
		}
		else if (iRecvLen <= 0)
		{
			printf("disconnected,reconnect...\n");
			MCloseScoket(m_sock);
			m_sock = INVALID_SOCKET;
		}
	}
}
//��������
void CTCPClient::SendDataFun()
{
	while (m_bRunning)
	{
		if (INVALID_SOCKET == m_sock)
		{
			MSleep(10);
			continue;
		}
		
		BuffStruc lstruTcpSendBuf;
		bool lbRet = moSendTcpList.ReadTCPData(lstruTcpSendBuf);
		
		if (lbRet)
		{
			char* pstart = lstruTcpSendBuf.mszBuff;
			int left_len = lstruTcpSendBuf.miLen;
			while (left_len > 0)
			{
				int send_len = send(m_sock,pstart,left_len,0);
				if (send_len < 0)
				{
					int err_code = MGetLastError();
					if (err_code != SOCKET_NOERROR)
					{
						WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"CTCPClient::SendData","�������ݴ���left_len:%d,total_len:%d,ErrorCode:%d",left_len,lstruTcpSendBuf.miLen,err_code);
						MCloseScoket(m_sock);
						m_sock  = INVALID_SOCKET;
						break;
					}
					else
					{
						MSleep(1);
					}	
				}
				left_len-=send_len;
				pstart+=send_len;
			}
		}
		else
		{
			MSleep(5);
		}
	}
}

void CTCPClient::ReConnectFun()
{
	DWORD dCurrent = GetTickCount();
	DWORD dPreTime = GetTickCount();
	int iReConnectTime = 0;
	while (m_bRunning)
	{
		MSleep(100);

		dCurrent = GetTickCount();
		if ((dCurrent - dPreTime) < iReConnectTime)
		{
			continue;
		}

		dPreTime = dCurrent;

		if (INVALID_SOCKET == m_sock)
		{
			moSendTcpList.Clear();
			SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
			if(INVALID_SOCKET == sock)
			{
				continue;
			}

			sockaddr_in sockin;
			sockin.sin_family=AF_INET; 
			sockin.sin_port=htons(m_iPort);							//ָ�����ӵĶ˿ں�
	
#ifdef _WIN32
			sockin.sin_addr.S_un.S_addr= inet_addr(m_szIP);     //ָ����������IP��ַ
#else
			sockin.sin_addr.s_addr = inet_addr(m_szIP);
#endif

			if(!m_bRunning)
				break;
			int iRet = connect(sock,(sockaddr*)&sockin,sizeof(sockin));
			if (SOCKET_ERROR == iRet)
			{
				MCloseScoket(sock);
				sock  = INVALID_SOCKET;

				//3�������
				iReConnectTime = 3000;
			}
			else
			{
				printf("CTCPClient::ReConnectFun  connect success  \n");
				int liTimeOut = 1000;
				setsockopt(sock, IPPROTO_TCP, SO_SNDTIMEO, (char *) &liTimeOut, 4);
				// ���ýϴ�Ķ�����
				long llRcvBufLen = 128*1024;
				long llSndBufLen = 128*1024;

				char * lpOptVal;
				lpOptVal = (char *)&llRcvBufLen;
				setsockopt(sock, SOL_SOCKET, SO_RCVBUF, lpOptVal, 4);
				lpOptVal = (char *)&llSndBufLen;
				setsockopt(sock, SOL_SOCKET, SO_SNDBUF, lpOptVal, 4); 

				m_sock = sock;										// ����������
				m_pCallBack->OnDealReconnect(0);
				iReConnectTime = 0;
			}
		}
		else
		{
			int lbRet = send(m_sock,"",0,0);
			if(lbRet < 0)
			{
				MCloseScoket(m_sock);
				m_sock = INVALID_SOCKET;
			}
		}
	}
}


//��������
bool CTCPClient::SendData(char* apSendBuf,int aiSendBufLen)
{
	if(!apSendBuf || aiSendBufLen <= 0 || !m_bRunning)
		return false;
	if(INVALID_SOCKET == m_sock)
		return false;
	moSendTcpList.PushTCPData(apSendBuf,aiSendBufLen);
	return true;
}
// �Ͽ�����
void CTCPClient::DisConnect()
{
	m_bRunning = false;
	if(m_sock)
	{
		MCloseScoket(m_sock);
		m_sock = INVALID_SOCKET;
	}

	MWaitThread(m_pThreadRecv);
	MWaitThread(m_pThreadSend);
	MWaitThread(m_pThreadReconnect);
}

bool CTCPClient::Reconnect()
{
	if(strlen(m_szIP) <= 0 || m_iPort <= 0)
		return false;
	if(m_sock)
	{
		MCloseScoket(m_sock);
		m_sock = INVALID_SOCKET;
	}
	do 
	{
		if(!m_bRunning)
			return false;
		if (INVALID_SOCKET == m_sock)
		{
			m_sock = socket(AF_INET, SOCK_STREAM, 0);
			if(INVALID_SOCKET == m_sock)
				continue;
		}

		sockaddr_in sockin;
		sockin.sin_family=AF_INET; 
		sockin.sin_port=htons(m_iPort);							//ָ�����ӵĶ˿ں�
#ifdef _WIN32
		sockin.sin_addr.S_un.S_addr= inet_addr(m_szIP);     //ָ����������IP��ַ
#else
		sockin.sin_addr.s_addr = inet_addr(m_szIP);
#endif
		int iRet = connect(m_sock,(sockaddr*)&sockin,sizeof(sockin));
		if (SOCKET_ERROR == iRet)
		{
			MCloseScoket(m_sock);
			m_sock = INVALID_SOCKET;
			MSleep(3000);										// 3������
		}
	} while (m_sock = INVALID_SOCKET && m_bRunning);
	printf("reconnect successful...\n");
	return true;
}