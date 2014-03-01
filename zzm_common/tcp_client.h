#pragma once
#include "mlock.h"
#include "munit.h"
#include "circle_buff.h"

class CTCPClientCallBack
{
public:
	//接收数据
	virtual void OnRecvData(char* pRecvBuf,int iRecvBufLen)=0;
	//处理重连
	virtual void OnDealReconnect(int aiType)=0;
	//连接断开
	virtual void OnLinkDown()=0;
};



class CTCPClient
{
public:
	CTCPClient();
	virtual ~CTCPClient(void);

public:
	//初始化
	bool Init(CTCPClientCallBack* pCallBack);
	//连接
	bool Connect(char* pDstIP, int iDstPort);
	//接收数据线程
	void RecvDataFun();
	//发送数据线程
	void SendDataFun();
	//重连线程
	void ReConnectFun();
	//发送数据
	bool SendData(char* pSendBuf,int iSendBufLen);
	//断开连接
	void DisConnect();
	// 重连
	bool Reconnect();
	bool GetStartUp(){return m_bRunning;}
private:

	void Clear();

private:
	char	m_szIP[0x20];
	int		m_iPort;
	//运行标记
	bool	m_bRunning;
	//接收线程句柄
	MThreadID	m_pThreadRecv;
	//发送线程句柄
	MThreadID	m_pThreadSend;
	//发送线程句柄
	MThreadID	m_pThreadReconnect;
	//对端地址
	SOCKADDR_IN m_dstAddr;
	//发送池
	CCircleBuff	moSendTcpList;
	//回调指针
	CTCPClientCallBack* m_pCallBack;
public:
	SOCKET	m_sock;
};
