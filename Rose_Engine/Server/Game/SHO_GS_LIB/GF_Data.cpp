
/*
	�ۼ��� �迵ȯ
*/

//#ifdef __CLAN_WAR_SET			// Ŭ���� �߰� ����


#include "GF_Define.h"


//	���� ��ȭ ����
HWND	g_hUserWND = NULL;
char	G_Main_Dir[MAX_PATH];	// ���α׷� ��ġ ����
//	���� ���� ���� ����
HWND	g_hList_User = NULL;
//	���� Ŭ�� ���� ����
HWND	g_hList_Clan = NULL;
//	���� Ŭ���� ���� ����
HWND	g_hList_CWar = NULL;
//	�ε� ����Ʈ ����Ʈ
HWND	g_hList_Quest = NULL;

//	ũ��Ƽ�� ���� ó��
CRITICAL_SECTION	g_ClanWar_LOCK;
CRITICAL_SECTION	g_ClanWar_DBLOCK;

//	������ ���� ����Ʈ
classUSER	*g_User_List[DF_MAX_USER_LIST];
//	���� Ŭ�� ����Ʈ
GF_Clan_Info	*g_Clan_List;
DWORD			g_Clan_Max_List = 0;

////	Ŭ���� ����Ʈ
tPOINTF g_APosREVIVAL;	// �� A�� ��Ȱ ��ġ
tPOINTF g_BPosREVIVAL;	// �� B�� ��Ȱ ��ġ
GF_ClanWar_Set	g_ClanWar_List[DF_MAX_CWAR_LIST];
GF_World_Pos	g_ClanWar_ReCall;	// Ŭ���� ���� ��ġ

////	Ŭ���� ���� ���� ����
GF_ClanWar_MOB	g_CW_MOB_List[DF_MAX_CWAR_TYPE][DF_MAX_CWAR_MOB][DF_CWAR_TEAM_N];

////	Ŭ���� ���� ������ ����
GF_CWar_Win_Gift	g_CW_Win_Gift[DF_MAX_CWAR_TYPE];

/*
	Ŭ���� DB ó�� �Լ�
*/
DWORD		G_Server_ID = 0;			// ���� ID
DWORD		G_SV_Channel = 1;			// ä�� ID
KDB_SQL		*G_SQL_CWar = NULL;			// Ŭ���� DB ����
F_Thread	*G_SQL_CWar_Thread = NULL;	// Ŭ���� DB ó�� ������
F_Buffer	*G_SQL_CWar_Buffer = NULL;	// Ŭ���� DB ó�� ����

/*
	Ŭ���� ���� ����
*/
__int64		G_CW_BAT_MONEY[4];	// Ŭ���� ���� �ݾ�
DWORD	G_CW_MAX_LEVEL[4];		// Ŭ���� LV ó��
DWORD	G_CW_MAX_USER_NO[4][2];	// Ŭ���� �ִ� ���� �ο�
DWORD	DF_CWAR_PLAY_TIME = 120;	//120		// 20��
DWORD	DF_CWAR_END_TIME = 12;		//12		// 2��
DWORD	DF_CWAR_WAIT_TIME = 12;		//12		// 2�� ���
//#endif