
/*
	작성자 김영환
*/

//#ifdef __CLAN_WAR_SET			// 클랜전 추가 정의

#include "GF_Clan_War.h"
#include "GF_CWar_Time.h"
/*
	작성자 : 김영환
	유저 관리
*/

////	대화 상자 생성
BOOL	GF_User_Dlg_Create(HWND	p_Main_H);

////	대화 상자 닫기
BOOL	GF_User_Dlg_Delete();

////	유저 관리 대화상자
LRESULT CALLBACK GF_User_Dlg_Proc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);

/*
	관리 버퍼 생성,
*/
////	접속 유저 리스트 초기화
BOOL	GF_User_List_Init();
////	접속 유저 추가.
BOOL	GF_User_List_Add(LPVOID p_Add_User,DWORD Type);
////	접속 유저 삭제.
BOOL	GF_User_List_Del(LPVOID p_Del_User);

////	해당 유저를 클랜에서 재거 하는 함수
BOOL	GF_User_Out_Clan(LPVOID p_User,BOOL p_Type);

/*
	리스트 박스 관리
*/
////	유저 리스트 박스 생성
BOOL	GF_User_List_View_Set(HWND hwndDlg);
////	유저 리스트 추가.
BOOL	GF_ULV_Add(DWORD p_Num);

////	클랜 리스트 박스 생성
BOOL	GF_Clan_List_View_Set(HWND hwndDlg);
////	클랜 리스트 추가.
BOOL	GF_CLV_Add(DWORD p_Num);

////	클랜전 리스트 박스 생성
BOOL	GF_War_List_View_Set(HWND hwndDlg);
////	클랜 리스트 추가.
BOOL	GF_WLV_Add(DWORD p_Num);

////	퀘스트 리스트 박스 생성
BOOL	QL_QUEST_LIST(HWND hwndDlg);
////	퀘스트 리스트 STB에서 읽어서 추가추가
BOOL	QL_QUEST_LIST_INIT();

/*
	접속유저의 클랜 관리
*/
////	유저의 클랜 찾기
BOOL	GF_Find_Clan(DWORD p_idx,BOOL Type,BOOL Sub_Type);

////	접속 클랜 추가.
BOOL	GF_Clan_List_Add(LPVOID p_Add_Clan,char *p_Name,DWORD p_Idx);

////	접속 클랜 삭제.
BOOL	GF_Clan_List_Del(LPVOID p_Del_Clan);

////	접속 클랜 삭제.
BOOL	GF_Clan_List_Del(DWORD	idx);

/*
	ini 파일 읽기 관련 함수
*/
int		Get_ini_Read(char *p_File_Name,char *p_Title,char *p_Name);

//#endif