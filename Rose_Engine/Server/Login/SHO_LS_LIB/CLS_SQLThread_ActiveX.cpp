// 2006.06.10/김대성/추가 - 로그인 성공시 로그남기기

#include "stdAFX.h"
#include "blockLIST.h"

#include "CLS_Account.h"
#include "CLS_Server.h"
#include "CLS_Client.h"
#include "CLS_SqlTHREAD.h"

#include "CLS_SqlTHREAD_ActiveX.h"

#include "CLS_SqlTHREAD_Log.h"

#include "CAS_GUMS.h"

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
#include "AlphabetCvt/AlphabetCvt.h"
//-------------------------------------


extern classListBLOCK<tagBlockDATA> *g_pListBlackACCOUNT;

IMPLEMENT_INSTANCE( CLS_SqlTHREAD_ActiveX )

//-------------------------------------------------------------------------------------------------
CLS_SqlTHREAD_ActiveX::CLS_SqlTHREAD_ActiveX () : CSqlTHREAD( true )
{
    m_bCheckLogIN = false;
    m_dwCheckRIGHT = 0;

	::ZeroMemory( m_szTmpMD5, sizeof(m_szTmpMD5) );
}
__fastcall CLS_SqlTHREAD_ActiveX::~CLS_SqlTHREAD_ActiveX ()
{
    this->Free ();
}

//-------------------------------------------------------------------------------------------------
bool CLS_SqlTHREAD_ActiveX::Add_SqlPACKET (DWORD dwSocketID, char *szAccount, t_PACKET *pPacket)
{
    if ( 0 == dwSocketID )
        return false;

    // 접속 불가 계정 조사 !!!
    char *szID;
    short nOffset=sizeof( cli_LOGIN_REQ );
    szID = Packet_GetStringPtr( pPacket, nOffset );

    t_HASHKEY HashKEY = CStr::GetHASH( szID );
    tagBlockDATA *pBlockID = g_pListBlackACCOUNT->Search( HashKEY, szID, true );
    if ( pBlockID ) {
        g_pListBlackACCOUNT->Update( pBlockID, 0, 0 );
        return false;
    }

	return CSqlTHREAD::Add_SqlPACKET( dwSocketID, szAccount, (BYTE*)pPacket, pPacket->m_HEADER.m_nSize);
}

bool CLS_SqlTHREAD_ActiveX::Add_LogOutUSER (CLS_Account *pCAccount)
{
	m_csUserLIST.Lock ();
		m_AddUserLIST.AppendNode( pCAccount->m_pListNODE );
	m_csUserLIST.Unlock ();

	m_pEVENT->SetEvent ();

	return true;
}

//-------------------------------------------------------------------------------------------------
void CLS_SqlTHREAD_ActiveX::Execute ()
{
	// 높였더니...로그인 몰릴때..다른 처리를 못해서...문제...
	// this->SetPriority( THREAD_PRIORITY_HIGHEST );	// Priority 2 point above the priority class
	// this->SetPriority( THREAD_PRIORITY_ABOVE_NORMAL );	// Priority 2 point above the priority class

	CDLList< tagQueryDATA >::tagNODE *pSqlNODE;
	classDLLNODE< CLS_Account*  > *pUsrNODE;

	g_LOG.CS_ODS( 0xffff, ">>>> CLS_SqlTHREAD_ActiveX::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );

    while( TRUE ) {
		if ( !this->Terminated ) 
		{
			m_pEVENT->WaitFor( INFINITE );
		} 
		else
		{
			int iReaminCNT;
			this->m_CS.Lock ();
				iReaminCNT = m_AddPACKET.GetNodeCount();
			this->m_CS.Unlock ();

			if ( iReaminCNT <= 0 )
				break;
		}

		this->m_CS.Lock ();
			m_RunPACKET.AppendNodeList( &m_AddPACKET );
			m_AddPACKET.Init ();
		this->m_CS.Unlock ();
		m_pEVENT->ResetEvent ();

		for( pSqlNODE = m_RunPACKET.GetHeadNode(); pSqlNODE; ) {
			if ( Run_SqlPACKET( &pSqlNODE->m_VALUE ) )
				pSqlNODE = this->Del_SqlPACKET( pSqlNODE );
			else
				pSqlNODE = m_RunPACKET.GetNextNode ( pSqlNODE );
		}

		m_csUserLIST.Lock ();
			m_RunUserLIST.AppendNodeList( &m_AddUserLIST );
			m_AddUserLIST.Init ();
		m_csUserLIST.Unlock ();

        for (pUsrNODE=m_RunUserLIST.GetHeadNode(); pUsrNODE; pUsrNODE=m_RunUserLIST.GetHeadNode() )
		{

			m_RunUserLIST.DeleteNode( pUsrNODE->DATA->m_pListNODE );
			// 모든 CLS_Account()는 여기서 삭제된다...
            g_pListJOIN->Delete_ACCOUNT( pUsrNODE->DATA );
		}
	}

	g_LOG.CS_ODS( 0xffff, "<<<< CLS_SqlTHREAD_ActiveX::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );
}

//-------------------------------------------------------------------------------------------------
bool CLS_SqlTHREAD_ActiveX::Run_SqlPACKET( tagQueryDATA *pSqlPACKET )
{
	switch( pSqlPACKET->m_pPacket->m_wType ) {
		case CLI_LOGIN_REQ2 :
			if ( !Proc_cli_LOGIN_REQ2( pSqlPACKET ) )  {
				CLS_Client *pClient = (CLS_Client *)g_pListCLIENT->GetSOCKET( pSqlPACKET->m_iTAG );
				if ( pClient )
					pClient->m_nProcSTEP = CLIENT_STEP_LOGIN_WAIT;
				return true;
			}
			break;

		default :
			g_LOG.CS_ODS( 0xffff, "Undefined sql packet Type: %x, Size: %d \n", pSqlPACKET->m_pPacket->m_wType, pSqlPACKET->m_pPacket->m_nSize);
	}

	return true;
}

//-------------------------------------------------------------------------------------------------

	enum LOGINTBL_COL_IDX 
	{
		LGNTBL_RIGHT,
		LGNTBL_PASSWORD,
		LGNTBL_LAST_CONNECT ,
		LGNTBL_BLOCK_START,
		LGNTBL_BLOCK_END,
		LGNTBL_GENDER,
		LGNTBL_JUMIN,
		LGNTBL_MAILSCONFIRM = LGNTBL_JUMIN,
		LGNTBL_REALNAME,
	    LGNTBL_ENABLE =3,
	} ;



//-------------------------------------------------------------------------------------------------
#define	MAX_ACCOUNT_LEN		40		// 최대 계정입력 문자 16

//-------------------------------------
bool  CLS_SqlTHREAD_ActiveX::Proc_cli_LOGIN_REQ2 ( tagQueryDATA *pSqlPACKET)       //새로운버젼.. 2005.10.24 허재영 수정..
{
#ifdef  __JAPAN
	return Proc_JP_LOGIN_REQ2(pSqlPACKET);
#endif	

	return false;
}

//-------------------------------------

bool  CLS_SqlTHREAD_ActiveX::Proc_JP_LOGIN_REQ2(tagQueryDATA *pSqlPACKET)		 // 일본 로그인처리
{
	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	pPacket->m_cli_LOGIN_REQ2.szSessionKey[32] = '\0';

	DWORD dwSeq = pPacket->m_cli_LOGIN_REQ2.m_dwSeq;
	char *szSessionKey = pPacket->m_cli_LOGIN_REQ2.szSessionKey;
	
	if ( szSessionKey[0] == '\0' )	// 세션키없음.
	{
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT );
		return false;
	}

	if ( !this->m_pSQL->QuerySQL( "select txtAcc, MD5PW from ID_Matching where intSeq=%d and SessionKey='%s'", dwSeq, szSessionKey) )
	{
		g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_FAILED );
		return false;
	}

	if ( !this->m_pSQL->GetNextRECORD() )
	{
		// 등록 안된 계정이다.
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT );
		return false;
	}

	//// 한번 로그인에 사용한 세션키는 삭제한다.
	//if ( this->m_pSQL->ExecSQL( "UPDATE ID_Matching SET SessionKey = NULL where intSeq=%d", dwSeq) < 0 )
	//{
	//	// 고치기 실패 !!!
	//	g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
	//}

	BYTE *ptxtAcc = this->m_pSQL->GetDataPTR(0);
	BYTE *pMD5PW = this->m_pSQL->GetDataPTR(1);

	if(ptxtAcc == NULL || pMD5PW == NULL)
	{
		// 등록 안된 계정이다.
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT );
		return false;
	}

	// 클라에 계정과 비번 보내기
	//g_pListCLIENT->Send_lsv_LOGIN_REPLY2(pSqlPACKET->m_iTAG, ptxtAcc, pMD5PW);

	// Account와 MD5PW를 가지고 기존방식으로 로그인 처리
	classPACKET *pCPacket2 = Packet_AllocNLock ();
	if ( !pCPacket2 )
		return false;

	pCPacket2->m_HEADER.m_wType = CLI_LOGIN_REQ;
	pCPacket2->m_HEADER.m_nSize = sizeof( cli_LOGIN_REQ );
	memcpy(pCPacket2->m_cli_LOGIN_REQ.m_MD5Password, pMD5PW, sizeof(pCPacket2->m_cli_LOGIN_REQ.m_MD5Password)) ;
	if ( ptxtAcc )
		pCPacket2->AppendString( (char *)ptxtAcc );
	g_pThreadSQL->Add_SqlPACKET( pSqlPACKET->m_iTAG, NULL, pCPacket2 );

	Packet_ReleaseNUnlock( pCPacket2 );

	return true;

}
//-------------------------------------------------------------------------------------------------

void CLS_SqlTHREAD_ActiveX::Proc_Login_Log(int nMode, char *pAccountt, char *pIP)
{
#ifdef __LOGIN_LOG			// 로그인 성공시 로그남기기

#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  로그인 성공시 로그남기기    &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  로그인 성공시 로그남기기    &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  로그인 성공시 로그남기기    &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  로그인 성공시 로그남기기    &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  로그인 성공시 로그남기기    &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  로그인 성공시 로그남기기    &&&&&&&&&&&&&&&&&&&&&&&")

	char sz_sql_query[1024];
	sprintf(sz_sql_query, "{call AddLoginLog_LS(%d, '%s', '%s')}", nMode, pAccountt, pIP);
	g_pThreadSQL_Log->Add_QueryString(sz_sql_query);

#endif		// #ifdef __LOGIN_LOG			// 로그인 성공시 로그남기기
}
//-------------------------------------

