#pragma once
#include "munit.h"
#include "mlock.h"
#include <vector>
using namespace std;

class CBufUnit
{
public:
	CBufUnit(void);

	virtual ~CBufUnit(void);

	bool AddBuf(char*& pData,int& iLen);
	
	inline char* GetBuf();

	inline int GetBufLen();

	inline void Delbuf(int iLen);

private:
	char m_szData[BUF_UNIT_LENGTH];
	int m_iLen;
	int m_iReadLen;
};

class CBufVect
{
public:
	CBufVect(void);
	virtual ~CBufVect(void);
	
	void AddBuf(char* pData,int iLen);

	char* GetBuf(int& iBufLen);

	void DelBuf(int iLen);
	
	void ReleaseBuf();

	int GetSize();
private:
	vector<CBufUnit> m_vect;
	CMLock m_lock;
};
