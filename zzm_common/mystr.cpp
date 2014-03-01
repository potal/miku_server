// MyStr.cpp: implementation of the CMyStr class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h" 
#include "stdio.h"
#include "mystr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyStr::CMyStr()
{

}

CMyStr::~CMyStr()
{

}

int CMyStr::GetField(char* psrc,char* pdst,int iLen,char* pstart,char* pend)
{
    if (NULL == psrc || NULL == pdst || NULL == pstart || NULL == pend)
    {
        return RETURN_FAILED;
    } 

    char *pS = MyStrStr(psrc,pstart);
    if (NULL == pS) 
    {
        return RETURN_FAILED;
    }
    pS += strlen(pstart);
    char *pE = MyStrStr(pS,pend);
    if (NULL != pE)
    {
		int iCount = pE-pS;
		if (iCount > iLen)
		{
			return RETURN_FAILED;
		}
        strncpy(pdst,pS,iCount);
        return RETURN_SUCCESS;
    }

    return RETURN_FAILED;
}

int CMyStr::GetFieldLeft(char* psrc,char* pdst,int iLen,char* psub)
{
	if (NULL == psrc || NULL == pdst || NULL == psub)
	{
		return RETURN_FAILED;
	} 

	char *pS = MyStrStr(psrc,psub);
	if (NULL == pS) 
	{
		return RETURN_FAILED;
	}

	int iCount = pS-psrc;
	if (iCount > iLen)
	{
		return RETURN_FAILED;
	}
	strncpy(pdst,psrc,iCount);

	return RETURN_SUCCESS;
}

int CMyStr::GetFieldRight(char* psrc,char* pdst,int iLen,char* psub)
{
	if (NULL == psrc || NULL == pdst || NULL == psub)
	{
		return RETURN_FAILED;
	} 

	char *pS = MyStrStr(psrc,psub);
	if (NULL == pS) 
	{
		return RETURN_FAILED;
	}
	
	int iCount = strlen(psrc) - (pS-psrc) - strlen(psub);
	if (iCount > iLen)
	{
		return RETURN_FAILED;
	}
	strcpy(pdst,pS+strlen(psub));

	return RETURN_SUCCESS;
}

char* CMyStr::MyStrStr(char* psrc,char* psub)
{
	if (NULL == psrc || NULL == psub)
	{
		return NULL;
	}

	return MemStr(psrc,psub,strlen(psrc),strlen(psub));
}

char* CMyStr::MemStr(char* psrc,char* psub,int iSum,int iLen)
{
	if (NULL == psrc || NULL == psub)
	{
		return NULL;
	}

	for (int i = 0;i<= (iSum-iLen);i++)
	{
		if (0 == memcmp(psrc+i,psub,iLen))
		{
			return psrc+i;
		}
	}

	return NULL;
}

char* CMyStr::GetRecvFileType(char* psrc,char p)
{
	if (NULL == psrc)
	{
		return NULL;
	}
	
	char szReturn[128]="";
	int i = strlen(psrc) - 1;
	for (;i>=0;i--)
	{
		if (p == psrc[i])
		{
			i+=1;
			break;
		}
	}

	if (i < 0)
	{
		i=0;
	}

	strncpy(szReturn,psrc+i,sizeof(szReturn)-1);

	return szReturn;
}

void CMyStr::CharReplace(char* psrc,char pS,char pR)
{
	if (NULL == psrc)
	{
		return;
	}

	int iLen = strlen(psrc);
	for (int i=0;i<iLen;i++)
	{
		if (pS == psrc[i])
		{
			psrc[i]=pR;
		}
	}
}