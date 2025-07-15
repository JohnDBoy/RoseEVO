//---------------------------------------------------------------------------

#include "ClanWar.h"

// (20070427:남병철) : 클랜 접속 리스트 추가용 (cpp에만 사용)
#include "GF_Define.h"
#include "F_GDI_List.h"
#include "GF_Data.h"

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// CClanWar
//---------------------------------------------------------------------------

CClanWar::CClanWar()
{
	//	크리티컬 세션 생성
	//InitializeCriticalSection( &m_General_LOCK );
	//InitializeCriticalSection( &m_DB_LOCK );
}

//---------------------------------------------------------------------------

CClanWar::~CClanWar()
{
	int iCount = 0;
	CClanFamily* pClanFamily = NULL;
	CClanWarRoom* pClanWarRoom = NULL;
	CClanWarRoomTemplate* pClanWarRoomTemplate = NULL;



	// 클랜전 방을 유형별로 초기화한 맵 데이타 (CClanWarRoomTemplate*)
	// (클랜전 생성시 초기화 용으로 사용)
	iCount = m_zlWarRoomTemplate.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanWarRoomTemplate = (CClanWarRoomTemplate*)m_zlWarRoomTemplate.IndexOf(i)) != NULL )
		{
			SAFE_DELETE( pClanWarRoomTemplate );
		}
	}
	m_zlWarRoomTemplate.Clear();

	// 클랜전 진행 공간 (CClanWarRoom*)
	iCount = m_zlWarRoom.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanWarRoom = (CClanWarRoom*)m_zlWarRoom.IndexOf(i)) != NULL )
		{
			SAFE_DELETE( pClanWarRoom );
		}
	}
	m_zlWarRoom.Clear();

	// 클랜전에 참여할 클랜 리스트 (CClanFamily*)
	iCount = m_zlClanFamily.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanFamily = (CClanFamily*)m_zlClanFamily.IndexOf(i)) != NULL )
		{
			SAFE_DELETE( pClanFamily );
		}
	}
	m_zlClanFamily.Clear();

	//	크리티컬 세션 삭제
	//DeleteCriticalSection( &m_General_LOCK );
	//DeleteCriticalSection( &m_DB_LOCK );
}

//---------------------------------------------------------------------------

CClanWar* CClanWar::GetInstance()
{
	static CClanWar m_Instance;

	return &m_Instance;
}

//---------------------------------------------------------------------------
// 타임 처리 함수 (10초에 한번씩 들어온다.)
//---------------------------------------------------------------------------

BOOL CClanWar::TimeProc()
{
	// 1) 만들어진 방을 찾는다.
	// 2) 게임 진행 변수 확인하고 타임 카운트 감소 한다.
	// 3) 지나간 카운트를 출력한다.
	// 4) 다음 단계 처리 한다.
	// 5) 타임 아웃 되면 참여 유저 모두 포탈 시킨다.[강제]
	// 6) 해당 방의 몹을 정하고 방을 지운다. [다시 사용 가능하게]

	int iCount = 0;
	CClanWarRoom* pClanWarRoom = NULL;
	CClanWarRoomTemplate* pClanWarRoomTemplate = NULL;


	ZString zTemp;
	ZStringList zslPRoomNo; // 진행중인 클랜전 방 번호
	ZStringList zslPRoomFamilyMemberCount;
	ZStringList zslERoomNo; // 종료중인 클랜전 방 번호
	ZStringList zslERoomFamilyMemberCount;

	// 동기화
	//EnterCriticalSection( &m_General_LOCK );

	iCount = m_zlWarRoom.Count();
	for( int i = 0; i < iCount; i++ )
	{
		// 게임 중인 방인지 검사한다.
		if( (pClanWarRoom = (CClanWarRoom*)m_zlWarRoom.IndexOf(i)) != NULL )
		{
			switch( pClanWarRoom->GetPlayType() )
			{
			case CWPT_TEAM_B_APPLY:
			case CWPT_WARP_TEAM_A:
			case CWPT_WARP_TEAM_B:

#ifdef __USE_ZLOG__
				// 방번호
				zTemp.Format( "[PLAY] - TIME(%d sec) RoomNO(%d) ZoneNO(%d) BatJuly(%d)", pClanWarRoom->GetElapsedPlayTime()*10, i, pClanWarRoom->GetZoneNo(), pClanWarRoom->GetBatJuly() );
				zslPRoomNo.Add( zTemp );
				// 팀 별 인원 구성
				zTemp.Format( "[PLAY] - A Team(WAR MEMBER:%d), B Team(WAR MEMBER:%d)", 
							  pClanWarRoom->GetATeamMemberCount( TRUE ), 
							  pClanWarRoom->GetBTeamMemberCount( TRUE ) );
				zslPRoomFamilyMemberCount.Add( zTemp );
#endif

				// 게임 시작 중인 방
				pClanWarRoomTemplate = GetWarRoomTemplate( pClanWarRoom->GetNPC() );
				PlayTime( pClanWarRoom, pClanWarRoomTemplate );
				// 게임 진행중에 보스 몹 HP 전달 한다.
				NotifyBossHP( pClanWarRoom->GetZoneNo() );
				break;

			case CWPT_END_CLANWAR:

#ifdef __USE_ZLOG__
				// 방번호
				zTemp.Format( "[END] - TIME(%d sec) RoomNO(%d) ZoneNO(%d) BatJuly(%d)", pClanWarRoom->GetElapsedPlayTime()*10, i, pClanWarRoom->GetZoneNo(), pClanWarRoom->GetBatJuly() );
				zslERoomNo.Add( zTemp );
				// 팀 별 인원 구성
				zTemp.Format( "[END] - A Team(WAR MEMBER:%d), B Team(WAR MEMBER:%d)", 
							  pClanWarRoom->GetATeamMemberCount( TRUE ), 
							  pClanWarRoom->GetBTeamMemberCount( TRUE ) );
				zslERoomFamilyMemberCount.Add( zTemp );
#endif

				// 게임 종료 대기중
				EndTime( pClanWarRoom );
				// 게임 진행중에 보스 몹 HP 전달 한다.
				NotifyBossHP( pClanWarRoom->GetZoneNo() );
				break;
			}

			// 10초에 한번씩 클랜전 관련 정보 업데이트
			// 10초에 한번 모든 클랜전 정보를 업데이트 하므로 각 클랜전이 서로 중복되어 업데이트 될 일은 없다.
			//UpdateClanWarList( pClanWarRoom, i );
		}
	}

#ifdef __USE_ZLOG__

	// 클랜전 진행중인 방 정보
	iCount = zslPRoomNo.Count();
	if( iCount > 0 )
		g_WatchData.Log( "<< 클랜전 진행중... >>" );
	for( int i = 0; i < iCount; i++ )
	{
		g_WatchData.Log( zslPRoomNo.IndexOf(i).c_str() );
		g_WatchData.Log( zslPRoomFamilyMemberCount.IndexOf(i).c_str() );
	}

	// 클랜전 종료중인 방 정보
	iCount = zslERoomNo.Count();
	if( iCount > 0 )
		g_WatchData.Log( "<< 클랜전 종료중... >>" );
	for( int i = 0; i < iCount; i++ )
	{
		g_WatchData.Log( zslERoomNo.IndexOf(i).c_str() );
		g_WatchData.Log( zslERoomFamilyMemberCount.IndexOf(i).c_str() );
	}

	zslPRoomNo.Clear();
	zslPRoomFamilyMemberCount.Clear();
	zslERoomNo.Clear();
	zslERoomFamilyMemberCount.Clear();

#endif

	//LeaveCriticalSection( &m_General_LOCK );

	return TRUE;
}

//---------------------------------------------------------------------------
// 게임 진행 타임 감소
//---------------------------------------------------------------------------

BOOL CClanWar::PlayTime( CClanWarRoom* pClanWarRoom, CClanWarRoomTemplate* pClanWarRoomTemplate )
{
	DWORD dwElapsedTime = 0;
	DWORD dwPlayTime = 0, dwGateTime = 0, dwEndTime = 0;


	// INI에 설정된 클랜전 진행 시간 구성
	if( pClanWarRoomTemplate )
	{
		dwGateTime = pClanWarRoomTemplate->GetGateTime();
		dwPlayTime = pClanWarRoomTemplate->GetPlayTime();
		dwEndTime = pClanWarRoomTemplate->GetEndTime();
	}
	else
	{
		dwGateTime = 12;
		dwPlayTime = 120;
		dwEndTime = 12;
	}


	// 클랜전 진행
	dwElapsedTime = pClanWarRoom->GetElapsedPlayTime();
	if( dwElapsedTime > 0 )
	{
		// 타임 감소
		dwElapsedTime--;
		// 카운트 감소 메시지
		SendProcessMessage( pClanWarRoom, CWMPT_TIME_MSG, CWMWT_NO_WARP );
		pClanWarRoom->SetElapsedPlayTime( dwElapsedTime );

		// 게임 시작 메시지 보냄 대기시간 처리
		if( dwElapsedTime == (dwPlayTime - dwGateTime) )
		{
			// (20070417:남병철) : 게이트 열리기전 기권승 채크
			// 게이트 열릴때 아무도 없으면 바로 끊음
			// (A,B팀 모두 불참하면 역시 아무도 없으니.. 맵상의 숫자만 채크해도 모두 기권일 경우가 처리됨)
			if( pClanWarRoom->GetATeamMemberCount( TRUE ) <= 0 &&
				pClanWarRoom->GetBTeamMemberCount( TRUE ) <= 0 )
			{
				CClanMember* pClanMember = NULL;
				classUSER* pClanUser = NULL;

				if( (pClanMember = pClanWarRoom->GetATeamTopGradeMember( TRUE )) != NULL )
				{
					if( (pClanUser = pClanMember->GetMember()) != NULL )
					{
						g_LOG.CS_ODS( 0xffff, "\n[BREAK!] A TEAM INFO (A:%d, B:%d) ZONE(%d) WAR_TYPE(%d) CLANID(%d):%s [GRADE(%d)]TopMember(%s):%s",
							pClanWarRoom->GetATeamMemberCount(TRUE), pClanWarRoom->GetBTeamMemberCount(TRUE),
							pClanWarRoom->GetZoneNo(), pClanWarRoom->GetWarType(), pClanUser->GetClanID(), pClanUser->GetClanNAME(),
							pClanMember->GetGrade(), pClanUser->Get_ACCOUNT(), pClanUser->Get_NAME() );
					}
				}

				if( (pClanMember = pClanWarRoom->GetBTeamTopGradeMember( TRUE )) != NULL )
				{
					if( (pClanUser = pClanMember->GetMember()) != NULL )
					{
						g_LOG.CS_ODS( 0xffff, "\n[BREAK!] B TEAM INFO (A:%d, B:%d) ZONE(%d) WAR_TYPE(%d) CLANID(%d):%s [GRADE(%d)]TopMember(%s):%s",
							pClanWarRoom->GetATeamMemberCount(TRUE), pClanWarRoom->GetBTeamMemberCount(TRUE),
							pClanWarRoom->GetZoneNo(), pClanWarRoom->GetWarType(), pClanUser->GetClanID(), pClanUser->GetClanNAME(),
							pClanMember->GetGrade(), pClanUser->Get_ACCOUNT(), pClanUser->Get_NAME() );
					}
				}



				// 몹 제거
				RemoveMOB( pClanWarRoom, CWTT_ALL );
				// PK 모드 오프
				g_pZoneLIST->Set_PK_FLAG( (short)pClanWarRoom->GetZoneNo(), FALSE );
				// 모든 팀원의 체력을 회복
				ResetTeamHP( pClanWarRoom, CWTT_ALL );
				// 게임 종료 설정
				pClanWarRoom->SetPlayType( CWPT_END_CLANWAR );
				pClanWarRoom->SetElapsedPlayTime( 0 );

				// 클라이언트 클랜전 참가 요청창 닫기
				pClanWarRoom->StartTimeOut();
			}
			else if( pClanWarRoom->GetBTeamGiveUp() || pClanWarRoom->GetBTeamMemberCount( TRUE ) <= 0 )
			{ // A팀 승리 (기권승)
				CClanMember* pClanMember = NULL;
				classUSER* pClanUser = NULL;

				if( (pClanMember = pClanWarRoom->GetATeamTopGradeMember( TRUE )) != NULL )
				{
					if( (pClanUser = pClanMember->GetMember()) != NULL )
					{
						g_LOG.CS_ODS( 0xffff, "\n[A WIN] (A:%d, B:%d) ZONE(%d) WAR_TYPE(%d) CLANID(%d):%s [GRADE(%d)]TopMember(%s):%s",
							pClanWarRoom->GetATeamMemberCount(TRUE), pClanWarRoom->GetBTeamMemberCount(TRUE),
							pClanWarRoom->GetZoneNo(), pClanWarRoom->GetWarType(), pClanUser->GetClanID(), pClanUser->GetClanNAME(),
							pClanMember->GetGrade(), pClanUser->Get_ACCOUNT(), pClanUser->Get_NAME() );
					}
				}



				// 클랜전 중 상대편이 불참
				pClanWarRoom->SetWarResult(	CWWR_WAR_RESULT_AWIN );
				DropClanWar( pClanWarRoom, CWTT_B_TEAM );
				WinningRewardQuest( pClanWarRoom, CWTT_A_TEAM );

				// 게임 종료 설정
				pClanWarRoom->SetPlayType( CWPT_END_CLANWAR );
				pClanWarRoom->SetElapsedPlayTime( dwEndTime );

				// 클라이언트 클랜전 참가 요청창 닫기
				pClanWarRoom->StartTimeOut();
			}
			else if( pClanWarRoom->GetATeamGiveUp() || pClanWarRoom->GetATeamMemberCount( TRUE ) <= 0 )
			{ // B팀 승리 (기권승)
				CClanMember* pClanMember = NULL;
				classUSER* pClanUser = NULL;

				if( (pClanMember = pClanWarRoom->GetBTeamTopGradeMember( TRUE )) != NULL )
				{
					if( (pClanUser = pClanMember->GetMember()) != NULL )
					{
						g_LOG.CS_ODS( 0xffff, "\n[B WIN] (A:%d, B:%d) ZONE(%d) WAR_TYPE(%d) CLANID(%d):%s [GRADE(%d)]TopMember(%s):%s",
							pClanWarRoom->GetATeamMemberCount(TRUE), pClanWarRoom->GetBTeamMemberCount(TRUE),
							pClanWarRoom->GetZoneNo(), pClanWarRoom->GetWarType(), pClanUser->GetClanID(), pClanUser->GetClanNAME(),
							pClanMember->GetGrade(), pClanUser->Get_ACCOUNT(), pClanUser->Get_NAME() );
					}
				}



				// 클랜전 중 상대편이 불참
				pClanWarRoom->SetWarResult(	CWWR_WAR_RESULT_BWIN );
				DropClanWar( pClanWarRoom, CWTT_A_TEAM );
				WinningRewardQuest( pClanWarRoom, CWTT_B_TEAM );

				// 게임 종료 설정
				pClanWarRoom->SetPlayType( CWPT_END_CLANWAR );
				pClanWarRoom->SetElapsedPlayTime( dwEndTime );

				// 클라이언트 클랜전 참가 요청창 닫기
				pClanWarRoom->StartTimeOut();
			}
			else
			{
				// 게임 시작 처리
				Send_gsv_CLANWAR_CLIENT_PROGRESS( pClanWarRoom, CWCPM_CLANWAR_START );
			}

			// 게이트 열림
			pClanWarRoom->SetGateFlag( TRUE );
			Send_gsv_GateStatus( pClanWarRoom, NULL, FALSE );
		}
		else
			Send_gsv_GateStatus( pClanWarRoom, NULL, TRUE );
	}
	// 클랜전 시간 모두 지남 (무승부!)
	else
	{
		CClanMember* pClanMember = NULL;
		classUSER* pClanUser = NULL;

		if( (pClanMember = pClanWarRoom->GetATeamTopGradeMember( TRUE )) != NULL )
		{
			if( (pClanUser = pClanMember->GetMember()) != NULL )
			{
				g_LOG.CS_ODS( 0xffff, "\n[DRAW] A TEAM INFO (A:%d, B:%d) ZONE(%d) WAR_TYPE(%d) CLANID(%d):%s [GRADE(%d)]TopMember(%s):%s",
					pClanWarRoom->GetATeamMemberCount(TRUE), pClanWarRoom->GetBTeamMemberCount(TRUE),
					pClanWarRoom->GetZoneNo(), pClanWarRoom->GetWarType(), pClanUser->GetClanID(), pClanUser->GetClanNAME(),
					pClanMember->GetGrade(), pClanUser->Get_ACCOUNT(), pClanUser->Get_NAME() );
			}
		}

		if( (pClanMember = pClanWarRoom->GetBTeamTopGradeMember( TRUE )) != NULL )
		{
			if( (pClanUser = pClanMember->GetMember()) != NULL )
			{
				g_LOG.CS_ODS( 0xffff, "\n[DRAW] B TEAM INFO (A:%d, B:%d) ZONE(%d) WAR_TYPE(%d) CLANID(%d):%s [GRADE(%d)]TopMember(%s):%s",
					pClanWarRoom->GetATeamMemberCount(TRUE), pClanWarRoom->GetBTeamMemberCount(TRUE),
					pClanWarRoom->GetZoneNo(), pClanWarRoom->GetWarType(), pClanUser->GetClanID(), pClanUser->GetClanNAME(),
					pClanMember->GetGrade(), pClanUser->Get_ACCOUNT(), pClanUser->Get_NAME() );
			}
		}

		// 게임 종료 설정
        pClanWarRoom->SetPlayType( CWPT_END_CLANWAR );
		pClanWarRoom->SetElapsedPlayTime( dwEndTime );
		// 유저에게 방상태 전송
		SendProcessMessage( pClanWarRoom, CWMPT_TIME_MSG, CWMWT_NO_WARP );

		// 무승부 처리
		switch( pClanWarRoom->GetWarType() )
		{
		case CWWT_WAR_TYPE_A:
			ChangeMasterQuest( pClanWarRoom, CWTT_ALL, 2817, 2811 );
			break;

		case CWWT_WAR_TYPE_B:
			ChangeMasterQuest( pClanWarRoom, CWTT_ALL, 2818, 2812 );
			break;

		case CWWT_WAR_TYPE_C:
			ChangeMasterQuest( pClanWarRoom, CWTT_ALL, 2819, 2813 );
			break;

		default:;
		}
		pClanWarRoom->SetWarResult(	CWWR_WAR_RESULT_DRAW );
		DropClanWar( pClanWarRoom, CWTT_ALL );
		// 클라이언트 클랜전 참가 요청창 닫기
		pClanWarRoom->StartTimeOut();

		// 동점 승리 메시지
		Send_gsv_CLANWAR_CLIENT_PROGRESS( pClanWarRoom, CWCPM_CLANWAR_TEAM_DRAW );

		// 몹 제거
		RemoveMOB( pClanWarRoom, CWTT_ALL );

		// PK 모드 오프
		g_pZoneLIST->Set_PK_FLAG( (short)pClanWarRoom->GetZoneNo(), FALSE );

		// 모든 팀원의 체력을 회복
		ResetTeamHP( pClanWarRoom, CWTT_ALL );
	}
	
	return TRUE;
}

//---------------------------------------------------------------------------
// 게임 종료 타임 감소
//---------------------------------------------------------------------------

BOOL CClanWar::EndTime( CClanWarRoom* pClanWarRoom )
{
	CObjNPC* pNPC = NULL;
	DWORD dwElapsedTime = 0;

	dwElapsedTime = pClanWarRoom->GetElapsedPlayTime();

	// 타임값 확인
	if( dwElapsedTime > 1 )
	{
		// 타임 감소
		dwElapsedTime--;
		// 카운트 감소 메시지 보냄
		SendProcessMessage( pClanWarRoom, CWMPT_TIME_MSG, CWMWT_NO_WARP );
		// 문 상태값
		Send_gsv_GateStatus( pClanWarRoom, NULL, TRUE );	
		// 타임 세팅
		pClanWarRoom->SetElapsedPlayTime( dwElapsedTime );
	}
	// 모든 유저를 주논으로 보냄
	else if( dwElapsedTime == 1 )
	{
		// 타임 감소
		dwElapsedTime--;
		// 몹 제거
		RemoveMOB( pClanWarRoom, CWTT_ALL );
		// 카운트 감소 메시지 보냄
		SendProcessMessage( pClanWarRoom, CWMPT_TIME_MSG, CWMWT_NO_WARP );
		// 해당 클랜전 존의 모든 유저를 주논으로 워프
		SendProcessMessage( pClanWarRoom, CWMPT_RETURN_WARP_MSG, CWMWT_NO_WARP );
		// 타임 세팅
		pClanWarRoom->SetElapsedPlayTime( dwElapsedTime );
	}
	// 게임 종료
	else
	{
		// 해당 클랜전 존의 모든 유저를 주논으로 워프
		//SendProcessMessage( pClanWarRoom, CWMPT_RETURN_WARP_MSG, CWMWT_NO_WARP );
		g_pZoneLIST->Set_PK_FLAG( (short)pClanWarRoom->GetZoneNo(), TRUE );
		// 몬스터 제거 -> 않되네.. -_-;
		//RemoveMOB( pClanWarRoom, CWTT_ALL );
		// 방 초기화
		if( (pNPC = pClanWarRoom->GetNPC()) != NULL )
		{
			pNPC->Set_ObjVAR( 0, 0 );
			pNPC->Set_ObjVAR( 1, 0 );
			pNPC->Set_ObjVAR( 2, 0 );
			pNPC->Set_ObjVAR( 3, 0 );
		}
		pClanWarRoom->DataClear();
	}

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// PK 모드 채크후 처리
//---------------------------------------------------------------------------

BOOL CClanWar::IsPKMode( CObjCHAR* pUser )
{
	int iTeamNo = 0;
	CZoneTHREAD* pZoneThread = NULL;


	//EnterCriticalSection( &m_General_LOCK );

	iTeamNo = pUser->Get_TEAM();
	if( iTeamNo == 1000 || iTeamNo == 2000 )
	{
		if( (pZoneThread = pUser->GetZONE()) != NULL )
		{
			if( pZoneThread->Get_PK_FLAG() == FALSE )
			{
				// AI 행동 금지
				pUser->SetCMD_STOP();
				//LeaveCriticalSection( &m_General_LOCK );
				return FALSE;
			}
		}

	}

	//LeaveCriticalSection( &m_General_LOCK );

	return TRUE;
}

//---------------------------------------------------------------------------
// 클랜전 보스 몹인가?
//---------------------------------------------------------------------------

BOOL CClanWar::IsBoss( DWORD dwBossIDX )
{
	int iCount = 0;
	ZList* pMobList = NULL;
	stClanWarMOB* pClanWarMOB = NULL;
	CClanWarRoomTemplate* pClanWarRoomTemplate = NULL;


	iCount = m_zlWarRoomTemplate.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanWarRoomTemplate = (CClanWarRoomTemplate*)m_zlWarRoomTemplate.IndexOf(i)) != NULL )
		{
			for( int j = 0; j < MAX_CWWT_WAR_TYPE; j++ )
			{
				// A 팀 보스몹
				if( (pMobList = pClanWarRoomTemplate->GetATeamMOBInfo(j)) != NULL )
				{
					if( (pClanWarMOB = (stClanWarMOB*)pMobList->First()) != NULL )
					{
						if( pClanWarMOB->dwMOB_IDX == dwBossIDX )
						{
							return TRUE;
						}
					}
				}

				// B 팀 보스몹
				if( (pMobList = pClanWarRoomTemplate->GetBTeamMOBInfo(j)) != NULL )
				{
					if( (pClanWarMOB = (stClanWarMOB*)pMobList->First()) != NULL )
					{
						if( pClanWarMOB->dwMOB_IDX == dwBossIDX )
						{
							return TRUE;
						}
					}
				}
			}
		}
	}
	
	return FALSE;
}

//---------------------------------------------------------------------------
// 보스가 죽었는가?
//---------------------------------------------------------------------------

BOOL CClanWar::IsBossKilled( CObjMOB* pKilledMOB )
{
	//ZThreadSync Sync;

	DWORD dwMobIdx = 0;
	DWORD dwZoneNo = 0;
	ZList* pAMobList = NULL;
	ZList* pBMobList = NULL;
	CObjMOB* pRegenMOB = NULL;
	CClanWarRoom* pClanWarRoom = NULL;


	//EnterCriticalSection( &m_General_LOCK );

	// 클랜전 보스인가?
	dwMobIdx = pKilledMOB->Get_CharNO();
	//dwMobIdx = pKilledMOB->Get_INDEX();
	if( IsBoss( dwMobIdx ) == FALSE )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}


	if( pKilledMOB->GetZONE() )
	{
		// 클랜워룸 가져오기
		dwZoneNo = pKilledMOB->GetZONE()->Get_ZoneNO();
		if( (pClanWarRoom = GetClanWarRoomByZoneNo(dwZoneNo)) == NULL )
		{
			//LeaveCriticalSection( &m_General_LOCK );
			return FALSE;
		}

		// A 팀 리젠 몹 리스트 가져오기
		pAMobList = pClanWarRoom->GetATeamMOBList();
		if( pAMobList )
		{
			// 첫번째 몹이 보스이다.
			pRegenMOB = (CObjMOB*)pAMobList->First();

			// 죽은 몹이 보스몹과 같은가?
			if( pRegenMOB == pKilledMOB )
			{
				// (20070423:남병철) : 클랜전 로그 추가
				CClanMember* pClanMember = NULL;
				classUSER* pClanUser = NULL;
				if( (pClanMember = pClanWarRoom->GetBTeamTopGradeMember( TRUE )) != NULL )
				{
					if( (pClanUser = pClanMember->GetMember()) != NULL )
					{
						g_LOG.CS_ODS( 0xffff, "\n[B WIN] (A:%d, B:%d) ZONE(%d) WAR_TYPE(%d) CLANID(%d):%s [GRADE(%d)]TopMember(%s):%s",
									  pClanWarRoom->GetATeamMemberCount(TRUE), pClanWarRoom->GetBTeamMemberCount(TRUE),
									  pClanWarRoom->GetZoneNo(), pClanWarRoom->GetWarType(), pClanUser->GetClanID(), pClanUser->GetClanNAME(),
									  pClanMember->GetGrade(), pClanUser->Get_ACCOUNT(), pClanUser->Get_NAME() );
					}
				}

				// 클랜전 중 보스가 죽으면 클랜전 결과를 세팅하고 모든 팀을 드랍한다.
				pClanWarRoom->SetWarResult(	CWWR_WAR_RESULT_BWIN );
				DropClanWar( pClanWarRoom, CWTT_ALL );
				WinningRewardQuest( pClanWarRoom, CWTT_B_TEAM );
				// 클라이언트 클랜전 참가 요청창 닫기
				pClanWarRoom->StartTimeOut();

				//LeaveCriticalSection( &m_General_LOCK );
				return TRUE;
			}
		}

        // B 팀 리젠 몹 리스트 가져오기
		pBMobList = pClanWarRoom->GetBTeamMOBList();
		if( pBMobList )
		{
			// 첫번째 몹이 보스이다.
			pRegenMOB = (CObjMOB*)pBMobList->First();

			// 죽은 몹이 보스몹과 같은가?
			if( pRegenMOB == pKilledMOB )
			{
				CClanMember* pClanMember = NULL;
				classUSER* pClanUser = NULL;
				if( (pClanMember = pClanWarRoom->GetBTeamTopGradeMember( TRUE )) != NULL )
				{
					if( (pClanUser = pClanMember->GetMember()) != NULL )
					{
						g_LOG.CS_ODS( 0xffff, "\n[A WIN] (A:%d, B:%d) ZONE(%d) WAR_TYPE(%d) CLANID(%d):%s [GRADE(%d)]TopMember(%s):%s",
							pClanWarRoom->GetATeamMemberCount(TRUE), pClanWarRoom->GetBTeamMemberCount(TRUE),
							pClanWarRoom->GetZoneNo(), pClanWarRoom->GetWarType(), pClanUser->GetClanID(), pClanUser->GetClanNAME(),
							pClanMember->GetGrade(), pClanUser->Get_ACCOUNT(), pClanUser->Get_NAME() );
					}
				}

				// 클랜전 중 보스가 죽으면 클랜전 결과를 세팅하고 모든 팀을 드랍한다.
				pClanWarRoom->SetWarResult(	CWWR_WAR_RESULT_AWIN );
				DropClanWar( pClanWarRoom, CWTT_ALL );
				WinningRewardQuest( pClanWarRoom, CWTT_A_TEAM );
				// 클라이언트 클랜전 참가 요청창 닫기
				pClanWarRoom->StartTimeOut();

				//LeaveCriticalSection( &m_General_LOCK );
				return TRUE;
			}
		}
	}

	//LeaveCriticalSection( &m_General_LOCK );

	return FALSE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 진행중인 보스 HP 상태 전송
//---------------------------------------------------------------------------

BOOL CClanWar::NotifyBossHP( DWORD dwZoneNo )
{
	BOOL bCheckSendTiming = FALSE;
	DWORD dwCurTick = 0;
	CClanWarRoom* pClanWarRoom = NULL;


	//EnterCriticalSection( &m_General_LOCK );

	// 보스인가? (클랜전 진행중인 방이면 채크 어차피 최소 시간갭은 1500(1.5초)이므로)
	// 클랜전 진행중인 방일경우 pClanWarRoom 획득이 성공한다.
	if( (pClanWarRoom = GetClanWarRoomByZoneNo( dwZoneNo )) == NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}

	// 클랜전 진행중인가?
	dwCurTick = GetTickCount();
	if( pClanWarRoom->GetPlayType() == CWPT_WARP_TEAM_B )
	{
		if( pClanWarRoom->GetBossHPUpdateTime() < dwCurTick )
		{
			if( (dwCurTick - pClanWarRoom->GetBossHPUpdateTime()) > 1500 )
			{
				// HP 전송! OK~
				bCheckSendTiming = TRUE;

				// 현재 채크 타이밍 저장
				pClanWarRoom->SetBossHPUpdateTime( dwCurTick );
			}
		}
		else
		{
			// 타이머가 회전한 경우
			bCheckSendTiming = TRUE;
			
			// 현재 채크 타이밍 저장
			pClanWarRoom->SetBossHPUpdateTime( dwCurTick );
		}
	}

	// 게임이 종료된 경우도 보냄
	if( pClanWarRoom->GetPlayType() == CWPT_END_CLANWAR )
	{
		bCheckSendTiming = TRUE;
	}


	if( bCheckSendTiming )
	{
		// 클랜전 존에 있는 모든 유저에게 보스 HP양을 전송
		char szSendBuffer[512];
		ZeroMemory( szSendBuffer, 512 );
		gsv_cli_CLAN_WAR_BOSS_HP* pSendData = NULL;

		pSendData = (gsv_cli_CLAN_WAR_BOSS_HP*)szSendBuffer;
		pSendData->m_wType = GSV_CLANWAR_BOSS_HP;
		pSendData->m_nSize = sizeof(gsv_cli_CLAN_WAR_BOSS_HP);


		// << 각 상태 세팅 >>

		// 게임이 종료된 경우
		if( pClanWarRoom->GetPlayType() == CWPT_END_CLANWAR )
		{
			pSendData->m_A_nCharIdx = 0;
			pSendData->m_A_Boss_Max_HP = 0;
			pSendData->m_A_Boss_HP = 0;

			pSendData->m_B_nCharIdx = 0;
			pSendData->m_B_Boss_Max_HP = 0;
			pSendData->m_B_Boss_HP = 0;
		}
		else
		{
			ZList* pAMobList = NULL;
			ZList* pBMobList = NULL;
			CObjMOB* pRegenMOB = NULL;

			// A 팀 리젠 몹 리스트 가져오기
			if( (pAMobList = pClanWarRoom->GetATeamMOBList()) != NULL )
			{
				// 첫번째 몹이 보스이다.
				pRegenMOB = (CObjMOB*)pAMobList->First();
				if( pRegenMOB == NULL )
				{
					pSendData->m_A_nCharIdx = 0;
					pSendData->m_A_Boss_Max_HP = 0;
					pSendData->m_A_Boss_HP = 0;
				}
				else
				{
					pSendData->m_A_nCharIdx = pRegenMOB->Get_CharNO();
					pSendData->m_A_Boss_Max_HP = pRegenMOB->Get_MaxHP();
					pSendData->m_A_Boss_HP = pRegenMOB->Get_HP();
				}
			}

			// B 팀 리젠 몹 리스트 가져오기
			if( (pBMobList = pClanWarRoom->GetBTeamMOBList()) != NULL )
			{
				// 첫번째 몹이 보스이다.
				pRegenMOB = (CObjMOB*)pBMobList->First();
				if( pRegenMOB == NULL )
				{
					pSendData->m_B_nCharIdx = 0;
					pSendData->m_B_Boss_Max_HP = 0;
					pSendData->m_B_Boss_HP = 0;
				}
				else
				{
					pSendData->m_B_nCharIdx = pRegenMOB->Get_CharNO();
					pSendData->m_B_Boss_Max_HP = pRegenMOB->Get_MaxHP();
					pSendData->m_B_Boss_HP = pRegenMOB->Get_HP();
				}
			}
		}
		// 해당 존에 메시지 전달
		g_pZoneLIST->Send_gsv_All_User_Data( (short)dwZoneNo, (char*)pSendData );
	}

	//LeaveCriticalSection( &m_General_LOCK );

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 생성된 존(룸)인가?
//---------------------------------------------------------------------------

BOOL CClanWar::IsCreatedClanWarRoom( DWORD dwZoneNo )
{
	//EnterCriticalSection( &m_General_LOCK );

	// 생성된 클랜워룸인가?
	if( GetClanWarRoomByZoneNo( dwZoneNo ) != NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return TRUE;
	}

	//LeaveCriticalSection( &m_General_LOCK );

	return FALSE;
}

//---------------------------------------------------------------------------

DWORD CClanWar::IsCreatedClanWarRoom( classUSER* pUser )
{
	DWORD dwReturnValue = 999;
	DWORD dwRestrictLevel = 0;
	CClanWarRoom* pClanWarRoom = NULL;
	CClanWarRoomTemplate* pClanWarRoomTemplate = NULL;


	//EnterCriticalSection( &m_General_LOCK );

	if( (pClanWarRoom = GetClanWarRoomByClanID( pUser->GetClanID() )) == NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return dwReturnValue;
	}

	if( (pClanWarRoomTemplate = GetWarRoomTemplate( pClanWarRoom->GetNPC() )) == NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return dwReturnValue;
	}

	dwRestrictLevel = pClanWarRoomTemplate->GetWarTypeRestrictLevel( pClanWarRoom->GetWarType() );

	// 클랜전 진행중
	if( pClanWarRoom->GetPlayType() == CWPT_TEAM_B_APPLY || 
		pClanWarRoom->GetPlayType() == CWPT_WARP_TEAM_A || 
		pClanWarRoom->GetPlayType() == CWPT_WARP_TEAM_B )
	{
		if( pClanWarRoom->IsATeamMember( pUser ) )
		{
			if( !pClanWarRoom->GetATeamGiveUp() )
			{
				// 사용자 레벨이 제한 레벨 보다 크면
				if( (DWORD)pUser->Get_LEVEL() > dwRestrictLevel )
					// 레벨 제한 입장 불가
					dwReturnValue = CWTT_ALL;
				else
					dwReturnValue = CWTT_A_TEAM;
			}
		}
		else if( pClanWarRoom->IsBTeamMember( pUser ) )
		{
			if( !pClanWarRoom->GetBTeamGiveUp() )
			{
				// 사용자 레벨이 제한 레벨 보다 크면
				if( (DWORD)pUser->Get_LEVEL() > dwRestrictLevel )
					// 레벨 제한 입장 불가
					dwReturnValue = CWTT_ALL;
				else
					dwReturnValue = CWTT_B_TEAM;
			}
		}
	}

	//LeaveCriticalSection( &m_General_LOCK );

	return dwReturnValue;
}

//---------------------------------------------------------------------------

BOOL CClanWar::ReturnBatJulyOnly( classUSER* pUser )
{
	//ZThreadSync Sync;

	CClanWarRoom* pClanWarRoom = NULL;
	CClanWarRoomTemplate* pClanWarRoomTemplate = NULL;


	//EnterCriticalSection( &m_General_LOCK );

	if( (pClanWarRoom = GetClanWarRoomByZoneNo( pUser->m_nZoneNO )) == NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}

	if( (pClanWarRoomTemplate = GetWarRoomTemplate( pClanWarRoom->GetNPC() )) == NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}

    
	switch( pUser->m_Quests.m_nClanWarVAR[0] )
	{
	case 1:
	case 2:
		pUser->m_Inventory.m_i64Money = pUser->m_Inventory.m_i64Money + pClanWarRoomTemplate->GetWarTypeBatJuly( CWWT_WAR_TYPE_A );
		pUser->Send_gsv_SET_MONEYONLY( GSV_SET_MONEY_ONLY );
		break;
	case 31:
	case 32:
		pUser->m_Inventory.m_i64Money = pUser->m_Inventory.m_i64Money + pClanWarRoomTemplate->GetWarTypeBatJuly( CWWT_WAR_TYPE_B );
		pUser->Send_gsv_SET_MONEYONLY( GSV_SET_MONEY_ONLY );
		break;
	case 61:
	case 62:
		pUser->m_Inventory.m_i64Money = pUser->m_Inventory.m_i64Money + pClanWarRoomTemplate->GetWarTypeBatJuly( CWWT_WAR_TYPE_C );
		pUser->Send_gsv_SET_MONEYONLY( GSV_SET_MONEY_ONLY );
		break;
	}

	//	관련변수 초기화
	pUser->m_Quests.m_nClanWarVAR[0] = 0;	// 클랜전 신청 타입 (1000줄리... 등등)
	pUser->m_Quests.m_nClanWarVAR[6] = 0;	// 채널 번호
	pUser->m_Quests.m_nClanWarVAR[5] = 0;	// 보상 금액
	pUser->m_Quests.m_nClanWarVAR[7] = 0;

	////	수정된 유저 퀘스트 변수 내용 전달
	pUser->Send_gsv_Quest_VAR();

	//LeaveCriticalSection( &m_General_LOCK );

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전이면 게이트 상태 전송
//---------------------------------------------------------------------------

BOOL CClanWar::CheckGateStatus( classUSER* pUser )
{
	//ZThreadSync Sync;

	DWORD dwZoneNo = 0;
	CZoneTHREAD* pZoneThread = NULL;
	CClanWarRoom* pClanWarRoom = NULL;


	//EnterCriticalSection( &m_General_LOCK );

	if( (pZoneThread = pUser->GetZONE()) != NULL )
	{
		dwZoneNo = pZoneThread->Get_ZoneNO();
		if( (pClanWarRoom = GetClanWarRoomByClanID( pUser->GetClanID() )) != NULL )
		{
			if( Send_gsv_GateStatus( pClanWarRoom, pUser, TRUE ) )
			{
				//LeaveCriticalSection( &m_General_LOCK );
				return TRUE;
			}
		}		
	}

	//LeaveCriticalSection( &m_General_LOCK );

	return FALSE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 부활 위치 얻기
//---------------------------------------------------------------------------

BOOL CClanWar::GetRevivePos( classUSER* pUser, tPOINTF ptDeadPos, tPOINTF& ptRevivePos /*OUT*/ )
{
	DWORD dwZoneNo = 0;
	CZoneTHREAD* pZoneThread = NULL;
	CClanWarRoom* pClanWarRoom = NULL;
	CClanWarRoomTemplate* pClanWarRoomTemplate = NULL;


	//EnterCriticalSection( &m_General_LOCK );

	// 존 구하기
	if( (pZoneThread = pUser->GetZONE()) == NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}

	dwZoneNo = pZoneThread->Get_ZoneNO();
	// 클랜워룸 구하기
	if( (pClanWarRoom = GetClanWarRoomByZoneNo( dwZoneNo )) == NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}

	// 클랜워 템플릿 구하기
	if( (pClanWarRoomTemplate = GetWarRoomTemplate( pClanWarRoom->GetNPC() )) == NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}

	if( pClanWarRoom->IsATeamMember( pUser ) )
	{
		ptRevivePos = pClanWarRoomTemplate->GetATeamShortestRevivalPos( ptDeadPos.x, ptDeadPos.y );
		//LeaveCriticalSection( &m_General_LOCK );
		return TRUE;
	}
	else if( pClanWarRoom->IsBTeamMember( pUser ) )
	{
		ptRevivePos = pClanWarRoomTemplate->GetBTeamShortestRevivalPos( ptDeadPos.x, ptDeadPos.y );
		//LeaveCriticalSection( &m_General_LOCK );
		return TRUE;
	}

	//LeaveCriticalSection( &m_General_LOCK );

	return FALSE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 존에서 강제종료 되었으면 첫번째 클랜 맵에 설정하는 기본 복귀 위치에서 시작
//---------------------------------------------------------------------------

BOOL CClanWar::CheckDefaultReturnZone( classUSER* pUser /*IN,OUT*/ )
{
	int iCount = 0;
	DWORD dwMaxClanWarNo = 0;
	CClanWarRoomTemplate* pClanWarRoomTemplate = NULL;


	//EnterCriticalSection( &m_General_LOCK );

	iCount = m_zlWarRoomTemplate.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanWarRoomTemplate = (CClanWarRoomTemplate*)m_zlWarRoomTemplate.IndexOf(i)) != NULL )
		{
			dwMaxClanWarNo = pClanWarRoomTemplate->GetMAX_ClanWarNumber();
			if( pUser->m_nZoneNO >= (short)pClanWarRoomTemplate->GetZoneSeedNo() &&
				pUser->m_nZoneNO <= (short)(pClanWarRoomTemplate->GetZoneSeedNo() + dwMaxClanWarNo) )
			{
				// 최근 접속 종료를 클랜 맵에서 했다.
				// 첫번째 맵의 기본 위치에서 부활시킨다.
				pUser->m_nZoneNO = (short)pClanWarRoomTemplate->GetDefaultReturnZoneNo();
				pUser->m_PosCUR = pClanWarRoomTemplate->GetDefaultReturnPos();

				//LeaveCriticalSection( &m_General_LOCK );

				return TRUE;
			}
		}
	}

	//LeaveCriticalSection( &m_General_LOCK );

	return FALSE;
}

//---------------------------------------------------------------------------
// 유저가 어느팀 클랜전 소속인지 확인
//---------------------------------------------------------------------------

DWORD CClanWar::CheckClanWarUser( classUSER* pUser )
{
	CClanWarRoom* pClanWarRoom = NULL;


	//EnterCriticalSection( &m_General_LOCK );

	if( (pClanWarRoom = GetClanWarRoomByClanID( pUser->m_CLAN.m_dwClanID )) != NULL )
	{
		if( pClanWarRoom->IsUse() )
		{
			switch( pClanWarRoom->GetPlayType() )
			{
			// 클랜전 진행
			case CWPT_TEAM_B_APPLY:
			case CWPT_WARP_TEAM_A:
			case CWPT_WARP_TEAM_B:
			//case CWPT_END_CLANWAR:
				break;
			default:
				//LeaveCriticalSection( &m_General_LOCK );
				return 0;
			}

			if( pClanWarRoom->IsATeamMember( pUser ) ) 
			{
				// 클랜전 기권한 팀의 마스터 및 멤버는 클랜전 참여 못함
				if( pClanWarRoom->GetATeamGiveUp() )
				{
					//LeaveCriticalSection( &m_General_LOCK );
					return 0;
				}
				else
				{
					//LeaveCriticalSection( &m_General_LOCK );
					return 1;
				}
			}

			if( pClanWarRoom->IsBTeamMember( pUser ) ) 
			{
				// 클랜전 기권한 팀의 마스터 및 멤버는 클랜전 참여 못함
				if( pClanWarRoom->GetBTeamGiveUp() )
				{
					//LeaveCriticalSection( &m_General_LOCK );
					return 0;
				}
				else
				{
					//LeaveCriticalSection( &m_General_LOCK );
					return 2;
				}
			}
		}
	}

	//LeaveCriticalSection( &m_General_LOCK );

	return 0;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 지역으로 이동
//---------------------------------------------------------------------------

DWORD CClanWar::WarpUserToClanWar( classUSER* pUser, DWORD dwTeamType )
{
	DWORD dwRestrictLevel = 0;
	DWORD dwRestrictJoinCount = 0;
	CClanWarRoom* pClanWarRoom = NULL;
	CClanWarRoomTemplate* pClanWarRoomTemplate = NULL;


	//EnterCriticalSection( &m_General_LOCK );

	if( (pClanWarRoom = GetClanWarRoomByClanID( pUser->m_CLAN.m_dwClanID )) == NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return 0;
	}

	if( (pClanWarRoomTemplate = GetWarRoomTemplate( pClanWarRoom->GetNPC() )) == NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return 0;
	}

	dwRestrictLevel = pClanWarRoomTemplate->GetWarTypeRestrictLevel( pClanWarRoom->GetWarType() );
	dwRestrictJoinCount = pClanWarRoomTemplate->GetMAX_ClanWarJoiningNumberPerTeam();

	// 사용자 레벨이 제한 레벨 보다 크면
	if( (DWORD)pUser->Get_LEVEL() > dwRestrictLevel )
	{
		// 워프 시키지 않는다.
		pUser->Send_gsv_CLANWAR_Err( CWAR_CLAN_MASTER_LEVEL_RESTRICTED );
		//LeaveCriticalSection( &m_General_LOCK );
		return 1;
	}

	switch( dwTeamType )
	{
	case CWTT_A_TEAM:

		// 참여 인원 제한
		if( dwRestrictJoinCount <= pClanWarRoom->GetATeamMemberCount( TRUE ) )
		{
			pUser->Send_gsv_CLANWAR_Err( CWAR_TOO_MANY_CWAR_PROGRESS );
			//LeaveCriticalSection( &m_General_LOCK );
			return 2;
		}

		// 체력이 0인 경우 힐링
		if( pUser->Get_HP() <= 0 )
		{
			pUser->Set_HP( pUser->Get_MaxHP() );
			pUser->Send_CHAR_HPMP_INFO( FALSE );
		}

		// 파티 해제
		pUser->Party_Out();
		// 클랜전 이동
		pUser->Reward_WARP( pClanWarRoom->GetZoneNo(), pClanWarRoom->GetATeamWarpPos() );
		// 해당 유저 팀 번호 변경
		pUser->SetCur_TeamNO( CWTT_A_TEAM );
		pUser->Set_TeamNoFromClanIDX( CWTT_A_TEAM );

		break;

	case CWTT_B_TEAM:

		// 참여 인원 제한
		if( dwRestrictJoinCount <= pClanWarRoom->GetBTeamMemberCount( TRUE ) )
		{
			pUser->Send_gsv_CLANWAR_Err( CWAR_TOO_MANY_CWAR_PROGRESS );
			//LeaveCriticalSection( &m_General_LOCK );
			return 2;
		}

		// 체력이 0인 경우 힐링
		if( pUser->Get_HP() <= 0 )
		{
			pUser->Set_HP( pUser->Get_MaxHP() );
			pUser->Send_CHAR_HPMP_INFO( FALSE );
		}

		// 파티 해제
		pUser->Party_Out();
		// 클랜전 이동
		pUser->Reward_WARP( pClanWarRoom->GetZoneNo(), pClanWarRoom->GetBTeamWarpPos() );
		// 해당 유저 팀 번호 변경
		pUser->SetCur_TeamNO( CWTT_B_TEAM );
		pUser->Set_TeamNoFromClanIDX( CWTT_B_TEAM );

		break;

	default:
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}

	//LeaveCriticalSection( &m_General_LOCK );

	return TRUE;
}

//---------------------------------------------------------------------------
// 클랜전 포기
//---------------------------------------------------------------------------

BOOL CClanWar::DeclareMasterGiveUp( classUSER* pUser )
{
	//ZThreadSync Sync;

	DWORD dwEndTime = 0;
	CClanWarRoom* pClanWarRoom = NULL;
	CClanWarRoomTemplate* pClanWarRoomTemplate = NULL;



	//EnterCriticalSection( &m_General_LOCK );

	if( (pClanWarRoom = GetClanWarRoomByClanID( pUser->GetClanID())) == NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}

	if( (pClanWarRoomTemplate = GetWarRoomTemplate( pClanWarRoom->GetNPC() )) != NULL )
		dwEndTime = pClanWarRoomTemplate->GetEndTime();
	else
		dwEndTime = 12;

	if( !pClanWarRoom->SetTeamGiveUp( pUser ) )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}


#ifdef __USE_ZLOG__
	g_WatchData.Log( "ClanID:%d, 포기 존번호 : %d", pUser->GetClanID(), pClanWarRoom->GetZoneNo() );
#endif

	// << A, B 팀 모두 포기했으면 클랜워룸 종료 >>
	if( pClanWarRoom->GetATeamGiveUp() && pClanWarRoom->GetBTeamGiveUp() )
	{
#ifdef __USE_ZLOG__
		g_WatchData.Log( "[모두포기]ClanID:%d, 포기 존번호 : %d", pUser->GetClanID(), pClanWarRoom->GetZoneNo() );
#endif

		// 게임 종료 설정
		pClanWarRoom->SetPlayType( CWPT_END_CLANWAR );
		pClanWarRoom->SetElapsedPlayTime( 0 );
		// 몹 제거
		RemoveMOB( pClanWarRoom, CWTT_ALL );
		// PK 모드 오프
		g_pZoneLIST->Set_PK_FLAG( (short)pClanWarRoom->GetZoneNo(), FALSE );
		// 모든 팀원의 체력을 회복
		ResetTeamHP( pClanWarRoom, CWTT_ALL );
	}


	//LeaveCriticalSection( &m_General_LOCK );

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CClanWar::GiveUpClanWar( classUSER* pUser )
{
	//ZThreadSync Sync;

	classUSER* pTopMember = NULL;
	CClanMember* pClanMember = NULL;
	CClanWarRoom* pClanWarRoom = NULL;


	//EnterCriticalSection( &m_General_LOCK );

	if( (pClanWarRoom = GetClanWarRoomByClanID( pUser->GetClanID())) == NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}

	// 클랜 마스터이면 클랜전 포기 승인
	if( pUser->Is_ClanMASTER() )
	{
		if( pClanWarRoom->IsATeamMember( pUser ) )
		{
			// A 팀
			if( DropClanWar( pClanWarRoom, CWTT_A_TEAM ) )
			{
				// 퀘스트 보상 처리
				WinningRewardQuest( pClanWarRoom, CWTT_A_TEAM );

				// B 팀 마스터 or 최고 등급자에게 메시지 보내기
				if( (pClanMember = pClanWarRoom->GetBTeamTopGradeMember( TRUE )) != NULL )
				{
					pTopMember = pClanMember->GetMember();
					if( pTopMember )
						pTopMember->Send_gsv_CLANWAR_Err( CWAR_OPPONENT_MASTER_ALREADY_GIVEUP );
				}
			}
		}
		else if( pClanWarRoom->IsBTeamMember( pUser ) )
		{
			// B 팀
			if( DropClanWar( pClanWarRoom, CWTT_B_TEAM ) )
			{
				// 퀘스트 보상 처리
				WinningRewardQuest( pClanWarRoom, CWTT_B_TEAM );

				// A 팀 마스터 or 최고 등급자에게 메시지 보내기
				if( (pClanMember = pClanWarRoom->GetATeamTopGradeMember( TRUE )) != NULL )
				{
					pTopMember = pClanMember->GetMember();
					if( pTopMember )
						pTopMember->Send_gsv_CLANWAR_Err( CWAR_OPPONENT_MASTER_ALREADY_GIVEUP );
				}
			}
		}
	}

	//LeaveCriticalSection( &m_General_LOCK );

	return FALSE;
}

//---------------------------------------------------------------------------
// 클랜전 진행에 필요한 메시지 전달
// dwProcessType [01:마스터에게 클랜전 참여 메시지 전달]
//               [02:클랜원들에게 클랜전 참여 메시지 전달]
//               [07:접속한 유저들에게 클랜전 진행 수치 상태 전달]
//               [10:각자의 맵으로 보낸다. (기본:주논으로 워프)]
// dwWarpType [1:A 팀 클랜원들에게 클랜전 참여 메시지 전달]
//            [2:B 팀 클랜원들에게 클랜전 참여 메시지 전달]
//---------------------------------------------------------------------------

BOOL CClanWar::SendProcessMessage( classUSER* pUser, DWORD dwProcessType, DWORD dwWarpType )
{
	//ZThreadSync Sync;

	BOOL bReturn = FALSE;
	CClanWarRoom* pClanWarRoom = NULL;


	//EnterCriticalSection( &m_General_LOCK );

	if( (pClanWarRoom = GetClanWarRoomByClanID(pUser->GetClanID())) != NULL )
	{
		bReturn = SendProcessMessage( pClanWarRoom, dwProcessType, dwWarpType );
	}
	
	//LeaveCriticalSection( &m_General_LOCK );

	return bReturn;
}

//---------------------------------------------------------------------------
// 승리 보상 (줄리, 아이템) - 클랜전 후 "승리의 징표" 퀘스트로 보상 받음
//---------------------------------------------------------------------------

BOOL CClanWar::WinningRewardJulyItem( classUSER* pRewardUser )
{
	int iCount = 0;
	ZList* pzlItemList = NULL;
	DWORD dwNPC_NO = 0;
	DWORD dwWarType = 0;
	CObjNPC* pNPC = NULL;
	CClanWarRoomTemplate* pClanWarRoomTemplate = NULL;


	if( pRewardUser == NULL )
		return FALSE;

	//EnterCriticalSection( &m_General_LOCK );

	//	유저 금액 보상..
	if(pRewardUser->m_Quests.m_nClanWarVAR[5] > 0)
	{
		pRewardUser->m_Inventory.m_i64Money = pRewardUser->m_Inventory.m_i64Money + pRewardUser->m_Quests.m_nClanWarVAR[5]*1000;
		pRewardUser->Quest_SetClanWarVAR( 5, 0 );	// 클랜전 보상 금액 (1/1000 단위)
		pRewardUser->Quest_SetClanWarVAR( 6, 0 );	// 채널 번호
		pRewardUser->Send_gsv_SET_MONEYONLY(GSV_SET_MONEY_ONLY);
	}

	//	유저 아이템 보상
	if(pRewardUser->m_Quests.m_nClanWarVAR[7] > 0)
	{
		dwWarType = pRewardUser->m_Quests.m_nClanWarVAR[7];
		dwNPC_NO = pRewardUser->m_Quests.m_nClanWarVAR[8];
		if( dwNPC_NO > 0 )
			pNPC = g_pZoneLIST->Get_LocalNPC( dwNPC_NO );

		if( pNPC )
		{
			if( (pClanWarRoomTemplate = GetWarRoomTemplate( pNPC )) != NULL )
			{
				if( (pzlItemList = pClanWarRoomTemplate->GetWinningRewardItem( dwWarType )) != NULL )
				{
					int iItemNo = 0;
					int iItemDup = 0;
					stClanWarRewardItem* pItem = NULL;

					iCount = pzlItemList->Count();
					for( int i = 0; i < iCount; i++ )
					{
						if( (pItem = (stClanWarRewardItem*)pzlItemList->IndexOf(i)) != NULL )
						{
							g_WatchData.Log( "보상 이이템 번호:%d, 개수:%d", pItem->dwItemNo, pItem->dwItemDup );
							pRewardUser->In_Out_Item( TRUE, pItem->dwItemNo, (WORD)pItem->dwItemDup );
						}						
					}
				}
			}
		}

		pRewardUser->Quest_SetClanWarVAR( 7, 0 );	// 클랜전 타입
		pRewardUser->Quest_SetClanWarVAR( 8, 0 );	// 클랜전 담당 NPC 번호
	}

	//LeaveCriticalSection( &m_General_LOCK );

	return TRUE;
}

//---------------------------------------------------------------------------










//---------------------------------------------------------------------------

BOOL CClanWar::Init()
{
	ZIniFile zSetupINI, zMapINI;
	ZString zDBFileName, zMAPFileName, zValueName;

	zSetupINI.Open( "./CLAN_WAR_SETUP.ini" );

	// DB INI파일 세팅
	zSetupINI.GetValue( "SETUP", "CLAN_WAR_DB", zDBFileName );

	// (20070509:남병철) : 설정값 읽기
	m_dwUSE = 0;
	m_dwElapsedInvalidateTime = 0;
	m_dwInvalidateMAXCount = 0;
	zSetupINI.GetValue( "CHECK_HACKER", "USE", m_dwUSE );
	zSetupINI.GetValue( "CHECK_HACKER", "ELAPSED_TIME", m_dwElapsedInvalidateTime );
	zSetupINI.GetValue( "CHECK_HACKER", "COUNT", m_dwInvalidateMAXCount );

	// (20070511:남병철) : 각 유저의 시간당 패킷 횟수
	m_dwElapsedRecvPacketMAXTime = 0;
	zSetupINI.GetValue( "CHECK_HACKER", "ELAPSED_RECV_PACKET_CHECK_TIME", m_dwElapsedRecvPacketMAXTime );

	// (20070514:남병철) : 패킷 초과 사용 워닝
	m_dwWarningPacketCount = 0;
	zSetupINI.GetValue( "CHECK_HACKER", "WARNING_COUNT", m_dwWarningPacketCount );

	// (20070514:남병철) : 패킷 초과 사용으로 디스커넥
	m_dwDisconnectPacketCount = 0;
	zSetupINI.GetValue( "CHECK_HACKER", "CUT_OFF_COUNT", m_dwDisconnectPacketCount );



	// 클랜 맵 INI 파일 세팅 및 맵 데이타 템플릿 생성
	int iCount = 0;
	CClanWarRoomTemplate* pClanWarRoomTemplate = NULL;
	do
	{
		zValueName = "MAP";
		zValueName += ZString(iCount);
		zSetupINI.GetValue( "SETUP", zValueName, zMAPFileName );

		if( zMAPFileName.IsEmpty() == FALSE )
		{
			// 클랜 맵 데이타 템플릿 생성
			zMapINI.Open( zMAPFileName );

			if( (pClanWarRoomTemplate = new CClanWarRoomTemplate()) == NULL )
				return FALSE;


			// 템플릿에 클랜 NPC 세팅
			DWORD dwNPCIDX = 0;
			CObjNPC* pClanNPC = NULL;
			ZString zNPC_IDX;
			ZString zNPCName;

			zNPCName = "NPC";
			zNPCName += ZString(iCount);
			zSetupINI.GetValue( "NPC", zNPCName, zNPC_IDX );
			if( zNPC_IDX.IsEmpty() ) break;
			dwNPCIDX = (DWORD)zNPC_IDX.ToInt();
			pClanNPC = (CObjNPC*)g_pZoneLIST->Get_LocalNPC( (int)dwNPCIDX );
//			pClanNPC = (CObjNPC*)g_pObjMGR->Get_GameOBJ( (WORD)dwNPCIDX, OBJ_NPC );
//			int iii = g_pObjMGR->GetUsedSlotCNT();
			if( pClanNPC )
				pClanWarRoomTemplate->SetNPC( pClanNPC );
			else
				return FALSE;


			// 템플릿 존 씨드 넘버
			// (STB에 지정된 맵 시작 번호)
			DWORD dwZoneSeedNo = 0;
			zMapINI.GetValue( "CLANWAR_ZONE_SEED_NO", "SEED_NO", dwZoneSeedNo );
			// 세팅
			pClanWarRoomTemplate->SetZoneSeedNo( dwZoneSeedNo );


			// 시작 워프 위치
			ZString zValue;
			FLOAT fATeamPosX=0.f, fATeamPosY=0.f, fBTeamPosX=0.f, fBTeamPosY=0.f;

			zMapINI.GetValue( "CLANWAR_ZONE_WARP_POSITION", "ATEAM_POS_X", zValue );
			fATeamPosX = zValue.ToFloat();
			zMapINI.GetValue( "CLANWAR_ZONE_WARP_POSITION", "ATEAM_POS_Y", zValue );
			fATeamPosY = zValue.ToFloat();
			zMapINI.GetValue( "CLANWAR_ZONE_WARP_POSITION", "BTEAM_POS_X", zValue );
			fBTeamPosX = zValue.ToFloat();
			zMapINI.GetValue( "CLANWAR_ZONE_WARP_POSITION", "BTEAM_POS_Y", zValue );
			fBTeamPosY = zValue.ToFloat();
			// 세팅
			pClanWarRoomTemplate->SetStartPos( fATeamPosX, fATeamPosY, fBTeamPosX, fBTeamPosY );


			// 부활 위치
			ZString zTempRevival;
			int iTempRevival = 0;

			do
			{
				zTempRevival = "ATEAM_POS_X";
				zTempRevival += ZString(iTempRevival);
				zMapINI.GetValue( "CLANWAR_ZONE_REVIVAL_POSITION", zTempRevival, zValue );
				if( zValue.IsEmpty() ) break;
				fATeamPosX = zValue.ToFloat();

				zTempRevival = "ATEAM_POS_Y";
				zTempRevival += ZString(iTempRevival);
				zMapINI.GetValue( "CLANWAR_ZONE_REVIVAL_POSITION", zTempRevival, fATeamPosY );
				// 세팅
				pClanWarRoomTemplate->AddATeamRevivalPos( fATeamPosX, fATeamPosY );

				zTempRevival = "BTEAM_POS_X";
				zTempRevival += ZString(iTempRevival);
				zMapINI.GetValue( "CLANWAR_ZONE_REVIVAL_POSITION", zTempRevival, zValue );
				if( zValue.IsEmpty() ) break;
				fBTeamPosX = zValue.ToFloat();

				zTempRevival = "BTEAM_POS_Y";
				zTempRevival += ZString(iTempRevival);
				zMapINI.GetValue( "CLANWAR_ZONE_REVIVAL_POSITION", zTempRevival, fBTeamPosY );
				// 세팅
				pClanWarRoomTemplate->AddBTeamRevivalPos( fBTeamPosX, fBTeamPosY );

				iTempRevival++;

			} while(1);


			// 기본 귀환 위치
			DWORD dwZoneNo = 0;
			FLOAT fPosX=0.f, fPosY=0.f;

			zMapINI.GetValue( "DEFAULT_CLANWAR_ZONE_RETURN_POSITION", "ZONE_NO", dwZoneNo );
			pClanWarRoomTemplate->SetDefaultReturnZoneNo( dwZoneNo );
			zMapINI.GetValue( "DEFAULT_CLANWAR_ZONE_RETURN_POSITION", "POS_X", fPosX );
			zMapINI.GetValue( "DEFAULT_CLANWAR_ZONE_RETURN_POSITION", "POS_Y", fPosY );
			pClanWarRoomTemplate->SetDefaultReturnPos( fPosX, fPosY );


			// 클랜전 게임 시간
			DWORD dwClanWarTime = 0;

			zMapINI.GetValue( "CLANWAR_TIME", "GATE_TIME", dwClanWarTime );
			pClanWarRoomTemplate->SetGateTime( dwClanWarTime );
			zMapINI.GetValue( "CLANWAR_TIME", "PLAY_TIME", dwClanWarTime );
			pClanWarRoomTemplate->SetPlayTime( dwClanWarTime );
			zMapINI.GetValue( "CLANWAR_TIME", "END_TIME", dwClanWarTime );
			pClanWarRoomTemplate->SetEndTime( dwClanWarTime );


			// 클랜전 레벨 제한
			DWORD dwRestrictLevel = 0;
			ZString zTempWarType;
			int iTempWarType = 0;

			do 
			{
				zTempWarType = "WAR_TYPE";
				zTempWarType += ZString(iTempWarType++);
				zMapINI.GetValue( "CLANWAR_RESTRICT_LEVEL", zTempWarType, zValue );
				if( zValue.IsEmpty() ) break;
				dwRestrictLevel = (DWORD)zValue.ToInt();
				pClanWarRoomTemplate->AddWarTypeRestrictLevel( dwRestrictLevel );
			} while(1);


			// 클랜전 배팅 금액
			INT64 i64BatJuly = 0;
			ZString zTempBatJuly;
			int iTempBatType = 0;

			do 
			{
				zTempBatJuly = "WAR_BAT";
				zTempBatJuly += ZString(iTempBatType++);
				zMapINI.GetValue( "CLANWAR_BAT_JULY", zTempBatJuly, zValue );
				if( zValue.IsEmpty() ) break;
				i64BatJuly = (INT64)zValue.ToInt();
				pClanWarRoomTemplate->AddWarTypeBatJuly( i64BatJuly );
			} while(1);


			// 클랜전 팀당 최대 인원 수 (기본 : 15 (설정 최대값 : 50))
			DWORD dwMax_ClanWarJoiningNumberPerTeam = 0;

			zMapINI.GetValue( "MAX_CLANWAR_JOINING_NUMBER_PER_TEAM", "MAX_CLANWAR_JOINING_NUMBER_PER_TEAM", dwMax_ClanWarJoiningNumberPerTeam );
			pClanWarRoomTemplate->SetMAX_ClanWarJoiningNumberPerTeam( dwMax_ClanWarJoiningNumberPerTeam );


			// 클랜전 최대 수 (클랜전 인원 제한 50일 경우 25, 15일 경우 50) : 최대 접속자 2500명 제한 기준
			DWORD dwMAX_ClanWarNumber = 0;

			zMapINI.GetValue( "MAX_CLANWAR_NUMBER", "MAX_CLANWAR_NUMBER", dwMAX_ClanWarNumber );
			pClanWarRoomTemplate->SetMAX_ClanWarNumber( dwMAX_ClanWarNumber );


			// 클랜전 몬스터 정보 설정
			int iClanWarTypeCount = 0;
			ZString zClanWarTypeName;
			ZString zMobCheck;		// 몬스터 정보가 있는지 채크

			do 
			{
				// A 팀 정보 읽기
				zClanWarTypeName = "WAR_TYPE";
				zClanWarTypeName += ZString(iClanWarTypeCount);
				zClanWarTypeName += "_ATEAM_MOB_SETUP";

				// 몹 정보를 다 읽었는가?
				zMapINI.GetValue( zClanWarTypeName, "MOB_0", zMobCheck );
				if( zMobCheck.IsEmpty() ) break;

				// A 팀 WarType에 맞는 몬스터 로딩
				InitWarTypeMOB( zMapINI, 0, zClanWarTypeName, pClanWarRoomTemplate );


				// B 팀 정보 읽기
				zClanWarTypeName = "WAR_TYPE";
				zClanWarTypeName += ZString(iClanWarTypeCount);
				zClanWarTypeName += "_BTEAM_MOB_SETUP";

				// 몹 정보를 다 읽었는가?
				zMapINI.GetValue( zClanWarTypeName, "MOB_0", zMobCheck );
				if( zMobCheck.IsEmpty() ) break;

				// B 팀 WarType에 맞는 몬스터 로딩
				InitWarTypeMOB( zMapINI, 1, zClanWarTypeName, pClanWarRoomTemplate );

				// 카운트 증가
                iClanWarTypeCount++;

			} while(1);


			// 클랜전 승리 보상 아이템 읽기
			iClanWarTypeCount = 0;
			zClanWarTypeName = "";
			ZString zRewardItemCheck;	// 아이템 정보가 있는지 채크

			do 
			{
				// INI 타이틀 생성
				zClanWarTypeName = "CW_REWARD_ITEM_";
				zClanWarTypeName += ZString( iClanWarTypeCount );

				// 아이템 정보를 다 읽었는가?
				zMapINI.GetValue( zClanWarTypeName, "CW_REWARD_ITEM_0", zRewardItemCheck );
				if( zRewardItemCheck.IsEmpty() ) break;

				// 클랜전 타입별 아이템 읽기
				InitWarTypeRewardItem( zMapINI, zClanWarTypeName, pClanWarRoomTemplate );

				// 카운트 증가
				iClanWarTypeCount++;
				
			} while(1);


			// 클랜전 맵 데이타 템플릿 등록
			m_zlWarRoomTemplate.Add( (void*)pClanWarRoomTemplate );
		}
		else break;

		// 맵 데이타 구분 번호
		iCount++;

	} while(1);

	zMapINI.Close();
	zSetupINI.Close();

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CClanWar::InitWarTypeMOB( ZIniFile& zMapINI, int iTeamType, ZString zClanWarTypeName, CClanWarRoomTemplate* pClanWarRoomTemplate )
{
	int iClanWarMobCount = 0;
	ZString zClanWarMobName;
	ZString zMobCheck;		// 몬스터 정보가 있는지 채크
	stClanWarMOB* pClanWarMobInfo = NULL;
	ZList* pzlMobData = NULL;



	// 첫 데이타 채크는 하고 들어왔기 때문에 한번은 꼭 쓰인다. (즉, DELETE 않함)
	if( (pzlMobData = new ZList()) == NULL )
		return FALSE;

	do 
	{
		// 몹 번호
		zClanWarMobName = "MOB_";
		zClanWarMobName += ZString( iClanWarMobCount );
		zMapINI.GetValue( zClanWarTypeName, zClanWarMobName, zMobCheck );
		if( zMobCheck.IsEmpty() ) break;
		if( (pClanWarMobInfo = new stClanWarMOB()) == NULL )
			return FALSE;
		pClanWarMobInfo->dwMOB_IDX = (DWORD)zMobCheck.ToInt();

		// 몹 위치
		zClanWarMobName = "POS_X";
		zClanWarMobName += ZString( iClanWarMobCount );
		zMapINI.GetValue( zClanWarTypeName, zClanWarMobName, pClanWarMobInfo->fPosX );

		zClanWarMobName = "POS_Y";
		zClanWarMobName += ZString( iClanWarMobCount );
		zMapINI.GetValue( zClanWarTypeName, zClanWarMobName, pClanWarMobInfo->fPosY );

		// 몹 리젠 수
		zClanWarMobName = "REGEN_NUMBER";
		zClanWarMobName += ZString( iClanWarMobCount );
		zMapINI.GetValue( zClanWarTypeName, zClanWarMobName, pClanWarMobInfo->dwRegenNumber );

		// 몹 리젠 반경
		zClanWarMobName = "REGEN_RANGE";
		zClanWarMobName += ZString( iClanWarMobCount );
		zMapINI.GetValue( zClanWarTypeName, zClanWarMobName, pClanWarMobInfo->dwRegenRange );

		// 몹 개별 정보 등록
		if( (pzlMobData->Add( pClanWarMobInfo )) == FALSE )
			return FALSE;

		iClanWarMobCount++;

	} while(1);

	// 몹 정보 등록
	if( iTeamType == 0 ) // A 팀
		pClanWarRoomTemplate->AddATeamMOBInfo( pzlMobData );
	else // B 팀
		pClanWarRoomTemplate->AddBTeamMOBInfo( pzlMobData );

	return TRUE;
}

//---------------------------------------------------------------------------
// 클랜전 타입별 승리 보상 아이템
//---------------------------------------------------------------------------

BOOL CClanWar::InitWarTypeRewardItem( ZIniFile& zMapINI, ZString zClanWarTypeName, CClanWarRoomTemplate* pClanWarRoomTemplate )
{
	int iItemCount = 0;
	ZString zItemData;
	ZString zItemCheck;
	stClanWarRewardItem* pClanWarRewardItem = NULL;
	ZList* pzlItemData = NULL;



	// 첫 데이타 채크는 하고 들어왔기 때문에 한번은 꼭 쓰인다. (즉, DELETE 않함)
	if( (pzlItemData = new ZList()) == NULL )
		return FALSE;

	do 
	{
		// 아이템 번호
		zItemData = "CW_REWARD_ITEM_";
		zItemData += ZString( iItemCount );
		zMapINI.GetValue( zClanWarTypeName, zItemData, zItemCheck );
		if( zItemCheck.IsEmpty() ) break;
		if( (pClanWarRewardItem = new stClanWarRewardItem()) == NULL )
			return FALSE;
		pClanWarRewardItem->dwItemNo = (DWORD)zItemCheck.ToInt();

		// 아이템 중복 개수
		zItemData = "CW_REWARD_DUP_";
		zItemData += ZString( iItemCount );
		zMapINI.GetValue( zClanWarTypeName, zItemData, pClanWarRewardItem->dwItemDup );

		// 개별 아이템 등록
		if( (pzlItemData->Add( pClanWarRewardItem )) == FALSE )
			return FALSE;

		iItemCount++;

	} while(1);

	// 아이템 정보 등록
	pClanWarRoomTemplate->AddWinningRewardItem( pzlItemData );

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜 멤버 등록
//---------------------------------------------------------------------------

BOOL CClanWar::AddClanMember( classUSER* pMember )
{
	ZThreadSync Sync;

	int iFamilyCount = 0;
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;

	// 동기화
	//EnterCriticalSection( &m_General_LOCK );

	// 처음이면 CClanFamily 설정
	iFamilyCount = m_zlClanFamily.Count();
	if( iFamilyCount < 1 )
	{
		// << 최초 클랜원 추가 >>
		
		// 클랜&멤버 생성
		if( (pClanFamily = new CClanFamily()) == NULL )
		{
			//LeaveCriticalSection( &m_General_LOCK );
			return FALSE;
		}
		if( (pClanMember = new CClanMember()) == NULL )
		{
			//LeaveCriticalSection( &m_General_LOCK );
			return FALSE;
		}

		// 멤버정보 세팅
		pClanMember->DataClear();				// 초기화
		pClanMember->SetUseFlag( TRUE );		// 사용중인 데이타인가?
		pClanMember->SetUseWarFlag( FALSE );	// 클랜전에 참여중인가?
		pClanMember->SetMember( pMember );		// 클랜 멤버 정보

		// 패밀리에 추가
		pClanFamily->DataClear();
		//pMember->m_CLAN.m_iRankPoint
		DWORD dwClanID = pMember->GetClanID();
		char* pClanName = pMember->GetClanNAME();
		pClanFamily->AddMember( pClanMember );

		// 패밀리 리스트에 추가
		m_zlClanFamily.Add( pClanFamily );

#ifdef __USE_ZLOG__
		g_WatchData.Log( "[ m_zlClanFamily에 신규 클랜 추가 ] - 최초" );
		if( pMember->Is_ClanMASTER() )
			g_WatchData.Log( " ** 최초 클랜원 추가(마스터) : %s", pMember->Get_NAME() );
		else
			g_WatchData.Log( " ** 최초 클랜원 추가 : %s", pMember->Get_NAME() );
#endif

	}
	else
	{
		// << 클랜원 추가 >>
		
		// 이미 로딩된 클랜패밀리의 일원인가?
		if( (pClanFamily = CheckTheSameFamily( pMember )) != NULL )
		{
			// 클랜 멤버&패밀리 모두 로딩되어 있음
			if( (pClanMember = pClanFamily->CheckVacantMemberMemory()) != NULL )
			{
				// 멤버정보 세팅
				pClanMember->DataClear();				// 초기화
				pClanMember->SetUseFlag( TRUE );		// 사용중인 데이타인가?
				pClanMember->SetUseWarFlag( FALSE );	// 클랜전에 참여중인가?
				pClanMember->SetMember( pMember );		// 클랜 멤버 정보

				// 패밀리 데이타 업데이트
				pClanFamily->SetUseFlag( TRUE );
				pClanFamily->UpdateMember( pMember );

#ifdef __USE_ZLOG__
				if( pMember->Is_ClanMASTER() )
					g_WatchData.Log( " ** 클랜원 추가[패밀리, 멤버 재사용](마스터) : %s", pMember->Get_NAME() );
				else
					g_WatchData.Log( " ** 클랜원 추가[패밀리, 멤버 재사용] : %s", pMember->Get_NAME() );
#endif

			}
			// 멤버 추가
			else
			{
				// 클랜 멤버 로딩이 없으므로
				if( (pClanMember = new CClanMember()) == NULL )
				{
					//LeaveCriticalSection( &m_General_LOCK );
					return FALSE;
				}

				// 멤버정보 세팅
				pClanMember->DataClear();				// 초기화
				pClanMember->SetUseFlag( TRUE );		// 사용중인 데이타인가?
				pClanMember->SetUseWarFlag( FALSE );	// 클랜전에 참여중인가?
				pClanMember->SetMember( pMember );		// 클랜 멤버 정보

				// 패밀리에 추가
				pClanFamily->AddMember( pClanMember );

#ifdef __USE_ZLOG__
				if( pMember->Is_ClanMASTER() )
					g_WatchData.Log( " ** 클랜원 추가[패밀리 재사용, 멤버추가](마스터) : %s", pMember->Get_NAME() );
				else
					g_WatchData.Log( " ** 클랜원 추가[패밀리 재사용, 멤버추가] : %s", pMember->Get_NAME() );
#endif

			}

			// << 패밀리 리스트에 추가할 필요 없음 >>

		}
		else
		{
			// (20070427:남병철) 0427 이전 주석 : 사용하지 않는 패밀리 데이타가 있는가?
			// (20070427:남병철) 0427 이후 주석 : 사용하지 않는 패밀리의 일원이었는가?
			if( (pClanFamily = CheckVacantFamilyMemory( pMember )) != NULL )
			{
				// 클랜 멤버&패밀리 모두 로딩되어 있음
				if( (pClanMember = pClanFamily->CheckVacantMemberMemory()) != NULL )
				{
					// 멤버정보 세팅
					pClanMember->DataClear();				// 초기화
					pClanMember->SetUseFlag( TRUE );		// 사용중인 데이타인가?
					pClanMember->SetUseWarFlag( FALSE );	// 클랜전에 참여중인가?
					pClanMember->SetMember( pMember );		// 클랜 멤버 정보

					// 패밀리 데이타 업데이트
					pClanFamily->SetUseFlag( TRUE );
					pClanFamily->UpdateMember( pMember );

#ifdef __USE_ZLOG__
					if( pMember->Is_ClanMASTER() )
						g_WatchData.Log( " ** 클랜원 추가[패밀리, 멤버 재사용](마스터) : %s", pMember->Get_NAME() );
					else
						g_WatchData.Log( " ** 클랜원 추가[패밀리, 멤버 재사용] : %s", pMember->Get_NAME() );
#endif

				}
				// 멤버 추가
				else
				{
					if( (pClanMember = new CClanMember()) == NULL )
					{
						//LeaveCriticalSection( &m_General_LOCK );
						return FALSE;
					}

					// 멤버정보 세팅
					pClanMember->DataClear();				// 초기화
					pClanMember->SetUseFlag( TRUE );		// 사용중인 데이타인가?
					pClanMember->SetUseWarFlag( FALSE );	// 클랜전에 참여중인가?
					pClanMember->SetMember( pMember );		// 클랜 멤버 정보

					// 패밀리에 추가
					pClanFamily->AddMember( pClanMember );

#ifdef __USE_ZLOG__
					if( pMember->Is_ClanMASTER() )
						g_WatchData.Log( " ** 클랜원 추가[패밀리 재사용, 멤버추가](마스터) : %s", pMember->Get_NAME() );
					else
						g_WatchData.Log( " ** 클랜원 추가[패밀리 재사용, 멤버추가] : %s", pMember->Get_NAME() );
#endif

				}

				// << 패밀리 리스트에 추가할 필요 없음 >>

			}
			else
			{
				// 처음 접속하는 클랜이므로 정보 생성
				// 클랜 패밀리가 로딩되지 않았으므로 CClanFamily와 CClanMember 모두 생성
				// (최초 클랜원 추가와 같음)

				// 클랜&멤버 생성
				if( (pClanFamily = new CClanFamily()) == NULL )
				{
					//LeaveCriticalSection( &m_General_LOCK );
					return FALSE;
				}
				if( (pClanMember = new CClanMember()) == NULL )
				{
					//LeaveCriticalSection( &m_General_LOCK );
					return FALSE;
				}

				// 멤버정보 세팅
				pClanMember->DataClear();				// 초기화
				pClanMember->SetUseFlag( TRUE );		// 사용중인 데이타인가?
				pClanMember->SetUseWarFlag( FALSE );	// 클랜전에 참여중인가?
				pClanMember->SetMember( pMember );		// 클랜 멤버 정보

				// 패밀리에 추가
				pClanFamily->DataClear();
				pClanFamily->AddMember( pClanMember );

				// 패밀리 리스트에 추가
				m_zlClanFamily.Add( pClanFamily );

#ifdef __USE_ZLOG__
				g_WatchData.Log( "[ m_zlClanFamily에 신규 클랜 추가 ]" );
				if( pMember->Is_ClanMASTER() )
					g_WatchData.Log( " ** 클랜원 추가[패밀리, 멤버 생성](마스터) : %s", pMember->Get_NAME() );
				else
					g_WatchData.Log( " ** 클랜원 추가[패밀리, 멤버 생성] : %s", pMember->Get_NAME() );
#endif

			}
		}
	}

	// 클랜 접속 정보 업데이트
	//UpdateClanInfoList();

#ifdef __USE_ZLOG__
	int iClanCount = 0;
	int iLoginMemberCount = 0;
	ZList* pMemberList = NULL;


	g_WatchData.Log( " " );
	g_WatchData.Log( "<< 로그인 클랜들 >>" );
	iFamilyCount = m_zlClanFamily.Count();
	g_WatchData.Log( " - 클랜 수:%d", iFamilyCount );
	for( int i = 0; i < iFamilyCount; i++ )
	{
		if( (pClanFamily = (CClanFamily*)m_zlClanFamily.IndexOf(i)) != NULL )
		{
			if( pClanFamily->IsUse() )
				g_WatchData.Log( " -*- 클랜명:%s - USE:TRUE", (pClanFamily->GetName()).c_str() );
			else
				g_WatchData.Log( " -*- 클랜명:%s - USE:false", (pClanFamily->GetName()).c_str() );


			if( (pMemberList = pClanFamily->GetMemberList()) != NULL )
			{
				iLoginMemberCount = pMemberList->Count();
				for( int j = 0; j < iLoginMemberCount; j++ )
				{
					if( (pClanMember = (CClanMember*)pMemberList->IndexOf(j)) != NULL )
					{
						if( pClanMember->IsUse() )
							g_WatchData.Log( " --- %02d:멤버(%d):%s - USE:TRUE", j+1, pClanMember->GetGrade(), pClanMember->GetName() );
						else
							g_WatchData.Log( " --- %02d:멤버(%d):%s - USE:false", j+1, pClanMember->GetGrade(), pClanMember->GetName() );

					}
				}
			}
		}
	}
#endif

	//LeaveCriticalSection( &m_General_LOCK );

	return TRUE;
}

//---------------------------------------------------------------------------
// 클랜 멤버 정보 업데이트
//---------------------------------------------------------------------------

BOOL CClanWar::UpdateClanMember( classUSER* pMember )
{
	//ZThreadSync Sync;

	CClanFamily* pClanFamily = NULL;

	// 동기화
	//EnterCriticalSection( &m_General_LOCK );
	//if( (pClanFamily = CheckTheSameFamily( pMember )) != NULL )
	//{
	//	pClanFamily->UpdateMember( pMember );
	//}
	//LeaveCriticalSection( &m_General_LOCK );


	return TRUE;
}

//---------------------------------------------------------------------------
// 클랜 가입자인가?
//---------------------------------------------------------------------------

BOOL CClanWar::IsClanJoined( classUSER* pMember )
{
	int iCount = 0;
	CClanFamily* pClanFamily = NULL;


	//EnterCriticalSection( &m_General_LOCK );
	
	iCount = m_zlClanFamily.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanFamily = (CClanFamily*)m_zlClanFamily.IndexOf(i)) != NULL )
		{
			if( pClanFamily->IsMember( pMember ) )
			{
#ifdef __USE_ZLOG__
				g_WatchData.Log( " ** %s : %s", __FUNCTION__, pMember->Get_NAME() );
#endif
				//LeaveCriticalSection( &m_General_LOCK );
				return TRUE;
			}
		}
	}
#ifdef __USE_ZLOG__
	g_WatchData.Log( " ** %s : %s", __FUNCTION__, pMember->Get_NAME() );
#endif
	//LeaveCriticalSection( &m_General_LOCK );

	return FALSE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 신청
//---------------------------------------------------------------------------

BOOL CClanWar::ApplyClanWar( CObjNPC* pNPC, classUSER* pApplyUser, DWORD dwClientClanWarType )
{
	ZThreadSync Sync;

	if( pNPC == NULL ) return FALSE;
	if( pApplyUser == NULL ) return FALSE;
	if( dwClientClanWarType == 0 ) return FALSE; // 클라이언트에서는 1,2, 31,32, 61,62를 보내준다.

	DWORD dwWarType = 0;
	DWORD dwUserLevel = 0;
	CClanWarRoomTemplate* pWarRoomTemplate = NULL;

	// 동기화
	//EnterCriticalSection( &m_General_LOCK );

#ifdef __USE_ZLOG__
	g_WatchData.Log( __FUNCTION__ );
#endif

	// 마스터인가?
	if( pApplyUser->Is_ClanMASTER() == FALSE )
		return FALSE;

	// (20070503:남병철) : 이미 클랜전 진행 중인 사람이 신청하면 무시한다.
	if( IsApplyClanWar( pApplyUser ) )
		return FALSE;

	// 현재 클랜전 맵에 맞는 데이타 로딩
	if( (pWarRoomTemplate = GetWarRoomTemplate( pNPC )) == NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}

	// 현재는 0,1,2 즉, 3가지 타입만 사용중
	dwWarType = ChangeClientToServerClanWarType( dwClientClanWarType );
	dwUserLevel = pApplyUser->Get_LEVEL();
	if( dwUserLevel < 0 )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}

	// 클랜전 진행 레벨 제한 채크
	if( CheckUserLevel( pNPC, dwWarType, dwUserLevel ) == FALSE )
	{
		//	오류 메시지 전송
		pApplyUser->Send_gsv_CLANWAR_Err( CWAR_CLAN_MASTER_LEVEL_RESTRICTED );
		//LeaveCriticalSection(&m_General_LOCK);
		return FALSE;
	}

	// 줄리 부족 채크
	if( pApplyUser->Get_MONEY() < pWarRoomTemplate->GetWarTypeBatJuly( dwWarType ) )
	{
		//	오류 메시지 전송
		//LeaveCriticalSection(&m_General_LOCK);
		return FALSE;
	}

	// 클랜전 예약 상태 채크
	// (신청하는 조건에 부합하는 클랜전 예약이 있으면 연결시켜준다.)
	CClanWarRoom* pWarRoom = NULL;
	if( (pWarRoom = CheckWarRoomReservation( pNPC, dwWarType )) != NULL )
	{
		// << 이미 신청되어있는 클랜전 룸과 연결 >>
		
		if( pWarRoom->GetPlayType() == CWPT_TEAM_A_APPLY )
		{
			INT64 i64BatJuly = 0;
			INT64 i64Temp = 0;
			FLOAT fPosX = 0.f, fPosY = 0.f;


			// << 클랜전 진행 종류 >>
			// 2 : 클랜전 신청 (두번째 클랜)
			pWarRoom->SetPlayType( CWPT_TEAM_B_APPLY );

			// 플레이 시간 상수 (클랜전 진행 상태에 따라 다르다.)
			pWarRoom->SetElapsedPlayTime( pWarRoomTemplate->GetPlayTime() );

			// 배팅 줄리
			i64BatJuly = pWarRoom->GetBatJuly();
			i64BatJuly += pWarRoomTemplate->GetWarTypeBatJuly( dwWarType );
			pWarRoom->SetBatJuly( i64BatJuly );
			// 배팅 줄리 감소
			i64Temp = pApplyUser->GetCur_MONEY();
			i64Temp = i64Temp - pWarRoomTemplate->GetWarTypeBatJuly( dwWarType );
			if( i64Temp >= 0 )
			{
				pApplyUser->SetCur_MONEY( i64Temp );
				pApplyUser->Send_gsv_SET_MONEYONLY( GSV_SET_MONEY_ONLY );
			}

			// 클랜전 B 팀 존 워프 좌표
			fPosX = pWarRoomTemplate->GetBTeamStartPos().x;
			fPosY = pWarRoomTemplate->GetBTeamStartPos().y;
			pWarRoom->SetBTeamWarpPos( fPosX, fPosY );

			// B 팀 클랜 세팅
			SetBTeam( pWarRoom, pApplyUser->GetClanNAME() );

			// B 팀 클랜원 타입 세팅
			pWarRoom->SetBTeamWarType( dwWarType );

			// << 클랜전 B 팀 신청 DB 저장 >>
			GF_DB_CWar_Insert( 2, 0, i64BatJuly, 0, pApplyUser->GetClanID(), NULL );

			// 클랜장에게 입장 메시지 보낸다.
			SendProcessMessage( pWarRoom, CWMPT_MASTER_MSG, CWMWT_NO_WARP );

			// << 클랜전 시작으로 간주 DB 저장 >>
			// CheckWarRoomReservation 여기서 A, B 팀의 포인터 검증 되어서 나옴
			GF_DB_CWar_Insert( 3, 0, pWarRoom->GetBatJuly(), pWarRoom->GetATeam()->GetIDX(), pWarRoom->GetBTeam()->GetIDX(), NULL );

			// (20070423:남병철) : 클랜전 로그 추가
			g_LOG.CS_ODS( 0xffff, "\n[B TEAM] ZONE(%d) WAR_TYPE(%d) CLANID(%d):%s MASTERID(%s):%s",
						  pWarRoom->GetZoneNo(), pWarRoom->GetWarType(), pApplyUser->GetClanID(), pApplyUser->GetClanNAME(),
						  pApplyUser->Get_ACCOUNT(), pApplyUser->Get_NAME() );



#ifdef __USE_ZLOG__
			g_WatchData.Log( "[클랜전 B 팀 신청]" );
#endif
		}
		else
		{
#ifdef __USE_ZLOG__
			g_WatchData.Log( "[ERROR] B 팀 신청시 워룸이 CWPT_TEAM_A_APPLY 상태 아님(ZoneNO:%d)(PlayType:%d)", pWarRoom->GetZoneNo(), pWarRoom->GetPlayType() );
#endif
			//LeaveCriticalSection(&m_General_LOCK);
			return FALSE;
		}
	}
	else
	{
		// << 클랜전 신청 >>
		DWORD dwWarRoomIDX = 0;

		// 빈 방 가져오기 (없으면 허용하는한 만들어서라도!)
		if( (pWarRoom = CheckVacantWarRoom( pNPC, dwWarRoomIDX )) != NULL )
		{
			// 빈 방에 신청 세팅

			INT64 i64BatJuly = 0;
			INT64 i64Temp = 0;
			DWORD dwZoneNo = 0;
			FLOAT fPosX = 0.f, fPosY = 0.f;

			pWarRoom->SetUseFlag( TRUE );				// 사용중인 방으로 세팅
			pWarRoom->SetNPC( pNPC );					// NPC 세팅
			pWarRoom->SetPlayType( CWPT_TEAM_A_APPLY );	// 클랜전 진행 타입 설정
			pWarRoom->SetWarType( dwWarType );			// 클랜워 타입
			pWarRoom->SetElapsedPlayTime( 0 );			// 남은 진행 시간

			// 배팅 줄리
			i64BatJuly = pWarRoomTemplate->GetWarTypeBatJuly( dwWarType );
			pWarRoom->SetBatJuly( i64BatJuly );
			// 배팅 줄리 감소
			i64Temp = pApplyUser->GetCur_MONEY();
			i64Temp = i64Temp - pWarRoomTemplate->GetWarTypeBatJuly( dwWarType );
			if( i64Temp >= 0 )
			{
				pApplyUser->SetCur_MONEY( i64Temp );
				pApplyUser->Send_gsv_SET_MONEYONLY( GSV_SET_MONEY_ONLY );
			}

			// 클랜전 존 번호 생성
			pWarRoom->SetZoneNo( pWarRoomTemplate->GetZoneSeedNo() + dwWarRoomIDX );

			// 클랜전 A 팀 존 워프 좌표
			fPosX = pWarRoomTemplate->GetATeamStartPos().x;
			fPosY = pWarRoomTemplate->GetATeamStartPos().y;
			pWarRoom->SetATeamWarpPos( fPosX, fPosY );

			// 기본 복귀 좌표 설정
			pWarRoom->SetDefaultReturnZoneNo( pWarRoomTemplate->GetDefaultReturnZoneNo() );
			fPosX = pWarRoomTemplate->GetDefaultReturnPos().x;
			fPosY = pWarRoomTemplate->GetDefaultReturnPos().y;
			pWarRoom->SetDefaultReturnPos( fPosX, fPosY );

			// 기본 무승부 세팅	
			pWarRoom->SetWarResult( CWWR_WAR_RESULT_DRAW );

			// 드레곤 HP 갱신 시간 (HP 세팅은 몹 리젠할때)
			pWarRoom->SetBossHPUpdateTime( 10 ); // 10초

			// 문 닫힘 (A, B팀 동일함)
			pWarRoom->SetGateFlag( FALSE );

			// A 팀 클랜 세팅
			SetATeam( pWarRoom, pApplyUser->GetClanNAME() );

			// A 팀 클랜원 타입 세팅
			pWarRoom->SetATeamWarType( dwWarType );

			// << 몹 리젠은 게임 시작할때! >>

			// << 클랜전 A 팀 신청 DB 저장 >>
			GF_DB_CWar_Insert( 1, 0, i64BatJuly, pApplyUser->GetClanID(), 0, NULL );

			// (20070423:남병철) : 클랜전 로그 추가
			g_LOG.CS_ODS( 0xffff, "\n[A TEAM] ZONE(%d) WAR_TYPE(%d) CLANID(%d):%s MASTERID(%s):%s",
				pWarRoom->GetZoneNo(), pWarRoom->GetWarType(), pApplyUser->GetClanID(), pApplyUser->GetClanNAME(),
				pApplyUser->Get_ACCOUNT(), pApplyUser->Get_NAME() );


#ifdef __USE_ZLOG__
			g_WatchData.Log( "[클랜전 A 팀 신청]" );
#endif
	
		}
		else
		{
#ifdef __USE_ZLOG__
			g_WatchData.Log( "[ERROR] A 팀 신청할 공간이 없습니다.", pWarRoom->GetZoneNo() );
#endif
			// 신청할 방이 없음!!
			pApplyUser->Send_gsv_CLANWAR_Err( CWAR_TOO_MANY_CWAR_PROGRESS );
			//LeaveCriticalSection(&m_General_LOCK);
			return FALSE;
		}
	}

	// << 클랜전 진행 상태 윈도우의 정보 갱신 >>

	//LeaveCriticalSection(&m_General_LOCK);

	return TRUE;
}

//---------------------------------------------------------------------------
// 클랜전 신청 중인지 확인
//---------------------------------------------------------------------------

BOOL CClanWar::IsApplyClanWar( classUSER* pMember )
{
	int iCount = 0;
	CClanWarRoom* pClanWarRoom = NULL;


	iCount = m_zlWarRoom.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanWarRoom = (CClanWarRoom*)m_zlWarRoom.IndexOf(i)) != NULL )
		{
			// 신청후 사용중인 클랜전일 경우
			if( pClanWarRoom->IsUse() )
			{
				// A 팀 멤버인가?
				if( pClanWarRoom->GetATeam() )
				{
					if( pClanWarRoom->GetATeam()->IsUse() )
						if( pClanWarRoom->GetATeam()->IsMember( pMember ) )
							return TRUE;
				}

				// B 팀 멤버인가?
				if( pClanWarRoom->GetBTeam() )
				{
					if( pClanWarRoom->GetBTeam() )
						if( pClanWarRoom->GetBTeam()->IsMember( pMember ) )
							return TRUE;
				}
			}
		}
	}

	return FALSE;
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// 유저 로그아웃
//---------------------------------------------------------------------------

BOOL CClanWar::LogOut( classUSER* pUser )
{
	ZThreadSync Sync;

	CClanFamily* pClanFamily = NULL;


	// 동기화
	//EnterCriticalSection( &m_General_LOCK );
	
	if( (pClanFamily = GetFamily( pUser )) != NULL )
	{
		pClanFamily->SetLogOutMember( pUser );
	}

	// 클랜 접속 정보 업데이트
	//UpdateClanInfoList();

#ifdef __USE_ZLOG__
	int iClanCount = 0;
	int iFamilyCount = 0;
	int iLoginMemberCount = 0;
	CClanMember* pClanMember = NULL;
	ZList* pMemberList = NULL;


	g_WatchData.Log( " " );
	g_WatchData.Log( "<< 로그인 클랜들 >>" );
	iFamilyCount = m_zlClanFamily.Count();
	g_WatchData.Log( " - 클랜 수:%d", iFamilyCount );
	for( int i = 0; i < iFamilyCount; i++ )
	{
		if( (pClanFamily = (CClanFamily*)m_zlClanFamily.IndexOf(i)) != NULL )
		{
			if( pClanFamily->IsUse() )
				g_WatchData.Log( " -*- 클랜명:%s - USE:TRUE", (pClanFamily->GetName()).c_str() );
			else
				g_WatchData.Log( " -*- 클랜명:%s - USE:false", (pClanFamily->GetName()).c_str() );


			if( (pMemberList = pClanFamily->GetMemberList()) != NULL )
			{
				iLoginMemberCount = pMemberList->Count();
				for( int j = 0; j < iLoginMemberCount; j++ )
				{
					if( (pClanMember = (CClanMember*)pMemberList->IndexOf(j)) != NULL )
					{
						if( pClanMember->IsUse() )
							g_WatchData.Log( " --- %02d:멤버(%d):%s - USE:TRUE", j+1, pClanMember->GetGrade(), pClanMember->GetName() );
						else
							g_WatchData.Log( " --- %02d:멤버(%d):%s - USE:false", j+1, pClanMember->GetGrade(), pClanMember->GetName() );
					}
				}
			}
		}
	}
#endif

	//LeaveCriticalSection(&m_General_LOCK);

	return TRUE;
}

//---------------------------------------------------------------------------
// 클랜전 포기한 메시지 전달
//---------------------------------------------------------------------------

BOOL CClanWar::ClanWarRejoinGiveUp( classUSER* pUser, DWORD dwWinTeam )
{
	CClanWarRoom* pClanWarRoom = NULL;


	pClanWarRoom = GetClanWarRoomByClanID( pUser->GetClanID() );

	if( pClanWarRoom )
	{
		// 클랜전 진행 중이면 상대편 승리
		if( dwWinTeam == CWTT_A_TEAM )
		{
			// B팀 포기 --> A팀 승리 //
			pClanWarRoom->SetWarResult(	CWWR_WAR_RESULT_AWIN );
			DropClanWar( pClanWarRoom, CWTT_ALL );
			// A 팀 퀘스트 승리 처리
			WinningRewardQuest( pClanWarRoom, CWTT_A_TEAM );
			// 클라이언트 클랜전 참가 요청창 닫기
			pClanWarRoom->StartTimeOut();
			// 마스터의 기권을 알려준다.
			pClanWarRoom->ClanWarRejoinGiveUp( CWTT_A_TEAM );
		}
		else if( dwWinTeam == CWTT_B_TEAM )
		{
			// A팀 포기 --> B팀 승리 //
			pClanWarRoom->SetWarResult(	CWWR_WAR_RESULT_BWIN );
			DropClanWar( pClanWarRoom, CWTT_ALL );
			// A 팀 퀘스트 승리 처리
			WinningRewardQuest( pClanWarRoom, CWTT_B_TEAM );
			// 클라이언트 클랜전 참가 요청창 닫기
			pClanWarRoom->StartTimeOut();
			// 마스터의 기권을 알려준다.
			pClanWarRoom->ClanWarRejoinGiveUp( CWTT_B_TEAM );
		}
	}

	return TRUE;
}

//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
// NPC에 해당하는 맵 데이타 템플릿을 얻는다.
//---------------------------------------------------------------------------

CClanWarRoomTemplate* CClanWar::GetWarRoomTemplate( CObjNPC* pNPC )
{
	int iClientNpcNo = 0;	// 클라이언트에서 신청한 NPC 번호
	int iWarRoomCount = 0;
	int iServerNpcNo = 0;	// 서버 워룸 템플릿 NPC 번호
	CClanWarRoomTemplate* pWarRoomTemplate = NULL;

	iClientNpcNo = pNPC->Get_CharNO();
	iWarRoomCount = m_zlWarRoomTemplate.Count();

	for( int i = 0; i < iWarRoomCount; i++ )
	{
		pWarRoomTemplate = (CClanWarRoomTemplate*)m_zlWarRoomTemplate.IndexOf( i );
		if( pWarRoomTemplate )
		{
			CObjNPC* pWarRoomNPC = NULL;

			pWarRoomNPC = pWarRoomTemplate->GetNPC();
			if( pWarRoomNPC )
			{
				iServerNpcNo = pWarRoomNPC->Get_CharNO();

				// 클라이언트 NPC 번호와 서버 NPC 번호가 일치하는가?
				if( iClientNpcNo == iServerNpcNo )
					return pWarRoomTemplate;
			}
			else return NULL;
		}
		else return NULL;
	}

	return NULL;
}

//---------------------------------------------------------------------------

BOOL CClanWar::CheckUserLevel( CObjNPC* pNPC, DWORD dwWarType, DWORD dwUserLevel )
{
	DWORD dwNPC_NO = 0;
	CClanWarRoomTemplate* pWarRoomTemplate = NULL;

	// NPC에 해당하는 맵 데이타 템플릿을 얻는다.
	pWarRoomTemplate = GetWarRoomTemplate( pNPC );
	if( pWarRoomTemplate )
	{
		DWORD dwRestrictLevel = 0;

		dwRestrictLevel = pWarRoomTemplate->GetWarTypeRestrictLevel( dwWarType );
		// 허용 가능한 레벨인가?
		if( dwRestrictLevel >= dwUserLevel )
			return TRUE;
	}

	return FALSE;
}

//---------------------------------------------------------------------------
// 클라이언트 클랜워 타입 -> 서버 클랜워 타입으로 변환

DWORD CClanWar::ChangeClientToServerClanWarType( DWORD dwClientClanWarType )
{
	switch( dwClientClanWarType )
	{
	case 1:
	case 2:
		//	배팅비 클래전
		return 0;
	case 31:
	case 32:
		//	배팅비 클래전
		return 1;
	case 61:
	case 62:
		//	배팅비 클래전
		return 2;

	default:
		return 0;
/*
	case 3:
	case 4:
		//	참여비 클래전
		return 3;
	case 33:
	case 34:
		//	참여비 클래전
		return 4;
	case 63:
	case 64:
		//	참여비 클래전
		return 5;
*/
	}

	return 0;
}

//---------------------------------------------------------------------------
// 클랜전을 신청하고 기다리는 방이 있는지 채크

CClanWarRoom* CClanWar::CheckWarRoomReservation( CObjNPC* pNPC, DWORD dwClanWarType )
{
	CClanWarRoom* pWarRoom = NULL;
	int iRoomCount = m_zlWarRoom.Count();

	// 남은 방이 있는가?
	if( iRoomCount < 1 )
		return NULL;

	for( int i = 0; i < iRoomCount; i++ )
	{
		pWarRoom = (CClanWarRoom*)m_zlWarRoom.IndexOf( i );
		if( pWarRoom )
		{
			if( pWarRoom->IsUse() )
			{
				CObjNPC* pRoomNPC = NULL;

				pRoomNPC = pWarRoom->GetNPC();
				if( pRoomNPC )
				{
					// 동일한 NPC가 만든 맵인가?
					if( pRoomNPC->Get_CharNO() == pNPC->Get_CharNO() )
					{
						// 동일한 타입의 클랜전인가?
						if( pWarRoom->GetWarType() == dwClanWarType )
						{
							// 이미 신청한 A 팀이 있고 A 팀 마스터가 있으며 접속한 상태일때 클랜전 매칭
							if( pWarRoom->GetATeam() && pWarRoom->GetATeam()->IsUse() && 
								pWarRoom->GetATeam()->GetMaster() && pWarRoom->GetATeam()->GetMaster()->IsUse() )
							{
								// 예약중인 방인가?
								if( pWarRoom->GetPlayType() == CWPT_TEAM_A_APPLY )
								{
									// 동일한 방이다!!!
									return pWarRoom;
								}
							}
						}
					}
				}
			}
		}
	}

	return NULL;
}

//---------------------------------------------------------------------------
// 남은 방이 있는지 채크 없으면 만들어서라도 리턴해 준다.

CClanWarRoom* CClanWar::CheckVacantWarRoom( CObjNPC* pNPC, DWORD& dwRoomIDX )
{
	int iRoomCount = 0;
	CClanWarRoom* pWarRoom = NULL;
	CClanWarRoomTemplate* pWarRoomTemplate = NULL;

	// 진행중인 방의 템플릿 정보 가져오기
	pWarRoomTemplate = GetWarRoomTemplate( pNPC );
	if( pWarRoomTemplate )
	{
		// 최대 방수를 초과했는지 채크
		if( GetLiveClanWarRoom() < (int)pWarRoomTemplate->GetMAX_ClanWarNumber() )
		{
			// 남은 방을 찾아서 돌려준다.
			iRoomCount = m_zlWarRoom.Count();
			for( int i = 0; i < iRoomCount; i++ )
			{
				// pWarRoom을 방 찾기에 재활용
				pWarRoom = (CClanWarRoom*)m_zlWarRoom.IndexOf(i);
				if( pWarRoom )
				{
					// 사용중이지 않은 방 발견!
					if( pWarRoom->IsUse() == FALSE )
					{
#ifdef __USE_ZLOG__
						g_WatchData.Log( "%s : 기존맵 초기화(ROOM_IDX:%d)", __FUNCTION__, i );
#endif
						// 방 청소!
						pWarRoom->DataClear();
						dwRoomIDX = i; // 존 번호 생성을 위해 룸 인덱스 추출

						return pWarRoom;
					}
				}
			}

			// 방 추가!
			if( (pWarRoom = new CClanWarRoom()) == NULL )
				return NULL;
			pWarRoom->DataClear();
			m_zlWarRoom.Add( pWarRoom );
			// 존 번호 생성을 위해 룸 인덱스 추출
			//dwRoomIDX = m_zlWarRoom.Count() - 1;
			// Ex) 101번 방이 SEED번호이면 0부터 룸 인덱스가 증가 즉, 101번 맵이 첫번째 클랜전 맵이 된다.
			dwRoomIDX = iRoomCount; // 윗줄과 같은 의미

#ifdef __USE_ZLOG__
			g_WatchData.Log( "%s : 새 맵 초기화(ROOM_IDX:%d)", __FUNCTION__, iRoomCount );
#endif

			return pWarRoom;
		}
	}

	return NULL;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 이미 로딩된 패밀리가 있는가?
//---------------------------------------------------------------------------

CClanFamily* CClanWar::CheckTheSameFamily( classUSER* pMember )
{
	int iFamilyCount = 0;
	CClanFamily* pClanFamily = NULL;

	iFamilyCount = m_zlClanFamily.Count();
	for( int i = 0; i < iFamilyCount; i++ )
	{
		pClanFamily = (CClanFamily*)m_zlClanFamily.IndexOf(i);
		if( pClanFamily )
		{
			if( pClanFamily->IsUse() )
			{
				// 동일한 클랜명 인가?
//				if( pClanFamily->IsMember( pMember ) )
//				{
//					return pClanFamily;
//				}
				if( pClanFamily->GetName() == ZString(pMember->m_ClanNAME.Get()) )
				{
					return pClanFamily;
				}
			}
		}
	}

	return NULL;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 비어있는 패밀리 데이타가 있는가?
// 패밀리 정보를 로딩해서 사용하다가 지금은 않쓰는(m_bIsUse==FALSE) 데이타
// (20070427:남병철) 0427 이전 주석 : 사용하지 않는 패밀리 데이타가 있는가?
// (20070427:남병철) 0427 이후 주석 : 사용하지 않는 패밀리의 일원이었는가?
//---------------------------------------------------------------------------

CClanFamily* CClanWar::CheckVacantFamilyMemory( classUSER* pMember )
{
	int iFamilyCount = 0;
	CClanFamily* pClanFamily = NULL;

	iFamilyCount = m_zlClanFamily.Count();
	for( int i = 0; i < iFamilyCount; i++ )
	{
		pClanFamily = (CClanFamily*)m_zlClanFamily.IndexOf(i);
		if( pClanFamily )
		{
			if( pClanFamily->IsUse() == FALSE )
			{
				// (20070427:남병철) : 같은 클랜의 멤버였는지 채크
				if( pClanFamily->GetName() == ZString(pMember->m_ClanNAME.Get()) )
				{
					return pClanFamily;
				}
			}
		}
	}

	return NULL;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// A, B 팀 클랜 패밀리 세팅
//---------------------------------------------------------------------------

BOOL CClanWar::SetATeam( CClanWarRoom* pWarRoom, ZString zClanName )
{
	int iClanCount = 0;
	CClanFamily* pClanFamily = NULL;

	iClanCount = m_zlClanFamily.Count();
	for( int i = 0; i < iClanCount; i++ )
	{
		// 같은 클랜명을 가지고 있으면 세팅 후 리턴
		pClanFamily = (CClanFamily*)m_zlClanFamily.IndexOf(i);
		if( pClanFamily )
		{
			if( pClanFamily->GetName() == zClanName )
			{
				pWarRoom->SetATeam( pClanFamily );

				return TRUE;
			}
		}
	}

	// 같은 클랜이 없다? -0-; 누가 클랜전을 신청한걸까... -_-;
	return FALSE;
}

//---------------------------------------------------------------------------

BOOL CClanWar::SetBTeam( CClanWarRoom* pWarRoom, ZString zClanName )
{
	int iClanCount = 0;
	CClanFamily* pClanFamily = NULL;

	iClanCount = m_zlClanFamily.Count();
	for( int i = 0; i < iClanCount; i++ )
	{
		// 같은 클랜명을 가지고 있으면 세팅 후 리턴
		pClanFamily = (CClanFamily*)m_zlClanFamily.IndexOf(i);
		if( pClanFamily )
		{
			if( pClanFamily->GetName() == zClanName )
			{
				pWarRoom->SetBTeam( pClanFamily );

				return TRUE;
			}
		}
	}

	// 같은 클랜이 없다? -0-; 누가 클랜전을 신청한걸까... -_-;
	return FALSE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 진행에 필요한 메시지 전달
// dwProcessType [01:마스터에게 클랜전 참여 메시지 전달]
//               [02:클랜원들에게 클랜전 참여 메시지 전달]
//               [07:접속한 유저들에게 클랜전 진행 수치 상태 전달]
//               [10:각자의 맵으로 보낸다. (기본:주논으로 워프)]
// dwWarpType [1:A 팀 클랜원들에게 클랜전 참여 메시지 전달]
//            [2:B 팀 클랜원들에게 클랜전 참여 메시지 전달]
//---------------------------------------------------------------------------

BOOL CClanWar::SendProcessMessage( CClanWarRoom* pClanWarRoom, DWORD dwProcessType, DWORD dwWarpType )
{
	classUSER* pUser = NULL;
	CClanFamily* pAClanFamily = NULL;
	CClanFamily* pBClanFamily = NULL;
	CClanMember* pAClanMaster = NULL;
	CClanMember* pBClanMaster = NULL;
	CClanWarRoomTemplate* pClanWarTemplate = NULL;


	switch( dwProcessType )
	{
	case CWMPT_MASTER_MSG:

		// 클랜전에 참여한 두 팀이 모두 존재해야한다.
		if( (pAClanFamily = pClanWarRoom->GetATeam()) == NULL )
			return FALSE;
		if( (pBClanFamily = pClanWarRoom->GetBTeam()) == NULL )
			return FALSE;
		// 둘다 모두 존재하면 클랜전 진행
		if( pAClanFamily->IsUse() == FALSE || pBClanFamily->IsUse() == FALSE )
			return FALSE;


		// 클랜 마스터의 존재여부에 따라 클랜전 시작 형태를 결정한다.
        if( (pAClanMaster = pAClanFamily->GetMaster()) == NULL )
			return FALSE;
		if( (pBClanMaster = pBClanFamily->GetMaster()) == NULL )
			return FALSE;

		// 1. 클랜 마스터 모두 없음
		if( pAClanMaster->IsUse() == FALSE && pBClanMaster->IsUse() == FALSE )
		{
			// 클랜전 ROOM 초기화
			pClanWarRoom->DataClear();

			// 메시지 처리 실패
			return FALSE;
		}
		// 2. 클랜 마스터 모두 존재
		else if( pAClanMaster->IsUse() && pBClanMaster->IsUse() )
		{
			// 각 마스터에게 클랜전 진행 여부 묻기
			// A 팀
			Send_gsv_CLANWAR_MASTER_WARP( pClanWarRoom, CWTT_A_TEAM );
			// B 팀
			Send_gsv_CLANWAR_MASTER_WARP( pClanWarRoom, CWTT_B_TEAM );

			// 클랜전 몬스터 소환
			pClanWarTemplate = GetWarRoomTemplate( pClanWarRoom->GetNPC() );
			if( pClanWarTemplate )
			{
				// 몹 리젠 전에 청소 -> 않되네 -_-;
				//RemoveMOB( pClanWarRoom, CWTT_ALL );
				// 몹 리젠
				RegenMOB( pClanWarTemplate, pClanWarRoom );
			}
		}
		// 3. A팀 클랜 마스터 존재
		else if( pAClanMaster->IsUse() && pBClanMaster->IsUse() == FALSE )
		{
			// A 팀 보상없는 승리 처리
			WinningRewardJulyItemSetting( pClanWarRoom, CWTT_A_TEAM, FALSE );

			// A 팀 퀘스트 승리 처리
			WinningRewardQuest( pClanWarRoom, CWTT_A_TEAM );

			// 클랜전 ROOM 초기화
			pClanWarRoom->DataClear();
		}		
		// 4. B팀 클랜 마스터 존재
		else if( pAClanMaster->IsUse() == FALSE && pBClanMaster->IsUse() )
		{
			// B 팀 보상없는 승리 처리
			WinningRewardJulyItemSetting( pClanWarRoom, CWTT_B_TEAM, FALSE );

            // B 팀 퀘스트 승리 처리
			WinningRewardQuest( pClanWarRoom, CWTT_B_TEAM );

			// 클랜전 ROOM 초기화
			pClanWarRoom->DataClear();
		}
		// 에러!!
		else
		{
			// 뭐가 문제지?
		}
		break;


	case CWMPT_MEMBER_MSG:

		// 클랜전에 참여한 두 팀이 모두 존재해야한다.
		if( (pAClanFamily = pClanWarRoom->GetATeam()) == NULL )
			return FALSE;
		if( (pBClanFamily = pClanWarRoom->GetBTeam()) == NULL )
			return FALSE;
		// 둘다 모두 존재하면 클랜전 진행
		if( pAClanFamily->IsUse() == FALSE || pBClanFamily->IsUse() == FALSE )
			return FALSE;

	
		// 클랜 멤버들에게 워프 메시지 전송
		switch( dwWarpType )
		{
		case CWMWT_A_TEAM_WARP:

			// A 팀 멤버 워프
			Send_gsv_CLANWAR_MEMBER_WARP( pClanWarRoom, CWTT_A_TEAM );

			//	플레이 타입 변경
			if( pClanWarRoom->GetPlayType() == CWPT_TEAM_B_APPLY )
				pClanWarRoom->SetPlayType( CWPT_WARP_TEAM_A );
			else if( pClanWarRoom->GetPlayType() == CWPT_WARP_TEAM_A )
				pClanWarRoom->SetPlayType( CWPT_WARP_TEAM_B );

			break;

		case CWMWT_B_TEAM_WARP:

			// B 팀 멤버 워프
			Send_gsv_CLANWAR_MEMBER_WARP( pClanWarRoom, CWTT_B_TEAM );

			//	플레이 타입 변경
			if( pClanWarRoom->GetPlayType() == CWPT_TEAM_B_APPLY )
				pClanWarRoom->SetPlayType( CWPT_WARP_TEAM_A );
			else if( pClanWarRoom->GetPlayType() == CWPT_WARP_TEAM_A )
				pClanWarRoom->SetPlayType( CWPT_WARP_TEAM_B );

			break;

		default:;
		}

		break;


	case CWMPT_TIME_MSG:

		// 클랜전에 참여한 두 팀이 모두 존재해야한다.
		if( (pAClanFamily = pClanWarRoom->GetATeam()) == NULL )
			return FALSE;
		if( (pBClanFamily = pClanWarRoom->GetBTeam()) == NULL )
			return FALSE;
		// 둘중 하나라도 있으면 OK
		if( pAClanFamily->IsUse() == FALSE && pBClanFamily->IsUse() == FALSE )
			return FALSE;


		// 존재하며 클랜전 참여중인 멤버에게만 타임 메시지 전달 (마스터 & 멤버 모두)
		Send_gsv_CLANWAR_TIME_STATUS( pClanWarRoom );

		break;


	case CWMPT_RETURN_WARP_MSG:

		// 클랜전에 참여한 두 팀이 모두 존재해야한다.
		if( (pAClanFamily = pClanWarRoom->GetATeam()) == NULL )
			return FALSE;
		if( (pBClanFamily = pClanWarRoom->GetBTeam()) == NULL )
			return FALSE;
		// 둘중 하나라도 있으면 OK
		if( pAClanFamily->IsUse() == FALSE && pBClanFamily->IsUse() == FALSE )
			return FALSE;


		// 모든 팀 멤버에게 클랜전 복귀 메시지 전달
		Send_gsv_CLANWAR_RETURN_WARP( pClanWarRoom );

		break;
	}

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜몹 리젠
//---------------------------------------------------------------------------

BOOL CClanWar::RegenMOB( CClanWarRoomTemplate* pClanWarRoomTemplate, CClanWarRoom* pClanWarRoom )
{
	DWORD dwWarType = pClanWarRoom->GetWarType();
	ZList* pzlMobList = NULL;
	int iMobCount = 0;
	stClanWarMOB* pClanWarMOB = NULL;
	CObjMOB* pMob = NULL;
	
	// 각 클랜전 타입에 따른 몬스터 리젠
	// A 팀 몹 리젠
	pzlMobList = pClanWarRoomTemplate->GetATeamMOBInfo( dwWarType );
	iMobCount = pzlMobList->Count();
	for( int i = 0; i < iMobCount; i++ )
	{
		pClanWarMOB = (stClanWarMOB*)pzlMobList->IndexOf(i);
		if( pClanWarMOB )
		{
			DWORD dwZoneNo = pClanWarRoom->GetZoneNo();

			pMob = g_pZoneLIST->RegenCharacter( (short)dwZoneNo, pClanWarMOB->fPosX, 
												pClanWarMOB->fPosY, pClanWarMOB->dwRegenRange, 
												pClanWarMOB->dwMOB_IDX, pClanWarMOB->dwRegenNumber, 
												CWTT_A_TEAM );
			if( pMob )
				pClanWarRoom->AddATeamMOB( pMob );


		}
	}

	// B 팀 몹 리젠
	pzlMobList = pClanWarRoomTemplate->GetBTeamMOBInfo( dwWarType );
	iMobCount = pzlMobList->Count();
	for( int i = 0; i < iMobCount; i++ )
	{
		pClanWarMOB = (stClanWarMOB*)pzlMobList->IndexOf(i);
		if( pClanWarMOB )
		{
			DWORD dwZoneNo = pClanWarRoom->GetZoneNo();

			pMob = g_pZoneLIST->RegenCharacter( (short)dwZoneNo, pClanWarMOB->fPosX, 
												pClanWarMOB->fPosY, pClanWarMOB->dwRegenRange, 
												pClanWarMOB->dwMOB_IDX, pClanWarMOB->dwRegenNumber, 
												CWTT_B_TEAM );
			if( pMob )
				pClanWarRoom->AddBTeamMOB( pMob );
		}
	}

	// PK 모드 ON
	g_pZoneLIST->Set_PK_FLAG( (short)pClanWarRoom->GetZoneNo(), TRUE );

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 리젠몹 제거 (ZoneNo, Team구분)
// << 팀 구분:dwTeamType >>
// CWTT_ALL = 0,
// CWTT_A_TEAM = 1000,
// CWTT_B_TEAM = 2000
//---------------------------------------------------------------------------
BOOL CClanWar::RemoveMOB( CClanWarRoom* pClanWarRoom, DWORD dwTeamType )
{
	// << 클랜전 팀 구분 >>
	// 0	: 모든 팀
	// 1000 : A 팀
	// 2000 : B 팀
	classUSER* pUser = NULL;
	CClanMember* pMember = NULL;
	CZoneTHREAD* pZone = NULL;


	switch( dwTeamType )
	{
	case CWTT_ALL:
		g_pZoneLIST->Zone_MOB_Kill( (short)pClanWarRoom->GetZoneNo(), NULL, dwTeamType );
		pClanWarRoom->ClearAllTeamMOB();
		break;

	case CWTT_A_TEAM:
		g_pZoneLIST->Zone_MOB_Kill( (short)pClanWarRoom->GetZoneNo(), NULL, dwTeamType );
		pClanWarRoom->ClearATeamMOB();
		break;

	case CWTT_B_TEAM:
		g_pZoneLIST->Zone_MOB_Kill( (short)pClanWarRoom->GetZoneNo(), NULL, dwTeamType );
		pClanWarRoom->ClearBTeamMOB();
		break;

	default:;
	}

/*
	switch( dwTeamType )
	{
	case CWTT_ALL:
		pMember = pClanWarRoom->GetATeamTopGradeMember();
		pUser = pMember->GetMember();
		g_pZoneLIST->Zone_MOB_Kill( (short)pClanWarRoom->GetZoneNo(), pUser, dwTeamType );

		pMember = pClanWarRoom->GetBTeamTopGradeMember();
		pUser = pMember->GetMember();
		g_pZoneLIST->Zone_MOB_Kill( (short)pClanWarRoom->GetZoneNo(), pUser, dwTeamType );

		pClanWarRoom->ClearAllTeamMOB();
		break;

	case CWTT_A_TEAM:
		pMember = pClanWarRoom->GetATeamTopGradeMember();
		pUser = pMember->GetMember();
		g_pZoneLIST->Zone_MOB_Kill( (short)pClanWarRoom->GetZoneNo(), pUser, dwTeamType );
		pClanWarRoom->ClearATeamMOB();
		break;

	case CWTT_B_TEAM:
		pMember = pClanWarRoom->GetBTeamTopGradeMember();
		pUser = pMember->GetMember();
		g_pZoneLIST->Zone_MOB_Kill( (short)pClanWarRoom->GetZoneNo(), pUser, dwTeamType );
		pClanWarRoom->ClearBTeamMOB();
		break;

	default:;
	}
*/
	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 승리 보상
//---------------------------------------------------------------------------

BOOL CClanWar::WinningRewardJulyItemSetting( CClanWarRoom* pClanWarRoom, DWORD dwWinTeamType, BOOL bRewardItem )
{
	////	경우에 따른 보상 선택.
	WORD wJuly = 0;
	INT64 i64RewardJuly = 0;
	CClanFamily* pClanFamily = NULL;
	CObjNPC* pNPC = NULL;
	DWORD dwNPC_NO = 0;


	if( (pNPC = pClanWarRoom->GetNPC()) != NULL )
		dwNPC_NO = pNPC->Get_CharNO();

	i64RewardJuly = pClanWarRoom->GetBatJuly();
	switch( dwWinTeamType )
	{
	case CWTT_ALL: // 무승부

		// 배팅 금액의 30%를 제하고 돌려줌 (각 마스터는 15%가 줄어든 금액을 돌려받음)
		i64RewardJuly = i64RewardJuly - (3 * (i64RewardJuly / 10));
		// 보상금을 둘로 나눈다.
		i64RewardJuly = i64RewardJuly / 2;
		wJuly = (WORD)(i64RewardJuly/1000);

		// 마스터 유저의 클랜전 변수 세팅 (줄리, 아이템 보상)
		// A 팀 무승부 보상
		pClanFamily = pClanWarRoom->GetATeam();
		if( pClanFamily )
			ChangeMasterClanValue( pClanFamily, wJuly, bRewardItem, pClanWarRoom->GetWarType() );
		// B 팀 무승부 보상
		pClanFamily = pClanWarRoom->GetBTeam();
		if( pClanFamily )
			ChangeMasterClanValue( pClanFamily, wJuly, bRewardItem, pClanWarRoom->GetWarType() );

		break;

	case CWTT_A_TEAM: // A 팀 승리

		// 배팅 금액의 10%를 제하고 돌려줌 (승리팀이 모두 가짐)
		i64RewardJuly = i64RewardJuly - i64RewardJuly / 10;
		wJuly = (WORD)(i64RewardJuly/1000);

		// A 팀 승리 보상
		pClanFamily = pClanWarRoom->GetATeam();
		if( pClanFamily )
			ChangeMasterClanValue( pClanFamily, wJuly, bRewardItem, pClanWarRoom->GetWarType(), dwNPC_NO );

		break;

	case CWTT_B_TEAM: // B 팀 승리

		// 배팅 금액의 10%를 제하고 돌려줌 (승리팀이 모두 가짐)
		i64RewardJuly = i64RewardJuly - i64RewardJuly / 10;
		wJuly = (WORD)(i64RewardJuly/1000);

		// B 팀 승리 보상
		pClanFamily = pClanWarRoom->GetBTeam();
		if( pClanFamily )
			ChangeMasterClanValue( pClanFamily, wJuly, bRewardItem, pClanWarRoom->GetWarType(), dwNPC_NO );

		break;

	default:
		return FALSE;
	}

	return TRUE;
}

//---------------------------------------------------------------------------
// 마스터 클랜 변수 변경 (줄리, 아이템 보상)
//---------------------------------------------------------------------------

BOOL CClanWar::ChangeMasterClanValue( CClanFamily* pClanFamily, WORD wJuly, BOOL bRewardItem, DWORD dwWarType, DWORD dwNPC_NO )
{
	CClanMember* pClanMaster = NULL;

	// << 마스터가 없을 경우 클랜 내에서 등급이 가장 높은 사람에게 보상 지급 필요 >>
	if( (pClanMaster = pClanFamily->GetTopMember( TRUE )) != NULL )
	{
		// 1. 마스터에게 July금액(1/1000) 세팅
		pClanMaster->SetClanWarValue( 5, wJuly );

		// 2. 아이템 보상
		if( bRewardItem )
		{
			pClanMaster->SetClanWarValue( 7, (WORD)dwWarType );
			pClanMaster->SetClanWarValue( 8, (WORD)dwNPC_NO );
		}

		return TRUE;
	}

	return FALSE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 마스터 워프
//---------------------------------------------------------------------------

BOOL CClanWar::Send_gsv_CLANWAR_MASTER_WARP( CClanWarRoom* pClanWarRoom, DWORD dwTeamType )
{
	classUSER* pUser = NULL;
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;


	switch( dwTeamType )
	{
	case CWTT_A_TEAM:

		// A 팀 마스터 워프
		if( (pClanFamily = pClanWarRoom->GetATeam()) == NULL )
			return FALSE;
		if( (pClanMember = pClanFamily->GetMaster()) == NULL )
			return FALSE;
		if( (pUser = pClanMember->GetMember()) == NULL )
			return FALSE;

		pUser->Send_gsv_CLANWAR_OK( CWWT_MASTER_WARP, pClanFamily->GetIDX(), (WORD)pClanWarRoom->GetZoneNo(), CWTT_A_TEAM );

		break;

	case CWTT_B_TEAM:

		// B 팀 마스터 워프
		if( (pClanFamily = pClanWarRoom->GetBTeam()) == NULL )
			return FALSE;
		if( (pClanMember = pClanFamily->GetMaster()) == NULL )
			return FALSE;
		if( (pUser = pClanMember->GetMember()) == NULL )
			return FALSE;

		pUser->Send_gsv_CLANWAR_OK( CWWT_MASTER_WARP, pClanFamily->GetIDX(), (WORD)pClanWarRoom->GetZoneNo(), CWTT_B_TEAM );

		break;

	default:;
	}

	return TRUE;
}

//---------------------------------------------------------------------------
// 클랜전 멤버 워프
//---------------------------------------------------------------------------

BOOL CClanWar::Send_gsv_CLANWAR_MEMBER_WARP( CClanWarRoom* pClanWarRoom, DWORD dwTeamType )
{
	int iCount = 0;
	classUSER* pUser = NULL;
	ZList* pMemberList = NULL;
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;


	switch( dwTeamType )
	{
	case CWTT_A_TEAM:
		{
			// A 팀 멤버 워프
			if( (pClanFamily = pClanWarRoom->GetATeam()) == NULL )
				return FALSE;
			if( (pMemberList = pClanFamily->GetMemberList()) == NULL )
				return FALSE;

			iCount = pMemberList->Count();
			for( int i = 0; i < iCount; i++ )
			{
				if( (pClanMember = (CClanMember*)pMemberList->IndexOf(i)) != NULL )
				{
					if( pClanMember->IsUse() && pClanMember->IsMaster() == FALSE )
					{
						if( (pUser = pClanMember->GetMember()) != NULL )
						{
							// 워프
							pUser->Send_gsv_CLANWAR_OK( CWWT_MEMBER_WARP, pClanFamily->GetIDX(), (WORD)pClanWarRoom->GetZoneNo(), (WORD)dwTeamType );
						}
					}
				}
			}
		}
		break;

	case CWTT_B_TEAM:
		{
			// B 팀 멤버 워프
			if( (pClanFamily = pClanWarRoom->GetBTeam()) == NULL )
				return FALSE;
			if( (pMemberList = pClanFamily->GetMemberList()) == NULL )
				return FALSE;

			iCount = pMemberList->Count();
			for( int i = 0; i < iCount; i++ )
			{
				if( (pClanMember = (CClanMember*)pMemberList->IndexOf(i)) != NULL )
				{
					if( pClanMember->IsUse() && pClanMember->IsMaster() == FALSE )
					{
						if( (pUser = pClanMember->GetMember()) != NULL )
						{						
							// 워프
							pUser->Send_gsv_CLANWAR_OK( CWWT_MEMBER_WARP, pClanFamily->GetIDX(), (WORD)pClanWarRoom->GetZoneNo(), (WORD)dwTeamType );
						}
					}
				}
			}
		}
		break;

	default:;
	}

	return TRUE;
}

//---------------------------------------------------------------------------
// 모든 팀(A,B팀) 멤버에게 클랜전 진행 시간 전달(동기화)
//---------------------------------------------------------------------------

BOOL CClanWar::Send_gsv_CLANWAR_TIME_STATUS( CClanWarRoom* pClanWarRoom )
{
	int iCount = 0;
	classUSER* pUser = NULL;
	ZList* pMemberList = NULL;
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;


	// A 팀에 시간 전달
	if( (pClanFamily = pClanWarRoom->GetATeam()) != NULL )
	{
		// A 팀 멤버들에게 전달
		if( (pMemberList = pClanFamily->GetMemberList()) != NULL )
		{
			iCount = pMemberList->Count();
			for( int i = 0; i < iCount; i++ )
			{
				if( (pClanMember = (CClanMember*)pMemberList->IndexOf(i)) != NULL )
				{
					if( pClanMember->IsUse() && pClanMember->IsWarZone( pClanWarRoom->GetZoneNo() ) )
					{
						if( (pUser = pClanMember->GetMember()) != NULL )
						{
							pUser->Send_gsv_CLANWAR_Time( pClanWarRoom->GetPlayType(), pClanWarRoom->GetElapsedPlayTime() );
						}
					}
				}
			}
		}

	} // End of if( (pClanFamily = pClanWarRoom->GetATeam()) != NULL )


	// B 팀 멤버에게 시간 전달
	if( (pClanFamily = pClanWarRoom->GetBTeam()) != NULL )
	{
		// B 팀 멤버들에게 전달
		if( (pMemberList = pClanFamily->GetMemberList()) != NULL )
		{
			iCount = pMemberList->Count();
			for( int i = 0; i < iCount; i++ )
			{
				if( (pClanMember = (CClanMember*)pMemberList->IndexOf(i)) != NULL )
				{
					if( pClanMember->IsUse() && pClanMember->IsWarZone( pClanWarRoom->GetZoneNo() ) )
					{
						if( (pUser = pClanMember->GetMember()) != NULL )
						{
							pUser->Send_gsv_CLANWAR_Time( pClanWarRoom->GetPlayType(), pClanWarRoom->GetElapsedPlayTime() );
						}
					}
				}
			}

		}

	} // End of if( (pClanFamily = pClanWarRoom->GetBTeam()) != NULL )

	return TRUE;
}

//---------------------------------------------------------------------------
// 모든 팀 멤버에게 클랜전 복귀 메시지 전달
//---------------------------------------------------------------------------

BOOL CClanWar::Send_gsv_CLANWAR_RETURN_WARP( CClanWarRoom* pClanWarRoom )
{
	int iCount = 0;
	classUSER* pUser = NULL;
	ZList* pMemberList = NULL;
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;


	// A 팀 복귀
	if( (pClanFamily = pClanWarRoom->GetATeam()) != NULL )
	{
		// A 팀 멤버들
		if( (pMemberList = pClanFamily->GetMemberList()) != NULL )
		{
			iCount = pMemberList->Count();
			for( int i = 0; i < iCount; i++ )
			{
				if( (pClanMember = (CClanMember*)pMemberList->IndexOf(i)) != NULL )
				{
					if( pClanMember->IsUse() && pClanMember->IsWarZone( pClanWarRoom->GetZoneNo() ) )
					{
						if( (pUser = pClanMember->GetMember()) != NULL )
						{
							// 클랜전 종료
							pClanMember->SetUseWarFlag( FALSE );

							// 파티 해제
							pUser->Party_Out();
							// 체력이 0인경우 가득 채운다.
							if( pUser->Get_HP() <= 0 )
							{
								pUser->Set_HP( pUser->Get_MaxHP() );
								pUser->Send_CHAR_HPMP_INFO(FALSE);
							}

							// 임시 위치로 워프 (현재는 주논만)
							tPOINTF ptTempPos;
							ptTempPos.x = pClanWarRoom->GetDefaultReturnPos().x + (rand()%50) - 25;
							ptTempPos.y = pClanWarRoom->GetDefaultReturnPos().y + (rand()%50) - 25;
							// 해당 유저 클랜전 종료 위치로 보냄
							pUser->Reward_WARP( pClanWarRoom->GetDefaultReturnZoneNo(), ptTempPos );
							// 해당 유저 팀 번호 변경
							pUser->SetCur_TeamNO( 2 );
							pUser->Set_TeamNoFromClanIDX( 0 );
						}
					}
				}
			}
		}

	} // End of if( (pClanFamily = pClanWarRoom->GetATeam()) != NULL )


	// B 팀 복귀
	if( (pClanFamily = pClanWarRoom->GetBTeam()) != NULL )
	{
		// B 팀 멤버들
		if( (pMemberList = pClanFamily->GetMemberList()) != NULL )
		{
			iCount = pMemberList->Count();
			for( int i = 0; i < iCount; i++ )
			{
				if( (pClanMember = (CClanMember*)pMemberList->IndexOf(i)) != NULL )
				{
					if( pClanMember->IsUse() && pClanMember->IsWarZone( pClanWarRoom->GetZoneNo() ) )
					{
						if( (pUser = pClanMember->GetMember()) != NULL )
						{
							// 클랜전 종료
							pClanMember->SetUseWarFlag( FALSE );

							// 파티 해제
							pUser->Party_Out();
							// 체력이 0인경우 가득 채운다.
							if( pUser->Get_HP() <= 0 )
							{
								pUser->Set_HP( pUser->Get_MaxHP() );
								pUser->Send_CHAR_HPMP_INFO(FALSE);
							}

							// 임시 위치로 워프 (현재는 주논만)
							tPOINTF ptTempPos;
							ptTempPos.x = pClanWarRoom->GetDefaultReturnPos().x + (rand()%50) - 25;
							ptTempPos.y = pClanWarRoom->GetDefaultReturnPos().y + (rand()%50) - 25;
							// 해당 유저 클랜전 종료 위치로 보냄
							pUser->Reward_WARP( pClanWarRoom->GetDefaultReturnZoneNo(), ptTempPos );
							// 해당 유저 팀 번호 변경
							pUser->SetCur_TeamNO( 2 );
							pUser->Set_TeamNoFromClanIDX( 0 );
						}
					}
				}
			}
		}

	} // End of if( (pClanFamily = pClanWarRoom->GetATeam()) != NULL )

	return FALSE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 승리 보상 (퀘스트)
//---------------------------------------------------------------------------

BOOL CClanWar::WinningRewardQuest( CClanWarRoom* pClanWarRoom, DWORD dwTeamType )
{
	switch( dwTeamType )
	{
	case CWTT_ALL: // 무승부
		{
			switch( pClanWarRoom->GetWarType() )
			{
			case CWWT_WAR_TYPE_A:
				ChangeMasterQuest( pClanWarRoom, dwTeamType, 2817, 2811 );
				break;

			case CWWT_WAR_TYPE_B:
				ChangeMasterQuest( pClanWarRoom, dwTeamType, 2818, 2812 );
				break;

			case CWWT_WAR_TYPE_C:
				ChangeMasterQuest( pClanWarRoom, dwTeamType, 2819, 2813 );
				break;

			default:;
			}
			Send_gsv_CLANWAR_CLIENT_PROGRESS( pClanWarRoom, CWCPM_CLANWAR_TEAM_DRAW );
			SetRankPoint( pClanWarRoom );
			AddRewardPoint( pClanWarRoom );
		}
		break;

	case CWTT_A_TEAM: // A 팀 승리 처리
		{
			switch( pClanWarRoom->GetWarType() )
			{
			case CWWT_WAR_TYPE_A:
				ChangeMasterQuest( pClanWarRoom, dwTeamType, 2814, 2811 );
				break;

			case CWWT_WAR_TYPE_B:
				ChangeMasterQuest( pClanWarRoom, dwTeamType, 2815, 2812 );
				break;

			case CWWT_WAR_TYPE_C:
				ChangeMasterQuest( pClanWarRoom, dwTeamType, 2816, 2813 );
				break;

			default:;
			}
			Send_gsv_CLANWAR_CLIENT_PROGRESS( pClanWarRoom, CWCPM_CLANWAR_A_TEAM_WIN );
			SetRankPoint( pClanWarRoom );
			AddRewardPoint( pClanWarRoom );
		}
		break;

	case CWTT_B_TEAM: // B 팀 승리 처리
		{
			switch( pClanWarRoom->GetWarType() )
			{
			case CWWT_WAR_TYPE_A:
				ChangeMasterQuest( pClanWarRoom, dwTeamType, 2814, 2811 );
				break;

			case CWWT_WAR_TYPE_B:
				ChangeMasterQuest( pClanWarRoom, dwTeamType, 2815, 2812 );
				break;

			case CWWT_WAR_TYPE_C:
				ChangeMasterQuest( pClanWarRoom, dwTeamType, 2816, 2813 );
				break;

			default:;
			}
			Send_gsv_CLANWAR_CLIENT_PROGRESS( pClanWarRoom, CWCPM_CLANWAR_B_TEAM_WIN );
			SetRankPoint( pClanWarRoom );
			AddRewardPoint( pClanWarRoom );
		}
		break;

	default:;
	}

	return TRUE;
}

//---------------------------------------------------------------------------
// 클랜전 참여팀의 마스터가 보유한 퀘스트 변경 (무승부/승리/패배)
//---------------------------------------------------------------------------

BOOL CClanWar::ChangeMasterQuest( CClanWarRoom* pClanWarRoom, DWORD dwTeamType, DWORD dwAddQuestIDX, DWORD dwRemoveQuestIDX )
{
	CClanMember* pClanMaster = NULL;


	// << 마스터가 없을 경우 클랜 내에서 등급이 가장 높은 사람에게 퀘스트 지급 필요 >>
	switch( dwTeamType )
	{
	case CWTT_ALL: // 무승부!!
		{
			// A 팀 마스터의 퀘스트 세팅
			if( (pClanMaster = pClanWarRoom->GetATeamTopGradeMember( TRUE )) != NULL )
			{
				// 최고 등급자를 얻으면서 클랜전 존에 있는지 채크 했음
				if( pClanMaster->IsUse() )
				{
					// 퀘스트 제거
					if( pClanMaster->IsMaster() )
						if( dwRemoveQuestIDX > 0 )
							RemoveQuest( pClanMaster, dwRemoveQuestIDX );
					// 무승부 퀘스트 등록
					if( dwAddQuestIDX > 0 )
						AddQuest( pClanMaster, dwAddQuestIDX );
				}
			}

			// B 팀 마스터의 퀘스트 세팅
			if( (pClanMaster = pClanWarRoom->GetBTeamTopGradeMember( TRUE )) != NULL )
			{
				// 최고 등급자를 얻으면서 클랜전 존에 있는지 채크 했음
				if( pClanMaster->IsUse() )
				{
					// 퀘스트 제거
					if( pClanMaster->IsMaster() )
						if( dwRemoveQuestIDX > 0 )
							RemoveQuest( pClanMaster, dwRemoveQuestIDX );
					// 무승부 퀘스트 등록
					if( dwAddQuestIDX > 0 )
						AddQuest( pClanMaster, dwAddQuestIDX );
				}
			}
		}
		break;

	case CWTT_A_TEAM:
		{
			// A 팀 마스터의 퀘스트 세팅
			if( (pClanMaster = pClanWarRoom->GetATeamTopGradeMember( TRUE )) != NULL )
			{
				// 최고 등급자를 얻으면서 클랜전 존에 있는지 채크 했음
				if( pClanMaster->IsUse() )
				{
					// 퀘스트 제거
					if( pClanMaster->IsMaster() )
						if( dwRemoveQuestIDX > 0 )
							RemoveQuest( pClanMaster, dwRemoveQuestIDX );
					// A 팀 승리 퀘스트 등록
					if( dwAddQuestIDX > 0 )
						AddQuest( pClanMaster, dwAddQuestIDX );
				}
			}

			// B 팀 마스터의 퀘스트 세팅
			if( (pClanMaster = pClanWarRoom->GetBTeamTopGradeMember( TRUE )) != NULL )
			{
				// 최고 등급자를 얻으면서 클랜전 존에 있는지 채크 했음
				if( pClanMaster->IsUse() )
				{
					// 퀘스트 제거
					if( pClanMaster->IsMaster() )
						if( dwRemoveQuestIDX > 0 )
							RemoveQuest( pClanMaster, dwRemoveQuestIDX );
				}
			}
		}
		break;

	case CWTT_B_TEAM:
		{
			// B 팀 마스터의 퀘스트 세팅
			if( (pClanMaster = pClanWarRoom->GetBTeamTopGradeMember( TRUE )) != NULL )
			{
				// 최고 등급자를 얻으면서 클랜전 존에 있는지 채크 했음
				if( pClanMaster->IsUse() )
				{
					// 퀘스트 제거
					if( pClanMaster->IsMaster() )
						if( dwRemoveQuestIDX > 0 )
							RemoveQuest( pClanMaster, dwRemoveQuestIDX );
					// B 팀 승리 퀘스트 등록
					if( dwAddQuestIDX > 0 )
						AddQuest( pClanMaster, dwAddQuestIDX );
				}
			}

			// A 팀 마스터의 퀘스트 세팅
			if( (pClanMaster = pClanWarRoom->GetATeamTopGradeMember( TRUE )) != NULL )
			{
				// 최고 등급자를 얻으면서 클랜전 존에 있는지 채크 했음
				if( pClanMaster->IsUse() )
				{
					// 퀘스트 제거
					if( pClanMaster->IsMaster() )
						if( dwRemoveQuestIDX > 0 )
							RemoveQuest( pClanMaster, dwRemoveQuestIDX );
				}
			}
		}
		break;

	default:;
	}

	return TRUE;
}

//---------------------------------------------------------------------------
// 클랜전 퀘스트 삭제
//---------------------------------------------------------------------------

BOOL CClanWar::RemoveQuest( CClanMember* pClanMember, DWORD dwQuestIDX )
{
	DWORD dwQuestPos = 0;
	classUSER* pUser = NULL;


	if( (pUser = pClanMember->GetMember()) != NULL )
	{
		// 삭제할 퀘스트의 위치 얻기
		dwQuestPos = (DWORD)pUser->Quest_GetRegistered( (int)dwQuestIDX );
		if( dwQuestPos < QUEST_PER_PLAYER )
		{
			// 퀘스트 삭제
			pUser->Quest_Delete( (int)dwQuestIDX );
			// 유저에게 통보
			pUser->Send_gsv_QUEST_REPLY( RESULT_QUEST_REPLY_DEL_SUCCESS, (BYTE)dwQuestPos, (int)dwQuestIDX );
		}
	}
	else return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------
// 클랜전 퀘스트 등록
//---------------------------------------------------------------------------

BOOL CClanWar::AddQuest( CClanMember* pClanMember, DWORD dwQuestIDX )
{
	DWORD dwQuestPos = 0;
	classUSER* pUser = NULL;


	if( (pUser = pClanMember->GetMember()) != NULL )
	{
		// 퀘스트 등록
		dwQuestPos = pUser->Quest_Append( (int)dwQuestIDX );
		// 유저에게 통보
		pUser->Send_gsv_QUEST_REPLY( RESULT_QUEST_REPLY_ADD_SUCCESS, (BYTE)dwQuestPos, (int)dwQuestIDX );
	}
	else return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------
// 클랜전 클라이언트 진행 메시지
//---------------------------------------------------------------------------

BOOL CClanWar::Send_gsv_CLANWAR_CLIENT_PROGRESS( CClanWarRoom* pClanWarRoom, DWORD dwClientProgress )
{
	ZString zReward;


	// DB 기록
	switch( dwClientProgress )
	{
	case CWCPM_CLANWAR_A_TEAM_WIN: // A 팀 승리
		// (20070523:남병철) : 클랜전 참여 캐릭터의 보상 포인트 부여전 수치 기록
		zReward = pClanWarRoom->ClanWarResult();
		GF_DB_CWar_Insert( 4, 2, pClanWarRoom->GetBatJuly(), pClanWarRoom->GetATeam()->GetIDX(), pClanWarRoom->GetBTeam()->GetIDX(), zReward.c_str() );
		break;

	case CWCPM_CLANWAR_B_TEAM_WIN: // B 팀 승리
		// (20070523:남병철) : 클랜전 참여 캐릭터의 보상 포인트 부여전 수치 기록
		zReward = pClanWarRoom->ClanWarResult();
		GF_DB_CWar_Insert( 4, 3, pClanWarRoom->GetBatJuly(), pClanWarRoom->GetATeam()->GetIDX(), pClanWarRoom->GetBTeam()->GetIDX(), zReward.c_str() );
		break;

	case CWCPM_CLANWAR_TEAM_DRAW: // 무승부
		// (20070523:남병철) : 클랜전 참여 캐릭터의 보상 포인트 부여전 수치 기록
		zReward = pClanWarRoom->ClanWarResult();
		GF_DB_CWar_Insert( 4, 1, pClanWarRoom->GetBatJuly(), pClanWarRoom->GetATeam()->GetIDX(), pClanWarRoom->GetBTeam()->GetIDX(), zReward.c_str() );
		break;

	default:;
	}


	int iCount = 0;
	classUSER* pUser = NULL;
	ZList* pMemberList = NULL;
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;


	// A 팀 복귀
	if( (pClanFamily = pClanWarRoom->GetATeam()) != NULL )
	{
		// A 팀 멤버들
		if( (pMemberList = pClanFamily->GetMemberList()) != NULL )
		{
			iCount = pMemberList->Count();
			for( int i = 0; i < iCount; i++ )
			{
				if( (pClanMember = (CClanMember*)pMemberList->IndexOf(i)) != NULL )
				{
					if( pClanMember->IsUse() && pClanMember->IsWarZone( pClanWarRoom->GetZoneNo() ) )
					{
						if( (pUser = pClanMember->GetMember()) != NULL )
						{
							// 클라이언트에 진행 상태 전송
							pUser->Send_gsv_CLANWAR_Progress( (WORD)dwClientProgress );

							// 마스터인 경우
							if( pUser->Is_ClanMASTER() )
								pClanMember->SetClanWarValue( 6, 0 );

						}
					}
				}
			}
		}

	} // End of if( (pClanFamily = pClanWarRoom->GetATeam()) != NULL )


	// B 팀 복귀
	if( (pClanFamily = pClanWarRoom->GetBTeam()) != NULL )
	{
		// B 팀 멤버들
		if( (pMemberList = pClanFamily->GetMemberList()) != NULL )
		{
			iCount = pMemberList->Count();
			for( int i = 0; i < iCount; i++ )
			{
				if( (pClanMember = (CClanMember*)pMemberList->IndexOf(i)) != NULL )
				{
					if( pClanMember->IsUse() && pClanMember->IsWarZone( pClanWarRoom->GetZoneNo() ) )
					{
						if( (pUser = pClanMember->GetMember()) != NULL )
						{
							// 클라이언트에 진행 상태 전송
							pUser->Send_gsv_CLANWAR_Progress( (WORD)dwClientProgress );

							// 마스터인 경우
							if( pUser->Is_ClanMASTER() )
								pClanMember->SetClanWarValue( 6, 0 );
						}
					}
				}
			}
		}

	} // End of if( (pClanFamily = pClanWarRoom->GetATeam()) != NULL )

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 랭킹 포인트 세팅
//---------------------------------------------------------------------------

BOOL CClanWar::SetRankPoint( CClanWarRoom* pClanWarRoom )
{
	CClanMember* pATeamMember = NULL;
	CClanMember* pBTeamMember = NULL;
	classUSER* pAUser = NULL;
	classUSER* pBUser = NULL;
	DWORD dwARankPoint = 1000, dwBRankPoint = 1000;
	BOOL bNewA = TRUE, bNewB = TRUE;


	pATeamMember = pClanWarRoom->GetATeamTopGradeMember( TRUE );
	pBTeamMember = pClanWarRoom->GetBTeamTopGradeMember( TRUE );

	// 랭킹 포인트 세팅
	if( pATeamMember != NULL )
	{
		if( (pAUser = pATeamMember->GetMember()) != NULL )
		{
			if( pAUser->m_CLAN.m_iRankPoint > 0 )
			{
				dwARankPoint = (DWORD)pAUser->m_CLAN.m_iRankPoint;
				bNewA = FALSE;
			}
		}
	}
	if( pBTeamMember != NULL )
	{
		if( (pBUser = pBTeamMember->GetMember()) != NULL )
		{
			if( pBUser->m_CLAN.m_iRankPoint > 0 )
			{
				dwBRankPoint = (DWORD)pBUser->m_CLAN.m_iRankPoint;
				bNewB = FALSE;
			}
		}
	}

	// 랭크 포인트 비율
	FLOAT fRateA = GetATeamRankPointRate( dwARankPoint, dwBRankPoint );
	FLOAT fRateB = 1.f - fRateA;

	// 포인트 계산 결과값
	FLOAT fWin = 0.f, fLost = 0.f;

	// 클랜전 종류(렙제한)에 따른 비율 변수
	FLOAT fWinRate = 1.f, fLostRate = 1.f;

	// 랭크 포인트 계산값 (더해지는 값)
	int iAddRate = 0;

	// 클랜전 진행 종류에 따라서 비율 변경
	switch( pClanWarRoom->GetWarType() )
	{
	case CWWT_WAR_TYPE_A:
		fWinRate = 0.2f;
		fLostRate = 0.5f;
		break;

	case CWWT_WAR_TYPE_B:
		fWinRate = 0.5f;
		fLostRate = 0.8f;
		break;

	case CWWT_WAR_TYPE_C:
		fWinRate = 1.0f;
		fLostRate = 1.0f;
		break;

	default:;
	}


	// 랭킹 포인트 승점 처리
	switch( pClanWarRoom->GetWarResult() )
	{
	case CWWR_WAR_RESULT_AWIN:
		{
			fLost = 30.f * fRateB + ((FLOAT)dwBRankPoint) * 0.01f; // 진팀의 손실 점수
			fWin = fLost * (1.f - fRateA) + 1.f; // 이긴팀의 손실 점수

			if( pAUser != NULL )
			{
				iAddRate = (int)(fWin * fWinRate);
				dwARankPoint = dwARankPoint + iAddRate;
				pAUser->m_CLAN.m_iRankPoint = (int)dwARankPoint;
				if( bNewA )
				{
					pAUser->SetClanRank( (int)dwARankPoint );
					// (20070430:남병철) : 랭킹 포인트 로그
					g_LOG.CS_ODS( 0xffff, "\n[A] RANK_POINT(%d) Clan:(%d)(%s)", (int)dwARankPoint, pAUser->GetClanID(), pAUser->GetClanNAME() );
				}
				else
				{
					pAUser->SetClanRank( iAddRate );
					// (20070430:남병철) : 랭킹 포인트 로그
					g_LOG.CS_ODS( 0xffff, "\n[A] RANK_POINT(%d) Clan:(%d)(%s)", iAddRate, pAUser->GetClanID(), pAUser->GetClanNAME() );
				}
			}

			if( pBUser != NULL )
			{
				iAddRate = - (int)(fLost * fLostRate);
				if( (dwBRankPoint * iAddRate) < 0 )
				{
					// 1에 맞춘다.
					iAddRate = -1 * dwBRankPoint + 1;
					dwBRankPoint = 1;
				}
				else
					dwBRankPoint = dwBRankPoint + iAddRate;
				pBUser->m_CLAN.m_iRankPoint = (int)dwBRankPoint;
				if( bNewB )
				{
					pBUser->SetClanRank( (int)dwBRankPoint );
					// (20070430:남병철) : 랭킹 포인트 로그
					g_LOG.CS_ODS( 0xffff, "\n[B] RANK_POINT(%d) Clan:(%d)(%s)", (int)dwBRankPoint, pBUser->GetClanID(), pBUser->GetClanNAME() );
				}
				else
				{
					pBUser->SetClanRank( iAddRate );
					// (20070430:남병철) : 랭킹 포인트 로그
					g_LOG.CS_ODS( 0xffff, "\n[B] RANK_POINT(%d) Clan:(%d)(%s)", iAddRate, pBUser->GetClanID(), pBUser->GetClanNAME() );
				}
			}
		}
		break;

	case CWWR_WAR_RESULT_BWIN:
		{
			fLost = 30.f * fRateA + ((FLOAT)dwARankPoint) * 0.01f; // 진팀의 손실 점수
			fWin = fLost * (1.f - fRateB) + 1.f; // 이긴팀의 손실 점수

			if( pBUser != NULL )
			{
				iAddRate = (int)(fWin * fWinRate);
				dwBRankPoint = dwBRankPoint + iAddRate;
				pBUser->m_CLAN.m_iRankPoint = dwBRankPoint;
				if( bNewB )
				{
					pBUser->SetClanRank( (int)dwBRankPoint );
					// (20070430:남병철) : 랭킹 포인트 로그
					g_LOG.CS_ODS( 0xffff, "\n[B] RANK_POINT(%d) Clan:(%d)(%s)", (int)dwBRankPoint, pBUser->GetClanID(), pBUser->GetClanNAME() );
				}
				else
				{
					pBUser->SetClanRank( iAddRate );
					// (20070430:남병철) : 랭킹 포인트 로그
					g_LOG.CS_ODS( 0xffff, "\n[B] RANK_POINT(%d) Clan:(%d)(%s)", iAddRate, pBUser->GetClanID(), pBUser->GetClanNAME() );
				}
			}

			if( pAUser != NULL )
			{
				iAddRate = - (int)(fLost * fLostRate);
				if( (dwARankPoint * iAddRate) < 0 )
				{
					// 1에 맞춘다.
					iAddRate = -1 * dwARankPoint + 1;
					dwARankPoint = 1;
				}
				else
					dwARankPoint = dwARankPoint + iAddRate;
				pAUser->m_CLAN.m_iRankPoint = dwARankPoint;
				if( bNewA )
				{
					pAUser->SetClanRank( (int)dwARankPoint );
					// (20070430:남병철) : 랭킹 포인트 로그
					g_LOG.CS_ODS( 0xffff, "\n[A] RANK_POINT(%d) Clan:(%d)(%s)", (int)dwARankPoint, pAUser->GetClanID(), pAUser->GetClanNAME() );
				}
				else
				{
					pAUser->SetClanRank( iAddRate );
					// (20070430:남병철) : 랭킹 포인트 로그
					g_LOG.CS_ODS( 0xffff, "\n[A] RANK_POINT(%d) Clan:(%d)(%s)", iAddRate, pAUser->GetClanID(), pAUser->GetClanNAME() );
				}
			}
		}
		break;

	case CWWR_WAR_RESULT_DRAW:
		{
			// 랭킹 포인트 획득 없음!
		}
		break;

	default:;
	}

	return TRUE;
}

//---------------------------------------------------------------------------
// 랭킹 포인트 비율 얻기
//---------------------------------------------------------------------------

FLOAT CClanWar::GetATeamRankPointRate( DWORD dwATeamRankPoint, DWORD dwBTeamRankPoint )
{
	//	A팀을 기준으로 처리한다.
	int	iRate = (int)(dwBTeamRankPoint - dwATeamRankPoint);


	//	비율 처리한다.
	if(iRate >= 400)		return 0.091f;
	else if(iRate >= 350)	return 0.118f;
	else if(iRate >= 300)	return 0.151f;
	else if(iRate >= 250)	return 0.192f;
	else if(iRate >= 200)	return 0.24f;
	else if(iRate >= 150)	return 0.297f;
	else if(iRate >= 100)	return 0.36f;
	else if(iRate >= 50)	return 0.429f;
	else if(iRate > -50)	return 0.5f;
	else if(iRate > -100)	return 0.571f;
	else if(iRate > -150)	return 0.64f;
	else if(iRate > -200)	return 0.703f;
	else if(iRate > -250)	return 0.76f;
	else if(iRate > -300)	return 0.808f;
	else if(iRate > -350)	return 0.849f;
	else if(iRate > -400)	return 0.882f;
	else					return 0.909f;

	return 0.5f;
}

//---------------------------------------------------------------------------
// 보상 포인트 세팅
//---------------------------------------------------------------------------
#ifdef __CLAN_REWARD_POINT
BOOL CClanWar::AddRewardPoint( CClanWarRoom* pClanWarRoom )
{
	classUSER* pUser = NULL;
	CClanMember* pClanMember = NULL;
	int iATeamRewardPoint = 0, iBTeamRewardPoint = 0;
	ZString zClanWarJoinList;	// 클랜전 참여자 리스트


	switch( pClanWarRoom->GetWarResult() )
	{
	case CWWR_WAR_RESULT_DRAW:
		// 보상 포인트 없음
		break;

	case CWWR_WAR_RESULT_AWIN:
		{
			// (20070319:남병철) : 클랜전 보상 포인트 적용
			switch( pClanWarRoom->GetWarType() )
			{
			case CWWT_WAR_TYPE_A: // 1000 줄리
				iATeamRewardPoint = 5;
				iBTeamRewardPoint = 2;
				break;

			case CWWT_WAR_TYPE_B: // 100만 줄리
				iATeamRewardPoint = 50;
				iBTeamRewardPoint = 10;
				break;

			case CWWT_WAR_TYPE_C: // 1000만 줄리
				iATeamRewardPoint = 100;
				iBTeamRewardPoint = 10;
				break;
			}
		}
		break;

	case CWWR_WAR_RESULT_BWIN:
		{
			// (20070319:남병철) : 클랜전 보상 포인트 적용
			switch( pClanWarRoom->GetWarType() )
			{
			case CWWT_WAR_TYPE_A: // 1000 줄리
				iATeamRewardPoint = 2;
				iBTeamRewardPoint = 5;
				break;

			case CWWT_WAR_TYPE_B: // 100만 줄리
				iATeamRewardPoint = 10;
				iBTeamRewardPoint = 50;
				break;

			case CWWT_WAR_TYPE_C: // 1000만 줄리
				iATeamRewardPoint = 10;
				iBTeamRewardPoint = 100;
				break;
			}
		}
		break;

	default:;
	}

	// A 팀 클랜전 보상 포인트 세팅
	if( (pClanMember = pClanWarRoom->GetATeamTopGradeMember( TRUE )) != NULL )
	{
		// A 팀 클랜전 참여자 리스트 작성
		zClanWarJoinList = pClanWarRoom->GetATeamClanWarJoinList();

		if( (pUser = pClanMember->GetMember()) != NULL )
		{
			// (20070430:남병철) : 보상 포인트 로그
			g_LOG.CS_ODS( 0xffff, "\n[A] REWARD(%d) Clan:(%d)(%s) : %s", iATeamRewardPoint, pUser->GetClanID(), 
																	   pUser->GetClanNAME(), zClanWarJoinList.c_str() );
			pUser->AddClanRewardPoint( iATeamRewardPoint, zClanWarJoinList );
		}
	}

	// B 팀
	if( (pClanMember = pClanWarRoom->GetBTeamTopGradeMember( TRUE )) != NULL )
	{
		// B 팀 클랜전 참여자 리스트 작성
		zClanWarJoinList = pClanWarRoom->GetBTeamClanWarJoinList();

		if( (pUser = pClanMember->GetMember()) != NULL )
		{
			// (20070430:남병철) : 보상 포인트 로그
			g_LOG.CS_ODS( 0xffff, "\n[B] REWARD(%d) Clan:(%d)(%s) : %s", iBTeamRewardPoint, pUser->GetClanID(), 
																	   pUser->GetClanNAME(), zClanWarJoinList.c_str() );
			pUser->AddClanRewardPoint( iBTeamRewardPoint, zClanWarJoinList );
		}
	}

	return TRUE;
}
#endif

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 문 상태 전송
//---------------------------------------------------------------------------

BOOL CClanWar::Send_gsv_GateStatus( CClanWarRoom* pClanWarRoom, classUSER* pUser, BOOL bIsGateOpen )
{
	if( pUser == NULL )
	{
		char szSendBuffer[512];
		gsv_cli_USER_MSG_1LV* pSendData = NULL;

        ZeroMemory( szSendBuffer, 512 );
		pSendData = (gsv_cli_USER_MSG_1LV*)szSendBuffer;

		// 메시지 데이타를 넣는다.
		pSendData->m_wType = GSV_USER_MSG_1LV;
		pSendData->m_nSize = sizeof( gsv_cli_USER_MSG_1LV );
		pSendData->m_Sub_Type = 100;	// 문 컨트롤 변수... (남병철:뭐지? -_-;)

		if( bIsGateOpen )
		{
			if( pClanWarRoom->GetGateFlag() )
				pSendData->m_Data = 2; // 열려있는 상태 출력
			else
				pSendData->m_Data = pClanWarRoom->GetGateFlag();
		}
		else
		{
			pSendData->m_Data = pClanWarRoom->GetGateFlag();
		}

		g_pZoneLIST->Send_gsv_All_User_Data( (short)pClanWarRoom->GetZoneNo(), (char*)pSendData );

	}
	else
	{
		if( bIsGateOpen )
		{
			if( pClanWarRoom->GetGateFlag() )
				pUser->Send_gsv_User_MSG_1LV( 100, 2 );
			else
				pUser->Send_gsv_User_MSG_1LV( 100, (WORD)pClanWarRoom->GetGateFlag() );
		}
		else
		{
			pUser->Send_gsv_User_MSG_1LV( 100, pClanWarRoom->GetGateFlag() );
		}
	}

	return TRUE;
}

//---------------------------------------------------------------------------
// 팀 HP 리셋
//---------------------------------------------------------------------------

BOOL CClanWar::ResetTeamHP( CClanWarRoom* pClanWarRoom, DWORD dwTeamType )
{
	switch( dwTeamType )
	{
	case CWTT_ALL:
		g_pZoneLIST->Zone_MOB_HP_Reset( (short)pClanWarRoom->GetZoneNo(), CWTT_A_TEAM );
		g_pZoneLIST->Zone_MOB_HP_Reset( (short)pClanWarRoom->GetZoneNo(), CWTT_B_TEAM );
		break;

	case CWTT_A_TEAM:
		g_pZoneLIST->Zone_MOB_HP_Reset( (short)pClanWarRoom->GetZoneNo(), CWTT_A_TEAM );
		break;

	case CWTT_B_TEAM:
		g_pZoneLIST->Zone_MOB_HP_Reset( (short)pClanWarRoom->GetZoneNo(), CWTT_B_TEAM );
		break;

	default:;
	}

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜워룸 가져오기 by Zone No
//---------------------------------------------------------------------------

CClanWarRoom* CClanWar::GetClanWarRoomByZoneNo( DWORD dwZoneNo )
{
	int iCount = 0;
	CClanWarRoom* pClanWarRoom = NULL;


	iCount = m_zlWarRoom.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanWarRoom = (CClanWarRoom*)m_zlWarRoom.IndexOf(i)) != NULL )
		{
			if( pClanWarRoom->IsUse() )
			{
				// 사용중인 같은 존 번호 클랜전 방이 있다!!
				if( pClanWarRoom->GetZoneNo() == dwZoneNo )
					return pClanWarRoom;
			}
		}

	}

	return NULL;
}

//---------------------------------------------------------------------------
// 클랜워룸 가져오기 by Clan ID
//---------------------------------------------------------------------------

CClanWarRoom* CClanWar::GetClanWarRoomByClanID( DWORD dwClanID )
{
	int iCount = 0;
	CClanWarRoom* pClanWarRoom = NULL;
	CClanFamily* pATeam = NULL;
	CClanFamily* pBTeam = NULL;


    iCount = m_zlWarRoom.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanWarRoom = (CClanWarRoom*)m_zlWarRoom.IndexOf(i)) != NULL )
		{
			if( pClanWarRoom->IsUse() && pClanWarRoom->GetPlayType() < CWPT_END_CLANWAR )
			{
				// A 팀의 클랜 ID와 같은가?
				if( (pATeam = pClanWarRoom->GetATeam()) != NULL )
				{
					if( pATeam->GetIDX() == dwClanID )
					{
						return pClanWarRoom;
					}
				}

				// B 팀의 클랜 ID와 같은가?
				if( (pBTeam = pClanWarRoom->GetBTeam()) != NULL )
				{
					if( pBTeam->GetIDX() == dwClanID )
					{
						return pClanWarRoom;
					}
				}
			}
		}
	}

	return NULL;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 종료처리
//---------------------------------------------------------------------------

BOOL CClanWar::DropClanWar( CClanWarRoom* pClanWarRoom, DWORD dwDropTeamType )
{
	switch( dwDropTeamType )
	{
	case CWTT_ALL:

		// 클랜전 중 보스가 죽을 경우 DropTeamType = 0으로 처리한다.
		// DropClanWar 채크하기전 클랜전 결과를 세팅해 둔다.
		switch( pClanWarRoom->GetWarResult() )
		{
		case CWWR_WAR_RESULT_DRAW: // 무승부
			WinningRewardJulyItemSetting( pClanWarRoom, CWTT_ALL, FALSE );
			break;

		case CWWR_WAR_RESULT_AWIN:
			WinningRewardJulyItemSetting( pClanWarRoom, CWTT_A_TEAM, TRUE );
			break;

		case CWWR_WAR_RESULT_BWIN:
			WinningRewardJulyItemSetting( pClanWarRoom, CWTT_B_TEAM, TRUE );
			break;

		default:;
		}

		break;


	case CWTT_A_TEAM: // A 팀 기권
		WinningRewardJulyItemSetting( pClanWarRoom, CWTT_B_TEAM, FALSE );
		pClanWarRoom->SetWarResult( CWWR_WAR_RESULT_BWIN );
		break;

	case CWTT_B_TEAM: // B 팀 기권
		WinningRewardJulyItemSetting( pClanWarRoom, CWTT_A_TEAM, FALSE );
		pClanWarRoom->SetWarResult( CWWR_WAR_RESULT_AWIN );
		break;

	default:
		return FALSE;
	}


	// 게임 종료 타임으로 변환
	CClanWarRoomTemplate* pClanWarRoomTemplate = NULL;
	DWORD dwGateTime = 0, dwPlayTime = 0, dwEndTime = 0;

	// INI에 설정된 클랜전 진행 시간 구성
	pClanWarRoomTemplate = GetWarRoomTemplate( pClanWarRoom->GetNPC() );
	if( pClanWarRoomTemplate )
	{
		dwGateTime = pClanWarRoomTemplate->GetGateTime();
		dwPlayTime = pClanWarRoomTemplate->GetPlayTime();
		dwEndTime = pClanWarRoomTemplate->GetEndTime();
	}
	else
	{
		dwGateTime = 12;
		dwPlayTime = 120;
		dwEndTime = 12;
	}

	pClanWarRoom->SetPlayType( CWPT_END_CLANWAR );
	pClanWarRoom->SetElapsedPlayTime( dwEndTime );

	// 유저에게 방상태 알림
	SendProcessMessage( pClanWarRoom, CWMPT_TIME_MSG, CWMWT_NO_WARP );

	// 몬스터 삭제
	switch( pClanWarRoom->GetWarResult() )
	{
	case CWWR_WAR_RESULT_DRAW:
		RemoveMOB( pClanWarRoom, CWTT_ALL );
		break;

	case CWWR_WAR_RESULT_AWIN:
		//RemoveMOB( pClanWarRoom, CWTT_B_TEAM );
		RemoveMOB( pClanWarRoom, CWTT_ALL );
		ResetTeamHP( pClanWarRoom, CWTT_A_TEAM );
		break;

	case CWWR_WAR_RESULT_BWIN:
		//RemoveMOB( pClanWarRoom, CWTT_A_TEAM );
		RemoveMOB( pClanWarRoom, CWTT_ALL );
		ResetTeamHP( pClanWarRoom, CWTT_B_TEAM );
		break;

	default:;
	}

	// PK 모드 오프
	g_pZoneLIST->Set_PK_FLAG( (short)pClanWarRoom->GetZoneNo(), FALSE );

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 유저가 소속된 클랜을 리턴
//---------------------------------------------------------------------------

CClanFamily* CClanWar::GetFamily( classUSER* pUser )
{
	int iCount = 0;
	CClanFamily* pClanFamily = NULL;


	iCount = m_zlClanFamily.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanFamily = (CClanFamily*)m_zlClanFamily.IndexOf(i)) != NULL )
		{
			if( pClanFamily->IsMember( pUser ) == TRUE )
				return pClanFamily;
		}
	}

	return NULL;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 진행중인 방 개수
//---------------------------------------------------------------------------

DWORD CClanWar::GetLiveClanWarRoom()
{
	int iRoomCount = 0;
	int iLiveCount = 0;
	CClanWarRoom* pClanWarRoom = NULL;


	// 최대 방수를 초과했는지 채크
	iRoomCount = m_zlWarRoom.Count();
	for( int i = 0; i < iRoomCount; i++ )
	{
		if( (pClanWarRoom = (CClanWarRoom*)m_zlWarRoom.IndexOf(i)) != NULL )
		{
			if( pClanWarRoom->IsUse() )
				iLiveCount++;
		}
	}

	return iLiveCount;
}

//---------------------------------------------------------------------------
// 클랜 정보를 서버 리스트 박스에 표시
// (크리티컬 섹션처리 함수 내에서만 사용할것)
//---------------------------------------------------------------------------

BOOL CClanWar::UpdateClanInfoList()
{
	int iClanFamilyCount = 0;
	CClanFamily* pClanFamily = NULL;
	ZString zTemp;


	iClanFamilyCount = m_zlClanFamily.Count();
	for( int i = 0; i < iClanFamilyCount; i++ )
	{
		if( (pClanFamily = (CClanFamily*)m_zlClanFamily.IndexOf(i)) != NULL )
		{
			// NO
			zTemp = i;
			FL_Set_List_Data( g_hList_Clan, i, 0, zTemp.c_str() );	// NO
			// Connected
			if( pClanFamily->IsUse() )
				zTemp = "O";
			else
				zTemp = "X";
			FL_Set_List_Data( g_hList_Clan, i, 1, zTemp.c_str() );
			// ClanID(LV)
			zTemp.Format( "%d(%d)", pClanFamily->GetIDX(), pClanFamily->GetLevel() );
			FL_Set_List_Data( g_hList_Clan, i, 2, zTemp.c_str() );
			// ClanName
			zTemp.Format( "%s", pClanFamily->GetName().c_str() );
			FL_Set_List_Data( g_hList_Clan, i, 3, zTemp.c_str() );
			// MasterConnected
			if( pClanFamily->GetMaster() && pClanFamily->GetMaster()->IsUse() )
				zTemp = "O";
			else
				zTemp = "X";
			FL_Set_List_Data( g_hList_Clan, i, 4, zTemp.c_str() );
			// MasterName(Account)
			if( pClanFamily->GetMaster() )
			{
				zTemp.Format( "%s(%s)", pClanFamily->GetMaster()->GetName(),
					pClanFamily->GetMaster()->GetAccountName() );
			}
			else
				zTemp = "";
			FL_Set_List_Data( g_hList_Clan, i, 5, zTemp.c_str() );
			// ConnectedMemberCount
			zTemp = pClanFamily->GetMemberCount();
			FL_Set_List_Data( g_hList_Clan, i, 6, zTemp.c_str() );
		}
	}

	return TRUE;
}

//---------------------------------------------------------------------------
// 클랜 진행 정보를 리스트에 표시
//---------------------------------------------------------------------------

BOOL CClanWar::UpdateClanWarList( CClanWarRoom* pClanWarRoom, int i )
{
	int iClanWarRoomCount = 0;
	//CClanWarRoom* pClanWarRoom = NULL;
	ZString zTemp;



	// ZoneNO
	zTemp.Format( "%d(%d)", pClanWarRoom->GetZoneNo(), pClanWarRoom->GetWarType() );
	FL_Set_List_Data( g_hList_CWar, i, 0, zTemp.c_str() );

	// Condition
	if( pClanWarRoom->IsUse() )
		zTemp = "O";
	else
		zTemp = "X";
	FL_Set_List_Data( g_hList_CWar, i, 1, zTemp.c_str() );

	// PlayType
	zTemp = pClanWarRoom->GetPlayType();
	FL_Set_List_Data( g_hList_CWar, i, 2, zTemp.c_str() );

	// Time
	zTemp = pClanWarRoom->GetElapsedPlayTime();
	FL_Set_List_Data( g_hList_CWar, i, 3, zTemp.c_str() );

	// (ClanID)(ATeamName)(RunningMemberCount)
	if( pClanWarRoom->GetATeam() )
		zTemp.Format( "(%d)(%s)(%d)", pClanWarRoom->GetATeam()->GetIDX(),
		pClanWarRoom->GetATeam()->GetName().c_str(), 
		pClanWarRoom->GetATeamMemberCount( TRUE ) );
	else
		zTemp.Format( "(-1)(%s)(%d)", "-", pClanWarRoom->GetATeamMemberCount( TRUE ) );
	FL_Set_List_Data( g_hList_CWar, i, 4, zTemp.c_str() );

	// (Account)(ATopGradeMemberName)
	if( pClanWarRoom->GetATeamTopGradeMember() )
		zTemp.Format( "(%s)(%s)", pClanWarRoom->GetATeamTopGradeMember()->GetAccountName(),
		pClanWarRoom->GetATeamTopGradeMember()->GetName() );
	else
		zTemp = "";
	FL_Set_List_Data( g_hList_CWar, i, 5, zTemp.c_str() );

	// (ClanID)(BTeamName)(RunningMemberCount)
	if( pClanWarRoom->GetBTeam() )
		zTemp.Format( "(%d)(%s)(%d)", pClanWarRoom->GetBTeam()->GetIDX(),
		pClanWarRoom->GetBTeam()->GetName().c_str(), 
		pClanWarRoom->GetBTeamMemberCount( TRUE ) );
	else
		zTemp.Format( "(-1)(%s)(%d)", "-", pClanWarRoom->GetBTeamMemberCount( TRUE ) );
	FL_Set_List_Data( g_hList_CWar, i, 6, zTemp.c_str() );

	// (Account)(BTopGradeMemberName)
	if( pClanWarRoom->GetBTeamTopGradeMember() )
		zTemp.Format( "(%s)(%s)", pClanWarRoom->GetBTeamTopGradeMember()->GetAccountName(),
		pClanWarRoom->GetBTeamTopGradeMember()->GetName() );
	else
		zTemp = "";
	FL_Set_List_Data( g_hList_CWar, i, 7, zTemp.c_str() );
/*
	iClanWarRoomCount = m_zlWarRoom.Count();
	for( int i = 0; i < iClanWarRoomCount; i++ )
	{
		if( (pClanWarRoom = (CClanWarRoom*)m_zlWarRoom.IndexOf(i)) != NULL )
		{
			if( pClanWarRoom == pWarRoom )
			{
				// ZoneNO
				zTemp.Format( "%d(%d)", pClanWarRoom->GetZoneNo(), pClanWarRoom->GetWarType() );
				FL_Set_List_Data( g_hList_CWar, i, 0, zTemp.c_str() );

				// Condition
				if( pClanWarRoom->IsUse() )
					zTemp = "O";
				else
					zTemp = "X";
				FL_Set_List_Data( g_hList_CWar, i, 1, zTemp.c_str() );

				// PlayType
				zTemp = pClanWarRoom->GetPlayType();
				FL_Set_List_Data( g_hList_CWar, i, 2, zTemp.c_str() );

				// Time
				zTemp = pClanWarRoom->GetElapsedPlayTime();
				FL_Set_List_Data( g_hList_CWar, i, 3, zTemp.c_str() );

				// (ClanID)(ATeamName)(RunningMemberCount)
				if( pClanWarRoom->GetATeam() )
					zTemp.Format( "(%d)(%s)(%d)", pClanWarRoom->GetATeam()->GetIDX(),
												  pClanWarRoom->GetATeam()->GetName().c_str(), 
												  pClanWarRoom->GetATeamMemberCount( TRUE ) );
				else
					zTemp.Format( "(-1)(%s)(%d)", "-", pClanWarRoom->GetATeamMemberCount( TRUE ) );
				FL_Set_List_Data( g_hList_CWar, i, 4, zTemp.c_str() );

				// (Account)(ATopGradeMemberName)
				if( pClanWarRoom->GetATeamTopGradeMember() )
					zTemp.Format( "(%s)(%s)", pClanWarRoom->GetATeamTopGradeMember()->GetAccountName(),
											  pClanWarRoom->GetATeamTopGradeMember()->GetName() );
				else
					zTemp = "";
				FL_Set_List_Data( g_hList_CWar, i, 5, zTemp.c_str() );

				// (ClanID)(BTeamName)(RunningMemberCount)
				if( pClanWarRoom->GetBTeam() )
					zTemp.Format( "(%d)(%s)(%d)", pClanWarRoom->GetBTeam()->GetIDX(),
												  pClanWarRoom->GetBTeam()->GetName().c_str(), 
												  pClanWarRoom->GetBTeamMemberCount( TRUE ) );
				else
					zTemp.Format( "(-1)(%s)(%d)", "-", pClanWarRoom->GetBTeamMemberCount( TRUE ) );
				FL_Set_List_Data( g_hList_CWar, i, 6, zTemp.c_str() );

				// (Account)(BTopGradeMemberName)
				if( pClanWarRoom->GetBTeamTopGradeMember() )
					zTemp.Format( "(%s)(%s)", pClanWarRoom->GetBTeamTopGradeMember()->GetAccountName(),
											  pClanWarRoom->GetBTeamTopGradeMember()->GetName() );
				else
					zTemp = "";
				FL_Set_List_Data( g_hList_CWar, i, 7, zTemp.c_str() );
			}
		}
	}
*/
	return TRUE;
}

//---------------------------------------------------------------------------

