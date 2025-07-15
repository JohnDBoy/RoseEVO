#include "LIB_gsMAIN.h"
#include "CSocketWND.h"

#include "GS_ThreadZONE.h"
#include "GS_ThreadSQL.h"
#include "GS_ThreadLOG.h"

#include "GS_ListUSER.h"
#include "ZoneLIST.h"
#include "GS_PARTY.h"
#include "IO_AI.h"
#include "IO_Skill.h"
#include "IO_Quest.h"
#include "IO_PAT.h"
#include "classTIME.h"
#include "ioDataPOOL.h"
#include "GS_SocketLSV.h"
#include "GS_SocketASV.h"
#include "CUrlTHREAD.h"
#include "CThreadGUILD.h"
#include "GS_ThreadMALL.h"
#include "classFILE.h"
#include "MiniDumper.h"


#include <ctime>
#include <cstdlib>



////------------------------------------------------------------------------------
//// (20060831:남병철) : 프로토콜 디버그 및 분석용 ZLOG, ZTRACE 추가
//#include "ZSTD.h"
// (20060831:남병철) : 디버깅용
ZLog g_WatchData( ZLOG_WINDOW|ZLOG_FILE, "WATCH_DATA_LOG.txt" );
//ZLog g_GSUser( ZLOG_WINDOW );//|ZLOG_FILE, "GS_USER_LOG.txt" );
//ZLog g_ObjCHAR( ZLOG_WINDOW|ZLOG_FILE, "OBJ_CHAR_LOG.txt" );
//ZLog g_ObjNPC( ZLOG_WINDOW );//|ZLOG_FILE, "OBJ_NPC_LOG.txt" );
//ZLog g_RegenAREA( ZLOG_WINDOW|ZLOG_FILE, "REGEN_AREA_LOG.txt" );
ZStringList g_zQuestList[QLZ_MAX];
////------------------------------------------------------------------------------

//-------------------------------------
// 2007.02.14/김대성/추가 - 기간제 아이템
#ifdef __ITEM_TIME_LIMIT
int			m_nItemTimeLimitCheckHour;


#include "GF_Define.h"



//	접속자 유저 리스트
extern	classUSER	*g_User_List[DF_MAX_USER_LIST];
#endif
//-------------------------------------

// #include "AStrTable.h"

#define __MAX_ITEM_999_EXT //정의			// 2006.07.06/김대성/추가 - 아이템 999 제한 확장

#define	TEST_ZONE_NO 250

// 최대 게임 객체수 ( 사용자 포함 )
#define	MAX_GAME_OBJECTS			65535
#ifdef	__INC_WORLD
	// 개인 서버 :: 최대 접속 가능자.
	#undef	MAX_ZONE_USER_BUFF
	#define	MAX_ZONE_USER_BUFF			64

	#define	DEF_GAME_USER_POOL_SIZE		128
	#define	INC_GAME_USER_POOL_SIZE		128

	#define	DEF_GAME_PARTY_POOL_SIZE	128
	#define	INC_GAME_PARTY_POOL_SIZE	128

	#define	INC_RECV_IO_POOL_SIZE		( DEF_GAME_USER_POOL_SIZE )
	#define	INC_SEND_IO_POOL_SIZE		( DEF_GAME_USER_POOL_SIZE )
	#define	INC_PACKET_POOL_SIZE		( DEF_GAME_USER_POOL_SIZE )

	#include "CChatROOM.h"
	CChatRoomLIST	*g_pChatROOMs=NULL;

	#include "CThreadMSGR.h"
	CThreadMSGR		*g_pThreadMSGR = NULL;

	#define	DEF_RECV_IO_POOL_SIZE		( DEF_GAME_USER_POOL_SIZE + INC_GAME_USER_POOL_SIZE )
	#define	DEF_SEND_IO_POOL_SIZE		( DEF_GAME_USER_POOL_SIZE + INC_GAME_USER_POOL_SIZE )
	#define	DEF_PACKET_POOL_SIZE		( DEF_RECV_IO_POOL_SIZE + DEF_SEND_IO_POOL_SIZE )
#else
	#define	DEF_GAME_USER_POOL_SIZE		8192
	#define	INC_GAME_USER_POOL_SIZE		1024

	#define	DEF_GAME_PARTY_POOL_SIZE	4096
	#define	INC_GAME_PARTY_POOL_SIZE	1024

	#define	INC_RECV_IO_POOL_SIZE		2048
	#define	INC_SEND_IO_POOL_SIZE		8192
	#define	INC_PACKET_POOL_SIZE		8192

	#define	DEF_RECV_IO_POOL_SIZE		( DEF_GAME_USER_POOL_SIZE + DEF_GAME_USER_POOL_SIZE/2 )
	#define	DEF_SEND_IO_POOL_SIZE		32768
	#define	DEF_PACKET_POOL_SIZE		49152	// 32768+16834
#endif

//-------------------------------------------------------------------------------------------------
STBDATA			 g_TblHAIR("hair");
STBDATA			 g_TblFACE("face");
STBDATA			 g_TblARMOR("armor");
STBDATA			 g_TblGAUNTLET("gauntlet");
STBDATA			 g_TblBOOTS("boots");
STBDATA			 g_TblHELMET("helmet");
STBDATA			 g_TblWEAPON("weapon");
STBDATA			 g_TblSUBWPN("sub-weapon");
STBDATA			 g_TblEFFECT("effect");
STBDATA			 g_TblNPC("npc");
STBDATA			 g_TblAniTYPE("ani-type");
STBDATA			 g_TblPRODUCT("product");
STBDATA			 g_TblNATUAL("natual");

STBDATA			 g_TblFACEITEM("face-item");
STBDATA			 g_TblUSEITEM("use-item");
STBDATA			 g_TblBACKITEM("back-item");
STBDATA			 g_TblGEMITEM("gem-item");
STBDATA			 g_TblQUESTITEM("quest-item");
STBDATA			 g_TblJEWELITEM("jewel-item");

STBDATA			 g_TblDropITEM("drop-item");

STBDATA          g_TblStore("store");

STBDATA	         g_TblWARP("warp");
STBDATA			 g_TblEVENT("event");

STBDATA			 g_TblZONE("zone");

STBDATA			*g_pTblSTBs[ ITEM_TYPE_RIDE_PART+1 ];

STBDATA			 g_TblAVATAR("avatar");		// 아바타 초기 설정 데이타..
STBDATA			 g_TblSTATE("state");		/// 캐릭터의 상태를 변경하는 정보.

STBDATA			 g_TblUnion("union");
STBDATA			 g_TblClass("class");
STBDATA			 g_TblItemGRADE("item-grade");

STBDATA			 g_TblSkillPoint("skill-point");
STBDATA			 g_TblATTR("attr");

STBDATA			 g_TblUPGRADE("item-upgrade");		// 재련 리뉴얼

STBDATA			g_TblBreak("item-break");            //한국 리뉴얼 아이템 분해..

// classMYSQL       g_MySQL;

CObjMNG         *g_pObjMGR = NULL;
CAI_LIST		 g_AI_LIST;

CUserLIST		*g_pUserLIST;
CZoneLIST		*g_pZoneLIST;
CCharDatLIST	*g_pCharDATA;
CMotionLIST		 g_MotionFILE;
CPartyBUFF		*g_pPartyBUFF;

		GS_lsvSOCKET	*g_pSockLSV = NULL;
		GS_asvSOCKET	*g_pSockASV = NULL;
//		GS_logSOCKET	*g_pSockLOG = NULL;

		GS_CThreadSQL	*g_pThreadSQL = NULL;
		GS_CThreadLOG	*g_pThreadLOG = NULL;
		CurlTHREAD		*g_pThreadURL = NULL;

		CThreadGUILD	*g_pThreadGUILD = NULL;

		GS_CThreadMALL  *g_pThreadMALL = NULL;

		char			 g_szURL[ MAX_PATH ];

		classLogFILE	 g_ChatLOG;
		classLogFILE	 g_ChatGMLOG;
		int				 g_iChatLogDAY; /// 로그 파일을 주기적으로 바꾸기 위한 날짜

#define	BASE_DATA_DIR	m_BaseDataDIR.Get()	//	"..\\..\\sho\\"


CLIB_GameSRV *CLIB_GameSRV::m_pInstance=NULL;
CPacketCODEC *g_pPacketCODEC;

//Minidumpper
MiniDumper g_MD;


#define	__GEN_TEXT

FILE	*g_fpTXT=NULL;

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#define	WORLD_TIME_TICK			10000	// 10 sec


enum eUserCntIDX {
	USERCNT_HG = 0,
	USERCNT_EX,
	USERCNT_BB,
	USERCNT_GL,
//-------------------------------------
// 2007.03.12/김대성/추가 - 일본 웹 ActiveX 연동
	USERCNT_LV,
	USERCNT_FJ,
//-------------------------------------
	USERCNT_MAX
} ;
int	g_iUserCount[ USERCNT_MAX ];

#define	LANG_KOR			0
#define	LANG_ENG			1
#define	LANG_JPN			2
#define	LANG_TWAIN			3
#define	LANG_CHINA			4
#define LANG_PHILIPPINE		5
#define LANG_USA			6
#define LANG_EUROPE		    7

bool IsJAPAN ()
{
	return ( LANG_JPN == ::Get_ServerLangTYPE() );
}
bool IsTAIWAN ()
{
#ifdef	__TAW_RENEWAL
	return true;
#else
//	return ( LANG_TWAIN == ::Get_ServerLangTYPE() );
	switch( ::Get_ServerLangTYPE() ) {
		case LANG_TWAIN :
		case LANG_PHILIPPINE :
		case LANG_USA:
		case LANG_EUROPE:
		case LANG_ENG:
			return true;
	} 
	return false;
#endif
}
bool IsIROSE ()
{
//	return ( LANG_ENG == ::Get_ServerLangTYPE() );
	switch( ::Get_ServerLangTYPE() ) {
		case LANG_TWAIN :
		case LANG_ENG :
			return true;
	} 
	return false;
}

VOID CALLBACK GS_TimerProc (HWND hwnd/* handle to window */, UINT uMsg/* WM_TIMER message */, UINT_PTR idEvent/* timer identifier */, DWORD dwTime/* current system time */ ) 
{
	switch( idEvent ) {
		case GS_TIMER_ASV :
		{
			CLIB_GameSRV *pGS = CLIB_GameSRV::GetInstance();
			if ( pGS ) {
				pGS->ConnectToASV ();
			}
			break;
		}

		case GS_TIMER_LSV :
		{
			CLIB_GameSRV *pGS = CLIB_GameSRV::GetInstance();
			if ( pGS ) {
				pGS->ConnectToLSV ();
			}
			break;
		}

		case GS_TIMER_LOG :
		{
			CLIB_GameSRV *pGS = CLIB_GameSRV::GetInstance();
			if ( pGS ) {
				pGS->ConnectToLOG ();
			}
			break;
		}

#ifdef __CLAN_FIELD_SET
		//	클랜 필드 타이머 처리.
		case GS_TIMER_CLANFIELD:
		{
			////	클랜 필드 타이머 처리 함수
			GF_CF_Time_Proc();
			break;
		}
#endif

#ifdef __CLAN_WAR_SET
		////	클랜전 타이머 처리
		case GS_TIMER_CLANWAR:
		{

#ifdef __CLAN_WAR_EXT
			//	클랜전 타이머 처리 함수..
			CClanWar::GetInstance()->TimeProc();
#else
			GF_CWar_Time_Proc();
#endif

			break;
		}
#endif


//-------------------------------------
// 2006.12.06/김대성/추가 - 경험치 X배 치트키 시간대별 자동 실행
#ifdef __CHEAT_SET_WORLDEXP
		case GS_TIMER_CHEAT_SET_WORLDEXP:
		{
			struct _A
			{
				char Year[5]; char Month[3]; char Day[3]; char Hour[3]; char Min[3]; char Sec[3];
				int nYear, nMonth, nDay, nHour, nMin, nSec;

				Set()	// 2006.12.06 15:46:00
				{
					Year[4] = '\0'; Month[2] = '\0'; Day[2] = '\0'; Hour[2] = '\0'; Min[2] = '\0'; Sec[2] = '\0';

					nYear = atoi(Year); nMonth = atoi(Month); nDay = atoi(Day);
					nHour = atoi(Hour);	nMin = atoi(Min); nSec = atoi(Sec);
				};
			};

			_A Bt, Et;
			char szExp[100];
			short nExp;
			char buf[1024];
			int i;
			bool bChange = false;

			for(i=1; i<1000; i++)
			{
				sprintf(buf, "Begin%d", i);
				GetPrivateProfileString("SetWorldExp", buf, "", (char *)&Bt, sizeof(Bt), "./SHO_GS.ini");
				sprintf(buf, "End%d", i);
				GetPrivateProfileString("SetWorldExp", buf, "", (char *)&Et, sizeof(Et), "./SHO_GS.ini");
				sprintf(buf, "Exp%d", i);
				GetPrivateProfileString("SetWorldExp", buf, "", szExp, sizeof(szExp), "./SHO_GS.ini");
			
				if(strcmp((char *)&Bt, "") == 0)
					break;

				Bt.Set(); Et.Set(); nExp = atoi(szExp);
			
				time_t ct, bt, et;
				struct tm t;

				// 현재시간
				time( &ct );				/* Get time as long integer. */

				// 시작시간
				t.tm_year = Bt.nYear - 1900; t.tm_mon = Bt.nMonth - 1; t.tm_mday = Bt.nDay;
				t.tm_hour = Bt.nHour; t.tm_min = Bt.nMin; t.tm_sec = Bt.nSec;
				bt = mktime( &t);	

				// 끝시간
				t.tm_year = Et.nYear - 1900; t.tm_mon = Et.nMonth - 1; t.tm_mday = Et.nDay;
				t.tm_hour = Et.nHour; t.tm_min = Et.nMin; t.tm_sec = Et.nSec;
				et = mktime( &t);	

				if(bt > et)	// 시작시간이 끝시간보다 크면 시간설정이 잘못됐다.
					continue;

				if(ct >= bt && ct <= et)	// 설정한 시간범위에 포함되면
				{
					g_pZoneLIST->Set_WorldVAR( WORLD_VAR_EXP, nExp );	// 설정한 값으로 세팅
					bChange = true;
				}
			}

			if(bChange == false)	// 변경된게 없으면...
			{
				// 기본값으로 세팅
				g_pZoneLIST->Set_WorldVAR( WORLD_VAR_EXP, 100 );
			}

			break;
		}
#endif
//-------------------------------------
//-------------------------------------
// 2007.02.14/김대성/추가 - 기간제 아이템
#ifdef __ITEM_TIME_LIMIT
		case GS_TIMER_ITEM_TIME_LIMIT_CHECK:
		{
			struct tm *l;
			time_t long_time;
			time( &long_time );				
			l = localtime( &long_time );

			// 매 시간마다...
			if(m_nItemTimeLimitCheckHour == l->tm_hour)
				break;
			m_nItemTimeLimitCheckHour = l->tm_hour;

			// 서버에 접속해있는 모든 유저의 인벤토리를 조사해서 사용기간이 지난 아이템은 삭제한다.
			for(DWORD i = 0 ; i < DF_MAX_USER_LIST; i++)
			{
				if(g_User_List[i] != NULL)
				{
					g_User_List[i]->Cheat_item_Time_Limit_Check2( g_User_List[i], false );
				}
			}

		}
#endif
//-------------------------------------

		case GS_TIMER_WORLD_TIME :
		{
			g_pZoneLIST->Inc_WorldTIME ();
#ifndef	__INC_WORLD
			// 일본
			if ( LANG_JPN == ::Get_ServerLangTYPE() ) {
				// if ( 0 == ( g_pZoneLIST->m_dwAccTIME & 0x07 ) ) {
					// 10초 * 0x??간격 마다...
				static DWORD dwJapanTime = g_pZoneLIST->m_dwAccTIME;
				if ( 6 <= ( g_pZoneLIST->m_dwAccTIME - dwJapanTime ) ) {
					dwJapanTime = g_pZoneLIST->m_dwAccTIME;

					CLIB_GameSRV *pGS = CLIB_GameSRV::GetInstance();
					if ( pGS ) {
						// 일본
						if ( LANG_JPN == ::Get_ServerLangTYPE() ) {
							// http://www.roseon.jp/rta/reg_rta.asp?server_ip=xxx.xxx.xxx.xxx&total_count=3000&HG_count=1400&EX_count=500&BB_count=200
//-------------------------------------
/*
							sprintf( g_szURL, "http://www.roseon.jp/rta/reg_rta.asp?server_ip=%s&total_count=%d&HG_count=%d&EX_count=%d&BB_count=%d&GL_count=%d",
*/
// 2007.03.12/김대성/수정 - 일본 웹 ActiveX 연동
							sprintf( g_szURL, "http://www.roseon.jp/rta/reg_rta.asp?server_ip=%s&total_count=%d&HG_count=%d&EX_count=%d&BB_count=%d&GL_count=%d&LV_count=%d&FJ_count=%d",
//-------------------------------------
/*
								pGS->GetServerIP(), g_pUserLIST->GetUsedSocketCNT(),
*/
// 2006.10.11/김대성/수정 - 동접자수가 모니터링툴과 맞지않는 문제 해결
								pGS->GetServerIP(), g_pUserLIST->Get_AccountCNT(),
//-------------------------------------

//-------------------------------------
/*
								g_iUserCount[ USERCNT_HG ], g_iUserCount[ USERCNT_EX ], g_iUserCount[ USERCNT_BB ], g_iUserCount[ USERCNT_GL ] );
*/
// 2007.03.12/김대성/수정 - 일본 웹 ActiveX 연동
								g_iUserCount[ USERCNT_HG ], g_iUserCount[ USERCNT_EX ], g_iUserCount[ USERCNT_BB ], g_iUserCount[ USERCNT_GL ], g_iUserCount[ USERCNT_LV ], g_iUserCount[ USERCNT_FJ ] );
//-------------------------------------

							g_pThreadURL->SendURL( g_szURL );
						}
					}
				}
			} else {
				// 한국
				if ( 0 == ( g_pZoneLIST->m_dwAccTIME & 0x00f ) ) {
					// 0x00f = 16, 10초 * 16 = 160초 = 2.6초
					CLIB_GameSRV *pGS = CLIB_GameSRV::GetInstance();
					if ( pGS ) {
						// http://roseonline.co.kr/statistics/stime.asp?channelip=xxx.xxx.xxx.xxx&cnt=1500
						sprintf( g_szURL, "http://roseonline.co.kr/statistics/stime.asp?channelip=%s&cnt=%d",
//-------------------------------------
/*
							pGS->GetServerIP(), g_pUserLIST->GetUsedSocketCNT() );
*/
// 2006.10.11/김대성/수정 - 동접자수가 모니터링툴과 맞지않는 문제 해결
							pGS->GetServerIP(), g_pUserLIST->Get_AccountCNT() );
//-------------------------------------

						g_pThreadURL->SendURL( g_szURL );
					}
				}
			}

			switch( g_pZoneLIST->m_dwAccTIME % 6 ) {
				case 0 : case 3 :		// 30초에 한번씩 체크...	
					g_pUserLIST->Check_SocketALIVE ();
					break;
				case 1 : // case 4 :
					g_pUserLIST->CloseIdleSCOKET( 90*1000 );
					break;
			}
#endif

			if( IsTAIWAN() )
			{
				if( Get_WorldVAR( WORLD_VAR_DAY ) > g_iChatLogDAY ) /// 하루가 지나면 로그 파일을 바꿔라
				{
					g_iChatLogDAY = Get_WorldVAR( WORLD_VAR_DAY );
					if( ::g_ChatLOG.IsOpened() )
						::g_ChatLOG.Close();
					if( !::g_ChatLOG.Open( "USER" ) )
						g_LOG.CS_OUT( 0xffff, "User Chat Log file can't be opened" );

					if( ::g_ChatGMLOG.IsOpened() )
						::g_ChatGMLOG.Close();
					if( !::g_ChatGMLOG.Open( "GM" ) )
						g_LOG.CS_OUT( 0xffff, "GM Chat Log file can't be opened" );
				}
			}

			break;
		}
	} ;
}

//-------------------------------------------------------------------------------------------------
void WriteLOG (char *szMSG)
{
	CLIB_GameSRV::ExeAPI()->WriteLOG( szMSG );
}

#define	TAG_HG	0x4840		// @H G
#define	TAG_EX	0x4540		// @E X
#define	TAG_BB	0x4240		// @B B
#define	TAG_GL	0x4740		// @G L

//-------------------------------------
// 2007.03.12/김대성/추가 - 일본 웹 ActiveX 연동
#define	TAG_LV	0x4C40		// @L V
#define	TAG_FJ	0x4640		// @F J
//-------------------------------------

void IncUserCNT( int iUserCNT, classUSER *pUSER )
{
	if ( IsJAPAN() ) {
		char *szAccount = pUSER->Get_ACCOUNT();
		if ( pUSER->Get_AccountLEN() >= 4 ) {
			switch( *( (WORD *)(&szAccount[ pUSER->Get_AccountLEN()-3 ]) ) ) {
				case TAG_HG :
					g_iUserCount[ USERCNT_HG ] ++;
					break;
				case TAG_BB :
					g_iUserCount[ USERCNT_BB ] ++;
					break;
				case TAG_EX :
					g_iUserCount[ USERCNT_EX ] ++;
					break;
				case TAG_GL :
					g_iUserCount[ USERCNT_GL ] ++;
					break;
//-------------------------------------
// 2007.03.12/김대성/추가 - 일본 웹 ActiveX 연동
				case TAG_LV :
					g_iUserCount[ USERCNT_LV ] ++;
					break;
				case TAG_FJ :
					g_iUserCount[ USERCNT_FJ ] ++;
					break;
//-------------------------------------
			}
		}
	}

	CLIB_GameSRV::ExeAPI()->SetUserCNT( iUserCNT );
}
void DecUserCNT( int iUserCNT, classUSER *pUSER )
{
	if ( IsJAPAN() ) {
		char *szAccount = pUSER->Get_ACCOUNT();
		if ( pUSER->Get_AccountLEN() >= 4 ) {
			switch( *( (WORD *)(&szAccount[ pUSER->Get_AccountLEN()-3 ]) ) ) {
				case TAG_HG :
					g_iUserCount[ USERCNT_HG ] --;
					break;
				case TAG_BB :
					g_iUserCount[ USERCNT_BB ] --;
					break;
				case TAG_EX :
					g_iUserCount[ USERCNT_EX ] --;
					break;
				case TAG_GL :
					g_iUserCount[ USERCNT_GL ] --;
					break;
//-------------------------------------
// 2007.03.12/김대성/추가 - 일본 웹 ActiveX 연동
				case TAG_LV :
					g_iUserCount[ USERCNT_LV ] --;
					break;
				case TAG_FJ :
					g_iUserCount[ USERCNT_FJ ] --;
					break;
//-------------------------------------
			}
		}
	}

	CLIB_GameSRV::ExeAPI()->SetUserCNT( iUserCNT );
}

#include "../SHO_GS.ver"
DWORD GetServerBuildNO ()
{
	return BUILD_NUM;
}
DWORD g_dwStartTIME=0;
DWORD GetServerStartTIME ()
{
	return g_dwStartTIME;
}

//-------------------------------------------------------------------------------------------------
/*
bool LOG_SocketMSG(WPARAM wParam, LPARAM lParam)
{
#ifdef	__CDB_SOCKET
	return g_pSockLOG->WndPROC( wParam, lParam );
#else
	return true;
#endif
}
bool LSV_SocketMSG(WPARAM wParam, LPARAM lParam)
{
	return g_pSockLSV->Proc_SocketMSG( wParam, lParam );
}
*/

//-------------------------------------------------------------------------------------------------
#define	WM_LSVSOCK_MSG	( WM_SOCKETWND_MSG+0 )
#define	WM_ASVSOCK_MSG	( WM_SOCKETWND_MSG+1 )
//#define	WM_LOGSOCK_MSG	( WM_SOCKETWND_MSG+1 )

#define	USE_MY_SQL_AGENT		0


CLIB_GameSRV::CLIB_GameSRV ( EXE_GS_API *pExeAPI )
{
//	_set_security_error_handler( ReportFailure );

//	int sss = sizeof( tagGrowAbility );
#ifdef		USE_BUG_GUARD
	SetExceptionReport( GetServerBuildNO() , "SHO_GS_DLL", "SHO_GS_DLL.err" );
#endif

#if ( _WIN32_WINNT < 0x0500 ) || !defined( __SERVER )
	COMPILE_TIME_ASSERT( 0 );
#endif


#ifndef	__INC_WORLD
	COMPILE_TIME_ASSERT( MAX_ZONE_USER_BUFF > 4096 );
#endif

#ifdef	__MILEAGE_SKILL_USED
	COMPILE_TIME_ASSERT( sizeof(gsv_SELECT_CHAR) < 1424);
#else
	COMPILE_TIME_ASSERT( sizeof(gsv_SELECT_CHAR) < 1024);
#endif
	COMPILE_TIME_ASSERT( sizeof(tagGrowAbility) < 384 );
	COMPILE_TIME_ASSERT( (sizeof(__int64) + sizeof(tagITEM)*INVENTORY_TOTAL_SIZE) == sizeof(CInventory) );




	//// (20060831:남병철) : 디버깅용
	//g_WatchData.MoveZLogWIndow( "WATCH DATA", 0, 0, 900, 500 );
	//g_WatchData.MoveZLogWIndow( "WATCH DATA", 1280, 500, 900, 500 );

#ifdef __USE_ZLOG__
	g_WatchData.MoveZLogWIndow( "WATCH DATA", 1250, 0, 900, 1000 );
	g_WatchData.ShowZLogWindow( TRUE );
#endif

	//g_GSUser.MoveZLogWIndow( "GS_USER.cpp 프로토콜", 1580, 0, 300, 500 );
	//g_GSUser.ShowZLogWindow( TRUE );
	//g_ObjCHAR.MoveZLogWIndow( "CObjCHAR.cpp 프로토콜", 1880, 0, 300, 500 );
	//g_ObjCHAR.ShowZLogWindow( TRUE );
	//g_ObjNPC.MoveZLogWIndow( "CObjNPC.cpp 프로토콜", 2180, 0, 300, 500 );
	//g_ObjNPC.ShowZLogWindow( TRUE );
	//g_RegenAREA.MoveZLogWIndow( "CRegenAREA.cpp 프로토콜", 1280, 0, 300, 500 );
	//g_RegenAREA.ShowZLogWindow( TRUE );



	m_pExeAPI = pExeAPI; 
}

CLIB_GameSRV::~CLIB_GameSRV ()
{
#if defined(__N_PROTECT) 
	CleanupGameguardAuth();
#endif

	Shutdown ();

	if ( g_pThreadSQL ) {
		g_pThreadSQL->Free ();
		SAFE_DELETE( g_pThreadSQL );
	}
	if ( g_pThreadLOG ) {	// 항상 g_pThreadSQL보다 뒤에...
		g_pThreadLOG->Free ();
		SAFE_DELETE( g_pThreadLOG );
	}

#ifdef	__INC_WORLD
	if ( g_pChatROOMs ) {
		SAFE_DELETE( g_pChatROOMs );
	}

	if ( g_pThreadMSGR ) {
		g_pThreadMSGR->Free ();
		SAFE_DELETE( g_pThreadMSGR );
	}
#endif
	if ( g_pThreadGUILD ) {
		g_pThreadGUILD->Free ();
		SAFE_DELETE( g_pThreadGUILD );
	}
	if ( g_pThreadMALL ) {
		g_pThreadMALL->Free ();
		SAFE_DELETE( g_pThreadMALL );
	}

	DisconnectFromLSV ();
	DisconnectFromLOG ();

	SAFE_DELETE( g_pUserLIST );
	SAFE_DELETE( g_pPartyBUFF );

	m_pInstance = NULL;

	SAFE_DELETE( g_pSockLSV );
	SAFE_DELETE( g_pSockASV );
//	SAFE_DELETE( g_pSockLOG );

	Free_BasicDATA ();
	g_pCharDATA->Destroy ();

	CPoolRECVIO::Destroy ();
	CPoolSENDIO::Destroy ();
	CPoolPACKET::Destroy ();

	SAFE_DELETE( g_pPacketCODEC );
    CStr::Free ();

	if ( CSocketWND::GetInstance() )
		CSocketWND::GetInstance()->Destroy();

	SAFE_DELETE_ARRAY( m_pCheckedLocalZONE );

	CSOCKET::Free ();
	// Cleaning up
#ifdef	__USE_TRACE
	TRACE_FREE ();
#endif
}

void CLIB_GameSRV::SystemINIT( HINSTANCE hInstance, char *szBaseDataDIR, int iLangType )
{
	//int iSize;
	//
	//iSize = sizeof( CObjITEM );
	//iSize = sizeof( CObjMOB  );
	//iSize = sizeof( CObjSUMMON );
	//iSize = sizeof( tagPartyUSER );

#if defined(__N_PROTECT)
	char	szFullPath[ MAX_PATH ];
	::GetModuleFileName( hInstance, szFullPath, MAX_PATH );

	char	szPathOnly[ MAX_PATH ];
	::ZeroMemory( szPathOnly, MAX_PATH );
	CUtil::ExtractFilePath( szFullPath, szPathOnly, MAX_PATH );
	DWORD dwGGErrCode = InitGameguardAuth( szPathOnly, 50 );
	if ( ERROR_SUCCESS != dwGGErrCode ) 
	{
		LogString( 0xffff, "n-Protect init error \n" );
		return;
	}
#endif


	// Initializes the symbol files
#ifdef	__USE_TRACE
	TRACE_INIT ( NULL );
#endif
	m_pInstance = this;

	if ( iLangType < 0 )
		iLangType = 0;

	m_bTestServer = true;
	m_iLangTYPE = iLangType;
	m_pCheckedLocalZONE = NULL;
	m_pWorldTIMER = NULL;
#ifdef __CLAN_FIELD_SET
	m_pClanFieldTIMER = NULL;
#endif
#ifdef __CLAN_WAR_SET
	m_pClanWarTIMER = NULL;
#endif

//-------------------------------------
// 2006.12.06/김대성/추가 - 경험치 X배 치트키 시간대별 자동 실행
#ifdef __CHEAT_SET_WORLDEXP
	m_pWorldExpTIMER = NULL;
#endif
//-------------------------------------
//-------------------------------------
// 2007.02.14/김대성/추가 - 기간제 아이템
#ifdef __ITEM_TIME_LIMIT
	m_pItemTimeLimitCheckTIMER = NULL;
#endif
//-------------------------------------

	CSOCKET::Init ();
	CStr::Init ();

	m_BaseDataDIR.Alloc( (WORD)( strlen(szBaseDataDIR) + 3 ) );
	m_BaseDataDIR.Printf("%s\\", szBaseDataDIR );

	g_pPacketCODEC = new CPacketCODEC;

	// 주의 !!!! Pool을 사용 안하더라도 초기화 해야 한다 !!!
	CPoolPACKET::Instance (DEF_PACKET_POOL_SIZE,	INC_PACKET_POOL_SIZE );
	CPoolRECVIO::Instance (DEF_RECV_IO_POOL_SIZE,	INC_RECV_IO_POOL_SIZE);
	CPoolSENDIO::Instance (DEF_SEND_IO_POOL_SIZE,	INC_SEND_IO_POOL_SIZE);

	g_pCharDATA = CCharDatLIST::Instance ();

#ifdef	__GEN_TEXT
	g_fpTXT = fopen( "Lang_OUT.txt", "w" );
#endif


	// STB 데이타
	Load_BasicDATA ();


#ifdef	__GEN_TEXT
	fclose( g_fpTXT );
#endif

	::ZeroMemory( g_iUserCount, sizeof(int)*USERCNT_MAX );

	g_pSockLSV = new GS_lsvSOCKET;
	g_pSockASV = new GS_asvSOCKET;

//	g_pSockLOG = new GS_logSOCKET( USE_MY_SQL_AGENT );

	g_pPartyBUFF = new CPartyBUFF( MAX_PARTY_BUFF );
	g_pUserLIST  = new CUserLIST ( DEF_GAME_USER_POOL_SIZE,  INC_GAME_USER_POOL_SIZE  );

	// Lsv, Log = 2
	CSocketWND *pSockWND = CSocketWND::InitInstance( hInstance, 2 );
	if ( pSockWND ) {
		pSockWND->AddSocket( &g_pSockLSV->m_SockLSV, WM_LSVSOCK_MSG );
		pSockWND->AddSocket( &g_pSockASV->m_SockASV, WM_ASVSOCK_MSG );
	#ifdef	__CDB_SOCKET
		pSockWND->AddSocket( g_pSockLOG, WM_LOGSOCK_MSG );
	#endif
	}

	this->InitLocalZone( true );

	if( IsTAIWAN() )
	{
		if( !g_ChatLOG.Open( "USER" ) )
			g_LOG.CS_OUT( 0xffff, "User Chat Log file can't be opened" );
		else
			g_ChatLOG.Resume();
		if( !g_ChatGMLOG.Open( "GM" ) )
			g_LOG.CS_OUT( 0xffff, "GM Chat Log file can't be opened" );
		else
			g_ChatGMLOG.Resume();
	}

//-------------------------------------
// 2006.09.28/김대성/추가 - 영구히 삭제해야하는 아이템 목록
	read_LIST_DELITEM_STB();
//-------------------------------------

//-------------------------------------
// 2007.01.10/김대성/추가 - 서버다운된 시간 리스트
#ifdef __ITEM_TIME_LIMIT
	read_ServerDownTime();
#endif
//-------------------------------------
}

//-------------------------------------
// 2006.09.28/김대성/추가 - 영구히 삭제해야하는 아이템 목록
// 영구히 삭제해야하는 아이템 목록 읽어들이기
std::map<int, int> g_mapDeleteItemList;
#include <classSTB.h>
int CLIB_GameSRV::read_LIST_DELITEM_STB()
{
	classSTB stb;
	int nItemNum;
	int i;
	char szFileName[1024];

	sprintf(szFileName, "%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_DELITEM.STB");

	g_mapDeleteItemList.clear();

	if ( stb.Open (szFileName) ) 
	{
		for(i=1; i<stb.GetRowCount() ; ++i)
		{
			nItemNum = stb.GetInteger(1, i);					// 1: 레벨
			if(nItemNum > 0)
				g_mapDeleteItemList[nItemNum] = nItemNum;
		}

		stb.Close ();
	}
	else
	{
//		printf("LIST_STATUS.stb file open fail\n");
		return -1;
	}

	return 0;
}
//-------------------------------------

//-------------------------------------
// 2007.01.10/김대성/추가 - 서버다운된 시간 리스트
// 서버다운된 시간만큼 기간제 아이템의 사용시간을 보상해 주기위해서...
#ifdef __ITEM_TIME_LIMIT
std::map<int, _DOWN_TIME> g_mapServerDownTime;
int CLIB_GameSRV::read_ServerDownTime()
{
	struct _A
	{
		char Year[5]; char Month[3]; char Day[3]; char Hour[3]; char Min[3]; char Sec[3];
		int nYear, nMonth, nDay, nHour, nMin, nSec;

		void Set()	// 2006.12.06 15:46:00
		{
			Year[4] = '\0'; Month[2] = '\0'; Day[2] = '\0'; Hour[2] = '\0'; Min[2] = '\0'; Sec[2] = '\0';

			nYear = atoi(Year); nMonth = atoi(Month); nDay = atoi(Day);
			nHour = atoi(Hour);	nMin = atoi(Min); nSec = atoi(Sec);
		};
	};

	_A Bt, Et;
	char szValue[100];
	int nValue;
	char buf[1024];

	g_mapServerDownTime.clear();

	for(int i=1; i<1000; i++)
	{
		sprintf(buf, "Begin%d", i);
		GetPrivateProfileString("ServerDown", buf, "", (char *)&Bt, sizeof(Bt), "./Server_Down_Time.ini");
		sprintf(buf, "End%d", i);
		GetPrivateProfileString("ServerDown", buf, "", (char *)&Et, sizeof(Et), "./Server_Down_Time.ini");
		sprintf(buf, "Value%d", i);
		GetPrivateProfileString("ServerDown", buf, "", szValue, sizeof(szValue), "./Server_Down_Time.ini");

		if(strcmp((char *)&Bt, "") == 0)
			break;

		Bt.Set(); Et.Set(); nValue = atoi(szValue);

		time_t bt, et;
		struct tm t;

		// 시작시간
		t.tm_year = Bt.nYear - 1900; t.tm_mon = Bt.nMonth - 1; t.tm_mday = Bt.nDay;
		t.tm_hour = Bt.nHour; t.tm_min = Bt.nMin; t.tm_sec = Bt.nSec;
		bt = mktime( &t);	

		// 끝시간
		t.tm_year = Et.nYear - 1900; t.tm_mon = Et.nMonth - 1; t.tm_mday = Et.nDay;
		t.tm_hour = Et.nHour; t.tm_min = Et.nMin; t.tm_sec = Et.nSec;
		et = mktime( &t);	

		if(bt >= et)	// 시작시간이 끝시간보다 크면 시간설정이 잘못됐다.
			continue;

		_DOWN_TIME dt;
		dt.down_start_t = bt;
		dt.down_end_t = et;
		dt.nValue = nValue;

		g_mapServerDownTime[i] = dt;
	}

	return 0;
}
#endif
//-------------------------------------

//-------------------------------------------------------------------------------------------------
/*
bool CLIB_GameSRV::CheckSTB_AllITEM ()
{
	short nT, nI, nProductIDX;
	for (nT=ITEM_TYPE_FACE_ITEM; nT<=ITEM_TYPE_RIDE_PART; nT++) {
		for (nI=0; nI<g_pTblSTBs[nT]->m_nDataCnt; nI++) {
			nProductIDX = ITEM_PRODUCT_IDX(nT,nI);
			if ( 0 == nProductIDX )
				continue;

			if ( 0 == PRODUCT_NEED_ITEM_NO( nProductIDX, 0 ) ) {
				// 재료 입력 필요...
				ITEM_PRODUCT_IDX(nT,nI) = 0;
			}
		}
	}
}
*/

bool CLIB_GameSRV::CheckSTB_UseITEM ()
{
	for (short nD=0; nD<g_TblUSEITEM.m_nDataCnt; nD++) {
		if ( USEITEM_COOLTIME_TYPE(nD) < 0 )
			SET_USEITEM_COOLTIME_TYPE(nD, 0);
		else
		if ( USEITEM_COOLTIME_TYPE(nD) >= MAX_USEITEM_COOLTIME_TYPE )
			SET_USEITEM_COOLTIME_TYPE(nD, 0);
		if ( USEITEM_COOLTIME_DELAY(nD) < 0 )
			SET_USEITEM_COOLTIME_DELAY(nD, 0);

		if ( 0 == USEITME_STATUS_STB( nD ) )
			continue;
		
		short nIngSTB = USEITME_STATUS_STB( nD );
		short nDuringTime;

		for (short nI=0; nI<STATE_APPLY_ABILITY_CNT; nI++) {
			if ( !STATE_APPLY_ABILITY_VALUE( nIngSTB, nI ) )
				continue;
			
			assert( 0 != STATE_APPLY_ABILITY_VALUE( nIngSTB, nI ) );

			// 총 증가할 수치.
			nDuringTime = USEITEM_ADD_DATA_VALUE( nD ) / STATE_APPLY_ABILITY_VALUE( nIngSTB, nI );
		}
	}

	return true;
}
bool CLIB_GameSRV::CheckSTB_NPC ()
{
	t_HASHKEY HashKey;
	CQuestTRIGGER *pQuestTrigger;

	int iDropTYPE;
	for (short nI=0; nI<g_TblNPC.m_nDataCnt; nI++) {
		if ( NPC_LEVEL( nI ) < 1 ) SET_NPC_LEVEL( nI, 1 );
		if ( NPC_ATK_SPEED( nI ) <= 0 )	SET_NPC_ATK_SPEED( nI, 100 );
		if ( NPC_DROP_TYPE( nI ) >= g_TblDropITEM.m_nDataCnt ) {
			iDropTYPE = NPC_DROP_TYPE( nI );
			SET_NPC_DROP_TYPE( nI, 0 );

			assert( NPC_DROP_TYPE( nI ) < g_TblDropITEM.m_nDataCnt );
		}

		if ( nI && NPC_DEAD_EVENT( nI ) ) {
			HashKey = ::StrToHashKey( NPC_DEAD_EVENT( nI ) );
			pQuestTrigger = g_QuestList.GetQuest( HashKey );
			if ( !pQuestTrigger ) {
				SET_NPC_DEAD_EVENT(nI,NULL);
			} else {
				do {
					pQuestTrigger->m_iOwerNpcIDX = nI;	// 죽을때 발생되는 트리거다.
#ifdef	__INC_WORLD
					g_LOG.CS_ODS( 0xffff, " ** Check next:%d, Set Trigger \"%s\" Owner to NPC %d \n", 
							pQuestTrigger->GetCheckNext(), pQuestTrigger->m_Name.Get(), nI );
#endif
					pQuestTrigger = pQuestTrigger->m_pNextTrigger;
				} while( pQuestTrigger );
			}
		}
	}
	return true;
}

bool CLIB_GameSRV::CheckSTB_DropITEM ()
{
//	/*
	//아이템 드롭 계산방식이 바뀌면서 stb참조 값이 틀려졌다.
	int iDropITEM;
	tagITEM sITEM;

	for (short nI=1; nI<g_TblDropITEM.m_nDataCnt; nI++) {
		for (short nC=1; nC<g_TblDropITEM.m_nColCnt; nC++) {
			iDropITEM = DROPITEM_ITEMNO( nI, nC );
			if ( iDropITEM <= 0 )
				continue;

//-------------------------------------
/*
			sITEM.m_cType   = (BYTE) ( iDropITEM / 1000 );
			sITEM.m_nItemNo = iDropITEM % 1000;

			if ( (sITEM.m_cType <  ITEM_TYPE_FACE_ITEM ||
				 sITEM.m_cType >  ITEM_TYPE_RIDE_PART ||
				 sITEM.m_cType == ITEM_TYPE_QUEST) && iDropITEM > 1000 ) 
			{
				// 아이템 타입 오류...
				#ifdef	__INC_WORLD
				if ( nI ) {
					char *pMsg = CStr::Printf ("%d 라인 %d 컬럼 입력값: %d", nI, nC, DROPITEM_ITEMNO( nI, nC ) );
						 ::MessageBox( NULL, pMsg, "드롭아이템 stb 데이타 입력 오류", MB_OK );
				}
				#endif

				SET_DROPITEM_ITEMNO( nI, nC , 0);
				continue;
			}

			if( iDropITEM <= 1000 ) {
				if ( iDropITEM >= 1 && iDropITEM <= 4 ) {
					// 다시 계산
					int iDropTblIDX = 26 + (iDropITEM * 5) + 4 // RANDOM(5)의 최대값 4 ;
					if ( iDropTblIDX >= g_TblDropITEM.m_nColCnt ) {
						// 테이블 컬럼 갯수 초과...
						g_LOG.CS_ODS ( 0xffff, "This drop item[ %d %d ] may be too big\n", nI, nC);
					}
					continue;
				} 
				SET_DROPITEM_ITEMNO( nI, nC , 0);
				continue;
			}
			
			
			if ( sITEM.m_nItemNo > g_pTblSTBs[ sITEM.m_cType ]->m_nDataCnt ) {
				// 아이템 번호 오류...
			#ifdef	__INC_WORLD
				char *pMsg = CStr::Printf ("%d 라인 %d 컬럼 입력값: %d", nI, nC, DROPITEM_ITEMNO( nI, nC ) );
				::MessageBox( NULL, pMsg, "드롭아이템 stb 데이타 입력 오류", MB_OK );
			#endif
				SET_DROPITEM_ITEMNO( nI, nC, 0);
				continue;
			}

			assert( DROPITEM_ITEMNO( nI, nC ) > 1000 );
*/
// 2006.07.19/김대성/수정 - 아이템 999제한 확장
			if(iDropITEM < 1000000)
			{
				sITEM.m_cType   = (BYTE) ( iDropITEM / 1000 );
				sITEM.m_nItemNo = iDropITEM % 1000;

				if ( (sITEM.m_cType <  ITEM_TYPE_FACE_ITEM ||
					sITEM.m_cType >  ITEM_TYPE_RIDE_PART ||
					sITEM.m_cType == ITEM_TYPE_QUEST) && iDropITEM > 1000 ) 
				{
					// 아이템 타입 오류...
#ifdef	__INC_WORLD
					if ( nI ) {
						char *pMsg = CStr::Printf ("%d 라인 %d 컬럼 입력값: %d", nI, nC, DROPITEM_ITEMNO( nI, nC ) );
						::MessageBox( NULL, pMsg, "드롭아이템 stb 데이타 입력 오류", MB_OK );
					}
#endif

					SET_DROPITEM_ITEMNO( nI, nC , 0);
					continue;
				}

				if( iDropITEM <= 1000 ) {
					if ( iDropITEM >= 1 && iDropITEM <= 4 ) {
						// 다시 계산
						int iDropTblIDX = 26 + (iDropITEM * 5) + 4/*RANDOM(5)의 최대값 4 */;
						if ( iDropTblIDX >= g_TblDropITEM.m_nColCnt ) {
							// 테이블 컬럼 갯수 초과...
							g_LOG.CS_ODS ( 0xffff, "This drop item[ %d %d ] may be too big\n", nI, nC);
						}
						continue;
					} 
					SET_DROPITEM_ITEMNO( nI, nC , 0);
					continue;
				}


				if ( sITEM.m_nItemNo > g_pTblSTBs[ sITEM.m_cType ]->m_nDataCnt ) {
					// 아이템 번호 오류...
#ifdef	__INC_WORLD
					char *pMsg = CStr::Printf ("%d 라인 %d 컬럼 입력값: %d", nI, nC, DROPITEM_ITEMNO( nI, nC ) );
					::MessageBox( NULL, pMsg, "드롭아이템 stb 데이타 입력 오류", MB_OK );
#endif
					SET_DROPITEM_ITEMNO( nI, nC, 0);
					continue;
				}

				assert( DROPITEM_ITEMNO( nI, nC ) > 1000 );

			}
			else	// if(iDropITEM < 1000000)
			{
				sITEM.m_cType   = (BYTE) ( iDropITEM / 1000000 );
				sITEM.m_nItemNo = iDropITEM % 1000000;

				if ( (sITEM.m_cType <  ITEM_TYPE_FACE_ITEM ||
					sITEM.m_cType >  ITEM_TYPE_RIDE_PART ||
					sITEM.m_cType == ITEM_TYPE_QUEST) && iDropITEM > 1000000 ) 
				{
					// 아이템 타입 오류...
#ifdef	__INC_WORLD
					if ( nI ) {
						char *pMsg = CStr::Printf ("%d 라인 %d 컬럼 입력값: %d", nI, nC, DROPITEM_ITEMNO( nI, nC ) );
						::MessageBox( NULL, pMsg, "드롭아이템 stb 데이타 입력 오류", MB_OK );
					}
#endif

					SET_DROPITEM_ITEMNO( nI, nC , 0);
					continue;
				}

				if( iDropITEM <= 1000000 ) {
					if ( iDropITEM >= 1 && iDropITEM <= 4 ) {
						// 다시 계산
						int iDropTblIDX = 26 + (iDropITEM * 5) + 4/*RANDOM(5)의 최대값 4 */;
						if ( iDropTblIDX >= g_TblDropITEM.m_nColCnt ) {
							// 테이블 컬럼 갯수 초과...
							g_LOG.CS_ODS ( 0xffff, "This drop item[ %d %d ] may be too big\n", nI, nC);
						}
						continue;
					} 
					SET_DROPITEM_ITEMNO( nI, nC , 0);
					continue;
				}


				if ( sITEM.m_nItemNo > g_pTblSTBs[ sITEM.m_cType ]->m_nDataCnt ) {
					// 아이템 번호 오류...
#ifdef	__INC_WORLD
					char *pMsg = CStr::Printf ("%d 라인 %d 컬럼 입력값: %d", nI, nC, DROPITEM_ITEMNO( nI, nC ) );
					::MessageBox( NULL, pMsg, "드롭아이템 stb 데이타 입력 오류", MB_OK );
#endif
					SET_DROPITEM_ITEMNO( nI, nC, 0);
					continue;
				}

				assert( DROPITEM_ITEMNO( nI, nC ) > 1000000 );
			}
//-------------------------------------
		}
	}

	return true;
}

bool CLIB_GameSRV::CheckSTB_ReNewalDropITEM ()
{
	//	/*
	//아이템 드롭 계산방식이 바뀌면서 stb참조 값이 틀려졌다.
	int iDropITEM;
	tagITEM sITEM;

	for (short nI=1; nI<g_TblDropITEM.m_nDataCnt; nI++) {
		for (short nC=1; nC<20; nC++) {
			iDropITEM = DROP_ITEM_NO( nI, nC );
			if ( iDropITEM <= 0 )
				continue;

//-------------------------------------
/*
			sITEM.m_cType   = (BYTE) ( iDropITEM / 1000 );
			sITEM.m_nItemNo = iDropITEM % 1000;

			if ( (sITEM.m_cType <  ITEM_TYPE_FACE_ITEM ||
				sITEM.m_cType >  ITEM_TYPE_RIDE_PART ||
				sITEM.m_cType == ITEM_TYPE_QUEST) && iDropITEM > 1000 ) 
			{
				// 아이템 타입 오류...
#ifdef	__INC_WORLD
				if ( nI ) {
					char *pMsg = CStr::Printf ("%d 라인 %d 컬럼 입력값: %d", nI, nC, DROP_ITEM_NO( nI, nC ) );
					::MessageBox( NULL, pMsg, "드롭아이템 stb 데이타 입력 오류", MB_OK );
				}
#endif

				SET_DROP_ITEM_NO( nI, nC , 0);
				continue;
			}

			if( iDropITEM <= 1000 ) {
				if ( iDropITEM >= 1 && iDropITEM <= 4 ) {
					// 다시 계산
					int iDropTblIDX = 26 + (iDropITEM * 5) + 4 //RANDOM(5)의 최대값 4 ;
					if ( iDropTblIDX >= g_TblDropITEM.m_nColCnt ) {
						// 테이블 컬럼 갯수 초과...
						g_LOG.CS_ODS ( 0xffff, "This drop item[ %d %d ] may be too big\n", nI, nC);
					}
					continue;
				} 
				SET_DROP_ITEM_NO( nI, nC , 0);
				continue;
			}


			if ( sITEM.m_nItemNo > g_pTblSTBs[ sITEM.m_cType ]->m_nDataCnt ) {
				// 아이템 번호 오류...
#ifdef	__INC_WORLD
				char *pMsg = CStr::Printf ("%d 라인 %d 컬럼 입력값: %d", nI, nC, DROP_ITEM_NO( nI, nC ) );
				::MessageBox( NULL, pMsg, "드롭아이템 stb 데이타 입력 오류", MB_OK );
#endif
				SET_DROP_ITEM_NO( nI, nC ,0);
				continue;
			}

			assert( DROP_ITEM_NO( nI, nC ) > 1000 );
*/
// 2006.07.19/김대성/수정 - 아이템 999 제한 확장
			if(iDropITEM < 1000000)
			{
				sITEM.m_cType   = (BYTE) ( iDropITEM / 1000 );
				sITEM.m_nItemNo = iDropITEM % 1000;

				if ( (sITEM.m_cType <  ITEM_TYPE_FACE_ITEM ||
					sITEM.m_cType >  ITEM_TYPE_RIDE_PART ||
					sITEM.m_cType == ITEM_TYPE_QUEST) && iDropITEM > 1000 ) 
				{
					// 아이템 타입 오류...
#ifdef	__INC_WORLD
					if ( nI ) {
						char *pMsg = CStr::Printf ("%d 라인 %d 컬럼 입력값: %d", nI, nC, DROP_ITEM_NO( nI, nC ) );
						::MessageBox( NULL, pMsg, "드롭아이템 stb 데이타 입력 오류", MB_OK );
					}
#endif

					SET_DROP_ITEM_NO( nI, nC , 0);
					continue;
				}

				if( iDropITEM <= 1000 ) {
					if ( iDropITEM >= 1 && iDropITEM <= 4 ) {
						// 다시 계산
						int iDropTblIDX = 26 + (iDropITEM * 5) + 4/*RANDOM(5)의 최대값 4 */;
						if ( iDropTblIDX >= g_TblDropITEM.m_nColCnt ) {
							// 테이블 컬럼 갯수 초과...
							g_LOG.CS_ODS ( 0xffff, "This drop item[ %d %d ] may be too big\n", nI, nC);
						}
						continue;
					} 
					SET_DROP_ITEM_NO( nI, nC , 0);
					continue;
				}


				if ( sITEM.m_nItemNo > g_pTblSTBs[ sITEM.m_cType ]->m_nDataCnt ) {
					// 아이템 번호 오류...
#ifdef	__INC_WORLD
					char *pMsg = CStr::Printf ("%d 라인 %d 컬럼 입력값: %d", nI, nC, DROP_ITEM_NO( nI, nC ) );
					::MessageBox( NULL, pMsg, "드롭아이템 stb 데이타 입력 오류", MB_OK );
#endif
					SET_DROP_ITEM_NO( nI, nC ,0);
					continue;
				}

				assert( DROP_ITEM_NO( nI, nC ) > 1000 );
			}
			else	// if(iDropITEM < 1000000)
			{
				sITEM.m_cType   = (BYTE) ( iDropITEM / 1000000 );
				sITEM.m_nItemNo = iDropITEM % 1000000;

				if ( (sITEM.m_cType <  ITEM_TYPE_FACE_ITEM ||
					sITEM.m_cType >  ITEM_TYPE_RIDE_PART ||
					sITEM.m_cType == ITEM_TYPE_QUEST) && iDropITEM > 1000000 ) 
				{
					// 아이템 타입 오류...
#ifdef	__INC_WORLD
					if ( nI ) {
						char *pMsg = CStr::Printf ("%d 라인 %d 컬럼 입력값: %d", nI, nC, DROP_ITEM_NO( nI, nC ) );
						::MessageBox( NULL, pMsg, "드롭아이템 stb 데이타 입력 오류", MB_OK );
					}
#endif

					SET_DROP_ITEM_NO( nI, nC , 0);
					continue;
				}

				if( iDropITEM <= 1000000 ) {
					if ( iDropITEM >= 1 && iDropITEM <= 4 ) {
						// 다시 계산
						int iDropTblIDX = 26 + (iDropITEM * 5) + 4/*RANDOM(5)의 최대값 4 */;
						if ( iDropTblIDX >= g_TblDropITEM.m_nColCnt ) {
							// 테이블 컬럼 갯수 초과...
							g_LOG.CS_ODS ( 0xffff, "This drop item[ %d %d ] may be too big\n", nI, nC);
						}
						continue;
					} 
					SET_DROP_ITEM_NO( nI, nC , 0);
					continue;
				}


				if ( sITEM.m_nItemNo > g_pTblSTBs[ sITEM.m_cType ]->m_nDataCnt ) {
					// 아이템 번호 오류...
#ifdef	__INC_WORLD
					char *pMsg = CStr::Printf ("%d 라인 %d 컬럼 입력값: %d", nI, nC, DROP_ITEM_NO( nI, nC ) );
					::MessageBox( NULL, pMsg, "드롭아이템 stb 데이타 입력 오류", MB_OK );
#endif
					SET_DROP_ITEM_NO( nI, nC ,0);
					continue;
				}

				assert( DROP_ITEM_NO( nI, nC ) > 1000000 );
			}
//-------------------------------------
		}
	}

	return true;
}

bool CLIB_GameSRV::CheckSTB_ItemRateTYPE ()
{
	short nD, nRateTYPE;

	for (nD=0; nD<g_TblUSEITEM.m_nDataCnt; nD++) {
		nRateTYPE = ITEM_RATE_TYPE( ITEM_TYPE_USE, nD );
		assert( nRateTYPE >= 0 && nRateTYPE < MAX_PRICE_TYPE );
	}

	for (nD=0; nD<g_TblNATUAL.m_nDataCnt; nD++) {
		nRateTYPE = ITEM_RATE_TYPE( ITEM_TYPE_NATURAL, nD );
		assert( nRateTYPE >= 0 && nRateTYPE < MAX_PRICE_TYPE );
	}
	return true;
}
bool CLIB_GameSRV::CheckSTB_Motion ()
{
	short nX, nY, nFileIDX;

	// type_motion.stb
	for (nY=0; nY<g_TblAniTYPE.m_nDataCnt; nY++) {
		for (nX=0; nX<g_TblAniTYPE.m_nColCnt; nX++) {
			nFileIDX = FILE_MOTION( nX, nY );
			if ( !nFileIDX )
				continue;

			if ( !g_MotionFILE.IDX_GetMOTION( nFileIDX, 0 ) ) {
			#ifdef	__INC_WORLD
				char *pMsg = CStr::Printf ("File_motion.stb %d 라인 모션 필요, Type_motion.stb( %d, %d )", 
					nFileIDX, nX, nY);
				::MessageBox( NULL, pMsg, "모션 입력 오류", MB_OK );
			#endif
			}
		}
	}
	return true;
}

bool CLIB_GameSRV::CheckSTB_GemITEM ()
{
	short nType, nValue;

	for (short nC=0; nC<g_TblGEMITEM.m_nDataCnt; nC++) {
		for (short nI=0; nI<2; nI++) {
			nType  = GEMITEM_ADD_DATA_TYPE ( nC, nI );
			nValue = GEMITEM_ADD_DATA_VALUE( nC, nI );

			_ASSERT( nType >= 0 && nType <= AT_MAX );
		}
	}
	return true;
}

bool CLIB_GameSRV::CheckSTB_ListPRODUCT ()
{
	tagITEM sOutITEM;
	for (short nI=0; nI<g_TblPRODUCT.m_nDataCnt; nI++) {
		for (short nS=0; nS<4; nS++ ) {
			if ( PRODUCT_NEED_ITEM_NO( nI, nS) ) {
				sOutITEM.Init( PRODUCT_NEED_ITEM_NO( nI, nS), 1 );
				if ( !sOutITEM.IsValidITEM() ) {
					_ASSERT( 0 );
				}
				// 재료 아이템 번호
				_ASSERT( PRODUCT_NEED_ITEM_CNT( nI, nS) > 0 );	// 팰요 갯수
			}
		}
	}

	return true;
}
//-------------------------------------------------------------------------------------------------
void CLIB_GameSRV::TranslateNameWithDescKey( STBDATA *pOri, char *szStbFile, int iLangCol )
{
#ifdef	__GEN_TEXT
	fprintf( g_fpTXT, "\n\n\nLangCol:%d, FILE:%s \n", iLangCol, szStbFile );
#endif

	int iKeyCol = 0;

	STBDATA	tmpTBL;
	// 아이템 이름만 읽음 :: 이름 컬럼은 1, 3, 5 ... 설명 컬럼은 2, 4, 6 ...
	if ( !tmpTBL.LoadWSTB( true, CStr::Printf("%s%s", BASE_DATA_DIR, szStbFile), iKeyCol, iLangCol, -1 ) )
		return;
	STBDATA *pLang = &tmpTBL;

	char *szKey;
	int iIdx;
	for (int iL=0; iL<pOri->m_nDataCnt; iL++ ) {
		szKey = pOri->m_ppDESC[ iL ];
		if ( !szKey )
			continue;

		iIdx = pLang->GetRowIndex( szKey );
		if ( iIdx && pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetStrLEN() ) {
			// LogString(0xffff," %s :: %d : %s/%s\n", szKey, iIdx, pOri->m_ppNAME[ iL ], pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetSTR() );
			//szStr = pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetSTR();
			//while( *szStr ) {
			//	if ( *szStr == '\'' ) 
			//		*szStr = '`';
			//	szStr ++;
			//}

			SAFE_DELETE( pOri->m_ppNAME[ iL ] );
			pOri->m_ppNAME[ iL ] = new char[ pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetStrLEN()+1 ];
			strcpy( pOri->m_ppNAME[ iL ], pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetSTR() );
			// pOri->m_ppNAME[ iL ][ pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetStrLEN() ] = 0;

#ifdef	__GEN_TEXT
			assert( pOri->m_ppNAME[ iL ] );
			fprintf( g_fpTXT, "\t%d:: %s\n", iL, pOri->m_ppNAME[ iL ] );
#endif
		}
	}

	tmpTBL.Free();
}

void CLIB_GameSRV::TranslateNameWithColoumKey ( STBDATA *pOri, char *szStbFile, int iLangCol, int iNameCol, int iDescCol )
{
#ifdef	__GEN_TEXT
	fprintf( g_fpTXT, "\n\n\nLangCol:%d, FILE:%s \n", iLangCol, szStbFile );
#endif

	int iKeyCol = 0;

	STBDATA	tmpTBL;

	// 아이템 이름만 읽음 :: 이름 컬럼은 1, 3, 5 ... 설명 컬럼은 2, 4, 6 ...
	if ( !tmpTBL.LoadWSTB( true, CStr::Printf("%s%s", BASE_DATA_DIR, szStbFile), iKeyCol, iLangCol, -1 ) )
		return;
	STBDATA *pLang = &tmpTBL;

	char *szKey;
	int iIdx;
	for (int iL=1; iL<pOri->m_nDataCnt; iL++ ) {
		szKey = pOri->m_ppVALUE[ iL ][ iDescCol ].GetSTR();
		if ( !szKey )
			continue;

		iIdx = pLang->GetRowIndex( szKey );
		if ( iIdx && pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetStrLEN() ) {
			//szStr = pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetSTR();
			//while( *szStr ) {
			//	if ( *szStr == '\'' )
			//		*szStr = '`';
			//	szStr ++;
			//}
			// LogString(0xffff," %s :: %d:%d : %s/%s\n", szKey, iL, iIdx, pOri->m_ppVALUE[ iL ][ iNameCol ].GetSTR(), pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetSTR() );

			pOri->m_ppVALUE[ iL ][ iNameCol ].SetVALUE( pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetSTR() );

#ifdef	__GEN_TEXT
			fprintf( g_fpTXT, "\t%d:: %s\n", iL, pOri->m_ppVALUE[ iL ][ iNameCol ].GetSTR() );
#endif
		} else 
		if ( iIdx ) {
#ifdef __KCHS_TEST
			LogString( 0xffff, "Language string not found:: %d:%d / %s [ %d / %s ] \n",
				iL, iIdx, pOri->m_ppVALUE[ iL ][ iNameCol ].GetSTR(), 
				pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetStrLEN(),
				pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetSTR() );
#endif
		}
	}

	tmpTBL.Free();
}

//-------------------------------------------------------------------------------------------------

#include "IP_Addr.h"
bool CLIB_GameSRV::Load_BasicDATA ()
{
	//::MessageBox( NULL, "서버로딩! 20051108_0541", "MSG", MB_OK );
	//WORD *ppp=NULL;
	//ppp[ 1000 ] = 9999;

/*
	STBDATA	tbl;
	tbl.LoadWSTB( CStr::Printf("%s%s", BASE_DATA_DIR, "Language\\LIST_ARMS_s.STB"), -1, 1, 2, -1 );
	for (int i=5; i<=11; i++)
		LogString (0xfff, "%d %s / %s \n", i, 
			tbl.m_ppVALUE[i][ 1 ].GetSTR(),
			tbl.m_ppVALUE[i][ 2 ].GetSTR() );
*/
//	return true;
/*
	CIPv4Addr ipADDR;
	STBDATA IPv4Stb;

	IPv4Stb.Load2 ( CStr::Printf("%s%s", BASE_DATA_DIR, "IP_KOREA.STB"), false, false );
	for (int iR=1; iR<IPv4Stb.m_nDataCnt; iR++) {
		ipADDR.Add( 
				IPv4Stb.m_ppVALUE[ iR ][ 0 ].GetSTR(),
				IPv4Stb.m_ppVALUE[ iR ][ 1 ].GetSTR() );
	}


	LogString( 0xfff, "Result: %d \n", ipADDR.Find( "218.55.179.99"  ) );
	LogString( 0xfff, "Result: %d \n", ipADDR.Find( "202.43.214.151" ) );	// yahoo.co.kr
	LogString( 0xfff, "Result: %d \n", ipADDR.Find( "216.193.207.29" ) );	// onrpg.com
	LogString( 0xfff, "Result: %d \n", ipADDR.Find( "208.255.95.52" ) );		// ncsoft.com
	LogString( 0xfff, "Result: %d \n", ipADDR.Find( "211.233.26.242" ) );		// ncsoft.com
	
	LogString( 0xfff, "Result: %d \n", ipADDR.Find( "211.174.48.166" ) );		// ncsoft.com
	LogString( 0xfff, "Result: %d \n", ipADDR.Find( "211.172.247.212" ) );		// ncsoft.com
	LogString( 0xfff, "Result: %d \n", ipADDR.Find( "211.233.26.242" ) );		// ncsoft.com

	IPv4Stb.Free ();
*/
	size_t ttt = sizeof( tagGrowAbility );

	g_LOG.SetLogMODE( 0,			LOG_FILE );
	g_LOG.SetLogMODE( LOG_NORMAL,	LOG_SCR );
	g_LOG.SetLogMODE( LOG_NORMAL,	LOG_DBGOUT );

	if ( !g_AI_LIST.Load( BASE_DATA_DIR, "3DDATA\\STB\\FILE_AI.STB", "3DDATA\\AI\\AI_s.STB", m_iLangTYPE ) )
		return false;

	g_TblHAIR.Load		( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Hair.STB"),		true, false );
	g_TblFACE.Load		( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Face.STB"),		true, false );
	g_TblARMOR.Load		( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Body.STB"),		true, true  );


	g_TblGAUNTLET.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Arms.STB"),		true, true  );
	g_TblBOOTS.Load		( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Foot.STB"),		true, true  );
	g_TblHELMET.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Cap.STB"),			true, true  );

	g_TblWEAPON.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Weapon.STB"),		true, true  );
	g_TblSUBWPN.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_SUBWPN.STB"),		true, true  );
	g_TblEFFECT.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_EFFECT.STB"),		false,false );

#ifdef  __KRRE_ITEMDROP
	g_TblDropITEM.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\ITEM_DROP(new).STB"),		false, false);

#else
	g_TblDropITEM.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\ITEM_DROP.STB"),		false, false);

#endif

#ifdef __KRRE_UPGRADE
	g_TblPRODUCT.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_PRODUCT.STB"),	    true, false );
#else
	g_TblPRODUCT.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_PRODUCT.STB"),	    true, false );
#endif

	

	g_TblNATUAL.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_NATURAL.STB"),		true, true	);
	g_TblFACEITEM.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_FACEITEM.STB"),	true, true	);
	g_TblUSEITEM.Load   ( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_USEITEM.STB"),		true, true	);
	g_TblBACKITEM.Load  ( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_BACK.STB"),		true, true	);
	g_TblGEMITEM.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_JEMITEM.STB"),		true, true	);
	g_TblJEWELITEM.Load ( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_JEWEL.STB"),	    true, true	);
	g_TblQUESTITEM.Load ( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_QUESTITEM.STB"),	true, true  );
	g_TblStore.Load		( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_SELL.STB"),		true, false	);

	g_TblAniTYPE.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\TYPE_MOTION.STB"),		false, false);

	g_TblEVENT.Load2	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_EVENT.STB"),		false, true );

	g_TblWARP.Load2		( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\WARP.STB"),				true,	false);
	g_TblZONE.Load2		( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_ZONE.STB"),		true,	false);
	assert( g_TblZONE.m_nColCnt > 33 );

	g_TblNPC.Load2      ( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_NPC.STB"),			true,	false);
//	assert( g_TblNPC.m_nColCnt > 43 );


	g_TblAVATAR.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\INIT_AVATAR.STB"),		false,	false);
	g_TblSTATE.Load2	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_STATUS.STB"),		true,	false);

	g_TblUnion.Load2	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_UNION.STB"),		false, false );
	g_TblClass.Load2	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_CLASS.STB"),		false, false );

	g_TblItemGRADE.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_GRADE.STB"),		false,	false);
	g_TblSkillPoint.Load( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Skill_P.STB"),		false,	false);
#ifdef	__APPLY_2ND_JOB
	g_TblATTR.Load		( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_ATTRIBUTE.STB"),	false,	false);
#endif

#ifdef __KRRE_UPGRADE
	g_TblUPGRADE.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_UPGRADE.STB"),		false,	false);
#endif

#ifdef __KRRE_BREAK       //한국리뉴얼 아이템 분해
	g_TblBreak.Load(CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_BREAK.STB"),	    true, false );
#endif



	if ( !g_MotionFILE.Load ( "3DDATA\\STB\\FILE_MOTION.stb", 0, BASE_DATA_DIR ) )
		return false;

	if ( !g_QuestList.LoadQuestTable( "3DDATA\\STB\\LIST_Quest.STB", 
			"3DDATA\\STB\\LIST_QuestDATA.STB", BASE_DATA_DIR, "3DDATA\\QuestData\\Quest_s.STB", m_iLangTYPE ) )
		return false;

	if ( !g_SkillList.LoadSkillTable( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_SKILL.STB") ) )
		return false;

	if ( !g_pCharDATA->Load_MOBorNPC( BASE_DATA_DIR, "3DDATA\\NPC\\LIST_NPC.CHR") ) {
		assert( 0 && "LIST_NPC.chr loading error" );
		return false;
	}

	if ( !g_PatITEM.LoadPatITEM( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_PAT.STB") ) )
		return false;

	g_pTblSTBs[ ITEM_TYPE_FACE_ITEM ] = &g_TblFACEITEM;
	g_pTblSTBs[ ITEM_TYPE_HELMET	] = &g_TblHELMET;
	g_pTblSTBs[ ITEM_TYPE_ARMOR		] =	&g_TblARMOR;
	g_pTblSTBs[ ITEM_TYPE_GAUNTLET	] = &g_TblGAUNTLET;
	g_pTblSTBs[ ITEM_TYPE_BOOTS		] = &g_TblBOOTS;
	g_pTblSTBs[ ITEM_TYPE_KNAPSACK	] = &g_TblBACKITEM;
	g_pTblSTBs[ ITEM_TYPE_JEWEL		] = &g_TblJEWELITEM;
	g_pTblSTBs[ ITEM_TYPE_WEAPON	] = &g_TblWEAPON;
	g_pTblSTBs[ ITEM_TYPE_SUBWPN	] = &g_TblSUBWPN;
	g_pTblSTBs[ ITEM_TYPE_USE		] = &g_TblUSEITEM;
	g_pTblSTBs[ ITEM_TYPE_GEM		] = &g_TblGEMITEM;
	g_pTblSTBs[ ITEM_TYPE_NATURAL	] = &g_TblNATUAL;
	g_pTblSTBs[ ITEM_TYPE_QUEST		] = &g_TblQUESTITEM;
	g_pTblSTBs[ ITEM_TYPE_RIDE_PART ] = &g_PatITEM.m_ItemDATA;

	// 아이템 이름만 읽음 :: 이름 컬럼은 1, 3, 5 ... 설명 컬럼은 2, 4, 6 ...
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_FACE_ITEM  ], "Language\\LIST_FACEITEM_s.STB",	1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_HELMET	 ], "Language\\LIST_CAP_s.STB",			1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_ARMOR		 ], "Language\\LIST_BODY_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_GAUNTLET	 ], "Language\\LIST_ARMS_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_BOOTS		 ], "Language\\LIST_FOOT_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_KNAPSACK	 ], "Language\\LIST_BACK_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_JEWEL		 ], "Language\\LIST_JEWEL_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_WEAPON	 ], "Language\\LIST_WEAPON_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_SUBWPN	 ], "Language\\LIST_SUBWPN_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_USE		 ], "Language\\LIST_USEITEM_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_GEM		 ], "Language\\LIST_JEMITEM_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_NATURAL	 ], "Language\\LIST_NATURAL_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_QUEST		 ], "Language\\LIST_QUESTITEM_s.STB",	1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_RIDE_PART  ], "Language\\LIST_PAT_s.STB",			1+2*m_iLangTYPE );

	this->TranslateNameWithDescKey(&g_SkillList.m_SkillDATA,			"Language\\LIST_SKILL_s.stb",		1+2*m_iLangTYPE );

	// 이름 컬럼은 1,2,3 ...
	this->TranslateNameWithColoumKey( &g_TblNPC, "Language\\LIST_NPC_s.STB",	1+m_iLangTYPE, NPC_NAME_STR_COLUMN, NPC_STRING_ID_COLOUM);


	#define	ZONE_NAME_STR_COLUMN	0
	#define	ZONE_KEY_STR_COLOUM		26
//	assert( g_TblZONE.m_nColCnt == 30 );
	this->TranslateNameWithColoumKey( &g_TblZONE,"Language\\LIST_ZONE_s.STB",	1+2*m_iLangTYPE, ZONE_NAME_STR_COLUMN, ZONE_KEY_STR_COLOUM);

/*
	this->TranslateNameWithDescKey( , "Language\\LIST_ZONE_s.stb     
*/
	g_LOG.SetLogMODE( LOG_NORMAL | LOG_VERBOSE | LOG_DEBUG );

//	CheckSTB_AllITEM ();
	CheckSTB_UseITEM ();

#ifdef __KRRE_ITEMDROP
	CheckSTB_ReNewalDropITEM();
#else
	CheckSTB_DropITEM ();
#endif

	CheckSTB_NPC ();

	CheckSTB_ItemRateTYPE ();
	CheckSTB_Motion ();
	CheckSTB_GemITEM ();
	CheckSTB_ListPRODUCT ();

	return true;
}
void CLIB_GameSRV::Free_BasicDATA ()
{
	// STBDATA는 자동 풀림..
	g_PatITEM.Free ();
	g_QuestList.Free ();	
	g_SkillList.Free ();
	g_MotionFILE.Free ();
	g_AI_LIST.Free ();
}

//-------------------------------------------------------------------------------------------------
char* CLIB_GameSRV::GetZoneName( short nZoneNO )
{
	if ( nZoneNO > 0 && nZoneNO < g_TblZONE.m_nDataCnt ) {
		#ifndef	__INC_WORLD
		if ( nZoneNO >= TEST_ZONE_NO ) return NULL;
		#endif

        if ( ZONE_NAME( nZoneNO ) && ZONE_FILE( nZoneNO ) ) {
			char *szZoneFILE = CStr::Printf("%s%s", BASE_DATA_DIR, ZONE_FILE( nZoneNO ));
			if ( CUtil::Is_FileExist( szZoneFILE ) ) {
				return ZONE_NAME( nZoneNO );
			}
		}
	}
	return NULL;
}

short CLIB_GameSRV::InitLocalZone( bool bAllActive )
{
	SAFE_DELETE_ARRAY( m_pCheckedLocalZONE );

	m_pCheckedLocalZONE = new bool[ g_TblZONE.m_nDataCnt ];

	::FillMemory( m_pCheckedLocalZONE, sizeof(bool)*g_TblZONE.m_nDataCnt, bAllActive );

#ifndef	__INC_WORLD
	for (short nI=TEST_ZONE_NO; nI<g_TblZONE.m_nDataCnt; nI++) {
		m_pCheckedLocalZONE[ nI ] = false;
	}
#endif

	return g_TblZONE.m_nDataCnt;
}
bool CLIB_GameSRV::CheckZoneToLocal(short nZoneNO, bool bChecked)
{
	if ( nZoneNO > 0 && nZoneNO < g_TblZONE.m_nDataCnt ) {
		#ifndef	__INC_WORLD
		if ( nZoneNO >= TEST_ZONE_NO ) return false;
		#endif

		m_pCheckedLocalZONE[ nZoneNO ] = bChecked;
		return m_pCheckedLocalZONE[ nZoneNO ];
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
#define IS_SIGNED(type)				( ((type) (-1)) < ((type) 0) )
#define IS_UNSIGNED(type)			( ((type) (-1)) > ((type) 0) )

#ifndef COMPILE_TIME_ASSERT
#define COMPILE_TIME_ASSERT(expr)	{	typedef int compile_time_assert_fail[1 - 2 * !(expr)];	}
#endif

//-------------------------------------------------------------------------------------------------
bool CLIB_GameSRV::ConnectSERVER( char *szDBServerIP,		char *szDBName,
								  char *szDBUser,			char *szDBPassword,
								  char *szLogUser,			char *szLogPW,
								  char *szMallUser,			char *szMallPW,
								  char *szLoginServerIP,	int iLoginServerPort,
								  char *szAccountServerIP,	int iAccountServerPortNO )
{
	if ( NULL == this->GetInstance() )
		return false;

	m_AccountServerIP.Set( szAccountServerIP );
	m_iAccountServerPORT = iAccountServerPortNO;

	m_LoginServerIP.Set( szLoginServerIP );
	m_iLoginServerPORT = iLoginServerPort;

//	char *szDBUser     = "icarus";
//	char *szDBPassword = "1111";

	m_DBServerIP.Set( szDBServerIP );
	m_DBName.Set( szDBName );
	m_DBUser.Set( szDBUser );
	m_DBPassword.Set( szDBPassword );

	if ( *szLogUser == '?' ) {
		m_LogUser.Set( szDBUser );
		m_LogPW.Set( szDBPassword );
	} else {
		m_LogUser.Set( szLogUser );
		m_LogPW.Set( szLogPW );
	}

	g_pThreadSQL = new GS_CThreadSQL;	// suspend 모드로 시작됨.
	if ( !g_pThreadSQL->Connect( USE_ODBC , szDBServerIP, szDBUser, szDBPassword, szDBName, 32, 1024*8) ) {
		return false;
	}
	g_pThreadSQL->Resume ();

	CStrVAR stLogODBC;
#ifndef	__INC_WORLD
	stLogODBC.Set( "SHO_LOG" );
#else
	stLogODBC.Set( szDBName );
#endif

	g_pThreadLOG = new GS_CThreadLOG;
	if ( !g_pThreadLOG->Connect( USE_ODBC, szDBServerIP, m_LogUser.Get(), m_LogPW.Get(), stLogODBC.Get(), 32, 1024*8) ) {
		return false;
	}
	g_pThreadLOG->Resume ();

//	g_pThreadSQL->Sql_TEST ();
//	return false;


#ifdef	__INC_WORLD
	g_pThreadMSGR = new CThreadMSGR( 16, 0 );
	if ( !g_pThreadMSGR->Connect( USE_MY_SQL_AGENT ? USE_MY_SQL:USE_ODBC, (char*)szDBServerIP, szDBUser, szDBPassword, szDBName, 32, 1024*8) ) {
		return false;
	}
	g_pThreadMSGR->Resume ();

	g_pChatROOMs = new CChatRoomLIST( 20 );
#endif
	
	g_pThreadGUILD = new CThreadGUILD( 32, 16 );
	if ( !g_pThreadGUILD ||
		 !g_pThreadGUILD->Connect( USE_ODBC, (char*)szDBServerIP, szDBUser, szDBPassword, szDBName, 32, 1024*8) ) {
		return false;
	}
	g_pThreadGUILD->Resume ();

	CStrVAR strMallODBC;

#define	MALL_DB_IP		"127.0.0.1"
#ifdef	__INC_WORLD
	//#define	MALL_DB_NAME	"SHO"
	strMallODBC.Set(szDBName);
#else
	//#define	MALL_DB_NAME	"SHO_MALL"
	strMallODBC.Set("SHO_MALL");
#endif

	if ( *szMallUser != '?' ) {
		g_pThreadMALL = new GS_CThreadMALL;
		if ( !g_pThreadMALL->Connect( USE_ODBC, 
					MALL_DB_IP, 
					szMallUser, 
					szMallPW, 
					strMallODBC.Get(), 
					32, 1024 * 8 ) ) {
			return false;
		}
		g_pThreadMALL->Resume ();
	}

	return true;
}


//-------------------------------------------------------------------------------------------------
bool CLIB_GameSRV::Start( HWND hMainWND, char *szServerName, char *szClientListenIP, int iClientListenPort, BYTE btChannelNO, BYTE btLowAge, BYTE btHighAge )
{
	srand( timeGetTime() );

#ifndef	__INC_WORLD
	if ( _strnicmp( szServerName, "TEST", 4 ) == 0 )
		m_bTestServer = false;
#endif

	m_dwUserLIMIT = MAX_ZONE_USER_BUFF;

	g_pSockLSV->Init ( CSocketWND::GetInstance()->GetWindowHandle(), 
						m_LoginServerIP.Get(), m_iLoginServerPORT, 
						WM_LSVSOCK_MSG );
	this->ConnectToLSV ();

	g_pSockASV->Init ( CSocketWND::GetInstance()->GetWindowHandle(), 
						m_AccountServerIP.Get(), 
						m_iAccountServerPORT, 
						WM_ASVSOCK_MSG );
	this->ConnectToASV ();

#ifdef	__CDB_SOCKET
	g_pSockLOG->Init ( CSocketWND::GetInstance()->GetWindowHandle(), 
						m_LogServerIP.Get(),
						m_iLogServerPORT,
						WM_LOGSOCK_MSG, 
						m_DBName.Get(), 
						m_DBUser.Get(), m_DBPassword.Get(),
						GS_TIMER_LOG, RECONNECT_TIME_TICK, (TIMERPROC)GS_TimerProc );
#endif
	this->ConnectToLOG ();

	m_hMainWND = hMainWND;

	m_iListenPortNO = iClientListenPort;
	m_ServerNAME.Set( szServerName );
	m_btChannelNO = btChannelNO;
	m_btLowAGE = btLowAge;
	m_btHighAGE = btHighAge;

	// 접속된 IP로 공개되도록...
#ifdef	__INC_WORLD
	char *pHostName = CStr::GetString();
	char *pIP = CStr::GetString();

	CUtil::Get_HostName( pHostName, CStr::GetBufferLength() );
	CUtil::Get_IPAddressFromHostName( pHostName, pIP, CStr::GetBufferLength() );

	m_ServerIP.Set( pIP /* "127.0.0.1" */ );
#else
	m_ServerIP.Set( szClientListenIP );
#endif

	LogString( 0xfff, "sizeof(classUSER)	== %d \n", sizeof( classUSER ) );
	LogString( 0xfff, "sizeof(tagQuestData) == %d \n", sizeof( tagQuestData ) );

	COMPILE_TIME_ASSERT( sizeof(t_PACKETHEADER) == 6 );

//-------------------------------------
#ifndef __MAX_ITEM_999_EXT	// 2006.07.06/김대성/수정 - 아이템 999 제한 확장
	COMPILE_TIME_ASSERT( sizeof(tagITEM) == (6+sizeof(__int64)) );
#else



#ifndef	__ITEM_TIME_LIMIT
	COMPILE_TIME_ASSERT( sizeof(tagITEM) == (8+sizeof(__int64)) );
#else
	//	아이템 확장 방식 + 기간제 데이터 추가(DWORD, WORD)
	COMPILE_TIME_ASSERT( sizeof(tagITEM) == (8+sizeof(__int64) + sizeof(DWORD)+sizeof(WORD)) );
#endif



#endif
//-------------------------------------

//-------------------------------------
/*
	COMPILE_TIME_ASSERT( sizeof(gsv_DAMAGE) == (sizeof(t_PACKETHEADER)+6) );
*/
// 2006.03.07/김대성 - 최대 데미지 수정 => NET_Prototype.h 파일의 uniDAMAGE 구조체 수정
	COMPILE_TIME_ASSERT( sizeof(gsv_DAMAGE) == (sizeof(t_PACKETHEADER)+12) );
//-------------------------------------

	COMPILE_TIME_ASSERT( sizeof(char)  == 1);
	COMPILE_TIME_ASSERT( sizeof(short) >= 2);
	COMPILE_TIME_ASSERT( sizeof(long)  >= 4);
	COMPILE_TIME_ASSERT( sizeof(int)   >= sizeof(short));
	COMPILE_TIME_ASSERT( sizeof(long)  >= sizeof(int));

	COMPILE_TIME_ASSERT( sizeof( gsv_INVENTORY_DATA ) < MAX_PACKET_SIZE );
//	COMPILE_TIME_ASSERT( sizeof( gsv_QUEST_ONLY ) < MAX_PACKET_SIZE );

	COMPILE_TIME_ASSERT( IS_SIGNED(long) );
	COMPILE_TIME_ASSERT( IS_UNSIGNED(DWORD) );

    this->Shutdown ();
	g_dwStartTIME = classTIME::GetCurrentAbsSecond();

	LogString (LOG_NORMAL, "Size : %d / %d / %d \n", sizeof( t_PACKETHEADER ), sizeof( t_PACKET ), sizeof( classPACKET ) );

	g_pObjMGR = new CObjMNG( MAX_GAME_OBJECTS );
	g_pZoneLIST = CZoneLIST::Instance ();

#ifdef	__INC_WORLD
	// !!! 반드시 존 데이타 로딩하기 전에....
	COMPILE_TIME_ASSERT( sizeof( tagWorldVAR ) > 1 );
	g_pThreadSQL->Load_WORLDVAR( g_pZoneLIST->m_pVAR, sizeof( tagWorldVAR ) );
#else
	g_pThreadURL = new CurlTHREAD;
#endif

	g_pZoneLIST->InitZoneLIST( BASE_DATA_DIR );

	m_dwRandomSEED = ::timeGetTime();
	g_pPacketCODEC->Init( m_dwRandomSEED );

	m_pWorldTIMER = new CTimer( m_hMainWND, GS_TIMER_WORLD_TIME, WORLD_TIME_TICK, (TIMERPROC)GS_TimerProc );
	m_pWorldTIMER->Start ();

	g_pUserLIST->Active( m_iListenPortNO, MAX_ZONE_USER_BUFF, 5*60 );	// 5분 대기..

//	g_pSockLSV->Send_gsv_START( xxx )

#ifdef __CLAN_FIELD_SET
	//	크리세션
	GF_CF_CRITICAL_Set(true,m_hMainWND);
	
	//	클랜 필드 다시 설계 : 설정 정보 AI 에서 INI 로 처리.
	if( !GF_CF_Set_INI_Load() )
		MessageBox( NULL, "클필드 초기화 실패!", "ERROR", MB_OK );

	//	타이머 처리 함수 30초 간격으로 처리한다.
	m_pClanFieldTIMER = new CTimer( m_hMainWND, GS_TIMER_CLANFIELD, WORLD_TIME_TICK*3, (TIMERPROC)GS_TimerProc );
	m_pClanFieldTIMER->Start();
#endif

#ifdef __CLAN_WAR_SET
	//	관리 대화 상자 만들기 : 김영환
	GF_User_Dlg_Create(m_hMainWND);
	//	10초에 한번씩.
	m_pClanWarTIMER = new CTimer( m_hMainWND, GS_TIMER_CLANWAR, WORLD_TIME_TICK, (TIMERPROC)GS_TimerProc );
	m_pClanWarTIMER->Start();
#endif

//-------------------------------------
// 2006.12.06/김대성/추가 - 경험치 X배 치트키 시간대별 자동 실행	
#ifdef __CHEAT_SET_WORLDEXP
	//	60초에 한번씩 체크
	m_pWorldExpTIMER = new CTimer( m_hMainWND, GS_TIMER_CHEAT_SET_WORLDEXP, 60 * 1000, (TIMERPROC)GS_TimerProc );
	m_pWorldExpTIMER->Start();
#endif
//-------------------------------------

//-------------------------------------
// 2007.02.14/김대성/추가 - 기간제 아이템	
#ifdef __ITEM_TIME_LIMIT
	//	60초에 한번씩 체크
	m_pItemTimeLimitCheckTIMER = new CTimer( m_hMainWND, GS_TIMER_ITEM_TIME_LIMIT_CHECK, 60 * 1000, (TIMERPROC)GS_TimerProc );
	m_pItemTimeLimitCheckTIMER->Start();

	struct tm *l;
	time_t long_time;
	time( &long_time );				
	l = localtime( &long_time );
	m_nItemTimeLimitCheckHour = l->tm_hour;
#endif
//-------------------------------------

	SetWindowText(m_hMainWND,"Rose GameServer V1.17.1");
	return true;
}

//-------------------------------------------------------------------------------------------------
void CLIB_GameSRV::Shutdown ()
{

#ifdef __CLAN_WAR_SET
	//	관리 대화 상자 삭제 : 김영환
	GF_User_Dlg_Delete();
#endif

	SAFE_DELETE( m_pWorldTIMER );	// 타이머 삭제가 앞서도록...

#ifdef __CLAN_FIELD_SET
	SAFE_DELETE( m_pClanFieldTIMER );	// 타이머 삭제가 앞서도록...
	//	크리세션
	GF_CF_CRITICAL_Set(false,m_hMainWND);
#endif

#ifdef __CLAN_WAR_SET
	SAFE_DELETE( m_pClanWarTIMER );	// 타이머 삭제가 앞서도록..
#endif

//-------------------------------------
// 2006.12.06/김대성/추가 - 경험치 X배 치트키 시간대별 자동 실행	
#ifdef __CHEAT_SET_WORLDEXP
	SAFE_DELETE( m_pWorldExpTIMER );	// 타이머 삭제가 앞서도록..
#endif
//-------------------------------------

//-------------------------------------
// 2007.02.14/김대성/추가 - 기간제 아이템	
#ifdef __ITEM_TIME_LIMIT
	SAFE_DELETE( m_pItemTimeLimitCheckTIMER );	// 타이머 삭제가 앞서도록..
#endif
//-------------------------------------

	if ( g_pThreadURL ) {
		g_pThreadURL->Free ();
		SAFE_DELETE( g_pThreadURL );
	}

	//	종료 처리 수정 한다. 김영환. 유저 먼저 접속 해지 하고, 존 리스트 해지...

	g_pUserLIST->ShutdownACCEPT ();

	g_pUserLIST->ShutdownWORKER ();
	g_pUserLIST->ShutdownSOCKET ();

	if ( g_pZoneLIST ) {
		g_pZoneLIST->Destroy ();
		g_pZoneLIST = NULL;
	}

	// sql thread의 모든 내용이 기록 될동안 대기...
	if ( g_pThreadSQL ) {
		_ASSERT( g_pThreadSQL );
		g_pThreadSQL->Set_EVENT();
		do {
			::Sleep( 200 );		// wait 0.2 sec
		} while( !g_pThreadSQL->IsWaiting() || g_pThreadSQL->WaitUserCNT() > 0 ) ;	// 처리중이면 대기..
	}

    SAFE_DELETE( g_pObjMGR );
//	SAFE_DELETE( m_pWorldTIMER );
}

//-------------------------------------------------------------------------------------------------
bool CLIB_GameSRV::ConnectToLSV ()
{
	return g_pSockLSV->Connect();
}
void CLIB_GameSRV::DisconnectFromLSV ()
{
	g_pSockLSV->Disconnect ();
}

bool CLIB_GameSRV::ConnectToASV ()
{
	return g_pSockASV->Connect();
}
void CLIB_GameSRV::DisconnectFromASV ()
{
	g_pSockASV->Disconnect ();
}


//-------------------------------------------------------------------------------------------------
bool CLIB_GameSRV::ConnectToLOG ()
{
//	return g_pSockLOG->Connect();
	return true;
}
void CLIB_GameSRV::DisconnectFromLOG ()
{
//	g_pSockLOG->Disconnect ();
}

//-------------------------------------------------------------------------------------------------
void CLIB_GameSRV::Send_ANNOUNCE( short nZoneNO, char *szMsg )
{
	if ( nZoneNO ) 
		g_pZoneLIST->Send_gsv_ANNOUNCE_CHAT( nZoneNO, szMsg, NULL );
	else
		g_pZoneLIST->Send_gsv_ANNOUNCE_CHAT( szMsg, NULL );
}

//-------------------------------------------------------------------------------------------------
void CLIB_GameSRV::Set_UserLIMIT( DWORD dwUserLimit )
{
	m_dwUserLIMIT = dwUserLimit;

	if ( g_pSockLSV ) {
		g_pSockLSV->Send_srv_USER_LIMIT( dwUserLimit );		
	}
}

//-------------------------------------------------------------------------------------------------
