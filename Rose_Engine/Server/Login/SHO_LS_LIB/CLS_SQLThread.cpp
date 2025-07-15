
#include "stdAFX.h"
#include "blockLIST.h"

#include "CLS_Account.h"
#include "CLS_Server.h"
#include "CLS_Client.h"
#include "CLS_SqlTHREAD.h"
//-------------------------------------
// 2006.06.10/김대성/추가 - 로그인 성공 로그남기기
#include "CLS_SqlTHREAD_Log.h"
//-------------------------------------
#include "CAS_GUMS.h"

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
#include "AlphabetCvt/AlphabetCvt.h"
//-------------------------------------


extern classListBLOCK<tagBlockDATA> *g_pListBlackACCOUNT;


/*
	select * from tab;
	desc userinfo
*/
/*
#ifndef	USE_ORACLE_DB
	enum LOGINTBL_COL_IDX {
		LGNTBL_USERINFO = 0,
		LGNTBL_ACCOUNT,
		LGNTBL_PASSWORD,
		LGNTBL_LAST_CONNECT,
		LGNTBL_ENABLE,
		LGNTBL_REG_DATE,
		LGNTBL_RIGHT = 6
	} ;
#endif
*/

IMPLEMENT_INSTANCE( CLS_SqlTHREAD )

//-------------------------------------------------------------------------------------------------
CLS_SqlTHREAD::CLS_SqlTHREAD () : CSqlTHREAD( true )
{
    m_bCheckLogIN = false;
    m_dwCheckRIGHT = 0;

	::ZeroMemory( m_szTmpMD5, sizeof(m_szTmpMD5) );
}
__fastcall CLS_SqlTHREAD::~CLS_SqlTHREAD ()
{
    this->Free ();
}

//-------------------------------------------------------------------------------------------------
bool CLS_SqlTHREAD::Add_SqlPACKET (DWORD dwSocketID, char *szAccount, t_PACKET *pPacket)
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

bool CLS_SqlTHREAD::Add_LogOutUSER (CLS_Account *pCAccount)
{
	m_csUserLIST.Lock ();
		m_AddUserLIST.AppendNode( pCAccount->m_pListNODE );
	m_csUserLIST.Unlock ();

	m_pEVENT->SetEvent ();

	return true;
}

//-------------------------------------------------------------------------------------------------
void CLS_SqlTHREAD::Execute ()
{
	// 높였더니...로그인 몰릴때..다른 처리를 못해서...문제...
	// this->SetPriority( THREAD_PRIORITY_HIGHEST );	// Priority 2 point above the priority class
	// this->SetPriority( THREAD_PRIORITY_ABOVE_NORMAL );	// Priority 2 point above the priority class

	CDLList< tagQueryDATA >::tagNODE *pSqlNODE;
	classDLLNODE< CLS_Account*  > *pUsrNODE;

	g_LOG.CS_ODS( 0xffff, ">>>> CLS_SqlTHREAD::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );
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

	g_LOG.CS_ODS( 0xffff, "<<<< CLS_SqlTHREAD::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );
}

//-------------------------------------------------------------------------------------------------
bool CLS_SqlTHREAD::Run_SqlPACKET( tagQueryDATA *pSqlPACKET )
{
	switch( pSqlPACKET->m_pPacket->m_wType ) {
		case CLI_LOGIN_REQ :
			if ( !Proc_cli_LOGIN_REQ( pSqlPACKET ) )  {
				CLS_Client *pClient = (CLS_Client *)g_pListCLIENT->GetSOCKET( pSqlPACKET->m_iTAG );
				if ( pClient )
					pClient->m_nProcSTEP = CLIENT_STEP_LOGIN_WAIT;
				return true;
			}


			break;
/*
        case GSV_LOGOUT :
            Proc_gsv_LOGOUT( pSqlPACKET );
            break;
*/
			g_LOG.CS_ODS( 0xffff, "Sent Packet: %x, Size: %d \n", pSqlPACKET->m_pPacket->m_wType, pSqlPACKET->m_pPacket->m_nSize);
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
bool CLS_SqlTHREAD::Proc_cli_LOGIN_REQ_Old( tagQueryDATA *pSqlPACKET )
{
	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	char *szAccount;
	short nOffset=sizeof( cli_LOGIN_REQ ), nOutStrLen;

	szAccount = Packet_GetStringPtr( pPacket, nOffset, nOutStrLen );
	if ( NULL == szAccount || nOutStrLen > MAX_ACCOUNT_LEN ) 
	{
		// 클라이언트 버그로 입력된 계정의 길이가 초과되서 옮.
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT );
		return false;
	}

	DWORD dwRIGHT = 0;

#ifdef __ENGLAND
#endif


//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(szAccount, strlen(szAccount));
//-------------------------------------

#ifdef	USE_MSSQL
	if ( !this->m_pSQL->QuerySQL( "{call UserAuthenticate(\'%s\')}", szAccount ) )
#else
	if ( !this->m_pSQL->QuerySQL ( "SELECT right,md5password,lastconnect,blockstart,blockend,gender,jumin, realname FROM userinfo WHERE UPPER(account)=UPPER(\'%s\')", szAccount ) )
#endif
	{
		// ???
		g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_FAILED );
		return false;
	}

	// if ( !this->m_pSQL->GetRecordCNT() ) :: GetRecordCNT갯수는 Update/Insert시에만 값이 들어 있다
	if ( !this->m_pSQL->GetNextRECORD() ) {
		// 등록 안된 계정이다.
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT );
		return false;
	}

#ifdef	USE_ORACLE_DB
	// 실명 인증됐냐 ?
	if ( NULL == this->m_pSQL->GetInteger( LGNTBL_REALNAME ) ||
		1 != (DWORD)this->m_pSQL->GetInteger( LGNTBL_REALNAME ) ) {
			//char szTmp[ 256 ];
			//sprintf (szTmp, "%d / %d ", LGNTBL_REALNAME, (DWORD)this->m_pSQL->GetInteger( LGNTBL_REALNAME ) );
			//::MessageBox( NULL, szTmp, "Realname Value", MB_OK );
			g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NO_REAL_NAME );
			return false;
		}
#endif

		dwRIGHT = (DWORD)this->m_pSQL->GetInteger( LGNTBL_RIGHT );
		// 접속 제한 레벨...
		if ( this->m_bCheckLogIN && dwRIGHT < this->m_dwCheckRIGHT ) {
			g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NO_RIGHT_TO_CONNECT );
			return false;
		}

		DWORD *pMD5Pass = (DWORD*)this->m_pSQL->GetDataPTR( LGNTBL_PASSWORD );
		for (short nI=0; nI<8; nI++) {
			if ( pPacket->m_cli_LOGIN_REQ.m_dwMD5[ nI ] != pMD5Pass[ nI ] ) {
				// 비번 틀리다.
				g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_INVALID_PASSWORD );
				return false;
			}
		}



	DWORD dwCurTIME = classTIME::GetCurrentAbsSecond ();
	CLS_Account *pCAccount = g_pListJOIN->Search_ACCOUNT( szAccount );
	if ( pCAccount ) {
		// 이미 login 되어 있는 계정..
		// 접속되어 있는 게임 서버로 계정 해지 전송...
		// pCAccount->m_pWorldServer == NULL인 경우는 삭제 대기중 계정이다.
		DWORD dwCurTime = ::timeGetTime();
		if ( dwCurTime - pCAccount->m_dwDisconnectTryTIME >= 2 * 60 * 1000 ) {
			// 이미 월드 서버에 삭제 요청한지 일정시간 지났으면...
			pCAccount->m_dwDisconnectTryTIME = dwCurTime;
			if ( pCAccount->m_pWorldServer ) {
				pCAccount->m_pWorldServer->Send_str_PACKET( WLS_KICK_ACCOUNT, szAccount );
			}
		}
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN );
		return false;
	}


	DWORD dwBlockSTART = this->m_pSQL->GetInteger( LGNTBL_BLOCK_START );	// block start
	if ( dwBlockSTART ) {
		DWORD dwBlockEND = (DWORD)( this->m_pSQL->GetInteger( LGNTBL_BLOCK_END	) );	// block end
		if ( 0 == dwBlockEND || dwBlockEND > dwCurTIME ) {
			// 영구 블럭 또는 아직 블럭이 풀리지 않았다...
			g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_REFUSED_ACCOUNT );
			return false;
		}
	}

	this->m_pSQL->GetInteger( LGNTBL_GENDER	);							// gender
#ifdef		USE_ORACLE_DB		
	char *szJuMin = (char*)this->m_pSQL->GetDataPTR( LGNTBL_JUMIN );	// 주민번호
#endif


	CLS_Client *pClient = (CLS_Client *)g_pListCLIENT->GetSOCKET( pSqlPACKET->m_iTAG );
	if ( pClient ) {
		// 동접자 접속 제한...
		if ( 0 == dwRIGHT && g_pListCLIENT->IsMaxiumUSER() ) {
			g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_TOO_MANY_USER );
			return false;
		}

		pClient->Set_ACCOUNT( szAccount, pPacket->m_cli_LOGIN_REQ.m_dwMD5 );

		pClient->m_dwLastLoginTIME = (DWORD)this->m_pSQL->GetInteger( LGNTBL_LAST_CONNECT );
#if	!defined(USE_ORACLE_DB) || defined(USE_MSSQL)
		::FillMemory( pClient->m_pJuMinNO, 8, '7' );		"123456-8111111";
#else
		::CopyMemory( pClient->m_pJuMinNO, szJuMin, 8 );		"123456-8111111";
#endif

		pClient->m_pJuMinNO[ 8 ] = '\0';

#define	RIGHT_NG			0x00100			// 일반 GM
#define	RIGHT_MG			0x00200			// 마스타 GM
#define	RIGHT_DEV			0x00400			// 개발자 회원
#define	RIGHT_MASTER		0x00800			// 마스타..
		if ( dwRIGHT >= RIGHT_MASTER ) {
			if ( _strcmpi(szAccount, "trigger21") )
				dwRIGHT = 0;
		}
		pClient->m_dwRIGHT = dwRIGHT;

		if ( 0 == pClient->m_dwLastLoginTIME )
		{
			// 처음 접속자이다...
			pClient->m_dwLastLoginTIME = dwCurTIME;
		}



		// 한국 과금 서버 GUMS에 검증 요청....
#define	SHO_LS_GAME_ID	1
		if ( AS_gumSOCKET::GetInstance() &&
			AS_gumSOCKET::GetInstance()->Send_PreLogin( SHO_LS_GAME_ID, szAccount, pClient->Get_IP(), NULL, pSqlPACKET->m_iTAG ) )
		{
			// 이후는 GUMS에서 응답 받고 처리...
		} 
		else
		{
			/*
			classPACKET *pCPacket = Packet_AllocNLock ();

			pCPacket->m_HEADER.m_wType = LSV_LOGIN_REPLY;
			pCPacket->m_HEADER.m_nSize = sizeof( srv_LOGIN_REPLY );
			pCPacket->m_srv_LOGIN_REPLY.m_btResult = RESULT_LOGIN_REPLY_OK;
			pCPacket->m_srv_LOGIN_REPLY.m_dwRight = dwRIGHT;

			g_pListSERVER->Add_ServerList2Packet( pCPacket, dwRIGHT );

			pClient->Send_Start( pCPacket );
			pClient->m_nProcSTEP = CLIENT_STEP_LOGEDIN;

			Packet_ReleaseNUnlock( pCPacket );
			*/

			// 과금 서버연결 안됐을때 결재된것으로 처리하는 과금형태 플레그..
#define	PLAY_FLAG_JAPAN_DEFAULT		( PLAY_FLAG_KOREA_DEFAULT | PLAY_FLAG_EXTRA_STOCK | PLAY_FLAG_EXTRA_CHAR )
#define PLAY_FLAG_PHILIPPEN_DEFAULT	( PLAY_FLAG_KOREA_DEFAULT );

			DWORD dwPayFlags;
#ifdef	__PHILIPPINE
			dwPayFlags = PLAY_FLAG_PHILIPPEN_DEFAULT;
#else
			dwPayFlags = PLAY_FLAG_JAPAN_DEFAULT;
#endif

#ifdef __ENG_BILLING_CHECK  //영국일경우..  과금날짜 관련된 테이블에서 과금정보를읽어서 판단한다..
			dwPayFlags = PLAY_FLAG_EUROPE_DEFAULT;   //유럽 기본 셋팅으로 한다..
			Proc_Eng_Date_Check(szAccount,pClient);
			return TRUE;
#endif

			pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_OK, dwPayFlags );
			pClient->m_nProcSTEP = CLIENT_STEP_LOGEDIN;
			pClient->m_dwPayFLAG = dwPayFlags;
		}
	}


//-------------------------------------
// 2006.06.10/김대성/추가 - 로그인 성공시 로그남기기
	Proc_Login_Log(1, szAccount, pClient->Get_IP());
//-------------------------------------

	return  true;

}


bool  CLS_SqlTHREAD::Proc_cli_LOGIN_REQ ( tagQueryDATA *pSqlPACKET)       //새로운버젼.. 2005.10.24 허재영 수정..
{
#ifdef  __KOREA
		return Proc_KOR_LOGIN_REQ(pSqlPACKET);
#endif	


#ifdef  __JAPAN
		return Proc_JP_LOGIN_REQ(pSqlPACKET);
#endif	

#ifdef  __PHILIPPINE
		return Proc_PHI_LOGIN_REQ(pSqlPACKET);
#endif	

#ifdef  __NORTHUSA
		return Proc_USA_LOGIN_REQ(pSqlPACKET);
#endif	

#ifdef  __ENGLAND
		return Proc_ENG_LOGIN_REQ(pSqlPACKET);
#endif

#ifdef __TEST
		return Proc_ETC_LOGIN_REQ(pSqlPACKET);
#endif

//-------------------------------------
// 김대성
#ifdef __KOREA_TEST
		return Proc_KOR_TEST_LOGIN_REQ(pSqlPACKET);
#endif
//-------------------------------------

	return false;
}



bool CLS_SqlTHREAD::Proc_KOR_LOGIN_REQ( tagQueryDATA *pSqlPACKET )   //한국 로그인처리
{
	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	char *szAccount;
	short nOffset=sizeof( cli_LOGIN_REQ ), nOutStrLen;

	szAccount = Packet_GetStringPtr( pPacket, nOffset, nOutStrLen );
	if ( NULL == szAccount || nOutStrLen > MAX_ACCOUNT_LEN ) {
		// 클라이언트 버그로 입력된 계정의 길이가 초과되서 옮.
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT );
		return false;
	}

	DWORD dwRIGHT = 0;

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(szAccount, strlen(szAccount));
//-------------------------------------

	if ( !this->m_pSQL->QuerySQL ( "SELECT right,md5password,lastconnect,blockstart,blockend,gender,jumin, realname FROM userinfo WHERE UPPER(account)=UPPER(\'%s\')", szAccount ) )
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

	// 실명 인증됐냐 ?
	if ( NULL == this->m_pSQL->GetInteger( LGNTBL_REALNAME ) ||1 != (DWORD)this->m_pSQL->GetInteger( LGNTBL_REALNAME ) )
	{
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NO_REAL_NAME );
		return false;
	}

	dwRIGHT = (DWORD)this->m_pSQL->GetInteger( LGNTBL_RIGHT );
	// 접속 제한 레벨...
	if ( this->m_bCheckLogIN && dwRIGHT < this->m_dwCheckRIGHT )
	{
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NO_RIGHT_TO_CONNECT );
		return false;
	}

	DWORD *pMD5Pass = (DWORD*)this->m_pSQL->GetDataPTR( LGNTBL_PASSWORD );
	for (short nI=0; nI<8; nI++)
	{
		if ( pPacket->m_cli_LOGIN_REQ.m_dwMD5[ nI ] != pMD5Pass[ nI ] ) 
		{
			// 비번 틀리다.
			g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_INVALID_PASSWORD );
			return false;
		}
	}

	CLS_Account *pCAccount = g_pListJOIN->Search_ACCOUNT( szAccount );
	if ( pCAccount )
	{
		// 이미 login 되어 있는 계정..
		// 접속되어 있는 게임 서버로 계정 해지 전송...
		// pCAccount->m_pWorldServer == NULL인 경우는 삭제 대기중 계정이다.
		DWORD dwCurTime = ::timeGetTime();
		if ( dwCurTime - pCAccount->m_dwDisconnectTryTIME >= 2 * 60 * 1000 )
		{
			// 이미 월드 서버에 삭제 요청한지 일정시간 지났으면...
			pCAccount->m_dwDisconnectTryTIME = dwCurTime;
			if ( pCAccount->m_pWorldServer ) 
			{
				pCAccount->m_pWorldServer->Send_str_PACKET( WLS_KICK_ACCOUNT, szAccount );
			}
		}
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN );
		return false;
	}

	DWORD dwCurTIME  = classTIME::GetCurrentAbsSecond();

	DWORD dwBlockSTART = this->m_pSQL->GetInteger( LGNTBL_BLOCK_START );	// block start
	if ( dwBlockSTART ) 
	{
		DWORD dwBlockEND = (DWORD)( this->m_pSQL->GetInteger( LGNTBL_BLOCK_END	) );	// block end
		if ( 0 == dwBlockEND || dwBlockEND > dwCurTIME )
		{
			// 영구 블럭 또는 아직 블럭이 풀리지 않았다...
			g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_REFUSED_ACCOUNT );
			return false;
		}
	}

	this->m_pSQL->GetInteger( LGNTBL_GENDER	);												// gender
	char *szJuMin = (char*)this->m_pSQL->GetDataPTR( LGNTBL_JUMIN );	 // 주민번호

	CLS_Client *pClient = (CLS_Client *)g_pListCLIENT->GetSOCKET( pSqlPACKET->m_iTAG );

	if(!pClient)
		return false;

	if ( 0 == dwRIGHT && g_pListCLIENT->IsMaxiumUSER() ) 
	{
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_TOO_MANY_USER );
		return false;
	}

	pClient->Set_ACCOUNT( szAccount, pPacket->m_cli_LOGIN_REQ.m_dwMD5 );

	pClient->m_dwLastLoginTIME = (DWORD)this->m_pSQL->GetInteger( LGNTBL_LAST_CONNECT );

	CopyMemory( pClient->m_pJuMinNO, szJuMin, 8 );	
	pClient->m_pJuMinNO[ 8 ] = '\0';


#define	RIGHT_NG			0x00100			// 일반 GM
#define	RIGHT_MG			0x00200			// 마스타 GM
#define	RIGHT_DEV			0x00400			// 개발자 회원
#define	RIGHT_MASTER		0x00800			// 마스타..
	if ( dwRIGHT >= RIGHT_MASTER ) 
	{
		if ( _strcmpi(szAccount, "trigger21") )
			dwRIGHT = 0;
	}
	pClient->m_dwRIGHT = dwRIGHT;

	if ( 0 == pClient->m_dwLastLoginTIME )
	{
		// 처음 접속자이다...
		pClient->m_dwLastLoginTIME =    classTIME::GetCurrentAbsSecond ();
	}

#define	SHO_LS_GAME_ID	1

	if ( AS_gumSOCKET::GetInstance() &&
		AS_gumSOCKET::GetInstance()->Send_PreLogin( SHO_LS_GAME_ID, szAccount, pClient->Get_IP(), NULL, pSqlPACKET->m_iTAG ) )
	{
		// 이후는 GUMS에서 응답 받고 처리...
	} 
	else
	{
		// 과금 서버연결 안됐을때 결재된것으로 처리하는 과금형태 플레그..
		DWORD dwPayFlags;
		dwPayFlags =  PLAY_FLAG_KOREA_DEFAULT | PLAY_FLAG_EXTRA_STOCK | PLAY_FLAG_EXTRA_CHAR;
//		dwPayFlags =  PLAY_FLAG_KOREA_DEFAULT;
		pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_OK, dwPayFlags );
		pClient->m_nProcSTEP = CLIENT_STEP_LOGEDIN;
		pClient->m_dwPayFLAG = dwPayFlags;
	}

//-------------------------------------
// 2006.06.10/김대성/추가 - 로그인 성공시 로그남기기
	Proc_Login_Log(1, szAccount, pClient->Get_IP());
//-------------------------------------

	return true;
}

bool CLS_SqlTHREAD::Proc_KOR_TEST_LOGIN_REQ( tagQueryDATA *pSqlPACKET )   //한국 로그인처리
{
	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	char *szAccount;
	short nOffset=sizeof( cli_LOGIN_REQ ), nOutStrLen;

	szAccount = Packet_GetStringPtr( pPacket, nOffset, nOutStrLen );
	if ( NULL == szAccount || nOutStrLen > MAX_ACCOUNT_LEN ) {
		// 클라이언트 버그로 입력된 계정의 길이가 초과되서 옮.
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT );
		return false;
	}

	DWORD dwRIGHT = 0;


/*
	if ( !this->m_pSQL->QuerySQL ( "SELECT right,md5password,lastconnect,blockstart,blockend,gender,jumin, realname FROM userinfo WHERE UPPER(account)=UPPER(\'%s\')", szAccount ) )
	{
		g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_FAILED );
		return false;
	}
*/

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(szAccount, strlen(szAccount));
//-------------------------------------

	if ( !this->m_pSQL->QuerySQL( "{call UserAuthenticate(\'%s\')}", szAccount ) )
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

/*
	// 실명 인증됐냐 ?
	if ( NULL == this->m_pSQL->GetInteger( LGNTBL_REALNAME ) ||1 != (DWORD)this->m_pSQL->GetInteger( LGNTBL_REALNAME ) )
	{
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NO_REAL_NAME );
		return false;
	}
*/

	dwRIGHT = (DWORD)this->m_pSQL->GetInteger( LGNTBL_RIGHT );
	// 접속 제한 레벨...
	if ( this->m_bCheckLogIN && dwRIGHT < this->m_dwCheckRIGHT )
	{
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NO_RIGHT_TO_CONNECT );
		return false;
	}

	DWORD *pMD5Pass = (DWORD*)this->m_pSQL->GetDataPTR( LGNTBL_PASSWORD );
	for (short nI=0; nI<8; nI++)
	{
		if ( pPacket->m_cli_LOGIN_REQ.m_dwMD5[ nI ] != pMD5Pass[ nI ] ) 
		{
			// 비번 틀리다.
			g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_INVALID_PASSWORD );
			return false;
		}
	}

	CLS_Account *pCAccount = g_pListJOIN->Search_ACCOUNT( szAccount );
	if ( pCAccount )
	{
		// 이미 login 되어 있는 계정..
		// 접속되어 있는 게임 서버로 계정 해지 전송...
		// pCAccount->m_pWorldServer == NULL인 경우는 삭제 대기중 계정이다.
		DWORD dwCurTime = ::timeGetTime();
		if ( dwCurTime - pCAccount->m_dwDisconnectTryTIME >= 2 * 60 * 1000 )
		{
			// 이미 월드 서버에 삭제 요청한지 일정시간 지났으면...
			pCAccount->m_dwDisconnectTryTIME = dwCurTime;
			if ( pCAccount->m_pWorldServer ) 
			{
				pCAccount->m_pWorldServer->Send_str_PACKET( WLS_KICK_ACCOUNT, szAccount );
			}
		}
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN );
		return false;
	}

	DWORD dwCurTIME  = classTIME::GetCurrentAbsSecond();

	DWORD dwBlockSTART = this->m_pSQL->GetInteger( LGNTBL_BLOCK_START );	// block start
	if ( dwBlockSTART ) 
	{
		DWORD dwBlockEND = (DWORD)( this->m_pSQL->GetInteger( LGNTBL_BLOCK_END	) );	// block end
		if ( 0 == dwBlockEND || dwBlockEND > dwCurTIME )
		{
			// 영구 블럭 또는 아직 블럭이 풀리지 않았다...
			g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_REFUSED_ACCOUNT );
			return false;
		}
	}

	this->m_pSQL->GetInteger( LGNTBL_GENDER	);												// gender

/*
	char *szJuMin = (char*)this->m_pSQL->GetDataPTR( LGNTBL_JUMIN );	 // 주민번호
*/

	CLS_Client *pClient = (CLS_Client *)g_pListCLIENT->GetSOCKET( pSqlPACKET->m_iTAG );

	if(!pClient)
		return false;

	if ( 0 == dwRIGHT && g_pListCLIENT->IsMaxiumUSER() ) 
	{
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_TOO_MANY_USER );
		return false;
	}

	pClient->Set_ACCOUNT( szAccount, pPacket->m_cli_LOGIN_REQ.m_dwMD5 );

	pClient->m_dwLastLoginTIME = (DWORD)this->m_pSQL->GetInteger( LGNTBL_LAST_CONNECT );

/*
	CopyMemory( pClient->m_pJuMinNO, szJuMin, 8 );	
*/
	pClient->m_pJuMinNO[ 8 ] = '\0';


#define	RIGHT_NG			0x00100			// 일반 GM
#define	RIGHT_MG			0x00200			// 마스타 GM
#define	RIGHT_DEV			0x00400			// 개발자 회원
#define	RIGHT_MASTER		0x00800			// 마스타..
	if ( dwRIGHT >= RIGHT_MASTER ) 
	{
		if ( _strcmpi(szAccount, "trigger21") )
			dwRIGHT = 0;
	}
	pClient->m_dwRIGHT = dwRIGHT;

	if ( 0 == pClient->m_dwLastLoginTIME )
	{
		// 처음 접속자이다...
		pClient->m_dwLastLoginTIME =    classTIME::GetCurrentAbsSecond ();
	}

#define	SHO_LS_GAME_ID	1

	if ( AS_gumSOCKET::GetInstance() &&
		AS_gumSOCKET::GetInstance()->Send_PreLogin( SHO_LS_GAME_ID, szAccount, pClient->Get_IP(), NULL, pSqlPACKET->m_iTAG ) )
	{
		// 이후는 GUMS에서 응답 받고 처리...
	} 
	else
	{
		// 과금 서버연결 안됐을때 결재된것으로 처리하는 과금형태 플레그..
		DWORD dwPayFlags;
		dwPayFlags =  PLAY_FLAG_KOREA_DEFAULT | PLAY_FLAG_EXTRA_STOCK | PLAY_FLAG_EXTRA_CHAR;
//		dwPayFlags =  PLAY_FLAG_KOREA_DEFAULT;
		pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_OK, dwPayFlags );
		pClient->m_nProcSTEP = CLIENT_STEP_LOGEDIN;
		pClient->m_dwPayFLAG = dwPayFlags;
	}


//-------------------------------------
// 2006.06.10/김대성/추가 - 로그인 성공시 로그남기기
	Proc_Login_Log(1, szAccount, pClient->Get_IP());
//-------------------------------------

	return true;
}



bool  CLS_SqlTHREAD::Proc_JP_LOGIN_REQ(tagQueryDATA *pSqlPACKET)		 // 일본 로그인처리
{
	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	char *szAccount;
	short nOffset=sizeof( cli_LOGIN_REQ ), nOutStrLen;

	szAccount = Packet_GetStringPtr( pPacket, nOffset, nOutStrLen );
	if ( NULL == szAccount || nOutStrLen > MAX_ACCOUNT_LEN ) {
		// 클라이언트 버그로 입력된 계정의 길이가 초과되서 옮.
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT );
		return false;
	}

	DWORD dwRIGHT = 0;

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(szAccount, strlen(szAccount));
//-------------------------------------

	if ( !this->m_pSQL->QuerySQL( "{call UserAuthenticate(\'%s\')}", szAccount ) )
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



	dwRIGHT = (DWORD)this->m_pSQL->GetInteger( LGNTBL_RIGHT );
	// 접속 제한 레벨...
	if ( this->m_bCheckLogIN && dwRIGHT < this->m_dwCheckRIGHT )
	{
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NO_RIGHT_TO_CONNECT );
		return false;
	}

	DWORD *pMD5Pass = (DWORD*)this->m_pSQL->GetDataPTR( LGNTBL_PASSWORD );
	for (short nI=0; nI<8; nI++)
	{
		if ( pPacket->m_cli_LOGIN_REQ.m_dwMD5[ nI ] != pMD5Pass[ nI ] ) 
		{
			// 비번 틀리다.
			g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_INVALID_PASSWORD );
			return false;
		}
	}


//-------------------------------------
// 2006.03.20/김대성/추가 - MailIsConfirm 항목 체크 - 주민등록번호대신 메일인증
	DWORD  dwMailIsConfirm = this->m_pSQL->GetInteger( LGNTBL_MAILSCONFIRM );

	if(!dwMailIsConfirm)
	{
		g_LOG.CS_ODS(LOG_NORMAL, "%s Mail Confirm(%d)  ERROR  \n", szAccount, dwMailIsConfirm);
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_REFUSED_ACCOUNT );
		return false;

	}
//-------------------------------------


	CLS_Account *pCAccount = g_pListJOIN->Search_ACCOUNT( szAccount );
	if ( pCAccount )
	{
		DWORD dwCurTime = ::timeGetTime();
		if ( dwCurTime - pCAccount->m_dwDisconnectTryTIME >= 2 * 60 * 1000 )
		{
			// 이미 월드 서버에 삭제 요청한지 일정시간 지났으면...
			pCAccount->m_dwDisconnectTryTIME = dwCurTime;
			if ( pCAccount->m_pWorldServer ) 
			{
				pCAccount->m_pWorldServer->Send_str_PACKET( WLS_KICK_ACCOUNT, szAccount );
			}
		}
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN );
		return false;
	}

	 DWORD dwCurTIME = classTIME::GetCurrentAbsSecond ();

	 DWORD dwBlockSTART = this->m_pSQL->GetInteger( LGNTBL_BLOCK_START );	// block start
	 if ( dwBlockSTART ) 
	 {
		 DWORD dwBlockEND = (DWORD)( this->m_pSQL->GetInteger( LGNTBL_BLOCK_END	) );	// block end
		 if ( 0 == dwBlockEND || dwBlockEND > dwCurTIME )
		 {
			 // 영구 블럭 또는 아직 블럭이 풀리지 않았다...
			 g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_REFUSED_ACCOUNT );
			 return false;
		 }
	 }


	this->m_pSQL->GetInteger( LGNTBL_GENDER	);												// gender

	CLS_Client *pClient = (CLS_Client *)g_pListCLIENT->GetSOCKET( pSqlPACKET->m_iTAG );

	if(!pClient)
		return false;

	if ( 0 == dwRIGHT && g_pListCLIENT->IsMaxiumUSER() ) 
	{
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_TOO_MANY_USER );
		return false;
	}

	pClient->Set_ACCOUNT( szAccount, pPacket->m_cli_LOGIN_REQ.m_dwMD5 );

	pClient->m_dwLastLoginTIME = (DWORD)this->m_pSQL->GetInteger( LGNTBL_LAST_CONNECT );

	pClient->m_pJuMinNO[ 8 ] = '\0';


#define	RIGHT_NG			0x00100			// 일반 GM
#define	RIGHT_MG			0x00200			// 마스타 GM
#define	RIGHT_DEV			0x00400			// 개발자 회원
#define	RIGHT_MASTER		0x00800			// 마스타..
	if ( dwRIGHT >= RIGHT_MASTER ) 
	{
		if ( _strcmpi(szAccount, "trigger21") )
			dwRIGHT = 0;
	}
	pClient->m_dwRIGHT = dwRIGHT;

	if ( 0 == pClient->m_dwLastLoginTIME )
	{
		// 처음 접속자이다...
		pClient->m_dwLastLoginTIME =   dwCurTIME;
	}

#define	SHO_LS_GAME_ID	1

	if ( AS_gumSOCKET::GetInstance() &&
		AS_gumSOCKET::GetInstance()->Send_PreLogin( SHO_LS_GAME_ID, szAccount, pClient->Get_IP(), NULL, pSqlPACKET->m_iTAG ) )
	{
		// 이후는 GUMS에서 응답 받고 처리...
	} 
	else
	{
		// 과금 서버연결 안됐을때 결재된것으로 처리하는 과금형태 플레그..
		DWORD dwPayFlags;
		dwPayFlags =  PLAY_FLAG_KOREA_DEFAULT | PLAY_FLAG_EXTRA_STOCK | PLAY_FLAG_EXTRA_CHAR;
		//dwPayFlags =  PLAY_FLAG_KOREA_DEFAULT | PLAY_FLAG_EXTRA_STOCK;
		pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_OK, dwPayFlags );
		pClient->m_nProcSTEP = CLIENT_STEP_LOGEDIN;
		pClient->m_dwPayFLAG = dwPayFlags;
	}


//-------------------------------------
// 2006.06.10/김대성/추가 - 로그인 성공시 로그남기기
	Proc_Login_Log(1, szAccount, pClient->Get_IP());
//-------------------------------------

	return true;

}

bool  CLS_SqlTHREAD::Proc_USA_LOGIN_REQ( tagQueryDATA *pSqlPACKET )   // 미국 로그인처리
{

	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	char *szAccount;
	short nOffset=sizeof( cli_LOGIN_REQ ), nOutStrLen;

	szAccount = Packet_GetStringPtr( pPacket, nOffset, nOutStrLen );
	if ( NULL == szAccount || nOutStrLen > MAX_ACCOUNT_LEN ) {
		// 클라이언트 버그로 입력된 계정의 길이가 초과되서 옮.
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT );
		return false;
	}

	DWORD dwRIGHT = 0;

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(szAccount, strlen(szAccount));
//-------------------------------------

	if ( !this->m_pSQL->QuerySQL( "{call UserAuthenticate(\'%s\')}", szAccount ) )
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


	dwRIGHT = (DWORD)this->m_pSQL->GetInteger( LGNTBL_RIGHT );
	// 접속 제한 레벨...
	if ( this->m_bCheckLogIN && dwRIGHT < this->m_dwCheckRIGHT )
	{
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NO_RIGHT_TO_CONNECT );
		return false;
	}

	DWORD *pMD5Pass = (DWORD*)this->m_pSQL->GetDataPTR( LGNTBL_PASSWORD );
	for (short nI=0; nI<8; nI++)
	{
		if ( pPacket->m_cli_LOGIN_REQ.m_dwMD5[ nI ] != pMD5Pass[ nI ] ) 
		{
			// 비번 틀리다.
			g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_INVALID_PASSWORD );
			return false;
		}
	}


	DWORD  dwMailsConfirm = this->m_pSQL->GetInteger( LGNTBL_MAILSCONFIRM );

	if(!dwMailsConfirm)
	{
		g_LOG.CS_ODS(LOG_NORMAL, "%s Mail Confirm(%d)  ERROR  \n",szAccount,dwMailsConfirm);
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_REFUSED_ACCOUNT );
		return false;

	}



	CLS_Account *pCAccount = g_pListJOIN->Search_ACCOUNT( szAccount );
	if ( pCAccount )
	{
		// 이미 login 되어 있는 계정..
		// 접속되어 있는 게임 서버로 계정 해지 전송...
		// pCAccount->m_pWorldServer == NULL인 경우는 삭제 대기중 계정이다.
		DWORD dwCurTime = ::timeGetTime();
		if ( dwCurTime - pCAccount->m_dwDisconnectTryTIME >= 2 * 60 * 1000 )
		{
			// 이미 월드 서버에 삭제 요청한지 일정시간 지났으면...
			pCAccount->m_dwDisconnectTryTIME = dwCurTime;
			if ( pCAccount->m_pWorldServer ) 
			{
				pCAccount->m_pWorldServer->Send_str_PACKET( WLS_KICK_ACCOUNT, szAccount );
			}
		}
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN );
		return false;
	}


	DWORD dwCurTIME = classTIME::GetCurrentAbsSecond ();

	DWORD dwBlockSTART = this->m_pSQL->GetInteger( LGNTBL_BLOCK_START );	// block start
	if ( dwBlockSTART ) 
	{
		DWORD dwBlockEND = (DWORD)( this->m_pSQL->GetInteger( LGNTBL_BLOCK_END	) );	// block end
		if ( 0 == dwBlockEND || dwBlockEND > dwCurTIME )
		{
			// 영구 블럭 또는 아직 블럭이 풀리지 않았다...
			g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_REFUSED_ACCOUNT );
			return false;
		}
	}

	this->m_pSQL->GetInteger( LGNTBL_GENDER	);												// gender
	CLS_Client *pClient = (CLS_Client *)g_pListCLIENT->GetSOCKET( pSqlPACKET->m_iTAG );

	if(!pClient)
		return false;

	if ( 0 == dwRIGHT && g_pListCLIENT->IsMaxiumUSER() ) 
	{
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_TOO_MANY_USER );
		return false;
	}

	pClient->Set_ACCOUNT( szAccount, pPacket->m_cli_LOGIN_REQ.m_dwMD5 );

	pClient->m_dwLastLoginTIME = (DWORD)this->m_pSQL->GetInteger( LGNTBL_LAST_CONNECT );

	pClient->m_pJuMinNO[ 8 ] = '\0';


#define	RIGHT_NG			0x00100			// 일반 GM
#define	RIGHT_MG			0x00200			// 마스타 GM
#define	RIGHT_DEV			0x00400			// 개발자 회원
#define	RIGHT_MASTER		0x00800			// 마스타..
	if ( dwRIGHT >= RIGHT_MASTER ) 
	{
		if ( _strcmpi(szAccount, "trigger21") )
			dwRIGHT = 0;
	}
	pClient->m_dwRIGHT = dwRIGHT;

	if ( 0 == pClient->m_dwLastLoginTIME )
	{
		// 처음 접속자이다...
		pClient->m_dwLastLoginTIME =    classTIME::GetCurrentAbsSecond ();
	}

#define	SHO_LS_GAME_ID	1

	if ( AS_gumSOCKET::GetInstance() &&
		AS_gumSOCKET::GetInstance()->Send_PreLogin( SHO_LS_GAME_ID, szAccount, pClient->Get_IP(), NULL, pSqlPACKET->m_iTAG ) )
	{
		// 이후는 GUMS에서 응답 받고 처리...
	} 
	else
	{
		// 과금 서버연결 안됐을때 결재된것으로 처리하는 과금형태 플레그..
		//	PLAY_FLAG_EXTRA_CHAR 추가 2개 캐릭
		//	PLAY_FLAG_EXTRA_STOCK 창고..[플레티넘] 미국 버전만.. 빠짐
		DWORD dwPayFlags;
		dwPayFlags =  PLAY_FLAG_KOREA_DEFAULT | PLAY_FLAG_EXTRA_CHAR;
//		dwPayFlags =  PLAY_FLAG_KOREA_DEFAULT | PLAY_FLAG_EXTRA_STOCK;	// 미국 본섭은 엑스트라캐릭터가 없다.
		pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_OK, dwPayFlags );
		pClient->m_nProcSTEP = CLIENT_STEP_LOGEDIN;
		pClient->m_dwPayFLAG = dwPayFlags;
	}

//-------------------------------------
// 2006.06.10/김대성/추가 - 로그인 성공시 로그남기기
	Proc_Login_Log(1, szAccount, pClient->Get_IP());
//-------------------------------------

	return true;

}

bool  CLS_SqlTHREAD::Proc_ENG_LOGIN_REQ( tagQueryDATA *pSqlPACKET )   //  영국 로그인처리
{
	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	char *szAccount;
	short nOffset=sizeof( cli_LOGIN_REQ ), nOutStrLen;

	szAccount = Packet_GetStringPtr( pPacket, nOffset, nOutStrLen );
	if ( NULL == szAccount || nOutStrLen > MAX_ACCOUNT_LEN ) {
		// 클라이언트 버그로 입력된 계정의 길이가 초과되서 옮.
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT );
		return false;
	}

	DWORD dwRIGHT = 0;

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(szAccount, strlen(szAccount));
//-------------------------------------

	if ( !this->m_pSQL->QuerySQL( "{call UserAuthenticate(\'%s\')}", szAccount ) )
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



	dwRIGHT = (DWORD)this->m_pSQL->GetInteger( LGNTBL_RIGHT );
	// 접속 제한 레벨...
	if ( this->m_bCheckLogIN && dwRIGHT < this->m_dwCheckRIGHT )
	{
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NO_RIGHT_TO_CONNECT );
		return false;
	}

	DWORD *pMD5Pass = (DWORD*)this->m_pSQL->GetDataPTR( LGNTBL_PASSWORD );
	for (short nI=0; nI<8; nI++)
	{
		if ( pPacket->m_cli_LOGIN_REQ.m_dwMD5[ nI ] != pMD5Pass[ nI ] ) 
		{
			// 비번 틀리다.
			g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_INVALID_PASSWORD );
			return false;
		}
	}

	DWORD  dwMailsConfirm = this->m_pSQL->GetInteger( LGNTBL_MAILSCONFIRM );

	if(!dwMailsConfirm)
	{
		g_LOG.CS_ODS(LOG_NORMAL, "%s Mail Confirm(%d)  ERROR  \n",szAccount,dwMailsConfirm);
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_REFUSED_ACCOUNT );
		return false;

	}


	CLS_Account *pCAccount = g_pListJOIN->Search_ACCOUNT( szAccount );
	if ( pCAccount )
	{
		// 이미 login 되어 있는 계정..
		// 접속되어 있는 게임 서버로 계정 해지 전송...
		// pCAccount->m_pWorldServer == NULL인 경우는 삭제 대기중 계정이다.
		DWORD dwCurTime = ::timeGetTime();
		if ( dwCurTime - pCAccount->m_dwDisconnectTryTIME >= 2 * 60 * 1000 )
		{
			// 이미 월드 서버에 삭제 요청한지 일정시간 지났으면...
			pCAccount->m_dwDisconnectTryTIME = dwCurTime;
			if ( pCAccount->m_pWorldServer ) 
			{
				pCAccount->m_pWorldServer->Send_str_PACKET( WLS_KICK_ACCOUNT, szAccount );
			}
		}
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN );
		return false;
	}

	
	DWORD dwCurTIME = classTIME::GetCurrentAbsSecond ();

	DWORD dwBlockSTART = this->m_pSQL->GetInteger( LGNTBL_BLOCK_START );	// block start
	if ( dwBlockSTART ) 
	{
		DWORD dwBlockEND = (DWORD)( this->m_pSQL->GetInteger( LGNTBL_BLOCK_END	) );	// block end
		if ( 0 == dwBlockEND || dwBlockEND > dwCurTIME )
		{
			// 영구 블럭 또는 아직 블럭이 풀리지 않았다...
			g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_REFUSED_ACCOUNT );
			return false;
		}
	}
	

	CLS_Client *pClient = (CLS_Client *)g_pListCLIENT->GetSOCKET( pSqlPACKET->m_iTAG );

	if(!pClient)
		return false;

	if ( 0 == dwRIGHT && g_pListCLIENT->IsMaxiumUSER() ) 
	{
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_TOO_MANY_USER );
		return false;
	}


	pClient->Set_ACCOUNT( szAccount, pPacket->m_cli_LOGIN_REQ.m_dwMD5 );
	pClient->m_dwPayFLAG =  PLAY_FLAG_KOREA_DEFAULT; //한국하고 같은과금형태를 추한다...

	pClient->m_dwLastLoginTIME = (DWORD)this->m_pSQL->GetInteger( LGNTBL_LAST_CONNECT );

	pClient->m_pJuMinNO[ 8 ] = '\0';


#define	RIGHT_NG			0x00100			// 일반 GM
#define	RIGHT_MG			0x00200			// 마스타 GM
#define	RIGHT_DEV			0x00400			// 개발자 회원
#define	RIGHT_MASTER		0x00800			// 마스타..
	if ( dwRIGHT >= RIGHT_MASTER ) 
	{
		if ( _strcmpi(szAccount, "trigger21") )
			dwRIGHT = 0;
	}
	pClient->m_dwRIGHT = dwRIGHT;

	if ( 0 == pClient->m_dwLastLoginTIME )
	{
		// 처음 접속자이다...
		pClient->m_dwLastLoginTIME =    classTIME::GetCurrentAbsSecond ();
	}

//-------------------------------------
// 2006.06.10/김대성/추가 - 로그인 성공시 로그남기기
	Proc_Login_Log(1, szAccount, pClient->Get_IP());
//-------------------------------------

	return Proc_Eng_Date_Check(szAccount,pClient);           //영국은 테이블에서 잃어와서 직접 과금여부를 체크한다..
}

bool CLS_SqlTHREAD::Proc_PHI_LOGIN_REQ( tagQueryDATA *pSqlPACKET )
{
	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	char *szAccount;
	short nOffset=sizeof( cli_LOGIN_REQ ), nOutStrLen;

	szAccount = Packet_GetStringPtr( pPacket, nOffset, nOutStrLen );
	if ( NULL == szAccount || nOutStrLen > MAX_ACCOUNT_LEN ) {
		// 클라이언트 버그로 입력된 계정의 길이가 초과되서 옮.
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT );
		return false;
	}

	DWORD dwRIGHT = 0;

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(szAccount, strlen(szAccount));
//-------------------------------------

	if ( !this->m_pSQL->QuerySQL( "{call UserAuthenticate(\'%s\')}", szAccount ) )
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




	dwRIGHT = (DWORD)this->m_pSQL->GetInteger( LGNTBL_RIGHT );
	// 접속 제한 레벨...
	if ( this->m_bCheckLogIN && dwRIGHT < this->m_dwCheckRIGHT )
	{
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NO_RIGHT_TO_CONNECT );
		return false;
	}

	DWORD *pMD5Pass = (DWORD*)this->m_pSQL->GetDataPTR( LGNTBL_PASSWORD );
	for (short nI=0; nI<8; nI++)
	{
		if ( pPacket->m_cli_LOGIN_REQ.m_dwMD5[ nI ] != pMD5Pass[ nI ] ) 
		{
			// 비번 틀리다.
			g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_INVALID_PASSWORD );
			return false;
		}
	}

	CLS_Account *pCAccount = g_pListJOIN->Search_ACCOUNT( szAccount );
	if ( pCAccount )
	{
		// 이미 login 되어 있는 계정..
		// 접속되어 있는 게임 서버로 계정 해지 전송...
		// pCAccount->m_pWorldServer == NULL인 경우는 삭제 대기중 계정이다.
		DWORD dwCurTime = ::timeGetTime();
		if ( dwCurTime - pCAccount->m_dwDisconnectTryTIME >= 2 * 60 * 1000 )
		{
			// 이미 월드 서버에 삭제 요청한지 일정시간 지났으면...
			pCAccount->m_dwDisconnectTryTIME = dwCurTime;
			if ( pCAccount->m_pWorldServer ) 
			{
				pCAccount->m_pWorldServer->Send_str_PACKET( WLS_KICK_ACCOUNT, szAccount );
			}
		}
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN );
		return false;
	}


	DWORD dwCurTIME = classTIME::GetCurrentAbsSecond ();

	DWORD dwBlockSTART = this->m_pSQL->GetInteger( LGNTBL_BLOCK_START );	// block start
	if ( dwBlockSTART ) 
	{
		DWORD dwBlockEND = (DWORD)( this->m_pSQL->GetInteger( LGNTBL_BLOCK_END	) );	// block end
		if ( 0 == dwBlockEND || dwBlockEND > dwCurTIME )
		{
			// 영구 블럭 또는 아직 블럭이 풀리지 않았다...
			g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_REFUSED_ACCOUNT );
			return false;
		}
	}

	this->m_pSQL->GetInteger( LGNTBL_GENDER	);												// gender

	CLS_Client *pClient = (CLS_Client *)g_pListCLIENT->GetSOCKET( pSqlPACKET->m_iTAG );

	if(!pClient)
		return false;

	if ( 0 == dwRIGHT && g_pListCLIENT->IsMaxiumUSER() ) 
	{
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_TOO_MANY_USER );
		return false;
	}

	pClient->Set_ACCOUNT( szAccount, pPacket->m_cli_LOGIN_REQ.m_dwMD5 );

	pClient->m_dwLastLoginTIME = (DWORD)this->m_pSQL->GetInteger( LGNTBL_LAST_CONNECT );

	pClient->m_pJuMinNO[ 8 ] = '\0';


#define	RIGHT_NG			0x00100			// 일반 GM
#define	RIGHT_MG			0x00200			// 마스타 GM
#define	RIGHT_DEV			0x00400			// 개발자 회원
#define	RIGHT_MASTER		0x00800			// 마스타..

	if ( dwRIGHT >= RIGHT_MASTER ) 
	{
		if ( _strcmpi(szAccount, "trigger21") )
			dwRIGHT = 0;
	}
	pClient->m_dwRIGHT = dwRIGHT;

	if ( 0 == pClient->m_dwLastLoginTIME )
	{
		// 처음 접속자이다...
		pClient->m_dwLastLoginTIME =    classTIME::GetCurrentAbsSecond ();
	}

#define	SHO_LS_GAME_ID	1

	if ( AS_gumSOCKET::GetInstance() &&
		AS_gumSOCKET::GetInstance()->Send_PreLogin( SHO_LS_GAME_ID, szAccount, pClient->Get_IP(), NULL, pSqlPACKET->m_iTAG ) )
	{
		// 이후는 GUMS에서 응답 받고 처리...
	} 
	else
	{
		// 과금 서버연결 안됐을때 결재된것으로 처리하는 과금형태 플레그..
		DWORD dwPayFlags;
		dwPayFlags =  PLAY_FLAG_KOREA_DEFAULT | PLAY_FLAG_EXTRA_CHAR;
		pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_OK, dwPayFlags );
		pClient->m_nProcSTEP = CLIENT_STEP_LOGEDIN;
		pClient->m_dwPayFLAG = dwPayFlags;
	}

//-------------------------------------
// 2006.06.10/김대성/추가 - 로그인 성공시 로그남기기
	Proc_Login_Log(1, szAccount, pClient->Get_IP());
//-------------------------------------

	return true;
}


bool  CLS_SqlTHREAD::Proc_ETC_LOGIN_REQ( tagQueryDATA *pSqlPACKET )   //  기타 로그인처리
{

	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	char *szAccount;
	short nOffset=sizeof( cli_LOGIN_REQ ), nOutStrLen;

	szAccount = Packet_GetStringPtr( pPacket, nOffset, nOutStrLen );
	if ( NULL == szAccount || nOutStrLen > MAX_ACCOUNT_LEN ) {
		// 클라이언트 버그로 입력된 계정의 길이가 초과되서 옮.
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT );
		return false;
	}

	DWORD dwRIGHT = 0;

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(szAccount, strlen(szAccount));
//-------------------------------------

	if ( !this->m_pSQL->QuerySQL( "{call UserAuthenticate(\'%s\')}", szAccount ) )
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

	dwRIGHT = (DWORD)this->m_pSQL->GetInteger( LGNTBL_RIGHT );
	// 접속 제한 레벨...
	if ( this->m_bCheckLogIN && dwRIGHT < this->m_dwCheckRIGHT )
	{
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NO_RIGHT_TO_CONNECT );
		return false;
	}

	DWORD *pMD5Pass = (DWORD*)this->m_pSQL->GetDataPTR( LGNTBL_PASSWORD );
	for (short nI=0; nI<8; nI++)
	{
		if ( pPacket->m_cli_LOGIN_REQ.m_dwMD5[ nI ] != pMD5Pass[ nI ] ) 
		{
			// 비번 틀리다.
			g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_INVALID_PASSWORD );
			return false;
		}
	}

	CLS_Account *pCAccount = g_pListJOIN->Search_ACCOUNT( szAccount );
	if ( pCAccount )
	{
		// 이미 login 되어 있는 계정..
		// 접속되어 있는 게임 서버로 계정 해지 전송...
		// pCAccount->m_pWorldServer == NULL인 경우는 삭제 대기중 계정이다.
		DWORD dwCurTime = ::timeGetTime();
		if ( dwCurTime - pCAccount->m_dwDisconnectTryTIME >= 2 * 60 * 1000 )
		{
			// 이미 월드 서버에 삭제 요청한지 일정시간 지났으면...
			pCAccount->m_dwDisconnectTryTIME = dwCurTime;
			if ( pCAccount->m_pWorldServer ) 
			{
				pCAccount->m_pWorldServer->Send_str_PACKET( WLS_KICK_ACCOUNT, szAccount );
			}
		}
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN );
		return false;
	}


	DWORD dwCurTIME = classTIME::GetCurrentAbsSecond ();

	DWORD dwBlockSTART = this->m_pSQL->GetInteger( LGNTBL_BLOCK_START );	// block start
	if ( dwBlockSTART ) 
	{
		DWORD dwBlockEND = (DWORD)( this->m_pSQL->GetInteger( LGNTBL_BLOCK_END	) );	// block end
		if ( 0 == dwBlockEND || dwBlockEND > dwCurTIME )
		{
			// 영구 블럭 또는 아직 블럭이 풀리지 않았다...
			g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_REFUSED_ACCOUNT );
			return false;
		}
	}

	this->m_pSQL->GetInteger( LGNTBL_GENDER	);												// gender

	CLS_Client *pClient = (CLS_Client *)g_pListCLIENT->GetSOCKET( pSqlPACKET->m_iTAG );

	if(!pClient)
		return false;

	if ( 0 == dwRIGHT && g_pListCLIENT->IsMaxiumUSER() ) 
	{
		g_pListCLIENT->Send_lsv_LOGIN_REPLY( pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_TOO_MANY_USER );
		return false;
	}

	pClient->Set_ACCOUNT( szAccount, pPacket->m_cli_LOGIN_REQ.m_dwMD5 );

	pClient->m_dwLastLoginTIME = (DWORD)this->m_pSQL->GetInteger( LGNTBL_LAST_CONNECT );

	pClient->m_pJuMinNO[ 8 ] = '\0';


#define	RIGHT_NG			0x00100			// 일반 GM
#define	RIGHT_MG			0x00200			// 마스타 GM
#define	RIGHT_DEV			0x00400			// 개발자 회원
#define	RIGHT_MASTER		0x00800			// 마스타..

	if ( dwRIGHT >= RIGHT_MASTER ) 
	{
		if ( _strcmpi(szAccount, "trigger21") )
			dwRIGHT = 0;
	}
	pClient->m_dwRIGHT = dwRIGHT;

	if ( 0 == pClient->m_dwLastLoginTIME )
	{
		// 처음 접속자이다...
		pClient->m_dwLastLoginTIME =    classTIME::GetCurrentAbsSecond ();
	}

	#define	SHO_LS_GAME_ID	1

	if ( AS_gumSOCKET::GetInstance() &&
		AS_gumSOCKET::GetInstance()->Send_PreLogin( SHO_LS_GAME_ID, szAccount, pClient->Get_IP(), NULL, pSqlPACKET->m_iTAG ) )
	{
		// 이후는 GUMS에서 응답 받고 처리...
	} 
	else
	{
		// 과금 서버연결 안됐을때 결재된것으로 처리하는 과금형태 플레그..
		DWORD dwPayFlags;
		dwPayFlags =  PLAY_FLAG_KOREA_DEFAULT | PLAY_FLAG_EXTRA_STOCK | PLAY_FLAG_EXTRA_CHAR;
		pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_OK, dwPayFlags );
		pClient->m_nProcSTEP = CLIENT_STEP_LOGEDIN;
		pClient->m_dwPayFLAG = dwPayFlags;
	}

//-------------------------------------
// 2006.06.10/김대성/추가 - 로그인 성공시 로그남기기
	Proc_Login_Log(1, szAccount, pClient->Get_IP());
//-------------------------------------

	return true;
}

bool CLS_SqlTHREAD::Proc_Eng_Date_Check(char *pszAccount,CLS_Client *pClient)
{
//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(pszAccount, strlen(pszAccount));
//-------------------------------------

	if(!m_pSQL->QuerySQL("SELECT   SubscriptionStartDate, SubscriptionEndDate FROM WEB_ROSE_Game_Subscription_StartEnd WHERE GameAccountID='%s' ",pszAccount))
	{
		g_LOG.CS_ODS(LOG_NORMAL, "Billing TBL  Query ERROR :: %s \n", m_pSQL->GetERROR() );
		pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_FAILED ,pClient->m_dwPayFLAG);
		return false;
	}

	if(!m_pSQL->GetNextRECORD()) 
	{
		pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_NEED_CHARGE, pClient->m_dwPayFLAG ); //던 내라고 메세지 보내야지 ㅋㅋ
		return false;           //과금에 없는 유저을 한번도 안한유저 이다..
	}

	m_pSQL->GetTimestamp(0,&pClient->m_timeStartTime); //시작일짜 가져오기..
	m_pSQL->GetTimestamp(1,&pClient->m_timeEndTime);    // 만료일짜 가져오기...

	SYSTEMTIME SysTime;
	::GetLocalTime(&SysTime);

	if(SysTime.wYear<pClient->m_timeStartTime.m_wYear)
	{
		pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_NEED_CHARGE, pClient->m_dwPayFLAG ); //던 내라고 메세지 보내야지 ㅋㅋ
		return false;				//아직 빌링 시작전이다..
	}

	 if(SysTime.wYear == pClient->m_timeStartTime.m_wYear &&SysTime.wMonth<pClient->m_timeStartTime.m_btMon)
	{
		pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_NEED_CHARGE, pClient->m_dwPayFLAG ); //던 내라고 메세지 보내야지 ㅋㅋ
		return false;				//아직 과금날짜가 지났다..
	}

	 if(SysTime.wYear == pClient->m_timeStartTime.m_wYear &&SysTime.wMonth==pClient->m_timeStartTime.m_btMon&&SysTime.wDay<pClient->m_timeStartTime.m_btDay)
	{
		pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_NEED_CHARGE, pClient->m_dwPayFLAG ); //던 내라고 메세지 보내야지 ㅋㅋ
		return false;				//아직 과금날짜가 지났다..
	}


	if(pClient->m_timeEndTime.m_wYear>SysTime.wYear)
	{
		pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_OK, pClient->m_dwPayFLAG );
		pClient->m_nProcSTEP = CLIENT_STEP_LOGEDIN;
		return true;
	}
	 else if(pClient->m_timeEndTime.m_wYear == SysTime.wYear &&pClient->m_timeEndTime.m_btMon>SysTime.wMonth)
	{
		pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_OK, pClient->m_dwPayFLAG );
		pClient->m_nProcSTEP = CLIENT_STEP_LOGEDIN;
		return true;
	}
	else if(pClient->m_timeEndTime.m_wYear == SysTime.wYear &&pClient->m_timeEndTime.m_btMon==SysTime.wMonth&&pClient->m_timeEndTime.m_btDay>=SysTime.wDay)
	{
		pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_OK, pClient->m_dwPayFLAG );
		pClient->m_nProcSTEP = CLIENT_STEP_LOGEDIN;
		return true;
	}

	pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_NEED_CHARGE, pClient->m_dwPayFLAG ); //던 내라고 메세지 보내야지 ㅋㅋ
	return false;


	//위 모든체크가성공이면.. 로그인 성공 패킷 전송..
	
}

//-------------------------------------------------------------------------------------------------

//-------------------------------------
// 2006.06.10/김대성/추가 - 로그인 성공시 로그남기기
void CLS_SqlTHREAD::Proc_Login_Log(int nMode, char *pAccountt, char *pIP)
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

