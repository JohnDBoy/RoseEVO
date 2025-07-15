
/*
	�ۼ��� �迵ȯ
*/

// #ifdef __CLAN_WAR_SET			// Ŭ���� �߰� ����
#include <stdio.h>

#include "GF_Define.h"
#include "GF_Data.h"

/*
	Ŭ���� ����/ó�� �Լ�
*/
int	CW_Warp_AX,CW_Warp_AY,CW_Warp_BX,CW_Warp_BY;
int	CW_Revival_AX,CW_Revival_AY,CW_Revival_BX,CW_Revival_BY;

////	Ŭ���� ���� �ʱ�ȭ
BOOL	GF_Clan_War_Init()
{
	//	Ŭ���� ����Ʈ
	ZeroMemory(g_ClanWar_List,DF_MAX_CWAR_LIST*sizeof(GF_ClanWar_Set));
	ZeroMemory(G_CW_BAT_MONEY,3*8);	// ���� �ݾ�
	ZeroMemory(G_CW_MAX_LEVEL,3*4);	// ���� ����.
	ZeroMemory(G_CW_MAX_USER_NO,2*3*4);	// �ο� ����

	//	���� �б�
	if( !GF_Clan_War_Ini_Read() )
		return false;

	//	Ŭ���� �ʵ� ������ �ʱ�ȭ
	if( !GF_CW_MOB_Init() )
		return false;

	return true;
}

////	Ŭ���� ���� ini �б�
BOOL	GF_Clan_War_Ini_Read()
{
	// ini ���� �б�.
	char	Temp_File[MAX_PATH];
	ZeroMemory(Temp_File,MAX_PATH);
	wsprintf( Temp_File, "%s\\SHO_GS_DATA.ini", G_Main_Dir );

	// INI ���� ���� ���� (-_-;;)
	FILE* fp = NULL;
	if( (fp = fopen( Temp_File, "rb" )) )
	{ // �����ϴ� ����
		fclose(fp);
	}
	else
	{ // NULL
		return false;
	}

	// ���� ��ġ
	// ���� ��ġ
	CW_Warp_AX = Get_ini_Read(Temp_File,"CLANWAR_ZONE_WARP_A","CW_POS_X");
	CW_Warp_AY = Get_ini_Read(Temp_File,"CLANWAR_ZONE_WARP_A","CW_POS_Y");
	CW_Warp_BX = Get_ini_Read(Temp_File,"CLANWAR_ZONE_WARP_B","CW_POS_X");
	CW_Warp_BY = Get_ini_Read(Temp_File,"CLANWAR_ZONE_WARP_B","CW_POS_Y");
	// ��Ȱ ��ġ
	CW_Revival_AX = Get_ini_Read(Temp_File,"CLANWAR_ZONE_REVIVAL_A","CW_POS_X");
	g_APosREVIVAL.x = CW_Revival_AX;
	CW_Revival_AY = Get_ini_Read(Temp_File,"CLANWAR_ZONE_REVIVAL_A","CW_POS_Y");
	g_APosREVIVAL.y = CW_Revival_AY;
	CW_Revival_BX = Get_ini_Read(Temp_File,"CLANWAR_ZONE_REVIVAL_B","CW_POS_X");
	g_BPosREVIVAL.x = CW_Revival_BX;
	CW_Revival_BY = Get_ini_Read(Temp_File,"CLANWAR_ZONE_REVIVAL_B","CW_POS_Y");
	g_BPosREVIVAL.y = CW_Revival_BY;
	//	��ȯ ��ġ
	int	Temp_z = Get_ini_Read(Temp_File,"CLANWAR_RETURN","CW_ZONE");
	int	Temp_x = Get_ini_Read(Temp_File,"CLANWAR_RETURN","CW_POS_X");
	int	Temp_y = Get_ini_Read(Temp_File,"CLANWAR_RETURN","CW_POS_Y");

	//	Ŭ���� ���� �ݾ� ����.
	G_CW_BAT_MONEY[0] = (__int64)Get_ini_Read(Temp_File,"CLANWAR_BAT","CW_BAT_1");
	G_CW_BAT_MONEY[1] = (__int64)Get_ini_Read(Temp_File,"CLANWAR_BAT","CW_BAT_2");
	G_CW_BAT_MONEY[2] = (__int64)Get_ini_Read(Temp_File,"CLANWAR_BAT","CW_BAT_3");
	//	Ŭ���� ���� �ð� ���� �б�
	DF_CWAR_PLAY_TIME = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_TIME","CW_PLAY_TIME");
	DF_CWAR_END_TIME = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_TIME","CW_END_TIME");

	//	Ŭ���� ���� ����.
	G_CW_MAX_LEVEL[0] = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_LEVEL","CW_LEVEL_1");
	G_CW_MAX_LEVEL[1] = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_LEVEL","CW_LEVEL_2");
	G_CW_MAX_LEVEL[2] = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_LEVEL","CW_LEVEL_3");



	//	Ŭ���� �ο� ���� �б�
	char	Temp_S[50];
	for(DWORD k = 0 ; k < 3; k++)
	{
		// (20070125:����ö) : �� Ŭ���� Ÿ�Կ� �°� ������ ó���� ������ ���� �ʾƼ� �ϳ��� ����
		// << Ŭ���� �����ο� ���� ���� >>
		// EX) �Ϲ������� �ش� Ŭ������ ������ ã��� �� Ÿ���� �����Ͽ� Ŭ���� �����͸� ã�� ���
		//     ������ GF_Clan_Master_Fine()�� ����ϰԵǾ� �� Ÿ�� ���п� �´� Ŭ���� ���� �ٷ� �� ����.
		//     �׷��Ƿ�, �� Ŭ���� Ÿ���� �ϳ��� ��ġ ���Ѽ� Ŭ������ 50���� �� �� �ְ� �����Ѵ�.
		ZeroMemory( Temp_S, 50 );
		wsprintf( Temp_S, "CW_USER_NO", k+1 );
		G_CW_MAX_USER_NO[k][0] = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_USER_NUMBER",Temp_S);
		G_CW_MAX_USER_NO[k][1] = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_USER_NUMBER",Temp_S);

		////	A�� �ο� �б�
		//ZeroMemory(Temp_S,50);
		//wsprintf(Temp_S,"CW_USER_NO_%d_A",k+1);
		//G_CW_MAX_USER_NO[k][0] = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_USER_NUMBER",Temp_S);

		////	B�� �ο� �б�
		//ZeroMemory(Temp_S,50);
		//wsprintf(Temp_S,"CW_USER_NO_%d_B",k+1);
		//G_CW_MAX_USER_NO[k][1] = (DWORD)Get_ini_Read(Temp_File,"CLANWAR_USER_NUMBER",Temp_S);
	}
	//G_CW_MAX_USER_NO[0][0] = 50;



	if(CW_Warp_AX < 0)		MessageBox(NULL,"Ŭ���� �������� �б� ����!","���!!",MB_OK);

	//	Ŭ���� �� ��ġ �ʱ�ȭ
	for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
	{
		g_ClanWar_List[i].m_ZoneNO = (WORD)(101 + i);
		//	���� ��ġ
		g_ClanWar_List[i].m_APosWARP.x = (float)CW_Warp_AX;
		g_ClanWar_List[i].m_APosWARP.y = (float)CW_Warp_AY;
		g_ClanWar_List[i].m_BPosWARP.x = (float)CW_Warp_BX;
		g_ClanWar_List[i].m_BPosWARP.y = (float)CW_Warp_BY;
		//	��Ȱ ��ġ
		g_ClanWar_List[i].m_APosREVIVAL.x = (float)CW_Revival_AX;
		g_ClanWar_List[i].m_APosREVIVAL.y = (float)CW_Revival_AY;
		g_ClanWar_List[i].m_BPosREVIVAL.x = (float)CW_Revival_BX;
		g_ClanWar_List[i].m_BPosREVIVAL.y = (float)CW_Revival_BY;
	}
	//	Ŭ���� ���� ��ġ ����
	g_ClanWar_ReCall.m_ZoneNO = (WORD)Temp_z;
	g_ClanWar_ReCall.m_Pos.x = (float)Temp_x;
	g_ClanWar_ReCall.m_Pos.y = (float)Temp_y;

	return true;
}

////	Ŭ���� �ʵ� Ȯ��
DWORD	GF_Clan_War_Zone_Is(DWORD p_Zone_N)
{
	// Ȯ��
	for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
	{
		if(g_ClanWar_List[i].m_ZoneNO == (WORD)p_Zone_N)
		{
			ZTRACE( "Ŭ���� ����ȣ:%d", i+1 );
			return (i+1);	// ����ȣ ��ȯ.
		}
	}

	return 0;
}

////	Ŭ���� �ʵ� �� Ȯ��.
DWORD	GF_Clan_War_Zone_Is(DWORD p_Zone_N,DWORD p_Clan_N)
{
	// Ȯ��
	for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
	{
		//	�ش� �� Ȯ���Ѵ�.
		if(g_ClanWar_List[i].m_ZoneNO == (WORD)p_Zone_N)
		{
			//	���� Ŭ���� ���������� Ȯ���Ѵ�.
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

////	Ŭ���� �� �������� Ȯ��
BOOL	GF_Clan_War_Zone_Play_Is(DWORD p_List_N)
{
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	if(g_ClanWar_List[p_List_N - 1].m_Play_Type == 4) return true;	// �÷�����
	return false;
}

////	Ŭ���� �ʵ� ���� �� �ʱ�ȭ
BOOL	GF_CW_MOB_Init()
{
	//	�ʱ�ȭ
	ZeroMemory(g_CW_MOB_List,DF_MAX_CWAR_TYPE*DF_MAX_CWAR_MOB*DF_CWAR_TEAM_N*sizeof(GF_ClanWar_MOB));
	////	Ŭ���� ���� ������ ����
	ZeroMemory(g_CW_Win_Gift,DF_MAX_CWAR_TYPE*sizeof(GF_CWar_Win_Gift));
	
	// ini ���� �б�. (INI���� ���� ������ GF_Clan_War_Ini_Read()���� äũ ����)
	char	Temp_File[MAX_PATH];
	ZeroMemory(Temp_File,MAX_PATH);
	wsprintf(Temp_File,"%s\\SHO_GS_DATA.ini",G_Main_Dir);

	//	�ӽ� ����
	char	Temp_Title[30];
	char	Temp_Name[30];
	int	Temp_Int = 0;

	//	���� �ݺ�
	for(int i = 0 ; i < DF_MAX_CWAR_TYPE; i++)
	{
		//	�� �ݺ�
		for(int t = 0 ; t < DF_CWAR_TEAM_N; t++)
		{
			//	Ÿ��Ʋ ����
			ZeroMemory(Temp_Title,30);
			if(t == 0)	wsprintf(Temp_Title,"MOB_A_%d",i+1);
			else		wsprintf(Temp_Title,"MOB_B_%d",i+1);
			//	������ �ݺ�
			for(int n = 0 ;n < DF_MAX_CWAR_MOB; n++)
			{
				//	���� ��ȣ
				ZeroMemory(Temp_Name,30);
				wsprintf(Temp_Name,"CW_MOB_%d",n+1);
				int iMOB_IDX = (DWORD)Get_ini_Read(Temp_File,Temp_Title,Temp_Name);
				g_CW_MOB_List[i][n][t].m_MOB_Idx = (DWORD)Get_ini_Read(Temp_File,Temp_Title,Temp_Name);
				//	��ġ X
				ZeroMemory(Temp_Name,30);
				wsprintf(Temp_Name,"CW_POS_%d_X",n+1);
				g_CW_MOB_List[i][n][t].m_Pos_X = (float)Get_ini_Read(Temp_File,Temp_Title,Temp_Name);
				//	��ġ Y
				ZeroMemory(Temp_Name,30);
				wsprintf(Temp_Name,"CW_POS_%d_Y",n+1);
				g_CW_MOB_List[i][n][t].m_Pos_Y = (float)Get_ini_Read(Temp_File,Temp_Title,Temp_Name);
				//	����
				ZeroMemory(Temp_Name,30);
				wsprintf(Temp_Name,"CW_NUM_%d",n+1);
				g_CW_MOB_List[i][n][t].m_Number = (DWORD)Get_ini_Read(Temp_File,Temp_Title,Temp_Name);
				//	����
				ZeroMemory(Temp_Name,30);
				wsprintf(Temp_Name,"CW_RANGE_%d",n+1);
				g_CW_MOB_List[i][n][t].m_Range = (DWORD)Get_ini_Read(Temp_File,Temp_Title,Temp_Name);
			}
		}
	}

	////	Ŭ���� ���� ���� �о� ���δ�.
	ZeroMemory(Temp_File,MAX_PATH);
	wsprintf(Temp_File,"%s\\SHO_GS_ITEM.ini",G_Main_Dir);

	// INI ���� ���� ���� (-_-;;)
	FILE* fp = NULL;
	if( (fp = fopen( Temp_File, "rb" )) )
	{ // �����ϴ� ����
		fclose(fp);
	}
	else
	{ // NULL
		return false;
	}

	////	���� �������� ó�� �Ѵ�.
	for(int i = 0 ; i < DF_MAX_CWAR_TYPE; i++)
	{
		//	Ÿ��Ʋ �̸� ����.
		ZeroMemory(Temp_Title,30);
		wsprintf(Temp_Title,"CW_ITEM_%d",i+1);
		//// ������ ���� �б�
		for(int j = 0 ; j < DF_CWAR_WIN_ITEM_N; j++)
		{
			//	�̸� ����.
			ZeroMemory(Temp_Name,30);
			wsprintf(Temp_Name,"CW_GIFT_ITEM_%d",j+1);
			Temp_Int = Get_ini_Read(Temp_File,Temp_Title,Temp_Name);
			if(Temp_Int < 0)
				g_CW_Win_Gift[i].m_Item_No[j] = 0;
			else
				g_CW_Win_Gift[i].m_Item_No[j] = (DWORD)Temp_Int;
			//	�̸� ����.
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

////	Ŭ���� ���� Ÿ�� ���..
DWORD	GF_Get_CWar_Type(DWORD	p_Type)
{
	switch(p_Type)
	{
	case 1:
	case 2:
		//	���ú� Ŭ����
		return 0;
	case 3:
	case 4:
		//	������ Ŭ����
		return 3;
	case 31:
	case 32:
		//	���ú� Ŭ����
		return 1;
	case 33:
	case 34:
		//	������ Ŭ����
		return 4;
	case 61:
	case 62:
		//	���ú� Ŭ����
		return 2;
	case 63:
	case 64:
		//	������ Ŭ����
		return 5;
	}
	return 0;
}

////	Ŭ���� ���� ���� �ݾ� ����
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
	//	Ŭ�� ���� ã�Ƽ� �ݾ��� ���ش�. [�ݾ� ������ ���� ó�� �ʿ�]
	if(!GF_Clan_Master_Money_Sub(p_Clan_Inx,return_m)) return 0;
	return return_m;
}

////	Ŭ���� ��û ��ҿ� ���� ���� ó��...
BOOL	GF_CWar_Bat_Return(LPVOID p_User)
{
	//	���ް� Ȯ��.
	if(p_User == NULL) return false;
	classUSER *Temp_User;
	Temp_User = (classUSER *)p_User;
	//	������ Ŭ�� ���� �� Ȯ���ϰ� ����.
	
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
	//	���ú��� �ʱ�ȭ
	Temp_User->m_Quests.m_nClanWarVAR[0] = 0;
	Temp_User->m_Quests.m_nClanWarVAR[6] = 0;
	Temp_User->m_Quests.m_nClanWarVAR[5] = 0;
	Temp_User->m_Quests.m_nClanWarVAR[7] = 0;
	////	������ ���� ����Ʈ ���� ���� ����
	Temp_User->Send_gsv_Quest_VAR();
	return true;
}

////	������ ������ �ݾ��� ����. [Ŭ���� ��û�� ���]
BOOL	GF_Clan_Master_Money_Sub(DWORD p_Clan_Inx,__int64 p_Money)
{
	// Ŭ�� ����Ʈ���� Ŭ�� ã��
	DWORD temp_CN = GF_Clan_Fine_List(p_Clan_Inx);
	if(temp_CN == 0) return false;
	//	Ŭ������ ������ ã��
	DWORD temp_Ms = GF_Clan_Master_Fine(temp_CN - 1);
	if(temp_Ms == 0) return false;	
	temp_Ms = temp_Ms - 1;
	//	������ �ݾ��� ����.
	if(g_User_List[temp_Ms]->m_Inventory.m_i64Money >= p_Money)
	{
		g_User_List[temp_Ms]->m_Inventory.m_i64Money = g_User_List[temp_Ms]->m_Inventory.m_i64Money - p_Money;
		g_User_List[temp_Ms]->Send_gsv_SET_MONEYONLY(GSV_SET_MONEY_ONLY);
	}
	else
		return false;
	return true;
}

////	������ ������ �ݾ��� �߰�. [������� �ʴ´�.]
BOOL	GF_Clan_Master_Money_Add(DWORD p_Clan_Inx,__int64 p_Money)
{
	// Ŭ�� ����Ʈ���� Ŭ�� ã��
	DWORD temp_CN = GF_Clan_Fine_List(p_Clan_Inx);
	if(temp_CN == 0) return false;
	//	Ŭ������ ������ ã��
	DWORD temp_Ms = GF_Clan_Master_Fine(temp_CN - 1);
	if(temp_Ms == 0) return false;	
	temp_Ms = temp_Ms - 1;
	//	������ �ݾ��� �߰�
	g_User_List[temp_Ms]->m_Inventory.m_i64Money = g_User_List[temp_Ms]->m_Inventory.m_i64Money + p_Money;
	g_User_List[temp_Ms]->Send_gsv_SET_MONEYONLY(GSV_SET_MONEY_ONLY);
	return true;
}

////	������ ������ Ŭ���� ���� ����.
BOOL	GF_Clan_Master_ClanWar_IDX(DWORD p_Clan_Inx,WORD Num,bool b_WinType,DWORD p_WarType)
{
	// Ŭ�� ����Ʈ���� Ŭ�� ã��
	DWORD temp_CN = GF_Clan_Fine_List(p_Clan_Inx);
	if(temp_CN == 0) return false;
	//	Ŭ������ ������ ã��
	DWORD temp_Ms = GF_Clan_Master_Fine(temp_CN - 1);
	if(temp_Ms == 0) 
	{
		// ������ ���࿡�� �ִ´�.
		temp_Ms = GF_Clan_Master_Sub_Fine(temp_CN - 1);
		if(temp_Ms > 0)
		{
			temp_Ms = temp_Ms - 1;
			//	������ ���࿡�� �ݾ� �ֱ�.
			g_User_List[temp_Ms]->m_Quests.m_nClanWarVAR[5] = Num;
			//	���� ������ ����
			if(b_WinType)
			{
				g_User_List[temp_Ms]->m_Quests.m_nClanWarVAR[7] = (short)(p_WarType+1);
			}
		}
	}
	else
	{
		temp_Ms = temp_Ms - 1;
		//	������ �ݾ��� �߰�
		g_User_List[temp_Ms]->m_Quests.m_nClanWarVAR[5] = Num;
		//	���� ������ ����
		if(b_WinType)
		{
			g_User_List[temp_Ms]->m_Quests.m_nClanWarVAR[7] = (short)(p_WarType+1);
		}
	}
	return true;
}

////	���� ������ Ŭ���� ���󺯼� Ȯ��
BOOL	GF_Clan_Master_ClanWar_Gift(LPVOID p_User)
{
	if(p_User == NULL) return false;
	classUSER *Temp_User;
	Temp_User = (classUSER *)p_User;
	//	���� �ݾ� ����..
	if(Temp_User->m_Quests.m_nClanWarVAR[5] > 0)
	{
		Temp_User->m_Inventory.m_i64Money =
			Temp_User->m_Inventory.m_i64Money + Temp_User->m_Quests.m_nClanWarVAR[5]*1000;
		Temp_User->m_Quests.m_nClanWarVAR[5] = 0;
		//	ä�� ��ȣ �����
		Temp_User->m_Quests.m_nClanWarVAR[6] = 0;
		Temp_User->Send_gsv_SET_MONEYONLY(GSV_SET_MONEY_ONLY);
	}
	//	���� ������ ����
	if(Temp_User->m_Quests.m_nClanWarVAR[7] > 0)
	{
		//	���� ��ȣ ����
		int t_int = Temp_User->m_Quests.m_nClanWarVAR[7];
		//	���� ��ȣ 0���� ����
		t_int = t_int - 1;
		//	���� ������ ��ȣ ����
		Temp_User->m_Quests.m_nClanWarVAR[7] = 0;
		//	���� ��ȣ �Ѱ�Ȯ��.
		if(t_int >= DF_MAX_CWAR_TYPE) return true;	// ������ �ʰ�
		//	���� ������ ����.
		for(int i = 0 ; i < DF_CWAR_WIN_ITEM_N; i++)
		{
			//	���� Ȯ��
			if(g_CW_Win_Gift[t_int].m_Item_Dup[i] > 0)
			{
				//	������ ���� �Ѵ�.
				Temp_User->In_Out_Item(true,g_CW_Win_Gift[t_int].m_Item_No[i],g_CW_Win_Gift[t_int].m_Item_Dup[i]);
			}
		}
	}
	return true;
}


////	�¸��� Ŭ���� ó��
BOOL	GF_Win_CWar_Set(LPVOID p_Room,DWORD p_Type,bool b_WinType)
{
	if(p_Room == NULL) return false;
	GF_ClanWar_Set *p_CWar_Room;
	p_CWar_Room = (GF_ClanWar_Set *)p_Room;

	////	��쿡 ���� ���� ����.
	if(p_CWar_Room->m_Bat_Coin == 0) return false;
	__int64 Temp_MN = p_CWar_Room->m_Bat_Coin;

	switch(p_Type)
 	{
	case 0:
		{
			//	30% ����. ����
			Temp_MN = Temp_MN - 3*Temp_MN/10;
			//	���º�
			Temp_MN = Temp_MN/2;	// ����� ������.
			//	A,B ����
			GF_Clan_Master_ClanWar_IDX(p_CWar_Room->m_Clan_A,(WORD)(Temp_MN/1000),false,p_CWar_Room->m_War_Type);
			GF_Clan_Master_ClanWar_IDX(p_CWar_Room->m_Clan_B,(WORD)(Temp_MN/1000),false,p_CWar_Room->m_War_Type);
			break;
		}
	case 1:
		{
			//	10% ����. ����
			Temp_MN = Temp_MN - Temp_MN/10;
			//	A �� ��
			GF_Clan_Master_ClanWar_IDX(p_CWar_Room->m_Clan_A,(WORD)(Temp_MN/1000),b_WinType,p_CWar_Room->m_War_Type);
			break;
		}
	case 2:
		{
			//	10% ����. ����
			Temp_MN = Temp_MN - Temp_MN/10;
			//	B �� ��
			GF_Clan_Master_ClanWar_IDX(p_CWar_Room->m_Clan_B,(WORD)(Temp_MN/1000),b_WinType,p_CWar_Room->m_War_Type);
			break;
		}
	default:
		return false;
	}
	return true;
}

////	Ŭ���� ��� ó��
BOOL	GF_Clan_War_Drop(DWORD p_List_N,DWORD p_Team)
{
	if(p_List_N == 0)  return false;
	p_List_N = p_List_N - 1;
	//	��� Ȯ��.
	if(g_ClanWar_List[p_List_N].m_CW_Npc == NULL) return false;
	if(g_ClanWar_List[p_List_N].m_Play_Type == 5) return false;	// ������ ó�� ���� ����
	//	����ȣ Ȯ��.
	DWORD	Temp_M = 0;
	switch(p_Team)
	{
	case 0:
		{
			//	Ŭ�������� ó���Ǵ� ���
			if(g_ClanWar_List[p_List_N].m_Win_Team == 1)
			{
				//	A �� �¸�
				GF_Win_CWar_Set((LPVOID)&g_ClanWar_List[p_List_N],1,true);
				Temp_M = 1;
			}
			else if(g_ClanWar_List[p_List_N].m_Win_Team == 2)
			{
				//	B �� �¸�
				GF_Win_CWar_Set((LPVOID)&g_ClanWar_List[p_List_N],2,true);
				Temp_M = 2;
			}
			else
			{
				//	���
				GF_Win_CWar_Set((LPVOID)&g_ClanWar_List[p_List_N],0,true);
				Temp_M = 0;
			}
			break;
		}
	case 1000:
		{
			//	��ǿ� ���� ó��
			//	B �� �¸�
			GF_Win_CWar_Set((LPVOID)&g_ClanWar_List[p_List_N],2,false);
			Temp_M = 2;
			break;
		}
	case 2000:
		{
			//	��ǿ� ���� ó��
			//	A �� �¸�
			GF_Win_CWar_Set((LPVOID)&g_ClanWar_List[p_List_N],1,false);
			Temp_M = 1;
			break;
		}
	default:
		return false;
	}
	//	���� ���� Ÿ������ ��ȯ
	g_ClanWar_List[p_List_N].m_Play_Type = 5;
	g_ClanWar_List[p_List_N].m_Play_Count = DF_CWAR_END_TIME;
	//	�������� �� ���� ����.
	GF_ClanWar_Join_OK_Send(p_List_N,7,0);
	//	PK ��� ���� ��Ų��.
	g_pZoneLIST->Set_PK_FLAG((short)g_ClanWar_List[p_List_N].m_ZoneNO,false);
	//	���� ��� ����.
	switch(Temp_M)
	{
	case 0:
		//	��� ���͸� ���� �Ѵ�,
		GF_ClanWar_Mob_Del2((LPVOID)&g_ClanWar_List[p_List_N],0);
		break;
	case 1:
		//	A�� �¸� B���� ���� ���� �Ѵ�.
		GF_ClanWar_Mob_Del2((LPVOID)&g_ClanWar_List[p_List_N],2);
		//	A�� ���� ä���� ȸ�� �Ѵ�.
		GF_ClanWar_MOB_HP_ReSet((LPVOID)&g_ClanWar_List[p_List_N],1);
		break;
	case 2:
		//	B�� �¸� A���� ���� ���� �Ѵ�.
		GF_ClanWar_Mob_Del2((LPVOID)&g_ClanWar_List[p_List_N],1);
		//	B�� ���� ä���� ȸ�� �Ѵ�.
		GF_ClanWar_MOB_HP_ReSet((LPVOID)&g_ClanWar_List[p_List_N],2);
		break;
	}
	return true;
}

////	Ŭ���� ������� �� ã��
DWORD	GF_Find_CWar_List(LPVOID p_Npc,DWORD p_Type)
{
	if(p_Npc == NULL) return 0;
	//	p_Type = 0, 1, 2 �� ���� ���ù� ó��.
	CObjNPC	*Temp_Var;
	Temp_Var = (CObjNPC *)p_Npc;
	DWORD	t_A = 0;
	//	������� �濡�� �� �� �ִ� ���� ã�´�.
	for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
	{
		//	NPC �� ���� ���� ã��[NPC ����� ���]

		//	�����Ѵ�. ���� ���� ���� �ݾ��� �ʿ��ϴ�.
		//	���� ���� �ݾ����� ���� ������� �ִ��� Ȯ���Ѵ�.
		//	���� ���� �ݾ����� ������� ���� ���ٸ� ����� ã�� �ش�.
		if(g_ClanWar_List[i].m_CW_Npc != NULL)
		{
			//	��û Ŭ���� �ִ� ���
			if(g_ClanWar_List[i].m_Play_Type == 1)
			{
				//	��û �ݾ��� ���� ���
				if(g_ClanWar_List[i].m_War_Type == p_Type)
				{
					//	�ѹ��� �˻� : A�� Ŭ���� Ŭ���� ���� Ȯ�� �ʿ�
					//	A Ŭ���� ����Ʈ ��ġ�� ã�´�.
					t_A = GF_Clan_Fine_List(g_ClanWar_List[i].m_Clan_A);
					if(t_A > 0)
					{
						t_A = t_A - 1;
						// A �� ������ ã��
						if(0 < GF_Clan_Master_Fine(t_A))
						{
							//	B ���� ��� ��Ŵ
							return i+1;
						}
					}
				}
			}
		}
	}
	//	����� ã�´�.
	return 0;
}

////	Ŭ���� ������� �� ã�� : �ش� �濡 �̱� �� ����.
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

////	Ŭ���� ��� ã��
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

////	Ŭ������ �����
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
	//	�켱 NPC ���� �ʱ�ȭ... ���߿� �ٲ���.
	if(g_ClanWar_List[p_Num].m_CW_Npc != NULL)
	{
		//	NPC ���� �ʱ�ȭ	, ���°��� �����Ѵ�.
		g_ClanWar_List[p_Num].m_CW_Npc->Set_ObjVAR(0,0);
		g_ClanWar_List[p_Num].m_CW_Npc->Set_ObjVAR(1,0);
		g_ClanWar_List[p_Num].m_CW_Npc->Set_ObjVAR(2,0);
		g_ClanWar_List[p_Num].m_CW_Npc->Set_ObjVAR(3,0);
	}
	ZeroMemory(&g_ClanWar_List[p_Num],sizeof(GF_ClanWar_Set));	
	//	Ŭ���� �� ��ġ �ʱ�ȭ
	g_ClanWar_List[p_Num].m_ZoneNO = (WORD)(101 + p_Num);
	g_ClanWar_List[p_Num].m_APosWARP.x = (float)CW_Warp_AX;
	g_ClanWar_List[p_Num].m_APosWARP.y = (float)CW_Warp_AY;
	g_ClanWar_List[p_Num].m_BPosWARP.x = (float)CW_Warp_BX;
	g_ClanWar_List[p_Num].m_BPosWARP.y = (float)CW_Warp_BY;
	//	����
	GF_WLV_Add(p_Num);
	return true;
}

////	Ŭ������ ��û ��� ó��
BOOL	GF_Clan_War_Cancel(DWORD p_Num)
{
	//	Ŭ�� ��ȣ Ȯ��.
	if(g_Clan_List[p_Num].m_CLAN.m_dwClanID == 0) return false;

	//	������ �濡 Ŭ�� ��ȣ Ȯ��
	for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
	{
		//	��û �߽��� Ȯ��
		if(g_ClanWar_List[i].m_CW_Npc != NULL)
		{
			if(g_ClanWar_List[i].m_Play_Type == 1)
			{
				//	��û ����� : �ܼ� ����
				if(g_ClanWar_List[i].m_Clan_A == g_Clan_List[p_Num].m_CLAN.m_dwClanID)
				{
					// �ش�� �ʱ�ȭ.
					GF_Del_CWar_List2(i);
					return true;
				}
			}
			else if(g_ClanWar_List[i].m_Play_Type > 1)
			{
				////	���� ������ : ���� ��� ó��
				if(g_ClanWar_List[i].m_Clan_A == g_Clan_List[p_Num].m_CLAN.m_dwClanID)
				{
					//	�巴 ó��
					if(GF_Clan_War_Drop(i+1,1000))
					{
						//	�ش� ���� ����Ʈ ó��
						GF_ClanWar_Quest_DropProc(i+1,1000);
						//	����� Ŭ���忡�� ��ǽ� �޽��� ����
						GF_ClanWar_GiveUP(i+1,1000);
					}
					return true;
				}
				else if(g_ClanWar_List[i].m_Clan_B == g_Clan_List[p_Num].m_CLAN.m_dwClanID)
				{
					//	�巴 ó��
					if(GF_Clan_War_Drop(i+1,2000))
					{
						//	�ش� ���� ����Ʈ ó��
						GF_ClanWar_Quest_DropProc(i+1,2000);
						//	����� Ŭ���忡�� ��ǽ� �޽��� ����
						GF_ClanWar_GiveUP(i+1,2000);
					}
					return true;
				}
			}
		}
	}
	return false;
}

////	Ŭ���� ��û ó��
BOOL	GF_Clan_War_Input(LPVOID p_Npc,DWORD p_Clan_Inx,DWORD p_LV, DWORD p_Type, LPVOID pClanUser)
{
	if(p_Npc == NULL) return false;
	if( pClanUser == NULL ) return false;
	if(p_Clan_Inx == 0) return false;
	if(p_Type == 0) return false;

	//	����
	EnterCriticalSection(&g_ClanWar_LOCK);

	CObjNPC	*Temp_Var;
	classUSER* pUser = NULL;
	Temp_Var = (CObjNPC *)p_Npc;
	pUser = (classUSER*)pClanUser;		// Ŭ���� ��û��

	//	���� LV Ȯ��.
	if(!GF_Clan_War_Check_LV(p_LV,GF_Get_CWar_Type(p_Type)))
		return false;

	//	NPC �ּҸ� ������� ����� ã�´�.
	//	NPC ��ϵ� �濡�� ���ڸ� ã�� �κ� ���� ��Ŵ
	DWORD	Temp_N = GF_Find_CWar_List(p_Npc,GF_Get_CWar_Type(p_Type));
	__int64	Temp_MM = 0;
	if(Temp_N == 0)
	{
		//	����� ã�ƾ���
		Temp_N = GF_Find_CWar_Void(p_Npc);
		if(Temp_N == 0)
		{
			// Ŭ���� ��û�� �� ����
			pUser->Send_gsv_CLANWAR_Err(CWAR_TOO_MANY_CWAR_PROGRESS);
			LeaveCriticalSection(&g_ClanWar_LOCK);
			return false;
		}	// ��� ����

		//	���� �ݾ� Ȯ��.
		Temp_MM = GF_Set_CWar_Bat(p_Clan_Inx,p_Type);
		if(Temp_MM == 0)
		{
			LeaveCriticalSection(&g_ClanWar_LOCK);
			return false;
		}
		//	�迭 ������ ����.
		Temp_N = Temp_N - 1;
		g_ClanWar_List[Temp_N].m_CW_Npc = Temp_Var;
		g_ClanWar_List[Temp_N].m_Play_Type = 1;
		g_ClanWar_List[Temp_N].m_Play_Count = 0;
		g_ClanWar_List[Temp_N].m_War_Type = GF_Get_CWar_Type(p_Type);
		//	���� �ݾ� �ֱ�
		g_ClanWar_List[Temp_N].m_Bat_Coin = Temp_MM;
		g_ClanWar_List[Temp_N].m_Win_Team = 0;
		//	����� ��� A���� p_Clan_Inx ���� p_Type ���� �ִ´�.
		g_ClanWar_List[Temp_N].m_Clan_A = p_Clan_Inx;	// Ŭ�� ��ȣ.
		g_ClanWar_List[Temp_N].m_Clan_B = 0;
		g_ClanWar_List[Temp_N].m_Type_A = p_Type;
		g_ClanWar_List[Temp_N].m_Type_B = 0;

		//	A�� ��û DB ����.
		GF_DB_CWar_Insert(1,0,Temp_MM,p_Clan_Inx,0,NULL);
	}
	else
	{
		//	�迭 ������ ���� : ����������� ����
		Temp_N = Temp_N-1;
		//	����� �ƴ� ��� B���� ������ Ȯ���Ѵ�.
		if(g_ClanWar_List[Temp_N].m_Play_Type == 1)
		{
			//	���� �ݾ� Ȯ��.
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
			//	���� �ݾ� �ֱ�
			g_ClanWar_List[Temp_N].m_Bat_Coin = g_ClanWar_List[Temp_N].m_Bat_Coin + Temp_MM;
			//	B���� p_Clan_Inx ���� p_Type ���� �ִ´�..
			g_ClanWar_List[Temp_N].m_Clan_B = p_Clan_Inx;
			g_ClanWar_List[Temp_N].m_Type_B = p_Type;
			//	���� ������ p_Type = 1:2 , 31:32, 61:62  ���ؾ��Ѵ�.
			
			//	B�� ��û DB ����.
			GF_DB_CWar_Insert(2,0,Temp_MM,0,p_Clan_Inx,NULL);

			//	�ش� Ŭ���忡�� ���� �޽��� ����.
			GF_ClanWar_Join_OK_Send(Temp_N,1,0);

			//	���� ���۵� ������ ����.
			//	Ŭ���� ���� DB ����.
			GF_DB_CWar_Insert(3,0,g_ClanWar_List[Temp_N].m_Bat_Coin,g_ClanWar_List[Temp_N].m_Clan_A,g_ClanWar_List[Temp_N].m_Clan_B,NULL);
		}
	}
	//	����
	GF_WLV_Add(Temp_N);

	LeaveCriticalSection(&g_ClanWar_LOCK);
	return true;
}


////	Ŭ���� ���� LV Ȯ��.
BOOL	GF_Clan_War_Check_LV(DWORD p_LV,DWORD p_Type)
{
	if(p_Type > 2) return false;
	if(p_LV <= G_CW_MAX_LEVEL[p_Type])	return true;
	return false;
}

////	Ŭ�� ����Ʈ���� ã��
DWORD	GF_Clan_Fine_List(DWORD p_Clan_ID)
{
	//	�޸� Ȯ��.
	if(g_Clan_List == NULL) return 0;
	//	�ش� Ŭ�� Ȯ��.
	for(DWORD i = 0 ; i < g_Clan_Max_List; i++)
	{
		//	Ŭ�� ��ȣ Ȯ��
		if(g_Clan_List[i].m_CLAN.m_dwClanID == p_Clan_ID)
			return i+1;
	}
	return 0;
}

////	Ŭ���� �����͸� ã�´�.
DWORD	GF_Clan_Master_Fine(DWORD p_Clan)
{
	DWORD	Temp_ID = 0;
	// for(DWORD i = 0; i < 15;i++)
	// (20070125:����ö) : Ŭ���� ��ġ�� 15�� �ϵ��ڵ��� CW_USER_NO�� ��ġ�� ���� (�ִ� 50����� ����)
	for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0];i++ )
	{
		//	���� ���� Ȯ��.
		Temp_ID = g_Clan_List[p_Clan].m_User_IDX[i];
		if(0 < Temp_ID)
		{
			//	Ŭ�� �������� ��� Ȯ��
			if(g_User_List[Temp_ID - 1]->Is_ClanMASTER())
				return Temp_ID;
		}
	}
	return 0;
}

////	Ŭ�� ������ ���� ������ ã�´�.
DWORD	GF_Clan_Master_Sub_Fine(DWORD p_Clan)
{
	DWORD	Temp_ID = 0;
	if(g_Clan_List[Temp_ID - 1].m_Sub_Master > 0)
	{
		Temp_ID = g_Clan_List[Temp_ID - 1].m_Sub_Master;
		g_Clan_List[Temp_ID - 1].m_Sub_Master = 0;
		return Temp_ID;
	}
	//	��� ������� ã��
	for(DWORD p = 5; p > 0; p--)
	{
		// for(DWORD i = 0; i < 15;i++)
		// (20070125:����ö) : Ŭ���� ��ġ�� 15�� �ϵ��ڵ��� CW_USER_NO�� ��ġ�� ���� (�ִ� 50����� ����)
		for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0];i++ )
		{
			//	���� ���� Ȯ��.
			Temp_ID = g_Clan_List[p_Clan].m_User_IDX[i];
			if(0 < Temp_ID)
			{
				//	Ŭ�� �������� ��� Ȯ��
				if(g_User_List[Temp_ID - 1]->GetClanPOS() == p)
				{
					g_Clan_List[Temp_ID - 1].m_Sub_Master = Temp_ID;
					return Temp_ID;
				}
			}
		}
	}
	//	���� ��� ã�� ���� ��� : ù��° ���� ����
	// for(DWORD i = 0; i < 15;i++)
	// (20070125:����ö) : Ŭ���� ��ġ�� 15�� �ϵ��ڵ��� CW_USER_NO�� ��ġ�� ���� (�ִ� 50����� ����)
	for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0]; i++ )
	{
		//	���� ���� Ȯ��.
		Temp_ID = g_Clan_List[p_Clan].m_User_IDX[i];
		if(0 < Temp_ID)
		{
			g_Clan_List[Temp_ID - 1].m_Sub_Master = Temp_ID;
			return Temp_ID;
		}
	}
	return 0;
}

////	Ŭ���� ��� Ŭ�� Ȯ��
BOOL	GF_ClanWar_Check_myClan(DWORD p_List_N,DWORD p_Clan_N)
{
	//	����Ʈ ���� Ȯ��.
	if(p_List_N == 0) return false;
	p_List_N = p_List_N - 1;
	if(p_List_N >= DF_MAX_CWAR_LIST) return false;
	if(p_Clan_N == 0) return false;

	//	�ش� ����Ʈ�� Ŭ�� ��ȣ Ȯ��
	if(g_ClanWar_List[p_List_N].m_CW_Npc == NULL) return false;	// ��� �ȵ� ��.
	//	������ ����� ��� ���� �Ұ� ó��.
	switch(g_ClanWar_List[p_List_N].m_Play_Type)
	{
		//	Ŭ���� ������̴�. �� Ȯ������.
	case 2:
	case 3:
	case 4:
		break;
		//	Ŭ���� ������� �ƴϴ�. ���� �Ұ�.
	default:
		return false;
	}
	//	Ŭ�� Ȯ��.
	if(g_ClanWar_List[p_List_N].m_Clan_A == p_Clan_N) return 1;
	if(g_ClanWar_List[p_List_N].m_Clan_B == p_Clan_N) return 2;
	return false;
}

////	Ŭ�� �Ҽ� ���� �������� ���� Ȯ�� �޽��� ó��
BOOL	GF_ClanWar_Join_OK_Send(DWORD p_List_N,DWORD p_Type,DWORD p_Sub_Type)
{
	//	NPC Ȯ��.
	if(g_ClanWar_List[p_List_N].m_CW_Npc == NULL) return false;
	if(p_Type == 0) return false;
	DWORD	t_A = 0;
	DWORD	t_B = 0;
	BOOL	b_A = false;
	BOOL	b_B = false;
	//	A,B�� Ŭ�� ����Ʈ ��ȣ ã�� [�ش� Ŭ���� ���� ��� �޽��� ������ ���Ѵ�. Ȯ�� ó��...]
	t_A = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
	t_B = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);

	//	Ŭ�� �Ҽ� �������� ��� ����.
	DWORD	TempA_ID = 0;
	DWORD	TempB_ID = 0;
	switch(p_Type)
	{
	case 1:
		{
			//	p_Type�� 1~2�̸� �ݵ�� ���� �ʿ� �������� �����ϴ� ���� ó��
			if((t_A*t_B) == 0) return false;	// 0���� ������ ����.
			//	���迭�� ��ȯ
			t_A = t_A - 1;
			t_B = t_B - 1;
			//	Ŭ�� �������� ��� Ȯ�� (������ �����Ͱ� �������� �ʴ� ���� Ȯ�� �ʿ�.)
			TempA_ID = GF_Clan_Master_Fine(t_A);	// A �� ������ ã��
			TempB_ID = GF_Clan_Master_Fine(t_B);	// B �� ������ ã��
			if((TempA_ID > 0)&&(TempB_ID > 0))
			{
				//	�� ���� �����Ͱ� ����.
				//	������ ���� ���� ���� �뺸.
				//	LV ���� Ȯ���� ������ �Ѵ�.
				g_User_List[TempA_ID - 1]->Send_gsv_CLANWAR_OK(1,g_ClanWar_List[p_List_N].m_Clan_A,(WORD)(p_List_N + 1),1000);
				g_User_List[TempB_ID - 1]->Send_gsv_CLANWAR_OK(1,g_ClanWar_List[p_List_N].m_Clan_B,(WORD)(p_List_N + 1),2000);
				
				//	����ȯ
				GF_ClanWar_Mob_Set(p_List_N + 1);
			}
			//	��Ī �Ǿ��µ� ���� �� �����Ͱ� ���� ���.... ��Ī �ߴ�.
			else if((TempA_ID == 0)&&(TempB_ID == 0))
			{
				//	�� ���� ������ ����. ���
				GF_Del_CWar_List2(p_List_N);
			}
			else if(TempA_ID == 0)
			{
				//	B�� �¸� ó�� [���� ���� �¸� ó��]
				GF_Win_CWar_Set((LPVOID)&g_ClanWar_List[p_List_N],2,false);
				//	B���� ����Ʈ �¸� ó��
				GF_ClanWar_Quest_DropProc(p_List_N+1,2000);
				////	��� ���� �־��ش�.
				//g_User_List[TempB_ID - 1]->m_Inventory.m_i64Money = 
				//	g_User_List[TempB_ID - 1]->m_Inventory.m_i64Money + (g_ClanWar_List[p_List_N].m_Bat_Coin/200)*110;
				//g_User_List[TempB_ID - 1]->Send_gsv_SET_MONEYONLY(GSV_SET_MONEY_ONLY);
				//	������ �ʱ�ȭ
				GF_Del_CWar_List2(p_List_N);
			}
			else
			{
				//	A�� �¸� ó�� [���� ���� �¸� ó��]
				GF_Win_CWar_Set((LPVOID)&g_ClanWar_List[p_List_N],1,false);
				//	A���� ����Ʈ �¸� ó��
				GF_ClanWar_Quest_DropProc(p_List_N+1,1000);
				//	������ �ʱ�ȭ
				GF_Del_CWar_List2(p_List_N);
			}
			break;
		}
	case 2:
		{
			//	p_Type�� 1~2�̸� �ݵ�� ���� �ʿ� �������� �����ϴ� ���� ó��
			if((t_A*t_B) == 0) return false;	// 0���� ������ ����.
			//	���迭�� ��ȯ
			t_A = t_A - 1;
			t_B = t_B - 1;
			//	Ŭ�� �����鿡�� �뺸.
			switch(p_Sub_Type)
			{
			case 1:
				{
					//	A �� Ŭ�� ���� ����.
					// for(DWORD i = 0; i < 15;i++)
					// (20070125:����ö) : Ŭ���� ��ġ�� 15�� �ϵ��ڵ��� CW_USER_NO�� ��ġ�� ���� (�ִ� 50����� ����)
					for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0]; i++ )
					{
						//	���� ���� Ȯ��.
						TempA_ID = g_Clan_List[t_A].m_User_IDX[i];
						ZTRACE( "%s, USER_ID(%d)", g_Clan_List[t_A].m_ClanNAME, i );
						if(0 < TempA_ID)
						{
							TempA_ID = TempA_ID  - 1;
							if(!g_User_List[TempA_ID]->Is_ClanMASTER())
							{
								ZTRACE( "%s, USER_ID(%d) - Ŭ����", g_Clan_List[t_A].m_ClanNAME, i );
								//	LV ���� Ȯ��.
								if(GF_Clan_War_Check_LV(g_User_List[TempA_ID]->Get_LEVEL(),g_ClanWar_List[p_List_N].m_War_Type))
								{
									// Ŭ�� �����Ͱ� �ƴ� ��� ���� �޽��� ����
									g_User_List[TempA_ID]->Send_gsv_CLANWAR_OK(p_Type,g_ClanWar_List[p_List_N].m_Clan_A,(WORD)(p_List_N + 1),1000);
								}
								else
								{
									// LV ���ѿ� ���� ���� �Ұ� �޽��� ����.
									g_User_List[TempA_ID]->Send_gsv_CLANWAR_Err(CWAR_CLAN_MASTER_LEVEL_RESTRICTED);
								}
							}
							else
							{
								ZTRACE( "%s, USER_ID(%d) - Ŭ��������", g_Clan_List[t_A].m_ClanNAME, i );
							}
						}
						else
						{
							ZTRACE( "%s, USER_ID(%d) - ���� ����", g_Clan_List[t_A].m_ClanNAME, i );
						}
					}
					//	�ɼ� ó��
					if(g_ClanWar_List[p_List_N].m_Play_Type == 2)
						g_ClanWar_List[p_List_N].m_Play_Type = 3;
					else if(g_ClanWar_List[p_List_N].m_Play_Type == 3)
						g_ClanWar_List[p_List_N].m_Play_Type = 4;
					break;
				}
			case 2:
				{
					//	B �� Ŭ�� ���� ����.
					// for(DWORD i = 0; i < 15;i++)
					// (20070125:����ö) : Ŭ���� ��ġ�� 15�� �ϵ��ڵ��� CW_USER_NO�� ��ġ�� ���� (�ִ� 50����� ����)
					for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0]; i++ )
					{
						//	���� ���� Ȯ��.
						TempB_ID = g_Clan_List[t_B].m_User_IDX[i];
						ZTRACE( "%s, USER_ID(%d)", g_Clan_List[t_B].m_ClanNAME, i );
						if(0 < TempB_ID)
						{
							TempB_ID = TempB_ID  - 1;
							if(!g_User_List[TempB_ID]->Is_ClanMASTER())
							{
								ZTRACE( "%s, USER_ID(%d) - Ŭ����", g_Clan_List[t_B].m_ClanNAME, i );
								//	LV ���� Ȯ��.
								if(GF_Clan_War_Check_LV(g_User_List[TempB_ID]->Get_LEVEL(),g_ClanWar_List[p_List_N].m_War_Type))
								{
									//	Ŭ�� �����Ͱ� �ƴ� ��� ���� �޽��� ����
									g_User_List[TempB_ID]->Send_gsv_CLANWAR_OK(p_Type,g_ClanWar_List[p_List_N].m_Clan_B,(WORD)(p_List_N + 1),2000);
								}
								else
								{
									//	LV ���ѿ� ���� ���� �Ұ� �޽��� ����.
									g_User_List[TempB_ID]->Send_gsv_CLANWAR_Err(CWAR_CLAN_MASTER_LEVEL_RESTRICTED);
								}
							}
							else
							{
								ZTRACE( "%s, USER_ID(%d) - Ŭ��������", g_Clan_List[t_B].m_ClanNAME, i );
							}
						}
						else
						{
							ZTRACE( "%s, USER_ID(%d) - ���� ����", g_Clan_List[t_B].m_ClanNAME, i );
						}
					}
					//	�ɼ� ó��
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
			//	�����ϴ� ���� ó��
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
				//	���迭�� ��ȯ
				t_B = t_B - 1;
			}
			//	Ÿ�� ī��Ʈ �޽��� ����.
			//	A,B �� Ŭ�� ���� ����.
			// for(DWORD i = 0; i < 15;i++)
			// (20070125:����ö) : Ŭ���� ��ġ�� 15�� �ϵ��ڵ��� CW_USER_NO�� ��ġ�� ���� (�ִ� 50����� ����)
			for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0]; i++ )
			{
				//	A���� �������ϴ� ��츸 ó��
				if(b_A)
				{
					//	���� ���� Ȯ��.
					TempA_ID = g_Clan_List[t_A].m_User_IDX[i];
					if(0 < TempA_ID)
					{
						TempA_ID = TempA_ID  - 1;
						if(g_User_List[TempA_ID]->GetZONE() != NULL)
						{
							if(g_ClanWar_List[p_List_N].m_ZoneNO == g_User_List[TempA_ID]->m_nZoneNO)
							{
								//	Ÿ�� �޽��� ����
								g_User_List[TempA_ID]->Send_gsv_CLANWAR_Time(
									g_ClanWar_List[p_List_N].m_Play_Type,
									g_ClanWar_List[p_List_N].m_Play_Count);
							}
						}
					}
				}
				//	���� ���� Ȯ��.
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
								//	Ÿ�� �޽��� ����
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
			//	�����ϴ� ���� ó��
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
				//	���迭�� ��ȯ
				t_B = t_B - 1;
			}
			//	Ŭ������ ������ ���� ����
			//	A,B �� Ŭ�� ���� ����.
			// for(DWORD i = 0; i < 15;i++)
			// (20070125:����ö) : Ŭ���� ��ġ�� 15�� �ϵ��ڵ��� CW_USER_NO�� ��ġ�� ���� (�ִ� 50����� ����)
			for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0]; i++ )
			{
				//	���� ���� Ȯ��.
				if(b_A)
				{
					TempA_ID = g_Clan_List[t_A].m_User_IDX[i];
					if(0 < TempA_ID)
					{
						TempA_ID = TempA_ID  - 1;
						if(g_ClanWar_List[p_List_N].m_ZoneNO == g_User_List[TempA_ID]->m_nZoneNO)
						{
							//	�ֳ����� ������.
							GF_ClanWar_Out_Move((LPVOID)g_User_List[TempA_ID]);
						}
					}
				}
				//	���� ���� Ȯ��
				if(b_B)
				{
					TempB_ID = g_Clan_List[t_B].m_User_IDX[i];
					if(0 < TempB_ID)
					{
						TempB_ID = TempB_ID  - 1;
						if(g_ClanWar_List[p_List_N].m_ZoneNO == g_User_List[TempB_ID]->m_nZoneNO)
						{
							//	�ֳ����� ������.
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

////	Ŭ���� �������� ���� ���� ��Ŵ
BOOL	GF_ClanWar_Move(LPVOID p_User,DWORD p_List_N,DWORD p_Team_N)
{
	if(p_User == NULL) return false;
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	if(p_Team_N == 0) return false;
	classUSER	*Temp_User;
	Temp_User = (classUSER *)p_User;

	// ������ LV Ȯ�� ó��.
	if(!GF_Clan_War_Check_LV(Temp_User->Get_LEVEL(),g_ClanWar_List[p_List_N - 1].m_War_Type))
	{
		// ���� ��Ű�� ������.
		Temp_User->Send_gsv_CLANWAR_Err(CWAR_CLAN_MASTER_LEVEL_RESTRICTED);
		return true;
	}

	//	��ġ�� ��Ż ��Ŵ.
	if(p_Team_N == 1)
	{
		//	���� �ο� ���� Ȯ��.
		if(!GF_Add_ClanWar_Number(p_List_N,1000))
		{
			//	�ο� ���� ó�� �޽��� ����.
			Temp_User->Send_gsv_CLANWAR_Err(CWAR_TOO_MANY_CWAR_PROGRESS);
			return 2;
		}

		//	ä���� 0�� ��� �����Ѵ�.
		if(Temp_User->Get_HP() <= 0)
		{
			Temp_User->Set_HP(Temp_User->Get_MaxHP());
			Temp_User->Send_CHAR_HPMP_INFO(0);		// HP, MP������ �ѹ� �� �����ش�.
		}

		//	��Ƽ ����
		Temp_User->Party_Out();
		//	Ŭ���� �̵�
		Temp_User->Reward_WARP(g_ClanWar_List[p_List_N - 1].m_ZoneNO,g_ClanWar_List[p_List_N - 1].m_APosWARP);
		//	�ش� ���� �� ��ȣ ����.
		Temp_User->SetCur_TeamNO(1000);
		Temp_User->Set_TeamNoFromClanIDX(1000);	// �Ϲ� ����.w
	}
	else if(p_Team_N == 2)
	{
		//	���� �ο� ���� Ȯ��.
		if(!GF_Add_ClanWar_Number(p_List_N,2000))
		{
			//	�ο� ���� ó�� �޽��� ����.
			Temp_User->Send_gsv_CLANWAR_Err(CWAR_TOO_MANY_CWAR_PROGRESS);
			return 2;
		}

		//	ä���� 0�� ��� �����Ѵ�.
		if(Temp_User->Get_HP() <= 0)
		{
			Temp_User->Set_HP(Temp_User->Get_MaxHP());
			Temp_User->Send_CHAR_HPMP_INFO(0);		// HP, MP������ �ѹ� �� �����ش�.
		}

		//	��Ƽ ����
		Temp_User->Party_Out();
		//	Ŭ���� �̵�
		Temp_User->Reward_WARP(g_ClanWar_List[p_List_N - 1].m_ZoneNO,g_ClanWar_List[p_List_N - 1].m_BPosWARP);
		//	�ش� ���� �� ��ȣ ����.
		Temp_User->SetCur_TeamNO(2000);
		Temp_User->Set_TeamNoFromClanIDX(2000);	// �Ϲ� ����.
	}
	else return false;
	return true;
}

////	Ŭ���� ���� ��ġ�� ����
BOOL	GF_ClanWar_Out_Move(LPVOID p_User)
{
	if(p_User == NULL) return false;
	classUSER	*Temp_User;
	Temp_User = (classUSER *)p_User;

	//	��Ƽ ����
	Temp_User->Party_Out();

	//	ä���� 0�� ��� �����Ѵ�.
	if(Temp_User->Get_HP() <= 0)
	{
		Temp_User->Set_HP(Temp_User->Get_MaxHP());
		Temp_User->Send_CHAR_HPMP_INFO(0);		// HP, MP������ �ѹ� �� �����ش�.
	}

	// �ӽ� ��ġ�� ��
	tPOINTF	Temp_Pos;
	Temp_Pos.x = g_ClanWar_ReCall.m_Pos.x + (rand()%50) - 25;
	Temp_Pos.y = g_ClanWar_ReCall.m_Pos.y + (rand()%50) - 25;
	//	�ش� ���� ���� ���� ��ġ�� ����.
	Temp_User->Reward_WARP(g_ClanWar_ReCall.m_ZoneNO,Temp_Pos);
	//	�ش� ���� �� ��ȣ ����.
	Temp_User->SetCur_TeamNO(2);
	Temp_User->Set_TeamNoFromClanIDX(0);	// �Ϲ� ����.
	return true;
}

////	Ŭ���� ������ �� ���� �ϱ�
BOOL	GF_ClanWar_Mob_Set(DWORD p_List_N)
{
	//	����Ʈ ���� Ȯ��.
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	p_List_N = p_List_N - 1;
	//	�ش� ����Ʈ�� �� �����
	GF_ClanWar_Mob_Del(p_List_N + 1,0);

	//	���� Ÿ�� ���
	DWORD Temp_1 = g_ClanWar_List[p_List_N].m_War_Type;
	if(Temp_1 > 2) Temp_1 = 0;	// ���� ���� ���� ��� 0 : �̷� ��� ����

	for(DWORD i = 0; i < DF_MAX_CWAR_MOB; i++)
	{
		// (20070123:����ö) : ������ ���� ������ ���� ����
		if( g_CW_MOB_List[Temp_1][i][0].m_MOB_Idx > 0 )
		{
			//	�ش� ������ �� ���� �ϱ�.
			g_ClanWar_List[p_List_N].m_ATeam_Mob[i] = 
				g_pZoneLIST->RegenCharacter(g_ClanWar_List[p_List_N].m_ZoneNO,
				g_CW_MOB_List[Temp_1][i][0].m_Pos_X, g_CW_MOB_List[Temp_1][i][0].m_Pos_Y,
				g_CW_MOB_List[Temp_1][i][0].m_Range, g_CW_MOB_List[Temp_1][i][0].m_MOB_Idx,
				g_CW_MOB_List[Temp_1][i][0].m_Number, DF_CWAR_A_TEAM);
		}

		// (20070123:����ö) : ������ ���� ������ ���� ����
		if( g_CW_MOB_List[Temp_1][i][1].m_MOB_Idx > 0 )
		{
			g_ClanWar_List[p_List_N].m_BTeam_Mob[i] = 
				g_pZoneLIST->RegenCharacter(g_ClanWar_List[p_List_N].m_ZoneNO,
				g_CW_MOB_List[Temp_1][i][1].m_Pos_X, g_CW_MOB_List[Temp_1][i][1].m_Pos_Y, 
				g_CW_MOB_List[Temp_1][i][1].m_Range, g_CW_MOB_List[Temp_1][i][1].m_MOB_Idx,
				g_CW_MOB_List[Temp_1][i][1].m_Number, DF_CWAR_B_TEAM);
		}
	}

	//	PK ��� ����
	g_pZoneLIST->Set_PK_FLAG((short)g_ClanWar_List[p_List_N].m_ZoneNO,true);
	return true;
}

////	Ŭ���� ������ �� �����
BOOL	GF_ClanWar_Mob_Del(DWORD p_List_N,DWORD p_Type)
{
	//	����Ʈ ���� Ȯ��.
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	p_List_N = p_List_N - 1;

	//	���� ��� ���� ���δ�.
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
				// ���� ��� �ʱ�ȭ
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
				// A�� ��� �ʱ�ȭ
				if(g_ClanWar_List[p_List_N].m_ATeam_Mob[i] != NULL)
				{
					g_ClanWar_List[p_List_N].m_ATeam_Mob[i] = NULL;
				}
				break;
			}
		case 2:
			{
				// B�� ��� �ʱ�ȭ
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
	//	����Ʈ ���� Ȯ��.
	if(p_Room == NULL) return false;
	GF_ClanWar_Set *p_CWar_Room;
	p_CWar_Room = (GF_ClanWar_Set *)p_Room;

	//	���� ��� ���� ���δ�.
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
				// ���� ��� �ʱ�ȭ
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
				// A�� ��� �ʱ�ȭ
				if(p_CWar_Room->m_ATeam_Mob[i] != NULL)
				{
					p_CWar_Room->m_ATeam_Mob[i] = NULL;
				}
				break;
			}
		case 2:
			{
				// B�� ��� �ʱ�ȭ
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

////	�ش� ���� ���� ü���� ȸ���Ѵ�.
BOOL	GF_ClanWar_MOB_HP_ReSet(LPVOID p_Room,DWORD p_Type)
{
	//	����Ʈ ���� Ȯ��.
	if(p_Room == NULL) return false;
	GF_ClanWar_Set *p_CWar_Room;
	p_CWar_Room = (GF_ClanWar_Set *)p_Room;

	//	�� ��ȣ Ȯ��.
	if(p_Type == 1)
		g_pZoneLIST->Zone_MOB_HP_Reset(p_CWar_Room->m_ZoneNO,1000);
	else if(p_Type == 2)
		g_pZoneLIST->Zone_MOB_HP_Reset(p_CWar_Room->m_ZoneNO,2000);
	return true;
}

////	Ŭ���� ������ ���� �޽��� ó��
BOOL	GF_ClanWar_Boss_MOB_Kill(DWORD List_N,DWORD p_idx)
{
	////	Ŭ���� ���� ����Ʈ�� ������ ��� : ó�� �����ÿ��� false ��ȯ. []
	//	���� ���� Ȯ��
	//if(p_User == NULL) return true;
	//classUSER	*Temp_User;
	//Temp_User = (classUSER *)p_User;
	if(List_N == 0) return false;	// ���ȣ.

	DWORD	Temp_ID = List_N;
	//	����Ʈ �ε��� Ȯ��.
	switch(p_idx)
	{
	case 2814:
		{
			//	50�� ���� ��
			//	2811�� ����Ʈ ����

			////	2) �ش� ������ Ŭ���� �� ��ȣ ���.
			//Temp_ID = GF_Find_CWar_List2(Temp_User->GetClanID());
			if(Temp_ID == 0) return false;	// �� ��ȣ ����.. ���� ó�� �ȵ�. �̹� ó����.;
			//	3) �ش� �� ���� �� ó��.
			GF_Clan_War_Drop(Temp_ID,0);	// �ڵ� ����.

			//	�ݴ��� ��. ����Ʈ ����
			DWORD	t_Cl = 0;
			DWORD	t_CM = 0;
			if(g_ClanWar_List[Temp_ID - 1].m_Win_Team == 1)
			{
				//	B�� Ŭ���� ã��
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_B);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;		
					t_CM = GF_Clan_Master_Fine(t_Cl);	//B �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
					}
					else
					{
						//	������ Ŭ������ ����.. ��� ó���ұ�?
						//	�� ���� ó�� ���Ѵ�.
					}
				}

				//	A�� �¸� ����Ʈ ó��
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_A);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	//A �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
						//	����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
					}
					else
					{
						// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_Cl);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
						}
					}
				}
				//	A�� �¸� �޽��� ����
				GF_Send_CLANWAR_Progress(Temp_ID,2);
			}
			else if(g_ClanWar_List[Temp_ID - 1].m_Win_Team == 2)
			{
				//	A�� Ŭ���� ã��
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_A);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;
					t_CM = GF_Clan_Master_Fine(t_Cl);	//A�� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
					}
					else
					{
						// ������ Ŭ������ ����.. ��� ó���ұ�?
					}
				}
				//	B�� �¸� ����Ʈ ó��
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_B);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	// B�� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
						//	����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
					}
					else
					{
						// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_Cl);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
						}
					}
				}
				//	B�� �¸� �޽��� ����
				GF_Send_CLANWAR_Progress(Temp_ID,3);
			}
			return false;
			break;
		}
	case 2815:
		{
			//	100�� ���� ��
			//	2812�� ����Ʈ ����
			
			////	1) �ش� ������ Ŭ�������� Ȯ�� �Ѵ�.
			//if(!Temp_User->Is_ClanMASTER())
			//	return false;	// ���� Ʈ���� ó�� �ߴ�. �ش� �濡 �̱� �� ó�� [�̹� ó�� �Ǿ� �ִ��� ���� ó��.]
			
			//	2) �ش� ������ Ŭ���� �� ��ȣ ���.
			//Temp_ID = GF_Find_CWar_List2(Temp_User->GetClanID());
			if(Temp_ID == 0) return false;	// �� ��ȣ ����.. ���� ó�� �ȵ�. �̹� ó����.;
			//	3) �ش� �� ���� �� ó��.
			GF_Clan_War_Drop(Temp_ID,0);	// �ڵ� ����.
			
			////	�����ϰ� �ִ� ����Ʈ ���� �ʿ�.
			////	����Ʈ �����.
			//GF_ClanWar_Quest_Del((LPVOID)Temp_User,2812);

			//	�ݴ��� ��. ����Ʈ ����
			DWORD	t_Cl = 0;
			DWORD	t_CM = 0;
			if(g_ClanWar_List[Temp_ID - 1].m_Win_Team == 1)
			{
				//	B�� Ŭ���� ã��
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_B);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	//B �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
					}
				}
				//	A�� �¸� ����Ʈ ó��
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_A);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	//A �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
						//	����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
					}
					else
					{
						// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_Cl);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
						}
					}
				}
				//	A�� �¸� �޽��� ����
				GF_Send_CLANWAR_Progress(Temp_ID,2);
			}
			else if(g_ClanWar_List[Temp_ID - 1].m_Win_Team == 2)
			{
				//	A�� Ŭ���� ã��
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_A);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	//A�� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
					}
				}
				//	B�� �¸� ����Ʈ ó��
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_B);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	// B�� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
						//	����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
					}
					else
					{
						// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_Cl);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
						}
					}
				}
				//	B�� �¸� �޽��� ����
				GF_Send_CLANWAR_Progress(Temp_ID,3);
			}
			return false;
			break;
		}
	case 2816:
		{
			//	1000�� ���� ��
			//	2813�� ����Ʈ ����
			
			////	1) �ش� ������ Ŭ�������� Ȯ�� �Ѵ�.
			//if(!Temp_User->Is_ClanMASTER())
			//	return false;	// ���� Ʈ���� ó�� �ߴ�. �ش� �濡 �̱� �� ó�� [�̹� ó�� �Ǿ� �ִ��� ���� ó��.]
			
			//	2) �ش� ������ Ŭ���� �� ��ȣ ���.
			//Temp_ID = GF_Find_CWar_List2(Temp_User->GetClanID());
			if(Temp_ID == 0) return false;	// �� ��ȣ ����.. ���� ó�� �ȵ�. �̹� ó����.;
			//	3) �ش� �� ���� �� ó��.
			GF_Clan_War_Drop(Temp_ID,0);	// �ڵ� ����.
			
			////	�����ϰ� �ִ� ����Ʈ ���� �ʿ�.
			////	����Ʈ �����.
			//GF_ClanWar_Quest_Del((LPVOID)Temp_User,2813);

			//	�ݴ��� ��. ����Ʈ ����
			DWORD	t_Cl = 0;
			DWORD	t_CM = 0;
			if(g_ClanWar_List[Temp_ID - 1].m_Win_Team == 1)
			{
				//	B�� Ŭ���� ã��
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_B);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	//B �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
					}
				}
				//	A�� �¸� ����Ʈ ó��
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_A);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	//A �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
						//	����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
					}
					else
					{
						// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_Cl);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
						}
					}
				}
				//	A�� �¸� �޽��� ����
				GF_Send_CLANWAR_Progress(Temp_ID,2);
			}
			else if(g_ClanWar_List[Temp_ID - 1].m_Win_Team == 2)
			{
				//	A�� Ŭ���� ã��
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_A);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;
					t_CM = GF_Clan_Master_Fine(t_Cl);	// A�� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
					}
				}
				//	B�� �¸� ����Ʈ ó��
				t_Cl = GF_Clan_Fine_List(g_ClanWar_List[Temp_ID - 1].m_Clan_B);
				if(t_Cl > 0)
				{
					t_Cl = t_Cl - 1;	
					t_CM = GF_Clan_Master_Fine(t_Cl);	// B�� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
						//	����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
					}
					else
					{
						// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_Cl);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],p_idx);
						}
					}
				}
				//	B�� �¸� �޽��� ����
				GF_Send_CLANWAR_Progress(Temp_ID,3);
			}
			////	����Ʈ ���
			//GF_ClanWar_Quest_Add((LPVOID)Temp_User,p_idx);
			return false;
			break;
		}
	default:
		return true;
	}
	return true;
}

////	Ŭ���� �������� �׾����� üũ [2�� Ŭ���� ���� : �������� ���� �׾����� üũ]
BOOL	GF_ClanWar_Boss_Kill_Check(LPVOID p_Boss,LPVOID p_User)
{
	//	�ش� ���Ͱ� Ŭ���� �������� Ȯ��
	//	Ŭ���� �����̸� �ش� ���� ���� ó��
	//	���� Ŭ���� ������ HP ������ �ش�.
	//	this�� ������ �ƴ϶� ���� ��ȯ ���ϼ��ִ�.
	if(p_Boss == NULL) return false;
	if(p_User == NULL) return false;
	//	���� Ŭ������ ����ȯ.
	DWORD	Temp_List_N = GF_ClanWar_Boss_MOB_Is(p_Boss);
	if(Temp_List_N == 0) return false;
	//	�ش����� ���� ����Ʈ Ȯ��.
	CObjMOB	*Temp_MOB = (CObjMOB *)p_Boss;
	//	A �� ���� Ȯ��
	if(g_ClanWar_List[Temp_List_N - 1].m_ATeam_Mob[0] == Temp_MOB)
	{
		g_ClanWar_List[Temp_List_N - 1].m_ATeam_Mob[0] = NULL;
		g_ClanWar_List[Temp_List_N - 1].m_Win_Team = 2;
		//	Ŭ���� �� Ÿ�� ó��
		switch(g_ClanWar_List[Temp_List_N - 1].m_War_Type)
		{
		case 0:
			//	50�� ���� (2814:�巹�� ��� ����Ʈ (50���ٸ�))
			GF_ClanWar_Boss_MOB_Kill(Temp_List_N,2814);
			break;
		case 1:
			//	100�� ���� (2815:�巹�� ��� ����Ʈ (100���ٸ�))
			GF_ClanWar_Boss_MOB_Kill(Temp_List_N,2815);
			break;
		case 2:
			//	1000�� ���� (2816:�巹�� ��� ����Ʈ (1000���ٸ�))
			GF_ClanWar_Boss_MOB_Kill(Temp_List_N,2816);
			break;
		}
		return true;
	}
	//	B �� ���� Ȯ��
	if(g_ClanWar_List[Temp_List_N - 1].m_BTeam_Mob[0] == Temp_MOB)
	{
		g_ClanWar_List[Temp_List_N - 1].m_BTeam_Mob[0] = NULL;	
		g_ClanWar_List[Temp_List_N - 1].m_Win_Team = 1;
		//	Ŭ���� �� Ÿ�� ó��
		switch(g_ClanWar_List[Temp_List_N - 1].m_War_Type)
		{
		case 0:
			//	50�� ����
			GF_ClanWar_Boss_MOB_Kill(Temp_List_N,2814);
			break;
		case 1:
			//	100�� ����
			GF_ClanWar_Boss_MOB_Kill(Temp_List_N,2815);
			break;
		case 2:
			//	1000�� ����
			GF_ClanWar_Boss_MOB_Kill(Temp_List_N,2816);
			break;
		}
		return true;
	}
	//	������ ������ ���Ͱ� A��,B�� ��� �������� �ʾҴ�.
	return false;
}

////	HP ������ ���� Ŭ���� ���� ������ Ȯ���Ѵ� . [2�� Ŭ���� ����]
DWORD	GF_ClanWar_Boss_MOB_Is(LPVOID p_Boss)
{
	//	���� �� Ȯ��.
	if(p_Boss == NULL) return 0;
	//	���� Ŭ������ ����ȯ.
	CObjMOB	*Temp_MOB = (CObjMOB *)p_Boss;
	//	���� �ݺ�
	for(int i = 0 ; i < DF_MAX_CWAR_TYPE; i++)
	{
		//	�� �ݺ�
		for(int t = 0 ; t < DF_CWAR_TEAM_N; t++)
		{
			//	������ Ȯ��.
			if(	Temp_MOB->Get_CharNO() == g_CW_MOB_List[i][0][t].m_MOB_Idx)
			{
				//	�������� �� ��ȣ�� �������� Ŭ���� �� ����Ʈ ��ȣ ���.
				return GF_Clan_War_Zone_Is((DWORD)Temp_MOB->GetZONE()->Get_ZoneNO());
			}
		}
	}
	return 0;
}

////	Ŭ���� ������ HP ���� ����
BOOL	GF_ClanWar_Boss_MOB_HP(DWORD p_List_N)
{
	//	����Ʈ ���� Ȯ��.
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	p_List_N = p_List_N - 1;
	
	//	���� ī��Ʈ üũ
	DWORD	Temp_Count = GetTickCount();
	BOOL	Temp_TF = false;
	//	������ ������ HP ī��Ʈ�� ��
	if(g_ClanWar_List[p_List_N].m_Count < Temp_Count)
	{
		//	���� �ð� ī��Ʈ Ȯ��	1.5�ʿ� �ѹ��� üũ
		if((Temp_Count - g_ClanWar_List[p_List_N].m_Count) > 1500)
		{
			//	HP �����Ѵ�.
			Temp_TF = true;
			//	���� ī��Ʈ �����Ѵ�.
			g_ClanWar_List[p_List_N].m_Count = Temp_Count;
		}
	}
	else
	{
		//	Ÿ�̸� ȸ���� ���.
		//	HP �����Ѵ�.
		Temp_TF = true;
		//	���� ī��Ʈ �����Ѵ�.
		g_ClanWar_List[p_List_N].m_Count = Temp_Count;
	}

	// ������ ����� ���
	if(g_ClanWar_List[p_List_N].m_Play_Type == 5)
	{
		Temp_TF = true;	// ����.
	}

	if(Temp_TF)
	{
		//	Ŭ���� ���� ��� �������� ���� HP���� ���� �Ѵ�.
		char	SendBuffer[512];
		ZeroMemory(SendBuffer,512);
		gsv_cli_CLAN_WAR_BOSS_HP	*Send_Data;
		//	���� ���۸� ������ �������� ����ȯ..
		Send_Data = (gsv_cli_CLAN_WAR_BOSS_HP *)SendBuffer;
		//	�޽��� ������ �ִ´�.
		Send_Data->m_wType = GSV_CLANWAR_BOSS_HP;
		Send_Data->m_nSize = sizeof(gsv_cli_CLAN_WAR_BOSS_HP);
	
		// ������ ����� ���
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
			//	A�� ���� ���� HP �ֱ�.
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
			//	B�� ���� ���� HP �ֱ�.
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
		//	�޽��� �ش� ���� ����.
		g_pZoneLIST->Send_gsv_All_User_Data(g_ClanWar_List[p_List_N].m_ZoneNO,SendBuffer);
	}

	return true;
}


////	Ŭ���� ����Ʈ ����
BOOL	GF_ClanWar_Quest_Del(LPVOID p_User,DWORD p_Quest_Idx)
{
	if(p_User == NULL) return false;
	classUSER	*Temp_User;
	Temp_User = (classUSER *)p_User;
	//	����Ʈ ����
	DWORD	Temp_Q = Temp_User->Quest_GetRegistered(p_Quest_Idx);
	//	����Ʈ ��� ��ġ�� ��´�.
	if(Temp_Q < QUEST_PER_PLAYER)
	{
		//	�ش� ����Ʈ�� ���� �Ѵ�.
		Temp_User->Quest_Delete(p_Quest_Idx);
		//	�������� ���� �Ѵ�.
		Temp_User->Send_gsv_QUEST_REPLY( RESULT_QUEST_REPLY_DEL_SUCCESS,(BYTE)Temp_Q,p_Quest_Idx );
	}
	return true;
}

////	Ŭ���� ����Ʈ ���
BOOL	GF_ClanWar_Quest_Add(LPVOID p_User,DWORD p_Quest_Idx)
{
	if(p_User == NULL) return false;
	classUSER	*Temp_User;
	Temp_User = (classUSER *)p_User;
	//	����Ʈ ���
	DWORD	Temp_Q = Temp_User->Quest_Append(p_Quest_Idx);
	//	����Ʈ �������� ���� �Ѵ�.
	Temp_User->Send_gsv_QUEST_REPLY( RESULT_QUEST_REPLY_ADD_SUCCESS, (BYTE)Temp_Q, p_Quest_Idx );
	return true;
}

////	Ŭ���� PK ��� ���� Ȯ��
BOOL	GF_ClanWar_PK_Is(LPVOID p_User)
{
	if(p_User == NULL) return true;
	CObjCHAR	*Temp_User;
	Temp_User = (CObjCHAR *)p_User;
	//	������ �� ��ȣ Ȯ��
	switch(Temp_User->Get_TEAM())
	{
	//	����ȣ
	case 1000:
	case 2000:
		{
			//	�ش� ���� PK ��尡 �ƴϸ� ���� �Ұ�
			CZoneTHREAD	*Temp_Zone = NULL;
			Temp_Zone = Temp_User->GetZONE();
			if(Temp_Zone != NULL)
			{
				//	PK ��尡 ���� ��� �ش� ������ ���� ����.
				if(!Temp_Zone->Get_PK_FLAG())
				{
					//	AI �ൿ ����
					Temp_User->SetCMD_STOP();
					return false;
				}
			}
			break;
		}
	}
	return true;
}


////	Ŭ���� ���� ���� ����Ʈ ó�� �Լ�
//	����Ʈ ��ȣ ini ���� ó�� �ʿ���.
BOOL	GF_ClanWar_Quest_DropProc(DWORD p_List_N,DWORD p_Team)
{
	if(p_List_N == 0)  return false;
	p_List_N = p_List_N - 1;
	//	�ش� ���� �����ϴ� ������ Ȯ�� �ʿ�
	if(g_ClanWar_List[p_List_N].m_CW_Npc == NULL) return false;

	DWORD t_AM,t_BM,t_CM;
	switch(p_Team)
	{
	case 0:
		{
			//	���º�
			//	��� ��� ����Ʈ �ϴ�. ���� ����Ʈ ����.
			switch(g_ClanWar_List[p_List_N].m_War_Type)
			{
			case 0:
				//	A,B �� ������ ����Ʈ ����.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
						//	���� ����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2817);
					}
					else
					{
						// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_AM);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2817);
						}
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
						//	���� ����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2817);
					}
					else
					{
						// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_BM);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2817);
						}
					}
				}
				//	���º� �¸� �޽��� ����
				GF_Send_CLANWAR_Progress(p_List_N + 1,4);
				break;
			case 1:
				//	A,B �� ������ ����Ʈ ����.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
						//	���� ����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2818);
					}
					else
					{
						// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_AM);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2818);
						}
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
						//	���� ����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2818);
					}
					else
					{
						// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_BM);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2818);
						}
					}
				}
				//	���º� �¸� �޽��� ����
				GF_Send_CLANWAR_Progress(p_List_N + 1,4);
				break;
			case 2:
				//	A,B �� ������ ����Ʈ ����.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
						//	���� ����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2819);
					}
					else
					{
						// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_AM);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2819);
						}
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
						//	���� ����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2819);
					}
					else
					{	// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_BM);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2819);
						}
					}
				}
				//	���º� �¸� �޽��� ����
				GF_Send_CLANWAR_Progress(p_List_N + 1,4);
				break;
			}
			break;
		}
	case 1000:
		{
			//	B �� �¸�.
			switch(g_ClanWar_List[p_List_N].m_War_Type)
			{
			case 0:
				//	A,B �� ������ ����Ʈ ����.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
						//	�¸� ����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2814);
					}
					else
					{
						// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_BM);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2814);
						}
					}
				}
				break;
			case 1:
				//	A,B �� ������ ����Ʈ ����.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
						//	�¸� ����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2815);
					}
					else
					{
						// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_BM);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2815);
						}
					}
				}
				break;
			case 2:
				//	A,B �� ������ ����Ʈ ����.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
						//	�¸� ����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2816);
					}
					else
					{
						// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_BM);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2816);
						}
					}
				}
				break;
			}
			//	B�� �¸� �޽��� ����
			GF_Send_CLANWAR_Progress(p_List_N + 1,3);
			break;
		}
	case 2000:
		{
			//	A �� �¸�.
			switch(g_ClanWar_List[p_List_N].m_War_Type)
			{
			case 0:
				//	A,B �� ������ ����Ʈ ����.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
						//	�¸� ����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2814);
					}
					else
					{
						// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_AM);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2814);
						}
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
					}
				}
				break;
			case 1:
				//	A,B �� ������ ����Ʈ ����.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
						//	�¸� ����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2815);
					}
					else
					{
						// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_AM);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2815);
						}
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
					}
				}
				break;
			case 2:
				//	A,B �� ������ ����Ʈ ����.
				t_AM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
				t_BM = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
				if(t_AM > 0)
				{
					t_AM = t_AM - 1;	
					t_CM = GF_Clan_Master_Fine(t_AM);	//A �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
						//	�¸� ����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2816);
					}
					else
					{
						// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
						// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
						t_CM = GF_Clan_Master_Sub_Fine(t_AM);
						if(t_CM > 0)
						{
							//	����Ʈ ���
							GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2816);
						}
					}
				}
				if(t_BM > 0)
				{
					t_BM = t_BM - 1;	
					t_CM = GF_Clan_Master_Fine(t_BM);	//B �� ������ ã��
					if(t_CM > 0)
					{
						//	����Ʈ �����.
						GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
					}
				}
				break;
			}
			//	A�� �¸� �޽��� ����
			GF_Send_CLANWAR_Progress(p_List_N + 1,2);
			break;
		}
	default:
		return false;
	}
	return true;
}

////	Ŭ���� ���� ó�� �޽���
BOOL	GF_ClanWar_GiveUP(DWORD p_List_N,DWORD p_Team)
{
	//	������ ���� �ݴ� ���� �¸� �޽��� ����
	if(p_List_N == 0)  return false;
	p_List_N = p_List_N - 1;
	//	A,B�� Ŭ�� ����Ʈ ��ȣ ã��
	DWORD	t_n = 0;
	DWORD	t_ID = 0;
	//	����ȣ Ȯ��
	if(p_Team == 1000)
	{
		//	B �� �¸�
		//	�ش� Ŭ�� ����Ʈ ��ȣ ���
		t_n = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
		if(t_n > 0)
		{
			t_n = t_n - 1;
			//	���� Ŭ���� ������ ���
			t_ID = GF_Clan_Master_Fine(t_n);	// B �� ������ ã��
			if(t_ID > 0)
			{
				//	������ ���� ���� �޽��� ����
				g_User_List[t_ID - 1]->Send_gsv_CLANWAR_Err(CWAR_OPPONENT_MASTER_ALREADY_GIVEUP);
			}
		}
	}
	else if(p_Team == 2000)
	{
		//	A �� �¸�
		t_n = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);
		if(t_n > 0)
		{
			t_n = t_n - 1;
			t_ID = GF_Clan_Master_Fine(t_n);	// A �� ������ ã��
			if(t_ID > 0)
			{
				//	������ ã��
				g_User_List[t_ID - 1]->Send_gsv_CLANWAR_Err(CWAR_OPPONENT_MASTER_ALREADY_GIVEUP);
			}
		}
	}
	return true;
}

////	Ŭ���� �������� Ŭ�� Ȯ�� �Լ�
DWORD	GF_Get_CLANWAR_Check(DWORD p_Clan_N,DWORD p_User_LV)
{
	if(p_Clan_N == 0)		return 0;
	DWORD	Temp_HI = 0;
	DWORD	Temp_LO = 0;
	DWORD	Temp_DW = 0;
	// Ȯ��
	for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
	{
		//	�ش� ���� ���� ���� ������ �˻� �Ѵ�.
		if(g_ClanWar_List[i].m_CW_Npc != NULL)
		{
			//	Ŭ���� ������
			if(g_ClanWar_List[i].m_Play_Type == 4)
			{
				//	���� Ŭ���� ���������� Ȯ���Ѵ�.
				if(g_ClanWar_List[i].m_Clan_A == p_Clan_N)
				{
					//	���� ó�� �Ѵ�.
					if(GF_Clan_War_Check_LV(p_User_LV,g_ClanWar_List[i].m_War_Type))
						Temp_HI = 1;
					else
						Temp_HI = 3;	// LV ����.
					//	���ȣ �ֱ�
					Temp_LO = i+1;
					//	���ϰ� ���� : ���� ���� ����ȣ, ���� ���� ���ȣ�̴�.
					Temp_DW = (Temp_HI<<16) + Temp_LO;

					return Temp_DW;
				}
				if(g_ClanWar_List[i].m_Clan_B == p_Clan_N)
				{
					//	���� ó�� �Ѵ�.
					if(GF_Clan_War_Check_LV(p_User_LV,g_ClanWar_List[i].m_War_Type))
						Temp_HI = 2;
					else
						Temp_HI = 3;	// LV ����.
					//	���ȣ �ֱ�
					Temp_LO = i+1;
					//	���ϰ� ���� : ���� ���� ����ȣ, ���� ���� ���ȣ�̴�.
					Temp_DW = (Temp_HI<<16) + Temp_LO;

					return Temp_DW;
				}
			}
			//	Ŭ���� ��û ���� �浵 üũ�Ѵ�.
			else if(g_ClanWar_List[i].m_Play_Type == 1)
			{
				//	��û ���� ���� �ִ�. ���ȣ �� �Ѱ��ش�.
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

////	Ŭ���� ���� ������ ���� �޽��� �����Ѵ�.
BOOL	GF_Send_CLANWAR_Progress(DWORD p_List_N,DWORD p_Type)
{
	if(p_List_N == 0) return false;
	p_List_N = p_List_N - 1;
	if(p_List_N >= DF_MAX_CWAR_LIST) return false;

	//	Ŭ������ �����ϰ� �ִ� �����鿡�� �޽��� ������.
	DWORD	Temp_CN = 0;
	DWORD	Temp_ID = 0;

	//	DB�� ���� ��� �ϱ�
	switch(p_Type)
	{
	case 2:
		// A�� �¸�
		GF_DB_CWar_Insert(4,2,g_ClanWar_List[p_List_N].m_Bat_Coin,g_ClanWar_List[p_List_N].m_Clan_A,g_ClanWar_List[p_List_N].m_Clan_B,NULL);
		//	A,B �� ���� ó�� �߰�.
		break;
	case 3:
		//	B�� �¸�
		GF_DB_CWar_Insert(4,3,g_ClanWar_List[p_List_N].m_Bat_Coin,g_ClanWar_List[p_List_N].m_Clan_A,g_ClanWar_List[p_List_N].m_Clan_B,NULL);
		//	A,B �� ���� ó�� �߰�.
		break;
	case 4:
		//	Ŭ���� ��� ��� ó�� DB ����.
		GF_DB_CWar_Insert(4,1,g_ClanWar_List[p_List_N].m_Bat_Coin,g_ClanWar_List[p_List_N].m_Clan_A,g_ClanWar_List[p_List_N].m_Clan_B,NULL);
		//	A,B �� ���� ó�� �߰�.
		break;
	}

	//	A�� B�� ��ǥ ����.
	LPVOID	p_ATeam_U = NULL;
	LPVOID	p_BTeam_U = NULL;

	//	A�� Ŭ�����鿡�� �޽��� ����
	Temp_CN = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_A);

	//	Ŭ�� ����Ʈ ��ȣ ����
	if(Temp_CN > 0)
	{
		Temp_CN = Temp_CN - 1;
		// for(DWORD i = 0; i < 15;i++)
		// (20070125:����ö) : Ŭ���� ��ġ�� 15�� �ϵ��ڵ��� CW_USER_NO�� ��ġ�� ���� (�ִ� 50����� ����)
		for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0]; i++ )
		{
			//	Ŭ�� ���� ����Ʈ ��ȣ ���
			Temp_ID = g_Clan_List[Temp_CN].m_User_IDX[i];
			if(0 < Temp_ID)
			{
				//	�ش� ���� �ִ��� Ȯ��
				if(g_User_List[Temp_ID - 1]->m_nZoneNO == g_ClanWar_List[p_List_N].m_ZoneNO)
				{
					//	�޽��� ������.
					g_User_List[Temp_ID - 1]->Send_gsv_CLANWAR_Progress((WORD)p_Type);
					//	Ŭ���� ó�� �˻� ���� ����
					if(p_ATeam_U == NULL)	p_ATeam_U = (LPVOID)g_User_List[Temp_ID - 1];
				}
				//	Ŭ������ ���
				if(g_User_List[Temp_ID - 1]->Is_ClanMASTER())
				{
					g_User_List[Temp_ID - 1]->m_Quests.m_nClanWarVAR[6] = 0;
					//	Ŭ�� ������ ����
					p_ATeam_U = (LPVOID)g_User_List[Temp_ID - 1];
				}
			}
		}
	}
	//	B�� Ŭ�����鿡�� �޽��� ����
	Temp_CN = GF_Clan_Fine_List(g_ClanWar_List[p_List_N].m_Clan_B);
	//	Ŭ�� ����Ʈ ��ȣ ����
	if(Temp_CN > 0)
	{
		Temp_CN = Temp_CN - 1;
		// for(DWORD i = 0; i < 15;i++)
		// (20070125:����ö) : Ŭ���� ��ġ�� 15�� �ϵ��ڵ��� CW_USER_NO�� ��ġ�� ���� (�ִ� 50����� ����)
		for( DWORD i = 0; i < G_CW_MAX_USER_NO[0][0]; i++ )
		{
			//	Ŭ�� ���� ����Ʈ ��ȣ ���
			Temp_ID = g_Clan_List[Temp_CN].m_User_IDX[i];
			if(0 < Temp_ID)
			{
				//	�ش� ���� �ִ��� Ȯ��
				if(g_User_List[Temp_ID - 1]->m_nZoneNO == g_ClanWar_List[p_List_N].m_ZoneNO)
				{
					//	�޽��� ������.
					g_User_List[Temp_ID - 1]->Send_gsv_CLANWAR_Progress((WORD)p_Type);
					//	Ŭ���� ó�� �˻� ���� ����
					if(p_BTeam_U == NULL)	p_BTeam_U = (LPVOID)g_User_List[Temp_ID - 1];
				}
				//	Ŭ������ ���
				if(g_User_List[Temp_ID - 1]->Is_ClanMASTER())
				{
					g_User_List[Temp_ID - 1]->m_Quests.m_nClanWarVAR[6] = 0;
					//	Ŭ�� ������ ����
					p_BTeam_U = (LPVOID)g_User_List[Temp_ID - 1];
				}
			}
		}
	}

	////	Ŭ�� ��ŷ ����Ʈ ����
	GF_Set_ClanWar_RankPoint(p_Type,p_ATeam_U,p_BTeam_U,&g_ClanWar_List[p_List_N]);//.m_War_Type);

	return true;
}


////	Ŭ���� ��Ȱ ��ġ ���
BOOL	GF_Get_ClanWar_Revival_Pos(DWORD p_List_N,DWORD pTeam,float &Out_x,float &Out_Y)
{
	//	���� ó��.
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	if(pTeam == 1000)
	{
		//	A �� ��Ȱ ��ġ��
		Out_x = g_APosREVIVAL.x;
		Out_Y = g_APosREVIVAL.y;
		//Out_x = g_ClanWar_List[p_List_N - 1].m_APosREVIVAL.x;
		//Out_Y = g_ClanWar_List[p_List_N - 1].m_APosREVIVAL.y;
		return true;
	}
	else
	{
		//	B �� ��Ȱ ��ġ��
		Out_x = g_BPosREVIVAL.x;
		Out_Y = g_BPosREVIVAL.y;
		//Out_x = g_ClanWar_List[p_List_N - 1].m_BPosREVIVAL.x;
		//Out_Y = g_ClanWar_List[p_List_N - 1].m_BPosREVIVAL.y;
		return true;
	}
	return false;
}

////	Ŭ���� �ʿ� GATE NPC ���� ó�� 
BOOL	GF_Set_ClanWar_Gate_NPC(LPVOID p_User,DWORD p_List_N,BOOL p_Type)
{
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	p_List_N = p_List_N - 1;
	//	���� ���� �ݵ��� ó���Ѵ�.
	if(p_User == NULL)
	{
		//	p_List_N ���� ��� �������� ����
		char	SendBuffer[512];
		ZeroMemory(SendBuffer,512);
		gsv_cli_USER_MSG_1LV	*Send_Data;
		//	���� ���۸� ������ �������� ����ȯ..
		Send_Data = (gsv_cli_USER_MSG_1LV *)SendBuffer;

		//	�޽��� ������ �ִ´�.
		Send_Data->m_wType = GSV_USER_MSG_1LV;
		Send_Data->m_nSize = sizeof(gsv_cli_USER_MSG_1LV);
		Send_Data->m_Sub_Type = 100;	// �� ��Ʈ�� ����
		if(p_Type)
		{
			//	���� ���� ����
			if(g_ClanWar_List[p_List_N].m_Gate_TF)
				Send_Data->m_Data = 2;	// �����ִ� ���� ���.
			else
				Send_Data->m_Data = g_ClanWar_List[p_List_N].m_Gate_TF;		// ON/OFF ����
		}
		else
		{
			Send_Data->m_Data = g_ClanWar_List[p_List_N].m_Gate_TF;		// ON/OFF ����
		}

		//	�޽��� �ش� ���� ����.
		g_pZoneLIST->Send_gsv_All_User_Data(g_ClanWar_List[p_List_N].m_ZoneNO,SendBuffer);
	}
	else
	{
		//	p_User �������� ����.
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

////	Ŭ���� ���� �ο� ����
BOOL	GF_Add_ClanWar_Number(DWORD p_List_N,DWORD pTeam)
{
	//	���� ���� �渮��Ʈ ��ȣ Ȯ��.
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	p_List_N = p_List_N - 1;
	//	������ ����ȣ Ȯ��
	switch(pTeam)
	{
	case 1000:
		{
			//	A �� �ο� ����.
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
			//	B �� �ο� ����.
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

////	Ŭ���� ���� �ο� ����
BOOL	GF_Sub_ClanWar_Number(DWORD p_List_N,DWORD pTeam)
{
	//	���� ���� �渮��Ʈ ��ȣ Ȯ��.
	if(p_List_N == 0) return false;
	if(p_List_N > DF_MAX_CWAR_LIST) return false;
	p_List_N = p_List_N - 1;
	//	������ ����ȣ Ȯ��
	switch(pTeam)
	{
	case 1000:
		{
			//	A �� �ο� ����.
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
			//	B �� �ο� ����.
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

////	Ŭ���� ����Ʈ ó�� �κ�
BOOL	GF_Set_ClanWar_RankPoint(DWORD p_Type,LPVOID p_User_A,LPVOID p_User_B, void* vp_ClanWar_Set )//DWORD	WarType)
{
	// (20061230:����ö) : ���� ����Ʈ �߰��� ���� ����
	GF_ClanWar_Set* p_ClanWar_Set = (GF_ClanWar_Set*)vp_ClanWar_Set;
	DWORD WarType = p_ClanWar_Set->m_War_Type;

	//	���� Ŭ������ ����.
	classUSER *T_User_A;
	classUSER *T_User_B;
	T_User_A = (classUSER *)p_User_A;
	T_User_B = (classUSER *)p_User_B;
	//	������ Ŭ�� ���� ���� ����
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
	//	���� ��´�.
	float	fRate_A = GF_Get_Rank_Rate(Rank_A,Rank_B);
	float	fRate_B = 1.0f - fRate_A;

	//	����Ʈ ���
	float	fLost = 0.0f,fWin = 0.0f;

	//	�����ѿ� ���� ���� ����
	float	fLost_Rat = 1.0f;
	float	fWin_Rat = 1.0f;

	//	����Ǵ� ��.
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

	//	���� ó�� �κ�.
	switch(p_Type)
	{
	case 2:
		//	A�� �¸�
		fLost = 30.0f*fRate_B + ((float)Rank_B)*0.01f;	// ������ �ս� ����
		fWin = fLost*(1.0f - fRate_A) + 1.0f;	// �̱����� �ս� ����
		if(T_User_A != NULL)
		{
			//	���� �����Ѵ�.
			i_Add_Rate = (int)(fWin*fWin_Rat);
			Rank_A = Rank_A + i_Add_Rate;
			T_User_A->m_CLAN.m_iRankPoint = Rank_A;
			//	�ش� Ŭ���� ��ũ ���� ���� �޽��� ����
			if(NewA_TF)
				T_User_A->SetClanRank(Rank_A);
			else
				T_User_A->SetClanRank(i_Add_Rate);


#ifdef __CLAN_REWARD_POINT
			// (20061229:����ö) : Ŭ���� ���� ����Ʈ ����
			switch( WarType )
			{
			case 0: // 50�� �ٸ�
				p_ClanWar_Set->m_dwATeamRewardPoint = 20;
				break;

			case 1: // 100�� �ٸ�
				p_ClanWar_Set->m_dwATeamRewardPoint = 50;
				break;

			case 2: // 1000�� �ٸ�
				p_ClanWar_Set->m_dwATeamRewardPoint = 100;
				break;
			}
#endif
		}

		if(T_User_B != NULL)
		{
			//	���� �����Ѵ�.
			i_Add_Rate = - (int)(fLost*fLost_Rat);
			if((Rank_B + i_Add_Rate) < 0)
			{
				//	0�� �����.
				i_Add_Rate = -Rank_B;
				Rank_B = 0;
			}
			else
                Rank_B = Rank_B + i_Add_Rate;

			T_User_B->m_CLAN.m_iRankPoint = Rank_B;

			//	�ش� Ŭ���� ��ũ ���� ���� �޽��� ����
			if(NewB_TF)
				T_User_B->SetClanRank(Rank_B);
			else
				T_User_B->SetClanRank(i_Add_Rate);


#ifdef __CLAN_REWARD_POINT
			// (20061229:����ö) : Ŭ���� ���� ����Ʈ ����
			switch( WarType )
			{
			case 0: // 50�� �ٸ�
				p_ClanWar_Set->m_dwBTeamRewardPoint = 10;
				break;

			case 1: // 100�� �ٸ�
				p_ClanWar_Set->m_dwBTeamRewardPoint = 10;
				break;

			case 2: // 1000�� �ٸ�
				p_ClanWar_Set->m_dwBTeamRewardPoint = 10;
				break;
			}
#endif
		}
		break;
	case 3:
		//	B�� �¸�.
		fLost = 30.0f*fRate_A + ((float)Rank_A)*0.01f;	// ������ �ս� ����
		fWin = fLost*(1.0f - fRate_B) + 1.0f;	// �̱����� �ս� ����
		if(T_User_B != NULL)
		{
			//	���� �����Ѵ�.
			i_Add_Rate = (int)(fWin*fWin_Rat);
			Rank_B = Rank_B + i_Add_Rate;
			T_User_B->m_CLAN.m_iRankPoint = Rank_B;
			//	�ش� Ŭ���� ��ũ ���� ���� �޽��� ����
			if(NewB_TF)
				T_User_B->SetClanRank(Rank_B);
			else
				T_User_B->SetClanRank(i_Add_Rate);


#ifdef __CLAN_REWARD_POINT
			// (20061229:����ö) : Ŭ���� ���� ����Ʈ ����
			switch( WarType )
			{
			case 0: // 50�� �ٸ�
				p_ClanWar_Set->m_dwBTeamRewardPoint = 20;
				break;

			case 1: // 100�� �ٸ�
				p_ClanWar_Set->m_dwBTeamRewardPoint = 50;
				break;

			case 2: // 1000�� �ٸ�
				p_ClanWar_Set->m_dwBTeamRewardPoint = 100;
				break;
			}
#endif
		}
		if(T_User_A != NULL)
		{
			//	���� �����Ѵ�.
			i_Add_Rate = - (int)(fLost*fLost_Rat);
			if((Rank_A + i_Add_Rate) < 0)
			{
				i_Add_Rate = -Rank_A;
				Rank_A = 0;
			}
			else
				Rank_A = Rank_A + i_Add_Rate;
			T_User_A->m_CLAN.m_iRankPoint = Rank_A;
			//	�ش� Ŭ���� ��ũ ���� ���� �޽��� ����
			if(NewA_TF)
				T_User_A->SetClanRank(Rank_A);
			else
				T_User_A->SetClanRank(i_Add_Rate);


#ifdef __CLAN_REWARD_POINT
			// (20061229:����ö) : Ŭ���� ���� ����Ʈ ����
			switch( WarType )
			{
			case 0: // 50�� �ٸ�
				p_ClanWar_Set->m_dwATeamRewardPoint = 10;
				break;

			case 1: // 100�� �ٸ�
				p_ClanWar_Set->m_dwATeamRewardPoint = 10;
				break;

			case 2: // 1000�� �ٸ�
				p_ClanWar_Set->m_dwATeamRewardPoint = 10;
				break;
			}
#endif
		}
		break;

	case 4:
#ifdef __CLAN_REWARD_POINT
		// Ŭ���� ��� ��� ó�� : ���� ���� ����
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

////	Ŭ�� ����Ʈ �������� ���
float	GF_Get_Rank_Rate(int Ateam_P,int BTeam_P)
{
	//	A���� �������� ó���Ѵ�.
	int	iRate = BTeam_P - Ateam_P;
	//	���� ó���Ѵ�.
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

// (20070104:����ö) : Ŭ�� ���� ����Ʈ ����
/*
BOOL	GF_Add_ClanWar_RewardPoint( int iClanID, int iRewardPoint )
{
	DWORD	Temp_ID = 0;

	for(DWORD i = 0; i < 15;i++)
	{
		//	���� ���� Ȯ��.
		Temp_ID = g_Clan_List[iClanID].m_User_IDX[i];
		if(0 < Temp_ID)
		{
			// ��� Ŭ������ ���� ����Ʈ�� ������Ų��.
//			g_User_List[Temp_ID - 1]->m_GrowAbility.AddClanRewardPoint( iRewardPoint );

			// ���� Ŭ���̾�Ʈ �������� ���� ����
			//g_User_List[Temp_ID - 1]->Send_
			//this->Send_SetCLAN ( RESULT_CLAN_MY_DATA, pUSER );
		}
	}

	return TRUE;
}
*/



//#endif

