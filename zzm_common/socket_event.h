#pragma once
#include "event2/listener.h"
#include "event2/util.h"
#include "event2/event.h"
#include "event2/thread.h"
#include "base_log.h"
#include "buf_vector.h"
#include "mlock.h"
#include <signal.h>
#include <list>
#include <map>
using namespace std;


class CSocketContext;


struct StuThraedInfo 
{
	int m_iID;
	void* m_pIocp;
	struct event_base* m_base;
	CMLock m_lock;
	list<CSocketContext*> m_list;
	struct event* m_event;
	sockaddr_in m_addr;
	int notify_receive_fd;
	int notify_send_fd;
	StuThraedInfo()
	{
		m_iID = 0;
		m_pIocp = NULL;
		m_base = NULL;
		m_event = NULL;
	}
};

class CSocketRes
{
public:
	virtual CSocketContext* GetContext()=0;
	virtual void ReleaseContext(CSocketContext* pContext)=0;
};

class CSocketEvent
{
public:
	CSocketEvent(void);
	virtual ~CSocketEvent(void);

public:
	//开启服务
	bool Start(char* pIP,int iPort,int iTimeout,CSocketRes* pRes);
	//停止服务
	bool Stop();
	//检查超时连接
	void CheckTimeOut();
	//检查超时连接
	void Dispatch();
	void Do(int iIndex);
	//直接发送数据
	bool SendTcpData(CSocketContext* pContext,char *apSendBuff, int aiSendLen);

	//获取Context
	CSocketContext* GetContext();
	//释放Context 
	void ReleaseContext(CSocketContext* pContext);

public:
	CMLock m_listlock;
	list<CSocketContext*> m_list;
	
	CMLock m_acceptlock;
		
private:
	//多久未收到包超时时长，0 表示不超时
	int m_iTimeout;
	//业务线程句柄
	MThreadID m_hTimeOutHandle;
	//业务线程句柄
	MThreadID* m_pWorkHandle;
	//监听套接字
	SOCKET m_serSock;
	//资源池
	CSocketRes* m_pRes;

	CMLock m_maplock;
	map<SOCKET,CSocketContext*> m_map;

	bool m_bRunning;

public:
	StuThraedInfo* m_pThreadInfo;
	int m_iThreadCount;
	struct event_config *cfg;
	struct event_base* base;
	struct evconnlistener *listener;
	struct event *listener_event;
	struct event *signal_event;
	int m_iIndex;
};


class CSocketContext
{
public:
	CSocketContext()
	{
		m_sock = INVALID_SOCKET;
		m_read_event = NULL;
		m_write_event = NULL;
		Clear();
		m_bState = false;
		m_pIocp = NULL;
	}

	~CSocketContext()
	{
		if (NULL != m_read_event)
		{
			event_free(m_read_event);
			m_read_event = NULL;
		}
		if (NULL != m_write_event)
		{
			event_free(m_write_event);
			m_write_event = NULL;
		}
	}

	void CloseConn()
	{
		Lock();
		SOCKET sock = m_sock;
		m_sock = INVALID_SOCKET;
		UnLock();

		if (NULL != m_read_event)
		{
			event_del(m_read_event);
		}
		if (NULL != m_write_event)
		{
			event_del(m_write_event);
		}

		Lock();
		if (INVALID_SOCKET != sock)
		{
			WRITEFORMATINFOLOG(__THREADID__,__FILE__,__LINE__,"CloseConn","socket:%d",sock);
			MCloseScoket(sock);
		}
		UnLock();
	}

	virtual void Init()
	{

	};

	virtual	void Clear()
	{
		Lock();

		SOCKET sock = m_sock;
		m_bState = false;
		m_dStateTime = 0;
		m_dSendRecvtime = 0;
		m_sendvectbuf.ReleaseBuf();		
		m_sock = INVALID_SOCKET;
		m_bSending = false;

		UnLock();

		if (NULL != m_read_event)
		{
			event_del(m_read_event);
		}
		if (NULL != m_write_event)
		{
			event_del(m_write_event);
		}

		Lock();
		if (INVALID_SOCKET != sock)
		{
			WRITEFORMATINFOLOG(__THREADID__,__FILE__,__LINE__,"Clear","socket:%d",sock);
			MCloseScoket(sock);
		}
		UnLock();
	}

	void Lock()
	{
		m_lock.Lock();
	}

	void UnLock()
	{
		m_lock.UnLock();
	}

	void UpdateState(bool bState,bool bUpdateTime = false)
	{
		if (m_bState == bState)
		{
			return ;
		}

		m_bState = bState;
		if (bUpdateTime)
		{
			m_dStateTime = timeGetTime();
		}
	}

	//直接发送数据
	bool SendTcpData(char *apSendBuff, int aiSendLen)
	{
		if (NULL == m_pIocp)
		{
			return false;
		}

		return ((CSocketEvent*)m_pIocp)->SendTcpData(this,apSendBuff,aiSendLen);
	}

	virtual void DoRecv(SOCKET sock,char* pData,int iLen,char* pResponse,int& iResponseLen,char* pIP,int iPort)=0;

	SOCKET m_sock;
	SOCKADDR_IN m_ClientAddr;
	void* m_pIocp;
	DWORD m_dStateTime;
	DWORD m_dSendRecvtime;
	bool m_bUse;
	bool m_bState;
	CMLock m_lock;
	CBufVect m_sendvectbuf;
	bool m_bSending;
	event* m_write_event;
	event* m_read_event;
};
