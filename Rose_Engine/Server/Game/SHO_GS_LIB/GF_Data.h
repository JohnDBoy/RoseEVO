/*
	�ۼ��� �迵ȯ
*/

//#ifdef __CLAN_WAR_SET			// Ŭ���� �߰� ����

//	���� ��ȭ ����
extern	HWND	g_hUserWND;
extern	char	G_Main_Dir[MAX_PATH];	// ���α׷� ��ġ ����
//	���� ���� ���� ����
extern	HWND	g_hList_User;
//	���� Ŭ�� ���� ����
extern	HWND	g_hList_Clan;
//	���� Ŭ���� ���� ����
extern	HWND	g_hList_CWar;
// �ε� ����Ʈ ����Ʈ
extern	HWND	g_hList_Quest;

//	ũ��Ƽ�� ���� ó��
extern CRITICAL_SECTION	g_ClanWar_LOCK;
extern CRITICAL_SECTION	g_ClanWar_DBLOCK;

//	������ ���� ����Ʈ
extern	classUSER	*g_User_List[DF_MAX_USER_LIST];
//	���� Ŭ�� ����Ʈ
extern	GF_Clan_Info	*g_Clan_List;
extern	DWORD		g_Clan_Max_List;

////	Ŭ���� ����Ʈ
//	��Ȱ ����Ʈ.
extern tPOINTF g_APosREVIVAL;	// �� A�� ��Ȱ ��ġ
extern tPOINTF g_BPosREVIVAL;	// �� B�� ��Ȱ ��ġ
extern	GF_ClanWar_Set	g_ClanWar_List[DF_MAX_CWAR_LIST];
extern	GF_World_Pos	g_ClanWar_ReCall;	// Ŭ���� ���� ��ġ

////	Ŭ���� ���� ���� ����
extern	GF_ClanWar_MOB	g_CW_MOB_List[DF_MAX_CWAR_TYPE][DF_MAX_CWAR_MOB][DF_CWAR_TEAM_N];

////	Ŭ���� ���� ������ ����
extern	GF_CWar_Win_Gift	g_CW_Win_Gift[DF_MAX_CWAR_TYPE];

////	���� ����Ʈ
extern CUserLIST		*g_pUserLIST;
////	�� ����Ʈ
extern CZoneLIST		*g_pZoneLIST;


/*
	Ŭ���� DB ó�� �Լ�
*/
extern DWORD		G_Server_ID;		// ���� ID
extern DWORD		G_SV_Channel;		// ä�� ID
extern KDB_SQL		*G_SQL_CWar;		// Ŭ���� DB ����
extern F_Thread		*G_SQL_CWar_Thread;	// Ŭ���� DB ó�� ������
extern F_Buffer		*G_SQL_CWar_Buffer;	// Ŭ���� DB ó�� ����

/*
	Ŭ���� ���� ����
*/
extern __int64		G_CW_BAT_MONEY[3];	// Ŭ���� ���� �ݾ�
extern DWORD	G_CW_MAX_LEVEL[3];		// Ŭ���� LV ó��
extern DWORD	G_CW_MAX_USER_NO[3][2];	// Ŭ���� �ִ� ���� �ο�
extern DWORD	DF_CWAR_PLAY_TIME;	//48//120		// 20��
extern DWORD	DF_CWAR_END_TIME;	//3 //12		// 2��
extern DWORD	DF_CWAR_WAIT_TIME;		//12		// 2�� ���
//#endif