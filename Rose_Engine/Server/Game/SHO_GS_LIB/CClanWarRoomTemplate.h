/*---------------------------------------------------------------------------
Make:		2007.02.27
Name:		CClanWarRoomTemplate.h
Author:     남병철.레조

Desc:		클랜맵 파일을 로딩한다.
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __CCLANWARROOMTEMPLATE_H__
#define __CCLANWARROOMTEMPLATE_H__

//---------------------------------------------------------------------------

class CClanWarRoomTemplate
{
private: 
	// << 로딩 데이타 >>

	// 몇번째 맵 정보를 가진 클랜전인가?
	// Ex) MAP_FEDION_H.ini --> 100 ~ 생성가능한 번호까지(Ex 50) = 100 ~ 150
	DWORD m_dwZoneSeedNo;

	// 클랜전 신청을 받은 NPC
	CObjNPC*	m_pNPC;

	// 시작 위치 (tPOINT*)
	tPOINTF m_ptATeamStartPos;	// A 팀
	tPOINTF m_ptBTeamStartPos;	// B 팀

	// 부활 위치 (tPOINTF*)
	ZList m_zlATeamRevivalPos;	// A 팀
	ZList m_zlBTeamRevivalPos;	// B 팀

	// 기본 귀환 위치
	DWORD m_dwDefaultReturnZoneNo;
	tPOINTF m_ptDefaultReturnPos;

	// 클랜전 진행 시간
	DWORD m_dwGateTime;
	DWORD m_dwPlayTime;
	DWORD m_dwEndTime;

	// 클랜전 WarType별 레벨 제한 (DWORD*)
	ZList m_zlWarTypeRestrictLevel;

	// 클랜전 WarType별 배팅 금액 (INT64*)
	ZList m_zlWarTypeBatJuly;

	// 클랜전 팀당 최대 인원 수 (기본 : 15 (설정 최대값 : 50))
	DWORD m_dwMAX_ClanWarJoiningNumberPerTeam;

	// 클랜전 최대 수 (클랜전 인원 제한 50일 경우 25, 15일 경우 50) - 최대 접속자 2500명 제한 기준
	DWORD m_dwMAX_ClanWarNumber;

	// 클랜전 몬스터 설정 정보 ( stClanWarMOB* )
	ZList m_zlATeamWarTypeMOB;		// 워타입 리스트 (ZList*)
	ZList m_zlBTeamWarTypeMOB;

	// 클랜전 보상 아이템들
	// 클랜전 타입에 따른 보상 아이템 리스트 (ZList* 로보관하면 이 ZList는 stClanWarItem*이다)
	ZList m_zlRewardItems;


public:
	CClanWarRoomTemplate();
	~CClanWarRoomTemplate();

	// 방 청소! : 기존의 데이타 값을 초기화 한다.
	BOOL DataClear();

	// << 로딩 데이타 접근 >>

	// 몇번째 맵 정보를 가진 클랜전인가?
	BOOL SetZoneSeedNo( DWORD dwZoneSeedNo );
	DWORD GetZoneSeedNo();

	// 클랜전 NPC
	BOOL SetNPC( CObjNPC* pNpc );
	CObjNPC* GetNPC();

	// 시작 위치
	BOOL SetStartPos( FLOAT fATeamX, FLOAT fATeamY, FLOAT fBTeamX, FLOAT fBTeamY );
	tPOINTF GetATeamStartPos();
	tPOINTF GetBTeamStartPos();

	// 부활 위치
	BOOL AddATeamRevivalPos( FLOAT fPosX, FLOAT fPosY );
	BOOL AddBTeamRevivalPos( FLOAT fPosX, FLOAT fPosY );
	tPOINTF GetATeamShortestRevivalPos( FLOAT fDeadPosX, FLOAT fDeadPosY );
	tPOINTF GetBTeamShortestRevivalPos( FLOAT fDeadPosX, FLOAT fDeadPosY );

	// 귀환 위치
	BOOL SetDefaultReturnZoneNo( DWORD dwZone );
	DWORD GetDefaultReturnZoneNo();
	BOOL SetDefaultReturnPos( FLOAT fPosX, FLOAT fPosY );
	tPOINTF GetDefaultReturnPos();

	// 클랜전 게임 시간
	BOOL SetGateTime( DWORD dwGateTime );
	BOOL SetPlayTime( DWORD dwPlayTime );
	BOOL SetEndTime( DWORD dwEndTime );
	DWORD GetGateTime();
	DWORD GetPlayTime();
	DWORD GetEndTime();

	// 클랜전 WarType별 레벨 제한
	BOOL AddWarTypeRestrictLevel( DWORD dwRestrictLevel );
	DWORD GetWarTypeRestrictLevel( DWORD dwWarType );

	// 클랜전 WarType별 배팅 금액
	BOOL AddWarTypeBatJuly( INT64 i64BatJuly );
	INT64 GetWarTypeBatJuly( DWORD dwWarType );

	// 클랜전 팀당 최대 인원 수 (기본 : 15 (설정 최대값 : 50))
	BOOL SetMAX_ClanWarJoiningNumberPerTeam( DWORD dwMaxNumber );
	DWORD GetMAX_ClanWarJoiningNumberPerTeam();

	// 클랜전 최대 수 (클랜전 인원 제한 50일 경우 25, 15일 경우 50) - 최대 접속자 2500명 제한 기준
	BOOL SetMAX_ClanWarNumber( DWORD dwMaxNumber );
	DWORD GetMAX_ClanWarNumber();

	// 클랜전 몹 정보
	BOOL AddATeamMOBInfo( ZList* pzlMobData );
	BOOL AddBTeamMOBInfo( ZList* pzlMobData );
	ZList* GetATeamMOBInfo( DWORD dwWarType );
	ZList* GetBTeamMOBInfo( DWORD dwWarType );

	// 클랜전 아이템 정보
	BOOL AddWinningRewardItem( ZList* pzlItemData );
	ZList* GetWinningRewardItem( DWORD dwWarType );
};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
