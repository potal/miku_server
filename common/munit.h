#pragma once

#include "base_header.h"

#define TCP_BUF_LENGTH 4096
#define BUF_UNIT_LENGTH 10240

#include <time.h>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#include <shlwapi.h>
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"Shlwapi.lib")
#else
#include <sys/stat.h>
#include <dirent.h>
#include <functional>
#include <algorithm>

#endif


#ifdef _WIN32

typedef HANDLE	MThreadID;
//�̺߳�����������
#define TTHREAD_FUNC_PARAMETER LPVOID
//�̺߳�����������
#define TTHREAD_FUNC_RESULT DWORD WINAPI 

#define SOCKET_NOERROR  WSAEWOULDBLOCK 

#else

typedef pthread_t    MThreadID;
//�̺߳�����������
typedef void* TTHREAD_FUNC_PARAMETER;
//�̺߳�����������
typedef void* TTHREAD_FUNC_RESULT;
typedef void* (*LPTHREAD_START_ROUTINE)(void*); 
typedef void* LPVOID;

#define SOCKET_NOERROR  EAGAIN

#endif

//�ȴ��̷߳���
inline void MWaitThread(MThreadID threadID)
{
#ifdef _WIN32
	WaitForSingleObject(threadID, INFINITE);
#else
	pthread_join(threadID, NULL);
#endif
};

//�����߳�
inline MThreadID MCreateThread(LPTHREAD_START_ROUTINE ThreadFunc, LPVOID Parameter)
{
#ifdef _WIN32
	MThreadID threadID = CreateThread(NULL, 0, ThreadFunc, Parameter, 0, NULL); 		
	if(NULL != threadID)
#else
	MThreadID threadID = 0;
	int ret =  pthread_create(&threadID, NULL, ThreadFunc, Parameter);
	if(0 == ret)
#endif		
	{
		return threadID;
	}
	else
	{
		return 0;
	}
};

//��Ϣ����(��λ : ms)
inline void MSleep(int uiMilliSecond)
{
#ifdef _WIN32
	Sleep(uiMilliSecond);
#else
	usleep(uiMilliSecond*1000); //usleep(��λ : ΢��)
#endif
};

//��Ϣ����(��λ : ms)
inline void MCloseScoket(SOCKET sock)
{
#ifdef _WIN32
	shutdown(sock,2);
	closesocket(sock);
	
#else
	shutdown(sock,2);
	close(sock);
#endif
	
};

// �õ�socket�Ĵ���
inline int MGetLastError()
{	
#ifdef _WIN32	
	return WSAGetLastError();
#else
	return errno;
#endif 	
}


#ifdef _WIN32	

#else
inline DWORD timeGetTime()
{
	DWORD uptime = 0;
	struct timespec on;
	if(clock_gettime(CLOCK_MONOTONIC, &on) == 0)
		uptime = on.tv_sec*1000 + on.tv_nsec/1000000;
	return uptime;
}

inline DWORD GetTickCount()
{
	DWORD uptime = 0;
	struct timespec on;
	if(clock_gettime(CLOCK_MONOTONIC, &on) == 0)
		uptime = on.tv_sec*1000 + on.tv_nsec/1000000;
	return uptime;
}
#endif


// ��ȡ����װĿ¼ 
inline void GetProgramDir(char* pPath,int iLen)
{
	if (NULL == pPath)
	{
		return;
	}

#ifdef _WIN32
	char szPathName[MAX_PATH]={0};
	char strPath[MAX_PATH]={0};
	::GetModuleFileName(NULL , szPathName , MAX_PATH);
	::PathRemoveFileSpec(szPathName);
	strncpy(pPath,szPathName,iLen);

#else
	char szlink[512]="";
	int rslt = readlink("/proc/self/exe", szlink, sizeof(szlink)-1);
	if (rslt < 0 || rslt >= sizeof(szlink))
	{
		printf("get path error\n");
		return;
	}

	int iStrLen = strlen(szlink);
	for(int i=iStrLen;i>0;i--)
	{
		if('/' == szlink[i])
		{
			strncpy(pPath,szlink,i);
		}
	}
#endif
}

//��֤������Ŀ¼
inline bool TestAndCreatePath(char* pStrPath)
{
	if(NULL == pStrPath)
	{
		return false;
	}

	if(0 != access(pStrPath, 0 ))
	{
#ifdef _WIN32
		if(0 != mkdir(pStrPath))
		{
			return false;
		}
#else
		if(0 != mkdir(pStrPath,S_IRWXU))
		{
			return false;
		}
#endif
	}

	return true;
}
