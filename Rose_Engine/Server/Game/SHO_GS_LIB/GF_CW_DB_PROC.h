//#ifdef __CLAN_WAR_SET			// Ŭ���� �߰� ����
////	Ŭ���� ��û �� ��� DB ���� ó�� �Լ�
//	�ۼ��� : �迵ȯ

////	Ŭ���� ���� ó��
DWORD	WINAPI	GF_CWar_Thread_Proc(LPDWORD lpData);

////	Ŭ���� ���.
BOOL	GF_DB_CWar_Insert(DWORD p_WarType,DWORD p_WarWin,__int64 p_Money,DWORD p_AClan,DWORD p_BClan, char* pComment);

////	Ŭ���� ���...
BOOL	GF_DB_CWar_Insert(GF_CWar_DB_ST *p_St);

////	DB ó�� �Լ�
BOOL	GF_DB_CWar_Proc(GF_CWar_DB_ST *p_ST);

//#endif