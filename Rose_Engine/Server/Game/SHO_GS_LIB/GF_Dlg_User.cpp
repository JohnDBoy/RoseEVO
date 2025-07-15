
/*
	작성자 김영환
*/

//#ifdef __CLAN_WAR_SET			// 클랜전 추가 정의


#include "GF_Define.h"
#include "F_GDI_List.h"
#include "GF_Data.h"
#include "ClanWar.h"

////	클랜 필드 오픈 정보 보기
extern HWND	g_hList_CField;

////	버튼 핸들
HWND	g_Tool_BT1 = NULL;
HWND	g_Tool_BT2 = NULL;
HWND	g_Tool_BT3 = NULL;
HWND	g_Tool_BT4 = NULL;
HWND	g_Tool_BT5 = NULL;
HWND	g_Tool_BT6 = NULL;
////	상태변수
BOOL	g_Tool_TF1 = true;
BOOL	g_Tool_TF2 = true;
BOOL	g_Tool_TF3 = true;
BOOL	g_Tool_TF4 = true;
BOOL	g_Tool_TF6 = true;

////	대화 상자 생성
BOOL	GF_User_Dlg_Create(HWND	p_Main_H)
{
	if(g_hUserWND != NULL) return false;
	WNDCLASS	wc; // 윈도우 클래스 구조체 선언
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS | CS_CLASSDC;
	wc.lpfnWndProc = GF_User_Dlg_Proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(DWORD);
	wc.hInstance = NULL;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("USER_DLG1");
	// 윈도우 클래스 등록
	if(!RegisterClass(&wc))	return false;

	//  윈도 생성
	g_hUserWND = CreateWindow("USER_DLG1","Game Server Toolbar",WS_BORDER | WS_THICKFRAME,
							  0,0,200,170,NULL,NULL,NULL,NULL);
	if(!g_hUserWND)	return false;

#ifdef __CLAN_WAR_EXT
	//	크리티컬 세션 생성
	InitializeCriticalSection( &g_ClanWar_LOCK );
	InitializeCriticalSection( &g_ClanWar_DBLOCK );
#endif

	//  윈도우 그리기
	ShowWindow(g_hUserWND,SW_SHOWNORMAL);

	//	경로.
	ZeroMemory(G_Main_Dir,MAX_PATH);
	GetCurrentDirectory(MAX_PATH,G_Main_Dir);

	// 유저 리스트 초기화
	if( !GF_User_List_Init() )
		MessageBox( NULL, "클랜전 초기화 실패!", "ERROR", MB_OK );

	//	뷰 리스트 생성
	GF_User_List_View_Set(NULL);
	GF_Clan_List_View_Set(NULL);
	GF_War_List_View_Set(NULL);
	QL_QUEST_LIST(NULL);		// (20061121:남병철) : 퀘스트 ID 리스트

	//	DB 연결.
	if(!GF_Connect_DB_CW(p_Main_H))
		MessageBox(NULL,"클랜전 DB 연결 실패! 확인 바람...","뭐냐고??!!",MB_OK);

	////	기본 데이터 넣기
	//for(DWORD i = 0 ; i < 100; i++)
	//	GF_ULV_Add(i);
	return true;
}

////	닫기
BOOL	GF_User_Dlg_Delete()
{
	//	핸들 확인
	if(g_hUserWND == NULL) return false;

	//	클랜전 DB 연결 해지
	GF_DisConnect_DB_CW();

	//	메모리 삭제
	if(g_Clan_List != NULL)	
	{
		VirtualFree(g_Clan_List,0,MEM_DECOMMIT);
		g_Clan_List = NULL;
	}

	//	자식들 지운다.
	DestroyWindow(g_hList_CWar);
	DestroyWindow(g_hList_User);
	DestroyWindow(g_hList_Clan);
	DestroyWindow(g_hList_Quest);

	// 윈도우 종료
	DestroyWindow(g_hUserWND);
	g_hUserWND = NULL;

#ifdef __CLAN_WAR_EXT
	//	크리티컬 세션 삭제
	DeleteCriticalSection(&g_ClanWar_LOCK);
	DeleteCriticalSection(&g_ClanWar_DBLOCK);
#endif

	return true;
}

////	유저 관리 대화상자
LRESULT CALLBACK GF_User_Dlg_Proc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    switch(uMsg)
	{
	case WM_CREATE:
		{
			////	종속 버튼 생성
			g_Tool_BT1 = FL_Create_Button(hwndDlg,"User View",0,0,192,18);
			g_Tool_BT2 = FL_Create_Button(hwndDlg,"Clan View",0,20,192,18);
			g_Tool_BT3 = FL_Create_Button(hwndDlg,"Clan War View",0,40,192,18);
			g_Tool_BT4 = FL_Create_Button(hwndDlg,"Clan Field View",0,60,192,18);
			g_Tool_BT5 = FL_Create_Button(hwndDlg,"Clan Field ini ReLoad",0,80,192,18);
			g_Tool_BT6 = FL_Create_Button(hwndDlg,"Quest List View",0,100,192,18);
			break;
		}
	case WM_PAINT:
		{
			break;
		}
	case WM_CLOSE:
		{
			//	버튼 삭제
			FL_Delete_Button(g_Tool_BT1);
			FL_Delete_Button(g_Tool_BT2);
			FL_Delete_Button(g_Tool_BT3);
			FL_Delete_Button(g_Tool_BT4);
			FL_Delete_Button(g_Tool_BT5);
			FL_Delete_Button(g_Tool_BT6);
			DestroyWindow(hwndDlg);
			break;
		}
	case WM_DESTROY:
		{
			//	버튼 삭제
			FL_Delete_Button(g_Tool_BT1);
			FL_Delete_Button(g_Tool_BT2);
			FL_Delete_Button(g_Tool_BT3);
			FL_Delete_Button(g_Tool_BT4);
			FL_Delete_Button(g_Tool_BT5);
			FL_Delete_Button(g_Tool_BT6);
			//	대화 상자 닫기.
			g_hUserWND = NULL;
			break;
		}
	case WM_COMMAND:
		{
			//	버튼 이벤트 처리
			if(FL_Command_Button(g_Tool_BT1,lParam))
			{
				// 유저 보기 활성화 비활성화
				g_Tool_TF1 = !g_Tool_TF1;
				if(g_Tool_TF1)
				{
					ShowWindow(g_hList_User,SW_HIDE);
				}
				else
				{
					ShowWindow(g_hList_User,SW_SHOWNORMAL);
				}
			}
			if(FL_Command_Button(g_Tool_BT2,lParam))
			{
				// 클랜 보기 활성화 비활성화
				g_Tool_TF2 = !g_Tool_TF2;
				if(g_Tool_TF2)
				{
					ShowWindow(g_hList_Clan,SW_HIDE);
				}
				else
				{
					ShowWindow(g_hList_Clan,SW_SHOWNORMAL);
				}
			}
			if(FL_Command_Button(g_Tool_BT3,lParam))
			{
				// 클랜전 보기 활성화 비활성화
				g_Tool_TF3 = !g_Tool_TF3;
				if(g_Tool_TF3)
				{
					ShowWindow(g_hList_CWar,SW_HIDE);
				}
				else
				{
					ShowWindow(g_hList_CWar,SW_SHOWNORMAL);
				}
			}
			if(FL_Command_Button(g_Tool_BT4,lParam))
			{
				// 클랜 필드 보기 활성화 비활성화
				g_Tool_TF4 = !g_Tool_TF4;
				if(g_Tool_TF4)
				{
					ShowWindow(g_hList_CField,SW_HIDE);
				}
				else
				{
					ShowWindow(g_hList_CField,SW_SHOWNORMAL);
				}
			}
			if(FL_Command_Button(g_Tool_BT5,lParam))
			{
				// 클랜 필드 다시 읽기
				if(IDOK == MessageBox(hwndDlg," -SHO_GS_CField.ini- File ReLoad...?","Clan Field Event Setting",MB_OKCANCEL))
				{
					if( !GF_CF_Set_INI_Load() )
						MessageBox( NULL, "클필드 초기화 실패!", "ERROR", MB_OK );
				}
			}
			if(FL_Command_Button(g_Tool_BT6,lParam))
			{
				// 클랜 필드 보기 활성화 비활성화
				g_Tool_TF6 = !g_Tool_TF6;
				if(g_Tool_TF6)
				{
					ShowWindow(g_hList_Quest,SW_HIDE);
				}
				else
				{
					ShowWindow(g_hList_Quest,SW_SHOWNORMAL);
				}
			}
			break;
		}
	case WM_KEYDOWN:
		{
			switch(LOWORD(wParam))
			{
			case VK_F2:
				{
					////	테스트
					//GF_CF_Set_INI_Load();
					if(g_User_List[0] != NULL)
						g_User_List[0]->SetClanRank(25);
					break;
				}
			case VK_F3:
				{
					if(g_User_List[0] != NULL)
						g_User_List[0]->SetClanRank(-25);
					////	퀘스트 정보 다시 전송.
					//g_User_List[0]->Quest_SetClanWarVAR(0,0);
					//g_User_List[0]->Send_gsv_Quest_VAR();
					break;
				}
			case VK_F4:
				{
					break;
				}
			case VK_F5:
				{
					break;
				}
			case VK_F6:
				{
					break;
				}
			case VK_F7:
				{
					break;
				}
			case VK_F8:
				{
					break;
				}
			case VK_F9:
				{
					break;
				}
			}
			break;
		}
	case WM_CHAR:
		{
			break;
		}
	default :
		return DefWindowProc(hwndDlg,uMsg,wParam,lParam);
	}
	return DefWindowProc(hwndDlg,uMsg,wParam,lParam);
}



/*
	관리 버퍼 생성,
*/
////	접속 유저 리스트 초기화
BOOL	GF_User_List_Init()
{

#ifdef __CLAN_WAR_EXT

	// 클랜전 초기화
	if( CClanWar::GetInstance()->Init() == FALSE )
		return FALSE;

	// (20070424:남병철) : 치트키 호환성을 위해 기존의 유저 리스트 살림
	// Ex) send1, send2, send3
	//	유저 리스트 초기화
	ZeroMemory( g_User_List, DF_MAX_USER_LIST * 4 );

#else

	//	유저 리스트 초기화
	ZeroMemory(g_User_List,DF_MAX_USER_LIST*4);

	// 클랜전 초기화
	if( !GF_Clan_War_Init() )
		return FALSE;

	//	클랜 리스트 초기화
	if(g_Clan_List == NULL)
	{
		g_Clan_List = (GF_Clan_Info *)VirtualAlloc(NULL,DF_MAX_CLAN_LIST*sizeof(GF_Clan_Info),MEM_COMMIT | MEM_RESERVE,PAGE_READWRITE);
		if(g_Clan_List == NULL)	return false;
		ZeroMemory(g_Clan_List,DF_MAX_CLAN_LIST*sizeof(GF_Clan_Info));
	}
	g_Clan_Max_List = 0;

#endif

	return TRUE;
}

////	접속 유저 추가.
BOOL	GF_User_List_Add(LPVOID p_Add_User,DWORD Type)
{
	if(p_Add_User == NULL) return false;
	classUSER	*Temp_User;
	Temp_User = (classUSER *)p_Add_User;
	
	switch(Type)
	{
	case 0:
		{
			//	유저 추가.
			for(DWORD i = 0 ; i < DF_MAX_USER_LIST; i++)
			{
				if(g_User_List[i] == NULL)
				{
					g_User_List[i] = Temp_User;
					// (20070424:남병철) : 치트키에서 유저 리스트를 사용하므로 아래의 클랜 정보는 막음
					//GF_Find_Clan(i,true,false);
					//	뷰리스트에 추가
					GF_ULV_Add(i);
					return true;
				}
			}
			break;
		}
	case 1:
		{
			//	기존 접속 유저의 업데이트 확인하기
			for(DWORD i = 0 ; i < DF_MAX_USER_LIST; i++)
			{
				//	동일 메모리 포인터.
				if(g_User_List[i] == Temp_User)
				{
					//	유저 이름 확인.
					if(0 == lstrcmp(g_User_List[i]->Get_NAME(),Temp_User->Get_NAME()))
					{
						g_User_List[i] = Temp_User;
						//GF_Find_Clan(i,true,false);
						//	뷰리스트에 추가
						GF_ULV_Add(i);
						return true;
					}
					else
					{
						// 메모리 포인터는 같은데. 이름이 다른 경우.. [오류에 해당. 이전 유저 정보 확인한다. 있을수 없다.]
						// 변경 한다.
						g_User_List[i] = Temp_User;
						//GF_Find_Clan(i,true,false);
						//	뷰리스트에 추가
						GF_ULV_Add(i);
						return true;
					}
				}
			}
			break;
		}
	case 2:
		{
			//	기존 접속 유저의 클랜 삭제 확인하기
			for(DWORD i = 0 ; i < DF_MAX_USER_LIST; i++)
			{
				//	동일 메모리 포인터.
				if(g_User_List[i] == Temp_User)
				{
					//	유저 이름 확인.
					if(0 == lstrcmp(g_User_List[i]->Get_NAME(),Temp_User->Get_NAME()))
					{
						g_User_List[i] = Temp_User;
						//if(Temp_User->Is_ClanMASTER())
						//	GF_Find_Clan(i,false,true);
						//else
						//	GF_Find_Clan(i,false,false);
						//	뷰리스트에 추가
						GF_ULV_Add(i);
						return true;
					}
					else
					{
						// 메모리 포인터는 같은데. 이름이 다른 경우.. [오류에 해당. 이전 유저 정보 확인한다. 있을수 없다.]
						// 변경 한다.
						g_User_List[i] = Temp_User;
						//if(Temp_User->Is_ClanMASTER())
						//	GF_Find_Clan(i,false,true);
						//else
						//	GF_Find_Clan(i,false,false);
						//	뷰리스트에 추가
						GF_ULV_Add(i);
						return true;
					}
				}
			}
			break;
		}
	}
	return false;
}



////	접속 유저 삭제.
BOOL	GF_User_List_Del(LPVOID p_Del_User)
{
	if(p_Del_User == NULL) return false;
	classUSER	*Temp_User;
	Temp_User = (classUSER *)p_Del_User;

	//	클랜전 존에 있었다면 인원 감소
	//if(Temp_User->GetZONE() != NULL)
	//{
	//	DWORD	Temp_Zone_N = GF_Clan_War_Zone_Is(Temp_User->GetZONE()->Get_ZoneNO());
	//	//	클랜전 존 확인
	//	if(Temp_Zone_N > 0)
	//	{
	//		GF_Sub_ClanWar_Number(Temp_Zone_N,Temp_User->Get_TeamNO());
	//	}
	//}


	//	제거
	for(DWORD i = 0 ; i < DF_MAX_USER_LIST; i++)
	{
		//	동일 유저 포인터 찾기.
		if(g_User_List[i] == Temp_User)
		{
			//	해당 유저의 클랜 처리
			//GF_Find_Clan(i,false,false);
			//	삭제 부분에서 동일 유저인지 확인 하는 기능 있어야 함
			g_User_List[i] = NULL;
			//	뷰리스트에서 삭제
			GF_ULV_Add(i);
			return true;
		}
	}
	return false;
}



////	해당 유저를 클랜에서 재거 하는 함수
BOOL	GF_User_Out_Clan(LPVOID p_User,BOOL p_Type)
{
	//	유저 포인트 확인
	if(p_User == NULL) return false;
	classUSER	*Temp_User;
	Temp_User = (classUSER *)p_User;
	//	해당 유저를 리스트에서 찾는다.
	for(DWORD i = 0 ; i < DF_MAX_USER_LIST; i++)
	{
		//	동일 유저 포인터 찾기.
		if(g_User_List[i] == Temp_User)
		{
			//	클랜에서 유저 제거한다. 클랜 유저가 없은 해당 클랜은 리스트에서 없어짐
			//GF_Find_Clan(i,false,p_Type);

			return true;
		}
	}

	return false;
}


/*
	리스트 박스 관리.
*/
////	유저 리스트 박스 생성
BOOL	GF_User_List_View_Set(HWND hwndDlg)
{
	//	리스트 박스 확인
	if(g_hList_User == NULL)
	{
		//	생성
		g_hList_User = FL_Create_List(hwndDlg,"Connect User",0,200,420,500);
		ShowWindow(g_hList_User,SW_HIDE);
		if(g_hList_User == NULL) return false;
		//	리스트에 타이틀 넣기
		FL_Set_List_Col(g_hList_User,0,50,"NO");
		FL_Set_List_Col(g_hList_User,1,100,"ID");
		FL_Set_List_Col(g_hList_User,2,50,"Lv");
		FL_Set_List_Col(g_hList_User,3,50,"Job");
		FL_Set_List_Col(g_hList_User,4,90,"money");
		FL_Set_List_Col(g_hList_User,5,80,"Clan ID");
		//FL_Set_List_Col(g_hList_User,6,80,"Team No");
	}
	return true;
}

////	유저 리스트 추가.
BOOL	GF_ULV_Add(DWORD p_Num)
{
	if(g_hList_User == NULL) return false;
	//	유저 정보 넣기.
	char	Temp_S[100];
	//	데이터 넣기
	wsprintf(Temp_S,"%d",p_Num);
	FL_Set_List_Data(g_hList_User,p_Num,0,Temp_S);
	if(g_User_List[p_Num] == NULL)
	{
		ZeroMemory(Temp_S,100);
		FL_Set_List_Data(g_hList_User,p_Num,1,Temp_S);
		FL_Set_List_Data(g_hList_User,p_Num,2,Temp_S);
		FL_Set_List_Data(g_hList_User,p_Num,3,Temp_S);
		FL_Set_List_Data(g_hList_User,p_Num,4,Temp_S);
		FL_Set_List_Data(g_hList_User,p_Num,5,Temp_S);
		FL_Set_List_Data(g_hList_User,p_Num,6,Temp_S);
	}
	else
	{
		//	이름
		wsprintf(Temp_S,"%s",g_User_List[p_Num]->Get_NAME());
		FL_Set_List_Data(g_hList_User,p_Num,1,Temp_S);
		//	레벨
		wsprintf(Temp_S,"%d",g_User_List[p_Num]->Get_LEVEL());
		FL_Set_List_Data(g_hList_User,p_Num,2,Temp_S);
		//	직업
		wsprintf(Temp_S,"%d",g_User_List[p_Num]->Get_JOB());
		FL_Set_List_Data(g_hList_User,p_Num,3,Temp_S);
		//	보유 금액
		wsprintf(Temp_S,"%ld",g_User_List[p_Num]->Get_MONEY());
		FL_Set_List_Data(g_hList_User,p_Num,4,Temp_S);
		//	클랜 번호
		wsprintf(Temp_S,"%d",g_User_List[p_Num]->GetClanID());
		FL_Set_List_Data(g_hList_User,p_Num,5,Temp_S);
		//	유저의 팀 번호
		//wsprintf(Temp_S,"%d",g_User_List[p_Num]->Get_TEAM());
		//FL_Set_List_Data(g_hList_User,p_Num,6,Temp_S);

		//g_User_List[p_Num]->m_CLAN;
		//g_User_List[p_Num]->m_ClanNAME
	}
	return true;
}



////	클랜 리스트 박스 생성
BOOL	GF_Clan_List_View_Set( HWND hwndDlg )
{
	//	리스트 박스 확인
	if( g_hList_Clan == NULL )
	{
		//	생성
		g_hList_Clan = FL_Create_List( hwndDlg, "Connect Clan", 0, 200, 900, 500 );
		ShowWindow( g_hList_Clan, SW_HIDE );
		if( g_hList_Clan == NULL ) return FALSE;

		//	리스트에 타이틀 넣기
		//FL_Set_List_Col(g_hList_Clan,0,50,"NO");
		//FL_Set_List_Col(g_hList_Clan,1,50,"Clan ID");
		//FL_Set_List_Col(g_hList_Clan,2,50,"Clan NameLv");
		//FL_Set_List_Col(g_hList_Clan,3,50,"Clan Lv");
		//FL_Set_List_Col(g_hList_Clan,4,50,"Clan money");
		//FL_Set_List_Col(g_hList_Clan,5,50,"Rank Point");

		// 접속 클랜 정보
		FL_Set_List_Col( g_hList_Clan, 0, 50, "NO" );
		FL_Set_List_Col( g_hList_Clan, 1, 100, "Connected" );
		FL_Set_List_Col( g_hList_Clan, 2, 100, "ClanID(LV)" );
		FL_Set_List_Col( g_hList_Clan, 3, 100, "ClanName" );
		FL_Set_List_Col( g_hList_Clan, 4, 150, "MasterConnected" );
		FL_Set_List_Col( g_hList_Clan, 5, 200, "MasterName(Account)" );
		FL_Set_List_Col( g_hList_Clan, 6, 200, "ConnectedMemberCount" );
	}

	return true;
}

////	클랜 리스트 추가.
// (20070427:남병철) : CClanWar의 BOOL UpdateClanInfoList( classUSER* pMember ); 함수로 대체함
BOOL	GF_CLV_Add( DWORD p_Num )
{
	if( g_hList_Clan == NULL ) return false;
	//	유저 정보 넣기.
	char	Temp_S[100];
	//	데이터 넣기
	wsprintf( Temp_S, "%d", p_Num );
	FL_Set_List_Data( g_hList_Clan, p_Num, 0, Temp_S );	// NO
	if( g_Clan_List[p_Num].m_CLAN.m_dwClanID == 0 )
	{
		ZeroMemory( Temp_S, 100 );
		FL_Set_List_Data( g_hList_Clan, p_Num, 1, Temp_S );	// Connected
		FL_Set_List_Data( g_hList_Clan, p_Num, 2, Temp_S );	// ClanID(LV)
		FL_Set_List_Data( g_hList_Clan, p_Num, 3, Temp_S );	// ClanName
		FL_Set_List_Data( g_hList_Clan, p_Num, 4, Temp_S );	// MasterName(Account)
		FL_Set_List_Data( g_hList_Clan, p_Num, 5, Temp_S );	// MasterConnected
		FL_Set_List_Data( g_hList_Clan, p_Num, 6, Temp_S );	// MemberCount
	}
	else
	{
		//	ID
		wsprintf(Temp_S,"%d",g_Clan_List[p_Num].m_CLAN.m_dwClanID);
		FL_Set_List_Data(g_hList_Clan,p_Num,1,Temp_S);
		//	이름
		wsprintf(Temp_S,"%s",g_Clan_List[p_Num].m_ClanNAME);
		FL_Set_List_Data(g_hList_Clan,p_Num,2,Temp_S);
		//	레벨
		wsprintf(Temp_S,"%d",g_Clan_List[p_Num].m_CLAN.m_btClanLEVEL);
		FL_Set_List_Data(g_hList_Clan,p_Num,3,Temp_S);
		//	보유 금액
		wsprintf(Temp_S,"%ld",g_Clan_List[p_Num].m_CLAN.m_biClanMONEY);
		FL_Set_List_Data(g_hList_Clan,p_Num,4,Temp_S);
		//	랭크 포인트 처리
		wsprintf(Temp_S,"%d",g_Clan_List[p_Num].m_CLAN.m_iRankPoint);
		FL_Set_List_Data(g_hList_Clan,p_Num,4,Temp_S);
	}
	//g_Clan_List[0]->GetUserLimitCNT()
	return true;
}



////	클랜전 리스트 박스 생성
BOOL	GF_War_List_View_Set( HWND hwndDlg )
{
	//	리스트 박스 확인
	if( g_hList_CWar == NULL )
	{
		//	생성
		g_hList_CWar = FL_Create_List( hwndDlg, "Clan War List", 0, 200, 1270, 500 );
		ShowWindow( g_hList_CWar, SW_HIDE );
		if( g_hList_CWar == NULL ) return false;
		//	리스트에 타이틀 넣기
		FL_Set_List_Col( g_hList_CWar, 0, 90, "ZoneNo(WT)" );
		FL_Set_List_Col( g_hList_CWar, 1, 70, "Condition" );
		FL_Set_List_Col( g_hList_CWar, 2, 70, "PlayType" );
		FL_Set_List_Col( g_hList_CWar, 3, 50, "Time" );
		FL_Set_List_Col( g_hList_CWar, 4, 300, "(ClanID)(ATeamName)(RunningMemberCount)" );
		FL_Set_List_Col( g_hList_CWar, 5, 200, "(Account)(ATopGradeMember)" );
		FL_Set_List_Col( g_hList_CWar, 6, 300, "(ClanID)(BTeamName)(RunningMemberCount)" );
		FL_Set_List_Col( g_hList_CWar, 7, 200, "(Account)(BTopGradeMember)" );
	}

	return TRUE;
}

////	클랜전 리스트 추가.
// (20070428:남병철) : CClanWar의 BOOL UpdateClanWarList(); 함수로 대체함
BOOL	GF_WLV_Add(DWORD p_Num)
{
	if(g_hList_CWar == NULL) return false;
	//	유저 정보 넣기.
	char	Temp_S[100];
	//	데이터 넣기
	wsprintf(Temp_S,"%d",p_Num);
	FL_Set_List_Data(g_hList_CWar,p_Num,0,Temp_S);
	if(g_ClanWar_List[p_Num].m_CW_Npc == NULL)
	{
		ZeroMemory(Temp_S,100);
		FL_Set_List_Data(g_hList_CWar,p_Num,1,Temp_S);
		FL_Set_List_Data(g_hList_CWar,p_Num,2,Temp_S);
		FL_Set_List_Data(g_hList_CWar,p_Num,3,Temp_S);
		FL_Set_List_Data(g_hList_CWar,p_Num,4,Temp_S);
		FL_Set_List_Data(g_hList_CWar,p_Num,5,Temp_S);
		FL_Set_List_Data(g_hList_CWar,p_Num,6,Temp_S);
	}
	else
	{
		//	NPC 번호
		wsprintf(Temp_S,"%d",g_ClanWar_List[p_Num].m_CW_Npc->m_nCharIdx);
		FL_Set_List_Data(g_hList_CWar,p_Num,1,Temp_S);
		//	A팀
		wsprintf(Temp_S,"%d",g_ClanWar_List[p_Num].m_Clan_A);
		FL_Set_List_Data(g_hList_CWar,p_Num,2,Temp_S);
		wsprintf(Temp_S,"%d",g_ClanWar_List[p_Num].m_Type_A);
		FL_Set_List_Data(g_hList_CWar,p_Num,3,Temp_S);
		//	B팀
		wsprintf(Temp_S,"%d",g_ClanWar_List[p_Num].m_Clan_B);
		FL_Set_List_Data(g_hList_CWar,p_Num,4,Temp_S);
		wsprintf(Temp_S,"%d",g_ClanWar_List[p_Num].m_Type_B);
		FL_Set_List_Data(g_hList_CWar,p_Num,5,Temp_S);
		//	변수
		wsprintf(Temp_S,"%d,%d,%d,%d",g_ClanWar_List[p_Num].m_CW_Npc->Get_ObjVAR(0)
			,g_ClanWar_List[p_Num].m_CW_Npc->Get_ObjVAR(1)
			,g_ClanWar_List[p_Num].m_CW_Npc->Get_ObjVAR(2)
			,g_ClanWar_List[p_Num].m_CW_Npc->Get_ObjVAR(3));
		FL_Set_List_Data(g_hList_CWar,p_Num,6,Temp_S);
	}
	return true;
}



BOOL	QL_QUEST_LIST(HWND hwndDlg)
{
	//	리스트 박스 확인
	if( g_hList_Quest == NULL )
	{
		//	생성
		g_hList_Quest = FL_Create_List( hwndDlg, "Quest List", 400, 200, 450, 500 );
		ShowWindow(g_hList_Quest,SW_HIDE);
		if( g_hList_Quest == NULL ) return false;
		//	리스트에 타이틀 넣기
		FL_Set_List_Col( g_hList_Quest, QLZ_QUEST_IDX, 50, "NO" );
		FL_Set_List_Col( g_hList_Quest, QLZ_QUEST_NO, 100, "Quest ID");
		FL_Set_List_Col( g_hList_Quest, QLZ_QUEST_NAME, 300, "Quest Name");
		//	공백 만듬
		//for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
		//	GF_WLV_Add(i);
		QL_QUEST_LIST_INIT();
	}
	return true;
}

BOOL	QL_QUEST_LIST_INIT()
{
	int iRow = g_zQuestList[QLZ_QUEST_NO].Count();

	int i = 0;
	for( i = 0; i < iRow; i++ )
	{
		FL_Set_List_Data( g_hList_Quest, i, QLZ_QUEST_IDX, ZString( i+1 ).c_str() );
		FL_Set_List_Data( g_hList_Quest, i, QLZ_QUEST_NO, g_zQuestList[QLZ_QUEST_NO].IndexOf(i).c_str() );
		FL_Set_List_Data( g_hList_Quest, i, QLZ_QUEST_NAME, g_zQuestList[QLZ_QUEST_NAME].IndexOf(i).c_str() );
	}

	// 더미 라인
	FL_Set_List_Data( g_hList_Quest, i, QLZ_QUEST_IDX, "" );
	FL_Set_List_Data( g_hList_Quest, i, QLZ_QUEST_NO, "" );
	FL_Set_List_Data( g_hList_Quest, i, QLZ_QUEST_NAME, "" );
	FL_Set_List_Data( g_hList_Quest, i+1, QLZ_QUEST_IDX, "" );
	FL_Set_List_Data( g_hList_Quest, i+1, QLZ_QUEST_NO, "" );
	FL_Set_List_Data( g_hList_Quest, i+1, QLZ_QUEST_NAME, "" );

	return true;
}

/*
	접속유저의 클랜 관리
*/
////	유저의 클랜 찾기
BOOL	GF_Find_Clan(DWORD p_idx,BOOL Type,BOOL Sub_Type)
{
	if(p_idx >= DF_MAX_USER_LIST) return false;
	if(g_User_List[p_idx] == NULL) return false;
	//	유저의 클랜 확인
	if(g_User_List[p_idx]->m_CLAN.m_dwClanID == 0) return false;
	if(g_Clan_List == NULL) return false;

	//	등록된 클랜이 있는지 찾는다.
	for(DWORD i = 0 ; i < g_Clan_Max_List; i++)
	{
		if(g_User_List[p_idx]->m_CLAN.m_dwClanID
			== g_Clan_List[i].m_CLAN.m_dwClanID)
		{
			// 해당 클랜이 등록되어 있음 [유저만 등록]
			// for(DWORD u = 0 ; u < 15; u++)
			// (20070202:남병철) : 클랜원 수치의 15명 하드코딩을 CW_USER_NO의 수치로 변경 (최대 50명까지 가능)
			for( DWORD u = 0; u < G_CW_MAX_USER_NO[0][0];u++ )
			{
				//	동일 번호가 있는지 확인.
				if(g_Clan_List[i].m_User_IDX[u] == (p_idx+1))
				{
					//	등록을 위한 검색
					if(Type) g_Clan_List[i].m_User_IDX[u] = 0;
					//	삭제를 위한 검색
					else g_Clan_List[i].m_User_IDX[u] = 0;
				}
			}
			
			//	등록을 위한 검색
			if(Type)
			{
				//	빈자리 찾기
				// for(DWORD u = 0 ; u < 15; u++)
				// (20070202:남병철) : 클랜원 수치의 15명 하드코딩을 CW_USER_NO의 수치로 변경 (최대 50명까지 가능)
				for( DWORD u = 0; u < G_CW_MAX_USER_NO[0][0]; u++ )
				{
					//	동일 번호가 있는지 확인.
					if( g_Clan_List[i].m_User_IDX[u] == 0 )
					{
						g_Clan_List[i].m_User_IDX[u] = (p_idx+1);

						//	뷰 갱신
						GF_CLV_Add(i);

						return true;
					}
				}
			}
			//	삭제를 위한 검색
			else
			{
				if(Sub_Type)
				{
					//	클랜에서 유저를 제거하기 전에
					//	해당 클랜이 신청한 클랜전이 있다면 포기한다.
					GF_Clan_War_Cancel(i); // 클랜 리스트 번호 넘겨 준다.
				}
				//	1명이라도 있으면 삭제하지 않음
				// for(DWORD u = 0 ; u < 15; u++)
				// (20070202:남병철) : 클랜원 수치의 15명 하드코딩을 CW_USER_NO의 수치로 변경 (최대 50명까지 가능)
				for( DWORD u = 0; u < G_CW_MAX_USER_NO[0][0]; u++ )
				{
					if(g_Clan_List[i].m_User_IDX[u] != 0) 
					{
						//	뷰 갱신
						GF_CLV_Add(i);
						return true;
					}
				}
				//	모두 접속 안한 경우 삭제.
				GF_Clan_List_Del(i);
			}

			return true;
		}
	}
	//	신규 등록용.
	if(Type)
	{
		//	해당 클랜 신규 등록.
		GF_Clan_List_Add((LPVOID)&g_User_List[p_idx]->m_CLAN,g_User_List[p_idx]->m_ClanNAME.Get(),p_idx+1);
	}
	return true;
}

////	접속 클랜 추가.
BOOL	GF_Clan_List_Add(LPVOID p_Add_Clan,char *p_Name,DWORD p_Idx)
{
	if(p_Add_Clan == NULL) return false;
	tag_MY_CLAN	*Temp_Clan;
	Temp_Clan = (tag_MY_CLAN *)p_Add_Clan;
	////	빈자리에 넣기
	for(DWORD i = 0 ; i < g_Clan_Max_List; i++)
	{
		if(g_Clan_List[i].m_CLAN.m_dwClanID == 0)
		{
			//	클랜 정보 복사.
			memcpy(&g_Clan_List[i].m_CLAN,Temp_Clan,sizeof(tag_MY_CLAN));
			//	클랜 이름 복사
			wsprintf(g_Clan_List[i].m_ClanNAME,"%s",p_Name);
			//	클랜 접속 인원 번호 추가.
			g_Clan_List[i].m_User_IDX[0] = p_Idx;
			//	뷰 리스트에 추가
			GF_CLV_Add(i);
			return true;
		}
	}
	if(g_Clan_List[g_Clan_Max_List].m_CLAN.m_dwClanID == 0)
	{
		//	클랜 정보 복사.
		memcpy(&g_Clan_List[g_Clan_Max_List].m_CLAN,Temp_Clan,sizeof(tag_MY_CLAN));
		//	클랜 이름 복사
		wsprintf(g_Clan_List[g_Clan_Max_List].m_ClanNAME,"%s",p_Name);
		//	클랜 접속 인원 번호 추가.
		g_Clan_List[g_Clan_Max_List].m_User_IDX[0] = p_Idx;
		//	뷰 리스트에 추가
		GF_CLV_Add(g_Clan_Max_List);
		//	증가.
		g_Clan_Max_List++;

		return true;
	}

	return false;
}

////	접속 클랜 삭제.
BOOL	GF_Clan_List_Del(LPVOID p_Del_Clan)
{
	if(p_Del_Clan == NULL) return false;
	tag_MY_CLAN	*Temp_Clan;
	Temp_Clan = (tag_MY_CLAN *)p_Del_Clan;
	//	제거
	for(DWORD i = 0 ; i < g_Clan_Max_List; i++)
	{
		//	동일 유저 포인터 찾기.
		if(g_Clan_List[i].m_CLAN.m_dwClanID == Temp_Clan->m_dwClanID)
		{
			GF_Clan_List_Del(i);
			return true;
		}
	}
	return false;
}

////	접속 클랜 삭제.
BOOL	GF_Clan_List_Del(DWORD	idx)
{
	if(idx >= DF_MAX_CLAN_LIST) return false;
	//	삭제할 정보 지운다.
	ZeroMemory(&g_Clan_List[idx],sizeof(GF_Clan_Info));
	//	뷰 리스트에서 삭제
	GF_CLV_Add(idx);
	return true;
}

/*
	ini 파일 읽기 관련 함수
*/
int		Get_ini_Read(char *p_File_Name,char *p_Title,char *p_Name)
{
	//	전달된 값 확인
	if(p_File_Name == NULL) return -1;
	if(p_Title == NULL) return -1;
	if(p_Name == NULL) return -1;

	//	임시 버퍼.
	char	Temp_String[30];
	int		Temp_i = 0;
	ZeroMemory(Temp_String,30);
	//	파일에서 읽기
	GetPrivateProfileString(p_Title, p_Name, "0", Temp_String,30, p_File_Name);
	Temp_i = atoi(Temp_String);
	if(Temp_i < 0) return -1;
	else return Temp_i;
	//	오류
	return -1;
}

//#endif