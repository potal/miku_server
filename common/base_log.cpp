#include "base_log.h"
#ifdef _WIN32
#include <time.h>
#else
#include <sys/time.h>
#include <stdarg.h>
#endif

CBaseLog* CBaseLog::m_pInstance = NULL;

#define MAX_LOG_FILE_SISE  (20*1024*1024)
#define MAX_LOG_ONELINE		(1900)

CBaseLog::CBaseLog(void)
{
	m_pLogvect = &m_logvect1;
	m_pUservect = &m_uservect1;
	memset(m_szPath,0,sizeof(m_szPath));
	memset(m_szLogName,0,sizeof(m_szLogName));
	m_iLogLevel = LOG_DEBUG;
	m_bRunning = false;
	m_uiLogNum = 10;
	m_handleWritefile = NULL;
}

CBaseLog::~CBaseLog(void)
{
	m_bRunning = false;

	if (NULL != m_handleWritefile)
	{
		MWaitThread(m_handleWritefile);
	}
	
	WriteLogToDisk(&m_logvect1,false);
	WriteLogToDisk(&m_logvect2,false);
}


TTHREAD_FUNC_RESULT LogWriteThread(TTHREAD_FUNC_PARAMETER lpParam)
{
	CBaseLog* pSer = (CBaseLog*)lpParam;

	if (NULL != pSer)
	{
		pSer->CheckWriteLogFun();
	}

	return 0;
}


bool CBaseLog::Init(char* pPath,char* pLogName)
{
	if (NULL == pPath || m_bRunning)
	{
		return false;
	}

	memset(m_szPath,0,sizeof(m_szPath));
	memset(m_szLogName,0,sizeof(m_szLogName));

	if (NULL == pLogName)
	{
		strncpy(m_szLogName,"BaseLog",sizeof(m_szLogName)-1);
	}
	else
	{
		strncpy(m_szLogName,pLogName,sizeof(m_szLogName)-1);
	}
	
	strncpy(m_szPath,pPath,sizeof(m_szPath)-1);

	if (!TestAndCreatePath(pPath))
	{
		return false;
	}

	RefleshLogFile(false);

	m_bRunning = true;

	m_handleWritefile = MCreateThread(LogWriteThread,(void *)this);

	return true;
}

void CBaseLog::SetLogLevel(int iLevel)
{
	m_iLogLevel = iLevel;
}

void CBaseLog::SetLogBackupNum(unsigned int iNum)
{
	if (iNum > 1 && iNum < 100)
	{
		m_uiLogNum = iNum;
	}
}

void CBaseLog::WriteFromatLog(BASELOGLEVEL iLevel,unsigned long lThreadId,const char* pFileName,const unsigned int iLine,const char* pFunName,const char* pLog,...)
{
	if (iLevel < m_iLogLevel || NULL == pFunName || NULL == pLog)
	{
		return;
	}

	va_list argptr;
	char szFormatLog[MAX_LOG_ONELINE] = "";

	//把可变参数序列化到strPrintBuff中
	va_start(argptr,pLog);
#ifdef _WIN32
	vsprintf_s(szFormatLog, MAX_LOG_ONELINE-1, pLog, argptr);
#else
	vsnprintf(szFormatLog, MAX_LOG_ONELINE-1, pLog, argptr);
#endif
	va_end( argptr );
	
	WriteLog(iLevel,lThreadId,pFunName,szFormatLog,pFileName,iLine);
}

void CBaseLog::WriteDebugFromatLog(unsigned long lThreadId,const char* pFileName,const unsigned int iLine,const char* pFunName,const char* pLog,...)
{
	if (LOG_DEBUG < m_iLogLevel || NULL == pFunName || NULL == pLog)
	{
		return;
	}

	va_list argptr;
	char szFormatLog[MAX_LOG_ONELINE] = "";

	//把可变参数序列化到strPrintBuff中
	va_start(argptr,pLog);
#ifdef _WIN32
	vsprintf_s(szFormatLog, MAX_LOG_ONELINE-1, pLog, argptr);
#else
	vsnprintf(szFormatLog, MAX_LOG_ONELINE-1, pLog, argptr);
#endif
	va_end( argptr );

	WriteLog(LOG_DEBUG,lThreadId,pFunName,szFormatLog,pFileName,iLine);
}

void CBaseLog::WriteInfoFromatLog(unsigned long lThreadId,const char* pFileName,const unsigned int iLine,const char* pFunName,const char* pLog,...)
{
	if (LOG_INFO < m_iLogLevel || NULL == pFunName || NULL == pLog)
	{
		return;
	}

	va_list argptr;
	char szFormatLog[MAX_LOG_ONELINE] = "";

	//把可变参数序列化到strPrintBuff中
	va_start(argptr,pLog);
#ifdef _WIN32
	vsprintf_s(szFormatLog, MAX_LOG_ONELINE-1, pLog, argptr);
#else
	vsnprintf(szFormatLog, MAX_LOG_ONELINE-1, pLog, argptr);
#endif
	va_end( argptr );

	WriteLog(LOG_INFO,lThreadId,pFunName,szFormatLog,pFileName,iLine);
}
void CBaseLog::WriteErrorFromatLog(unsigned long lThreadId,const char* pFileName,const unsigned int iLine,const char* pFunName,const char* pLog,...)
{
	if (LOG_ERROR < m_iLogLevel || NULL == pFunName || NULL == pLog)
	{
		return;
	}

	va_list argptr;
	char szFormatLog[MAX_LOG_ONELINE] = "";

	//把可变参数序列化到strPrintBuff中
	va_start(argptr,pLog);
#ifdef _WIN32
	vsprintf_s(szFormatLog, MAX_LOG_ONELINE-1, pLog, argptr);
#else
	vsnprintf(szFormatLog, MAX_LOG_ONELINE-1, pLog, argptr);
#endif
	va_end( argptr );

	WriteLog(LOG_ERROR,lThreadId,pFunName,szFormatLog,pFileName,iLine);
}


void CBaseLog::WriteLog(BASELOGLEVEL iLevel,unsigned long lThreadId,const char* pFunName,const char* pLog,const char* pFileName,const unsigned int iLine)
{
	if (iLevel < m_iLogLevel || NULL == pFunName || NULL == pLog)
	{
		return;
	}

	unsigned int  uiLen = strlen(pFunName) + strlen(pLog) + strlen(pFileName);
	if (uiLen > MAX_LOG_ONELINE)
	{
		return;
	}
	
	char szLog[2048]="";

#ifdef _WIN32
	SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	sprintf(szLog,"%4d-%02d-%02d %02d:%02d:%02d.%03d %s %08ul [%s] %s(%s:%d)\n",sys.wYear,sys.wMonth,sys.wDay
		,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds
		,GetLevel(iLevel),lThreadId,pFunName,pLog,pFileName,iLine);
#else
	struct timeval t;
	gettimeofday(&t, NULL);
	struct tm * ptm=localtime(&t.tv_sec);

	sprintf(szLog,"%4d-%02d-%02d %02d:%02d:%02d.%03d %s %08ul [%s] %s(%s:%d)\n",ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday
		,ptm->tm_hour,ptm->tm_min,ptm->tm_sec,t.tv_usec/1000
		,GetLevel(iLevel),lThreadId,pFunName,pLog,pFileName,iLine);

#endif

	m_lock.Lock();
	if (NULL != m_pLogvect)
	{
		m_pLogvect->push_back(szLog);
	}
	m_lock.UnLock();
}

void CBaseLog::WriteUserlog(const char* pLog,...)
{
	char szLog[2048]="";

	va_list argptr;
	char szFormatLog[MAX_LOG_ONELINE] = "";

	//把可变参数序列化到strPrintBuff中
	va_start(argptr,pLog);
#ifdef _WIN32
	vsprintf_s(szFormatLog, MAX_LOG_ONELINE-1, pLog, argptr);
#else
	vsnprintf(szFormatLog, MAX_LOG_ONELINE-1, pLog, argptr);
#endif
	va_end( argptr );


#ifdef _WIN32
	SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	sprintf(szLog,"%4d-%02d-%02d %02d:%02d:%02d,%03d %s \n",sys.wYear,sys.wMonth,sys.wDay
		,sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds,szFormatLog);
#else
	struct timeval t;
	gettimeofday(&t, NULL);
	struct tm * ptm=localtime(&t.tv_sec);

	sprintf(szLog,"%4d-%02d-%02d %02d:%02d:%02d,%03d %s\n",ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday
		,ptm->tm_hour,ptm->tm_min,ptm->tm_sec,t.tv_usec/1000,szFormatLog);

#endif

	m_userlock.Lock();
	if (NULL != m_pUservect)
	{
		m_pUservect->push_back(szLog);
	}
	m_userlock.UnLock();
}


char* CBaseLog::GetLevel(BASELOGLEVEL iLevel)
{
	switch (iLevel)
	{
	case LOG_DEBUG:
		return "LOG_DEBUG";
	case LOG_INFO:
		return "LOG_INFO";
	case LOG_ERROR:
		return "LOG_ERROR";
	default:
		break;
	}

	return "LOG_UNKNOW";
}

void CBaseLog::CheckWriteLogFun()
{
	vector<string>* pVect = NULL;
	while (m_bRunning)
	{
		pVect = GetLogVect(false);
		if (NULL != pVect)
		{
			WriteLogToDisk(pVect,false);
		}

		pVect = GetLogVect(true);
		if (NULL != pVect)
		{
			WriteLogToDisk(pVect,true);
		}

		MSleep(200);
	}
}

FILE* CBaseLog::OpenFilePtr(bool bUser)
{
	char szFileName[512]="";

	if(!bUser)
	{

#ifdef _WIN32
		sprintf(szFileName,"%s\\%s-1.log",m_szPath,m_szLogName);
#else
		sprintf(szFileName,"%s/%s-1.log",m_szPath,m_szLogName);
#endif
	}
	else
	{
#ifdef _WIN32
		SYSTEMTIME sys; 
		GetLocalTime( &sys ); 
		sprintf(szFileName,"%s\\user-%4d%02d%02d%02d.log",m_szPath,sys.wYear,sys.wMonth,sys.wDay,sys.wHour);
#else
		struct timeval t;
		gettimeofday(&t, NULL);
		struct tm * ptm=localtime(&t.tv_sec);
		sprintf(szFileName,"%s/user-%4d%02d%02d%02d.log",m_szPath,ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour);
#endif
	}
	
	FILE* pFile = fopen(szFileName,"a");

	return pFile;
}

void CBaseLog::CloseFilePtr(FILE* pFile,bool bUser)
{
	if (NULL == pFile)
	{
		return;
	}

	fseek(pFile,0,SEEK_END);
	long lFileSize = ftell(pFile);
	
	fclose(pFile);

	if (lFileSize >= MAX_LOG_FILE_SISE)
	{
		RefleshLogFile(bUser);
	}
}

void CBaseLog::RefleshLogFile(bool bUser)
{
	for(int i= m_uiLogNum;i>1;i--)
	{
		char szFileName[512]="";
		char szPreFileName[512]="";
		
		if(!bUser)
		{
#ifdef _WIN32
			sprintf(szFileName,"%s\\%s-%d.log",m_szPath,m_szLogName,i);
			sprintf(szPreFileName,"%s\\%s-%d.log",m_szPath,m_szLogName,i-1);
			DeleteFile(szFileName);
#else
			sprintf(szFileName,"%s/%s-%d.log",m_szPath,m_szLogName,i);
			sprintf(szPreFileName,"%s/%s-%d.log",m_szPath,m_szLogName,i-1);
			remove(szFileName);
#endif
			rename(szPreFileName,szFileName);	
		}			
	}
}

void CBaseLog::WriteLogToDisk(vector<string>* pVect,bool bUser)
{
	if (pVect == NULL || pVect->size() < 1)
	{
		return;
	}
	
	FILE* pFile = OpenFilePtr(bUser);
	if (NULL == pFile)
	{
		pVect->clear();
		return;
	}

	vector<string>::iterator iter = pVect->begin();
	for (;iter != pVect->end();iter++)
	{	
		fwrite(iter->c_str(),sizeof( char ),iter->length(),pFile);
	}
	
	pVect->clear();

	CloseFilePtr(pFile,bUser);
}


vector<string>* CBaseLog::GetLogVect(bool bUser)
{
	if(!bUser)
	{
		CMAutoLock autolock(&m_lock);
		if (NULL == m_pLogvect)
		{
			return NULL;
		}

		if (m_pLogvect->size() < 1)
		{
			return NULL;
		}


		if (m_pLogvect == &m_logvect1)
		{
			m_pLogvect = &m_logvect2;
			return &m_logvect1;
		}
		else if (m_pLogvect == &m_logvect2)
		{
			m_pLogvect = &m_logvect1;
			return &m_logvect2;
		}
	}
	else
	{
		CMAutoLock autolock(&m_userlock);
		if (NULL == m_pUservect)
		{
			return NULL;
		}

		if (m_pUservect->size() < 1)
		{
			return NULL;
		}

		if (m_pUservect == &m_uservect1)
		{
			m_pUservect = &m_uservect2;
			return &m_uservect1;
		}
		else if (m_pUservect == &m_uservect2)
		{
			m_pUservect = &m_uservect1;
			return &m_uservect2;
		}
	}

	return NULL;
}

