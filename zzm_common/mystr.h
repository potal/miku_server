// MyStr.h: interface for the CMyStr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYSTR_H__A4081E4D_6150_4759_BEFE_D1FD98249CFB__INCLUDED_)
#define AFX_MYSTR_H__A4081E4D_6150_4759_BEFE_D1FD98249CFB__INCLUDED_

enum RETURN_CODE
{
	RETURN_SUCCESS = 0,		//³É¹¦
	RETURN_FAILED,         //Ê§°Ü
};

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMyStr  
{
public:
	CMyStr();
	virtual ~CMyStr();
    static int GetField(char* psrc,char* pdst,int iLen,char* pstart,char* pend);
	static int GetFieldLeft(char* psrc,char* pdst,int iLen,char* psub);
	static int GetFieldRight(char* psrc,char* pdst,int iLen,char* psub);
	static char* MyStrStr(char* psrc,char* psub);
	static char* MemStr(char* psrc,char* psub,int iSum,int iLen);
	static char* GetRecvFileType(char* psrc,char p='.');
	static void CharReplace(char* psrc,char pS,char pR);
};

#endif // !defined(AFX_MYSTR_H__A4081E4D_6150_4759_BEFE_D1FD98249CFB__INCLUDED_)
