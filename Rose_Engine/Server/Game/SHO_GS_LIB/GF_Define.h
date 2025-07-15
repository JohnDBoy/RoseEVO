
/*
	작성자 김영환
*/

//#ifdef __CLAN_WAR_SET			// 클랜전 추가 정의

#ifndef __GF_DEFINE_H__
#define __GF_DEFINE_H__

//	정의 값
#define	DF_MAX_USER_LIST	2500	// 게임 서버 최대 유저 리스트 개수
#define	DF_MAX_CLAN_LIST	2500	// 게임 서버 최대 클랜 등록 개수
#define	DF_MAX_CWAR_LIST	50		// 클랜전 방 리스트 개수
//#define	DF_MAX_CWAR_LIST	25		// 클랜전 방 리스트 개수
#define	DF_MAX_CWAR_TYPE	3		// 클랜전 배팅 금액 종류.
//#define	DF_MAX_CWAR_MOB		3		// 클랜전 필드 몹 개수
#define	DF_MAX_CWAR_MOB		5		// 클랜전 필드 몹 개수
#define	DF_CWAR_TEAM_N		2		// 클랜전 참여 팀 개수
#define	DF_CWAR_A_TEAM		1000	// 클랜전 A 팀 번호
#define	DF_CWAR_B_TEAM		2000	// 클랜전 B 팀 번호
#define DF_CWAR_WIN_ITEM_N	10		// 클랜전 보상 아이템 최대 개수

//#define DF_CWAR_PLAY_TIME	48//120		// 20분
//#define DF_CWAR_END_TIME	3 //12		// 2분

//	ini 스트링 크기
#define	DB_INI_STRING		30
#define DB_BUFFER			512

//
//#ifndef __MILEAGE_SKILL_USED
//#define __MILEAGE_SKILL_USED	// 마일리지 스킬 : DB, Userdata, 패킷 등등 관련 수정
//#endif
//
//#ifndef __CLAN_WAR_SET
//#define __CLAN_WAR_SET			// 클랜전 추가 정의
//#endif

#include <windows.h>
#include "stdAFX.h"
#include "F_Buffer.h"
#include "F_Thread.h"
#include "KDB_SQL.h"
#include "GS_ListUSER.h"
#include "CThreadGUILD.h"
#include "ZoneLIST.h"

//	클랜전 정보 구조체
struct	GF_ClanWar_Set
{
	CObjNPC		*m_CW_Npc;	/// 클랜전 NPC
	DWORD	m_Play_Type;	// 게임 진행 타입
	DWORD	m_War_Type;		// 게임 타입 종류에 금액 처리.
	DWORD	m_Play_Count;	// 1분을 6으로 설정한다[1 = 10초. ㅋㅋ] 20분은 20*6 = 120 이다.
	__int64 m_Bat_Coin;		// 배팅 금액.
	DWORD	m_Clan_A;		// A 팀	: 클랜 번호
	DWORD	m_Clan_B;		// B 팀 : 클랜 번호
	DWORD	m_Type_A;		// A 팀 타입	 처리
	DWORD	m_Type_B;		// B 팀 타입	 처리
	//	존 정보.
	WORD	m_ZoneNO;		// 존 번호
	WORD	m_Win_Team;		// 이긴팀 설정 : 0 = 없음, 1 = A팀, 2 = B팀
	tPOINTF m_APosWARP;		// 존 A팀 위치
	tPOINTF m_BPosWARP;		// 존 B팀 위치
	//	부활 포인트.
	tPOINTF m_APosREVIVAL;	// 존 A팀 부활 위치
	tPOINTF m_BPosREVIVAL;	// 존 B팀 부활 위치
	//	부가 기능
	DWORD	m_Count;		// 세부 카운트
	BOOL	m_Gate_TF;		// 게이트 처리 변수
	DWORD	m_ATeam_No;		// A 팀 접속 인원
	DWORD	m_BTeam_No;		// B 팀 접속 인원
	//	존의 몹 정보 처리
	CObjMOB	*m_ATeam_Mob[DF_MAX_CWAR_MOB];
	CObjMOB	*m_BTeam_Mob[DF_MAX_CWAR_MOB];

	// (20061230:남병철) : 클랜전 결과에 대한 보상 포인트 기록
	classUSER* m_pATeamMaster;	// A팀을 대표하는 마스터 or 부마스터
	classUSER* m_pBTeamMaster;	// B팀을 대표하는 마스터 or 부마스터
	DWORD m_dwATeamRewardPoint;	// A팀 보상 포인트
	DWORD m_dwBTeamRewardPoint;	// B팀 보상 포인트

	GF_ClanWar_Set()
	{
		m_pATeamMaster = NULL;
		m_pBTeamMaster = NULL;
		m_dwATeamRewardPoint = 0;
		m_dwBTeamRewardPoint = 0;
	}
};

//	클랜 정보 구조체
struct	GF_Clan_Info
{
	//	클랜 정보
	tag_MY_CLAN		m_CLAN;
	char	m_ClanNAME[24];		// 클랜 이름. 20바이트 사용.
	DWORD	m_User_IDX[50];		// 클랜유저
	DWORD	m_Sub_Master;		// 서브 마스터 처리 (기본 0)
};

//	지역 위치 구조체
struct	GF_World_Pos
{
	WORD	m_ZoneNO;	// 존 번호
	tPOINTF m_Pos;		// 존 위치
};

//	클랜전 지역 보스 몹 정보 구조체
struct	GF_ClanWar_MOB
{
	DWORD	m_MOB_Idx;	// 몬스터 인덱스
	float	m_Pos_X;	// 위치 1
	float	m_Pos_Y;	// 위치 2
	DWORD	m_Range;	// 범위
	DWORD	m_Number;	// 몬스터 개수
};

////	클랜전 DB 기록 구조체
//struct GF_CWar_DB_ST
//{
//	DWORD	m_Server_ID;	// 서버 번호
//	DWORD	m_WarType;		// 전투 타입 (1:A팀 전투신청, 2:B팀 전투 신청,3:전투시작,4:전투종료)
//	DWORD	m_WarWin;		// 전투 결과 (1:무승부,2:A팀 승,3:B팀 승)
//	__int64	m_Money;		// 배팅 금액
//	DWORD	m_ATeam_ID;		// A팀 번호
//	char	m_ATeam_Name[40];	// A팀 이름
//	DWORD	m_BTeam_ID;		// B팀 번호
//	char	m_BTeam_Name[40];	// B팀 이름
//	DWORD	m_Channel_ID;	// 채널 ID
//};

//	클랜전 보상 ITEM 리스트 구조체
struct	GF_CWar_Win_Gift
{
	//	아이템 번호
	DWORD	m_Item_No[DF_CWAR_WIN_ITEM_N];
	//	아이템 개수
	WORD	m_Item_Dup[DF_CWAR_WIN_ITEM_N];
};


#include "GF_CW_DB_INIT.h"
#include "GF_CW_DB_PROC.h"


#endif
