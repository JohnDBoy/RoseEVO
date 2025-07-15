/*
	리스트 뷰 컨트롤 소스 : 작성자 : 김영환
*/

#include <windows.h>
#include <commctrl.h>
#include "F_GDI_List.h"

//	리스트 박스 생성
HWND	FL_Create_List(HWND p_Main_H,char *p_Title,DWORD Xx,DWORD Yy,DWORD Xw,DWORD Yh)
{
	HWND	Return_H = NULL; 
	//	리스트 박스 생성  LVS_OWNERDATA WS_BORDER | WS_THICKFRAME
	Return_H = CreateWindow(WC_LISTVIEW,p_Title,WS_CAPTION  | WS_VISIBLE | WS_BORDER | WS_THICKFRAME | LVS_REPORT ,Xx,Yy,Xw,Yh,p_Main_H,NULL,NULL,NULL);
	return Return_H;
}

//	리스트 박스에 컬럼 추가
BOOL	FL_Set_List_Col(HWND p_List_H,int p_Col_N,int p_Col_Size,char *p_Text)
{
	if(p_List_H == NULL) return false;
	LVCOLUMN	t_Col;
	t_Col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	t_Col.fmt = LVCFMT_LEFT;
	t_Col.iSubItem = p_Col_N;	// 서브 번호
	t_Col.cx = p_Col_Size;		// 버퍼 크기
	t_Col.pszText = p_Text;		// 문자열
	SendMessage(p_List_H,LVM_INSERTCOLUMN,p_Col_N,(LPARAM)&t_Col);
	return true;
}

//	리스트 박스에 데이터 추가한다.
BOOL	FL_Set_List_Data(HWND p_List_H,int p_Row_N,int p_Col_N,char *p_Text)
{
	if(p_List_H == NULL) return false;
	if(p_Row_N < ListView_GetItemCount(p_List_H))
	{
		// 변경
		ListView_SetItemText(p_List_H,p_Row_N,p_Col_N,p_Text);
	}
	else
	{
		LVITEM	t_Item;
		t_Item.mask = LVIF_TEXT | LVIF_IMAGE;
		t_Item.state = 0;
		t_Item.stateMask = 0;
		t_Item.iImage = 0;
		t_Item.iSubItem = p_Col_N;
		t_Item.iItem = p_Row_N;
		t_Item.pszText = p_Text;
		// 추가
		SendMessage(p_List_H,LVM_INSERTITEM,0,(LPARAM)&t_Item);
	}
	return true;
}


//	버튼 생성
HWND	FL_Create_Button(HWND p_Main_H,char *p_Title,DWORD Xx,DWORD Yy,DWORD Xw,DWORD Yh)
{
	HWND	Return_H = NULL; 
	//	버튼 생성 
	Return_H = CreateWindow(WC_BUTTON,p_Title,BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE ,Xx,Yy,Xw,Yh,p_Main_H,NULL,NULL,NULL);
	return Return_H;
}

//	버튼 삭제
BOOL	FL_Delete_Button(HWND p_Button_H)
{
	if(p_Button_H != NULL)
	{
		DestroyWindow(p_Button_H);
	}
	return true;
}

//	버튼 이벤트 확인
BOOL	FL_Command_Button(HWND p_Button_H,LPARAM Lparam)
{
	if(p_Button_H == NULL) return false;
	if(p_Button_H == (HWND)Lparam) return true;
	return false;
}