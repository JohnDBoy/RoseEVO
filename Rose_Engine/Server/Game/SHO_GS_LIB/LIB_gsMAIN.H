#ifndef	__LIB_GAMEMAIN_H
#define	__LIB_GAMEMAIN_H
#include "CGS_API.h"
//-------------------------------------------------------------------------------------------------

#ifndef	_WINDOWS_
	#define	_WIN32_WINNT	0x0500
#endif

#define	__SHO_GS 

//	#define	__USE_TRACE


#include <windows.h>
#include <wchar.h>
#include <crtdbg.h>
#include <math.h>
#include <assert.h>

#include "StdAfx.h"
#include "LIB_Util.h"
#include "CVector.h"
#include "DataTYPE.h"
#include "NET_Prototype.h"
#include "DLLIST.h"
#include "SLLIST.h"
#include "classLOG.h"
#include "classSTR.h"
#include "classUTIL.h"
#include "classHASH.h"
#include "classINDEX.h"
#include "classIOCP.h"
#include "classTIME.h"
#include "classTRACE.h"
#include "classSYNCOBJ.h"
#include "CClientSOCKET.h"

#include "ETC_Math.h"

#include "IO_STB.h"
#include "OBJECT.h"
#include "ClanField_Control.h"

//-------------------------------------------------------------------------------------------------

/**
 * \ingroup SHO_GS_LIB
 * \file	LIB_gsMAIN.h
 * \class	CLIB_GameSRV
 * \author	wookSang.Jo
 * \brief	gs 전역 싱글톤 클래스
 */
class CLIB_GameSRV {
private  :
	static CLIB_GameSRV	*m_pInstance;
	EXE_GS_API *m_pExeAPI;

	HWND		m_hMainWND;				///< 윈도우 핸들
	CTimer	   *m_pWorldTIMER;			///< 월드 타이머
	
	//	클랜 필드 타이머
#ifdef __CLAN_FIELD_SET
	CTimer	   *m_pClanFieldTIMER;		/// 클랜 필드 타이머
#endif

	//	클랜전 타이머 메시지 선언
#ifdef __CLAN_WAR_SET
	CTimer	   *m_pClanWarTIMER;			///<클랜전 타이머
#endif

//-------------------------------------
// 2006.12.06/김대성/추가 - 경험치 X배 치트키 시간대별 자동 실행
#ifdef __CHEAT_SET_WORLDEXP
	CTimer	   *m_pWorldExpTIMER;			///< 경험치 X배 치트키 시간대별 자동 실행 타이머
#endif
//-------------------------------------

//-------------------------------------
// 2007.02.14/김대성/추가 - 기간제 아이템
#ifdef __ITEM_TIME_LIMIT
	CTimer	   *m_pItemTimeLimitCheckTIMER;			
#endif
//-------------------------------------


	bool	   *m_pCheckedLocalZONE;

	CStrVAR	m_BaseDataDIR;				///< 게임 데이타 폴더
	CStrVAR	m_ServerNAME;				///< 설정된 서버 이름
	CStrVAR m_ServerIP;					///< 서버 IP
	int		m_iListenPortNO;			///< 사용자 접속 포트번호

	CStrVAR	m_DBServerIP;				///< 디비 서버 IP
	CStrVAR	m_DBName;					///< 디비 네임
	CStrVAR	m_DBUser;					///< 디비 사용자
	CStrVAR	m_DBPassword;				///< 디비 사용자 비밀번호
	CStrVAR	m_LogUser;
	CStrVAR	m_LogPW;

	CStrVAR m_AccountServerIP;			///< sho_as에 접속할 IP
	int		m_iAccountServerPORT;		///< sho_as에 접속할 port no

	CStrVAR m_LoginServerIP;			///< sho_ws에 접속할 IP
	int		m_iLoginServerPORT;			///< sho_ws에 접속할 port no

	DWORD	m_dwRandomSEED;

	BYTE	m_btChannelNO;				///< 동작중인 채널 번호
	BYTE	m_btLowAGE;					///< 연령 접속 제한 최저 나이
	BYTE	m_btHighAGE;				///< 연령 접속 제한 최고 나이
	DWORD	m_dwUserLIMIT;				///< 최대 접속 가능한 인원수

	int		m_iLangTYPE;				///< 국가 코드
	bool	m_bTestServer;				///< 테스트 서버인가 ?

	CLIB_GameSRV ( EXE_GS_API *pExeAPI );
	virtual ~CLIB_GameSRV ();
	void SystemINIT( HINSTANCE hInstance, char *szBaseDataDIR, int iLangType );


	bool CheckSTB_UseITEM ();
	bool CheckSTB_NPC ();
	bool CheckSTB_DropITEM ();
	bool CheckSTB_ReNewalDropITEM ();

	bool CheckSTB_ItemRateTYPE ();
	bool CheckSTB_Motion ();
	bool CheckSTB_GemITEM ();
	bool CheckSTB_ListPRODUCT ();

	bool Load_BasicDATA ();
	void Free_BasicDATA ();

	bool ConnectToLSV ();
	void DisconnectFromLSV ();

	bool ConnectToASV ();
	void DisconnectFromASV ();

	bool ConnectToLOG ();
	void DisconnectFromLOG ();

	void TranslateNameWithDescKey	( STBDATA *pOri, char *szStbFile, int iNameCol );
	void TranslateNameWithColoumKey ( STBDATA *pOri, char *szStbFile, int iLangCol, int iNameCol, int iDescCol );

	friend VOID CALLBACK GS_TimerProc ( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );

public   :
	bool ConnectSERVER( char *szDBServerIP,		char *szDBName,
						char *szDBUser,			char *szDBPW,
						char *szLogUser,		char *szLogPW,
						char *szMallUser,		char *szMallPW,
						char *szLoginServerIP,	int iLoginServerPort,
						char *szLogServerIP,	int iLogServerPortNO );

	bool Start( HWND hMainWND, char *szServerName, char *szClientListenIP, int iClientListenPort, BYTE btChannelNO, BYTE btLowAge, BYTE btHighAge );
	void Shutdown ();

	short InitLocalZone( bool bAllActive );
	bool  CheckZoneToLocal(short nZoneNO, bool bChecked);
	char* GetZoneName( short nZoneNO );
	bool  IsLocalCheckedZONE( short nZoneNO )	{	return m_pCheckedLocalZONE[nZoneNO];	}	

	char* GetServerName()						{	return m_ServerNAME.Get();	}
	char* GetServerIP()							{	return m_ServerIP.Get();	}
	int	  GetListenPort()						{	return m_iListenPortNO;		}
	DWORD GetRandomSeed()						{	return m_dwRandomSEED;		}

	BYTE  GetChannelNO ()						{	return m_btChannelNO;		}
	BYTE  GetLowAGE ()							{	return m_btLowAGE;			}
	BYTE  GetHighAGE ()							{	return m_btHighAGE;			}
	bool  IsTestServer()						{	return m_bTestServer;		}

	int	  GetLangTYPE()							{	return m_iLangTYPE;			}

	void  Send_ANNOUNCE( short nZoneNO, char *szMsg );
	void  Set_UserLIMIT( DWORD dwUserLimit );
	DWORD Get_UserLIMIT()						{	return m_dwUserLIMIT;		}
	
	static EXE_GS_API *ExeAPI()					{	return m_pInstance->m_pExeAPI;	}
	static CLIB_GameSRV *GetInstance ()			{	return m_pInstance;	}
	static CLIB_GameSRV *InitInstance( HINSTANCE hInstance, char *szBaseDataDIR, EXE_GS_API *pExeAPI, int iLangType )
	{
		if ( NULL == m_pInstance ) {
			m_pInstance = new CLIB_GameSRV( pExeAPI );
			_ASSERT( m_pInstance );
			m_pInstance->SystemINIT( hInstance, szBaseDataDIR, iLangType );
		}
		return m_pInstance;
	}
	void Destroy ()								{	SAFE_DELETE( m_pInstance );	}

//-------------------------------------
// 2006.09.28/김대성/추가 - 영구히 삭제해야하는 아이템 목록
	int read_LIST_DELITEM_STB();
//-------------------------------------

//-------------------------------------
// 2007.01.10/김대성/추가 - 서버다운된 시간 리스트
#ifdef __ITEM_TIME_LIMIT
	int read_ServerDownTime();
#endif
//-------------------------------------
} ;

#define	GS_TIMER_LSV			1
#define	GS_TIMER_LOG			2
#define	GS_TIMER_WORLD_TIME		3
#define	GS_TIMER_ASV			4

//	클랜 필드 타이머
#ifdef __CLAN_FIELD_SET
#define GS_TIMER_CLANFIELD		822
#endif

//	클랜전 타이머 메시지 선언
#ifdef __CLAN_WAR_SET
#define GS_TIMER_CLANWAR		820
#endif

//-------------------------------------
// 2006.12.06/김대성/추가 - 경험치 X배 치트키 시간대별 자동 실행
#ifdef __CHEAT_SET_WORLDEXP
#define GS_TIMER_CHEAT_SET_WORLDEXP		821
#endif
//-------------------------------------
//-------------------------------------
// 2007.02.14/김대성/추가 - 기간제 아이템
#ifdef __ITEM_TIME_LIMIT
#define GS_TIMER_ITEM_TIME_LIMIT_CHECK		823
#endif
//-------------------------------------

#define	RECONNECT_TIME_TICK		10000	// 10 sec

extern VOID CALLBACK GS_TimerProc ( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );

extern void Save_AllUSER ();

extern DWORD GetServerBuildNO ();
extern DWORD GetServerStartTIME ();

inline short Get_ServerChannelNO()
{
	return CLIB_GameSRV::GetInstance()->GetChannelNO();
}
inline int	 Get_ServerLangTYPE()
{
	return CLIB_GameSRV::GetInstance()->GetLangTYPE();
}

//-------------------------------------
// 2006.09.28/김대성/추가 - 영구히 삭제해야하는 아이템 목록
#include <map>
extern std::map<int, int> g_mapDeleteItemList;
//-------------------------------------

//-------------------------------------
// 2007.01.10/김대성/추가 - 서버다운된 시간 리스트
#ifdef __ITEM_TIME_LIMIT
#include <map>
extern std::map<int, _DOWN_TIME> g_mapServerDownTime;
#endif
//-------------------------------------

//-------------------------------------------------------------------------------------------------
#endif
