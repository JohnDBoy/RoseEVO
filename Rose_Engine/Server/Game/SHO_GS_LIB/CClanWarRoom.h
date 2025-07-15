/*---------------------------------------------------------------------------
Make:		2007.02.21
Name:		CClanWarRoom.h
Author:     남병철.레조

Desc:		클랜전 방 데이타 클래스
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __CCLANWARROOM_H__
#define __CCLANWARROOM_H__

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------

// << 클랜전 진행 종류 >>
// 0 : 클랜전 진행 전
// 1 : 클랜전 신청 (첫번째 클랜)
// 2 : 클랜전 신청 (두번째 클랜)
// 3,4 : 클랜전 참여자들 해당 클랜 맵으로 전송
// 5 : 게임 종료 시간
enum CW_PLAY_TYPE 
{
	CWPT_BEFORE_CLANWAR = 0,
	CWPT_TEAM_A_APPLY,
	CWPT_TEAM_B_APPLY,
	CWPT_WARP_TEAM_A,
	CWPT_WARP_TEAM_B,
	CWPT_END_CLANWAR
};

//---------------------------------------------------------------------------

// << 클랜전 세부 구분 >>
// 0 : 1000 줄리전
// 1 : 100만 줄리전
// 2 : 1000만 줄리전
enum CW_WAR_TYPE
{
	CWWT_WAR_TYPE_A = 0,
	CWWT_WAR_TYPE_B,
	CWWT_WAR_TYPE_C,

	MAX_CWWT_WAR_TYPE
};

//---------------------------------------------------------------------------

// << 클랜전 결과 >>
// 0 : 무승부
// 1 : A 팀 승리
// 2 : B 팀 승리
enum CW_WAR_RESULT
{
	CWWR_WAR_RESULT_DRAW = 0,
	CWWR_WAR_RESULT_AWIN,
	CWWR_WAR_RESULT_BWIN
};

//---------------------------------------------------------------------------

class CClanWarRoom
{
private:
	// << 게임 진행 데이타 >>

	// 클랜전 진행중?
	BOOL m_bIsUse;

	// 기권한 팀
	BOOL m_bIsATeamGiveUp;
	BOOL m_bIsBTeamGiveUp;

	// 몇번째 맵 정보를 가진 클랜전인가?
	// Ex) MAP_FEDION_H.ini --> 100 ~ 생성가능한 번호까지(Ex 50) = 100 ~ 150
	DWORD m_dwZoneSeedNo;

	// 클랜전 신청을 받은 NPC
	CObjNPC*	m_pNPC;

	// << 클랜전 진행 종류 >>
	// 0 : 클랜전 진행 전
	// 1 : 클랜전 신청 (첫번째 클랜)
	// 2 : 클랜전 신청 (두번째 클랜)
	// 3,4 : 클랜전 참여자들 해당 클랜 맵으로 전송
	// 5 : 게임 종료 시간
	DWORD m_dwPlayType;

	// << 클랜전 세부 구분 >>
	// 0 : 1000 줄리전
	// 1 : 100만 줄리전
	// 2 : 1000만 줄리전
	DWORD m_dwWarType;

	// 현재 게임 남은 시간 ( 1 = 10초, 1분 = 6 ) Ex) 20분 = 20 * 6 = 120
	DWORD m_dwElapsedPlayTime;

	// 배팅 금액
	INT64 m_i64BatJuly;

	// 클랜전 존 번호
	DWORD m_dwZoneNo;
	tPOINTF m_ptATeamZoneWarpPos;
	tPOINTF m_ptBTeamZoneWarpPos;

	// 기본 귀환 위치
	DWORD m_dwDefaultReturnZoneNo;
	tPOINTF m_ptDefaultReturnPos;

	// << 클랜전 결과 >>
	// 0 : 무승부
	// 1 : A 팀 승리
	// 2 : B 팀 승리
	DWORD m_dwClanWarResult;

	// 드레곤 HP 갱신 시간 및 갱신시 HP
	DWORD m_dwBossHPUpdateTime;
	DWORD m_dwATeamBossHP;
	DWORD m_dwBTeamBossHP;

	// 클랜전 게이트 열림/닫힘 상태
	BOOL m_bGateOpen;

	// A, B 팀 클랜 포인터
	CClanFamily* m_pATeam;
	CClanFamily* m_pBTeam;
	// 신청한 클라이언트에서 전달된 클랜워 타입
	DWORD m_dwATeamWarType;
	DWORD m_dwBTeamWarType;
    
	// A, B 팀 접속 인원 - 사용 않할듯... ㅡ.ㅡ 구현하고 지워라!
	//DWORD m_dwATeamMemberCount;
	//DWORD m_dwBTeamMemberCount;

	// 맵에 리젠된 몹 (CObjMOB*)
	ZList m_zlATeamMOB;
	ZList m_zlBTeamMOB;

	// (20070523:남병철) : 클랜전 보상 포인트 등 결과 정보
	ZString m_zClanWarResult;

public:
	CClanWarRoom();
	~CClanWarRoom();

	// 방 청소! : 기존의 데이타 값을 초기화 한다.
	BOOL DataClear();
	
	// << 게임 진행 데이타 접근 >>

	// 클랜워 룸의 사용 여부 Set/확인
	BOOL SetUseFlag( BOOL bUse );
	BOOL IsUse();

	// 기권한 팀(마스터)
	BOOL SetTeamGiveUp( classUSER* pMaster );
	BOOL GetATeamGiveUp();
	BOOL GetBTeamGiveUp();


	// 몇번째 맵 정보를 가진 클랜전인가?
	BOOL SetZoneSeedNo( DWORD dwRoomTemplateID );
	DWORD GetZoneSeedNo();

	// 클랜전 NPC
	BOOL SetNPC( CObjNPC* pNpc );
	CObjNPC* GetNPC();

	// << 클랜전 진행 종류 >>
	// 0 : 클랜전 진행 전
	// 1 : 클랜전 신청 (첫번째 클랜)
	// 2 : 클랜전 신청 (두번째 클랜)
	// 3,4 : 클랜전 참여자들 해당 클랜 맵으로 전송
	// 5 : 게임 종료 시간
	BOOL SetPlayType( DWORD dwPlayType );
	DWORD GetPlayType();

	// << 클랜워 타입 >>
	// 0 : 1000 줄리전
	// 1 : 100만 줄리전
	// 2 : 1000만 줄리전
	BOOL SetWarType( DWORD dwWarType );
	DWORD GetWarType();

	// 플레이 시간 상수 (클랜전 진행 상태에 따라 다르다.)
	BOOL SetElapsedPlayTime( DWORD dwElapsedPlayTime );
	DWORD GetElapsedPlayTime();

	// 줄리 배팅
	BOOL SetBatJuly( INT64 i64BatJuly );
	INT64 GetBatJuly();

	// 클랜전 존 번호
	BOOL SetZoneNo( DWORD dwZoneNo );
	DWORD GetZoneNo();

	// 클랜전 워프 위치
	BOOL SetATeamWarpPos( FLOAT fPosX, FLOAT fPosY );
	tPOINTF GetATeamWarpPos();
	BOOL SetBTeamWarpPos( FLOAT fPosX, FLOAT fPosY );
	tPOINTF GetBTeamWarpPos();

	// 귀환 위치
	BOOL SetDefaultReturnZoneNo( DWORD dwZoneNo );
	DWORD GetDefaultReturnZoneNo();
	BOOL SetDefaultReturnPos( FLOAT fPosX, FLOAT fPosY );
	tPOINTF GetDefaultReturnPos();

	// << 클랜전 결과 >>
	// 0 : 무승부
	// 1 : A 팀 승리
	// 2 : B 팀 승리
	BOOL SetWarResult( DWORD dwWarResult );
	DWORD GetWarResult();

	// 드레곤 HP 갱신 시간 및 갱신시 HP
	BOOL SetBossHPUpdateTime( DWORD dwUpdateTime );
	DWORD GetBossHPUpdateTime();
	BOOL SetATeamBossHP( DWORD dwBossHP );
	DWORD GetATeamBossHP();
	BOOL SetBTeamBossHP( DWORD dwBossHP );
	DWORD GetBTeamBossHP();

	// 클랜전 게이트 열림/닫힘 상태
	BOOL SetGateFlag( BOOL bGateOpen );
	BOOL GetGateFlag();

	// A, B 팀 클랜 포인터
	BOOL SetATeam( CClanFamily* pClanFamily );
	BOOL SetBTeam( CClanFamily* pClanFamily );
	CClanFamily* GetATeam();
	CClanFamily* GetBTeam();

	// 신청한 클라이언트에서 전달된 클랜워 타입
	BOOL SetATeamWarType( DWORD dwWarType );
	BOOL SetBTeamWarType( DWORD dwWarType );
	DWORD GetATeamWarType();
	DWORD GetBTeamWarType();

	// 최상위 등급자 리턴 (기본:클랜 마스터)
	CClanMember* GetATeamTopGradeMember( BOOL bCheckClanWarZone = FALSE );
	CClanMember* GetBTeamTopGradeMember( BOOL bCheckClanWarZone = FALSE );
	// A,B팀 클랜원 수
	DWORD GetATeamMemberCount( BOOL bCheckClanWarZone = FALSE );
	DWORD GetBTeamMemberCount( BOOL bCheckClanWarZone = FALSE );

	// 맵에 리젠된 몹 (CObjMOB*)
	BOOL AddATeamMOB( CObjMOB* pMOB );
	BOOL AddBTeamMOB( CObjMOB* pMOB );
	ZList* GetATeamMOBList();
	ZList* GetBTeamMOBList();

	// 몹 데이타 초기화
	BOOL ClearATeamMOB();
	BOOL ClearBTeamMOB();
	BOOL ClearAllTeamMOB();

	// 어느팀 멤버인지 구별
	BOOL IsATeamMember( classUSER* pMember );
	BOOL IsBTeamMember( classUSER* pMember );

	// 현 시점에 클랜전 존에 있는 유저 리스트를 스트링으로 작성한다.
	ZString GetATeamClanWarJoinList();
	ZString GetBTeamClanWarJoinList();

	// 클랜전 진입 타임아웃 (클라이언트에 떠있는 클랜전 참가관련 메시지창을 닫게한다.)
	BOOL StartTimeOut();

	// 클랜전 포기한 메시지 전달
	BOOL ClanWarRejoinGiveUp( DWORD dwWinTeam );

	// 보상 포인트 현황 zClanWarResult에 기록
	ZString& ClanWarResult();
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
