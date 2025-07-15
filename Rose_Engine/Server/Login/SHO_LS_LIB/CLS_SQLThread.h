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

	bool Proc_cli_LOGIN_REQ_Old ( tagQueryDATA *pSqlPACKET );     //���� ����.. 2005.10.24 ���翵 ����..


	bool Proc_cli_LOGIN_REQ ( tagQueryDATA *pSqlPACKET );      //���ο����.. 2005.10.24 ���翵 ����..
																													 // ���� �ȿ���. ���󺰷� �б�ȴ�..
	
	bool Proc_KOR_LOGIN_REQ( tagQueryDATA *pSqlPACKET );   //�ѱ� �α���ó��
	bool Proc_JP_LOGIN_REQ( tagQueryDATA *pSqlPACKET );		 // �Ϻ� �α���ó��
	bool Proc_PHI_LOGIN_REQ( tagQueryDATA *pSqlPACKET );   //  �ʸ��� �α���ó��
	bool Proc_USA_LOGIN_REQ( tagQueryDATA *pSqlPACKET );   // �̱� �α���ó��
	bool Proc_ENG_LOGIN_REQ( tagQueryDATA *pSqlPACKET );   //  ���� �α���ó��
	bool Proc_ETC_LOGIN_REQ( tagQueryDATA *pSqlPACKET );   //  ��Ÿ �α���ó��

// ��뼺/�߰�
	bool Proc_KOR_TEST_LOGIN_REQ( tagQueryDATA *pSqlPACKET );   //�ѱ� �α���ó��

//-------------------------------------
// 2006.06.10/��뼺/�߰� - �α��� ������ �α׳����
	void Proc_Login_Log(int nMode, char *pAccountt, char *pIP);
//-------------------------------------

	bool Proc_Eng_Date_Check(char *pszAccount,CLS_Client *pClient);  //���� ������ ���� üũ..
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