
/*
	�ۼ��� �迵ȯ
*/

//#ifdef __CLAN_WAR_SET			// Ŭ���� �߰� ����

#include "GF_Clan_War.h"
#include "GF_CWar_Time.h"
/*
	�ۼ��� : �迵ȯ
	���� ����
*/

////	��ȭ ���� ����
BOOL	GF_User_Dlg_Create(HWND	p_Main_H);

////	��ȭ ���� �ݱ�
BOOL	GF_User_Dlg_Delete();

////	���� ���� ��ȭ����
LRESULT CALLBACK GF_User_Dlg_Proc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);

/*
	���� ���� ����,
*/
////	���� ���� ����Ʈ �ʱ�ȭ
BOOL	GF_User_List_Init();
////	���� ���� �߰�.
BOOL	GF_User_List_Add(LPVOID p_Add_User,DWORD Type);
////	���� ���� ����.
BOOL	GF_User_List_Del(LPVOID p_Del_User);

////	�ش� ������ Ŭ������ ��� �ϴ� �Լ�
BOOL	GF_User_Out_Clan(LPVOID p_User,BOOL p_Type);

/*
	����Ʈ �ڽ� ����
*/
////	���� ����Ʈ �ڽ� ����
BOOL	GF_User_List_View_Set(HWND hwndDlg);
////	���� ����Ʈ �߰�.
BOOL	GF_ULV_Add(DWORD p_Num);

////	Ŭ�� ����Ʈ �ڽ� ����
BOOL	GF_Clan_List_View_Set(HWND hwndDlg);
////	Ŭ�� ����Ʈ �߰�.
BOOL	GF_CLV_Add(DWORD p_Num);

////	Ŭ���� ����Ʈ �ڽ� ����
BOOL	GF_War_List_View_Set(HWND hwndDlg);
////	Ŭ�� ����Ʈ �߰�.
BOOL	GF_WLV_Add(DWORD p_Num);

////	����Ʈ ����Ʈ �ڽ� ����
BOOL	QL_QUEST_LIST(HWND hwndDlg);
////	����Ʈ ����Ʈ STB���� �о �߰��߰�
BOOL	QL_QUEST_LIST_INIT();

/*
	���������� Ŭ�� ����
*/
////	������ Ŭ�� ã��
BOOL	GF_Find_Clan(DWORD p_idx,BOOL Type,BOOL Sub_Type);

////	���� Ŭ�� �߰�.
BOOL	GF_Clan_List_Add(LPVOID p_Add_Clan,char *p_Name,DWORD p_Idx);

////	���� Ŭ�� ����.
BOOL	GF_Clan_List_Del(LPVOID p_Del_Clan);

////	���� Ŭ�� ����.
BOOL	GF_Clan_List_Del(DWORD	idx);

/*
	ini ���� �б� ���� �Լ�
*/
int		Get_ini_Read(char *p_File_Name,char *p_Title,char *p_Name);

//#endif