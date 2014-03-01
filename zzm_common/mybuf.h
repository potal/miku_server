#pragma once

class CMyBuf
{
public:
	CMyBuf();
	~CMyBuf();
	char* GetBuf();
	int GetBufLen();
	bool AddBuf(char* pBuf,int iLen);
	bool DelBuf(int iLen);
	void ReleaseBuf();

private:
	char* m_pBuf;
	int m_iBufLen;
	int m_iCount;
};