#include "stdafx.h"
#include "socket_event.h"

#ifdef _WIN32

#pragma comment(lib,"libevent_core.lib")

#include <io.h>
#include <fcntl.h>

#endif

TTHREAD_FUNC_RESULT IOCPWorkerThread(TTHREAD_FUNC_PARAMETER lpParam)
{
	StuThraedInfo* pSer = (StuThraedInfo*)lpParam;
	if (NULL == pSer)
	{
		return 0;
	}

	printf("Woker %d Thread Runing\r\n",pSer->m_iID);
	
	CSocketEvent* pSock = (CSocketEvent*)pSer->m_pIocp;
	pSock->Do(pSer->m_iID);
	
	printf("Worker %d Thread stop\r\n",pSer->m_iID);
	return 0;
}


TTHREAD_FUNC_RESULT IOCPTimeOutThread(TTHREAD_FUNC_PARAMETER lpParam)
{
	CSocketEvent* pSer = (CSocketEvent*)lpParam;
	printf("TimeOut Thread Runing\r\n");
	if (NULL != pSer)
	{
		pSer->CheckTimeOut();
	}
	printf("TimeOut Thread stop\r\n");
	return 0;
}

TTHREAD_FUNC_RESULT StartThread(TTHREAD_FUNC_PARAMETER lpParam)
{
	CSocketEvent* pSer = (CSocketEvent*)lpParam;
	printf("StartThread Runing\r\n");
	if (NULL != pSer)
	{
		pSer->Dispatch();
	}
	printf("StartThread stop\r\n");
	return 0;
}

CSocketEvent::CSocketEvent(void)
{
	m_serSock = INVALID_SOCKET;
	m_pRes = NULL;
	m_iIndex = 0;

}

CSocketEvent::~CSocketEvent(void)
{
	Stop();
}

void do_write(evutil_socket_t fd, short events, void *arg)
{
	CSocketContext* pContext = (CSocketContext*)arg;
	if (NULL == pContext)
	{
		return;
	}
	
	int iSize = 0;
	int iCount = 5;
	pContext->Lock();
	while(iCount-- > 0)
	{
		char* pStart = pContext->m_sendvectbuf.GetBuf(iSize);
		if (iSize < 1)
		{
			event_del(pContext->m_write_event);
			pContext->m_bSending = false;
			break;
		}
		int iSend = send(fd,pStart,iSize,0);
		int iErrorCoe = MGetLastError();
		if (iSend > 0)
		{
			pContext->m_sendvectbuf.DelBuf(iSend);
		}
		else if(0 == iSend || iErrorCoe == SOCKET_NOERROR)
		{
			break;
		}
		else 
		{
			pContext->UnLock();
			WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"do_write","fd:%d,iSend:%d,ErrorCode:%d",fd,iSend,iErrorCoe);
			((CSocketEvent*)pContext->m_pIocp)->ReleaseContext(pContext);
			return;
		}
	}
	pContext->UnLock();
}

void do_read(evutil_socket_t fd, short events, void *arg)
{
	CSocketContext* pContext = (CSocketContext*)arg;
	if (NULL == pContext)
	{
		return;
	}

	char szRecv[TCP_BUF_LENGTH]="";
	int iSize = 0;
	char szResponse[TCP_BUF_LENGTH]="";
	int iResponse = 0;

	iSize = recv(fd, szRecv, sizeof(szRecv), 0);
	if (iSize > 0)
	{
		pContext->DoRecv(pContext->m_sock,szRecv,iSize,szResponse,iResponse,inet_ntoa(pContext->m_ClientAddr.sin_addr),ntohs(pContext->m_ClientAddr.sin_port));
		if (iResponse > 0)
		{
			if (NULL != pContext->m_pIocp)
			{
				((CSocketEvent*)pContext->m_pIocp)->SendTcpData(pContext,szResponse,iResponse);
			}
		}
	}
	else if (iSize <= 0)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"do_read","fd:%d,iSize:%d,ErrorCode:%d",fd,iSize,MGetLastError());
		((CSocketEvent*)pContext->m_pIocp)->ReleaseContext(pContext);
	}
}

void do_accept(evutil_socket_t listener, short event, void *arg)
{
	CSocketEvent* pSock = (CSocketEvent*)arg;
	if (NULL == pSock)
	{
		return;
	}
	
	struct event_base *base = pSock->base;
	SOCKADDR_IN ss;
#ifdef _WIN32
	int slen = sizeof(ss);
#else
	socklen_t slen = sizeof(ss);
#endif
	int fd = accept(listener, (sockaddr*)&ss, &slen);

	if (fd < 0)
	{ 
		printf("accept fd < 0\r\n");
	}
	else
	{
		CSocketContext* pContext = pSock->GetContext();
		if (NULL == pContext)
		{
			return;
		}

		evutil_make_socket_nonblocking(fd);

		pContext->m_sock = fd;
		pContext->m_pIocp = pSock;
		pContext->m_bState = false;
		pContext->m_dStateTime = timeGetTime();
		pContext->m_ClientAddr = ss;
		pContext->Init();

		WRITEFORMATINFOLOG(__THREADID__,__FILE__,__LINE__,"do_accept","accept %s:%d connect sock:%d",inet_ntoa(pContext->m_ClientAddr.sin_addr),ntohs(pContext->m_ClientAddr.sin_port),pContext->m_sock);

		pSock->m_listlock.Lock();
		pSock->m_list.push_back(pContext);
		WRITEFORMATINFOLOG(__THREADID__,__FILE__,__LINE__,"do_accept","m_list size:%d",pSock->m_list.size());
		pSock->m_listlock.UnLock();
		
		pSock->m_iIndex++;
		pSock->m_iIndex = pSock->m_iIndex%pSock->m_iThreadCount;

		pSock->m_pThreadInfo[pSock->m_iIndex].m_lock.Lock();
		pSock->m_pThreadInfo[pSock->m_iIndex].m_list.push_back(pContext);
		pSock->m_pThreadInfo[pSock->m_iIndex].m_lock.UnLock();
		
#ifdef _WIN32
		send(pSock->m_pThreadInfo[pSock->m_iIndex].notify_send_fd,"1",1,0);
#else
		write(pSock->m_pThreadInfo[pSock->m_iIndex].notify_send_fd,"1",1);
#endif
	}
}

//启动完成端口的SOKCET服务
bool CSocketEvent::Start(char* pIP,int iPort,int iTimeout,CSocketRes* pRes)
{
	if (NULL == pIP || NULL == pRes)
	{
		return false;
	}

	m_iTimeout = iTimeout;
	
#ifdef _WIN32
	if(0 != evthread_use_windows_threads())
	{
		printf("evthread_use_windows_threads failed\r\n");
	}
#else
	if(0 != evthread_use_pthreads())
	{
		printf("evthread_use_pthreads failed\r\n");
	}

	signal(SIGPIPE, SIG_IGN);

#endif

	base = event_base_new();
	if (!base) 
	{
		fprintf(stderr, "Could not initialize libevent!\n");
		return false;
	}

	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	//sin.sin_addr.s_addr = 0;//本机
	sin.sin_port = htons(iPort);

#ifdef _WIN32

	if (0 == strlen(pIP))
	{
		sin.sin_addr.S_un.S_addr = INADDR_ANY;
	}
	else
	{
		sin.sin_addr.S_un.S_addr = inet_addr(pIP);
	}
#else
	if (0 == strlen(pIP))
	{
		sin.sin_addr.s_addr = inet_addr(INADDR_ANY);
	}
	else
	{
		sin.sin_addr.s_addr = inet_addr(pIP);
	}
#endif
	

	m_serSock = socket(AF_INET, SOCK_STREAM, 0);
	
	if (bind(m_serSock, (sockaddr*)&sin, sizeof(sin)) < 0)
	{
		//perror("bind");
		printf("bind failed:%d\r\n",MGetLastError());
		return false;
	}
	if (listen(m_serSock, 16)<0)
	{
		perror("listen");
		return false;
	}

	evutil_make_socket_nonblocking(m_serSock);

	listener_event = event_new(base, m_serSock, EV_READ|EV_PERSIST, do_accept, (void*)this);
	
	event_add(listener_event, NULL);
	
	m_pRes = pRes;

	m_bRunning =true;

	m_hTimeOutHandle = MCreateThread(StartThread,(void *)this);

	//开启超时检测线程
	m_hTimeOutHandle = MCreateThread(IOCPTimeOutThread,(void *)this);

	m_iThreadCount = 4;
	m_pWorkHandle = new MThreadID[m_iThreadCount];
	m_pThreadInfo = new StuThraedInfo[m_iThreadCount];

	for (int i=0;i<m_iThreadCount;i++)
	{
		m_pThreadInfo[i].m_iID = i;
		m_pThreadInfo[i].m_pIocp = this;

		m_pWorkHandle[i] = MCreateThread(IOCPWorkerThread,(void *)&m_pThreadInfo[i]);
	}
	
	return true;
}

//停止
bool CSocketEvent::Stop()
{
	m_bRunning = false;

	if (NULL != m_pThreadInfo)
	{
		for (int i=0;i<m_iThreadCount;i++)
		{
			event_base_loopbreak(m_pThreadInfo[i].m_base);
		}
	}

	if (NULL != base)
	{
		event_base_loopbreak(base);
	}

	if (NULL != m_pWorkHandle)
	{
		if (NULL != m_pWorkHandle)
		{
			for (int i=0;i<m_iThreadCount;i++)
			{
				MWaitThread(m_pWorkHandle[i]);
			}

			delete [] m_pWorkHandle;
			m_pWorkHandle = NULL;
		}
	}

	if (NULL != m_hTimeOutHandle)
	{
		MWaitThread(m_hTimeOutHandle);
		m_hTimeOutHandle = NULL;
	}

	return true;
}

void CSocketEvent::Dispatch()
{
	while(m_bRunning)
	{
		event_base_dispatch(base);

		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"CIOCPSock::Dispatch","event_base_dispatch exit:%d",MGetLastError());
	}	

	event_del(listener_event);
	event_free(listener_event);
	event_base_free(base);
	base = NULL;
}

static void pipe_read_cb(evutil_socket_t fd, short event, void *arg)
{
	LOG_FUNC_IN(pipe_read_cb);
	char buf[2]="";
#ifdef _WIN32
	if (recv(fd, buf, 1,0) != 1)
#else
	if (read(fd, buf, 1) != 1)
#endif
	{
		WRITEFORMATINFOLOG(__THREADID__,__FILE__,__LINE__,"pipe_read_cb","read return is not 1");
		return;
	}

	StuThraedInfo* pThread = (StuThraedInfo*)arg;
	pThread->m_lock.Lock();
	list<CSocketContext*>::iterator iter;
	for (iter = pThread->m_list.begin();iter!=pThread->m_list.end();iter++)
	{
		CSocketContext* pContext = *iter;
		CSocketEvent* pSock = (CSocketEvent*)pThread->m_pIocp;

		WRITEFORMATINFOLOG(__THREADID__,__FILE__,__LINE__,"pipe_read_cb","accept %s:%d connect sock:%d",inet_ntoa(pContext->m_ClientAddr.sin_addr),ntohs(pContext->m_ClientAddr.sin_port),pContext->m_sock);
		
		if (NULL == pContext->m_read_event)
		{
			pContext->m_read_event = event_new(pThread->m_base, pContext->m_sock, EV_READ|EV_PERSIST, do_read, pContext);
		}
		else
		{
			event_assign(pContext->m_read_event,pThread->m_base, pContext->m_sock, EV_READ|EV_PERSIST, do_read, pContext);
		}

		if (NULL == pContext->m_write_event)
		{
			pContext->m_write_event = event_new(pThread->m_base, pContext->m_sock, EV_WRITE|EV_PERSIST, do_write, pContext);
		}
		else
		{
			event_assign(pContext->m_write_event,pThread->m_base, pContext->m_sock, EV_WRITE|EV_PERSIST, do_write, pContext);
		}

		event_add(pContext->m_read_event, NULL);
	}
	pThread->m_list.clear();
	pThread->m_lock.UnLock();

	LOG_FUNC_OUT(pipe_read_cb);
}

#ifdef _WIN32

int pipe(int fildes[2])
{
	int tcp1, tcp2;
	sockaddr_in name;
	memset(&name, 0, sizeof(name));
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	int namelen = sizeof(name);
	tcp1 = tcp2 = -1;
	int tcp = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp == -1){
		goto clean;
	}
	if (bind(tcp, (sockaddr*)&name, namelen) == -1){
		goto clean;
	}
	if (listen(tcp, 5) == -1){
		goto clean;
	}
	if (getsockname(tcp, (sockaddr*)&name, &namelen) == -1){
		goto clean;
	}
	tcp1 = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp1 == -1){
		goto clean;
	}
	if (-1 == connect(tcp1, (sockaddr*)&name, namelen)){
		goto clean;
	}
	tcp2 = accept(tcp, (sockaddr*)&name, &namelen);
	if (tcp2 == -1){
		goto clean;
	}

	MCloseScoket(tcp);
	
	fildes[0] = tcp1;
	fildes[1] = tcp2;
	return 0;
clean:
	if (tcp != -1){
		MCloseScoket(tcp);
	}
	if (tcp2 != -1){
		MCloseScoket(tcp2);
	}
	if (tcp1 != -1){
		MCloseScoket(tcp1);
	}
	return -1;
}
#endif

void CSocketEvent::Do(int iIndex)
{
	if (iIndex >= m_iThreadCount)
	{
		return;
	}

	m_pThreadInfo[iIndex].m_base = event_base_new();

	int hpipe[2];

	if(pipe(hpipe))
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"CIOCPSock::Do","_pipe return failed id:%d",iIndex);
		return;
	}
	
	m_pThreadInfo[iIndex].notify_receive_fd = hpipe[0];
	m_pThreadInfo[iIndex].notify_send_fd = hpipe[1];

	/* Initalize one event */
	m_pThreadInfo[iIndex].m_event = event_new(m_pThreadInfo[iIndex].m_base, m_pThreadInfo[iIndex].notify_receive_fd
		, EV_READ|EV_PERSIST, pipe_read_cb, (void*) &m_pThreadInfo[iIndex]);
	
	event_add(m_pThreadInfo[iIndex].m_event,NULL);
	
	while(m_bRunning)
	{
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"CIOCPSock::Do","event_base_loop begin iIndex:%d",iIndex);
		event_base_loop(m_pThreadInfo[iIndex].m_base, 0);
		WRITEFORMATERRORLOG(__THREADID__,__FILE__,__LINE__,"CIOCPSock::Do","event_base_loop exit Code:%d",MGetLastError());
	}

	event_del(m_pThreadInfo[iIndex].m_event);
	event_free(m_pThreadInfo[iIndex].m_event);
	m_pThreadInfo[iIndex].m_event = NULL;
	event_base_free(m_pThreadInfo[iIndex].m_base);
}
//检查超时连接
void CSocketEvent::CheckTimeOut()
{
	bool bDel = false;
	DWORD dCurrent = 0;
	while (NULL != base)
	{
		CSocketContext* pContext = NULL;
		m_listlock.Lock();

		if (m_list.size() < 1)
		{
			m_listlock.UnLock();
			MSleep(50);
			continue;
		}

		pContext = m_list.front();
		m_list.pop_front();
		m_list.push_back(pContext);

		m_listlock.UnLock();

		if (NULL != pContext)
		{
			bDel = false;
			dCurrent = timeGetTime();
			if (INVALID_SOCKET == pContext->m_sock)
			{
				bDel = true;
			}
			else if (!pContext->m_bState && (dCurrent - pContext->m_dStateTime) > m_iTimeout)
			{
				bDel = true;
			}
			else
			{
				int iRet = send(pContext->m_sock,"",0,0);
				if (iRet < 0)
				{
				    int iCode = MGetLastError();
					if (iCode != SOCKET_NOERROR)
					{
						WRITEFORMATINFOLOG(__THREADID__,__FILE__,__LINE__,"CIOCPSock::CheckTimeOut","Socket:%d,iRet %d,iCode:%d",pContext->m_sock,iRet,iCode);
						bDel = true;
					}
				}
			}
			
			if (bDel)
			{
				WRITEFORMATINFOLOG(__THREADID__,__FILE__,__LINE__,"CIOCPSock::CheckTimeOut","close %s:%d connect sock:%d",inet_ntoa(pContext->m_ClientAddr.sin_addr),ntohs(pContext->m_ClientAddr.sin_port),pContext->m_sock);
				ReleaseContext(pContext);
			}
			else
			{
				MSleep(50);
			}	
		}
	}
}

//直接发送数据
bool CSocketEvent::SendTcpData(CSocketContext* pContext,char *apSendBuff, int aiSendLen)
{
	if (NULL == pContext || NULL == apSendBuff || aiSendLen < 0 || !m_bRunning)
	{
		return false;
	}

	pContext->m_sendvectbuf.AddBuf(apSendBuff,aiSendLen);

	pContext->Lock();

	if (!pContext->m_bSending && INVALID_SOCKET != pContext->m_sock && pContext->m_bUse)
	{
		event_add(pContext->m_write_event, NULL);
		pContext->m_bSending = true;
	}

	pContext->UnLock();

	return true;
}

//获取Context
CSocketContext* CSocketEvent::GetContext()
{
	CSocketContext* pReturn = NULL;
	if (NULL != m_pRes)
	{
		pReturn = m_pRes->GetContext();
	}
	
	if (NULL != pReturn)
	{
		pReturn->Clear();
	}
	
	return pReturn;
}

//释放Context 
void CSocketEvent::ReleaseContext(CSocketContext* pContext)
{
	if (NULL == pContext)
	{
		return;
	}
	
	m_listlock.Lock();

	WRITEFORMATINFOLOG(__THREADID__,__FILE__,__LINE__,"CIOCPSock::ReleaseContext","close %s:%d connect,scoket:%d,m_list size:%d"
		,inet_ntoa(pContext->m_ClientAddr.sin_addr),ntohs(pContext->m_ClientAddr.sin_port),pContext->m_sock,m_list.size());

	if (NULL != m_pRes)
	{
		m_pRes->ReleaseContext(pContext);
	}
	
	list<CSocketContext*>::iterator iter;
	for (iter=m_list.begin();iter != m_list.end();iter++)
	{
		if (*iter == pContext)
		{
			m_list.erase(iter);
			break;
		}
	}
	m_listlock.UnLock();
}

