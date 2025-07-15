/*---------------------------------------------------------------------------
Make:		2007.02.21
Name:		ClanWar.h
Author:     ����ö.����

Desc:		
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __CLANWAR_H__
#define __CLANWAR_H__

//---------------------------------------------------------------------------

#include "ZSTD.h"

//---------------------------------------------------------------------------
// (20070117:����ö) : Ŭ���� ��� �޽���
//---------------------------------------------------------------------------

enum CWAR_WARNING_MSGCODE
{
	CWAR_OPPONENT_MASTER_ALREADY_GIVEUP = 1,		// ��� Ŭ������ �̹� �������� ��
	CWAR_QUEST_LIST_FULL,							// Ŭ�� ��û�� ����Ʈ ����� FULL (����Ʈ ��� �� �ϳ��� Ŭ�������� ���)
	CWAR_CLAN_MASTER_LEVEL_RESTRICTED,				// Ŭ�� ������ �� Ŭ������ Ŭ���� ���� ���� ���� �޽���
	CWAR_TOO_MANY_CWAR_PROGRESS,					// Ŭ���� ���� �ο� ����
	CWAR_START_TIME_OUT,							// Ŭ���� ���Խð� 

	// 974 : [Ŭ����] ���� Ŭ�� �����Ͱ� ����Ͽ� Ŭ������ �¸��Ͽ����ϴ�.
	CWAR_OPPONENT_MASTER_GIVEUP,
	// 975 : [Ŭ����] Ŭ�� �����Ͱ� ����Ͽ� Ŭ������ �й��Ͽ����ϴ�.
	CWAR_MASTER_GIVEUP,
};

//---------------------------------------------------------------------------
// Ŭ���� �� ����
//---------------------------------------------------------------------------

struct stClanWarMOB
{
	DWORD dwMOB_IDX;		// ���� �ε���
	FLOAT fPosX;
	FLOAT fPosY;
	DWORD dwRegenNumber;	// ���� ��
	DWORD dwRegenRange;		// ���� ���� ����

	stClanWarMOB()
	{
		dwMOB_IDX = dwRegenNumber = dwRegenRange = 0;
		fPosX = fPosY = 0.f;
	}
};

//---------------------------------------------------------------------------
// Ŭ���� �¸� ���� ������
//---------------------------------------------------------------------------

struct stClanWarRewardItem
{
	DWORD dwItemNo;		// ������ ��ȣ
	DWORD dwItemDup;	// ������ �ߺ� ����

	stClanWarRewardItem()
	{
		dwItemNo = dwItemDup = 0;
	}
};

//---------------------------------------------------------------------------

#include "StdAfx.h"

#include "NET_Prototype.h"
#include "assert.h"
#include "DataTYPE.h"		// tPOINTF
#include "DLLIST.h"			// classDLLNODE
#include "ETC_Math.h"		// distance()
#include "classutil.h"
#include "classhash.h"		// t_HASHKEY
#include "CObjITEM.h"		// CObjITEM
#include "CObjNPC.h"		// CObjNPC

#include "UserDATA.h"
#include "classindex.h"
#include "GS_USER.h"
#include "ZoneLIST.h"


//	Ŭ���� DB ��� ����ü
struct GF_CWar_DB_ST
{
	DWORD	m_Server_ID;	// ���� ��ȣ
	DWORD	m_WarType;		// ���� Ÿ�� (1:A�� ������û, 2:B�� ���� ��û,3:��������,4:��������)
	DWORD	m_WarWin;		// ���� ��� (1:���º�,2:A�� ��,3:B�� ��)
	__int64	m_Money;		// ���� �ݾ�
	DWORD	m_ATeam_ID;		// A�� ��ȣ
	char	m_ATeam_Name[40];	// A�� �̸�
	DWORD	m_BTeam_ID;		// B�� ��ȣ
	char	m_BTeam_Name[40];	// B�� �̸�
	DWORD	m_Channel_ID;	// ä�� ID
	char*	m_pComment;		// ���� ������� ����
};
#include "GF_CW_DB_INIT.h"
#include "GF_CW_DB_PROC.h"


// Ŭ���� ���ο����� ���
#include "CClanMember.h"
#include "CClanFamily.h"
#include "CClanWarRoom.h"
#include "CClanWarRoomTemplate.h"

// GS �ڵ忡�� public ����� �����Ͽ� ���
#include "CClanWar.h"

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
