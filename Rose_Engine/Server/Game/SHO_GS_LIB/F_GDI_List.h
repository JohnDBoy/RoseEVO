
/*
	작성자 김영환
*/
//	리스트 박스 생성
HWND	FL_Create_List(HWND p_Main_H,char *p_Title,DWORD Xx,DWORD Yy,DWORD Xw,DWORD Yh);

//	리스트 박스에 컬럼 추가
BOOL	FL_Set_List_Col(HWND p_List_H,int p_Col_N,int p_Col_Size,char *p_Text);

//	리스트 박스에 데이터 추가한다.
BOOL	FL_Set_List_Data(HWND p_List_H,int p_Row_N,int p_Col_N,char *p_Text);


//	버튼 생성
HWND	FL_Create_Button(HWND p_Main_H,char *p_Title,DWORD Xx,DWORD Yy,DWORD Xw,DWORD Yh);

//	버튼 삭제
BOOL	FL_Delete_Button(HWND p_Button_H);

//	버튼 이벤트 확인
BOOL	FL_Command_Button(HWND p_Button_H,LPARAM Lparam);
