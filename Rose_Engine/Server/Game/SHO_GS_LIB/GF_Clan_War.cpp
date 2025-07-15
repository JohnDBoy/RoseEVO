
/*
	작성자 김영환
*/

// #ifdef __CLAN_WAR_SET			// 클랜전 추가 정의
#include <stdio.h>

#include "GF_Define.h"
#include "GF_Data.h"

/*
	클랜전 관리/처리 함수
*/
int	CW_Warp_AX,CW_Warp_AY,CW_Warp_BX,CW_Warp_BY;
int	CW_Revival_AX,CW_Revival_AY,CW_Revival_BX,CW_Revival_BY;

////	클랜전 정보 초기화
BOOL	GF_Clan_War_Init()
{
	//	클랜전 리스트
	ZeroMemory(g_ClanWar_List,DF_MAX_CWAR_LIST*sizeof(GF_ClanWar_Set));
	ZeroMemory(G_CW_BAT_MONEY,3*8);	// 배팅 금액
	ZeroMemory(G_CW_MAX_LEVEL,3*4);	// 제한 레벨.
	ZeroMemory(G_CW_MAX_USER_NO,2*3*4);	// 인원 제한

	//	정보 읽기
	if( !GF_Clan_War_Ini_Read() )
		return false;

	//	클랜전 필드 보스몹 초기화
	if( !GF_CW_MOB_Init() )
		return false;

	return true;
}

////	클랜전 정보 ini 읽기
BOOL	GF_Clan_War_Ini_Read()
{
	// ini 파일 읽기.
	char	Temp_File[MAX_PATH];
	ZeroMemory(Temp_File,MAX_PATH);
	wsprintf( Temp_File, "%s\\SHO_GS_DATA.ini", G_Main_Dir );

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

	// 워프 위치
	// 워프 위치
	CW_Warp_AX = Get_ini_Read(Temp_File,"CLANWAR_ZONE_WARP_A","CW_POS_X");
	CW_Warp_AY = Get_ini_Read(Temp_File,"CLANWAR_ZONE_WARP_A","CW_POS_Y");
	CW_Warp_BX = Get_ini_Read(Temp_File,"CLANWAR_ZONE_WARP_B","CW_POS_X");
	CW_Warp_BY = Get_ini_Read(Temp_File,"CLANWAR_ZONE_WARP_B","CW_POS_Y");
	// 부활 위치
	CW_Revival_AX = Get_ini_Read(Temp_File,"CLANWAR_ZONE_REVIVAL_A","CW_POS_X");
	g_APosREVIVAL.x = CW_Revival_AX;
	CW_Revival_AY = Get_ini_Read(Temp_File,"CLANWAR_ZONE_REVIVAL_A","CW_POS_Y");
	g_APosREVIVAL.y = CW_Revival_AY;
	CW_Revival_BX = Get_ini_Read(Temp_File,"CLANWAR_ZONE_REVIVAL_B","CW_POS_X");
	g_BPosREVIVAL.x = CW_Revival_BX;
	CW_Revival_BY = Get_ini_Read(Temp_File,"CLANWAR_ZONE_REVIVAL_B","CW_POS_Y");
	g_BPosREVIVAL.y = CW_Revival_BY;
	//	귀환 위치
	int	Temp_z = Get_ini_Read(Temp_File,"CLANWAR_RETURN","CW_ZONE");
	int	Temp_x = Get_ini_Read(Temp_File,"CLANWAR_RETURN","CW_POS_X");
	int	Temp_y = Get_ini_Read(Temp_File,"CLANWAR_RETURN","CW_POS_Y");

	//	클랜전 배팅 금액 설정.
	G_CW_BAT_MONEY[0] = (__int64)Get_ini_Read(Temp_File,"CLANWAR_BAT","CW_BAT_1");
	G_CW_BAT_MONEY[1] = (__int64)Get_ini_Read(Temp_File,"CLANWAR_BAT","CW_BAT_2");
	G_CW_BAT_MONEY[2] = (__int64)Get_ini_Read(Temp_File,"CLANWAR_BAT","CW_BAT_3");
	//	클랜전 게임 시간 설정 읽기
	DF_CWAR_PLAY_TIME = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_TIME","CW_PLAY_TIME");
	DF_CWAR_END_TIME = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_TIME","CW_END_TIME");

	//	클랜전 레벨 제한.
	G_CW_MAX_LEVEL[0] = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_LEVEL","CW_LEVEL_1");
	G_CW_MAX_LEVEL[1] = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_LEVEL","CW_LEVEL_2");
	G_CW_MAX_LEVEL[2] = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_LEVEL","CW_LEVEL_3");



	//	클랜전 인원 제한 읽기
	char	Temp_S[50];
	for(DWORD k = 0 ; k < 3; k++)
	{
		// (20070125:남병철) : 각 클랜전 타입에 맞게 유연한 처리가 적절히 되지 않아서 하나로 통일
		// << 클랜전 참여인원 공통 세팅 >>
		// EX) 일반적으로 해당 클랜원의 마스터 찾기와 각 타입을 구분하여 클랜전 마스터를 찾을 경우
		//     동일한 GF_Clan_Master_Fine()을 사용하게되어 각 타입 구분에 맞는 클랜원 수를 다룰 수 없다.
		//     그러므로, 각 클랜전 타입을 하나로 일치 시켜서 클랜전에 50명이 들어갈 수 있게 수정한다.
		ZeroMemory( Temp_S, 50 );
		wsprintf( Temp_S, "CW_USER_NO", k+1 );
		G_CW_MAX_USER_NO[k][0] = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_USER_NUMBER",Temp_S);
		G_CW_MAX_USER_NO[k][1] = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_USER_NUMBER",Temp_S);

		////	A팀 인원 읽기
		//ZeroMemory(Temp_S,50);
		//wsprintf(Temp_S,"CW_USER_NO_%d_A",k+1);
		//G_CW_MAX_USER_NO[k][0] = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_USER_NUMBER",Temp_S);

		////	B팀 인원 읽기
		//ZeroMemory(Temp_S,50);
		//wsprintf(Temp_S,"CW_USER_NO_%d_B",k+1);
		//G_CW_MAX_USER_NO[k][1] = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_USER_NUMBER",Temp_S);
	}
	//G_CW_MAX_USER_NO[0][0] = 50;



	if(CW_Warp_AX < 0)		MessageBox(NULL,"클랜전 정보파일 읽기 실패!","경고!!",MB_OK);

	//	클랜전 존 위치 초기화
	for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
	{
		g_ClanWar_List[i].m_ZoneNO = (WORD)(101 + i);
		//	워프 위치
		g_ClanWar_List[i].m_APosWARP.x = (float)CW_Warp_AX;
		g_ClanWar_List[i].m_APosWARP.y = (float)CW_Warp_AY;
		g_ClanWar_List[i].m_BPosWARP.x = (float)CW_Warp_BX;
		g_ClanWar_List[i].m_BPosWARP.y = (float)CW_Warp_BY;
		//	부활 위치
		g_ClanWar_List[i].m_APosREVIVAL.x = (float)CW_Revival_AX;
		g_ClanWar_List[i].m_APosREVIVAL.y = (float)CW_Revival_AY;
		g_ClanWar_List[i].m_BPosREVIVAL.x = (float)CW_Revival_BX;
		g_ClanWar_List[i].m_BPosREVIVAL.y = (float)CW_Revival_BY;
	}
	//	클랜전 복귀 위치 설정
	g_ClanWar_ReCall.m_ZoneNO = (WORD)Temp_z;
	g_ClanWar_ReCall.m_Pos.x = (float)Temp_x;
	g_ClanWar_ReCall.m_Pos.y = (float)Temp_y;

	return true;
}

////	클랜전 필드 확인
DWORD	GF_Clan_War_Zone_Is(DWORD p_Zone_N)
{
	// 확인
	for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
	{
		if(g_ClanWar_List[i].m_ZoneNO == (WORD)p_Zone_N)
		{
			ZTRACE( "클랜전 존번호:%d", i+1 );
			return (i+1);	// 존번호 반환.
		}
	}

	return 0;
}

////	클랜전 필드 팀 확인.
DWORD	GF_Clan_War_Zone_Is(DWORD p_Zone_N,DWORD p_Clan_N)
{
	// 확인
	for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
	{
		//	해당 존 확인한다.
		if(g_ClanWar_List[i].m_ZoneNO == (WORD)p_Zone_N)
		{
			//	본인 클랜이 참여중인지 확인한다.
			if(p_Clan_N == 0)
				return 1;
			else if(g_ClanWar_List[i].m_Clan_A == p_Clan_N)
				return 1000;
			else if(g_ClanWar_List[i].m_Clan_B == p_Clan_N)
				return 2000;
			return 1;
		}
	}
	return 0;
}

////	클랜전 존 게임중인 확인
BOOL	GF_Clan_War_Zone_Play_Is(DWORD p_List_N)
{
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	if(g_ClanWar_List[p_List_N - 1].m_Play_Type == 4) return true;	// 플레이중
	return false;
}

////	클랜전 필드 보스 몹 초기화
BOOL	GF_CW_MOB_Init()
{
	//	초기화
	ZeroMemory(g_CW_MOB_List,DF_MAX_CWAR_TYPE*DF_MAX_CWAR_MOB*DF_CWAR_TEAM_N*sizeof(GF_ClanWar_MOB));
	////	클랜전 보상 아이템 정보
	ZeroMemory(g_CW_Win_Gift,DF_MAX_CWAR_TYPE*sizeof(GF_CWar_Win_Gift));
	
	// ini 파일 읽기. (INI파일 존재 유무는 GF_Clan_War_Ini_Read()에서 채크 했음)
	char	Temp_File[MAX_PATH];
	ZeroMemory(Temp_File,MAX_PATH);
	wsprintf(Temp_File,"%s\\SHO_GS_DATA.ini",G_Main_Dir);

	//	임시 변수
	char	Temp_Title[30];
	char	Temp_Name[30];
	int	Temp_Int = 0;

	//	레벨 반복
	for(int i = 0 ; i < DF_MAX_CWAR_TYPE; i++)
	{
		//	팀 반복
		for(int t = 0 ; t < DF_CWAR_TEAM_N; t++)
		{
			//	타이틀 정의
			ZeroMemory(Temp_Title,30);
			if(t == 0)	wsprintf(Temp_Title,"MOB_A_%d",i+1);
			else		wsprintf(Temp_Title,"MOB_B_%d",i+1);
			//	몹개수 반복
			for(int n = 0 ;n < DF_MAX_CWAR_MOB; n++)
			{
				//	몬스터 번호
				ZeroMemory(Temp_Name,30);
				wsprintf(Temp_Name,"CW_MOB_%d",n+1);
				int iMOB_IDX = (DWORD)Get_ini_Read(Temp_File,Temp_Title,Temp_Name);
				g_CW_MOB_List[i][n][t].m_MOB_Idx = (DWORD)Get_ini_Read(Temp_File,Temp_Title,Temp_Name);
				//	위치 X
				ZeroMemory(Temp_Name,30);
				wsprintf(Temp_Name,"CW_POS_%d_X",n+1);
				g_CW_MOB_List[i][n][t].m_Pos_X = (float)Get_ini_Read(Temp_File,Temp_Title,Temp_Name);
				//	위치 Y
				ZeroMemory(Temp_Name,30);
				wsprintf(Temp_Name,"CW_POS_%d_Y",n+1);
				g_CW_MOB_List[i][n][t].m_Pos_Y = (float)Get_ini_Read(Temp_File,Temp_Title,Temp_Name);
				//	개수
				ZeroMemory(Temp_Name,30);
				wsprintf(Temp_Name,"CW_NUM_%d",n+1);
				g_CW_MOB_List[i][n][t].m_Number = (DWORD)Get_ini_Read(Temp_File,Temp_Title,Temp_Name);
				//	영역
				ZeroMemory(Temp_Name,30);
				wsprintf(Temp_Name,"CW_RANGE_%d",n+1);
				g_CW_MOB_List[i][n][t].m_Range = (DWORD)Get_ini_Read(Temp_File,Temp_Title,Temp_Name);
			}
		}
	}

	////	클랜전 보상 정보 읽어 들인다.
	ZeroMemory(Temp_File,MAX_PATH);
	wsprintf(Temp_File,"%s\\SHO_GS_ITEM.ini",G_Main_Dir);

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

	////	게임 유형별로 처리 한다.
	for(int i = 0 ; i < DF_MAX_CWAR_TYPE; i++)
	{
		//	타이틀 이름 지정.
		ZeroMemory(Temp_Title,30);
		wsprintf(Temp_Title,"CW_ITEM_%d",i+1);
		//// 아이템 정보 읽기
		for(int j = 0 ; j < DF_CWAR_WIN_ITEM_N; j++)
		{
			//	이름 지정.
			ZeroMemory(Temp_Name,30);
			wsprintf(Temp_Name,"CW_GIFT_ITEM_%d",j+1);
			Temp_Int = Get_ini_Read(Temp_File,Temp_Title,Temp_Name);
			if(Temp_Int < 0)
				g_CW_Win_Gift[i].m_Item_No[j] = 0;
			else
				g_CW_Win_Gift[i].m_Item_No[j] = (DWORD)Temp_Int;
			//	이름 지정.
			ZeroMemory(Temp_Name,30);
			wsprintf(Temp_Name,"CW_GIFT_DUP_%d",j+1);
			Temp_Int = Get_ini_Read(Temp_File,Temp_Title,Temp_Name);
			if(Temp_Int < 0)
				g_CW_Win_Gift[i].m_Item_Dup[j] = 0;
			else
				g_CW_Win_Gift[i].m_Item_Dup[j] = (WORD)Temp_Int;
		}
	}

	return true;
}

////	클랜전 방의 타입 얻기..
DWORD	GF_Get_CWar_Type(DWORD	p_Type)
{
	switch(p_Type)
	{
	case 1:
	case 2:
		//	배팅비 클래전
		return 0;
	case 3:
	case 4:
		//	참여비 클래전
		return 3;
	case 31:
	case 32:
		//	배팅비 클래전
		return 1;
	case 33:
	case 34:
		//	참여비 클래전
		return 4;
	case 61:
	case 62:
		//	배팅비 클래전
		return 2;
	case 63:
	case 64:
		//	참여비 클래전
		return 5;
	}
	return 0;
}

////	클랜전 방의 배팅 금액 설정
__int64 GF_Set_CWar_Bat(DWORD p_Clan_Inx,DWORD p_Type)
{
	__int64 return_m = 0;
	switch(p_Type)
	{
	case 1:
	case 2:
		return_m = G_CW_BAT_MONEY[0];
		break;
	case 31:
	case 32:
		return_m = G_CW_BAT_MONEY[1];
		break;
	case 61:
	case 62:
		return_m = G_CW_BAT_MONEY[2];
		break;
	}
	//	클랜 장을 찾아서 금액을 빼준다. [금액 부족시 오류 처리 필요]
	if(!GF_Clan_Master_Money_Sub(p_Clan_Inx,return_m)) return 0;
	return return_m;
}

////	클랜전 신청 취소에 대한 보상 처리...
BOOL	GF_CWar_Bat_Return(LPVOID p_User)
{
	//	전달값 확인.
	if(p_User == NULL) return false;
	classUSER *Temp_User;
	Temp_User = (classUSER *)p_User;
	//	유저의 클랜 정보 값 확인하고 보상.
	
	switch(Temp_User->m_Quests.m_nClanWarVAR[0])
	{
	case 1:
	case 2:
		Temp_User->m_Inventory.m_i64Money = Temp_User->m_Inventory.m_i64Money + G_CW_BAT_MONEY[0];
		Temp_User->Send_gsv_SET_MONEYONLY(GSV_SET_MONEY_ONLY);
		break;
	case 31:
	case 32:
		Temp_User->m_Inventory.m_i64Money = Temp_User->m_Inventory.m_i64Money + G_CW_BAT_MONEY[1];
		Temp_User->Send_gsv_SET_MONEYONLY(GSV_SET_MONEY_ONLY);
		break;
	case 61:
	case 62:
		Temp_User->m_Inventory.m_i64Money = Temp_User->m_Inventory.m_i64Money + G_CW_BAT_MONEY[2];
		Temp_User->Send_gsv_SET_MONEYONLY(GSV_SET_MONEY_ONLY);
		break;
	}
	//	관련변수 초기화
	Temp_User->m_Quests.m_nClanWarVAR[0] = 0;
	Temp_User->m_Quests.m_nClanWarVAR[6] = 0;
	Temp_User->m_Quests.m_nClanWarVAR[5] = 0;
	Temp_User->m_Quests.m_nClanWarVAR[7] = 0;
	////	수정된 유저 퀘스트 변수 내용 전달
	Temp_User->Send_gsv_Quest_VAR();
	return true;
}

////	마스터 유저의 금액을 뺀다. [클랜전 신청때 사용]
BOOL	GF_Clan_Master_Money_Sub(DWORD p_Clan_Inx,__int64 p_Money)
{
	// 클랜 리스트에서 클랜 찾기
	DWORD temp_CN = GF_Clan_Fine_List(p_Clan_Inx);
	if(temp_CN == 0) return false;
	//	클랜에서 마스서 찾기
	DWORD temp_Ms = GF_Clan_Master_Fine(temp_CN - 1);
	if(temp_Ms == 0) return false;	
	temp_Ms = temp_Ms - 1;
	//	마스터 금액을 뺀다.
	if(g_User_List[temp_Ms]->m_Inventory.m_i64Money >= p_Money)
	{
		g_User_List[temp_Ms]->m_Inventory.m_i64Money = g_User_List[temp_Ms]->m_Inventory.m_i64Money - p_Money;
		g_User_List[temp_Ms]->Send_gsv_SET_MONEYONLY(GSV_SET_MONEY_ONLY);
	}
	else
		return false;
	return true;
}

////	마스터 유저의 금액을 추가. [사용하지 않는다.]
BOOL	GF_Clan_Master_Money_Add(DWORD p_Clan_Inx,__int64 p_Money)
{
	// 클랜 리스트에서 클랜 찾기
	DWORD temp_CN = GF_Clan_Fine_List(p_Clan_Inx);
	if(temp_CN == 0) return false;
	//	클랜에서 마스서 찾기
	DWORD temp_Ms = GF_Clan_Master_Fine(temp_CN - 1);
	if(temp_Ms == 0) return false;	
	temp_Ms = temp_Ms - 1;
	//	마스터 금액을 추가
	g_User_List[temp_Ms]->m_Inventory.m_i64Money = g_User_List[temp_Ms]->m_Inventory.m_i64Money + p_Money;
	g_User_List[temp_Ms]->Send_gsv_SET_MONEYONLY(GSV_SET_MONEY_ONLY);
	return true;
}

////	마스터 유저의 클랜전 변수 변경.
BOOL	GF_Clan_Master_ClanWar_IDX(DWORD p_Clan_Inx,WORD Num,bool b_WinType,DWORD p_WarType)
{
	// 클랜 리스트에서 클랜 찾기
	DWORD temp_CN = GF_Clan_Fine_List(p_Clan_Inx);
	if(temp_CN == 0) return false;
	//	클랜에서 마스서 찾기
	DWORD temp_Ms = GF_Clan_Master_Fine(temp_CN - 1);
	if(temp_Ms == 0) 
	{
		// 마스터 대행에게 넣는다.
		temp_Ms = GF_Clan_Master_Sub_Fine(temp_CN - 1);
		if(temp_Ms > 0)
		{
			temp_Ms = temp_Ms - 1;
			//	마스터 대행에게 금액 넣기.
			g_User_List[temp_Ms]->m_Quests.m_nClanWarVAR[5] = Num;
			//	보상 아이템 예약
			if(b_WinType)
			{
				g_User_List[temp_Ms]->m_Quests.m_nClanWarVAR[7] = (short)(p_WarType+1);
			}
		}
	}
	else
	{
		temp_Ms = temp_Ms - 1;
		//	마스터 금액을 추가
		g_User_List[temp_Ms]->m_Quests.m_nClanWarVAR[5] = Num;
		//	보상 아이템 예약
		if(b_WinType)
		{
			g_User_List[temp_Ms]->m_Quests.m_nClanWarVAR[7] = (short)(p_WarType+1);
		}
	}
	return true;
}

////	지정 유저의 클랜전 보상변수 확인
BOOL	GF_Clan_Master_ClanWar_Gift(LPVOID p_User)
{
	if(p_User == NULL) return false;
	classUSER *Temp_User;
	Temp_User = (classUSER *)p_User;
	//	유저 금액 보상..
	if(Temp_User->m_Quests.m_nClanWarVAR[5] > 0)
	{
		Temp_User->m_Inventory.m_i64Money =
			Temp_User->m_Inventory.m_i64Money + Temp_User->m_Quests.m_nClanWarVAR[5]*1000;
		Temp_User->m_Quests.m_nClanWarVAR[5] = 0;
		//	채널 번호 지우기
		Temp_User->m_Quests.m_nClanWarVAR[6] = 0;
		Temp_User->Send_gsv_SET_MONEYONLY(GSV_SET_MONEY_ONLY);
	}
	//	유저 아이템 보상
	if(Temp_User->m_Quests.m_nClanWarVAR[7] > 0)
	{
		//	보상 번호 지정
		int t_int = Temp_User->m_Quests.m_nClanWarVAR[7];
		//	보상 번호 0부터 산출
		t_int = t_int - 1;
		//	보상 아이템 번호 삭제
		Temp_User->m_Quests.m_nClanWarVAR[7] = 0;
		//	보상 번호 한계확인.
		if(t_int >= DF_MAX_CWAR_TYPE) return true;	// 범위값 초과
		//	보상 아이템 지급.
		for(int i = 0 ; i < DF_CWAR_WIN_ITEM_N; i++)
		{
			//	개수 확인
			if(g_CW_Win_Gift[t_int].m_Item_Dup[i] > 0)
			{
				//	아이템 지급 한다.
				Temp_User->In_Out_Item(true,g_CW_Win_Gift[t_int].m_Item_No[i],g_CW_Win_Gift[t_int].m_Item_Dup[i]);
			}
		}
	}
	return true;
}


////	승리한 클랜윈 처리
BOOL	GF_Win_CWar_Set(LPVOID p_Room,DWORD p_Type,bool b_WinType)
{
	if(p_Room == NULL) return false;
	GF_ClanWar_Set *p_CWar_Room;
	p_CWar_Room = (GF_ClanWar_Set *)p_Room;

	////	경우에 따른 보상 선택.
	if(p_CWar_Room->m_Bat_Coin == 0) return false;
	__int64 Temp_MN = p_CWar_Room->m_Bat_Coin;

	switch(p_Type)
 	{
	case 0:
		{
			//	30% 할인. ㅋㅋ
			Temp_MN = Temp_MN - 3*Temp_MN/10;
			//	무승부
			Temp_MN = Temp_MN/2;	// 보상금 나누다.
			//	A,B 보상
			GF_Clan_Master_ClanWar_IDX(p_CWar_Room->m_Clan_A,(WORD)(Temp_MN/1000),false,p_CWar_Room->m_War_Type);
			GF_Clan_Master_ClanWar_IDX(p_CWar_Room->m_Clan_B,(WORD)(Temp_MN/1000),false,p_CWar_Room->m_War_Type);
			break;
		}
	case 1:
		{
			//	10% 할인. ㅋㅋ
			Temp_MN = Temp_MN - Temp_MN/10;
			//	A 팀 승
			GF_Clan_Master_ClanWar_IDX(p_CWar_Room->m_Clan_A,(WORD)(Temp_MN/1000),b_WinType,p_CWar_Room->m_War_Type);
			break;
		}
	case 2:
		{
			//	10% 할인. ㅋㅋ
			Temp_MN = Temp_MN - Temp_MN/10;
			//	B 팀 승
			GF_Clan_Master_ClanWar_IDX(p_CWar_Room->m_Clan_B,(WORD)(Temp_MN/1000),b_WinType,p_CWar_Room->m_War_Type);
			break;
		}
	default:
		return false;
	}
	return true;
}

////	클랜전 드롭 처리
BOOL	GF_Clan_War_Drop(DWORD p_List_N,DWORD p_Team)
{
	if(p_List_N == 0)  return false;
	p_List_N = p_List_N - 1;
	//	빈방 확인.
	if(g_ClanWar_List[p_List_N].m_CW_Npc == NULL) return false;
	if(g_ClanWar_List[p_List_N].m_Play_Type == 5) return false;	// 종료중 처리 하지 않음
	//	팀번호 확인.
	DWORD	Temp_M = 0;
	switch(p_Team)
	{
	case 0:
		{
			//	클랜전에서 처리되는 경우
			if(g_ClanWar_List[p_List_N].m_Win_Team == 1)
			{
				//	A 팀 승리
				GF_Win_CWar_Set((LPVOID)&g_ClanWar_List[p_List_N],1,true);
				Temp_M = 1;
			}
			else if(g_ClanWar_List[p_List_N].m_Win_Team == 2)
			{
				//	B 팀 승리
				GF_Win_CWar_Set((LPVOID)&g_ClanWar_List[p_List_N],2,true);
				Temp_M = 2;
			}
			else
			{
				//	비김
				GF_Win_CWar_Set((LPVOID)&g_ClanWar_List[p_List_N],0,true);
				Temp_M = 0;
			}
			break;
		}
	case 1000:
		{
			//	기권에 의한 처리
			//	B 팀 승리
			GF_Win_CWar_Set((LPVOID)&g_ClanWar_List[p_List_N],2,false);
			Temp_M = 2;
			break;
		}
	case 2000:
		{
			//	기권에 의한 처리
			//	A 팀 승리
			GF_Win_CWar_Set((LPVOID)&g_ClanWar_List[p_List_N],1,false);
			Temp_M = 1;
			break;
		}
	default:
		return false;
	}
	//	게임 종료 타임으로 변환
	g_ClanWar_List[p_List_N].m_Play_Type = 5;
	g_ClanWar_List[p_List_N].m_Play_Count = DF_CWAR_END_TIME;
	//	유저에게 방 상태 전송.
	GF_ClanWar_Join_OK_Send(p_List_N,7,0);
	//	PK 모드 오프 시킨다.
	g_pZoneLIST->Set_PK_FLAG((short)g_ClanWar_List[p_List_N].m_ZoneNO,false);
	//	몬스터 모두 삭제.
	switch(Temp_M)
	{
	case 0:
		//	모든 몬스터를 제거 한다,
		GF_ClanWar_Mob_Del2((LPVOID)&g_ClanWar_List[p_List_N],0);
		break;
	case 1:
		//	A팀 승리 B팀의 몹을 제거 한다.
		GF_ClanWar_Mob_Del2((LPVOID)&g_ClanWar_List[p_List_N],2);
		//	A팀 몹의 채력을 회복 한다.
		GF_ClanWar_MOB_HP_ReSet((LPVOID)&g_ClanWar_List[p_List_N],1);
		break;
	case 2:
		//	B팀 승리 A팀의 몹을 제거 한다.
		GF_ClanWar_Mob_Del2((LPVOID)&g_ClanWar_List[p_List_N],1);
		//	B팀 몹의 채력을 회복 한다.
		GF_ClanWar_MOB_HP_ReSet((LPVOID)&g_ClanWar_List[p_List_N],2);
		break;
	}
	return true;
}

////	클랜전 만들어진 방 찾기
DWORD	GF_Find_CWar_List(LPVOID p_Npc,DWORD p_Type)
{
	if(p_Npc == NULL) return 0;
	//	p_Type = 0, 1, 2 에 따른 배팅방 처리.
	CObjNPC	*Temp_Var;
	Temp_Var = (CObjNPC *)p_Npc;
	DWORD	t_A = 0;
	//	만들어진 방에서 들어갈 수 있는 방을 찾는다.
	for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
	{
		//	NPC 가 같은 방을 찾음[NPC 기반의 경우]

		//	수정한다. 전달 값에 배팅 금액이 필요하다.
		//	동일 배팅 금액으로 방이 만들어져 있는지 확인한다.
		//	동일 배팅 금액으로 만들어진 방이 없다면 빈방을 찾아 준다.
		if(g_ClanWar_List[i].m_CW_Npc != NULL)
		{
			//	신청 클랜이 있는 경우
			if(g_ClanWar_List[i].m_Play_Type == 1)
			{
				//	신청 금액이 같은 경우
				if(g_ClanWar_List[i].m_War_Type == p_Type)
				{
					//	한번더 검색 : A팀 클랜의 클랜장 접속 확인 필요
					//	A 클랜의 리스트 위치를 찾는다.
					t_A = GF_Clan_Fine_List(g_ClanWar_List[i].m_Clan_A);
					if(t_A > 0)
					{
						t_A = t_A - 1;
						// A 팀 마스터 찾기
						if(0 < GF_Clan_Master_Fine(t_A))
						{
							//	B 팀에 배속 시킴
							return i+1;
						}
					}
				}
			}
		}
	}
	//	빈방을 찾는다.
	return 0;
}

////	클랜전 만들어진 방 찾고 : 해당 방에 이긴 팀 설정.
DWORD	GF_Find_CWar_List2(DWORD p_Clan_ID)
{
	for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
	{
		if(g_ClanWar_List[i].m_CW_Npc != NULL)
		{
			if(g_ClanWar_List[i].m_Clan_A == p_Clan_ID)
			{
				if(g_ClanWar_List[i].m_Win_Team == 0)
				{
					g_ClanWar_List[i].m_Win_Team = 1;
					return i+1;
				}
				else return 0;
			}
			if(g_ClanWar_List[i].m_Clan_B == p_Clan_ID)
			{
				if(g_ClanWar_List[i].m_Win_Team == 0)
				{
					g_ClanWar_List[i].m_Win_Team = 2;
					return i+1;
				}
				else return 0;
			}
		}
	}
	return 0;
}

////	클랜전 빈방 찾기
DWORD	GF_Find_CWar_Void(LPVOID p_Npc)
{
	if(p_Npc == NULL) return 0;
	CObjNPC	*Temp_Var;
	Temp_Var = (CObjNPC *)p_Npc;
	for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
	{
		if(g_ClanWar_List[i].m_CW_Npc == NULL)	return i+1;
		if(g_ClanWar_List[i].m_Play_Type == 0)
		{
			g_ClanWar_List[i].m_CW_Npc = NULL;
			return i+1;
		}
	}
	return 0;
}

////	클랜전방 지우기
BOOL	GF_Del_CWar_List(LPVOID p_Npc)
{
	if(p_Npc == NULL) return false;
	CObjNPC	*Temp_Var;
	Temp_Var = (CObjNPC *)p_Npc;
	for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
	{
		if(g_ClanWar_List[i].m_CW_Npc == Temp_Var)
		{
			GF_Del_CWar_List2(i);
			return true;
		}
	}
	return false;
}
BOOL	GF_Del_CWar_List2(DWORD	p_Num)
{
	//	우선 NPC 변수 초기화... 나중에 바꾸자.
	if(g_ClanWar_List[p_Num].m_CW_Npc != NULL)
	{
		//	NPC 변수 초기화	, 상태값을 전송한다.
		g_ClanWar_List[p_Num].m_CW_Npc->Set_ObjVAR(0,0);
		g_ClanWar_List[p_Num].m_CW_Npc->Set_ObjVAR(1,0);
		g_ClanWar_List[p_Num].m_CW_Npc->Set_ObjVAR(2,0);
		g_ClanWar_List[p_Num].m_CW_Npc->Set_ObjVAR(3,0);
	}
	ZeroMemory(&g_ClanWar_List[p_Num],sizeof(GF_ClanWar_Set));	
	//	클랜전 존 위치 초기화
	g_ClanWar_List[p_Num].m_ZoneNO = (WORD)(101 + p_Num);
	g_ClanWar_List[p_Num].m_APosWARP.x = (float)CW_Warp_AX;
	g_ClanWar_List[p_Num].m_APosWARP.y = (float)CW_Warp_AY;
	g_ClanWar_List[p_Num].m_BPosWARP.x = (float)CW_Warp_BX;
	g_ClanWar_List[p_Num].m_BPosWARP.y = (float)CW_Warp_BY;
	//	갱신
	GF_WLV_Add(p_Num);
	return true;
}

////	클랜전에 신청 취소 처리
BOOL	GF_Clan_War_Cancel(DWORD p_Num)
{
	//	클랜 번호 확인.
	if(g_Clan_List[p_Num].m_CLAN.m_dwClanID == 0) return false;

	//	생성된 방에 클랜 번호 확인
	for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
	{
		//	신청 중신지 확인
		if(g_ClanWar_List[i].m_CW_Npc != NULL)
		{
			if(g_ClanWar_List[i].m_Play_Type == 1)
			{
				//	신청 대기중 : 단순 포기
				if(g_ClanWar_List[i].m_Clan_A == g_Clan_List[p_Num].m_CLAN.m_dwClanID)
				{
					// 해당방 초기화.
					GF_Del_CWar_List2(i);
					return true;
				}
			}
			else if(g_ClanWar_List[i].m_Play_Type > 1)
			{
				////	게임 진행중 : 게임 드롭 처리
				if(g_ClanWar_List[i].m_Clan_A == g_Clan_List[p_Num].m_CLAN.m_dwClanID)
				{
					//	드럽 처리
					if(GF_Clan_War_Drop(i+1,1000))
					{
						//	해당 팀의 퀘스트 처리
						GF_ClanWar_Quest_DropProc(i+1,1000);
						//	상대팀 클랜장에게 기권승 메시지 전송
						GF_ClanWar_GiveUP(i+1,1000);
					}
					return true;
				}
				else if(g_ClanWar_List[i].m_Clan_B == g_Clan_List[p_Num].m_CLAN.m_dwClanID)
				{
					//	드럽 처리
					if(GF_Clan_War_Drop(i+1,2000))
					{
						//	해당 팀의 퀘스트 처리
						GF_ClanWar_Quest_DropProc(i+1,2000);
						//	상대팀 클랜장에게 기권승 메시지 전송
						GF_ClanWar_GiveUP(i+1,2000);
					}
					return true;
				}
			}
		}
	}
	return false;
}

////	클랜전 신청 처리
BOOL	GF_Clan_War_Input(LPVOID p_Npc,DWORD p_Clan_Inx,DWORD p_LV, DWORD p_Type, LPVOID pClanUser)
{
	if(p_Npc == NULL) return false;
	if( pClanUser == NULL ) return false;
	if(p_Clan_Inx == 0) return false;
	if(p_Type == 0) return false;

	//	진입
	EnterCriticalSection(&g_ClanWar_LOCK);

	CObjNPC	*Temp_Var;
	classUSER* pUser = NULL;
	Temp_Var = (CObjNPC *)p_Npc;
	pUser = (classUSER*)pClanUser;		// 클랜전 신청자

	//	진입 LV 확인.
	if(!GF_Clan_War_Check_LV(p_LV,GF_Get_CWar_Type(p_Type)))
		return false;

	//	NPC 주소를 기반으로 빈방을 찾는다.
	//	NPC 등록된 방에서 빈자리 찾기 부분 포함 시킴
	DWORD	Temp_N = GF_Find_CWar_List(p_Npc,GF_Get_CWar_Type(p_Type));
	__int64	Temp_MM = 0;
	if(Temp_N == 0)
	{
		//	빈방을 찾아야함
		Temp_N = GF_Find_CWar_Void(p_Npc);
		if(Temp_N == 0)
		{
			// 클랜전 신청시 방 부족
			pUser->Send_gsv_CLANWAR_Err(CWAR_TOO_MANY_CWAR_PROGRESS);
			LeaveCriticalSection(&g_ClanWar_LOCK);
			return false;
		}	// 빈방 없음

		//	배팅 금액 확인.
		Temp_MM = GF_Set_CWar_Bat(p_Clan_Inx,p_Type);
		if(Temp_MM == 0)
		{
			LeaveCriticalSection(&g_ClanWar_LOCK);
			return false;
		}
		//	배열 순서로 변경.
		Temp_N = Temp_N - 1;
		g_ClanWar_List[Temp_N].m_CW_Npc = Temp_Var;
		g_ClanWar_List[Temp_N].m_Play_Type = 1;
		g_ClanWar_List[Temp_N].m_Play_Count = 0;
		g_ClanWar_List[Temp_N].m_War_Type = GF_Get_CWar_Type(p_Type);
		//	배팅 금액 넣기
		g_ClanWar_List[Temp_N].m_Bat_Coin = Temp_MM;
		g_ClanWar_List[Temp_N].m_Win_Team = 0;
		//	빈방인 경우 A팀에 p_Clan_Inx 값과 p_Type 값을 넣는다.
		g_ClanWar_List[Temp_N].m_Clan_A = p_Clan_Inx;	// 클랜 번호.
		g_ClanWar_List[Temp_N].m_Clan_B = 0;
		g_ClanWar_List[Temp_N].m_Type_A = p_Type;
		g_ClanWar_List[Temp_N].m_Type_B = 0;

		//	A팀 신청 DB 저장.
		GF_DB_CWar_Insert(1,0,Temp_MM,p_Clan_Inx,0,NULL);
	}
	else
	{
		//	배열 값으로 변경 : 만들어진방이 있음
		Temp_N = Temp_N-1;
		//	빈방이 아닌 경우 B팀의 공간을 확인한다.
		if(g_ClanWar_List[Temp_N].m_Play_Type == 1)
		{
			//	배팅 금액 확인.
			Temp_MM = GF_Set_CWar_Bat(p_Clan_Inx,p_Type);
			if(Temp_MM == 0) 
			{
				LeaveCriticalSection(&g_ClanWar_LOCK);
				return false;
			}
			g_ClanWar_List[Temp_N].m_Play_Count = DF_CWAR_PLAY_TIME;
			g_ClanWar_List[Temp_N].m_Play_Type = 2;
			g_ClanWar_List[Temp_N].m_War_Type = GF_Get_CWar_Type(p_Type);
			g_ClanWar_List[Temp_N].m_Win_Team = 0;
			//	배팅 금액 넣기
			g_ClanWar_List[Temp_N].m_Bat_Coin = g_ClanWar_List[Temp_N].m_Bat_Coin + Temp_MM;
			//	B팀에 p_Clan_Inx 값과 p_Type 값을 넣는다..
			g_ClanWar_List[Temp_N].m_Clan_B = p_Clan_Inx;
			g_ClanWar_List[Temp_N].m_Type_B = p_Type;
			//	값을 넣을때 p_Type = 1:2 , 31:32, 61:62  비교해야한다.
			
			//	B팀 신청 DB 저장.
			GF_DB_CWar_Insert(2,0,Temp_MM,0,p_Clan_Inx,NULL);

			//	해당 클랜장에게 입장 메시지 보냄.
			GF_ClanWar_Join_OK_Send(Temp_N,1,0);

			//	게임 시작된 것으로 본다.
			//	클랜전 시작 DB 저장.
			GF_DB_CWar_Insert(3,0,g_ClanWar_List[Temp_N].m_Bat_Coin,g_ClanWar_List[Temp_N].m_Clan_A,g_ClanWar_List[Temp_N].m_Clan_B,NULL);
		}
	}
	//	갱신
	GF_WLV_Add(Temp_N);

	LeaveCriticalSection(&g_ClanWar_LOCK);
	return true;
}


////	클랜전 참여 LV 확인.
BOOL	GF_Clan_War_Check_LV(DWORD p_LV,DWORD p_Type)
{
	if(p_Type > 2) return false;
	if(p_LV <= G_CW_MAX_LEVEL[p_Type])	return true;
	return false;
}

////	클랜 리스트에서 찾기
DWORD	GF_Clan_Fine_List(DWORD p_Clan_ID)
{
	//	메모리 확인.
	if(g_Clan_List == NULL) return 0;
	//	해당 클랜 확인.
	for(DWORD i = 0 ; i < g_Clan_Max_List; i++)
	{
		//	클랜 번호 확인
		if(g_Clan_List[i].m_CLAN.m_dwClanID == p_Clan_ID)
			return i+1;
	}
	return 0;
}

////	클랜의 마스터를 찾는다.
DWORD	GF_Clan_Master_Fine(DWORD p_Clan)
{
	DWORD	Temp_ID = 0;
	// for(DWORD i = 0; i < 15;i++)
	// (20070125:남병철) : 클랜원 수치의 15명 하드코딩을 CW_USER_NO의 수치로 변경 (최대 50명까지 가능)
	for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0];i++ )
	{
		//	유저 접속 확인.
		Temp_ID = g_Clan_List[p_Clan].m_User_IDX[i];
		if(0 < Temp_ID)
		{
			//	클랜 마스터인 경우 확인
			if(g_User_List[Temp_ID - 1]->Is_ClanMASTER())
				return Temp_ID;
		}
	}
	return 0;
}

////	클랜 마스터 하위 유저를 찾는다.
DWORD	GF_Clan_Master_Sub_Fine(DWORD p_Clan)
{
	DWORD	Temp_ID = 0;
	if(g_Clan_List[Temp_ID - 1].m_Sub_Master > 0)
	{
		Temp_ID = g_Clan_List[Temp_ID - 1].m_Sub_Master;
		g_Clan_List[Temp_ID - 1].m_Sub_Master = 0;
		return Temp_ID;
	}
	//	등급 순서대로 찾기
	for(DWORD p = 5; p > 0; p--)
	{
		// for(DWORD i = 0; i < 15;i++)
		// (20070125:남병철) : 클랜원 수치의 15명 하드코딩을 CW_USER_NO의 수치로 변경 (최대 50명까지 가능)
		for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0];i++ )
		{
			//	유저 접속 확인.
			Temp_ID = g_Clan_List[p_Clan].m_User_IDX[i];
			if(0 < Temp_ID)
			{
				//	클랜 마스터인 경우 확인
				if(g_User_List[Temp_ID - 1]->GetClanPOS() == p)
				{
					g_Clan_List[Temp_ID - 1].m_Sub_Master = Temp_ID;
					return Temp_ID;
				}
			}
		}
	}
	//	순서 대로 찾지 못한 경우 : 첫번째 유저 보냄
	// for(DWORD i = 0; i < 15;i++)
	// (20070125:남병철) : 클랜원 수치의 15명 하드코딩을 CW_USER_NO의 수치로 변경 (최대 50명까지 가능)
	for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0]; i++ )
	{
		//	유저 접속 확인.
		Temp_ID = g_Clan_List[p_Clan].m_User_IDX[i];
		if(0 < Temp_ID)
		{
			g_Clan_List[Temp_ID - 1].m_Sub_Master = Temp_ID;
			return Temp_ID;
		}
	}
	return 0;
}

////	클랜전 등록 클랜 확인
BOOL	GF_ClanWar_Check_myClan(DWORD p_List_N,DWORD p_Clan_N)
{
	//	리스트 범위 확인.
	if(p_List_N == 0) return false;
	p_List_N = p_List_N - 1;
	if(p_List_N >= DF_MAX_CWAR_LIST) return false;
	if(p_Clan_N == 0) return false;

	//	해당 리스트에 클랜 번호 확인
	if(g_ClanWar_List[p_List_N].m_CW_Npc == NULL) return false;	// 배당 안된 방.
	//	게임이 종료된 경우 입장 불가 처리.
	switch(g_ClanWar_List[p_List_N].m_Play_Type)
	{
		//	클랜전 진행방이다. 팀 확인하자.
	case 2:
	case 3:
	case 4:
		break;
		//	클랜전 진행방이 아니다. 입장 불가.
	default:
		return false;
	}
	//	클랜 확인.
	if(g_ClanWar_List[p_List_N].m_Clan_A == p_Clan_N) return 1;
	if(g_ClanWar_List[p_List_N].m_Clan_B == p_Clan_N) return 2;
	return false;
}

////	클랜 소속 게임 유저에게 입장 확인 메시지 처리
BOOL	GF_ClanWar_Join_OK_Send(DWORD p_List_N,DWORD p_Type,DWORD p_Sub_Type)
{
	//	NPC 확인.
	if(g_ClanWar_List[p_List_N].m_CW_Npc == NULL) return false;
	if(p_Type == 0) return false;
	DWORD	t_A = 0;
	DWORD	t_B = 0;
	BOOL	b_A = false;
	BOOL	b_B = false;
	//	A,B팀 클랜 리스트 번호 찾기 [해당 클랜이 없는 경우 메시지 보내지 못한다. 확인 처리...]
	t_A = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
	t_B = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);

	//	클랜 소속 유저에게 모두 전송.
	DWORD	TempA_ID = 0;
	DWORD	TempB_ID = 0;
	switch(p_Type)
	{
	case 1:
		{
			//	p_Type이 1~2이면 반드시 두팀 필요 나머지는 존재하는 팀만 처리
			if((t_A*t_B) == 0) return false;	// 0값이 있으면 오류.
			//	팀배열로 변환
			t_A = t_A - 1;
			t_B = t_B - 1;
			//	클랜 마스터인 경우 확인 (두팀의 마스터가 존재하지 않는 유저 확인 필요.)
			TempA_ID = GF_Clan_Master_Fine(t_A);	// A 팀 마스터 찾기
			TempB_ID = GF_Clan_Master_Fine(t_B);	// B 팀 마스터 찾기
			if((TempA_ID > 0)&&(TempB_ID > 0))
			{
				//	두 팀의 마스터가 존재.
				//	마스터 게임 진행 여부 통보.
				//	LV 제한 확인은 별도로 한다.
				g_User_List[TempA_ID - 1]->Send_gsv_CLANWAR_OK(1,g_ClanWar_List[p_List_N].m_Clan_A,(WORD)(p_List_N + 1),1000);
				g_User_List[TempB_ID - 1]->Send_gsv_CLANWAR_OK(1,g_ClanWar_List[p_List_N].m_Clan_B,(WORD)(p_List_N + 1),2000);
				
				//	몹소환
				GF_ClanWar_Mob_Set(p_List_N + 1);
			}
			//	매칭 되었는데 상대방 팀 마스터가 없는 경우.... 매칭 중단.
			else if((TempA_ID == 0)&&(TempB_ID == 0))
			{
				//	두 팀다 마스터 없슴. 띠블
				GF_Del_CWar_List2(p_List_N);
			}
			else if(TempA_ID == 0)
			{
				//	B팀 승리 처리 [보상 없는 승리 처리]
				GF_Win_CWar_Set((LPVOID)&g_ClanWar_List[p_List_N],2,false);
				//	B팀에 퀘스트 승리 처리
				GF_ClanWar_Quest_DropProc(p_List_N+1,2000);
				////	비긴 돈만 넣어준다.
				//g_User_List[TempB_ID - 1]->m_Inventory.m_i64Money = 
				//	g_User_List[TempB_ID - 1]->m_Inventory.m_i64Money + (g_ClanWar_List[p_List_N].m_Bat_Coin/200)*110;
				//g_User_List[TempB_ID - 1]->Send_gsv_SET_MONEYONLY(GSV_SET_MONEY_ONLY);
				//	방정보 초기화
				GF_Del_CWar_List2(p_List_N);
			}
			else
			{
				//	A팀 승리 처리 [보상 없는 승리 처리]
				GF_Win_CWar_Set((LPVOID)&g_ClanWar_List[p_List_N],1,false);
				//	A팀에 퀘스트 승리 처리
				GF_ClanWar_Quest_DropProc(p_List_N+1,1000);
				//	방정보 초기화
				GF_Del_CWar_List2(p_List_N);
			}
			break;
		}
	case 2:
		{
			//	p_Type이 1~2이면 반드시 두팀 필요 나머지는 존재하는 팀만 처리
			if((t_A*t_B) == 0) return false;	// 0값이 있으면 오류.
			//	팀배열로 변환
			t_A = t_A - 1;
			t_B = t_B - 1;
			//	클랜 유저들에게 통보.
			switch(p_Sub_Type)
			{
			case 1:
				{
					//	A 팀 클랜 유저 전송.
					// for(DWORD i = 0; i < 15;i++)
					// (20070125:남병철) : 클랜원 수치의 15명 하드코딩을 CW_USER_NO의 수치로 변경 (최대 50명까지 가능)
					for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0]; i++ )
					{
						//	유저 접속 확인.
						TempA_ID = g_Clan_List[t_A].m_User_IDX[i];
						ZTRACE( "%s, USER_ID(%d)", g_Clan_List[t_A].m_ClanNAME, i );
						if(0 < TempA_ID)
						{
							TempA_ID = TempA_ID  - 1;
							if(!g_User_List[TempA_ID]->Is_ClanMASTER())
							{
								ZTRACE( "%s, USER_ID(%d) - 클랜원", g_Clan_List[t_A].m_ClanNAME, i );
								//	LV 제한 확인.
								if(GF_Clan_War_Check_LV(g_User_List[TempA_ID]->Get_LEVEL(),g_ClanWar_List[p_List_N].m_War_Type))
								{
									// 클랜 마스터가 아닌 경우 참여 메시지 보냄
									g_User_List[TempA_ID]->Send_gsv_CLANWAR_OK(p_Type,g_ClanWar_List[p_List_N].m_Clan_A,(WORD)(p_List_N + 1),1000);
								}
								else
								{
									// LV 제한에 의한 입장 불가 메시지 전송.
									g_User_List[TempA_ID]->Send_gsv_CLANWAR_Err(CWAR_CLAN_MASTER_LEVEL_RESTRICTED);
								}
							}
							else
							{
								ZTRACE( "%s, USER_ID(%d) - 클랜마스터", g_Clan_List[t_A].m_ClanNAME, i );
							}
						}
						else
						{
							ZTRACE( "%s, USER_ID(%d) - 없는 유저", g_Clan_List[t_A].m_ClanNAME, i );
						}
					}
					//	옵션 처리
					if(g_ClanWar_List[p_List_N].m_Play_Type == 2)
						g_ClanWar_List[p_List_N].m_Play_Type = 3;
					else if(g_ClanWar_List[p_List_N].m_Play_Type == 3)
						g_ClanWar_List[p_List_N].m_Play_Type = 4;
					break;
				}
			case 2:
				{
					//	B 팀 클랜 유저 전송.
					// for(DWORD i = 0; i < 15;i++)
					// (20070125:남병철) : 클랜원 수치의 15명 하드코딩을 CW_USER_NO의 수치로 변경 (최대 50명까지 가능)
					for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0]; i++ )
					{
						//	유저 접속 확인.
						TempB_ID = g_Clan_List[t_B].m_User_IDX[i];
						ZTRACE( "%s, USER_ID(%d)", g_Clan_List[t_B].m_ClanNAME, i );
						if(0 < TempB_ID)
						{
							TempB_ID = TempB_ID  - 1;
							if(!g_User_List[TempB_ID]->Is_ClanMASTER())
							{
								ZTRACE( "%s, USER_ID(%d) - 클랜원", g_Clan_List[t_B].m_ClanNAME, i );
								//	LV 제한 확인.
								if(GF_Clan_War_Check_LV(g_User_List[TempB_ID]->Get_LEVEL(),g_ClanWar_List[p_List_N].m_War_Type))
								{
									//	클랜 마스터가 아닌 경우 참여 메시지 보냄
									g_User_List[TempB_ID]->Send_gsv_CLANWAR_OK(p_Type,g_ClanWar_List[p_List_N].m_Clan_B,(WORD)(p_List_N + 1),2000);
								}
								else
								{
									//	LV 제한에 의한 입장 불가 메시지 전송.
									g_User_List[TempB_ID]->Send_gsv_CLANWAR_Err(CWAR_CLAN_MASTER_LEVEL_RESTRICTED);
								}
							}
							else
							{
								ZTRACE( "%s, USER_ID(%d) - 클랜마스터", g_Clan_List[t_B].m_ClanNAME, i );
							}
						}
						else
						{
							ZTRACE( "%s, USER_ID(%d) - 없는 유저", g_Clan_List[t_B].m_ClanNAME, i );
						}
					}
					//	옵션 처리
					if(g_ClanWar_List[p_List_N].m_Play_Type == 2)
						g_ClanWar_List[p_List_N].m_Play_Type = 3;
					else if(g_ClanWar_List[p_List_N].m_Play_Type == 3)
						g_ClanWar_List[p_List_N].m_Play_Type = 4;
					break;
				}
			}
			break;
		}
	case 7:
		{
			//	존재하는 팀만 처리
			if(t_A == 0) b_A = false;
			else 
			{
				b_A = true;
				t_A = t_A - 1;
			}
			if(t_B == 0) b_B = false;
			else 
			{
				b_B = true;
				//	팀배열로 변환
				t_B = t_B - 1;
			}
			//	타임 카운트 메시지 보냄.
			//	A,B 팀 클랜 유저 전송.
			// for(DWORD i = 0; i < 15;i++)
			// (20070125:남병철) : 클랜원 수치의 15명 하드코딩을 CW_USER_NO의 수치로 변경 (최대 50명까지 가능)
			for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0]; i++ )
			{
				//	A팀이 접존재하는 경우만 처리
				if(b_A)
				{
					//	유저 접속 확인.
					TempA_ID = g_Clan_List[t_A].m_User_IDX[i];
					if(0 < TempA_ID)
					{
						TempA_ID = TempA_ID  - 1;
						if(g_User_List[TempA_ID]->GetZONE() != NULL)
						{
							if(g_ClanWar_List[p_List_N].m_ZoneNO == g_User_List[TempA_ID]->m_nZoneNO)
							{
								//	타임 메시지 보냄
								g_User_List[TempA_ID]->Send_gsv_CLANWAR_Time(
									g_ClanWar_List[p_List_N].m_Play_Type,
									g_ClanWar_List[p_List_N].m_Play_Count);
							}
						}
					}
				}
				//	유저 접속 확인.
				if(b_B)
				{
					TempB_ID = g_Clan_List[t_B].m_User_IDX[i];
					if(0 < TempB_ID)
					{
						TempB_ID = TempB_ID  - 1;
						if(g_User_List[TempB_ID]->GetZONE() != NULL)
						{
							if(g_ClanWar_List[p_List_N].m_ZoneNO == g_User_List[TempB_ID]->m_nZoneNO)
							{
								//	타임 메시지 보냄
								g_User_List[TempB_ID]->Send_gsv_CLANWAR_Time(
									g_ClanWar_List[p_List_N].m_Play_Type,
									g_ClanWar_List[p_List_N].m_Play_Count);
							}
						}
					}
				}
			}
			break;
		}
	case 10:
		{
			//	존재하는 팀만 처리
			if(t_A == 0) b_A = false;
			else 
			{
				b_A = true;
				t_A = t_A - 1;
			}
			if(t_B == 0) b_B = false;
			else 
			{
				b_B = true;
				//	팀배열로 변환
				t_B = t_B - 1;
			}
			//	클랜전에 참여한 유저 리콜
			//	A,B 팀 클랜 유저 전송.
			// for(DWORD i = 0; i < 15;i++)
			// (20070125:남병철) : 클랜원 수치의 15명 하드코딩을 CW_USER_NO의 수치로 변경 (최대 50명까지 가능)
			for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0]; i++ )
			{
				//	유저 접속 확인.
				if(b_A)
				{
					TempA_ID = g_Clan_List[t_A].m_User_IDX[i];
					if(0 < TempA_ID)
					{
						TempA_ID = TempA_ID  - 1;
						if(g_ClanWar_List[p_List_N].m_ZoneNO == g_User_List[TempA_ID]->m_nZoneNO)
						{
							//	주논으로 보낸다.
							GF_ClanWar_Out_Move((LPVOID)g_User_List[TempA_ID]);
						}
					}
				}
				//	유저 접속 확인
				if(b_B)
				{
					TempB_ID = g_Clan_List[t_B].m_User_IDX[i];
					if(0 < TempB_ID)
					{
						TempB_ID = TempB_ID  - 1;
						if(g_ClanWar_List[p_List_N].m_ZoneNO == g_User_List[TempB_ID]->m_nZoneNO)
						{
							//	주논으로 보낸다.
							GF_ClanWar_Out_Move((LPVOID)g_User_List[TempB_ID]);
						}
					}
				}
			}
			break;
		}
	default:
		{
			return false;
		}
	}
	return true;
}

////	클랜전 지역으로 유저 워프 시킴
BOOL	GF_ClanWar_Move(LPVOID p_User,DWORD p_List_N,DWORD p_Team_N)
{
	if(p_User == NULL) return false;
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	if(p_Team_N == 0) return false;
	classUSER	*Temp_User;
	Temp_User = (classUSER *)p_User;

	// 워프시 LV 확인 처리.
	if(!GF_Clan_War_Check_LV(Temp_User->Get_LEVEL(),g_ClanWar_List[p_List_N - 1].m_War_Type))
	{
		// 워프 시키지 않은다.
		Temp_User->Send_gsv_CLANWAR_Err(CWAR_CLAN_MASTER_LEVEL_RESTRICTED);
		return true;
	}

	//	위치로 포탈 시킴.
	if(p_Team_N == 1)
	{
		//	참여 인원 제한 확인.
		if(!GF_Add_ClanWar_Number(p_List_N,1000))
		{
			//	인원 제한 처리 메시지 전송.
			Temp_User->Send_gsv_CLANWAR_Err(CWAR_TOO_MANY_CWAR_PROGRESS);
			return 2;
		}

		//	채력이 0인 경우 힐링한다.
		if(Temp_User->Get_HP() <= 0)
		{
			Temp_User->Set_HP(Temp_User->Get_MaxHP());
			Temp_User->Send_CHAR_HPMP_INFO(0);		// HP, MP정보를 한번 더 보내준다.
		}

		//	파티 해제
		Temp_User->Party_Out();
		//	클랜전 이동
		Temp_User->Reward_WARP(g_ClanWar_List[p_List_N - 1].m_ZoneNO,g_ClanWar_List[p_List_N - 1].m_APosWARP);
		//	해당 유저 팀 번호 변경.
		Temp_User->SetCur_TeamNO(1000);
		Temp_User->Set_TeamNoFromClanIDX(1000);	// 일반 변경.w
	}
	else if(p_Team_N == 2)
	{
		//	참여 인원 제한 확인.
		if(!GF_Add_ClanWar_Number(p_List_N,2000))
		{
			//	인원 제한 처리 메시지 전송.
			Temp_User->Send_gsv_CLANWAR_Err(CWAR_TOO_MANY_CWAR_PROGRESS);
			return 2;
		}

		//	채력이 0인 경우 힐링한다.
		if(Temp_User->Get_HP() <= 0)
		{
			Temp_User->Set_HP(Temp_User->Get_MaxHP());
			Temp_User->Send_CHAR_HPMP_INFO(0);		// HP, MP정보를 한번 더 보내준다.
		}

		//	파티 해제
		Temp_User->Party_Out();
		//	클랜전 이동
		Temp_User->Reward_WARP(g_ClanWar_List[p_List_N - 1].m_ZoneNO,g_ClanWar_List[p_List_N - 1].m_BPosWARP);
		//	해당 유저 팀 번호 변경.
		Temp_User->SetCur_TeamNO(2000);
		Temp_User->Set_TeamNoFromClanIDX(2000);	// 일반 변경.
	}
	else return false;
	return true;
}

////	클랜전 종료 위치로 워프
BOOL	GF_ClanWar_Out_Move(LPVOID p_User)
{
	if(p_User == NULL) return false;
	classUSER	*Temp_User;
	Temp_User = (classUSER *)p_User;

	//	파티 해제
	Temp_User->Party_Out();

	//	채력이 0인 경우 힐링한다.
	if(Temp_User->Get_HP() <= 0)
	{
		Temp_User->Set_HP(Temp_User->Get_MaxHP());
		Temp_User->Send_CHAR_HPMP_INFO(0);		// HP, MP정보를 한번 더 보내준다.
	}

	// 임시 위치로 넴
	tPOINTF	Temp_Pos;
	Temp_Pos.x = g_ClanWar_ReCall.m_Pos.x + (rand()%50) - 25;
	Temp_Pos.y = g_ClanWar_ReCall.m_Pos.y + (rand()%50) - 25;
	//	해당 유저 게임 종료 위치로 보냄.
	Temp_User->Reward_WARP(g_ClanWar_ReCall.m_ZoneNO,Temp_Pos);
	//	해당 유저 팀 번호 변경.
	Temp_User->SetCur_TeamNO(2);
	Temp_User->Set_TeamNoFromClanIDX(0);	// 일반 변경.
	return true;
}

////	클랜전 공간에 몹 설정 하기
BOOL	GF_ClanWar_Mob_Set(DWORD p_List_N)
{
	//	리스트 범위 확인.
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	p_List_N = p_List_N - 1;
	//	해당 리스트의 몹 지우기
	GF_ClanWar_Mob_Del(p_List_N + 1,0);

	//	배팅 타입 얻기
	DWORD Temp_1 = g_ClanWar_List[p_List_N].m_War_Type;
	if(Temp_1 > 2) Temp_1 = 0;	// 정의 되지 않은 경우 0 : 이런 경우 없음

	for(DWORD i = 0; i < DF_MAX_CWAR_MOB; i++)
	{
		// (20070123:남병철) : 설정된 몹이 없으면 리젠 않함
		if( g_CW_MOB_List[Temp_1][i][0].m_MOB_Idx > 0 )
		{
			//	해당 지역에 몹 생성 하기.
			g_ClanWar_List[p_List_N].m_ATeam_Mob[i] = 
				g_pZoneLIST->RegenCharacter(g_ClanWar_List[p_List_N].m_ZoneNO,
				g_CW_MOB_List[Temp_1][i][0].m_Pos_X, g_CW_MOB_List[Temp_1][i][0].m_Pos_Y,
				g_CW_MOB_List[Temp_1][i][0].m_Range, g_CW_MOB_List[Temp_1][i][0].m_MOB_Idx,
				g_CW_MOB_List[Temp_1][i][0].m_Number, DF_CWAR_A_TEAM);
		}

		// (20070123:남병철) : 설정된 몹이 없으면 리젠 않함
		if( g_CW_MOB_List[Temp_1][i][1].m_MOB_Idx > 0 )
		{
			g_ClanWar_List[p_List_N].m_BTeam_Mob[i] = 
				g_pZoneLIST->RegenCharacter(g_ClanWar_List[p_List_N].m_ZoneNO,
				g_CW_MOB_List[Temp_1][i][1].m_Pos_X, g_CW_MOB_List[Temp_1][i][1].m_Pos_Y, 
				g_CW_MOB_List[Temp_1][i][1].m_Range, g_CW_MOB_List[Temp_1][i][1].m_MOB_Idx,
				g_CW_MOB_List[Temp_1][i][1].m_Number, DF_CWAR_B_TEAM);
		}
	}

	//	PK 모드 설정
	g_pZoneLIST->Set_PK_FLAG((short)g_ClanWar_List[p_List_N].m_ZoneNO,true);
	return true;
}

////	클랜전 공간에 몹 지우기
BOOL	GF_ClanWar_Mob_Del(DWORD p_List_N,DWORD p_Type)
{
	//	리스트 범위 확인.
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	p_List_N = p_List_N - 1;

	//	존의 모든 몹을 죽인다.
//	if(g_ClanWar_List[p_List_N].m_CW_Npc != NULL)
//	{
		if(p_Type == 0)
			g_pZoneLIST->Zone_MOB_Kill(g_ClanWar_List[p_List_N].m_ZoneNO,NULL,0);
		else if(p_Type == 1)
			g_pZoneLIST->Zone_MOB_Kill(g_ClanWar_List[p_List_N].m_ZoneNO,NULL,1000);
		else if(p_Type == 2)
			g_pZoneLIST->Zone_MOB_Kill(g_ClanWar_List[p_List_N].m_ZoneNO,NULL,2000);
//	}
	for(DWORD i = 0; i < DF_MAX_CWAR_MOB; i++)
	{
		switch(p_Type)
		{
		case 0:
			{
				// 양팀 모두 초기화
				if(g_ClanWar_List[p_List_N].m_ATeam_Mob[i] != NULL)
				{
					g_ClanWar_List[p_List_N].m_ATeam_Mob[i] = NULL;
				}
				if(g_ClanWar_List[p_List_N].m_BTeam_Mob[i] != NULL)
				{
					g_ClanWar_List[p_List_N].m_BTeam_Mob[i] = NULL;
				}
				break;
			}
		case 1:
			{
				// A팀 모두 초기화
				if(g_ClanWar_List[p_List_N].m_ATeam_Mob[i] != NULL)
				{
					g_ClanWar_List[p_List_N].m_ATeam_Mob[i] = NULL;
				}
				break;
			}
		case 2:
			{
				// B팀 모두 초기화
				if(g_ClanWar_List[p_List_N].m_BTeam_Mob[i] != NULL)
				{
					g_ClanWar_List[p_List_N].m_BTeam_Mob[i] = NULL;
				}
				break;
			}
		}
	}
	return true;
}
BOOL	GF_ClanWar_Mob_Del2(LPVOID p_Room,DWORD p_Type)
{
	//	리스트 범위 확인.
	if(p_Room == NULL) return false;
	GF_ClanWar_Set *p_CWar_Room;
	p_CWar_Room = (GF_ClanWar_Set *)p_Room;

	//	존의 모든 몹을 죽인다.
	if(p_Type == 0)
		g_pZoneLIST->Zone_MOB_Kill(p_CWar_Room->m_ZoneNO,NULL,0);
	else if(p_Type == 1)
		g_pZoneLIST->Zone_MOB_Kill(p_CWar_Room->m_ZoneNO,NULL,1000);
	else if(p_Type == 2)
		g_pZoneLIST->Zone_MOB_Kill(p_CWar_Room->m_ZoneNO,NULL,2000);

	for(DWORD i = 0; i < DF_MAX_CWAR_MOB; i++)
	{
		switch(p_Type)
		{
		case 0:
			{
				// 양팀 모두 초기화
				if(p_CWar_Room->m_ATeam_Mob[i] != NULL)
				{
					p_CWar_Room->m_ATeam_Mob[i] = NULL;
				}
				if(p_CWar_Room->m_BTeam_Mob[i] != NULL)
				{
					p_CWar_Room->m_BTeam_Mob[i] = NULL;
				}
				break;
			}
		case 1:
			{
				// A팀 모두 초기화
				if(p_CWar_Room->m_ATeam_Mob[i] != NULL)
				{
					p_CWar_Room->m_ATeam_Mob[i] = NULL;
				}
				break;
			}
		case 2:
			{
				// B팀 모두 초기화
				if(p_CWar_Room->m_BTeam_Mob[i] != NULL)
				{
					p_CWar_Room->m_BTeam_Mob[i] = NULL;
				}
				break;
			}
		}
	}

	return true;
}

////	해당 팀의 몬스터 체력을 회복한다.
BOOL	GF_ClanWar_MOB_HP_ReSet(LPVOID p_Room,DWORD p_Type)
{
	//	리스트 범위 확인.
	if(p_Room == NULL) return false;
	GF_ClanWar_Set *p_CWar_Room;
	p_CWar_Room = (GF_ClanWar_Set *)p_Room;

	//	팀 번호 확인.
	if(p_Type == 1)
		g_pZoneLIST->Zone_MOB_HP_Reset(p_CWar_Room->m_ZoneNO,1000);
	else if(p_Type == 2)
		g_pZoneLIST->Zone_MOB_HP_Reset(p_CWar_Room->m_ZoneNO,2000);
	return true;
}

////	클랜전 보스몹 잡음 메시지 처리
BOOL	GF_ClanWar_Boss_MOB_Kill(DWORD List_N,DWORD p_idx)
{
	////	클랜전 종료 퀘스트가 도착한 경우 : 처리 오류시에만 false 반환. []
	//	유저 정보 확인
	//if(p_User == NULL) return true;
	//classUSER	*Temp_User;
	//Temp_User = (classUSER *)p_User;
	if(List_N == 0) return false;	// 방번호.

	DWORD	Temp_ID = List_N;
	//	퀘스트 인덱스 확인.
	switch(p_idx)
	{
	case 2814:
		{
			//	50만 배팅 방
			//	2811번 퀘스트 삭제

			////	2) 해당 유저의 클랜전 방 번호 얻기.
			//Temp_ID = GF_Find_CWar_List2(Temp_User->GetClanID());
			if(Temp_ID == 0) return false;	// 방 번호 없음.. 다음 처리 안됨. 이미 처리됨.;
			//	3) 해당 방 게임 윈 처리.
			GF_Clan_War_Drop(Temp_ID,0);	// 자동 설정.

			//	반대편 팀. 퀘스트 삭제
			DWORD	t_Cl = 0;
			DWORD	t_CM = 0;
			if(g_ClanWar_List[Temp_ID - 1].m_Win_Team == 1)
			{
				//	B팀 클랜장 찾기
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_B);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;		
					t_CM = GF_Clan_Master_Fine(t_Cl);	//B 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
					}
					else
					{
						//	진팀의 클랜장이 없다.. 어떻게 처리할까?
						//	진 팀은 처리 안한다.
					}
				}

				//	A팀 승리 퀘스트 처리
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_A);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	//A 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
						//	퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
					}
					else
					{
						// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_Cl);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
						}
					}
				}
				//	A팀 승리 메시지 전송
				GF_Send_CLANWAR_Progress(Temp_ID,2);
			}
			else if(g_ClanWar_List[Temp_ID - 1].m_Win_Team == 2)
			{
				//	A팀 클랜장 찾기
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_A);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;
					t_CM = GF_Clan_Master_Fine(t_Cl);	//A팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
					}
					else
					{
						// 진팀의 클랜장이 없다.. 어떻게 처리할까?
					}
				}
				//	B팀 승리 퀘스트 처리
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_B);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	// B팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
						//	퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
					}
					else
					{
						// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_Cl);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
						}
					}
				}
				//	B팀 승리 메시지 전송
				GF_Send_CLANWAR_Progress(Temp_ID,3);
			}
			return false;
			break;
		}
	case 2815:
		{
			//	100만 배팅 방
			//	2812번 퀘스트 삭제
			
			////	1) 해당 유저가 클랜장인지 확인 한다.
			//if(!Temp_User->Is_ClanMASTER())
			//	return false;	// 오류 트리거 처리 중단. 해당 방에 이긴 팀 처리 [이미 처리 되어 있더면 오류 처리.]
			
			//	2) 해당 유저의 클랜전 방 번호 얻기.
			//Temp_ID = GF_Find_CWar_List2(Temp_User->GetClanID());
			if(Temp_ID == 0) return false;	// 방 번호 없음.. 다음 처리 안됨. 이미 처리됨.;
			//	3) 해당 방 게임 윈 처리.
			GF_Clan_War_Drop(Temp_ID,0);	// 자동 설정.
			
			////	소유하고 있던 퀘스트 삭제 필요.
			////	퀘스트 지운다.
			//GF_ClanWar_Quest_Del((LPVOID)Temp_User,2812);

			//	반대편 팀. 퀘스트 삭제
			DWORD	t_Cl = 0;
			DWORD	t_CM = 0;
			if(g_ClanWar_List[Temp_ID - 1].m_Win_Team == 1)
			{
				//	B팀 클랜장 찾기
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_B);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	//B 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
					}
				}
				//	A팀 승리 퀘스트 처리
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_A);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	//A 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
						//	퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
					}
					else
					{
						// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_Cl);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
						}
					}
				}
				//	A팀 승리 메시지 전송
				GF_Send_CLANWAR_Progress(Temp_ID,2);
			}
			else if(g_ClanWar_List[Temp_ID - 1].m_Win_Team == 2)
			{
				//	A팀 클랜장 찾기
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_A);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	//A팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
					}
				}
				//	B팀 승리 퀘스트 처리
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_B);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	// B팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
						//	퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
					}
					else
					{
						// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_Cl);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
						}
					}
				}
				//	B팀 승리 메시지 전송
				GF_Send_CLANWAR_Progress(Temp_ID,3);
			}
			return false;
			break;
		}
	case 2816:
		{
			//	1000만 배팅 방
			//	2813번 퀘스트 삭제
			
			////	1) 해당 유저가 클랜장인지 확인 한다.
			//if(!Temp_User->Is_ClanMASTER())
			//	return false;	// 오류 트리거 처리 중단. 해당 방에 이긴 팀 처리 [이미 처리 되어 있더면 오류 처리.]
			
			//	2) 해당 유저의 클랜전 방 번호 얻기.
			//Temp_ID = GF_Find_CWar_List2(Temp_User->GetClanID());
			if(Temp_ID == 0) return false;	// 방 번호 없음.. 다음 처리 안됨. 이미 처리됨.;
			//	3) 해당 방 게임 윈 처리.
			GF_Clan_War_Drop(Temp_ID,0);	// 자동 설정.
			
			////	소유하고 있던 퀘스트 삭제 필요.
			////	퀘스트 지운다.
			//GF_ClanWar_Quest_Del((LPVOID)Temp_User,2813);

			//	반대편 팀. 퀘스트 삭제
			DWORD	t_Cl = 0;
			DWORD	t_CM = 0;
			if(g_ClanWar_List[Temp_ID - 1].m_Win_Team == 1)
			{
				//	B팀 클랜장 찾기
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_B);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	//B 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
					}
				}
				//	A팀 승리 퀘스트 처리
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_A);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	//A 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
						//	퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
					}
					else
					{
						// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_Cl);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
						}
					}
				}
				//	A팀 승리 메시지 전송
				GF_Send_CLANWAR_Progress(Temp_ID,2);
			}
			else if(g_ClanWar_List[Temp_ID - 1].m_Win_Team == 2)
			{
				//	A팀 클랜장 찾기
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_A);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;
					t_CM = GF_Clan_Master_Fine(t_Cl);	// A팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
					}
				}
				//	B팀 승리 퀘스트 처리
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_B);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	// B팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
						//	퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
					}
					else
					{
						// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_Cl);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
						}
					}
				}
				//	B팀 승리 메시지 전송
				GF_Send_CLANWAR_Progress(Temp_ID,3);
			}
			////	퀘스트 등록
			//GF_ClanWar_Quest_Add((LPVOID)Temp_User,p_idx);
			return false;
			break;
		}
	default:
		return true;
	}
	return true;
}

////	클랜전 보스몹이 죽었는지 체크 [2차 클랜전 보강 : 서버에서 보스 죽었는지 체크]
BOOL	GF_ClanWar_Boss_Kill_Check(LPVOID p_Boss,LPVOID p_User)
{
	//	해당 몬스터가 클랜전 보스인지 확인
	//	클랜전 보스이면 해당 존의 종료 처리
	//	최종 클랜전 보스의 HP 전달해 준다.
	//	this는 유저가 아니라 유저 소환 몹일수있다.
	if(p_Boss == NULL) return false;
	if(p_User == NULL) return false;
	//	몬스터 클래스로 형변환.
	DWORD	Temp_List_N = GF_ClanWar_Boss_MOB_Is(p_Boss);
	if(Temp_List_N == 0) return false;
	//	해당존의 보스 포인트 확인.
	CObjMOB	*Temp_MOB = (CObjMOB *)p_Boss;
	//	A 팀 보스 확인
	if(g_ClanWar_List[Temp_List_N - 1].m_ATeam_Mob[0] == Temp_MOB)
	{
		g_ClanWar_List[Temp_List_N - 1].m_ATeam_Mob[0] = NULL;
		g_ClanWar_List[Temp_List_N - 1].m_Win_Team = 2;
		//	클랜전 방 타입 처리
		switch(g_ClanWar_List[Temp_List_N - 1].m_War_Type)
		{
		case 0:
			//	50만 배팅 (2814:드레곤 사냥 퀘스트 (50만줄리))
			GF_ClanWar_Boss_MOB_Kill(Temp_List_N,2814);
			break;
		case 1:
			//	100만 배팅 (2815:드레곤 사냥 퀘스트 (100만줄리))
			GF_ClanWar_Boss_MOB_Kill(Temp_List_N,2815);
			break;
		case 2:
			//	1000만 배팅 (2816:드레곤 사냥 퀘스트 (1000만줄리))
			GF_ClanWar_Boss_MOB_Kill(Temp_List_N,2816);
			break;
		}
		return true;
	}
	//	B 팀 보스 확인
	if(g_ClanWar_List[Temp_List_N - 1].m_BTeam_Mob[0] == Temp_MOB)
	{
		g_ClanWar_List[Temp_List_N - 1].m_BTeam_Mob[0] = NULL;	
		g_ClanWar_List[Temp_List_N - 1].m_Win_Team = 1;
		//	클랜전 방 타입 처리
		switch(g_ClanWar_List[Temp_List_N - 1].m_War_Type)
		{
		case 0:
			//	50만 배팅
			GF_ClanWar_Boss_MOB_Kill(Temp_List_N,2814);
			break;
		case 1:
			//	100만 배팅
			GF_ClanWar_Boss_MOB_Kill(Temp_List_N,2815);
			break;
		case 2:
			//	1000만 배팅
			GF_ClanWar_Boss_MOB_Kill(Temp_List_N,2816);
			break;
		}
		return true;
	}
	//	보스로 설정된 몬스터가 A팀,B팀 모드 설정되지 않았다.
	return false;
}

////	HP 전송을 위한 클랜전 보스 몹인지 확인한다 . [2차 클랜전 보강]
DWORD	GF_ClanWar_Boss_MOB_Is(LPVOID p_Boss)
{
	//	전달 값 확인.
	if(p_Boss == NULL) return 0;
	//	몬스터 클래스로 형변환.
	CObjMOB	*Temp_MOB = (CObjMOB *)p_Boss;
	//	레벨 반복
	for(int i = 0 ; i < DF_MAX_CWAR_TYPE; i++)
	{
		//	팀 반복
		for(int t = 0 ; t < DF_CWAR_TEAM_N; t++)
		{
			//	보스몹 확인.
			if(	Temp_MOB->Get_CharNO() == g_CW_MOB_List[i][0][t].m_MOB_Idx)
			{
				//	보스몹의 존 번호를 기준으로 클랜전 존 리스트 번호 얻기.
				return GF_Clan_War_Zone_Is((DWORD)Temp_MOB->GetZONE()->Get_ZoneNO());
			}
		}
	}
	return 0;
}

////	클랜전 보스몹 HP 상태 전송
BOOL	GF_ClanWar_Boss_MOB_HP(DWORD p_List_N)
{
	//	리스트 범위 확인.
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	p_List_N = p_List_N - 1;
	
	//	현재 카운트 체크
	DWORD	Temp_Count = GetTickCount();
	BOOL	Temp_TF = false;
	//	이전에 전송한 HP 카운트와 비교
	if(g_ClanWar_List[p_List_N].m_Count < Temp_Count)
	{
		//	제한 시간 카운트 확인	1.5초에 한번씩 체크
		if((Temp_Count - g_ClanWar_List[p_List_N].m_Count) > 1500)
		{
			//	HP 전송한다.
			Temp_TF = true;
			//	현재 카운트 저장한다.
			g_ClanWar_List[p_List_N].m_Count = Temp_Count;
		}
	}
	else
	{
		//	타이머 회전한 경우.
		//	HP 전송한다.
		Temp_TF = true;
		//	현재 카운트 저장한다.
		g_ClanWar_List[p_List_N].m_Count = Temp_Count;
	}

	// 게임이 종료된 경우
	if(g_ClanWar_List[p_List_N].m_Play_Type == 5)
	{
		Temp_TF = true;	// 보냄.
	}

	if(Temp_TF)
	{
		//	클랜전 존에 모든 유저에게 보스 HP양을 전송 한다.
		char	SendBuffer[512];
		ZeroMemory(SendBuffer,512);
		gsv_cli_CLAN_WAR_BOSS_HP	*Send_Data;
		//	전송 버퍼를 데이터 형식으로 형변환..
		Send_Data = (gsv_cli_CLAN_WAR_BOSS_HP *)SendBuffer;
		//	메시지 데이터 넣는다.
		Send_Data->m_wType = GSV_CLANWAR_BOSS_HP;
		Send_Data->m_nSize = sizeof(gsv_cli_CLAN_WAR_BOSS_HP);
	
		// 게임이 종료된 경우
		if(g_ClanWar_List[p_List_N].m_Play_Type == 5)
		{
			Send_Data->m_A_nCharIdx = 0;
			Send_Data->m_A_Boss_Max_HP = 0;
			Send_Data->m_A_Boss_HP = 0;
			Send_Data->m_B_nCharIdx = 0;
			Send_Data->m_B_Boss_Max_HP = 0;
			Send_Data->m_B_Boss_HP = 0;
		}
		else
		{
			//	A팀 보스 몹의 HP 넣기.
			if(g_ClanWar_List[p_List_N].m_ATeam_Mob[0] == NULL)
			{
				Send_Data->m_A_nCharIdx = 0;
				Send_Data->m_A_Boss_Max_HP = 0;
				Send_Data->m_A_Boss_HP = 0;
			}
			else
			{
				Send_Data->m_A_nCharIdx = (short)g_ClanWar_List[p_List_N].m_ATeam_Mob[0]->Get_CharNO();
				Send_Data->m_A_Boss_Max_HP = g_ClanWar_List[p_List_N].m_ATeam_Mob[0]->Get_MaxHP();
				Send_Data->m_A_Boss_HP = g_ClanWar_List[p_List_N].m_ATeam_Mob[0]->Get_HP();
			}
			//	B팀 보스 몹의 HP 넣기.
			if(g_ClanWar_List[p_List_N].m_BTeam_Mob[0] == NULL)
			{
				Send_Data->m_B_nCharIdx = 0;
				Send_Data->m_B_Boss_Max_HP = 0;
				Send_Data->m_B_Boss_HP = 0;
			}
			else
			{
				Send_Data->m_B_nCharIdx = (short)g_ClanWar_List[p_List_N].m_BTeam_Mob[0]->Get_CharNO();
				Send_Data->m_B_Boss_Max_HP = g_ClanWar_List[p_List_N].m_BTeam_Mob[0]->Get_MaxHP();
				Send_Data->m_B_Boss_HP = g_ClanWar_List[p_List_N].m_BTeam_Mob[0]->Get_HP();
			}
		}
		//	메시지 해당 존에 전달.
		g_pZoneLIST->Send_gsv_All_User_Data(g_ClanWar_List[p_List_N].m_ZoneNO,SendBuffer);
	}

	return true;
}


////	클랜전 퀘스트 삭제
BOOL	GF_ClanWar_Quest_Del(LPVOID p_User,DWORD p_Quest_Idx)
{
	if(p_User == NULL) return false;
	classUSER	*Temp_User;
	Temp_User = (classUSER *)p_User;
	//	퀘스트 삭제
	DWORD	Temp_Q = Temp_User->Quest_GetRegistered(p_Quest_Idx);
	//	퀘스트 등록 위치를 얻는다.
	if(Temp_Q < QUEST_PER_PLAYER)
	{
		//	해당 퀘스트를 삭제 한다.
		Temp_User->Quest_Delete(p_Quest_Idx);
		//	유저에게 동보 한다.
		Temp_User->Send_gsv_QUEST_REPLY( RESULT_QUEST_REPLY_DEL_SUCCESS,(BYTE)Temp_Q,p_Quest_Idx );
	}
	return true;
}

////	클랜전 퀘스트 등록
BOOL	GF_ClanWar_Quest_Add(LPVOID p_User,DWORD p_Quest_Idx)
{
	if(p_User == NULL) return false;
	classUSER	*Temp_User;
	Temp_User = (classUSER *)p_User;
	//	퀘스트 등록
	DWORD	Temp_Q = Temp_User->Quest_Append(p_Quest_Idx);
	//	퀘스트 유저에게 동보 한다.
	Temp_User->Send_gsv_QUEST_REPLY( RESULT_QUEST_REPLY_ADD_SUCCESS, (BYTE)Temp_Q, p_Quest_Idx );
	return true;
}

////	클랜전 PK 모드 전투 확인
BOOL	GF_ClanWar_PK_Is(LPVOID p_User)
{
	if(p_User == NULL) return true;
	CObjCHAR	*Temp_User;
	Temp_User = (CObjCHAR *)p_User;
	//	유저의 팀 번호 확인
	switch(Temp_User->Get_TEAM())
	{
	//	팀번호
	case 1000:
	case 2000:
		{
			//	해당 존이 PK 모드가 아니면 공격 불가
			CZoneTHREAD	*Temp_Zone = NULL;
			Temp_Zone = Temp_User->GetZONE();
			if(Temp_Zone != NULL)
			{
				//	PK 모드가 꺼진 경우 해당 존에서 전투 금지.
				if(!Temp_Zone->Get_PK_FLAG())
				{
					//	AI 행동 금지
					Temp_User->SetCMD_STOP();
					return false;
				}
			}
			break;
		}
	}
	return true;
}


////	클랜전 참여 팀의 퀘스트 처리 함수
//	퀘스트 번호 ini 파일 처리 필요함.
BOOL	GF_ClanWar_Quest_DropProc(DWORD p_List_N,DWORD p_Team)
{
	if(p_List_N == 0)  return false;
	p_List_N = p_List_N - 1;
	//	해당 방이 존재하는 방인지 확인 필요
	if(g_ClanWar_List[p_List_N].m_CW_Npc == NULL) return false;

	DWORD t_AM,t_BM,t_CM;
	switch(p_Team)
	{
	case 0:
		{
			//	무승부
			//	비긴 경우 퀘스트 하달. 기존 퀘스트 삭제.
			switch(g_ClanWar_List[p_List_N].m_War_Type)
			{
			case 0:
				//	A,B 팀 방장의 퀘스트 삭제.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
						//	동점 퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2817);
					}
					else
					{
						// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_AM);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2817);
						}
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
						//	동점 퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2817);
					}
					else
					{
						// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_BM);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2817);
						}
					}
				}
				//	무승부 승리 메시지 전송
				GF_Send_CLANWAR_Progress(p_List_N + 1,4);
				break;
			case 1:
				//	A,B 팀 방장의 퀘스트 삭제.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
						//	동점 퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2818);
					}
					else
					{
						// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_AM);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2818);
						}
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
						//	동점 퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2818);
					}
					else
					{
						// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_BM);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2818);
						}
					}
				}
				//	무승부 승리 메시지 전송
				GF_Send_CLANWAR_Progress(p_List_N + 1,4);
				break;
			case 2:
				//	A,B 팀 방장의 퀘스트 삭제.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
						//	동점 퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2819);
					}
					else
					{
						// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_AM);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2819);
						}
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
						//	동점 퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2819);
					}
					else
					{	// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_BM);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2819);
						}
					}
				}
				//	무승부 승리 메시지 전송
				GF_Send_CLANWAR_Progress(p_List_N + 1,4);
				break;
			}
			break;
		}
	case 1000:
		{
			//	B 팀 승리.
			switch(g_ClanWar_List[p_List_N].m_War_Type)
			{
			case 0:
				//	A,B 팀 방장의 퀘스트 삭제.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
						//	승리 퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2814);
					}
					else
					{
						// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_BM);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2814);
						}
					}
				}
				break;
			case 1:
				//	A,B 팀 방장의 퀘스트 삭제.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
						//	승리 퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2815);
					}
					else
					{
						// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_BM);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2815);
						}
					}
				}
				break;
			case 2:
				//	A,B 팀 방장의 퀘스트 삭제.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
						//	승리 퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2816);
					}
					else
					{
						// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_BM);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2816);
						}
					}
				}
				break;
			}
			//	B팀 승리 메시지 전송
			GF_Send_CLANWAR_Progress(p_List_N + 1,3);
			break;
		}
	case 2000:
		{
			//	A 팀 승리.
			switch(g_ClanWar_List[p_List_N].m_War_Type)
			{
			case 0:
				//	A,B 팀 방장의 퀘스트 삭제.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
						//	승리 퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2814);
					}
					else
					{
						// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_AM);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2814);
						}
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
					}
				}
				break;
			case 1:
				//	A,B 팀 방장의 퀘스트 삭제.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
						//	승리 퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2815);
					}
					else
					{
						// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_AM);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2815);
						}
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
					}
				}
				break;
			case 2:
				//	A,B 팀 방장의 퀘스트 삭제.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
						//	승리 퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2816);
					}
					else
					{
						// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
						// 다음 순위 클랜 유저를 얻어서 처리한다.
						t_CM = GF_Clan_Master_Sub_Fine(t_AM);
						if(t_CM > 0)
						{
							//	퀘스트 등록
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2816);
						}
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B 팀 마스터 찾기
					if(t_CM > 0)
					{
						//	퀘스트 지운다.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
					}
				}
				break;
			}
			//	A팀 승리 메시지 전송
			GF_Send_CLANWAR_Progress(p_List_N + 1,2);
			break;
		}
	default:
		return false;
	}
	return true;
}

////	클랜전 포기 처리 메시지
BOOL	GF_ClanWar_GiveUP(DWORD p_List_N,DWORD p_Team)
{
	//	포기한 팀의 반대 편에게 승리 메시지 보냄
	if(p_List_N == 0)  return false;
	p_List_N = p_List_N - 1;
	//	A,B팀 클랜 리스트 번호 찾기
	DWORD	t_n = 0;
	DWORD	t_ID = 0;
	//	팀번호 확인
	if(p_Team == 1000)
	{
		//	B 팀 승리
		//	해당 클랜 리스트 번호 얻기
		t_n = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
		if(t_n > 0)
		{
			t_n = t_n - 1;
			//	지정 클랜의 마스터 얻기
			t_ID = GF_Clan_Master_Fine(t_n);	// B 팀 마스터 찾기
			if(t_ID > 0)
			{
				//	마스터 에게 오류 메시지 전송
				g_User_List[t_ID - 1]->Send_gsv_CLANWAR_Err(CWAR_OPPONENT_MASTER_ALREADY_GIVEUP);
			}
		}
	}
	else if(p_Team == 2000)
	{
		//	A 팀 승리
		t_n = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
		if(t_n > 0)
		{
			t_n = t_n - 1;
			t_ID = GF_Clan_Master_Fine(t_n);	// A 팀 마스터 찾기
			if(t_ID > 0)
			{
				//	마스터 찾음
				g_User_List[t_ID - 1]->Send_gsv_CLANWAR_Err(CWAR_OPPONENT_MASTER_ALREADY_GIVEUP);
			}
		}
	}
	return true;
}

////	클랜전 참여중인 클랜 확인 함수
DWORD	GF_Get_CLANWAR_Check(DWORD p_Clan_N,DWORD p_User_LV)
{
	if(p_Clan_N == 0)		return 0;
	DWORD	Temp_HI = 0;
	DWORD	Temp_LO = 0;
	DWORD	Temp_DW = 0;
	// 확인
	for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
	{
		//	해당 존이 게임 진행 중인지 검사 한다.
		if(g_ClanWar_List[i].m_CW_Npc != NULL)
		{
			//	클랜전 진행중
			if(g_ClanWar_List[i].m_Play_Type == 4)
			{
				//	본인 클랜이 참여중인지 확인한다.
				if(g_ClanWar_List[i].m_Clan_A == p_Clan_N)
				{
					//	워프 처리 한다.
					if(GF_Clan_War_Check_LV(p_User_LV,g_ClanWar_List[i].m_War_Type))
						Temp_HI = 1;
					else
						Temp_HI = 3;	// LV 제한.
					//	방번호 넣기
					Temp_LO = i+1;
					//	리턴값 설정 : 상위 값은 팀번호, 하위 값은 방번호이다.
					Temp_DW = (Temp_HI<<16) + Temp_LO;

					return Temp_DW;
				}
				if(g_ClanWar_List[i].m_Clan_B == p_Clan_N)
				{
					//	워프 처리 한다.
					if(GF_Clan_War_Check_LV(p_User_LV,g_ClanWar_List[i].m_War_Type))
						Temp_HI = 2;
					else
						Temp_HI = 3;	// LV 제한.
					//	방번호 넣기
					Temp_LO = i+1;
					//	리턴값 설정 : 상위 값은 팀번호, 하위 값은 방번호이다.
					Temp_DW = (Temp_HI<<16) + Temp_LO;

					return Temp_DW;
				}
			}
			//	클랜전 신청 중인 방도 체크한다.
			else if(g_ClanWar_List[i].m_Play_Type == 1)
			{
				//	신청 중인 방이 있다. 방번호 만 넘겨준다.
				if(g_ClanWar_List[i].m_Clan_A == p_Clan_N)
				{
					return i+1;
				}
				if(g_ClanWar_List[i].m_Clan_B == p_Clan_N)
				{
					return i+1;
				}
			}
		}
	}

	return 0;
}

////	클랜전 참여 유저에 진행 메시지 전송한다.
BOOL	GF_Send_CLANWAR_Progress(DWORD p_List_N,DWORD p_Type)
{
	if(p_List_N == 0) return false;
	p_List_N = p_List_N - 1;
	if(p_List_N >= DF_MAX_CWAR_LIST) return false;

	//	클랜전에 참여하고 있는 유저들에게 메시지 보낸다.
	DWORD	Temp_CN = 0;
	DWORD	Temp_ID = 0;

	//	DB에 먼저 기록 하기
	switch(p_Type)
	{
	case 2:
		// A팀 승리
		GF_DB_CWar_Insert(4,2,g_ClanWar_List[p_List_N].m_Bat_Coin,g_ClanWar_List[p_List_N].m_Clan_A,g_ClanWar_List[p_List_N].m_Clan_B,NULL);
		//	A,B 팀 승점 처리 추가.
		break;
	case 3:
		//	B팀 승리
		GF_DB_CWar_Insert(4,3,g_ClanWar_List[p_List_N].m_Bat_Coin,g_ClanWar_List[p_List_N].m_Clan_A,g_ClanWar_List[p_List_N].m_Clan_B,NULL);
		//	A,B 팀 승점 처리 추가.
		break;
	case 4:
		//	클랜전 결과 비김 처리 DB 저장.
		GF_DB_CWar_Insert(4,1,g_ClanWar_List[p_List_N].m_Bat_Coin,g_ClanWar_List[p_List_N].m_Clan_A,g_ClanWar_List[p_List_N].m_Clan_B,NULL);
		//	A,B 팀 승점 처리 추가.
		break;
	}

	//	A팀 B팀 대표 유저.
	LPVOID	p_ATeam_U = NULL;
	LPVOID	p_BTeam_U = NULL;

	//	A팀 클랜원들에게 메시지 보냄
	Temp_CN = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);

	//	클랜 리스트 번호 얻음
	if(Temp_CN > 0)
	{
		Temp_CN = Temp_CN - 1;
		// for(DWORD i = 0; i < 15;i++)
		// (20070125:남병철) : 클랜원 수치의 15명 하드코딩을 CW_USER_NO의 수치로 변경 (최대 50명까지 가능)
		for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0]; i++ )
		{
			//	클랜 유저 리스트 번호 얻기
			Temp_ID = g_Clan_List[Temp_CN].m_User_IDX[i];
			if(0 < Temp_ID)
			{
				//	해당 존에 있는지 확인
				if(g_User_List[Temp_ID - 1]->m_nZoneNO == g_ClanWar_List[p_List_N].m_ZoneNO)
				{
					//	메시지 보낸다.
					g_User_List[Temp_ID - 1]->Send_gsv_CLANWAR_Progress((WORD)p_Type);
					//	클랜의 처음 검색 유저 선택
					if(p_ATeam_U == NULL)	p_ATeam_U = (LPVOID)g_User_List[Temp_ID - 1];
				}
				//	클랜장인 경우
				if(g_User_List[Temp_ID - 1]->Is_ClanMASTER())
				{
					g_User_List[Temp_ID - 1]->m_Quests.m_nClanWarVAR[6] = 0;
					//	클랜 마스터 선택
					p_ATeam_U = (LPVOID)g_User_List[Temp_ID - 1];
				}
			}
		}
	}
	//	B팀 클랜원들에게 메시지 보냄
	Temp_CN = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
	//	클랜 리스트 번호 얻음
	if(Temp_CN > 0)
	{
		Temp_CN = Temp_CN - 1;
		// for(DWORD i = 0; i < 15;i++)
		// (20070125:남병철) : 클랜원 수치의 15명 하드코딩을 CW_USER_NO의 수치로 변경 (최대 50명까지 가능)
		for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0]; i++ )
		{
			//	클랜 유저 리스트 번호 얻기
			Temp_ID = g_Clan_List[Temp_CN].m_User_IDX[i];
			if(0 < Temp_ID)
			{
				//	해당 존에 있는지 확인
				if(g_User_List[Temp_ID - 1]->m_nZoneNO == g_ClanWar_List[p_List_N].m_ZoneNO)
				{
					//	메시지 보낸다.
					g_User_List[Temp_ID - 1]->Send_gsv_CLANWAR_Progress((WORD)p_Type);
					//	클랜의 처음 검색 유저 선택
					if(p_BTeam_U == NULL)	p_BTeam_U = (LPVOID)g_User_List[Temp_ID - 1];
				}
				//	클랜장인 경우
				if(g_User_List[Temp_ID - 1]->Is_ClanMASTER())
				{
					g_User_List[Temp_ID - 1]->m_Quests.m_nClanWarVAR[6] = 0;
					//	클랜 마스터 선택
					p_BTeam_U = (LPVOID)g_User_List[Temp_ID - 1];
				}
			}
		}
	}

	////	클랜 랭킹 포인트 변경
	GF_Set_ClanWar_RankPoint(p_Type,p_ATeam_U,p_BTeam_U,&g_ClanWar_List[p_List_N]);//.m_War_Type);

	return true;
}


////	클랜전 부활 위치 얻기
BOOL	GF_Get_ClanWar_Revival_Pos(DWORD p_List_N,DWORD pTeam,float &Out_x,float &Out_Y)
{
	//	오류 처리.
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	if(pTeam == 1000)
	{
		//	A 팀 부활 위치로
		Out_x = g_APosREVIVAL.x;
		Out_Y = g_APosREVIVAL.y;
		//Out_x = g_ClanWar_List[p_List_N - 1].m_APosREVIVAL.x;
		//Out_Y = g_ClanWar_List[p_List_N - 1].m_APosREVIVAL.y;
		return true;
	}
	else
	{
		//	B 팀 부활 위치로
		Out_x = g_BPosREVIVAL.x;
		Out_Y = g_BPosREVIVAL.y;
		//Out_x = g_ClanWar_List[p_List_N - 1].m_BPosREVIVAL.x;
		//Out_Y = g_ClanWar_List[p_List_N - 1].m_BPosREVIVAL.y;
		return true;
	}
	return false;
}

////	클랜전 맵에 GATE NPC 조정 처리 
BOOL	GF_Set_ClanWar_Gate_NPC(LPVOID p_User,DWORD p_List_N,BOOL p_Type)
{
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	p_List_N = p_List_N - 1;
	//	문을 열고 닫도록 처리한다.
	if(p_User == NULL)
	{
		//	p_List_N 존의 모든 유저에게 전달
		char	SendBuffer[512];
		ZeroMemory(SendBuffer,512);
		gsv_cli_USER_MSG_1LV	*Send_Data;
		//	전송 버퍼를 데이터 형식으로 형변환..
		Send_Data = (gsv_cli_USER_MSG_1LV *)SendBuffer;

		//	메시지 데이터 넣는다.
		Send_Data->m_wType = GSV_USER_MSG_1LV;
		Send_Data->m_nSize = sizeof(gsv_cli_USER_MSG_1LV);
		Send_Data->m_Sub_Type = 100;	// 문 컨트롤 변수
		if(p_Type)
		{
			//	기존 상태 전송
			if(g_ClanWar_List[p_List_N].m_Gate_TF)
				Send_Data->m_Data = 2;	// 열려있는 상태 출력.
			else
				Send_Data->m_Data = g_ClanWar_List[p_List_N].m_Gate_TF;		// ON/OFF 변수
		}
		else
		{
			Send_Data->m_Data = g_ClanWar_List[p_List_N].m_Gate_TF;		// ON/OFF 변수
		}

		//	메시지 해당 존에 전달.
		g_pZoneLIST->Send_gsv_All_User_Data(g_ClanWar_List[p_List_N].m_ZoneNO,SendBuffer);
	}
	else
	{
		//	p_User 유저에게 전달.
		classUSER	*Temp_User;
		Temp_User = (classUSER *)p_User;
		if(p_Type)
		{
			if(g_ClanWar_List[p_List_N].m_Gate_TF)
                Temp_User->Send_gsv_User_MSG_1LV(100,2);
			else
                Temp_User->Send_gsv_User_MSG_1LV(100,g_ClanWar_List[p_List_N].m_Gate_TF);
		}
		else
		{
			Temp_User->Send_gsv_User_MSG_1LV(100,g_ClanWar_List[p_List_N].m_Gate_TF);
		}
	}

	return true;
}

////	클랜전 참여 인원 증가
BOOL	GF_Add_ClanWar_Number(DWORD p_List_N,DWORD pTeam)
{
	//	전달 받은 방리스트 번호 확인.
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	p_List_N = p_List_N - 1;
	//	지정된 팀번호 확인
	switch(pTeam)
	{
	case 1000:
		{
			//	A 팀 인원 증가.
			switch(g_ClanWar_List[p_List_N].m_War_Type)
			{
			case 0:
				{
					if(g_ClanWar_List[p_List_N].m_ATeam_No < G_CW_MAX_USER_NO[0][0])
					{
						g_ClanWar_List[p_List_N].m_ATeam_No++;
						return true;
					}
					else
						return false;
					break;
				}
			case 1:
				{
					if(g_ClanWar_List[p_List_N].m_ATeam_No < G_CW_MAX_USER_NO[0][0])
					{
						g_ClanWar_List[p_List_N].m_ATeam_No++;
						return true;
					}
					else
						return false;
					break;
				}
			case 2:
				{
					if(g_ClanWar_List[p_List_N].m_ATeam_No < G_CW_MAX_USER_NO[0][0])
					{
						g_ClanWar_List[p_List_N].m_ATeam_No++;
						return true;
					}
					else
						return false;
					break;
				}
			}
			break;
		}
	case 2000:
		{
			//	B 팀 인원 증가.
			switch(g_ClanWar_List[p_List_N].m_War_Type)
			{
			case 0:
				{
					if(g_ClanWar_List[p_List_N].m_BTeam_No < G_CW_MAX_USER_NO[0][0])
					{
						g_ClanWar_List[p_List_N].m_BTeam_No++;
						return true;
					}
					else
						return false;
					break;
				}
			case 1:
				{
					if(g_ClanWar_List[p_List_N].m_BTeam_No < G_CW_MAX_USER_NO[0][0])
					{
						g_ClanWar_List[p_List_N].m_BTeam_No++;
						return true;
					}
					else
						return false;
					break;
				}
			case 2:
				{
					if(g_ClanWar_List[p_List_N].m_BTeam_No < G_CW_MAX_USER_NO[0][0])
					{
						g_ClanWar_List[p_List_N].m_BTeam_No++;
						return true;
					}
					else
						return false;
					break;
				}
			}
			break;
		}
	}
	return false;
}

////	클랜전 참여 인원 감소
BOOL	GF_Sub_ClanWar_Number(DWORD p_List_N,DWORD pTeam)
{
	//	전달 받은 방리스트 번호 확인.
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	p_List_N = p_List_N - 1;
	//	지정된 팀번호 확인
	switch(pTeam)
	{
	case 1000:
		{
			//	A 팀 인원 감소.
			switch(g_ClanWar_List[p_List_N].m_War_Type)
			{
			case 0:
				{
					if(g_ClanWar_List[p_List_N].m_ATeam_No > 0)
						g_ClanWar_List[p_List_N].m_ATeam_No--;
					return true;
				}
			case 1:
				{
					if(g_ClanWar_List[p_List_N].m_ATeam_No > 0)
						g_ClanWar_List[p_List_N].m_ATeam_No--;
					return true;
				}
			case 2:
				{
					if(g_ClanWar_List[p_List_N].m_ATeam_No > 0)
						g_ClanWar_List[p_List_N].m_ATeam_No--;
					return true;
				}
			}
			break;
		}
	case 2000:
		{
			//	B 팀 인원 감소.
			switch(g_ClanWar_List[p_List_N].m_War_Type)
			{
			case 0:
				{
					if(g_ClanWar_List[p_List_N].m_BTeam_No > 0)
						g_ClanWar_List[p_List_N].m_BTeam_No--;
					return true;
				}
			case 1:
				{
					if(g_ClanWar_List[p_List_N].m_BTeam_No > 0)
						g_ClanWar_List[p_List_N].m_BTeam_No--;
					return true;
				}
			case 2:
				{
					if(g_ClanWar_List[p_List_N].m_BTeam_No > 0)
						g_ClanWar_List[p_List_N].m_BTeam_No--;
					return true;
				}
			}
			break;
		}
	}
	return false;
}

////	클랜전 포인트 처리 부분
BOOL	GF_Set_ClanWar_RankPoint(DWORD p_Type,LPVOID p_User_A,LPVOID p_User_B, void* vp_ClanWar_Set )//DWORD	WarType)
{
	// (20061230:남병철) : 보상 포인트 추가로 인해 수정
	GF_ClanWar_Set* p_ClanWar_Set = (GF_ClanWar_Set*)vp_ClanWar_Set;
	DWORD WarType = p_ClanWar_Set->m_War_Type;

	//	유저 클래스로 연결.
	classUSER *T_User_A;
	classUSER *T_User_B;
	T_User_A = (classUSER *)p_User_A;
	T_User_B = (classUSER *)p_User_B;
	//	각각의 클랜 점수 보관 변수
	int	Rank_A = 1000;
	int	Rank_B = 1000;
	BOOL	NewA_TF = true;
	BOOL	NewB_TF = true;
	if(T_User_A != NULL)
	{
		if(T_User_A->m_CLAN.m_iRankPoint > 0)
		{
			Rank_A = T_User_A->m_CLAN.m_iRankPoint;
			NewA_TF = false;
		}
	}
	if(T_User_B != NULL)
	{
		if(T_User_B->m_CLAN.m_iRankPoint > 0)
		{
			Rank_B = T_User_B->m_CLAN.m_iRankPoint;
			NewB_TF = false;
		}
	}
	//	비율 얻는다.
	float	fRate_A = GF_Get_Rank_Rate(Rank_A,Rank_B);
	float	fRate_B = 1.0f - fRate_A;

	//	포인트 계산
	float	fLost = 0.0f,fWin = 0.0f;

	//	렙제한에 따른 비율 조정
	float	fLost_Rat = 1.0f;
	float	fWin_Rat = 1.0f;

	//	변경되는 값.
	int	i_Add_Rate = 0;

	switch(WarType)
	{
	case 0:
		fWin_Rat = 0.2f;
		fLost_Rat = 0.5f;
		break;
	case 1:
		fWin_Rat = 0.5f;
		fLost_Rat = 0.8f;
		break;
	case 2:
		fWin_Rat = 1.0f;
		fLost_Rat = 1.0f;
		break;
	default:
		break;
	}

	//	승점 처리 부분.
	switch(p_Type)
	{
	case 2:
		//	A팀 승리
		fLost = 30.0f*fRate_B + ((float)Rank_B)*0.01f;	// 진팀의 손실 점수
		fWin = fLost*(1.0f - fRate_A) + 1.0f;	// 이긴팀의 손실 점수
		if(T_User_A != NULL)
		{
			//	점수 지급한다.
			i_Add_Rate = (int)(fWin*fWin_Rat);
			Rank_A = Rank_A + i_Add_Rate;
			T_User_A->m_CLAN.m_iRankPoint = Rank_A;
			//	해당 클랜의 랭크 점수 변경 메시지 전송
			if(NewA_TF)
				T_User_A->SetClanRank(Rank_A);
			else
				T_User_A->SetClanRank(i_Add_Rate);


#ifdef __CLAN_REWARD_POINT
			// (20061229:남병철) : 클랜전 보상 포인트 적용
			switch( WarType )
			{
			case 0: // 50만 줄리
				p_ClanWar_Set->m_dwATeamRewardPoint = 20;
				break;

			case 1: // 100만 줄리
				p_ClanWar_Set->m_dwATeamRewardPoint = 50;
				break;

			case 2: // 1000만 줄리
				p_ClanWar_Set->m_dwATeamRewardPoint = 100;
				break;
			}
#endif
		}

		if(T_User_B != NULL)
		{
			//	점수 차감한다.
			i_Add_Rate = - (int)(fLost*fLost_Rat);
			if((Rank_B + i_Add_Rate) < 0)
			{
				//	0에 마춘다.
				i_Add_Rate = -Rank_B;
				Rank_B = 0;
			}
			else
                Rank_B = Rank_B + i_Add_Rate;

			T_User_B->m_CLAN.m_iRankPoint = Rank_B;

			//	해당 클랜의 랭크 점수 변경 메시지 전송
			if(NewB_TF)
				T_User_B->SetClanRank(Rank_B);
			else
				T_User_B->SetClanRank(i_Add_Rate);


#ifdef __CLAN_REWARD_POINT
			// (20061229:남병철) : 클랜전 보상 포인트 적용
			switch( WarType )
			{
			case 0: // 50만 줄리
				p_ClanWar_Set->m_dwBTeamRewardPoint = 10;
				break;

			case 1: // 100만 줄리
				p_ClanWar_Set->m_dwBTeamRewardPoint = 10;
				break;

			case 2: // 1000만 줄리
				p_ClanWar_Set->m_dwBTeamRewardPoint = 10;
				break;
			}
#endif
		}
		break;
	case 3:
		//	B팀 승리.
		fLost = 30.0f*fRate_A + ((float)Rank_A)*0.01f;	// 진팀의 손실 점수
		fWin = fLost*(1.0f - fRate_B) + 1.0f;	// 이긴팀의 손실 점수
		if(T_User_B != NULL)
		{
			//	점수 지급한다.
			i_Add_Rate = (int)(fWin*fWin_Rat);
			Rank_B = Rank_B + i_Add_Rate;
			T_User_B->m_CLAN.m_iRankPoint = Rank_B;
			//	해당 클랜의 랭크 점수 변경 메시지 전송
			if(NewB_TF)
				T_User_B->SetClanRank(Rank_B);
			else
				T_User_B->SetClanRank(i_Add_Rate);


#ifdef __CLAN_REWARD_POINT
			// (20061229:남병철) : 클랜전 보상 포인트 적용
			switch( WarType )
			{
			case 0: // 50만 줄리
				p_ClanWar_Set->m_dwBTeamRewardPoint = 20;
				break;

			case 1: // 100만 줄리
				p_ClanWar_Set->m_dwBTeamRewardPoint = 50;
				break;

			case 2: // 1000만 줄리
				p_ClanWar_Set->m_dwBTeamRewardPoint = 100;
				break;
			}
#endif
		}
		if(T_User_A != NULL)
		{
			//	점수 차감한다.
			i_Add_Rate = - (int)(fLost*fLost_Rat);
			if((Rank_A + i_Add_Rate) < 0)
			{
				i_Add_Rate = -Rank_A;
				Rank_A = 0;
			}
			else
				Rank_A = Rank_A + i_Add_Rate;
			T_User_A->m_CLAN.m_iRankPoint = Rank_A;
			//	해당 클랜의 랭크 점수 변경 메시지 전송
			if(NewA_TF)
				T_User_A->SetClanRank(Rank_A);
			else
				T_User_A->SetClanRank(i_Add_Rate);


#ifdef __CLAN_REWARD_POINT
			// (20061229:남병철) : 클랜전 보상 포인트 적용
			switch( WarType )
			{
			case 0: // 50만 줄리
				p_ClanWar_Set->m_dwATeamRewardPoint = 10;
				break;

			case 1: // 100만 줄리
				p_ClanWar_Set->m_dwATeamRewardPoint = 10;
				break;

			case 2: // 1000만 줄리
				p_ClanWar_Set->m_dwATeamRewardPoint = 10;
				break;
			}
#endif
		}
		break;

	case 4:
#ifdef __CLAN_REWARD_POINT
		// 클랜전 결과 비김 처리 : 점수 변경 없음
		p_ClanWar_Set->m_dwATeamRewardPoint = 0;
		p_ClanWar_Set->m_dwBTeamRewardPoint = 0;
#endif
		break;

	default:
		return false;
	}


#ifdef __CLAN_REWARD_POINT
	T_User_A->AddClanRewardPoint( p_ClanWar_Set->m_dwATeamRewardPoint );
	T_User_B->AddClanRewardPoint( p_ClanWar_Set->m_dwBTeamRewardPoint );
#endif


	return true;
}

////	클랜 포인트 점수차이 얻기
float	GF_Get_Rank_Rate(int Ateam_P,int BTeam_P)
{
	//	A팀을 기준으로 처리한다.
	int	iRate = BTeam_P - Ateam_P;
	//	비율 처리한다.
	if(iRate >= 400)		return 0.091f;
	else if(iRate >= 350)	return 0.118f;
	else if(iRate >= 300)	return 0.151f;
	else if(iRate >= 250)	return 0.192f;
	else if(iRate >= 200)	return 0.24f;
	else if(iRate >= 150)	return 0.297f;
	else if(iRate >= 100)	return 0.36f;
	else if(iRate >= 50)	return 0.429f;
	else if(iRate > -50)	return 0.5f;
	else if(iRate > -100)	return 0.571f;
	else if(iRate > -150)	return 0.64f;
	else if(iRate > -200)	return 0.703f;
	else if(iRate > -250)	return 0.76f;
	else if(iRate > -300)	return 0.808f;
	else if(iRate > -350)	return 0.849f;
	else if(iRate > -400)	return 0.882f;
	else					return 0.909f;
	return 0.5f;
}

// (20070104:남병철) : 클랜 보상 포인트 적용
/*
BOOL	GF_Add_ClanWar_RewardPoint( int iClanID, int iRewardPoint )
{
	DWORD	Temp_ID = 0;

	for(DWORD i = 0; i < 15;i++)
	{
		//	유저 접속 확인.
		Temp_ID = g_Clan_List[iClanID].m_User_IDX[i];
		if(0 < Temp_ID)
		{
			// 모든 클랜원의 보상 포인트를 증가시킨다.
//			g_User_List[Temp_ID - 1]->m_GrowAbility.AddClanRewardPoint( iRewardPoint );

			// 개별 클라이언트 유저에게 갱신 전달
			//g_User_List[Temp_ID - 1]->Send_
			//this->Send_SetCLAN ( RESULT_CLAN_MY_DATA, pUSER );
		}
	}

	return TRUE;
}
*/



//#endif

