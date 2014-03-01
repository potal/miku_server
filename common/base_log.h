#pragma once
#include "mlock.h"
#include "munit.h"
#include <vector>
#include <string>
using namespace std;

#ifdef _WIN32
#define __THREADID__ GetCurrentThreadId()
#else
#define __THREADID__ pthread_self()
#endif


#define WRITEFORMATDEBUGLOG CBaseLog::GetInstance()->WriteDebugFromatLog
#define WRITEFORMATINFOLOG CBaseLog::GetInstance()->WriteInfoFromatLog
#define WRITEFORMATERRORLOG CBaseLog::GetInstance()->WriteErrorFromatLog
#define WRITEFORMATLOG CBaseLog::GetInstance()->WriteFromatLog

#define WRITEUSERLOG CBaseLog::GetInstance()->WriteUserlog

#define WRITEDEBUGLOG(FunName,Log) CBaseLog::GetInstance()->WriteLog(LOG_DEBUG,__THREADID__,#FunName,Log,__FILE__,__LINE__)
#define WRITEINFOLOG(FunName,Log) CBaseLog::GetInstance()->WriteLog(LOG_INFO,__THREADID__,#FunName,Log,__FILE__,__LINE__)
#define WRITEERRORLOG(FunName,Log) CBaseLog::GetInstance()->WriteLog(LOG_ERROR,__THREADID__,#FunName,Log,__FILE__,__LINE__)
#define WRITELOG(iLevel,FunName,Log) CBaseLog::GetInstance()->WriteLog(iLevel,__THREADID__,#FunName,Log,__FILE__,__LINE__)

#define LOG_FUNC_IN(FunName) CBaseLog::GetInstance()->WriteLog(LOG_DEBUG,__THREADID__,#FunName,"IN",__FILE__,__LINE__)
#define LOG_FUNC_OUT(FunName) CBaseLog::GetInstance()->WriteLog(LOG_DEBUG,__THREADID__,#FunName,"OUT",__FILE__,__LINE__)

enum BASELOGLEVEL
{
	 LOG_DEBUG
	,LOG_INFO
	,LOG_ERROR
};

class CBaseLog
{
private:
	CBaseLog(void);
	virtual ~CBaseLog(void);

public:
	static CBaseLog* GetInstance()
	{
		if (NULL == m_pInstance)
		{
			m_pInstance = new CBaseLog();
		}
		return m_pInstance;
	}	
	static void DelInstacne()
	{
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

private:
	static CBaseLog* m_pInstance;

public:
	bool Init(char* pPath,char* pLogName);
	void SetLogLevel(int iLevel);
	void SetLogBackupNum(unsigned int iNum);
	void CheckWriteLogFun();

	void WriteLog(BASELOGLEVEL iLevel,unsigned long lThreadId,const char* pFunName,const char* pLog,const char* pFileName,const unsigned int iLine);
	void WriteFromatLog(BASELOGLEVEL iLevel,unsigned long lThreadId,const char* pFileName,const unsigned int iLine,const char* pFunName,const char* pLog,...);
	void WriteDebugFromatLog(unsigned long lThreadId,const char* pFileName,const unsigned int iLine,const char* pFunName,const char* pLog,...);
	void WriteInfoFromatLog(unsigned long lThreadId,const char* pFileName,const unsigned int iLine,const char* pFunName,const char* pLog,...);
	void WriteErrorFromatLog(unsigned long lThreadId,const char* pFileName,const unsigned int iLine,const char* pFunName,const char* pLog,...);

	void WriteUserlog(const char* pLog,...);
private:
	char* GetLevel(BASELOGLEVEL iLevel);
	vector<string>* GetLogVect(bool bUser);
	void WriteLogToDisk(vector<string>* pVect,bool bUser);
	FILE* OpenFilePtr(bool bUser);
	void CloseFilePtr(FILE* pFile,bool bUser);
	void RefleshLogFile(bool bUser);

private:
	//日志缓存
	CMLock m_lock;
	vector<string> m_logvect1;
	vector<string> m_logvect2;
	vector<string>* m_pLogvect;
	
	CMLock m_userlock;
	vector<string> m_uservect1;
	vector<string> m_uservect2;
	vector<string>* m_pUservect;

private:
	//路径
	char m_szPath[256];
	//日志前缀
	char m_szLogName[128];
	//日志个数
	unsigned int m_uiLogNum;
	//日志等级
	int m_iLogLevel;
	//线程控制开关
	bool m_bRunning;
	//文件保存线程句柄
	MThreadID m_handleWritefile;
};
