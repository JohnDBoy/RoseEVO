
/*
	Ŭ�� �ʵ� ���� �Լ� �߰� 
	2006�� 10�� 16�� : �ۼ��� �迵ȯ
*/

#include "StdAfx.h"
#include "ZoneLIST.h"
#include "F_GDI_List.h"

////	Ŭ�� �ʵ� ���� ���� ����
CF_Setting	g_CF_Set[MAX_CF_SET_N];

////	Ŭ�� �ʵ� �÷��� ���� ���� �迭 ��ȣ
BOOL	g_CF_Play_TF = false;
int		g_CF_Play_N = 0;

////	Ŭ�� �ʵ� ���� ���� ����
HWND	g_hList_CField = NULL;

//	ũ��Ƽ�� ���� ó��
CRITICAL_SECTION	g_CF_Play_LOCK;

////	�� ����Ʈ
extern CZoneLIST		*g_pZoneLIST;


////	Ŭ�� ��Ʈ ũ��Ƽ�� ����
BOOL	GF_CF_CRITICAL_Set(bool pType,HWND	p_HWND)
{
	//	ũ��Ƽ�� ����, ó��
	if(pType)
	{
		InitializeCriticalSection( &g_CF_Play_LOCK );

		//	����Ʈ ����
		g_hList_CField = FL_Create_List(p_HWND,"Clan Field Event View",200,0,300,150);
		ShowWindow(g_hList_CField,SW_HIDE);
		if(g_hList_CField == NULL) return false;
		//	����Ʈ�� Ÿ��Ʋ �ֱ�
		FL_Set_List_Col(g_hList_CField,0,50,"NO");
		FL_Set_List_Col(g_hList_CField,1,50,"Open Start");
		FL_Set_List_Col(g_hList_CField,2,50,"Open End");
		FL_Set_List_Col(g_hList_CField,3,50,"Close Notice ");
		FL_Set_List_Col(g_hList_CField,4,50,"Close Time");
	}
	else
	{
		DeleteCriticalSection(&g_CF_Play_LOCK);
		//	����Ʈ ����
		DestroyWindow(g_hList_CField);
	}
	return true;
}

////	Ŭ�� �ʵ� ���� ���� �б�
BOOL	GF_CF_Set_INI_Load()
{
	//	����ü �ʱ�ȭ
	ZeroMemory(g_CF_Set,MAX_CF_SET_N*sizeof(CF_Setting));

	//	��� ���� �б�
	char	Temp_File[MAX_PATH];
	char	Temp_Dir[MAX_PATH];
	ZeroMemory(Temp_File,MAX_PATH);
	ZeroMemory(Temp_Dir,MAX_PATH);
	//	���� ��� ���
	GetCurrentDirectory(MAX_PATH,Temp_Dir);
	wsprintf(Temp_File,"%s\\SHO_GS_CField.ini",Temp_Dir);

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

	//	�ӽ� ����
	char	Temp_String[30];
	char	t_Title[30];
	char	t_Name[30];
	int		Temp_i = 0;
	//	INI ���Ͽ��� �о� ���δ�.
	for(int i = 0 ; i < MAX_CF_SET_N; i++)
	{
		//	�̺�Ʈ ��ȣ �ֱ�
		ZeroMemory(t_Title,30);
		wsprintf(t_Title,"CF_EVENT_%d",i+1);

		//	���Ͽ��� �б�
		ZeroMemory(Temp_String,30);
		GetPrivateProfileString(t_Title,"CF_NPC_NO", "0", Temp_String,30, Temp_File);
		Temp_i = atoi(Temp_String);
		if(Temp_i < 0) Temp_i = 0;
		g_CF_Set[i].m_NPC_No = (DWORD)Temp_i;	// NPC ��ȣ
		g_CF_Set[i].m_NPC = NULL;				// NPC ������
		
		ZeroMemory(Temp_String,30);
		GetPrivateProfileString(t_Title,"CF_NPC_ZONE", "0", Temp_String,30, Temp_File);
		Temp_i = atoi(Temp_String);
		if(Temp_i < 0) Temp_i = 0;
		g_CF_Set[i].m_NPC_Zone = (DWORD)Temp_i;	// NPC�� �ִ� ��.

		ZeroMemory(Temp_String,30);
		GetPrivateProfileString(t_Title,"CF_OPEN_S_TIME", "0", Temp_String,30, Temp_File);
		Temp_i = atoi(Temp_String);
		if(Temp_i < 0) Temp_i = 0;
		g_CF_Set[i].m_Start_Time[0] = (DWORD)((Temp_i/100)*60 + Temp_i%100);	// ���� ���� �ð�
		
		ZeroMemory(Temp_String,30);
		GetPrivateProfileString(t_Title,"CF_OPEN_E_TIME", "0", Temp_String,30, Temp_File);
		Temp_i = atoi(Temp_String);
		if(Temp_i < 0) Temp_i = 0;
		g_CF_Set[i].m_Start_Time[1] = (DWORD)((Temp_i/100)*60 + Temp_i%100);	// ���� ���� �ð�
		
		ZeroMemory(Temp_String,30);
		GetPrivateProfileString(t_Title,"CF_CLOSE_S_TIME", "0", Temp_String,30, Temp_File);
		Temp_i = atoi(Temp_String);
		if(Temp_i < 0) Temp_i = 0;
		g_CF_Set[i].m_End_Time[0] = (DWORD)((Temp_i/100)*60 + Temp_i%100);	// �ݱ� ���� �ð�
		
		ZeroMemory(Temp_String,30);
		GetPrivateProfileString(t_Title,"CF_CLOSE_E_TIME", "0", Temp_String,30, Temp_File);
		Temp_i = atoi(Temp_String);
		if(Temp_i < 0) Temp_i = 0;
		g_CF_Set[i].m_End_Time[1] = (DWORD)((Temp_i/100)*60 + Temp_i%100);	// �ݱ� ���� �ð�

		////	��ȯ �� �� ��ġ �б�.
		DWORD j;
		for(j = 0 ; j < 5; j++)
		{
			ZeroMemory(t_Name,30);
			wsprintf(t_Name,"CF_RETURN_ZONE_%d",j+1);

			ZeroMemory(Temp_String,30);
			GetPrivateProfileString(t_Title,t_Name, "0", Temp_String,30, Temp_File);
			Temp_i = atoi(Temp_String);
			if(Temp_i < 0) Temp_i = 0;
			g_CF_Set[i].m_ReturnNO[j] = (WORD)Temp_i;	// ��ȯ ��
		}
		for(j = 0 ; j < 5; j++)
		{
			ZeroMemory(t_Name,30);
			wsprintf(t_Name,"CF_RETURN_POS_%d_X",j+1);

			ZeroMemory(Temp_String,30);
			GetPrivateProfileString(t_Title,t_Name, "0", Temp_String,30, Temp_File);
			Temp_i = atoi(Temp_String);
			if(Temp_i < 0) Temp_i = 0;
			g_CF_Set[i].m_ReturnPos[j].x = (float)Temp_i;	// ��ȯ ��ġ X
			
			ZeroMemory(t_Name,30);
			wsprintf(t_Name,"CF_RETURN_POS_%d_Y",j+1);

			ZeroMemory(Temp_String,30);
			GetPrivateProfileString(t_Title,t_Name, "0", Temp_String,30, Temp_File);
			Temp_i = atoi(Temp_String);
			if(Temp_i < 0) Temp_i = 0;
			g_CF_Set[i].m_ReturnPos[j].y = (float)Temp_i;	// ��ȯ ��ġ Y
		}
	}
	//	NPC ã��
	GF_CF_INI_NPC_Find();
	//	���°� �ʱ�ȭ.
	g_CF_Play_TF = false;
	//	Ÿ�� ���ν��� �ʱ� ȣ��
	GF_CF_Time_Proc();

	//	����Ʈ �� ����
	GF_CF_Event_View();
	return true;
}

////	Ŭ�� �ʵ� ���� NPC ã�� �Լ�
BOOL	GF_CF_INI_NPC_Find()
{
	////	�� ����Ʈ Ȯ��
	if(!g_pZoneLIST) return false;
	
	//	NPC ������ ã��
	for(int i = 0 ; i < MAX_CF_SET_N;i++)
	{
		if(g_CF_Set[i].m_NPC_No > 0)
		{
			g_CF_Set[i].m_NPC = (LPVOID)g_pZoneLIST->Get_LocalNPC(g_CF_Set[i].m_NPC_No);
		}
	}
	// NPC �����Ͱ� NULL ��� ������� �ʴ� Ŭ�� �ʵ� �̺�Ʈ�̴�.
	return true;
}

////	Ŭ�� �ʵ� �̺�Ʈ ��
BOOL	GF_CF_Event_View()
{
	if(g_hList_CField == NULL) return false;
	//	Ŭ�� �ʵ� �̺�Ʈ ���� �ֱ�.
	char	Temp_S[100];
	//	������ �ֱ�
	for(int i = 0; i < MAX_CF_SET_N; i++)
	{
		//	��ȣ �ֱ�
		wsprintf(Temp_S,"%d",i+1);
		FL_Set_List_Data(g_hList_CField,i,0,Temp_S);
		if(g_CF_Set[i].m_NPC == NULL)
		{
			// ���� �ȵ� ����
			wsprintf(Temp_S,"0");
			FL_Set_List_Data(g_hList_CField,i,1,Temp_S);
			FL_Set_List_Data(g_hList_CField,i,2,Temp_S);
			FL_Set_List_Data(g_hList_CField,i,3,Temp_S);
			FL_Set_List_Data(g_hList_CField,i,4,Temp_S);
		}
		else
		{
			//	�ð� ���� ���
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

////	Ŭ�� �ʵ� ���� Ÿ�� �Լ�
BOOL	GF_CF_Time_Proc()
{
	//	30�ʿ� �ѹ��� ����ȴ�.
	//	�ý��� �ð��� ��´�.
	SYSTEMTIME	t_STime;
	GetLocalTime(&t_STime);

	//	�д����� Ÿ�� ó��.
	DWORD	t_Time = t_STime.wHour*60 + t_STime.wMinute;

	//	NPC ���� ó��.
	BOOL	t_Start_TF = false;
	int		t_Start_N = 0;
	//	Ŭ���ʵ� �÷����� Ȯ��
	BOOL	t_Play_TF = false;
	int		t_Play_N = 0;

	//	���� ���� ������ Ȯ���Ѵ�.
	for(int i = 0 ; i < MAX_CF_SET_N; i++)
	{
		//	NPC Ȯ��
		if((g_CF_Set[i].m_NPC != NULL)&&(g_CF_Set[i].m_NPC_No > 0))
		{
			//	���� �ð����� ���� ���� �ð� üũ
			if((g_CF_Set[i].m_Start_Time[0] <= t_Time)&&(g_CF_Set[i].m_Start_Time[1] >= t_Time))
			{
				//	NPC ���� 1�� ���� �ؾ��Ѵ�.
				t_Start_TF = true;
				t_Start_N = i;
			}
			//	���� ��� �ð����� ���� �ð� üũ
			if((g_CF_Set[i].m_End_Time[0] <= t_Time)&&(g_CF_Set[i].m_End_Time[1] > t_Time))
			{
				//	���� ��� �뺸 �Ѵ�.
			}
			//	�÷��� �ð� üũ.
			if((g_CF_Set[i].m_Start_Time[0] <= t_Time)&&(g_CF_Set[i].m_End_Time[1] >= t_Time))
			{
				// Ŭ�� �ʵ� �÷��� �ð�.
				t_Play_TF = true;
				t_Play_N = i;
			}
		}
	}
	
	//	NPC ���� ���� Ȯ��
	GF_CF_NPC_Var_Set(t_Start_TF,t_Start_N);

	//	�÷��� ���� ���� Ȯ��.
	if(g_CF_Play_TF != t_Play_TF)
	{
		//	���� �÷��� ���� ���� ���� ���� ���� ��.
		g_CF_Play_TF = t_Play_TF;
		g_CF_Play_N = t_Play_N;	// �迭 ��ġ ����.
	}

	//	������ ���� �� ��� Ŭ�� �ʵ��� ������ ���� ��Ų��.
	if(!g_CF_Play_TF)
	{
		//	��� ���� ��Ŵ.
		GF_CF_End_Warp();
	}

	return true;
}

////	Ŭ�� �ʵ� ���� NPC ���� ����.
//	m_nAI_VAR[0] ���� 0�̸� ���� �Ұ�. m_nAI_VAR[0] ���� 1�̸� ���� ����.
BOOL	GF_CF_NPC_Var_Set(BOOL p_Type,int p_List)
{
	//	���ް� Ȯ��
	if(MAX_CF_SET_N <= p_List) return false;
	//	�ش� �̺�Ʈ ������ NPC Ȯ���Ѵ�.
	if(g_CF_Set[p_List].m_NPC == NULL) return false;

	//	���޵� p_Type ���� ���Ͽ� m_nAI_VAR[0] ���� �����Ѵ�.
	CObjNPC	*Temp_NPC = (CObjNPC *)g_CF_Set[p_List].m_NPC;
	if(p_Type)
	{
		//	m_nAI_VAR[0] ���� 0�̸� 1�� ����.
		if(0 == Temp_NPC->Get_ObjVAR(0))
			Temp_NPC->Set_ObjVAR(0,1);
	}
	else
	{
		//	m_nAI_VAR[0] ���� 1�̸� 0�� ����.
		if(0 < Temp_NPC->Get_ObjVAR(0))
			Temp_NPC->Set_ObjVAR(0,0);
	}
	return true;
}

////	Ŭ�� �ʵ� ���� �뺸 ó��
BOOL	GF_CF_End_Time_Send()
{
	return true;
}

////	Ŭ�� �ʵ� ���� ���� ���� ��Ŵ
BOOL	GF_CF_End_Warp()
{
	//	���� ���� Ȯ��
	if(g_pZoneLIST == NULL) return false;

    //	Ŭ�� �ʵ� ���� ���� ���� ã��
//	classDLLNODE< CGameOBJ* > *pObjLIST;
//	classDLLNODE< CGameOBJ* > *pObjNODE;
	for(int i = 11; i < 14; i++)
	{
		// ���� �� Ȯ��.
		if(g_pZoneLIST->GetZONE(i)->Get_UserCNT() > 0)
		{
			//	��� ���� ���� ��Ų��.
			g_pZoneLIST->GetZONE(i)->All_User_Warp(0);
		}
	}
		
	return true;
}

////	Ŭ�� �ʵ� ���� ���� ��Ŵ
BOOL	GF_CF_End_User_Warp(classUSER *p_User,bool Type)
{
	//	���� ���� Ȯ��
	if(p_User == NULL) return false;
	//	���� ���� Ȯ��.
	if(g_CF_Play_N >= MAX_CF_SET_N) return false;
	if(g_CF_Set[g_CF_Play_N].m_NPC == NULL) return false;
	if (!( p_User->m_nZoneNO >= 11 && p_User->m_nZoneNO <= 13) ) return false;

	EnterCriticalSection(&g_CF_Play_LOCK);

	//	��ȯ ��ġ ����.
	int	t_Pos_N = rand()%5;

	if(Type)
	{
		//	���� ��Ŵ
		//	�ش� ���� ���� ���� ��ġ�� ����.
		p_User->Reward_WARP(g_CF_Set[g_CF_Play_N].m_ReturnNO[t_Pos_N],g_CF_Set[g_CF_Play_N].m_ReturnPos[t_Pos_N]);
		//	�ش� ���� �� ��ȣ ����.
		p_User->SetCur_TeamNO(2);
#ifdef __CLAN_WAR_SET
		p_User->Set_TeamNoFromClanIDX(0);	// �Ϲ� ����.
#endif
	}
	else
	{
		//	���� ��ġ�� ����.
		p_User->m_nZoneNO = g_CF_Set[g_CF_Play_N].m_ReturnNO[t_Pos_N];
		p_User->m_PosCUR = g_CF_Set[g_CF_Play_N].m_ReturnPos[t_Pos_N];
	}

	LeaveCriticalSection(&g_CF_Play_LOCK);
	return true;
}