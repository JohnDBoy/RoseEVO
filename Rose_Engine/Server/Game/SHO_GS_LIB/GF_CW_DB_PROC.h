//#ifdef __CLAN_WAR_SET			// 클랜전 추가 정의
////	클랜전 신청 및 결과 DB 저장 처리 함수
//	작성자 : 김영환

////	클랜전 정보 처리
DWORD	WINAPI	GF_CWar_Thread_Proc(LPDWORD lpData);

////	클랜전 기록.
BOOL	GF_DB_CWar_Insert(DWORD p_WarType,DWORD p_WarWin,__int64 p_Money,DWORD p_AClan,DWORD p_BClan, char* pComment);

////	클랜전 기록...
BOOL	GF_DB_CWar_Insert(GF_CWar_DB_ST *p_St);

////	DB 처리 함수
BOOL	GF_DB_CWar_Proc(GF_CWar_DB_ST *p_ST);

//#endif