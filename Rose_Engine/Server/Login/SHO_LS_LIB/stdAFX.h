#ifndef	__STDAFX_H
#define	__STDAFX_H
//-------------------------------------------------------------------------------------------------

#define	_WIN32_WINNT 0x0500
//#define	__SERVER


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//#define	__SHO_LS_LIB  속성 전처리기에서 셋팅했기때문에 여기서는 막는다.


#include <Winsock2.h>
	
#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#include <assert.h>
#include <stdio.h>

#include "LIB_Util.h"
#include "classHASH.h"
#include "classTIME.h"
#include "classLOG.h"
#include "classSTR.h"

#include "classSYNCOBJ.h"
#include "CDataPOOL.h"
#include "DLLIST.h"
#include "CDLList.h"
#include "CVector.h"
#include "Net_Prototype.h"

#include "IOCPSocketSERVER.h"
#include "ioDataPOOL.h"
#include "CRandom.h"


////////////////////////////////////////////////////////////////////////////////////////////////
////////  국 가    **DEFINE**    ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//  ************************** 중요 여기서는 하나의 define만 설정해야 한다...****************************
//#define __ENGLAND			// 영국 서비스 안함

//#define __KOREA				// 한국은 오라클 DB를 사용한다!!!  SEVEN_ORA DB의 UserInfo 테이블이 다르다.
							// 한국은 플래티넘 요금제와 프리미엄 요금제 2가지가 있다. 플래티넘요금제가 더 좋다.
							// ※ 플래티넘 요금제의 추가 옵션
							// - 게임내 계정 공유 창고 +40칸 추가  (기본90칸 + 플래티넘창고 40칸)
							// - 게임내 케릭터 생성 슬롯 +2칸 추가 ( 전부 5개 )

//#define __KOREA_TEST		// 사내테스트는 MS-SQL DB 사용
//#define __JAPAN			// 일본
//#define __PHILIPPINE		// 필리핀은 과거 버그로인해 요금체계와는 상관없이 엑스트라 캐릭터를 사용할수 있다. 
                            // 한가지 요금체계이며 플래티넘 창고는 사용하지 못한다. 
                            // 2006.06.13  검서버는 한국과 동일하다.
#define __NORTHUSA		// 미국
//-------------------------------------
// 2006.05.17/김대성/추가
#define __PACKET_HEADER_VERSION		0x0000 // 패킷변조를 막기위해 패킷헤더의 pPacketHeader->m_wReserved 값을 특정값으로 체크

#pragma COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&&&LS57005&&&&&&&&&&&&&&&")
//-------------------------------------


//////////////////////국가별 설정에 따른 DEFINE///////////////////////////////////////////////
#ifdef __KOREA
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __KOREA     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __KOREA     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __KOREA     &&&&&&&&&&&&&&&&&&&&&&&")

#define	USE_ORACLE_DB	1	// 사내섭을 제외한...
#define BS_GUMS
#endif 



/////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------
// 김대성/추가
#ifdef __KOREA_TEST			// 사내 테스트용 (MS-SQL 사용)
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __KOREA_TEST  사내 테스트용   &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __KOREA_TEST  사내 테스트용   &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __KOREA_TEST  사내 테스트용   &&&&&&&&&&&&&&&&&&&&&&&")

#define	USE_MSSQL
#define BS_GUMS
#endif
//-------------------------------------



/////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __JAPAN
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN     &&&&&&&&&&&&&&&&&&&&&&&")

#define	USE_MSSQL						// 한국(오라클) ,대만 (자체서버인증)을 제외한 다른 나라...
//-------------------------------------
//2006.06.15/김대성/추가
#define __LOGIN_LOG			// 로그인 성공시 로그남기기
//-------------------------------------
#endif




/////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __PHILIPPINE
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE     &&&&&&&&&&&&&&&&&&&&&&&")

#define	USE_MSSQL						// 한국(오라클) ,대만 (자체서버인증)을 제외한 다른 나라...
#define __LOGIN_NATION_CODE				//아이로즈랑 구별하기 위해서 국가코드를 집어 넣음..
//-------------------------------------
//2006.06.15/김대성/추가
#define __LOGIN_LOG						// 로그인 성공시 로그남기기
//-------------------------------------
#endif



/////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __ENGLAND
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __ENGLAND     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __ENGLAND     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __ENGLAND     &&&&&&&&&&&&&&&&&&&&&&&")

#define	USE_MSSQL							// 한국(오라클) ,대만 (자체서버인증)을 제외한 다른 나라...
#define __ENG_BILLING_CHECK     // 영국 자체빌링 테이블을 읽어서 과금여부를  체크한다..
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __NORTHUSA
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA     &&&&&&&&&&&&&&&&&&&&&&&")

#define	USE_MSSQL							// 한국(오라클) ,대만 (자체서버인증)을 제외한 다른 나라...
//-------------------------------------
//2006.06.15/김대성/추가
#define __LOGIN_LOG			// 로그인 성공시 로그남기기
//-------------------------------------
#endif


//-------------------------------------------------------------------------------------------------
#endif