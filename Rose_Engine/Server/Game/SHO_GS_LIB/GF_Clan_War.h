
/*
	�ۼ��� �迵ȯ
*/

//#ifdef __CLAN_WAR_SET			// Ŭ���� �߰� ����

/*
	Ŭ���� ����/ó�� �Լ�
*/

////	Ŭ���� ���� �ʱ�ȭ
BOOL	GF_Clan_War_Init();

////	Ŭ���� ���� ini �б�
BOOL	GF_Clan_War_Ini_Read();

////	Ŭ���� �ʵ� ���� �� �ʱ�ȭ
BOOL	GF_CW_MOB_Init();

////	Ŭ���� �ʵ� Ȯ��
DWORD	GF_Clan_War_Zone_Is(DWORD p_Zone_N);

////	Ŭ���� �ʵ� �� Ȯ��.
DWORD	GF_Clan_War_Zone_Is(DWORD p_Zone_N,DWORD p_Clan_N);

////	Ŭ���� �� �������� Ȯ�� [2�� Ŭ���� ����]
BOOL	GF_Clan_War_Zone_Play_Is(DWORD p_Zone_N);

////	Ŭ���� ������� �� ã��
DWORD	GF_Find_CWar_List(LPVOID p_Npc,DWORD p_Type);

////	Ŭ���� ������� �� ã�� : �ش� �濡 �̱� �� ����. : ������.
DWORD	GF_Find_CWar_List2(DWORD p_Clan_ID);

////	Ŭ���� ���� Ÿ�� ���..
DWORD	GF_Get_CWar_Type(DWORD	p_Type);

////	Ŭ���� ���� ���� �ݾ� ����
__int64 GF_Set_CWar_Bat(DWORD p_Clan_Inx,DWORD p_Type);

////	Ŭ���� ��û ��ҿ� ���� ���� ó��...
BOOL	GF_CWar_Bat_Return(LPVOID p_User);

////	������ ������ �ݾ��� ����.
BOOL	GF_Clan_Master_Money_Sub(DWORD p_Clan_Inx,__int64 p_Money);

////	������ ������ �ݾ��� �߰�.
BOOL	GF_Clan_Master_Money_Add(DWORD p_Clan_Inx,__int64 p_Money);

////	������ ������ Ŭ���� ���� ����.
BOOL	GF_Clan_Master_ClanWar_IDX(DWORD p_Clan_Inx,WORD Num,bool b_WinType,DWORD p_WarType);

////	���� ������ Ŭ���� ���󺯼� Ȯ��
BOOL	GF_Clan_Master_ClanWar_Gift(LPVOID p_User);

////	�¸��� Ŭ���� ó��
BOOL	GF_Win_CWar_Set(LPVOID p_Room,DWORD p_Type,bool b_WinType);

////	Ŭ���� ��� ó��
BOOL	GF_Clan_War_Drop(DWORD p_List_N,DWORD p_Team);

////	Ŭ���� ��� ã��
DWORD	GF_Find_CWar_Void(LPVOID p_Npc);

////	Ŭ���� �� �����
BOOL	GF_Del_CWar_List(LPVOID p_Npc);
BOOL	GF_Del_CWar_List2(DWORD	p_Num);

////	Ŭ������ ��û ��� ó��
BOOL	GF_Clan_War_Cancel(DWORD	p_Num);

////	Ŭ���� ��û ó��
BOOL	GF_Clan_War_Input(LPVOID p_Npc,DWORD p_Clan_Inx,DWORD p_LV, DWORD p_Type, LPVOID pClanUser);

////	Ŭ���� ���� LV Ȯ��.
BOOL	GF_Clan_War_Check_LV(DWORD p_LV,DWORD p_Type);

////	Ŭ�� ����Ʈ���� ã��
DWORD	GF_Clan_Fine_List(DWORD p_Clan_ID);

////	Ŭ���� �����͸� ã�´�.
DWORD	GF_Clan_Master_Fine(DWORD p_Clan);

////	Ŭ�� ������ ���� ������ ã�´�.
DWORD	GF_Clan_Master_Sub_Fine(DWORD p_Clan);

////	Ŭ���� ��� Ŭ�� Ȯ��
BOOL	GF_ClanWar_Check_myClan(DWORD p_List_N,DWORD p_Clan_N);

////	Ŭ�� �Ҽ� ���� �������� ���� Ȯ�� �޽��� ó��
BOOL	GF_ClanWar_Join_OK_Send(DWORD p_List_N,DWORD p_Type,DWORD p_Sub_Type);



////	Ŭ���� �������� ���� ���� ��Ŵ
BOOL	GF_ClanWar_Move(LPVOID p_User,DWORD p_List_N,DWORD p_Team_N);

////	Ŭ���� ���� ��ġ�� ����
BOOL	GF_ClanWar_Out_Move(LPVOID p_User);

////	Ŭ���� ������ �� ���� �ϱ�
BOOL	GF_ClanWar_Mob_Set(DWORD p_List_N);

////	Ŭ���� ������ �� �����
BOOL	GF_ClanWar_Mob_Del(DWORD p_List_N,DWORD p_Type);
BOOL	GF_ClanWar_Mob_Del2(LPVOID p_Room,DWORD p_Type);

////	�ش� ���� ���� ü���� ȸ���Ѵ�.
BOOL	GF_ClanWar_MOB_HP_ReSet(LPVOID p_Room,DWORD p_Type);

////	Ŭ���� ������ ���� �޽��� ó�� [�̰� Ʈ���� �������� ó���ϴ� ���]
BOOL	GF_ClanWar_Boss_MOB_Kill(DWORD List_N,DWORD p_idx);

////	Ŭ���� �������� �׾����� üũ [2�� Ŭ���� ���� : �������� ���� �׾����� üũ]
BOOL	GF_ClanWar_Boss_Kill_Check(LPVOID p_Boss,LPVOID p_User);

////	HP ������ ���� Ŭ���� ���� ������ Ȯ���Ѵ�. [2�� Ŭ���� ����]
DWORD	GF_ClanWar_Boss_MOB_Is(LPVOID p_Boss);

////	Ŭ���� ������ HP ���� ����.	 [2�� Ŭ���� ����]
BOOL	GF_ClanWar_Boss_MOB_HP(DWORD p_List_N);

////	Ŭ���� ����Ʈ ����
BOOL	GF_ClanWar_Quest_Del(LPVOID p_User,DWORD p_Quest_Idx);

////	Ŭ���� ����Ʈ ���
BOOL	GF_ClanWar_Quest_Add(LPVOID p_User,DWORD p_Quest_Idx);

////	Ŭ���� PK ��� ���� Ȯ��
BOOL	GF_ClanWar_PK_Is(LPVOID p_User);

////	Ŭ���� ���� ���� ����Ʈ ó�� �Լ�
BOOL	GF_ClanWar_Quest_DropProc(DWORD p_List_N,DWORD p_Team);

////	Ŭ���� ���� ó�� �޽���
BOOL	GF_ClanWar_GiveUP(DWORD p_List_N,DWORD p_Team);

////	Ŭ���� �������� Ŭ�� Ȯ�� �Լ�
DWORD	GF_Get_CLANWAR_Check(DWORD p_Clan_N,DWORD p_User_LV);

////	Ŭ���� ���� ������ ���� �޽��� �����Ѵ�.
BOOL	GF_Send_CLANWAR_Progress(DWORD p_List_N,DWORD p_Type);


////	Ŭ���� ��Ȱ ��ġ ���
BOOL	GF_Get_ClanWar_Revival_Pos(DWORD p_List_N,DWORD pTeam,float &Out_x,float &Out_Y);

////	Ŭ���� �ʿ� GATE NPC ���� ó��
BOOL	GF_Set_ClanWar_Gate_NPC(LPVOID p_User,DWORD p_List_N,BOOL p_Type);

////	Ŭ���� ���� �ο� ����
BOOL	GF_Add_ClanWar_Number(DWORD p_List_N,DWORD pTeam);

////	Ŭ���� ���� �ο� ����
BOOL	GF_Sub_ClanWar_Number(DWORD p_List_N,DWORD pTeam);

////	Ŭ���� ����Ʈ ó�� �κ�
BOOL	GF_Set_ClanWar_RankPoint(DWORD p_Type,LPVOID p_User_A,LPVOID p_User_B, void* vp_ClanWar_Set );//DWORD	WarType);

////	Ŭ�� ����Ʈ �������� ���
float	GF_Get_Rank_Rate(int Ateam_P,int BTeam_P);

// (20070104:����ö) : Ŭ�� ���� ����Ʈ ����
//BOOL	GF_Add_ClanWar_RewardPoint( int iClanID, int iRewardPoint );


//#endif