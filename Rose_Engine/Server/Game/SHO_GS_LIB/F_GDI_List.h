
/*
	�ۼ��� �迵ȯ
*/
//	����Ʈ �ڽ� ����
HWND	FL_Create_List(HWND p_Main_H,char *p_Title,DWORD Xx,DWORD Yy,DWORD Xw,DWORD Yh);

//	����Ʈ �ڽ��� �÷� �߰�
BOOL	FL_Set_List_Col(HWND p_List_H,int p_Col_N,int p_Col_Size,char *p_Text);

//	����Ʈ �ڽ��� ������ �߰��Ѵ�.
BOOL	FL_Set_List_Data(HWND p_List_H,int p_Row_N,int p_Col_N,char *p_Text);


//	��ư ����
HWND	FL_Create_Button(HWND p_Main_H,char *p_Title,DWORD Xx,DWORD Yy,DWORD Xw,DWORD Yh);

//	��ư ����
BOOL	FL_Delete_Button(HWND p_Button_H);

//	��ư �̺�Ʈ Ȯ��
BOOL	FL_Command_Button(HWND p_Button_H,LPARAM Lparam);
