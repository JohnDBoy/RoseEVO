//#ifdef __CLAN_WAR_SET			// 클랜전 추가 정의

////	클랜전 신청 및 결과 DB 저장 처리 함수 초기화
//	작성자 : 김영환

#include "GF_Define.h"
#include "GF_Data.h"


////	클랜전 정보 처리
DWORD	WINAPI	GF_CWar_Thread_Proc(LPDWORD lpData)
{
	if(G_SQL_CWar_Thread == NULL)
		return (0);
	while(G_SQL_CWar_Thread->Ex_ID != 0)
	{
		G_SQL_CWar_Thread->Wait_Event(INFINITE);
		if(G_SQL_CWar_Thread->Ex_ID == 0) return (0);
		//	데이터 가져오기
		if(G_SQL_CWar_Buffer != NULL)
		{
			if(G_SQL_CWar_Buffer->Output_Buffer(G_SQL_CWar_Buffer->m_Out_Buffer))
			{
				//	처리함수에 전달.
				GF_DB_CWar_Proc((GF_CWar_DB_ST *)G_SQL_CWar_Buffer->m_Out_Buffer);
				//	사용버퍼 초기화
				ZeroMemory(G_SQL_CWar_Buffer->m_Out_Buffer,G_SQL_CWar_Buffer->m_Buffer_Size);
				//	버퍼 다시 확인.
				SetEvent(G_SQL_CWar_Thread->Event_H);
			}
		}
	}
	return (0);
}

////	클랜전 기록.
BOOL	GF_DB_CWar_Insert(DWORD p_WarType,DWORD p_WarWin,__int64 p_Money,DWORD p_AClan,DWORD p_BClan, char* pComment)
{
	if(p_WarType == 0) return false;
	if(p_WarType > 4) return false;
	
	//	진입
	EnterCriticalSection(&g_ClanWar_DBLOCK);

	//	데이터 구조체 설정
	GF_CWar_DB_ST	Temp_ST;
	ZeroMemory(&Temp_ST,sizeof(GF_CWar_DB_ST));

	//	데이터 넣기
	Temp_ST.m_Server_ID = G_Server_ID;		// 서버 번호
	Temp_ST.m_Channel_ID = G_SV_Channel;	// 채널 번호
	Temp_ST.m_WarType = p_WarType;
	Temp_ST.m_WarWin = p_WarWin;
	Temp_ST.m_Money = p_Money;
	Temp_ST.m_ATeam_ID = p_AClan;
	Temp_ST.m_BTeam_ID = p_BClan;
	Temp_ST.m_pComment = pComment;
	//	클랜 번호에 해당 되는 클랜이름 저장.
	for(DWORD i = 0; i < g_Clan_Max_List; i++)
	{
		if(Temp_ST.m_ATeam_ID > 0)
		{
			//	클랜 비교
			if(g_Clan_List[i].m_CLAN.m_dwClanID == Temp_ST.m_ATeam_ID)
			{
				memcpy(Temp_ST.m_ATeam_Name,g_Clan_List[i].m_ClanNAME,24);
			}
		}
		if(Temp_ST.m_BTeam_ID > 0)
		{
			//	클랜 비교
			if(g_Clan_List[i].m_CLAN.m_dwClanID == Temp_ST.m_BTeam_ID)
			{
				memcpy(Temp_ST.m_BTeam_Name,g_Clan_List[i].m_ClanNAME,24);
			}
		}
	}

	//	기록.
	GF_DB_CWar_Insert(&Temp_ST);
	
	LeaveCriticalSection(&g_ClanWar_DBLOCK);
	return true;
}

////	클랜전 기록...
BOOL	GF_DB_CWar_Insert(GF_CWar_DB_ST *p_St)
{
	//	버퍼 및 전달값확인
	if(G_SQL_CWar_Buffer == NULL) return false;
	if(G_SQL_CWar_Thread == NULL) return false;
	if(G_SQL_CWar == NULL) return false;
	if(p_St == NULL) return false;

	//	버퍼 연결.
	ZeroMemory(G_SQL_CWar_Buffer->m_In_Buffer,G_SQL_CWar_Buffer->m_Buffer_Size);
	memcpy(G_SQL_CWar_Buffer->m_In_Buffer,p_St,sizeof(GF_CWar_DB_ST));

	//	버퍼에 넣기
	G_SQL_CWar_Buffer->Input_Buffer(G_SQL_CWar_Buffer->m_In_Buffer);
	//	처리 이벤트 호출
	SetEvent(G_SQL_CWar_Thread->Event_H);
	return true;
}

////	DB 처리 함수
BOOL	GF_DB_CWar_Proc(GF_CWar_DB_ST *p_ST)
{
	//	전달 데이터 확인
	if(p_ST == NULL) return false;
	if(G_SQL_CWar == NULL) return false;

	//	데이터 처리.

	//	DB Insert 처리
	char	Temp_S[1024];
	ZeroMemory(Temp_S,1024);

	//	DB 핸들 얻기
	HSTMT	Temp_HSTMT = NULL;
	G_SQL_CWar->Alloc(&Temp_HSTMT);
	if(Temp_HSTMT == NULL) return false;

	//	Insert 문장 작성하기
	if( p_ST->m_pComment == NULL )
	{
		wsprintf(Temp_S,"INSERT INTO tbl_ClanWar (nServerNo,nWarType,nWarWin,nMoney,nAClanID,txtAClanName,nBClanID,txtBClanName,nChannelNo) VALUES (%d,%d,%d,%I64d,%d,'%s',%d,'%s',%d)",
			p_ST->m_Server_ID,p_ST->m_WarType,p_ST->m_WarWin,p_ST->m_Money,
			p_ST->m_ATeam_ID,p_ST->m_ATeam_Name,p_ST->m_BTeam_ID,p_ST->m_BTeam_Name,
			p_ST->m_Channel_ID);
	}
	else
	{
		wsprintf(Temp_S,"INSERT INTO tbl_ClanWar (nServerNo,nWarType,nWarWin,nMoney,nAClanID,txtAClanName,nBClanID,txtBClanName,nChannelNo,Comment) VALUES (%d,%d,%d,%I64d,%d,'%s',%d,'%s',%d, '%s')",
			p_ST->m_Server_ID,p_ST->m_WarType,p_ST->m_WarWin,p_ST->m_Money,
			p_ST->m_ATeam_ID,p_ST->m_ATeam_Name,p_ST->m_BTeam_ID,p_ST->m_BTeam_Name,
			p_ST->m_Channel_ID,
			p_ST->m_pComment);
	}

	//	해당 퀘리문 실행.
	G_SQL_CWar->Exec(Temp_S,Temp_HSTMT);
	//	할당해제.
	G_SQL_CWar->Free(Temp_HSTMT);

	return true;
}

//#endif