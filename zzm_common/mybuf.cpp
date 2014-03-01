#include "stdafx.h"
#include "mybuf.h"

CMyBuf::CMyBuf()
{
	m_pBuf = NULL;
	m_iBufLen = 0;
	m_iCount = 0;
}

CMyBuf::~CMyBuf()
{
	ReleaseBuf();
}

char* CMyBuf::GetBuf()
{
	return m_pBuf;
}

int CMyBuf::GetBufLen()
{
	return m_iBufLen;
}

bool CMyBuf::AddBuf(char* pBuf,int iLen)
{
	if (NULL == pBuf || iLen < 1)
	{
		return false;
	}
	
	if (m_iCount < (m_iBufLen+iLen))
	{
		char* pPer = m_pBuf;

		try
		{
			int iBufLen = m_iBufLen+iLen;
			m_pBuf = new char[iBufLen];
		}
		catch (...)
		{
			m_pBuf = pPer;
			return false;
		}

		if (NULL == m_pBuf)
		{
			m_pBuf = pPer;
			return false;
		}
		
		m_iCount = m_iBufLen+iLen;

		if (NULL != pPer)
		{
			memcpy(m_pBuf,pPer,m_iBufLen);
			delete [] pPer;
		}
	}
	
	memcpy(m_pBuf+m_iBufLen,pBuf,iLen);
	m_iBufLen += iLen;
	return true;
}

bool CMyBuf::DelBuf(int iLen)
{
	if (NULL == m_pBuf)
	{
		m_iBufLen = 0;
		m_iCount = 0;
		return false;
	}

	if (m_iBufLen > iLen)
	{
		char* pLeft = m_pBuf + iLen;
		m_iBufLen -= iLen;
		memcpy(m_pBuf,pLeft,m_iBufLen);
	}
	else if (m_iBufLen == iLen)
	{
		m_iBufLen = 0;
	}
	else
	{
		return false;
	}

	return true;
}

void CMyBuf::ReleaseBuf()
{
	if(NULL != m_pBuf)
	{
		delete [] m_pBuf;
		m_pBuf = NULL;
		m_iBufLen = 0;
		m_iCount = 0;
	}
}
