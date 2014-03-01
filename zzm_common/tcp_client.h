#pragma once
#include "mlock.h"
#include "munit.h"
#include "circle_buff.h"

class CTCPClientCallBack
{
public:
	//��������
	virtual void OnRecvData(char* pRecvBuf,int iRecvBufLen)=0;
	//��������
	virtual void OnDealReconnect(int aiType)=0;
	//���ӶϿ�
	virtual void OnLinkDown()=0;
};



class CTCPClient
{
public:
	CTCPClient();
	virtual ~CTCPClient(void);

public:
	//��ʼ��
	bool Init(CTCPClientCallBack* pCallBack);
	//����
	bool Connect(char* pDstIP, int iDstPort);
	//���������߳�
	void RecvDataFun();
	//���������߳�
	void SendDataFun();
	//�����߳�
	void ReConnectFun();
	//��������
	bool SendData(char* pSendBuf,int iSendBufLen);
	//�Ͽ�����
	void DisConnect();
	// ����
	bool Reconnect();
	bool GetStartUp(){return m_bRunning;}
private:

	void Clear();

private:
	char	m_szIP[0x20];
	int		m_iPort;
	//���б��
	bool	m_bRunning;
	//�����߳̾��
	MThreadID	m_pThreadRecv;
	//�����߳̾��
	MThreadID	m_pThreadSend;
	//�����߳̾��
	MThreadID	m_pThreadReconnect;
	//�Զ˵�ַ
	SOCKADDR_IN m_dstAddr;
	//���ͳ�
	CCircleBuff	moSendTcpList;
	//�ص�ָ��
	CTCPClientCallBack* m_pCallBack;
public:
	SOCKET	m_sock;
};
