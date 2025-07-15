
/*
	작성자 김영환
*/

//#ifdef __CLAN_WAR_SET			// 클랜전 추가 정의


#include "GF_Define.h"
#include "GF_Data.h"


////	게임 진행 타임 감소
BOOL	GF_CWar_Play_Time(GF_ClanWar_Set *p_CWar_Room,DWORD	p_Idx);

////	게임 종료 타임 감소
BOOL	GF_CWar_End_Time(GF_ClanWar_Set *p_CWar_Room,DWORD	p_Idx);

////	타임 처리 함수
BOOL	GF_CWar_Time_Proc()
{
	//	10초에 한번씩 들어온다.
	//MessageBox(NULL,"10초 지났다 띠블","확인 눌러!",MB_OK);

	// 1) 만들어진 방을 찾는다.
	// 2) 게임 진행 변수 확인하고 타임 카운트 감소 한다.
	// 3) 지나간 카운트를 출력한다.
	// 4) 다음 단계 처리 한다.
	// 5) 타임 아웃 되면 참여 유저 모두 포탈 시킨다.[강제]
	// 6) 해당 방의 몹을 정하고 방을 지운다. [다시 사용 가능하게]

	/*
		코드 작성.
	*/
	//	클랜존 개수 만큼 반복 검색한다.
	for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
	{
		//	게임 중인 방인지 검사한다.
		switch(g_ClanWar_List[i].m_Play_Type)
		{
			//	클랜장 소환 때부터 게임 시작이다.
		case 2:
		case 3:
		case 4:
			{
				// 게임 시작 중인 방이다
				GF_CWar_Play_Time(&g_ClanWar_List[i],i);
				//	게임 진행중에 보스 몹 HP 전달 한다.
				GF_ClanWar_Boss_MOB_HP(i+1);
				break;
			}
		case 5:
			{
				// 게임 종료 대기중
				GF_CWar_End_Time(&g_ClanWar_List[i],i);
				//	게임 진행중에 보스 몹 HP 전달 한다.
				GF_ClanWar_Boss_MOB_HP(i+1);
				break;
			}
		}
	}

	return true;
}

////	게임 진행 타임 감소
BOOL	GF_CWar_Play_Time(GF_ClanWar_Set *p_CWar_Room,DWORD	p_Idx)
{
	if(p_CWar_Room == NULL) return false;
	//	타임값 확인
	if(p_CWar_Room->m_Play_Count > 0)
	{
		//	타임 감소
		p_CWar_Room->m_Play_Count--;
		//	카운트 감소 메시지 보냄.
		GF_ClanWar_Join_OK_Send(p_Idx,7,0);
		//	게임 시작 메시지 보냄. 대기시간 처리.
		if(p_CWar_Room->m_Play_Count == (DF_CWAR_PLAY_TIME - DF_CWAR_WAIT_TIME))
		{
			//	게임 시작 처리
			GF_Send_CLANWAR_Progress(p_Idx + 1, 1);
			//	게이트 열림.
			p_CWar_Room->m_Gate_TF = true;
			GF_Set_ClanWar_Gate_NPC(NULL,p_Idx + 1,false);	// 문을 연다.
		}
		else
			GF_Set_ClanWar_Gate_NPC(NULL,p_Idx + 1,true);	// 문의 상태값 보냄
	}
	else
	{
		//	게임 종료 타임으로 변환
		p_CWar_Room->m_Play_Type = 5;
		p_CWar_Room->m_Play_Count = DF_CWAR_END_TIME;
		//	유저에게 방 상태 전송.
		GF_ClanWar_Join_OK_Send(p_Idx,7,0);

		////	게이트 닫는다
		//p_CWar_Room->m_Gate_TF = false;
		//GF_Set_ClanWar_Gate_NPC(NULL,p_Idx + 1);
		
		////	몬스터 모두 삭제.
		//GF_ClanWar_Mob_Del2((LPVOID)p_CWar_Room,0);
		
		//	승자 없음 비겼슴.
		GF_Win_CWar_Set((LPVOID)p_CWar_Room,0,true);
		//	비긴 경우 퀘스트 하달. 기존 퀘스트 삭제.
		DWORD t_AM,t_BM,t_CM;
		switch(p_CWar_Room->m_War_Type)
		{
		case 0:
			//	A,B 팀 방장의 퀘스트 삭제.
			t_AM = GF_Clan_Fine_List(p_CWar_Room->m_Clan_A);
			t_BM = GF_Clan_Fine_List(p_CWar_Room->m_Clan_B);
			if(t_AM > 0)
			{
				t_AM = t_AM - 1;	
				t_CM = GF_Clan_Master_Fine(t_AM);	//A 팀 마스터 찾기
				if(t_CM > 0)
				{
					//	퀘스트 지운다.
					GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
					//	동점 퀘스트 등록
					GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2817);
				}
				else
				{
					// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
					// 다음 순위 클랜 유저를 얻어서 처리한다.
					t_CM = GF_Clan_Master_Sub_Fine(t_AM);
					if(t_CM > 0)
					{
						//	퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2817);
					}
				}
			}
			if(t_BM > 0)
			{
				t_BM = t_BM - 1;	
				t_CM = GF_Clan_Master_Fine(t_BM);	//B 팀 마스터 찾기
				if(t_CM > 0)
				{
					//	퀘스트 지운다.
					GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
					//	동점 퀘스트 등록
					GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2817);
				}
				else
				{
					// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
					// 다음 순위 클랜 유저를 얻어서 처리한다.
					t_CM = GF_Clan_Master_Sub_Fine(t_BM);
					if(t_CM > 0)
					{
						//	퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2817);
					}
				}
			}
			break;
		case 1:
			//	A,B 팀 방장의 퀘스트 삭제.
			t_AM = GF_Clan_Fine_List(p_CWar_Room->m_Clan_A);
			t_BM = GF_Clan_Fine_List(p_CWar_Room->m_Clan_B);
			if(t_AM > 0)
			{
				t_AM = t_AM - 1;	
				t_CM = GF_Clan_Master_Fine(t_AM);	//A 팀 마스터 찾기
				if(t_CM > 0)
				{
					//	퀘스트 지운다.
					GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
					//	동점 퀘스트 등록
					GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2818);
				}
				else
				{
					// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
					// 다음 순위 클랜 유저를 얻어서 처리한다.
					t_CM = GF_Clan_Master_Sub_Fine(t_AM);
					if(t_CM > 0)
					{
						//	퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2818);
					}
				}
			}
			if(t_BM > 0)
			{
				t_BM = t_BM - 1;	
				t_CM = GF_Clan_Master_Fine(t_BM);	//B 팀 마스터 찾기
				if(t_CM > 0)
				{
					//	퀘스트 지운다.
					GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
					//	동점 퀘스트 등록
					GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2818);
				}
				else
				{
					// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
					// 다음 순위 클랜 유저를 얻어서 처리한다.
					t_CM = GF_Clan_Master_Sub_Fine(t_BM);
					if(t_CM > 0)
					{
						//	퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2818);
					}
				}
			}
			break;
		case 2:
			//	A,B 팀 방장의 퀘스트 삭제.
			t_AM = GF_Clan_Fine_List(p_CWar_Room->m_Clan_A);
			t_BM = GF_Clan_Fine_List(p_CWar_Room->m_Clan_B);
			if(t_AM > 0)
			{
				t_AM = t_AM - 1;	
				t_CM = GF_Clan_Master_Fine(t_AM);	//A 팀 마스터 찾기
				if(t_CM > 0)
				{
					//	퀘스트 지운다.
					GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
					//	동점 퀘스트 등록
					GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2819);
				}
				else
				{
					// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
					// 다음 순위 클랜 유저를 얻어서 처리한다.
					t_CM = GF_Clan_Master_Sub_Fine(t_AM);
					if(t_CM > 0)
					{
						//	퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2819);
					}
				}
			}
			if(t_BM > 0)
			{
				t_BM = t_BM - 1;	
				t_CM = GF_Clan_Master_Fine(t_BM);	//B 팀 마스터 찾기
				if(t_CM > 0)
				{
					//	퀘스트 지운다.
					GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
					//	동점 퀘스트 등록
					GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2819);
				}
				else
				{
					// 이긴 팀의 클랜장이 없다.. 어떻게 처리할까?
					// 다음 순위 클랜 유저를 얻어서 처리한다.
					t_CM = GF_Clan_Master_Sub_Fine(t_BM);
					if(t_CM > 0)
					{
						//	퀘스트 등록
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2819);
					}
				}
			}
			break;
		}
		//	동점 승리 메시지 전송
		GF_Send_CLANWAR_Progress(p_Idx + 1,4);

		//	PK 모드 오프 시킨다.
		g_pZoneLIST->Set_PK_FLAG((short)p_CWar_Room->m_ZoneNO,false);

		//	모든 몬스터의 체력을 회복 시킨다.
		GF_ClanWar_MOB_HP_ReSet((LPVOID)&p_CWar_Room,1);
		GF_ClanWar_MOB_HP_ReSet((LPVOID)&p_CWar_Room,2);
	}
	return true;
}

////	게임 종료 타임 감소
BOOL	GF_CWar_End_Time(GF_ClanWar_Set *p_CWar_Room,DWORD p_Idx)
{
	if(p_CWar_Room == NULL) return false;
	//	타임값 확인
	if(p_CWar_Room->m_Play_Count > 1)
	{
		//	타임 감소
		p_CWar_Room->m_Play_Count--;
		//	카운트 감소 메시지 보냄.
		GF_ClanWar_Join_OK_Send(p_Idx,7,0);
		// 문 상태값 보냄
		GF_Set_ClanWar_Gate_NPC(NULL,p_Idx + 1,true);	// 문의 상태값 보냄
	}
	// 모든 유저를 주논으로 보냄
	else if(p_CWar_Room->m_Play_Count == 1)
	{
		//	타임 감소
		p_CWar_Room->m_Play_Count--;
		//	카운트 감소 메시지 보냄.
		GF_ClanWar_Join_OK_Send(p_Idx,7,0);
		//	해당 방에 있는 모든 유저 주논으로 보낸더.
		GF_ClanWar_Join_OK_Send(p_Idx,10,0);
	}
	else
	{
		////	해당 방에 있는 모든 유저 주논으로 보낸더.
		//GF_ClanWar_Join_OK_Send(p_Idx,10,0);
		//	게이트 닫음
		p_CWar_Room->m_Gate_TF = false;
		//	몬스터 모두 삭제.
		GF_ClanWar_Mob_Del2((LPVOID)p_CWar_Room,0);
		//	방을 초기화 한다.
		GF_Del_CWar_List2(p_Idx);
	}
	return true;
}

//#endif