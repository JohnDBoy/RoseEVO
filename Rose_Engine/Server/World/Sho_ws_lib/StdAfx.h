// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__BC125CC8_4AFD_4EA7_A465_E7BFBABBD26E__INCLUDED_)
#define AFX_STDAFX_H__BC125CC8_4AFD_4EA7_A465_E7BFBABBD26E__INCLUDED_



#define	_WIN32_WINNT 0x0500



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// TODO: reference additional headers your program requires here
#include <windows.h>
#include <crtdbg.h>
#include <assert.h>

//#define	__SHO_WS   //속성 전처리기에서 선언함....
//	마일리지 스킬 적용 정의
//	김영환 작성
#define __MILEAGE_SKILL_USED	// 마일리지 스킬 : DB, Userdata, 패킷 등등 관련 수정

#define __CLAN_INFO_ADD			// 클랜 정보 추가 : 랭킹 레더 점수 처리.

#define __CLAN_WAR_SET			// 클랜전 추가 정의

// (20061222:남병철) : 클랜전 보상 포인트 추가
#define __CLAN_REWARD_POINT


// 2007.01.10/김대성/추가	
#define __ITEM_TIME_LIMIT		// 기간제 아이템



#include "LIB_Util.h"
#include "classHASH.h"
#include "classTIME.h"
#include "classLOG.h"
#include "classSTR.h"
#include "classSTB.h"
#include "classSYNCOBJ.h"
#include "CRandom.h"
#include "CDataPOOL.h"
#include "DLLIST.h"
#include "CDLList.h"
#include "CSLList.h"
#include "CVector.h"





#include "ZSTD.h"
#define __COMMUNITY_1_1_CHAT



////////////////////////////////////////////////////////////////////////////////////////////////
////////  국 가    **DEFINE**    ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//  ************************** 중요 여기서는 하나의 define만 설정해야 한다...****************************

//#define __ENGLAND
//#define __JAPAN				// 일본 본섭 (구버전)
//#define __EUROPE				// 영국을 제외한 유럽국가....
//#define __KR_RENEWAL			// 한국 리뉴얼버전
//#define __KOREA				// 한국버전은 완전하지 않음. 더이상 개발 불가 (2006.04.11) => 복구완료 (2006.05.16)
//#define __PHILIPPINE			// 필리핀 본섭
//#define __NORTHUSA			// 미국 본섭


//#define __KOREA_EVO				// 한국버전은 완전하지 않음. 더이상 개발 불가 (2006.04.11) => 복구완료 (2006.05.16)
//#define __PHILIPPINE_EVO		// 필리핀 에볼루션
#define __NORTHUSA_EVO		// 미국 에볼루션
//#define __JAPAN_EVO				// 일본 에볼루션
/////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------
// 2006.05.17/김대성/추가
#define __PACKET_HEADER_VERSION		0x0000 // 패킷변조를 막기위해 패킷헤더의 pPacketHeader->m_wReserved 값을 특정값으로 체크


#pragma COMPILE_TIME_MSG("&&&&&&&&&&&&&&&WS57005&&&&&&&&&&&&&&&&&&&")
//-------------------------------------
#define __MAX_ITEM_999_EXT //정의			// 2006.07.06/김대성/추가 - 아이템 999 제한 확장


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __KOREA
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __KOREA     &&&&&&&&&&&&&&&&&&&&&&&")

#define __INC_PLATINUM				// 우리나라만 존재한다.		=> ??? NET_Prototype.h 에 정의되어 있어서 모든 국가에 다 적용된다.
#define __PASS_FREE_PLATINUM_CHAR   // 우리 나라 무료 이벤트시 프래티넘 캐릭터접속할수있게 한다..
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __KOREA_EVO
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __KOREA     &&&&&&&&&&&&&&&&&&&&&&&")

#define __INC_PLATINUM				// 우리나라만 존재한다.		=> ??? NET_Prototype.h 에 정의되어 있어서 모든 국가에 다 적용된다.
#define __KR_RENEWAL
#endif


#ifdef __JAPAN
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN     &&&&&&&&&&&&&&&&&&&&&&&")

#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __JAPAN_EVO
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN_EVO     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN_EVO     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN_EVO     &&&&&&&&&&&&&&&&&&&&&&&")

#define __KR_RENEWAL
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __PHILIPPINE
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE     &&&&&&&&&&&&&&&&&&&&&&&")

#define __TAW_RENEWAL					//대만 초기 리뉴얼 버젼
#define __KCHS_BATTLECART__
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __PHILIPPINE_EVO
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE_EVO     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE_EVO     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE_EVO     &&&&&&&&&&&&&&&&&&&&&&&")

#define __KR_RENEWAL
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __ENGLAND
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __ENGLAND     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __ENGLAND     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __ENGLAND     &&&&&&&&&&&&&&&&&&&&&&&")

#define __TAW_RENEWAL					// 대만 초기 리뉴얼 버젼
#define __MAX_CHAR_3			3		// 최대 케릭터수를 5 ->3 으로 조정..
#define __KCHS_BATTLECART__
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __EUROPE
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __EUROPE(영국제외)     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __EUROPE(영국제외)     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __EUROPE(영국제외)     &&&&&&&&&&&&&&&&&&&&&&&")

#define __TAW_RENEWAL					// 대만 초기 리뉴얼 버젼
#define __MAX_CHAR_3			3		// 최대 케릭터수를 5 ->3 으로 조정..
#define __KCHS_BATTLECART__	
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __NORTHUSA
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA(미국)     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA(미국)     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA(미국)     &&&&&&&&&&&&&&&&&&&&&&&")

#define __TAW_RENEWAL					// 대만 초기 리뉴얼 버젼
#define __MAX_CHAR_3			3		// 최대 케릭터수를 5 ->3 으로 조정..
#define __KCHS_BATTLECART__
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __NORTHUSA_EVO
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA_EVO(미국 에볼루션)     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA_EVO(미국 에볼루션)     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA_EVO(미국 에볼루션)     &&&&&&&&&&&&&&&&&&&&&&&")
 
#define __MAX_CHAR_3			3		// 최대 케릭터수를 5 ->3 으로 조정..
#define __KR_RENEWAL
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef  __KR_RENEWAL					//한국 리뉴얼버젼은 대만 초기 리뉴얼버젼으로 작업한다..

#define __TAW_RENEWAL					//대만 초기 리뉴얼 버젼
#define __KCHS_BATTLECART__				//배틀카트
#define __KRRE_START_ZONEPOS			//한국 리뉴얼 작업 AVATAR STB 에 시작존 위치가 저장됨.. 하드코딩에서 수정..

///////////////////////////  여기서 부터는 디비 디자인 변경된 버젼.///////////////////////////////////////////////////////////
 //(인벤확장,마일리지 아이템 추가,스킬인벤 구조변경,쿨타임추가)
#define __KRRE_NEW_AVATAR_DB			//한국리뉴얼 작업에 디자인 변경된 새로운 디비
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif



#include "Net_Prototype.h"
#include "ioDataPOOL.h"			//	Net_Prototype.h 보다 뒤에 !!!

#include "IOCPSocketSERVER.h"
#include "CClientSOCKET.h"
#include "CSocketWND.h"

typedef		classSTB	CGameSTB;	






//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BC125CC8_4AFD_4EA7_A465_E7BFBABBD26E__INCLUDED_)

