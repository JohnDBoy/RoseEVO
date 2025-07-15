#ifndef	__CLS_SQLTHREAD_H
#define	__CLS_SQLTHREAD_H
//-------------------------------------------------------------------------------------------------
#include "CLS_Account.h"
#include "CSqlTHREAD.h"
//-------------------------------------------------------------------------------------------------

class CLS_Client;

class CLS_SqlTHREAD : public CSqlTHREAD
{
private:
	CCriticalSection			m_csUserLIST;
	classDLLIST< CLS_Account* >	m_AddUserLIST;
	classDLLIST< CLS_Account* >	m_RunUserLIST;

	// inherit virtual function from CSqlTHREAD...
	bool Run_SqlPACKET( tagQueryDATA *pSqlPACKET );

	bool Proc_cli_LOGIN_REQ_Old ( tagQueryDATA *pSqlPACKET );     //옛날 버젼.. 2005.10.24 허재영 수정..


	bool Proc_cli_LOGIN_REQ ( tagQueryDATA *pSqlPACKET );      //새로운버젼.. 2005.10.24 허재영 수정..
																													 // 버전 안에서. 나라별로 분기된다..
	
	bool Proc_KOR_LOGIN_REQ( tagQueryDATA *pSqlPACKET );   //한국 로그인처리
	bool Proc_JP_LOGIN_REQ( tagQueryDATA *pSqlPACKET );		 // 일본 로그인처리
	bool Proc_PHI_LOGIN_REQ( tagQueryDATA *pSqlPACKET );   //  필리핀 로그인처리
	bool Proc_USA_LOGIN_REQ( tagQueryDATA *pSqlPACKET );   // 미국 로그인처리
	bool Proc_ENG_LOGIN_REQ( tagQueryDATA *pSqlPACKET );   //  영국 로그인처리
	bool Proc_ETC_LOGIN_REQ( tagQueryDATA *pSqlPACKET );   //  기타 로그인처리

// 김대성/추가
	bool Proc_KOR_TEST_LOGIN_REQ( tagQueryDATA *pSqlPACKET );   //한국 로그인처리

//-------------------------------------
// 2006.06.10/김대성/추가 - 로그인 성공시 로그남기기
	void Proc_Login_Log(int nMode, char *pAccountt, char *pIP);
//-------------------------------------

	bool Proc_Eng_Date_Check(char *pszAccount,CLS_Client *pClient);  //영국 정액제 과금 체크..
	void Execute ();

	CLS_SqlTHREAD ();
	~CLS_SqlTHREAD ();
    
	char	m_szTmpMD5[33];

public :
    bool    m_bCheckLogIN;
    DWORD   m_dwCheckRIGHT;

	void Set_EVENT ()		{	m_pEVENT->SetEvent ();		}
	bool Add_SqlPACKET  (DWORD dwSocketID, char *szAccount, t_PACKET *pPacket);
	bool Add_LogOutUSER (CLS_Account *pCAccount);

	DECLARE_INSTANCE( CLS_SqlTHREAD )
} ;
extern CLS_SqlTHREAD *g_pThreadSQL;

//-------------------------------------------------------------------------------------------------
#endif