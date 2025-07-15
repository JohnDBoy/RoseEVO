//#ifdef __CLAN_WAR_SET			// Ŭ���� �߰� ����

////	Ŭ���� ��û �� ��� DB ���� ó�� �Լ� �ʱ�ȭ
//	�ۼ��� : �迵ȯ

#include "GF_Define.h"
#include "GF_Data.h"


////	Ŭ���� ���� ó��
DWORD	WINAPI	GF_CWar_Thread_Proc(LPDWORD lpData)
{
	if(G_SQL_CWar_Thread == NULL)
		return (0);
	while(G_SQL_CWar_Thread->Ex_ID != 0)
	{
		G_SQL_CWar_Thread->Wait_Event(INFINITE);
		if(G_SQL_CWar_Thread->Ex_ID == 0) return (0);
		//	������ ��������
		if(G_SQL_CWar_Buffer != NULL)
		{
			if(G_SQL_CWar_Buffer->Output_Buffer(G_SQL_CWar_Buffer->m_Out_Buffer))
			{
				//	ó���Լ��� ����.
				GF_DB_CWar_Proc((GF_CWar_DB_ST *)G_SQL_CWar_Buffer->m_Out_Buffer);
				//	������ �ʱ�ȭ
				ZeroMemory(G_SQL_CWar_Buffer->m_Out_Buffer,G_SQL_CWar_Buffer->m_Buffer_Size);
				//	���� �ٽ� Ȯ��.
				SetEvent(G_SQL_CWar_Thread->Event_H);
			}
		}
	}
	return (0);
}

////	Ŭ���� ���.
BOOL	GF_DB_CWar_Insert(DWORD p_WarType,DWORD p_WarWin,__int64 p_Money,DWORD p_AClan,DWORD p_BClan, char* pComment)
{
	if(p_WarType == 0) return false;
	if(p_WarType > 4) return false;
	
	//	����
	EnterCriticalSection(&g_ClanWar_DBLOCK);

	//	������ ����ü ����
	GF_CWar_DB_ST	Temp_ST;
	ZeroMemory(&Temp_ST,sizeof(GF_CWar_DB_ST));

	//	������ �ֱ�
	Temp_ST.m_Server_ID = G_Server_ID;		// ���� ��ȣ
	Temp_ST.m_Channel_ID = G_SV_Channel;	// ä�� ��ȣ
	Temp_ST.m_WarType = p_WarType;
	Temp_ST.m_WarWin = p_WarWin;
	Temp_ST.m_Money = p_Money;
	Temp_ST.m_ATeam_ID = p_AClan;
	Temp_ST.m_BTeam_ID = p_BClan;
	Temp_ST.m_pComment = pComment;
	//	Ŭ�� ��ȣ�� �ش� �Ǵ� Ŭ���̸� ����.
	for(DWORD i = 0; i < g_Clan_Max_List; i++)
	{
		if(Temp_ST.m_ATeam_ID > 0)
		{
			//	Ŭ�� ��
			if(g_Clan_List[i].m_CLAN.m_dwClanID == Temp_ST.m_ATeam_ID)
			{
				memcpy(Temp_ST.m_ATeam_Name,g_Clan_List[i].m_ClanNAME,24);
			}
		}
		if(Temp_ST.m_BTeam_ID > 0)
		{
			//	Ŭ�� ��
			if(g_Clan_List[i].m_CLAN.m_dwClanID == Temp_ST.m_BTeam_ID)
			{
				memcpy(Temp_ST.m_BTeam_Name,g_Clan_List[i].m_ClanNAME,24);
			}
		}
	}

	//	���.
	GF_DB_CWar_Insert(&Temp_ST);
	
	LeaveCriticalSection(&g_ClanWar_DBLOCK);
	return true;
}

////	Ŭ���� ���...
BOOL	GF_DB_CWar_Insert(GF_CWar_DB_ST *p_St)
{
	//	���� �� ���ް�Ȯ��
	if(G_SQL_CWar_Buffer == NULL) return false;
	if(G_SQL_CWar_Thread == NULL) return false;
	if(G_SQL_CWar == NULL) return false;
	if(p_St == NULL) return false;

	//	���� ����.
	ZeroMemory(G_SQL_CWar_Buffer->m_In_Buffer,G_SQL_CWar_Buffer->m_Buffer_Size);
	memcpy(G_SQL_CWar_Buffer->m_In_Buffer,p_St,sizeof(GF_CWar_DB_ST));

	//	���ۿ� �ֱ�
	G_SQL_CWar_Buffer->Input_Buffer(G_SQL_CWar_Buffer->m_In_Buffer);
	//	ó�� �̺�Ʈ ȣ��
	SetEvent(G_SQL_CWar_Thread->Event_H);
	return true;
}

////	DB ó�� �Լ�
BOOL	GF_DB_CWar_Proc(GF_CWar_DB_ST *p_ST)
{
	//	���� ������ Ȯ��
	if(p_ST == NULL) return false;
	if(G_SQL_CWar == NULL) return false;

	//	������ ó��.

	//	DB Insert ó��
	char	Temp_S[1024];
	ZeroMemory(Temp_S,1024);

	//	DB �ڵ� ���
	HSTMT	Temp_HSTMT = NULL;
	G_SQL_CWar->Alloc(&Temp_HSTMT);
	if(Temp_HSTMT == NULL) return false;

	//	Insert ���� �ۼ��ϱ�
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

	//	�ش� ������ ����.
	G_SQL_CWar->Exec(Temp_S,Temp_HSTMT);
	//	�Ҵ�����.
	G_SQL_CWar->Free(Temp_HSTMT);

	return true;
}

//#endif