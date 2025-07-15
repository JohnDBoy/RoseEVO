/*---------------------------------------------------------------------------
Make:		2007.02.21
Name:		ClanWar.h
Author:     남병철.레조

Desc:		
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __CLANWAR_H__
#define __CLANWAR_H__

//---------------------------------------------------------------------------

#include "ZSTD.h"

//---------------------------------------------------------------------------
// (20070117:남병철) : 클랜전 경고 메시지
//---------------------------------------------------------------------------

enum CWAR_WARNING_MSGCODE
{
	CWAR_OPPONENT_MASTER_ALREADY_GIVEUP = 1,		// 상대 클랜장이 이미 포기했을 때
	CWAR_QUEST_LIST_FULL,							// 클랜 신청시 퀘스트 목록이 FULL (퀘스트 목록 중 하나를 클랜전으로 사용)
	CWAR_CLAN_MASTER_LEVEL_RESTRICTED,				// 클랜 마스터 및 클랜원의 클랜전 참여 레벨 제한 메시지
	CWAR_TOO_MANY_CWAR_PROGRESS,					// 클랜전 참여 인원 제한
	CWAR_START_TIME_OUT,							// 클랜전 진입시간 

	// 974 : [클랜전] 상대방 클랜 마스터가 기권하여 클랜전에 승리하였습니다.
	CWAR_OPPONENT_MASTER_GIVEUP,
	// 975 : [클랜전] 클랜 마스터가 기권하여 클랜전에 패배하였습니다.
	CWAR_MASTER_GIVEUP,
};

//---------------------------------------------------------------------------
// 클랜전 몹 정보
//---------------------------------------------------------------------------

struct stClanWarMOB
{
	DWORD dwMOB_IDX;		// 몬스터 인덱스
	FLOAT fPosX;
	FLOAT fPosY;
	DWORD dwRegenNumber;	// 리젠 수
	DWORD dwRegenRange;		// 리젠 랜덤 범위

	stClanWarMOB()
	{
		dwMOB_IDX = dwRegenNumber = dwRegenRange = 0;
		fPosX = fPosY = 0.f;
	}
};

//---------------------------------------------------------------------------
// 클랜전 승리 보상 아이템
//---------------------------------------------------------------------------

struct stClanWarRewardItem
{
	DWORD dwItemNo;		// 아이템 번호
	DWORD dwItemDup;	// 아이템 중복 개수

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


//	클랜전 DB 기록 구조체
struct GF_CWar_DB_ST
{
	DWORD	m_Server_ID;	// 서버 번호
	DWORD	m_WarType;		// 전투 타입 (1:A팀 전투신청, 2:B팀 전투 신청,3:전투시작,4:전투종료)
	DWORD	m_WarWin;		// 전투 결과 (1:무승부,2:A팀 승,3:B팀 승)
	__int64	m_Money;		// 배팅 금액
	DWORD	m_ATeam_ID;		// A팀 번호
	char	m_ATeam_Name[40];	// A팀 이름
	DWORD	m_BTeam_ID;		// B팀 번호
	char	m_BTeam_Name[40];	// B팀 이름
	DWORD	m_Channel_ID;	// 채널 ID
	char*	m_pComment;		// 전투 결과에만 존재
};
#include "GF_CW_DB_INIT.h"
#include "GF_CW_DB_PROC.h"


// 클랜전 내부에서만 사용
#include "CClanMember.h"
#include "CClanFamily.h"
#include "CClanWarRoom.h"
#include "CClanWarRoomTemplate.h"

// GS 코드에서 public 멤버만 접근하여 사용
#include "CClanWar.h"

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
