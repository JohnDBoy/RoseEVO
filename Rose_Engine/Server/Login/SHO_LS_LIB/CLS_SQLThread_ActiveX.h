// 2007.03.12/김대성/추가 - 일본 웹 ActiveX 연동

#ifndef	__CLS_SQLTHREAD_ACTIVEX_H
#define	__CLS_SQLTHREAD_ACTIVEX_H
//-------------------------------------------------------------------------------------------------
#include "CLS_Account.h"
#include "CSqlTHREAD.h"
//-------------------------------------------------------------------------------------------------

class CLS_Client;

class CLS_SqlTHREAD_ActiveX : public CSqlTHREAD
{
private:
	CCriticalSection			m_csUserLIST;
	classDLLIST< CLS_Account* >	m_AddUserLIST;
	classDLLIST< CLS_Account* >	m_RunUserLIST;

	// inherit virtual function from CSqlTHREAD...
	bool Run_SqlPACKET( tagQueryDATA *pSqlPACKET );

	bool Proc_cli_LOGIN_REQ2 ( tagQueryDATA *pSqlPACKET );      //새로운버젼.. 2005.10.24 허재영 수정..
	bool Proc_JP_LOGIN_REQ2( tagQueryDATA *pSqlPACKET );		 // 일본 로그인처리

	void Proc_Login_Log(int nMode, char *pAccountt, char *pIP);

	void Execute ();

	CLS_SqlTHREAD_ActiveX ();
	~CLS_SqlTHREAD_ActiveX ();
    
	char	m_szTmpMD5[33];

public :
    bool    m_bCheckLogIN;
    DWORD   m_dwCheckRIGHT;

	void Set_EVENT ()		{	m_pEVENT->SetEvent ();		}
	bool Add_SqlPACKET  (DWORD dwSocketID, char *szAccount, t_PACKET *pPacket);
	bool Add_LogOutUSER (CLS_Account *pCAccount);

	DECLARE_INSTANCE( CLS_SqlTHREAD_ActiveX )
} ;
extern CLS_SqlTHREAD_ActiveX *g_pThreadSQL_ActiveX;

//-------------------------------------------------------------------------------------------------
#endif