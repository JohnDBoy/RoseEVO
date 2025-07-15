
////	마일리지 스킬 적용 정의
////	김영환 작성
#define __MILEAGE_SKILL_USED	// 마일리지 스킬 : DB, Userdata, 패킷 등등 관련 수정

#define __CLAN_WAR_SET			// 1차 클랜전 추가 정의

#define __GEMITEM_BREAK			// 보석 분해 기능 추가.
#define __GIFT_BOX_ITEM			// 선물 상자 추가.
#define __MESSAGE_LV_1			// 중요도 1급 메시지 처리 추가 함수.[1:1 중요도.]
#define __SKILL_RESET_ITEM		// 스킬 리셋 아이템 추가
#define __ZONE_EXP_SETUP		// 존별 경험치 처리 추가
#define __CLAN_FIELD_SET		// 클랜 필드 설정 로직 추가.
#define __CLAN_INFO_ADD			// 클랜 정보 추가 : 랭킹 레더 점수 처리.


// (20060913:남병철) : 파티 경험치 제한
#define __PARTY_EXP_WARNING

// (20061106:남병철) : 리젠 시스템 수정 (그룹 리젠) (참고:순차 개별 리젠은 8월말에 받은 테스트용 서버 코드에 구현되어있음)
#define __GROUP_REGEN

// (20061106:남병철) : 커뮤니티 기능 추가 (대화방 1:1 채팅), 방제 중복 처리, 비밀방
// (**주의** 클라이언트와 같이 구현되었으므로 define을 제거하면 않된다.)
#define __COMMUNITY_1_1_CHAT

// 2006.10.23/김대성/추가
#define __BATTLECART2	// 배틀카트2(카트정탄화)

// (20061120:남병철) : HP가 있어야 자살 가능
// void F_AIACT23 (stActHead *pActDATA, t_AIPARAM *pEVENT) 수정됨


// (20061123:남병철) : QUEST_BLOCK_LIST 추가
// QUEST_BLOCK_LIST.ini 파일 추가
#define __QUEST_BLOCK_LIST

// 2006.11.09/김대성/추가
#define __CHEAT_GIVE_ITEM_	// 아이템 나눠주기 치트기 추가


// 2006.11.20/김대성/추가
#define __BUFF_CALC_		// 버프계산공식 수정


// 2006.12.06/김대성/추가
//#define __CHEAT_SET_WORLDEXP	// 경험치 X배 치트키 시간대별 자동 실행		


// 2007.01.10/김대성/추가	
#define __ITEM_TIME_LIMIT		// 기간제 아이템


// 2007.02.28/김대성/추가
#define __GIFT_BOX_ITEM_OPTION	// 선물상자 옵션붙이기

// 2007.03.06/김대성/추가
#define __CRAFT_UPGRADE_4_5		// 4,5단계의 제련실패 시 아이템이 사라지지 않고 1~3단계로 랜덤하게 등급이 떨어진다.

// (20061201:남병철) : USEITEM.STB 기능 추가 (경험치 아이템 추가)
#define __EXPERIENCE_ITEM


// (20061206:남병철) : 일정 확률로 전맵/특정맵에 이벤트 몬스터 리젠
//#define __EVENT_MOB_REGEN
//#define __EVENT_MOB_NO		203		// 젤리킹:203
//#define __EVENT_PERCENT		20		// 100% 기준
//#define __EVENT_ZONE_NO		24		// 아니마호수:24 (전체맵:0)



// (20061222:남병철) : 클랜전 보상 포인트 추가
#define __CLAN_REWARD_POINT

// (20070111:남병철) : 클랜전 창설 비용 수정 (100만 -> 10만) - 미구현
//#define __CLAN_CREATE_COST

// (20070305:남병철) : 클랜전 기능 확장 및 보상 포인트, 보상 포인트 상점 추가
// __CLAN_WAR_SET과 같이 사용한다.
#define __CLAN_WAR_EXT

#define	MAX_AVATAR_NAME	20


#include "LIB_gsMAIN.h"

#ifdef	__CLAN_WAR_SET
	#include "GF_Dlg_User.h"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
////////  국 가    **DEFINE**    ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//  ************************** 중요 여기서는 하나의 define만 설정해야 한다...****************************
//#define __TAIWAN				    // 완전히 서비스 접었음.	2006/03/:김대성
//#define __ENGLAND					// 완전히 서비스 접었음.	2006/03/:김대성
//#define __EUROPE					// 영국을 제외한 유럽국가....
//#define __KR_RENEWAL				// 한국 리뉴얼버전	 - 일본 에볼루션 버젼과 똑같다.
//#define __JAPAN					// 일본 본섭 (구버전) => 일본버전도 완전하지 않음. (2006.04.11)
//#define __NORTHUSA				// 미국 치트키 막은거
//#define __NORTHUSA_TEST			// 미국 치트키 되는거 -> 테스트용
//#define __PHILIPPINE				// 필리핀 본섭
//#define __KOREA					// 한국버전은 완전하지 않음. 더이상 개발 불가 (2006.04.11) => 복구완료 (2006.05.16)
//--------------------------------------------------------------


//#define __KOREA_EVO					// 한국 에볼루션. 2006.07 
//#define __PHILIPPINE_EVO			// 필리핀 에보 2006.06
//#define __NORTHUSA_EVO			// 미국에보 치트키 막은거
#define __NORTHUSA_EVO_TEST		// 미국에보 치트키 되는거 -> 테스트용
//#define __JAPAN_EVO					// 일본 에볼루션
/////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------
// 2006.05.17/김대성/추가
#define __PACKET_HEADER_VERSION				0x0000 // 패킷변조를 막기위해 패킷헤더의 pPacketHeader->m_wReserved 값을 특정값으로 체크

#define __MAX_ITEM_999_EXT //정의			// 2006.07.06/김대성/추가 - 아이템 999 제한 확장



#ifndef	COMPILE_TIME_MSG
#define TO_NAME(x)					#x 
#define TO_NAME2(x)					TO_NAME(x) 
#define COMPILE_TIME_MSG(szDESC)	message(__FILE__ "(" TO_NAME2(__LINE__) ") : check! " szDESC) 
#endif

#pragma COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&&GS57005&&&&&&&&&&&&&&&&&")

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __KOREA
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __KOREA     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __KOREA     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __KOREA     &&&&&&&&&&&&&&&&&&&&&&&")

#define __INC_PLATINUM					// 우리나라만 존재한다. => ??? NET_Prototype.h 에 정의되어 있어서 모든 국가에 다 적용된다.
#define __DONT_CHECK_CREATE_EXP			// 일본, 한국 버젼일 경우 제조경험치를 체크하지 않는다
#define __ADD_CREATE_ITEM_EXP			// 일본, 한국 버젼일 경우 제조경험치를 일반경험치에 더해준다.

//-------------------------------------
// 2006.05.12/김대성/추가
#define __PASS_LIMIT_CARTSPEED			// 일본,한국 버젼일경우..  무게에따른 카트스피드제한 해제(패스)...
//-------------------------------------
#endif




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __KOREA_EVO
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __KOREA_EVO     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __KOREA_EVO     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __KOREA_EVO     &&&&&&&&&&&&&&&&&&&&&&&")

#define __INC_PLATINUM					// 우리나라만 존재한다. => ??? NET_Prototype.h 에 정의되어 있어서 모든 국가에 다 적용된다.
#define __KR_RENEWAL	
#endif




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __TAIWAN					// 완전히 서비스 접었음.
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __TAIWAN     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __TAIWAN     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __TAIWAN     &&&&&&&&&&&&&&&&&&&&&&&")

#define __KCHS_CHATLOG__				// 대만 버젼에서 채팅로그 남김
#define __TAW_RENEWAL					// 대만 초기 리뉴얼 버젼
#define __KCHS_BATTLECART__
#define __TRY_CATCH_PROC_ZONEPACKET		// 존스래드에서 유저 패킷 처리하는 부분TRY_CATCH 사용 (허재영 2005.12.19)
#define __APPLY_2ND_JOB					// 리뉴얼 2차전직..
#define __KRRE_UPGRADE					// 한국 리뉴얼. 재련
#define __KRRE_BREAK					// 한국 리뉴얼 아이템분해  허재영 2005.11.1
//정식버젼 으로 넘결줄때는 반듯이 치트 막고...
//#define __LIMIT_CHEAT_ALL      
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __JAPAN						
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN     &&&&&&&&&&&&&&&&&&&&&&&")

//#define __JP_CLAN_CRREAT_MONEY 100000		// 일본 클랜창설비용.. 100만->10만으로 수정..(허재영 2005,10,31) => 지금은 취소(김대성/2006.04.011)
#define __DONT_CHECK_CREATE_EXP				// 일본, 한국 버젼일 경우 제조경험치를 체크하지 않는다
#define __ADD_CREATE_ITEM_EXP				// 일본, 한국 버젼일 경우 제조경험치를 일반경험치에 더해준다.
#define __PASS_LIMIT_CARTSPEED				// 일본,한국 버젼일경우..  무게에따른 카트스피드제한 해제(패스)...
#define __PASS_PLAY_FLAG_DUNGEON_ADV		// 일본 임시로 던젼 어드벤쳐요금 패스~~~ => 데이타상의 문제로 인해 아예 무료화.
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2006.03.20/김대성 - 일본 리뉴얼 버전은 현재(2006.03.20) 한국 리뉴얼 버전하고 똑같다.
#ifdef __JAPAN_EVO						// 한국 리뉴얼버전과 똑같음
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN_EVO     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN_EVO     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN_EVO     &&&&&&&&&&&&&&&&&&&&&&&")

#define __KR_RENEWAL						// 한국 리뉴얼 버전하고 똑같다.
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __PHILIPPINE
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE     &&&&&&&&&&&&&&&&&&&&&&&")

#define __KCHS_BATTLECART__
#define __APPLY_2ND_JOB
#define __N_PROTECT
#define __TAW_RENEWAL						//대만 초기 리뉴얼 버젼

#define __KRRE_UPGRADE						// 한국 리뉴얼. 재련
#define __KRRE_BREAK						// 한국 리뉴얼 아이템분해  허재영 2005.11.1
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __PHILIPPINE_EVO
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE_EVO     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE_EVO     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE_EVO     &&&&&&&&&&&&&&&&&&&&&&&")

// #define __N_PROTECT			// 임시로 제거함.
#define __KR_RENEWAL						// 한국 리뉴얼 버전하고 똑같다.
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __NORTHUSA
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA     &&&&&&&&&&&&&&&&&&&&&&&")

//#define __N_PROTECT							// nProtect 용
#define __TAW_RENEWAL						// 대만 초기 리뉴얼 버젼
#define __KCHS_BATTLECART__					// 미국 상용화에 따른추가  허재영 2005.11.16
#define __APPLY_2ND_JOB						// 미국 상용화에 따른추가  허재영 2005.11.16 
#define __KRRE_UPGRADE						// 한국 리뉴얼. 재련
#define __KRRE_BREAK						// 한국 리뉴얼 아이템분해  허재영 2005.11.1

#define __TRY_CATCH_PROC_ZONEPACKET			//존스래드에서 유저 패킷 처리하는 부분TRY_CATCH 사용 (허재영 2005.12.19)
//-------------------------------------
// 미국의 경우에는 정식버전에서는 치트키를 막는다.
#define __LIMIT_CHEAT_SET					//치트키 SET 막음 (허재영 2005.11.14)
#define __LIMIT_CHEAT_ITEM					// 치트키 ITEM 막음(허재영 2005.11.14)
#define __LIMIT_CHEAT_ADD					// 치트키 ADD 막음(허재영 2005.11.14)
#define __LIMIT_CHEAT_MON					// 치트키 MON 막음(허재영 2005.11.14)
//-------------------------------------
// 2006.06.16/김대성/추가 - 미국의 요청으로 '/' 로 시작하는 모든 메시지 로그 남기기
#define __CHAT_CHEAT_LOG_ALL						
//-------------------------------------
#endif




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __NORTHUSA_TEST		// 치트키 되는거 -> 테스트용
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA_TEST     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA_TEST     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA_TEST     &&&&&&&&&&&&&&&&&&&&&&&")

//#define __N_PROTECT							// nProtect 용
#define __TAW_RENEWAL						// 대만 초기 리뉴얼 버젼
#define __KCHS_BATTLECART__					// 미국 상용화에 따른추가  허재영 2005.11.16
#define __APPLY_2ND_JOB						// 미국 상용화에 따른추가  허재영 2005.11.16 
#define __KRRE_UPGRADE						// 한국 리뉴얼. 재련
#define __KRRE_BREAK						// 한국 리뉴얼 아이템분해  허재영 2005.11.1

#define __TRY_CATCH_PROC_ZONEPACKET			// 존스래드에서 유저 패킷 처리하는 부분TRY_CATCH 사용 (허재영 2005.12.19)
//-------------------------------------
// 미국의 경우에는 정식버전에서는 치트키를 막는다.
//#define __LIMIT_CHEAT_SET					// 치트키 SET 막음 (허재영 2005.11.14)
//#define __LIMIT_CHEAT_ITEM				// 치트키 ITEM 막음(허재영 2005.11.14)
//#define __LIMIT_CHEAT_ADD					// 치트키 ADD 막음(허재영 2005.11.14)
//#define __LIMIT_CHEAT_MON					// 치트키 MON 막음(허재영 2005.11.14)
//-------------------------------------
// 2006.06.16/김대성/추가 - 미국의 요청으로 '/' 로 시작하는 모든 메시지 로그 남기기
#define __CHAT_CHEAT_LOG_ALL						
//-------------------------------------
#endif




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __NORTHUSA_EVO
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA_EVO     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA_EVO     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA_EVO     &&&&&&&&&&&&&&&&&&&&&&&")

//#define __N_PROTECT							// nProtect 용
#define __TRY_CATCH_PROC_ZONEPACKET			// 존스래드에서 유저 패킷 처리하는 부분TRY_CATCH 사용 (허재영 2005.12.19)
//-------------------------------------
// 미국의 경우에는 정식버전에서는 치트키를 막는다.
#define __LIMIT_CHEAT_SET					// 치트키 SET 막음 (허재영 2005.11.14)
#define __ALLOW_CHEAT_SET_WORLDEXP			// 로컬이벤트를 위해서 막지 않음.(2006.10.12/김대성)
#define __ALLOW_CHEAT_SET_WORLDDROP		// 로컬이벤트를 위해서 막지 않음.(2006.10.12/김대성)

#define __LIMIT_CHEAT_ITEM					// 치트키 ITEM 막음(허재영 2005.11.14)
#define __LIMIT_CHEAT_ADD					// 치트키 ADD 막음(허재영 2005.11.14)
//#define __LIMIT_CHEAT_MON					// 치트키 MON 막음(허재영 2005.11.14)	// 로컬이벤트를 위해서 막지 않음.(2006.10.12/김대성)
//-------------------------------------
// 2006.06.16/김대성/추가 - 미국의 요청으로 '/' 로 시작하는 모든 메시지 로그 남기기
#define __CHAT_CHEAT_LOG_ALL						
//-------------------------------------
#define __KR_RENEWAL						// 한국 리뉴얼 버전하고 똑같다.
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __NORTHUSA_EVO_TEST	// 치트키 되는거 -> 테스트용
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA_EVO_TEST     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA_EVO_TEST     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA_EVO_TEST     &&&&&&&&&&&&&&&&&&&&&&&")

//#define __N_PROTECT							// nProtect 용
#define __TRY_CATCH_PROC_ZONEPACKET			// 존스래드에서 유저 패킷 처리하는 부분TRY_CATCH 사용 (허재영 2005.12.19)
//-------------------------------------
// 미국의 경우에는 정식버전에서는 치트키를 막는다.
//#define __LIMIT_CHEAT_SET					// 치트키 SET 막음 (허재영 2005.11.14)
//#define __LIMIT_CHEAT_ITEM				// 치트키 ITEM 막음(허재영 2005.11.14)
//#define __LIMIT_CHEAT_ADD					// 치트키 ADD 막음(허재영 2005.11.14)
//#define __LIMIT_CHEAT_MON					// 치트키 MON 막음(허재영 2005.11.14)
//-------------------------------------
// 2006.06.16/김대성/추가 - 미국의 요청으로 '/' 로 시작하는 모든 메시지 로그 남기기
#define __CHAT_CHEAT_LOG_ALL						
//-------------------------------------
#define __KR_RENEWAL						// 한국 리뉴얼 버전하고 똑같다.
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __ENGLAND
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __ENGLAND     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __ENGLAND     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __ENGLAND     &&&&&&&&&&&&&&&&&&&&&&&")

#define __N_PROTECT							// nProtect 용
#define __TAW_RENEWAL						// 대만 초기 리뉴얼 버젼
#define __KCHS_BATTLECART__					// 미국 상용화에 따른추가  허재영 2005.11.16
#define __APPLY_2ND_JOB						// 리뉴얼 2차전직..
#define __TRY_CATCH_PROC_ZONEPACKET			// 존스래드에서 유저 패킷 처리하는 부분TRY_CATCH 사용 (허재영 2005.12.19)
#define __KRRE_UPGRADE						// 한국 리뉴얼. 재련
#define __KRRE_BREAK						// 한국 리뉴얼 아이템분해  허재영 2005.11.1
//정식버젼 으로 넘결줄때는 반듯이 치트 막고...
#define __LIMIT_CHEAT_SET					// 치트키 SET 막음 (허재영 2005.11.14)
#define __LIMIT_CHEAT_ITEM					// 치트키 ITEM 막음(허재영 2005.11.14)
#define __LIMIT_CHEAT_ADD					// 치트키 ADD 막음(허재영 2005.11.14)
#define __LIMIT_CHEAT_MON					// 치트키 MON 막음(허재영 2005.11.14)
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __EUROPE
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __EUROPE(영국제외)     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __EUROPE(영국제외)     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __EUROPE(영국제외)     &&&&&&&&&&&&&&&&&&&&&&&")

#define __N_PROTECT							// nProtect 용
#define __TAW_RENEWAL						// 대만 초기 리뉴얼 버젼
#define __APPLY_2ND_JOB						// 리뉴얼 2차전직..
#endif 



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef  __KR_RENEWAL						// 한국 리뉴얼버젼은 대만 초기 리뉴얼버젼으로 작업한다..

#define __TAW_RENEWAL						// 대만 초기 리뉴얼 버젼
#define __KCHS_BATTLECART__					// 배틀카트
#define __APPLY_2ND_JOB						// 리뉴얼 2차전직..
#define __TEST_CHK_WARP						// 워프 체크루틴 테스트 허재영 2005.12.115

#define __KRRE_UPGRADE						// 한국 리뉴얼. 재련
#define __KRRE_BREAK						// 한국 리뉴얼 아이템분해  허재영 2005.11.1

// (20060922:남병철) : DROP_ITEM.STB 파일을 SVN에서 삭제 했음 (앞으로는 아래 주석을 풀면 오류남) - 추후 아래 정의 관련으로 삭제 가능
#define __KRRE_ITEMDROP						// 한국 리뉴얼 아이템 드랍... 허재영 2005.11.14

#define __KRRE_START_ZONEPOS				// 한국 리뉴얼 작업 AVATAR STB 에 시작존 위치가 저장됨.. 하드코딩에서 수정..
#define __KRRE_CAL_MAXHP					// 한국 리뉴얼 유저캐릭터 HP 계산식수정.
#define __KRRE_CAL_DEFENCE					// 한국 리뉴얼 방어력  계산식수정.
#define __KRRE_CAL_MON_CRITICAL				// 한국 리뉴얼 몬스터 크리티컬 계산식수정.
#define __KRRE_CAL_SKILL_DMG				// 한국 리뉴얼 스킬 계산식수정.

// ---------------------------------------- 여기서 부터는 디비 디자인 변경된 버젼 ---------------------------------------- //
#define __KRRE_HOTIOCN_EXT					// 한국 리뉴얼 단축슬롯 확장 4 ->6 페이지.. 허재영 2005 11.18
#define __KRRE_NEW_AVATAR_DB				// 한국리뉴얼 작업에 디자인 변경된 새로운 디비 (인벤확장,마일리지 아이템 추가,스킬인벤 구조변경,쿨타임추가)
#define __KRRE_MILEAGE_ITEM					// 마일리지 아이템 체크...
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// (20060919:남병철) : 디버깅용
//#include "ZSTD.h"
// (20070222:남병철) : 클랜전 헤더 ZSTD.h 포함
#include "ClanWar.h"
