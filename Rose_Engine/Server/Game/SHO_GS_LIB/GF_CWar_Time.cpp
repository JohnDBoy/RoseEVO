
/*
	�ۼ��� �迵ȯ
*/

//#ifdef __CLAN_WAR_SET			// Ŭ���� �߰� ����


#include "GF_Define.h"
#include "GF_Data.h"


////	���� ���� Ÿ�� ����
BOOL	GF_CWar_Play_Time(GF_ClanWar_Set *p_CWar_Room,DWORD	p_Idx);

////	���� ���� Ÿ�� ����
BOOL	GF_CWar_End_Time(GF_ClanWar_Set *p_CWar_Room,DWORD	p_Idx);

////	Ÿ�� ó�� �Լ�
BOOL	GF_CWar_Time_Proc()
{
	//	10�ʿ� �ѹ��� ���´�.
	//MessageBox(NULL,"10�� ������ ���","Ȯ�� ����!",MB_OK);

	// 1) ������� ���� ã�´�.
	// 2) ���� ���� ���� Ȯ���ϰ� Ÿ�� ī��Ʈ ���� �Ѵ�.
	// 3) ������ ī��Ʈ�� ����Ѵ�.
	// 4) ���� �ܰ� ó�� �Ѵ�.
	// 5) Ÿ�� �ƿ� �Ǹ� ���� ���� ��� ��Ż ��Ų��.[����]
	// 6) �ش� ���� ���� ���ϰ� ���� �����. [�ٽ� ��� �����ϰ�]

	/*
		�ڵ� �ۼ�.
	*/
	//	Ŭ���� ���� ��ŭ �ݺ� �˻��Ѵ�.
	for(DWORD i = 0 ; i < DF_MAX_CWAR_LIST; i++)
	{
		//	���� ���� ������ �˻��Ѵ�.
		switch(g_ClanWar_List[i].m_Play_Type)
		{
			//	Ŭ���� ��ȯ ������ ���� �����̴�.
		case 2:
		case 3:
		case 4:
			{
				// ���� ���� ���� ���̴�
				GF_CWar_Play_Time(&g_ClanWar_List[i],i);
				//	���� �����߿� ���� �� HP ���� �Ѵ�.
				GF_ClanWar_Boss_MOB_HP(i+1);
				break;
			}
		case 5:
			{
				// ���� ���� �����
				GF_CWar_End_Time(&g_ClanWar_List[i],i);
				//	���� �����߿� ���� �� HP ���� �Ѵ�.
				GF_ClanWar_Boss_MOB_HP(i+1);
				break;
			}
		}
	}

	return true;
}

////	���� ���� Ÿ�� ����
BOOL	GF_CWar_Play_Time(GF_ClanWar_Set *p_CWar_Room,DWORD	p_Idx)
{
	if(p_CWar_Room == NULL) return false;
	//	Ÿ�Ӱ� Ȯ��
	if(p_CWar_Room->m_Play_Count > 0)
	{
		//	Ÿ�� ����
		p_CWar_Room->m_Play_Count--;
		//	ī��Ʈ ���� �޽��� ����.
		GF_ClanWar_Join_OK_Send(p_Idx,7,0);
		//	���� ���� �޽��� ����. ���ð� ó��.
		if(p_CWar_Room->m_Play_Count == (DF_CWAR_PLAY_TIME - DF_CWAR_WAIT_TIME))
		{
			//	���� ���� ó��
			GF_Send_CLANWAR_Progress(p_Idx + 1, 1);
			//	����Ʈ ����.
			p_CWar_Room->m_Gate_TF = true;
			GF_Set_ClanWar_Gate_NPC(NULL,p_Idx + 1,false);	// ���� ����.
		}
		else
			GF_Set_ClanWar_Gate_NPC(NULL,p_Idx + 1,true);	// ���� ���°� ����
	}
	else
	{
		//	���� ���� Ÿ������ ��ȯ
		p_CWar_Room->m_Play_Type = 5;
		p_CWar_Room->m_Play_Count = DF_CWAR_END_TIME;
		//	�������� �� ���� ����.
		GF_ClanWar_Join_OK_Send(p_Idx,7,0);

		////	����Ʈ �ݴ´�
		//p_CWar_Room->m_Gate_TF = false;
		//GF_Set_ClanWar_Gate_NPC(NULL,p_Idx + 1);
		
		////	���� ��� ����.
		//GF_ClanWar_Mob_Del2((LPVOID)p_CWar_Room,0);
		
		//	���� ���� ��彿.
		GF_Win_CWar_Set((LPVOID)p_CWar_Room,0,true);
		//	��� ��� ����Ʈ �ϴ�. ���� ����Ʈ ����.
		DWORD t_AM,t_BM,t_CM;
		switch(p_CWar_Room->m_War_Type)
		{
		case 0:
			//	A,B �� ������ ����Ʈ ����.
			t_AM = GF_Clan_Fine_List(p_CWar_Room->m_Clan_A);
			t_BM = GF_Clan_Fine_List(p_CWar_Room->m_Clan_B);
			if(t_AM > 0)
			{
				t_AM = t_AM - 1;	
				t_CM = GF_Clan_Master_Fine(t_AM);	//A �� ������ ã��
				if(t_CM > 0)
				{
					//	����Ʈ �����.
					GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
					//	���� ����Ʈ ���
					GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2817);
				}
				else
				{
					// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
					// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
					t_CM = GF_Clan_Master_Sub_Fine(t_AM);
					if(t_CM > 0)
					{
						//	����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2817);
					}
				}
			}
			if(t_BM > 0)
			{
				t_BM = t_BM - 1;	
				t_CM = GF_Clan_Master_Fine(t_BM);	//B �� ������ ã��
				if(t_CM > 0)
				{
					//	����Ʈ �����.
					GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2811);
					//	���� ����Ʈ ���
					GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2817);
				}
				else
				{
					// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
					// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
					t_CM = GF_Clan_Master_Sub_Fine(t_BM);
					if(t_CM > 0)
					{
						//	����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2817);
					}
				}
			}
			break;
		case 1:
			//	A,B �� ������ ����Ʈ ����.
			t_AM = GF_Clan_Fine_List(p_CWar_Room->m_Clan_A);
			t_BM = GF_Clan_Fine_List(p_CWar_Room->m_Clan_B);
			if(t_AM > 0)
			{
				t_AM = t_AM - 1;	
				t_CM = GF_Clan_Master_Fine(t_AM);	//A �� ������ ã��
				if(t_CM > 0)
				{
					//	����Ʈ �����.
					GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
					//	���� ����Ʈ ���
					GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2818);
				}
				else
				{
					// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
					// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
					t_CM = GF_Clan_Master_Sub_Fine(t_AM);
					if(t_CM > 0)
					{
						//	����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2818);
					}
				}
			}
			if(t_BM > 0)
			{
				t_BM = t_BM - 1;	
				t_CM = GF_Clan_Master_Fine(t_BM);	//B �� ������ ã��
				if(t_CM > 0)
				{
					//	����Ʈ �����.
					GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2812);
					//	���� ����Ʈ ���
					GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2818);
				}
				else
				{
					// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
					// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
					t_CM = GF_Clan_Master_Sub_Fine(t_BM);
					if(t_CM > 0)
					{
						//	����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2818);
					}
				}
			}
			break;
		case 2:
			//	A,B �� ������ ����Ʈ ����.
			t_AM = GF_Clan_Fine_List(p_CWar_Room->m_Clan_A);
			t_BM = GF_Clan_Fine_List(p_CWar_Room->m_Clan_B);
			if(t_AM > 0)
			{
				t_AM = t_AM - 1;	
				t_CM = GF_Clan_Master_Fine(t_AM);	//A �� ������ ã��
				if(t_CM > 0)
				{
					//	����Ʈ �����.
					GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
					//	���� ����Ʈ ���
					GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2819);
				}
				else
				{
					// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
					// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
					t_CM = GF_Clan_Master_Sub_Fine(t_AM);
					if(t_CM > 0)
					{
						//	����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2819);
					}
				}
			}
			if(t_BM > 0)
			{
				t_BM = t_BM - 1;	
				t_CM = GF_Clan_Master_Fine(t_BM);	//B �� ������ ã��
				if(t_CM > 0)
				{
					//	����Ʈ �����.
					GF_ClanWar_Quest_Del((LPVOID)g_User_List[t_CM - 1],2813);
					//	���� ����Ʈ ���
					GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2819);
				}
				else
				{
					// �̱� ���� Ŭ������ ����.. ��� ó���ұ�?
					// ���� ���� Ŭ�� ������ �� ó���Ѵ�.
					t_CM = GF_Clan_Master_Sub_Fine(t_BM);
					if(t_CM > 0)
					{
						//	����Ʈ ���
						GF_ClanWar_Quest_Add((LPVOID)g_User_List[t_CM - 1],2819);
					}
				}
			}
			break;
		}
		//	���� �¸� �޽��� ����
		GF_Send_CLANWAR_Progress(p_Idx + 1,4);

		//	PK ��� ���� ��Ų��.
		g_pZoneLIST->Set_PK_FLAG((short)p_CWar_Room->m_ZoneNO,false);

		//	��� ������ ü���� ȸ�� ��Ų��.
		GF_ClanWar_MOB_HP_ReSet((LPVOID)&p_CWar_Room,1);
		GF_ClanWar_MOB_HP_ReSet((LPVOID)&p_CWar_Room,2);
	}
	return true;
}

////	���� ���� Ÿ�� ����
BOOL	GF_CWar_End_Time(GF_ClanWar_Set *p_CWar_Room,DWORD p_Idx)
{
	if(p_CWar_Room == NULL) return false;
	//	Ÿ�Ӱ� Ȯ��
	if(p_CWar_Room->m_Play_Count > 1)
	{
		//	Ÿ�� ����
		p_CWar_Room->m_Play_Count--;
		//	ī��Ʈ ���� �޽��� ����.
		GF_ClanWar_Join_OK_Send(p_Idx,7,0);
		// �� ���°� ����
		GF_Set_ClanWar_Gate_NPC(NULL,p_Idx + 1,true);	// ���� ���°� ����
	}
	// ��� ������ �ֳ����� ����
	else if(p_CWar_Room->m_Play_Count == 1)
	{
		//	Ÿ�� ����
		p_CWar_Room->m_Play_Count--;
		//	ī��Ʈ ���� �޽��� ����.
		GF_ClanWar_Join_OK_Send(p_Idx,7,0);
		//	�ش� �濡 �ִ� ��� ���� �ֳ����� ������.
		GF_ClanWar_Join_OK_Send(p_Idx,10,0);
	}
	else
	{
		////	�ش� �濡 �ִ� ��� ���� �ֳ����� ������.
		//GF_ClanWar_Join_OK_Send(p_Idx,10,0);
		//	����Ʈ ����
		p_CWar_Room->m_Gate_TF = false;
		//	���� ��� ����.
		GF_ClanWar_Mob_Del2((LPVOID)p_CWar_Room,0);
		//	���� �ʱ�ȭ �Ѵ�.
		GF_Del_CWar_List2(p_Idx);
	}
	return true;
}

//#endif