
/*
	작성자 김영환
*/

//#ifdef __CLAN_WAR_SET			// 클랜전 추가 정의


#include "GF_Define.h"


//	관리 대화 상자
HWND	g_hUserWND = NULL;
char	G_Main_Dir[MAX_PATH];	// 프로그램 설치 폴더
//	접속 유저 정보 보기
HWND	g_hList_User = NULL;
//	접속 클랜 정보 보기
HWND	g_hList_Clan = NULL;
//	접속 클랜전 정보 보기
HWND	g_hList_CWar = NULL;
//	로딩 퀘스트 리스트
HWND	g_hList_Quest = NULL;

//	크리티컬 세션 처리
CRITICAL_SECTION	g_ClanWar_LOCK;
CRITICAL_SECTION	g_ClanWar_DBLOCK;

//	접속자 유저 리스트
classUSER	*g_User_List[DF_MAX_USER_LIST];
//	접속 클랜 리스트
GF_Clan_Info	*g_Clan_List;
DWORD			g_Clan_Max_List = 0;

////	클랜전 리스트
tPOINTF g_APosREVIVAL;	// 존 A팀 부활 위치
tPOINTF g_BPosREVIVAL;	// 존 B팀 부활 위치
GF_ClanWar_Set	g_ClanWar_List[DF_MAX_CWAR_LIST];
GF_World_Pos	g_ClanWar_ReCall;	// 클랜전 복귀 위치

////	클랜전 몬스터 정보 구조
GF_ClanWar_MOB	g_CW_MOB_List[DF_MAX_CWAR_TYPE][DF_MAX_CWAR_MOB][DF_CWAR_TEAM_N];

////	클랜전 보상 아이템 정보
GF_CWar_Win_Gift	g_CW_Win_Gift[DF_MAX_CWAR_TYPE];

/*
	클랜전 DB 처리 함수
*/
DWORD		G_Server_ID = 0;			// 서버 ID
DWORD		G_SV_Channel = 1;			// 채널 ID
KDB_SQL		*G_SQL_CWar = NULL;			// 클랜전 DB 연결
F_Thread	*G_SQL_CWar_Thread = NULL;	// 클랜전 DB 처리 스레드
F_Buffer	*G_SQL_CWar_Buffer = NULL;	// 클랜전 DB 처리 버퍼

/*
	클랜전 설정 변수
*/
__int64		G_CW_BAT_MONEY[4];	// 클랜전 배팅 금액
DWORD	G_CW_MAX_LEVEL[4];		// 클랜전 LV 처리
DWORD	G_CW_MAX_USER_NO[4][2];	// 클랜전 최대 참여 인원
DWORD	DF_CWAR_PLAY_TIME = 120;	//120		// 20분
DWORD	DF_CWAR_END_TIME = 12;		//12		// 2분
DWORD	DF_CWAR_WAIT_TIME = 12;		//12		// 2분 대기
//#endif