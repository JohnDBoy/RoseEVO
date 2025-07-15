
/*
	클랜 필드 관리 함수 추가 
	2006년 10월 16일 : 작성자 김영환
*/

#include "StdAfx.h"
#include "ZoneLIST.h"
#include "F_GDI_List.h"

////	클랜 필드 관리 전역 변수
CF_Setting	g_CF_Set[MAX_CF_SET_N];

////	클랜 필드 플래이 상태 변수 배열 번호
BOOL	g_CF_Play_TF = false;
int		g_CF_Play_N = 0;

////	클랜 필드 오픈 정보 보기
HWND	g_hList_CField = NULL;

//	크리티컬 세션 처리
CRITICAL_SECTION	g_CF_Play_LOCK;

////	존 리스트
extern CZoneLIST		*g_pZoneLIST;


////	클랜 필트 크리티컬 설정
BOOL	GF_CF_CRITICAL_Set(bool pType,HWND	p_HWND)
{
	//	크리티컬 세션, 처리
	if(pType)
	{
		InitializeCriticalSection( &g_CF_Play_LOCK );

		//	리스트 생성
		g_hList_CField = FL_Create_List(p_HWND,"Clan Field Event View",200,0,300,150);
		ShowWindow(g_hList_CField,SW_HIDE);
		if(g_hList_CField == NULL) return false;
		//	리스트에 타이틀 넣기
		FL_Set_List_Col(g_hList_CField,0,50,"NO");
		FL_Set_List_Col(g_hList_CField,1,50,"Open Start");
		FL_Set_List_Col(g_hList_CField,2,50,"Open End");
		FL_Set_List_Col(g_hList_CField,3,50,"Close Notice ");
		FL_Set_List_Col(g_hList_CField,4,50,"Close Time");
	}
	else
	{
		DeleteCriticalSection(&g_CF_Play_LOCK);
		//	리스트 삭제
		DestroyWindow(g_hList_CField);
	}
	return true;
}

////	클랜 필드 설정 정보 읽기
BOOL	GF_CF_Set_INI_Load()
{
	//	구조체 초기화
	ZeroMemory(g_CF_Set,MAX_CF_SET_N*sizeof(CF_Setting));

	//	대상 파일 읽기
	char	Temp_File[MAX_PATH];
	char	Temp_Dir[MAX_PATH];
	ZeroMemory(Temp_File,MAX_PATH);
	ZeroMemory(Temp_Dir,MAX_PATH);
	//	현재 경로 얻기
	GetCurrentDirectory(MAX_PATH,Temp_Dir);
	wsprintf(Temp_File,"%s\\SHO_GS_CField.ini",Temp_Dir);

	// INI 파일 존재 유무 (-_-;;)
	FILE* fp = NULL;
	if( (fp = fopen( Temp_File, "rb" )) )
	{ // 존재하는 파일
		fclose(fp);
	}
	else
	{ // NULL
		return false;
	}

	//	임시 버퍼
	char	Temp_String[30];
	char	t_Title[30];
	char	t_Name[30];
	int		Temp_i = 0;
	//	INI 파일에서 읽어 들인다.
	for(int i = 0 ; i < MAX_CF_SET_N; i++)
	{
		//	이벤트 번호 넣기
		ZeroMemory(t_Title,30);
		wsprintf(t_Title,"CF_EVENT_%d",i+1);

		//	파일에서 읽기
		ZeroMemory(Temp_String,30);
		GetPrivateProfileString(t_Title,"CF_NPC_NO", "0", Temp_String,30, Temp_File);
		Temp_i = atoi(Temp_String);
		if(Temp_i < 0) Temp_i = 0;
		g_CF_Set[i].m_NPC_No = (DWORD)Temp_i;	// NPC 번호
		g_CF_Set[i].m_NPC = NULL;				// NPC 포인터
		
		ZeroMemory(Temp_String,30);
		GetPrivateProfileString(t_Title,"CF_NPC_ZONE", "0", Temp_String,30, Temp_File);
		Temp_i = atoi(Temp_String);
		if(Temp_i < 0) Temp_i = 0;
		g_CF_Set[i].m_NPC_Zone = (DWORD)Temp_i;	// NPC가 있는 존.

		ZeroMemory(Temp_String,30);
		GetPrivateProfileString(t_Title,"CF_OPEN_S_TIME", "0", Temp_String,30, Temp_File);
		Temp_i = atoi(Temp_String);
		if(Temp_i < 0) Temp_i = 0;
		g_CF_Set[i].m_Start_Time[0] = (DWORD)((Temp_i/100)*60 + Temp_i%100);	// 오픈 시작 시간
		
		ZeroMemory(Temp_String,30);
		GetPrivateProfileString(t_Title,"CF_OPEN_E_TIME", "0", Temp_String,30, Temp_File);
		Temp_i = atoi(Temp_String);
		if(Temp_i < 0) Temp_i = 0;
		g_CF_Set[i].m_Start_Time[1] = (DWORD)((Temp_i/100)*60 + Temp_i%100);	// 오픈 종료 시간
		
		ZeroMemory(Temp_String,30);
		GetPrivateProfileString(t_Title,"CF_CLOSE_S_TIME", "0", Temp_String,30, Temp_File);
		Temp_i = atoi(Temp_String);
		if(Temp_i < 0) Temp_i = 0;
		g_CF_Set[i].m_End_Time[0] = (DWORD)((Temp_i/100)*60 + Temp_i%100);	// 닫기 시작 시간
		
		ZeroMemory(Temp_String,30);
		GetPrivateProfileString(t_Title,"CF_CLOSE_E_TIME", "0", Temp_String,30, Temp_File);
		Temp_i = atoi(Temp_String);
		if(Temp_i < 0) Temp_i = 0;
		g_CF_Set[i].m_End_Time[1] = (DWORD)((Temp_i/100)*60 + Temp_i%100);	// 닫기 종료 시간

		////	귀환 존 및 위치 읽기.
		DWORD j;
		for(j = 0 ; j < 5; j++)
		{
			ZeroMemory(t_Name,30);
			wsprintf(t_Name,"CF_RETURN_ZONE_%d",j+1);

			ZeroMemory(Temp_String,30);
			GetPrivateProfileString(t_Title,t_Name, "0", Temp_String,30, Temp_File);
			Temp_i = atoi(Temp_String);
			if(Temp_i < 0) Temp_i = 0;
			g_CF_Set[i].m_ReturnNO[j] = (WORD)Temp_i;	// 귀환 존
		}
		for(j = 0 ; j < 5; j++)
		{
			ZeroMemory(t_Name,30);
			wsprintf(t_Name,"CF_RETURN_POS_%d_X",j+1);

			ZeroMemory(Temp_String,30);
			GetPrivateProfileString(t_Title,t_Name, "0", Temp_String,30, Temp_File);
			Temp_i = atoi(Temp_String);
			if(Temp_i < 0) Temp_i = 0;
			g_CF_Set[i].m_ReturnPos[j].x = (float)Temp_i;	// 귀환 위치 X
			
			ZeroMemory(t_Name,30);
			wsprintf(t_Name,"CF_RETURN_POS_%d_Y",j+1);

			ZeroMemory(Temp_String,30);
			GetPrivateProfileString(t_Title,t_Name, "0", Temp_String,30, Temp_File);
			Temp_i = atoi(Temp_String);
			if(Temp_i < 0) Temp_i = 0;
			g_CF_Set[i].m_ReturnPos[j].y = (float)Temp_i;	// 귀환 위치 Y
		}
	}
	//	NPC 찾기
	GF_CF_INI_NPC_Find();
	//	상태값 초기화.
	g_CF_Play_TF = false;
	//	타임 프로시저 초기 호출
	GF_CF_Time_Proc();

	//	리스트 뷰 보기
	GF_CF_Event_View();
	return true;
}

////	클랜 필드 관리 NPC 찾기 함수
BOOL	GF_CF_INI_NPC_Find()
{
	////	존 리스트 확인
	if(!g_pZoneLIST) return false;
	
	//	NPC 포인터 찾기
	for(int i = 0 ; i < MAX_CF_SET_N;i++)
	{
		if(g_CF_Set[i].m_NPC_No > 0)
		{
			g_CF_Set[i].m_NPC = (LPVOID)g_pZoneLIST->Get_LocalNPC(g_CF_Set[i].m_NPC_No);
		}
	}
	// NPC 포인터가 NULL 경우 사용하지 않는 클랜 필드 이벤트이다.
	return true;
}

////	클랜 필드 이벤트 뷰
BOOL	GF_CF_Event_View()
{
	if(g_hList_CField == NULL) return false;
	//	클랜 필드 이벤트 정보 넣기.
	char	Temp_S[100];
	//	데이터 넣기
	for(int i = 0; i < MAX_CF_SET_N; i++)
	{
		//	번호 넣기
		wsprintf(Temp_S,"%d",i+1);
		FL_Set_List_Data(g_hList_CField,i,0,Temp_S);
		if(g_CF_Set[i].m_NPC == NULL)
		{
			// 설정 안된 정보
			wsprintf(Temp_S,"0");
			FL_Set_List_Data(g_hList_CField,i,1,Temp_S);
			FL_Set_List_Data(g_hList_CField,i,2,Temp_S);
			FL_Set_List_Data(g_hList_CField,i,3,Temp_S);
			FL_Set_List_Data(g_hList_CField,i,4,Temp_S);
		}
		else
		{
			//	시간 정보 출력
			wsprintf(Temp_S,"%#02d:%#02d",g_CF_Set[i].m_Start_Time[0]/60,g_CF_Set[i].m_Start_Time[0]%60);
			FL_Set_List_Data(g_hList_CField,i,1,Temp_S);
			wsprintf(Temp_S,"%#02d:%#02d",g_CF_Set[i].m_Start_Time[1]/60,g_CF_Set[i].m_Start_Time[1]%60);
			FL_Set_List_Data(g_hList_CField,i,2,Temp_S);
			wsprintf(Temp_S,"%#02d:%#02d",g_CF_Set[i].m_End_Time[0]/60,g_CF_Set[i].m_End_Time[0]%60);
			FL_Set_List_Data(g_hList_CField,i,3,Temp_S);
			wsprintf(Temp_S,"%#02d:%#02d",g_CF_Set[i].m_End_Time[1]/60,g_CF_Set[i].m_End_Time[1]%60);
			FL_Set_List_Data(g_hList_CField,i,4,Temp_S);
		}
	}
	return true;
}

////	클랜 필드 관리 타입 함수
BOOL	GF_CF_Time_Proc()
{
	//	30초에 한번씩 실행된다.
	//	시스템 시간을 얻는다.
	SYSTEMTIME	t_STime;
	GetLocalTime(&t_STime);

	//	분단위로 타임 처리.
	DWORD	t_Time = t_STime.wHour*60 + t_STime.wMinute;

	//	NPC 시작 처리.
	BOOL	t_Start_TF = false;
	int		t_Start_N = 0;
	//	클랜필드 플레이중 확인
	BOOL	t_Play_TF = false;
	int		t_Play_N = 0;

	//	게임 진행 예약을 확인한다.
	for(int i = 0 ; i < MAX_CF_SET_N; i++)
	{
		//	NPC 확인
		if((g_CF_Set[i].m_NPC != NULL)&&(g_CF_Set[i].m_NPC_No > 0))
		{
			//	오픈 시간에서 오픈 종료 시간 체크
			if((g_CF_Set[i].m_Start_Time[0] <= t_Time)&&(g_CF_Set[i].m_Start_Time[1] >= t_Time))
			{
				//	NPC 변수 1로 변경 해야한다.
				t_Start_TF = true;
				t_Start_N = i;
			}
			//	종료 대기 시간에서 종료 시간 체크
			if((g_CF_Set[i].m_End_Time[0] <= t_Time)&&(g_CF_Set[i].m_End_Time[1] > t_Time))
			{
				//	종료 대기 통보 한다.
			}
			//	플레이 시간 체크.
			if((g_CF_Set[i].m_Start_Time[0] <= t_Time)&&(g_CF_Set[i].m_End_Time[1] >= t_Time))
			{
				// 클랜 필드 플레이 시간.
				t_Play_TF = true;
				t_Play_N = i;
			}
		}
	}
	
	//	NPC 변수 변경 확인
	GF_CF_NPC_Var_Set(t_Start_TF,t_Start_N);

	//	플레이 진행 변수 확인.
	if(g_CF_Play_TF != t_Play_TF)
	{
		//	현재 플레이 상태 값과 이전 상태 값과 비교.
		g_CF_Play_TF = t_Play_TF;
		g_CF_Play_N = t_Play_N;	// 배열 위치 저장.
	}

	//	게임이 종료 된 경우 클랜 필드의 유저들 워프 시킨다.
	if(!g_CF_Play_TF)
	{
		//	모두 워프 시킴.
		GF_CF_End_Warp();
	}

	return true;
}

////	클랜 필드 관리 NPC 변수 변경.
//	m_nAI_VAR[0] 값이 0이면 입장 불가. m_nAI_VAR[0] 값이 1이면 입장 가능.
BOOL	GF_CF_NPC_Var_Set(BOOL p_Type,int p_List)
{
	//	전달값 확인
	if(MAX_CF_SET_N <= p_List) return false;
	//	해당 이벤트 정보의 NPC 확인한다.
	if(g_CF_Set[p_List].m_NPC == NULL) return false;

	//	전달된 p_Type 값에 의하여 m_nAI_VAR[0] 값을 변경한다.
	CObjNPC	*Temp_NPC = (CObjNPC *)g_CF_Set[p_List].m_NPC;
	if(p_Type)
	{
		//	m_nAI_VAR[0] 값이 0이면 1로 설정.
		if(0 == Temp_NPC->Get_ObjVAR(0))
			Temp_NPC->Set_ObjVAR(0,1);
	}
	else
	{
		//	m_nAI_VAR[0] 값이 1이면 0로 설정.
		if(0 < Temp_NPC->Get_ObjVAR(0))
			Temp_NPC->Set_ObjVAR(0,0);
	}
	return true;
}

////	클랜 필드 종료 통보 처리
BOOL	GF_CF_End_Time_Send()
{
	return true;
}

////	클랜 필드 게임 종료 워프 시킴
BOOL	GF_CF_End_Warp()
{
	//	전역 변수 확인
	if(g_pZoneLIST == NULL) return false;

    //	클랜 필드 존의 접속 유저 찾기
//	classDLLNODE< CGameOBJ* > *pObjLIST;
//	classDLLNODE< CGameOBJ* > *pObjNODE;
	for(int i = 11; i < 14; i++)
	{
		// 접속 수 확인.
		if(g_pZoneLIST->GetZONE(i)->Get_UserCNT() > 0)
		{
			//	모든 유저 워프 시킨다.
			g_pZoneLIST->GetZONE(i)->All_User_Warp(0);
		}
	}
		
	return true;
}

////	클랜 필드 유저 워프 시킴
BOOL	GF_CF_End_User_Warp(classUSER *p_User,bool Type)
{
	//	전달 인자 확인
	if(p_User == NULL) return false;
	//	전역 변수 확인.
	if(g_CF_Play_N >= MAX_CF_SET_N) return false;
	if(g_CF_Set[g_CF_Play_N].m_NPC == NULL) return false;
	if (!( p_User->m_nZoneNO >= 11 && p_User->m_nZoneNO <= 13) ) return false;

	EnterCriticalSection(&g_CF_Play_LOCK);

	//	귀환 위치 선정.
	int	t_Pos_N = rand()%5;

	if(Type)
	{
		//	워프 시킴
		//	해당 유저 게임 종료 위치로 보냄.
		p_User->Reward_WARP(g_CF_Set[g_CF_Play_N].m_ReturnNO[t_Pos_N],g_CF_Set[g_CF_Play_N].m_ReturnPos[t_Pos_N]);
		//	해당 유저 팀 번호 변경.
		p_User->SetCur_TeamNO(2);
#ifdef __CLAN_WAR_SET
		p_User->Set_TeamNoFromClanIDX(0);	// 일반 변경.
#endif
	}
	else
	{
		//	워프 위치만 설정.
		p_User->m_nZoneNO = g_CF_Set[g_CF_Play_N].m_ReturnNO[t_Pos_N];
		p_User->m_PosCUR = g_CF_Set[g_CF_Play_N].m_ReturnPos[t_Pos_N];
	}

	LeaveCriticalSection(&g_CF_Play_LOCK);
	return true;
}