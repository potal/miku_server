#pragma once
#include "dispatcher.h"
#include "mlock.h"

class CDispatchers
{
public:
	CDispatchers(void);
	virtual ~CDispatchers(void);

public:
	//初始化
	bool Initialize(unsigned int uiCount);

	//激活，开启相关线程
	bool Active();

	//停止处理
	bool Terminate();

	//加载会场
	bool LoadObject(CDispatchClient* pClient,char* pValue);

	//卸载会场
	bool UnLoadObject(CDispatchClient* pClient,char* pValue);

private:
	//分发线程组
	vector<CDispatcher> m_Dispatchers;
	
	CMLock m_lock;

};
