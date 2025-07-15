/*
	작성자 김영환
*/

//#ifdef __CLAN_WAR_SET			// 클랜전 추가 정의

//	관리 대화 상자
extern	HWND	g_hUserWND;
extern	char	G_Main_Dir[MAX_PATH];	// 프로그램 설치 폴더
//	접속 유저 정보 보기
extern	HWND	g_hList_User;
//	접속 클랜 정보 보기
extern	HWND	g_hList_Clan;
//	접속 클랜전 정보 보기
extern	HWND	g_hList_CWar;
// 로딩 퀘스트 리스트
extern	HWND	g_hList_Quest;

//	크리티컬 세션 처리
extern CRITICAL_SECTION	g_ClanWar_LOCK;
extern CRITICAL_SECTION	g_ClanWar_DBLOCK;

//	접속자 유저 리스트
extern	classUSER	*g_User_List[DF_MAX_USER_LIST];
//	접속 클랜 리스트
extern	GF_Clan_Info	*g_Clan_List;
extern	DWORD		g_Clan_Max_List;

////	클랜전 리스트
//	부활 포인트.
extern tPOINTF g_APosREVIVAL;	// 존 A팀 부활 위치
extern tPOINTF g_BPosREVIVAL;	// 존 B팀 부활 위치
extern	GF_ClanWar_Set	g_ClanWar_List[DF_MAX_CWAR_LIST];
extern	GF_World_Pos	g_ClanWar_ReCall;	// 클랜전 복귀 위치

////	클랜전 몬스터 정보 구조
extern	GF_ClanWar_MOB	g_CW_MOB_List[DF_MAX_CWAR_TYPE][DF_MAX_CWAR_MOB][DF_CWAR_TEAM_N];

////	클랜전 보상 아이템 정보
extern	GF_CWar_Win_Gift	g_CW_Win_Gift[DF_MAX_CWAR_TYPE];

////	유저 리스트
extern CUserLIST		*g_pUserLIST;
////	존 리스트
extern CZoneLIST		*g_pZoneLIST;


/*
	클랜전 DB 처리 함수
*/
extern DWORD		G_Server_ID;		// 서버 ID
extern DWORD		G_SV_Channel;		// 채널 ID
extern KDB_SQL		*G_SQL_CWar;		// 클랜전 DB 연결
extern F_Thread		*G_SQL_CWar_Thread;	// 클랜전 DB 처리 스레드
extern F_Buffer		*G_SQL_CWar_Buffer;	// 클랜전 DB 처리 버퍼

/*
	클랜전 설정 변수
*/
extern __int64		G_CW_BAT_MONEY[3];	// 클랜전 배팅 금액
extern DWORD	G_CW_MAX_LEVEL[3];		// 클랜전 LV 처리
extern DWORD	G_CW_MAX_USER_NO[3][2];	// 클랜전 최대 참여 인원
extern DWORD	DF_CWAR_PLAY_TIME;	//48//120		// 20분
extern DWORD	DF_CWAR_END_TIME;	//3 //12		// 2분
extern DWORD	DF_CWAR_WAIT_TIME;		//12		// 2분 대기
//#endif