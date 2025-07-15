
/*
	�ۼ��� �迵ȯ
*/

//#ifdef __CLAN_WAR_SET			// Ŭ���� �߰� ����


#include "GF_Define.h"
#include "F_GDI_List.h"
#include "GF_Data.h"
#include "ClanWar.h"

////	Ŭ�� �ʵ� ���� ���� ����
extern HWND	g_hList_CField;

////	��ư �ڵ�
HWND	g_Tool_BT1 = NULL;
HWND	g_Tool_BT2 = NULL;
HWND	g_Tool_BT3 = NULL;
HWND	g_Tool_BT4 = NULL;
HWND	g_Tool_BT5 = NULL;
HWND	g_Tool_BT6 = NULL;
////	���º���
BOOL	g_Tool_TF1 = true;
BOOL	g_Tool_TF2 = true;
BOOL	g_Tool_TF3 = true;
BOOL	g_Tool_TF4 = true;
BOOL	g_Tool_TF6 = true;

////	��ȭ ���� ����
BOOL	GF_User_Dlg_Create(HWND	p_Main_H)
{
	if(g_hUserWND != NULL) return false;
	WNDCLASS	wc; // ������ Ŭ���� ����ü ����
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
	// ������ Ŭ���� ���
	if(!RegisterClass(&wc))	return false;

	//  ���� ����
	g_hUserWND = CreateWindow("USER_DLG1","Game Server Toolbar",WS_BORDER | WS_THICKFRAME,
							  0,0,200,170,NULL,NULL,NULL,NULL);
	if(!g_hUserWND)	return false;

#ifdef __CLAN_WAR_EXT
	//	ũ��Ƽ�� ���� ����
	InitializeCriticalSection( &g_ClanWar_LOCK );
	InitializeCriticalSection( &g_ClanWar_DBLOCK );
#endif

	//  ������ �׸���
	ShowWindow(g_hUserWND,SW_SHOWNORMAL);

	//	���.
	ZeroMemory(G_Main_Dir,MAX_PATH);
	GetCurrentDirectory(MAX_PATH,G_Main_Dir);

	// ���� ����Ʈ �ʱ�ȭ
	if( !GF_User_List_Init() )
		MessageBox( NULL, "Ŭ���� �ʱ�ȭ ����!", "ERROR", MB_OK );

	//	�� ����Ʈ ����
	GF_User_List_View_Set(NULL);
	GF_Clan_List_View_Set(NULL);
	GF_War_List_View_Set(NULL);
	QL_QUEST_LIST(NULL);		// (20061121:����ö) : ����Ʈ ID ����Ʈ

	//	DB ����.
	if(!GF_Connect_DB_CW(p_Main_H))
		MessageBox(NULL,"Ŭ���� DB ���� ����! Ȯ�� �ٶ�...","���İ�??!!",MB_OK);

	////	�⺻ ������ �ֱ�
	//for(DWORD i = 0 ; i < 100; i++)
	//	GF_ULV_Add(i);
	return true;
}

////	�ݱ�
BOOL	GF_User_Dlg_Delete()
{
	//	�ڵ� Ȯ��
	if(g_hUserWND == NULL) return false;

	//	Ŭ���� DB ���� ����
	GF_DisConnect_DB_CW();

	//	�޸� ����
	if(g_Clan_List != NULL)	
	{
		VirtualFree(g_Clan_List,0,MEM_DECOMMIT);
		g_Clan_List = NULL;
	}

	//	�ڽĵ� �����.
	DestroyWindow(g_hList_CWar);
	DestroyWindow(g_hList_User);
	DestroyWindow(g_hList_Clan);
	DestroyWindow(g_hList_Quest);

	// ������ ����
	DestroyWindow(g_hUserWND);
	g_hUserWND = NULL;

#ifdef __CLAN_WAR_EXT
	//	ũ��Ƽ�� ���� ����
	DeleteCriticalSection(&g_ClanWar_LOCK);
	DeleteCriticalSection(&g_ClanWar_DBLOCK);
#endif

	return true;
}

////	���� ���� ��ȭ����
LRESULT CALLBACK GF_User_Dlg_Proc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    switch(uMsg)
	{
	case WM_CREATE:
		{
			////	���� ��ư ����
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
			//	��ư ����
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
			//	��ư ����
			FL_Delete_Button(g_Tool_BT1);
			FL_Delete_Button(g_Tool_BT2);
			FL_Delete_Button(g_Tool_BT3);
			FL_Delete_Button(g_Tool_BT4);
			FL_Delete_Button(g_Tool_BT5);
			FL_Delete_Button(g_Tool_BT6);
			//	��ȭ ���� �ݱ�.
			g_hUserWND = NULL;
			break;
		}
	case WM_COMMAND:
		{
			//	��ư �̺�Ʈ ó��
			if(FL_Command_Button(g_Tool_BT1,lParam))
			{
				// ���� ���� Ȱ��ȭ ��Ȱ��ȭ
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
				// Ŭ�� ���� Ȱ��ȭ ��Ȱ��ȭ
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
				// Ŭ���� ���� Ȱ��ȭ ��Ȱ��ȭ
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
				// Ŭ�� �ʵ� ���� Ȱ��ȭ ��Ȱ��ȭ
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
				// Ŭ�� �ʵ� �ٽ� �б�
				if(IDOK == MessageBox(hwndDlg," -SHO_GS_CField.ini- File ReLoad...?","Clan Field Event Setting",MB_OKCANCEL))
				{
					if( !GF_CF_Set_INI_Load() )
						MessageBox( NULL, "Ŭ�ʵ� �ʱ�ȭ ����!", "ERROR", MB_OK );
				}
			}
			if(FL_Command_Button(g_Tool_BT6,lParam))
			{
				// Ŭ�� �ʵ� ���� Ȱ��ȭ ��Ȱ��ȭ
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
					////	�׽�Ʈ
					//GF_CF_Set_INI_Load();
					if(g_User_List[0] != NULL)
						g_User_List[0]->SetClanRank(25);
					break;
				}
			case VK_F3:
				{
					if(g_User_List[0] != NULL)
						g_User_List[0]->SetClanRank(-25);
					////	����Ʈ ���� �ٽ� ����.
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
	���� ���� ����,
*/
////	���� ���� ����Ʈ �ʱ�ȭ
BOOL	GF_User_List_Init()
{

#ifdef __CLAN_WAR_EXT

	// Ŭ���� �ʱ�ȭ
	if( CClanWar::GetInstance()->Init() == FALSE )
		return FALSE;

	// (20070424:����ö) : ġƮŰ ȣȯ���� ���� ������ ���� ����Ʈ �츲
	// Ex) send1, send2, send3
	//	���� ����Ʈ �ʱ�ȭ
	ZeroMemory( g_User_List, DF_MAX_USER_LIST * 4 );

#else

	//	���� ����Ʈ �ʱ�ȭ
	ZeroMemory(g_User_List,DF_MAX_USER_LIST*4);

	// Ŭ���� �ʱ�ȭ
	if( !GF_Clan_War_Init() )
		return FALSE;

	//	Ŭ�� ����Ʈ �ʱ�ȭ
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

////	���� ���� �߰�.
BOOL	GF_User_List_Add(LPVOID p_Add_User,DWORD Type)
{
	if(p_Add_User == NULL) return false;
	classUSER	*Temp_User;
	Temp_User = (classUSER *)p_Add_User;
	
	switch(Type)
	{
	case 0:
		{
			//	���� �߰�.
			for(DWORD i = 0 ; i < DF_MAX_USER_LIST; i++)
			{
				if(g_User_List[i] == NULL)
				{
					g_User_List[i] = Temp_User;
					// (20070424:����ö) : ġƮŰ���� ���� ����Ʈ�� ����ϹǷ� �Ʒ��� Ŭ�� ������ ����
					//GF_Find_Clan(i,true,false);
					//	�丮��Ʈ�� �߰�
					GF_ULV_Add(i);
					return true;
				}
			}
			break;
		}
	case 1:
		{
			//	���� ���� ������ ������Ʈ Ȯ���ϱ�
			for(DWORD i = 0 ; i < DF_MAX_USER_LIST; i++)
			{
				//	���� �޸� ������.
				if(g_User_List[i] == Temp_User)
				{
					//	���� �̸� Ȯ��.
					if(0 == lstrcmp(g_User_List[i]->Get_NAME(),Temp_User->Get_NAME()))
					{
						g_User_List[i] = Temp_User;
						//GF_Find_Clan(i,true,false);
						//	�丮��Ʈ�� �߰�
						GF_ULV_Add(i);
						return true;
					}
					else
					{
						// �޸� �����ʹ� ������. �̸��� �ٸ� ���.. [������ �ش�. ���� ���� ���� Ȯ���Ѵ�. ������ ����.]
						// ���� �Ѵ�.
						g_User_List[i] = Temp_User;
						//GF_Find_Clan(i,true,false);
						//	�丮��Ʈ�� �߰�
						GF_ULV_Add(i);
						return true;
					}
				}
			}
			break;
		}
	case 2:
		{
			//	���� ���� ������ Ŭ�� ���� Ȯ���ϱ�
			for(DWORD i = 0 ; i < DF_MAX_USER_LIST; i++)
			{
				//	���� �޸� ������.
				if(g_User_List[i] == Temp_User)
				{
					//	���� �̸� Ȯ��.
					if(0 == lstrcmp(g_User_List[i]->Get_NAME(),Temp_User->Get_NAME()))
					{
						g_User_List[i] = Temp_User;
						//if(Temp_User->Is_ClanMASTER())
						//	GF_Find_Clan(i,false,true);
						//else
						//	GF_Find_Clan(i,false,false);
						//	�丮��Ʈ�� �߰�
						GF_ULV_Add(i);
						return true;
					}
					else
					{
						// �޸� �����ʹ� ������. �̸��� �ٸ� ���.. [������ �ش�. ���� ���� ���� Ȯ���Ѵ�. ������ ����.]
						// ���� �Ѵ�.
						g_User_List[i] = Temp_User;
						//if(Temp_User->Is_ClanMASTER())
						//	GF_Find_Clan(i,false,true);
						//else
						//	GF_Find_Clan(i,false,false);
						//	�丮��Ʈ�� �߰�
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



////	���� ���� ����.
BOOL	GF_User_List_Del(LPVOID p_Del_User)
{
	if(p_Del_User == NULL) return false;
	classUSER	*Temp_User;
	Temp_User = (classUSER *)p_Del_User;

	//	Ŭ���� ���� �־��ٸ� �ο� ����
	//if(Temp_User->GetZONE() != NULL)
	//{
	//	DWORD	Temp_Zone_N = GF_Clan_War_Zone_Is(Temp_User->GetZONE()->Get_ZoneNO());
	//	//	Ŭ���� �� Ȯ��
	//	if(Temp_Zone_N > 0)
	//	{
	//		GF_Sub_ClanWar_Number(Temp_Zone_N,Temp_User->Get_TeamNO());
	//	}
	//}


	//	����
	for(DWORD i = 0 ; i < DF_MAX_USER_LIST; i++)
	{
		//	���� ���� ������ ã��.
		if(g_User_List[i] == Temp_User)
		{
			//	�ش� ������ Ŭ�� ó��
			//GF_Find_Clan(i,false,false);
			//	���� �κп��� ���� �������� Ȯ�� �ϴ� ��� �־�� ��
			g_User_List[i] = NULL;
			//	�丮��Ʈ���� ����
			GF_ULV_Add(i);
			return true;
		}
	}
	return false;
}



////	�ش� ������ Ŭ������ ��� �ϴ� �Լ�
BOOL	GF_User_Out_Clan(LPVOID p_User,BOOL p_Type)
{
	//	���� ����Ʈ Ȯ��
	if(p_User == NULL) return false;
	classUSER	*Temp_User;
	Temp_User = (classUSER *)p_User;
	//	�ش� ������ ����Ʈ���� ã�´�.
	for(DWORD i = 0 ; i < DF_MAX_USER_LIST; i++)
	{
		//	���� ���� ������ ã��.
		if(g_User_List[i] == Temp_User)
		{
			//	Ŭ������ ���� �����Ѵ�. Ŭ�� ������ ���� �ش� Ŭ���� ����Ʈ���� ������
			//GF_Find_Clan(i,false,p_Type);

			return true;
		}
	}

	return false;
}


/*
	����Ʈ �ڽ� ����.
*/
////	���� ����Ʈ �ڽ� ����
BOOL	GF_User_List_View_Set(HWND hwndDlg)
{
	//	����Ʈ �ڽ� Ȯ��
	if(g_hList_User == NULL)
	{
		//	����
		g_hList_User = FL_Create_List(hwndDlg,"Connect User",0,200,420,500);
		ShowWindow(g_hList_User,SW_HIDE);
		if(g_hList_User == NULL) return false;
		//	����Ʈ�� Ÿ��Ʋ �ֱ�
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

////	���� ����Ʈ �߰�.
BOOL	GF_ULV_Add(DWORD p_Num)
{
	if(g_hList_User == NULL) return false;
	//	���� ���� �ֱ�.
	char	Temp_S[100];
	//	������ �ֱ�
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
		//	�̸�
		wsprintf(Temp_S,"%s",g_User_List[p_Num]->Get_NAME());
		FL_Set_List_Data(g_hList_User,p_Num,1,Temp_S);
		//	����
		wsprintf(Temp_S,"%d",g_User_List[p_Num]->Get_LEVEL());
		FL_Set_List_Data(g_hList_User,p_Num,2,Temp_S);
		//	����
		wsprintf(Temp_S,"%d",g_User_List[p_Num]->Get_JOB());
		FL_Set_List_Data(g_hList_User,p_Num,3,Temp_S);
		//	���� �ݾ�
		wsprintf(Temp_S,"%ld",g_User_List[p_Num]->Get_MONEY());
		FL_Set_List_Data(g_hList_User,p_Num,4,Temp_S);
		//	Ŭ�� ��ȣ
		wsprintf(Temp_S,"%d",g_User_List[p_Num]->GetClanID());
		FL_Set_List_Data(g_hList_User,p_Num,5,Temp_S);
		//	������ �� ��ȣ
		//wsprintf(Temp_S,"%d",g_User_List[p_Num]->Get_TEAM());
		//FL_Set_List_Data(g_hList_User,p_Num,6,Temp_S);

		//g_User_List[p_Num]->m_CLAN;
		//g_User_List[p_Num]->m_ClanNAME
	}
	return true;
}



////	Ŭ�� ����Ʈ �ڽ� ����
BOOL	GF_Clan_List_View_Set( HWND hwndDlg )
{
	//	����Ʈ �ڽ� Ȯ��
	if( g_hList_Clan == NULL )
	{
		//	����
		g_hList_Clan = FL_Create_List( hwndDlg, "Connect Clan", 0, 200, 900, 500 );
		ShowWindow( g_hList_Clan, SW_HIDE );
		if( g_hList_Clan == NULL ) return FALSE;

		//	����Ʈ�� Ÿ��Ʋ �ֱ�
		//FL_Set_List_Col(g_hList_Clan,0,50,"NO");
		//FL_Set_List_Col(g_hList_Clan,1,50,"Clan ID");
		//FL_Set_List_Col(g_hList_Clan,2,50,"Clan NameLv");
		//FL_Set_List_Col(g_hList_Clan,3,50,"Clan Lv");
		//FL_Set_List_Col(g_hList_Clan,4,50,"Clan money");
		//FL_Set_List_Col(g_hList_Clan,5,50,"Rank Point");

		// ���� Ŭ�� ����
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

////	Ŭ�� ����Ʈ �߰�.
// (20070427:����ö) : CClanWar�� BOOL UpdateClanInfoList( classUSER* pMember ); �Լ��� ��ü��
BOOL	GF_CLV_Add( DWORD p_Num )
{
	if( g_hList_Clan == NULL ) return false;
	//	���� ���� �ֱ�.
	char	Temp_S[100];
	//	������ �ֱ�
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
		//	�̸�
		wsprintf(Temp_S,"%s",g_Clan_List[p_Num].m_ClanNAME);
		FL_Set_List_Data(g_hList_Clan,p_Num,2,Temp_S);
		//	����
		wsprintf(Temp_S,"%d",g_Clan_List[p_Num].m_CLAN.m_btClanLEVEL);
		FL_Set_List_Data(g_hList_Clan,p_Num,3,Temp_S);
		//	���� �ݾ�
		wsprintf(Temp_S,"%ld",g_Clan_List[p_Num].m_CLAN.m_biClanMONEY);
		FL_Set_List_Data(g_hList_Clan,p_Num,4,Temp_S);
		//	��ũ ����Ʈ ó��
		wsprintf(Temp_S,"%d",g_Clan_List[p_Num].m_CLAN.m_iRankPoint);
		FL_Set_List_Data(g_hList_Clan,p_Num,4,Temp_S);
	}
	//g_Clan_List[0]->GetUserLimitCNT()
	return true;
}



////	Ŭ���� ����Ʈ �ڽ� ����
BOOL	GF_War_List_View_Set( HWND hwndDlg )
{
	//	����Ʈ �ڽ� Ȯ��
	if( g_hList_CWar == NULL )
	{
		//	����
		g_hList_CWar = FL_Create_List( hwndDlg, "Clan War List", 0, 200, 1270, 500 );
		ShowWindow( g_hList_CWar, SW_HIDE );
		if( g_hList_CWar == NULL ) return false;
		//	����Ʈ�� Ÿ��Ʋ �ֱ�
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

////	Ŭ���� ����Ʈ �߰�.
// (20070428:����ö) : CClanWar�� BOOL UpdateClanWarList(); �Լ��� ��ü��
BOOL	GF_WLV_Add(DWORD p_Num)
{
	if(g_hList_CWar == NULL) return false;
	//	���� ���� �ֱ�.
	char	Temp_S[100];
	//	������ �ֱ�
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
		//	NPC ��ȣ
		wsprintf(Temp_S,"%d",g_ClanWar_List[p_Num].m_CW_Npc->m_nCharIdx);
		FL_Set_List_Data(g_hList_CWar,p_Num,1,Temp_S);
		//	A��
		wsprintf(Temp_S,"%d",g_ClanWar_List[p_Num].m_Clan_A);
		FL_Set_List_Data(g_hList_CWar,p_Num,2,Temp_S);
		wsprintf(Temp_S,"%d",g_ClanWar_List[p_Num].m_Type_A);
		FL_Set_List_Data(g_hList_CWar,p_Num,3,Temp_S);
		//	B��
		wsprintf(Temp_S,"%d",g_ClanWar_List[p_Num].m_Clan_B);
		FL_Set_List_Data(g_hList_CWar,p_Num,4,Temp_S);
		wsprintf(Temp_S,"%d",g_ClanWar_List[p_Num].m_Type_B);
		FL_Set_List_Data(g_hList_CWar,p_Num,5,Temp_S);
		//	����
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
	//	����Ʈ �ڽ� Ȯ��
	if( g_hList_Quest == NULL )
	{
		//	����
		g_hList_Quest = FL_Create_List( hwndDlg, "Quest List", 400, 200, 450, 500 );
		ShowWindow(g_hList_Quest,SW_HIDE);
		if( g_hList_Quest == NULL ) return false;
		//	����Ʈ�� Ÿ��Ʋ �ֱ�
		FL_Set_List_Col( g_hList_Quest, QLZ_QUEST_IDX, 50, "NO" );
		FL_Set_List_Col( g_hList_Quest, QLZ_QUEST_NO, 100, "Quest ID");
		FL_Set_List_Col( g_hList_Quest, QLZ_QUEST_NAME, 300, "Quest Name");
		//	���� ����
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

	// ���� ����
	FL_Set_List_Data( g_hList_Quest, i, QLZ_QUEST_IDX, "" );
	FL_Set_List_Data( g_hList_Quest, i, QLZ_QUEST_NO, "" );
	FL_Set_List_Data( g_hList_Quest, i, QLZ_QUEST_NAME, "" );
	FL_Set_List_Data( g_hList_Quest, i+1, QLZ_QUEST_IDX, "" );
	FL_Set_List_Data( g_hList_Quest, i+1, QLZ_QUEST_NO, "" );
	FL_Set_List_Data( g_hList_Quest, i+1, QLZ_QUEST_NAME, "" );

	return true;
}

/*
	���������� Ŭ�� ����
*/
////	������ Ŭ�� ã��
BOOL	GF_Find_Clan(DWORD p_idx,BOOL Type,BOOL Sub_Type)
{
	if(p_idx >= DF_MAX_USER_LIST) return false;
	if(g_User_List[p_idx] == NULL) return false;
	//	������ Ŭ�� Ȯ��
	if(g_User_List[p_idx]->m_CLAN.m_dwClanID == 0) return false;
	if(g_Clan_List == NULL) return false;

	//	��ϵ� Ŭ���� �ִ��� ã�´�.
	for(DWORD i = 0 ; i < g_Clan_Max_List; i++)
	{
		if(g_User_List[p_idx]->m_CLAN.m_dwClanID
			== g_Clan_List[i].m_CLAN.m_dwClanID)
		{
			// �ش� Ŭ���� ��ϵǾ� ���� [������ ���]
			// for(DWORD u = 0 ; u < 15; u++)
			// (20070202:����ö) : Ŭ���� ��ġ�� 15�� �ϵ��ڵ��� CW_USER_NO�� ��ġ�� ���� (�ִ� 50����� ����)
			for( DWORD u = 0; u < G_CW_MAX_USER_NO[0][0];u++ )
			{
				//	���� ��ȣ�� �ִ��� Ȯ��.
				if(g_Clan_List[i].m_User_IDX[u] == (p_idx+1))
				{
					//	����� ���� �˻�
					if(Type) g_Clan_List[i].m_User_IDX[u] = 0;
					//	������ ���� �˻�
					else g_Clan_List[i].m_User_IDX[u] = 0;
				}
			}
			
			//	����� ���� �˻�
			if(Type)
			{
				//	���ڸ� ã��
				// for(DWORD u = 0 ; u < 15; u++)
				// (20070202:����ö) : Ŭ���� ��ġ�� 15�� �ϵ��ڵ��� CW_USER_NO�� ��ġ�� ���� (�ִ� 50����� ����)
				for( DWORD u = 0; u < G_CW_MAX_USER_NO[0][0]; u++ )
				{
					//	���� ��ȣ�� �ִ��� Ȯ��.
					if( g_Clan_List[i].m_User_IDX[u] == 0 )
					{
						g_Clan_List[i].m_User_IDX[u] = (p_idx+1);

						//	�� ����
						GF_CLV_Add(i);

						return true;
					}
				}
			}
			//	������ ���� �˻�
			else
			{
				if(Sub_Type)
				{
					//	Ŭ������ ������ �����ϱ� ����
					//	�ش� Ŭ���� ��û�� Ŭ������ �ִٸ� �����Ѵ�.
					GF_Clan_War_Cancel(i); // Ŭ�� ����Ʈ ��ȣ �Ѱ� �ش�.
				}
				//	1���̶� ������ �������� ����
				// for(DWORD u = 0 ; u < 15; u++)
				// (20070202:����ö) : Ŭ���� ��ġ�� 15�� �ϵ��ڵ��� CW_USER_NO�� ��ġ�� ���� (�ִ� 50����� ����)
				for( DWORD u = 0; u < G_CW_MAX_USER_NO[0][0]; u++ )
				{
					if(g_Clan_List[i].m_User_IDX[u] != 0) 
					{
						//	�� ����
						GF_CLV_Add(i);
						return true;
					}
				}
				//	��� ���� ���� ��� ����.
				GF_Clan_List_Del(i);
			}

			return true;
		}
	}
	//	�ű� ��Ͽ�.
	if(Type)
	{
		//	�ش� Ŭ�� �ű� ���.
		GF_Clan_List_Add((LPVOID)&g_User_List[p_idx]->m_CLAN,g_User_List[p_idx]->m_ClanNAME.Get(),p_idx+1);
	}
	return true;
}

////	���� Ŭ�� �߰�.
BOOL	GF_Clan_List_Add(LPVOID p_Add_Clan,char *p_Name,DWORD p_Idx)
{
	if(p_Add_Clan == NULL) return false;
	tag_MY_CLAN	*Temp_Clan;
	Temp_Clan = (tag_MY_CLAN *)p_Add_Clan;
	////	���ڸ��� �ֱ�
	for(DWORD i = 0 ; i < g_Clan_Max_List; i++)
	{
		if(g_Clan_List[i].m_CLAN.m_dwClanID == 0)
		{
			//	Ŭ�� ���� ����.
			memcpy(&g_Clan_List[i].m_CLAN,Temp_Clan,sizeof(tag_MY_CLAN));
			//	Ŭ�� �̸� ����
			wsprintf(g_Clan_List[i].m_ClanNAME,"%s",p_Name);
			//	Ŭ�� ���� �ο� ��ȣ �߰�.
			g_Clan_List[i].m_User_IDX[0] = p_Idx;
			//	�� ����Ʈ�� �߰�
			GF_CLV_Add(i);
			return true;
		}
	}
	if(g_Clan_List[g_Clan_Max_List].m_CLAN.m_dwClanID == 0)
	{
		//	Ŭ�� ���� ����.
		memcpy(&g_Clan_List[g_Clan_Max_List].m_CLAN,Temp_Clan,sizeof(tag_MY_CLAN));
		//	Ŭ�� �̸� ����
		wsprintf(g_Clan_List[g_Clan_Max_List].m_ClanNAME,"%s",p_Name);
		//	Ŭ�� ���� �ο� ��ȣ �߰�.
		g_Clan_List[g_Clan_Max_List].m_User_IDX[0] = p_Idx;
		//	�� ����Ʈ�� �߰�
		GF_CLV_Add(g_Clan_Max_List);
		//	����.
		g_Clan_Max_List++;

		return true;
	}

	return false;
}

////	���� Ŭ�� ����.
BOOL	GF_Clan_List_Del(LPVOID p_Del_Clan)
{
	if(p_Del_Clan == NULL) return false;
	tag_MY_CLAN	*Temp_Clan;
	Temp_Clan = (tag_MY_CLAN *)p_Del_Clan;
	//	����
	for(DWORD i = 0 ; i < g_Clan_Max_List; i++)
	{
		//	���� ���� ������ ã��.
		if(g_Clan_List[i].m_CLAN.m_dwClanID == Temp_Clan->m_dwClanID)
		{
			GF_Clan_List_Del(i);
			return true;
		}
	}
	return false;
}

////	���� Ŭ�� ����.
BOOL	GF_Clan_List_Del(DWORD	idx)
{
	if(idx >= DF_MAX_CLAN_LIST) return false;
	//	������ ���� �����.
	ZeroMemory(&g_Clan_List[idx],sizeof(GF_Clan_Info));
	//	�� ����Ʈ���� ����
	GF_CLV_Add(idx);
	return true;
}

/*
	ini ���� �б� ���� �Լ�
*/
int		Get_ini_Read(char *p_File_Name,char *p_Title,char *p_Name)
{
	//	���޵� �� Ȯ��
	if(p_File_Name == NULL) return -1;
	if(p_Title == NULL) return -1;
	if(p_Name == NULL) return -1;

	//	�ӽ� ����.
	char	Temp_String[30];
	int		Temp_i = 0;
	ZeroMemory(Temp_String,30);
	//	���Ͽ��� �б�
	GetPrivateProfileString(p_Title, p_Name, "0", Temp_String,30, p_File_Name);
	Temp_i = atoi(Temp_String);
	if(Temp_i < 0) return -1;
	else return Temp_i;
	//	����
	return -1;
}

//#endif