/*---------------------------------------------------------------------------
Make:		2007.02.21
Name:		CClanWar.h
Author:     남병철.레조

Desc:		CClanWar 클래스
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __CCLANWAR_H__
#define __CCLANWAR_H__

//---------------------------------------------------------------------------
// BOOL CClanWar::SendProcessMessage( CClanWarRoom* pClanWarRoom, DWORD dwProcessType, DWORD dwWarpType = 0 );
// << 클랜전 진행 메시지 종류 >>
// 1 : 마스터에게 클랜전 참여 메시지 전달
// 2 : 클랜원들에게 클랜전 참여 메시지 전달
// 7 : 접속한 유저들에게 클랜전 진행 수치 상태 전달
//10 : 각자의 맵으로 보낸다. (기본:주논으로 워프)
enum CW_MSG_PROCESS_TYPE
{
	CWMPT_MASTER_MSG = 1,
	CWMPT_MEMBER_MSG = 2,
	CWMPT_TIME_MSG = 7,
	CWMPT_RETURN_WARP_MSG = 10
};

//---------------------------------------------------------------------------
// BOOL CClanWar::SendProcessMessage( CClanWarRoom* pClanWarRoom, DWORD dwProcessType, DWORD dwWarpType = 0 );
// << 클랜전 진행 워프 메시지 종류 >>
// 1 : A 팀 클랜원을 클랜전 맵으로 워프
// 2 : B 팀 클랜원을 클랜전 맵으로 워프
enum CW_MSG_WARP_TYPE
{
	CWMWT_NO_WARP = 0,
	CWMWT_A_TEAM_WARP = 1,
	CWMWT_B_TEAM_WARP = 2
};

//---------------------------------------------------------------------------
// BOOL classUSER::Send_gsv_CLANWAR_OK(DWORD p_Sub_Type,DWORD p_Clan_dw,WORD p_Zone_w,WORD p_Team_w);
// p_Sub_Type
// << 클랜전 진행 워프 대상 >>
// 1 : 마스터 워프
// 2 : 멤버 워프
enum CW_WARP_TYPE
{
	CWWT_NO_WARP = 0,
	CWWT_MASTER_WARP = 1,
	CWWT_MEMBER_WARP = 2
};

//---------------------------------------------------------------------------
// BOOL classUSER::Send_gsv_CLANWAR_OK(DWORD p_Sub_Type,DWORD p_Clan_dw,WORD p_Zone_w,WORD p_Team_w);
// p_Team_w
// << 클랜전 팀 구분 >>
// 0	: 모든 팀
// 1000 : A 팀
// 2000 : B 팀
enum CW_TEAM_TYPE
{
	CWTT_ALL = 0,
	CWTT_A_TEAM = 1000,
	CWTT_B_TEAM = 2000
};

//---------------------------------------------------------------------------
// BOOL Send_gsv_CLANWAR_CLIENT_PROGRESS( CClanWarRoom* pClanWarRoom, DWORD dwClientProgress );
// dwClinetProgress
// << 클랜전 클라이언트 진행 메시지 >>
// 1	: 클랜전 시작 메시지
// 2	: A 팀 승리 메시지
// 3	: B 팀 승리 메시지
// 4	: 무승부 메시지
enum CW_CLIENT_PROGRESS_MSG
{
	CWCPM_CLANWAR_START = 1,
	CWCPM_CLANWAR_A_TEAM_WIN = 2,
	CWCPM_CLANWAR_B_TEAM_WIN = 3,
	CWCPM_CLANWAR_TEAM_DRAW = 4
};

//---------------------------------------------------------------------------

class CClanWar : public ZMultiThreadSync< CClanWar >
{
private:
	//CRITICAL_SECTION	m_General_LOCK;
	//CRITICAL_SECTION	m_DB_LOCK;

	// 클랜전 방을 유형별로 초기화한 맵 데이타 (CClanWarRoomTemplate*)
	// (클랜전 생성시 초기화용으로 사용)
	ZList m_zlWarRoomTemplate;

	// 클랜전 진행 공간 (CClanWarRoom*)
	ZList m_zlWarRoom;

	// 클랜전에 참여할 클랜 리스트 (CClanFamily*)
	ZList m_zlClanFamily;

protected: 

	// << 크리티컬 섹션 내부에서 사용 되어야한다. >> //
	// Ex) BOOL ApplyClanWar( CObjNPC* pNPC, classUSER* pApplyUser, DWORD dwClientClanWarType );

	// NPC에 해당하는 맵 데이타 템플릿 or 템플릿 인덱스를 얻는다.
	CClanWarRoomTemplate* GetWarRoomTemplate( CObjNPC* pNPC );

	// 클랜전 진행 레벨 제한 채크
	// (NPC가 담당하는 클랜맵 세팅의 워타입별 레벨 제한에 부합하는가?)
	BOOL CheckUserLevel( CObjNPC* pNPC, DWORD dwWarType, DWORD dwUserLevel );

	// 클라이언트 클랜워 타입 -> 서버 클랜워 타입으로 변환
	DWORD ChangeClientToServerClanWarType( DWORD dwClientClanWarType );

	// 클랜전 예약 상태 채크
	CClanWarRoom* CheckWarRoomReservation( CObjNPC* pNPC, DWORD dwClanWarType );

	// 남은 방이 있는지 채크
	CClanWarRoom* CheckVacantWarRoom( CObjNPC* pNPC, DWORD& dwRoomIDX /*OUT*/ );

	// 이미 로딩된 패밀리가 있는가?
	CClanFamily* CheckTheSameFamily( classUSER* pMember );

	// 비어있는 패밀리 메모리가 있는가?
	// 패밀리 정보를 로딩해서 사용하다가 지금은 않쓰는(m_bIsUse==FALSE) 데이타
	// (20070427:남병철) 0427 이전 주석 : 사용하지 않는 패밀리 데이타가 있는가?
	// (20070427:남병철) 0427 이후 주석 : 사용하지 않는 패밀리의 일원이었는가?
	CClanFamily* CheckVacantFamilyMemory( classUSER* pMember );
	// A, B 팀 클랜 패밀리 세팅
	BOOL SetATeam( CClanWarRoom* pWarRoom, ZString zClanName );
	BOOL SetBTeam( CClanWarRoom* pWarRoom, ZString zClanName );

	// 클랜전 진행에 필요한 메시지 전달
	// dwProcessType [01:마스터에게 클랜전 참여 메시지 전달]
	//               [02:클랜원들에게 클랜전 참여 메시지 전달]
	//               [07:접속한 유저들에게 클랜전 진행 수치 상태 전달]
	//               [10:각자의 맵으로 보낸다. (기본:주논으로 워프)]
	// dwWarpType [1:A 팀 클랜원들에게 클랜전 참여 메시지 전달]
	//            [2:B 팀 클랜원들에게 클랜전 참여 메시지 전달]
	BOOL SendProcessMessage( CClanWarRoom* pClanWarRoom, DWORD dwProcessType, DWORD dwWarpType = 0 );

	// 클랜몹 리젠
	BOOL RegenMOB( CClanWarRoomTemplate* pClanWarRoomTemplate, CClanWarRoom* pClanWarRoom );

	// 리젠몹 제거 (ZoneNo, Team구분)
	// << 팀 구분 >>
	// CWTT_ALL = 0,
	// CWTT_A_TEAM = 1000,
	// CWTT_B_TEAM = 2000
	BOOL RemoveMOB( CClanWarRoom* pClanWarRoom, DWORD dwTeamType );

	// 승리 보상 세팅 (줄리, 아이템)
	BOOL WinningRewardJulyItemSetting( CClanWarRoom* pClanWarRoom, DWORD dwWinTeamType, BOOL bItemReward );
	// 마스터 클랜 변수 변경 (줄리, 아이템 보상)
	BOOL ChangeMasterClanValue( CClanFamily* pClanFamily, WORD wJuly, BOOL bRewardItem, DWORD dwWarType, DWORD dwNPC_NO = 0 );


	// 클랜전 마스터 워프
	BOOL Send_gsv_CLANWAR_MASTER_WARP( CClanWarRoom* pClanWarRoom, DWORD dwTeamType );
	// 클랜전 멤버 워프
	BOOL Send_gsv_CLANWAR_MEMBER_WARP( CClanWarRoom* pClanWarRoom, DWORD dwTeamType );
	// 모든 팀 멤버에게 클랜전 진행 시간 전달(시간 동기화용)
	BOOL Send_gsv_CLANWAR_TIME_STATUS( CClanWarRoom* pClanWarRoom );
	// 모든 팀 멤버에게 클랜전 복귀 메시지 전달
	BOOL Send_gsv_CLANWAR_RETURN_WARP( CClanWarRoom* pClanWarRoom );


	// 승리 보상 (퀘스트)
	BOOL WinningRewardQuest( CClanWarRoom* pClanWarRoom, DWORD dwTeamType );
	// 클랜전 참여팀의 마스터가 보유한 퀘스트 변경
	BOOL ChangeMasterQuest( CClanWarRoom* pClanWarRoom, DWORD dwTeamType, DWORD dwAddQuestIDX, DWORD dwRemoveQuestIDX );
	// 클랜전 퀘스트 삭제
	BOOL RemoveQuest( CClanMember* pClanMember, DWORD dwQuestIDX );
	// 클랜전 퀘스트 등록
	BOOL AddQuest( CClanMember* pClanMember, DWORD dwQuestIDX );
	// 클랜전 클라이언트 진행 메시지
	BOOL Send_gsv_CLANWAR_CLIENT_PROGRESS( CClanWarRoom* pClanWarRoom, DWORD dwClientProgress );


	// 랭킹 포인트 세팅
	BOOL SetRankPoint( CClanWarRoom* pClanWarRoom );
	// 랭킹 포인트 비율 얻기 (B 팀 랭킹 포인트 = 1.f - A 팀 랭킹 포인트)
	FLOAT GetATeamRankPointRate( DWORD dwATeamRankPoint, DWORD dwBTeamRankPoint );
	// 보상 포인트 세팅
	BOOL AddRewardPoint( CClanWarRoom* pClanWarRoom );


	// 문 상태 전송
	BOOL Send_gsv_GateStatus( CClanWarRoom* pClanWarRoom, classUSER* pUser, BOOL bIsGateOpen );
	// 팀 HP 리셋
	BOOL ResetTeamHP( CClanWarRoom* pClanWarRoom, DWORD dwTeamType );


	// 클랜워룸 가져오기
	CClanWarRoom* GetClanWarRoomByZoneNo( DWORD dwZoneNo );
	// 클랜워룸 가져오기
	CClanWarRoom* GetClanWarRoomByClanID( DWORD dwClanID );
	// 종료처리
	BOOL DropClanWar( CClanWarRoom* pClanWarRoom, DWORD dwDropTeamType );


	// 유저가 소속된 클랜을 리턴
	CClanFamily* GetFamily( classUSER* pUser );

	// 클랜전 진행중인 방 개수
	DWORD GetLiveClanWarRoom();


public:

	// << 타이머 함수 >>

	// 타임 처리 함수
	BOOL TimeProc();
		// 게임 진행 타임 감소
		BOOL PlayTime( CClanWarRoom* pClanWarRoom, CClanWarRoomTemplate* pClanWarRoomTemplate );
		// 게임 종료 타임 감소
		BOOL EndTime( CClanWarRoom* pClanWarRoom );


	// 승리 보상 (줄리, 아이템) - 클랜전 후 "승리의 징표" 퀘스트로 보상 받음
	BOOL WinningRewardJulyItem( classUSER* pRewardUser );
	// 클랜전 보스 몹인가?
	BOOL IsBoss( DWORD dwBossIDX );
	// 클랜전 진행중인 보스 HP 상태 전송
	BOOL NotifyBossHP( DWORD dwZoneNo );
	// 클랜전 생성된 존(룸)인가?
	BOOL IsCreatedClanWarRoom( DWORD dwZoneNo );
	DWORD IsCreatedClanWarRoom( classUSER* pUser );
	// 부활 위치 얻기
	BOOL GetRevivePos( classUSER* pUser, tPOINTF ptDeadPos, tPOINTF& ptRevivePos /*OUT*/ );
	// 클랜전이면 게이트 상태 전송
	BOOL CheckGateStatus( classUSER* pUser );
	// 클랜전 존에서 강제종료 되었으면 첫번째 클랜 맵에 설정하는 기본 복귀 위치에서 시작
	BOOL CheckDefaultReturnZone( classUSER* pUser /*IN,OUT*/ );
	// 유저가 어느팀 클랜전 소속인지 확인
	DWORD CheckClanWarUser( classUSER* pUser );
	// 클랜전 지역으로 이동
	DWORD WarpUserToClanWar( classUSER* pUser, DWORD dwTeamType );
	// 클랜 가입자인가?
	BOOL IsClanJoined( classUSER* pMember );
	// PK 모드 채크후 처리
	BOOL IsPKMode( CObjCHAR* pUser );

	// 클랜 정보를 리스트 표시
	// (아래의 크리티컬 섹션처리 함수 내에서만 사용할것)
	BOOL UpdateClanInfoList();

	// 클랜 진행 정보를 리스트에 표시
	// (아래의 크리티컬 섹션처리 함수 내에서만 사용할것)
	BOOL UpdateClanWarList( CClanWarRoom* pWarRoom, int i );




public: // Critical Section 처리
	CClanWar();
	~CClanWar();

	static CClanWar* GetInstance();


	// << 채크 및 노티파이 함수 >>

	// 보스가 죽었는가?
	BOOL IsBossKilled( CObjMOB* pMOB );


	BOOL ReturnBatJulyOnly( classUSER* pUser );

	// 클랜전 포기
	BOOL DeclareMasterGiveUp( classUSER* pUser );
	BOOL GiveUpClanWar( classUSER* pUser );

	// 클랜전 진행에 필요한 메시지 전달
	// dwProcessType [01:마스터에게 클랜전 참여 메시지 전달]
	//               [02:클랜원들에게 클랜전 참여 메시지 전달]
	//               [07:접속한 유저들에게 클랜전 진행 수치 상태 전달]
	//               [10:각자의 맵으로 보낸다. (기본:주논으로 워프)]
	// dwWarpType [1:A 팀 클랜원들에게 클랜전 참여 메시지 전달]
	//            [2:B 팀 클랜원들에게 클랜전 참여 메시지 전달]
	BOOL SendProcessMessage( classUSER* pUser, DWORD dwProcessType, DWORD dwWarpType = 0 );



public:
	
	// (20070509:남병철) : 패킷 반복 공격 채크를 사용할것이냐?
	DWORD m_dwUSE;
	DWORD m_dwElapsedInvalidateTime;
	DWORD m_dwInvalidateMAXCount;

	// (20070511:남병철) : 각 유저의 시간당 패킷 횟수
	DWORD m_dwElapsedRecvPacketMAXTime;
	// (20070514:남병철) : 패킷 초과 사용 워닝
	DWORD m_dwWarningPacketCount;
	// (20070514:남병철) : 패킷 초과 사용으로 디스커넥
	DWORD m_dwDisconnectPacketCount;



	// 초기화
	BOOL Init();
		BOOL InitWarTypeMOB( ZIniFile& zMapINI, int iTeamType/*0:A팀, 1:B팀*/, 
							 ZString zClanWarTypeName, CClanWarRoomTemplate* pClanWarRoomTemplate );
		BOOL InitWarTypeRewardItem( ZIniFile& zMapINI, ZString zClanWarTypeName, CClanWarRoomTemplate* pClanWarRoomTemplate );


	// 프로그램과 생명을 같이하므로 데이타 삭제 없음
	//BOOL DataClear();

	// 클랜 멤버 등록
	// bFirstAdd : 처음 접속이면
	BOOL AddClanMember( classUSER* pMember );
	// 클랜 멤버 정보 업데이트
	BOOL UpdateClanMember( classUSER* pMember );

	// 클랜전 신청
	BOOL ApplyClanWar( CObjNPC* pNPC, classUSER* pApplyUser, DWORD dwClientClanWarType );
	// 클랜전 신청 중인지 확인
	BOOL IsApplyClanWar( classUSER* pMember );

	// 유저 로그아웃
	BOOL LogOut( classUSER* pUser );

	// 클랜전 포기한 메시지 전달
	BOOL ClanWarRejoinGiveUp( classUSER* pUser, DWORD dwWinTeam );
};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

