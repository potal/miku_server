#include "stdafx.h"
#include "buf_vector.h"



CBufUnit::CBufUnit()
{
	memset(m_szData,0,sizeof(m_szData));
	m_iLen = 0;
	m_iReadLen = 0;
}

CBufUnit::~CBufUnit()
{

}

bool CBufUnit::AddBuf(char*&  pData,int& iLen)
{
	bool bReturn = false;
	if (NULL == pData || iLen < 1)
	{
		return bReturn;
	}

	int iLeftLen = sizeof(m_szData)-m_iLen;
	if (iLeftLen < 1)
	{
		return true;
	}

	int iCopyLen = iLeftLen;
	if (iLeftLen > iLen)
	{
		iCopyLen = iLen;
	}

	memcpy(m_szData+m_iLen,pData,iCopyLen);
	pData+=iCopyLen;
	iLen -= iCopyLen;
	m_iLen += iCopyLen;
	
	if (m_iLen == sizeof(m_szData))
	{
		bReturn = true;
	}

	return bReturn;
}

char* CBufUnit::GetBuf()
{
	return m_szData + m_iReadLen;
}

int CBufUnit::GetBufLen()
{
	int iReturn = 0;
	iReturn = m_iLen - m_iReadLen;
	
	return iReturn;
}

void CBufUnit::Delbuf(int iLen)
{
	int iLeftLen = 0;
	iLeftLen = m_iLen - m_iReadLen;

	if (iLen < iLeftLen)
	{
		m_iReadLen += iLen;
		
	}
	else
	{
		m_iReadLen = m_iLen;
	}
}

CBufVect::CBufVect(void)
{

}

CBufVect::~CBufVect(void)
{

}

void CBufVect::AddBuf(char* pData,int iLen)
{
	CBufUnit* pUnit = NULL;
	
	CMAutoLock autolock(&m_lock);
	if (m_vect.size() < 1)
	{
		CBufUnit bufunit;
		m_vect.push_back(bufunit);
	}
	
	pUnit = &(m_vect.back());
	
	int iLeftLen = iLen;
	while(iLen > 0)
	{
		if(pUnit->AddBuf(pData,iLen))
		{
			CBufUnit bufunit;
			m_vect.push_back(bufunit);

			pUnit = &(m_vect.back());
		}
	}
}

char* CBufVect::GetBuf(int& iBufLen)
{
	CBufUnit* pUnit = NULL;

	CMAutoLock autolock(&m_lock);
	if (m_vect.size() < 1)
	{
		iBufLen = 0;
		return NULL;
	}

	pUnit = &(m_vect.front());

	iBufLen = pUnit->GetBufLen();

	return pUnit->GetBuf();
}

void CBufVect::DelBuf(int iLen)
{
	CBufUnit* pUnit = NULL;

	CMAutoLock autolock(&m_lock);
	if (m_vect.size() < 1)
	{
		return;
	}

	pUnit = &(m_vect.front());
	
	pUnit->Delbuf(iLen);

	if (pUnit->GetBufLen() < 1)
	{
		m_vect.erase(m_vect.begin());
	}
}

void CBufVect::ReleaseBuf()
{
	CMAutoLock autolock(&m_lock);
	m_vect.clear();
}

int CBufVect::GetSize()
{
	CMAutoLock autolock(&m_lock);
	return m_vect.size();
}