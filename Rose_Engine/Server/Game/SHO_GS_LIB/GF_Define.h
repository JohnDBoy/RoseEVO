
/*
	�ۼ��� �迵ȯ
*/

//#ifdef __CLAN_WAR_SET			// Ŭ���� �߰� ����

#ifndef __GF_DEFINE_H__
#define __GF_DEFINE_H__

//	���� ��
#define	DF_MAX_USER_LIST	2500	// ���� ���� �ִ� ���� ����Ʈ ����
#define	DF_MAX_CLAN_LIST	2500	// ���� ���� �ִ� Ŭ�� ��� ����
#define	DF_MAX_CWAR_LIST	50		// Ŭ���� �� ����Ʈ ����
//#define	DF_MAX_CWAR_LIST	25		// Ŭ���� �� ����Ʈ ����
#define	DF_MAX_CWAR_TYPE	3		// Ŭ���� ���� �ݾ� ����.
//#define	DF_MAX_CWAR_MOB		3		// Ŭ���� �ʵ� �� ����
#define	DF_MAX_CWAR_MOB		5		// Ŭ���� �ʵ� �� ����
#define	DF_CWAR_TEAM_N		2		// Ŭ���� ���� �� ����
#define	DF_CWAR_A_TEAM		1000	// Ŭ���� A �� ��ȣ
#define	DF_CWAR_B_TEAM		2000	// Ŭ���� B �� ��ȣ
#define DF_CWAR_WIN_ITEM_N	10		// Ŭ���� ���� ������ �ִ� ����

//#define DF_CWAR_PLAY_TIME	48//120		// 20��
//#define DF_CWAR_END_TIME	3 //12		// 2��

//	ini ��Ʈ�� ũ��
#define	DB_INI_STRING		30
#define DB_BUFFER			512

//
//#ifndef __MILEAGE_SKILL_USED
//#define __MILEAGE_SKILL_USED	// ���ϸ��� ��ų : DB, Userdata, ��Ŷ ��� ���� ����
//#endif
//
//#ifndef __CLAN_WAR_SET
//#define __CLAN_WAR_SET			// Ŭ���� �߰� ����
//#endif

#include <windows.h>
#include "stdAFX.h"
#include "F_Buffer.h"
#include "F_Thread.h"
#include "KDB_SQL.h"
#include "GS_ListUSER.h"
#include "CThreadGUILD.h"
#include "ZoneLIST.h"

//	Ŭ���� ���� ����ü
struct	GF_ClanWar_Set
{
	CObjNPC		*m_CW_Npc;	/// Ŭ���� NPC
	DWORD	m_Play_Type;	// ���� ���� Ÿ��
	DWORD	m_War_Type;		// ���� Ÿ�� ������ �ݾ� ó��.
	DWORD	m_Play_Count;	// 1���� 6���� �����Ѵ�[1 = 10��. ����] 20���� 20*6 = 120 �̴�.
	__int64 m_Bat_Coin;		// ���� �ݾ�.
	DWORD	m_Clan_A;		// A ��	: Ŭ�� ��ȣ
	DWORD	m_Clan_B;		// B �� : Ŭ�� ��ȣ
	DWORD	m_Type_A;		// A �� Ÿ��	 ó��
	DWORD	m_Type_B;		// B �� Ÿ��	 ó��
	//	�� ����.
	WORD	m_ZoneNO;		// �� ��ȣ
	WORD	m_Win_Team;		// �̱��� ���� : 0 = ����, 1 = A��, 2 = B��
	tPOINTF m_APosWARP;		// �� A�� ��ġ
	tPOINTF m_BPosWARP;		// �� B�� ��ġ
	//	��Ȱ ����Ʈ.
	tPOINTF m_APosREVIVAL;	// �� A�� ��Ȱ ��ġ
	tPOINTF m_BPosREVIVAL;	// �� B�� ��Ȱ ��ġ
	//	�ΰ� ���
	DWORD	m_Count;		// ���� ī��Ʈ
	BOOL	m_Gate_TF;		// ����Ʈ ó�� ����
	DWORD	m_ATeam_No;		// A �� ���� �ο�
	DWORD	m_BTeam_No;		// B �� ���� �ο�
	//	���� �� ���� ó��
	CObjMOB	*m_ATeam_Mob[DF_MAX_CWAR_MOB];
	CObjMOB	*m_BTeam_Mob[DF_MAX_CWAR_MOB];

	// (20061230:����ö) : Ŭ���� ����� ���� ���� ����Ʈ ���
	classUSER* m_pATeamMaster;	// A���� ��ǥ�ϴ� ������ or �θ�����
	classUSER* m_pBTeamMaster;	// B���� ��ǥ�ϴ� ������ or �θ�����
	DWORD m_dwATeamRewardPoint;	// A�� ���� ����Ʈ
	DWORD m_dwBTeamRewardPoint;	// B�� ���� ����Ʈ

	GF_ClanWar_Set()
	{
		m_pATeamMaster = NULL;
		m_pBTeamMaster = NULL;
		m_dwATeamRewardPoint = 0;
		m_dwBTeamRewardPoint = 0;
	}
};

//	Ŭ�� ���� ����ü
struct	GF_Clan_Info
{
	//	Ŭ�� ����
	tag_MY_CLAN		m_CLAN;
	char	m_ClanNAME[24];		// Ŭ�� �̸�. 20����Ʈ ���.
	DWORD	m_User_IDX[50];		// Ŭ������
	DWORD	m_Sub_Master;		// ���� ������ ó�� (�⺻ 0)
};

//	���� ��ġ ����ü
struct	GF_World_Pos
{
	WORD	m_ZoneNO;	// �� ��ȣ
	tPOINTF m_Pos;		// �� ��ġ
};

//	Ŭ���� ���� ���� �� ���� ����ü
struct	GF_ClanWar_MOB
{
	DWORD	m_MOB_Idx;	// ���� �ε���
	float	m_Pos_X;	// ��ġ 1
	float	m_Pos_Y;	// ��ġ 2
	DWORD	m_Range;	// ����
	DWORD	m_Number;	// ���� ����
};

////	Ŭ���� DB ��� ����ü
//struct GF_CWar_DB_ST
//{
//	DWORD	m_Server_ID;	// ���� ��ȣ
//	DWORD	m_WarType;		// ���� Ÿ�� (1:A�� ������û, 2:B�� ���� ��û,3:��������,4:��������)
//	DWORD	m_WarWin;		// ���� ��� (1:���º�,2:A�� ��,3:B�� ��)
//	__int64	m_Money;		// ���� �ݾ�
//	DWORD	m_ATeam_ID;		// A�� ��ȣ
//	char	m_ATeam_Name[40];	// A�� �̸�
//	DWORD	m_BTeam_ID;		// B�� ��ȣ
//	char	m_BTeam_Name[40];	// B�� �̸�
//	DWORD	m_Channel_ID;	// ä�� ID
//};

//	Ŭ���� ���� ITEM ����Ʈ ����ü
struct	GF_CWar_Win_Gift
{
	//	������ ��ȣ
	DWORD	m_Item_No[DF_CWAR_WIN_ITEM_N];
	//	������ ����
	WORD	m_Item_Dup[DF_CWAR_WIN_ITEM_N];
};


#include "GF_CW_DB_INIT.h"
#include "GF_CW_DB_PROC.h"


#endif
