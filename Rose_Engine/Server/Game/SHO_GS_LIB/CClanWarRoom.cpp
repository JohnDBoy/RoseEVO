//---------------------------------------------------------------------------

#include "ClanWar.h"

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// CClanWarRoom
//---------------------------------------------------------------------------

CClanWarRoom::CClanWarRoom()
{
	DataClear();
}

//---------------------------------------------------------------------------

CClanWarRoom::~CClanWarRoom()
{
	DataClear();
}

//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// 방 청소! : 기존의 데이타 값을 초기화 한다.
//---------------------------------------------------------------------------

BOOL CClanWarRoom::DataClear()
{
	int i = 0;	// for 루프 카운트
	int iCount = 0; // ZList Count() 보관

	// 클랜전 진행중?
	m_bIsUse = FALSE;

	// 기권한 팀
	m_bIsATeamGiveUp = FALSE;
	m_bIsBTeamGiveUp = FALSE;

	// 몇번째 맵 정보를 가진 클랜전인가?
	m_dwZoneSeedNo = 0;

	// 클랜전 신청을 받은 NPC
	m_pNPC = NULL;

	// << 클랜전 진행 종류 >>
	// 0 : 클랜전 진행 전
	// 1 : 클랜전 신청 (첫번째 클랜)
	// 2 : 클랜전 신청 (두번째 클랜)
	// 3,4 : 클랜전 참여자들 해당 클랜 맵으로 전송
	// 5 : 게임 종료 시간
	m_dwPlayType = CWPT_BEFORE_CLANWAR;

	// << 클랜전 세부 구분 >>
	// 0 : 1000 줄리전
	// 1 : 100만 줄리전
	// 2 : 1000만 줄리전
	m_dwWarType = CWWT_WAR_TYPE_A;

	// 현재 게임 남은 시간 ( 1 = 10초, 1분 = 6 ) Ex) 20분 = 20 * 6 = 120
	m_dwElapsedPlayTime = 0;

	// 배팅 금액
	m_i64BatJuly = 0;

	// A, B 팀 클랜 (CClanFamily*)
	m_pATeam = NULL;
	m_pBTeam = NULL;
	// 신청한 클라이언트에서 전달된 클랜워 타입
	m_dwATeamWarType = 0;
	m_dwBTeamWarType = 0;

	// 클랜전 존 번호
	m_dwZoneNo = 0;

	// << 클랜전 결과 >>
	// 0 : 무승부
	// 1 : A 팀 승리
	// 2 : B 팀 승리
	m_dwClanWarResult = 0;

	// 드레곤 HP 갱신 시간 및 갱신시 HP
	m_dwBossHPUpdateTime = 0;
	m_dwATeamBossHP = 0;
	m_dwBTeamBossHP = 0;

	// 클랜전 게이트 열림/닫힘 상태
	m_bGateOpen = FALSE;

	// A, B 팀 접속 인원 - 사용 않할듯... ㅡ.ㅡ 구현하고 지워라!
	//DWORD m_dwATeamMemberCount;
	//DWORD m_dwBTeamMemberCount;

	// 맵에 리젠된 몹 (CObjMOB*)
	//iCount = m_zlATeamMOB.Count();
	//for( i = 0; i < iCount; i++ )
	//	SAFE_DELETE( (CObjMOB*)m_zlATeamMOB.IndexOf(i) );
	// 몹의 실제 데이타 삭제는 g_pZoneLIST->Zone_MOB_Kill() 사용
	m_zlATeamMOB.Clear();

	//iCount = m_zlBTeamMOB.Count();
	//for( i = 0; i < iCount; i++ )
	//	SAFE_DELETE( (CObjMOB*)m_zlBTeamMOB.IndexOf(i) );
	// 몹의 실제 데이타 삭제는 g_pZoneLIST->Zone_MOB_Kill() 사용
	m_zlBTeamMOB.Clear();

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜워 룸의 사용 여부 Set/확인
//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetUseFlag( BOOL bUse )
{
	m_bIsUse = bUse;

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoom::IsUse()
{
	return m_bIsUse;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 기권한 팀
//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetTeamGiveUp( classUSER* pMaster )
{
	if( IsATeamMember( pMaster ) )
	{
		m_bIsATeamGiveUp = TRUE;
	}
	else if( IsBTeamMember( pMaster ) )
	{
		m_bIsBTeamGiveUp = TRUE;
	}

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoom::GetATeamGiveUp()
{
	return m_bIsATeamGiveUp;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoom::GetBTeamGiveUp()
{
	return m_bIsBTeamGiveUp;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 몇번째 맵 정보를 가진 클랜전인가?
//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetZoneSeedNo( DWORD dwZoneSeedNo )
{
	m_dwZoneSeedNo = dwZoneSeedNo;

	return TRUE;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoom::GetZoneSeedNo()
{
	return m_dwZoneSeedNo;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 NPC
//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetNPC( CObjNPC* pNPC )
{
	m_pNPC = pNPC;

	return TRUE;
}

//---------------------------------------------------------------------------

CObjNPC* CClanWarRoom::GetNPC()
{
	return m_pNPC;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 플레이 타입
//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetPlayType( DWORD dwPlayType )
{
	m_dwPlayType = dwPlayType;

	return TRUE;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoom::GetPlayType()
{
	return m_dwPlayType;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜워 타입
//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetWarType( DWORD dwWarType )
{
	m_dwWarType = dwWarType;

	return TRUE;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoom::GetWarType()
{
	return m_dwWarType;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 플레이 시간 상수 (클랜전 진행 상태에 따라 다르다.)
//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetElapsedPlayTime( DWORD dwElapsedPlayTime )
{
	m_dwElapsedPlayTime = dwElapsedPlayTime;

	return TRUE;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoom::GetElapsedPlayTime()
{
	return m_dwElapsedPlayTime;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 줄리 배팅
//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetBatJuly( INT64 i64BatJuly )
{
	m_i64BatJuly = i64BatJuly;

	return TRUE;
}

//---------------------------------------------------------------------------

INT64 CClanWarRoom::GetBatJuly()
{
	return m_i64BatJuly;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 사용자가 있던 존 번호 세팅
//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetZoneNo( DWORD dwZoneNo )
{
	m_dwZoneNo = dwZoneNo;

	return TRUE;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoom::GetZoneNo()
{
	return m_dwZoneNo;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 워프 위치
//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetATeamWarpPos( FLOAT fPosX, FLOAT fPosY )
{
	m_ptATeamZoneWarpPos.x = fPosX;
	m_ptATeamZoneWarpPos.y = fPosY;

	return TRUE;
}

//---------------------------------------------------------------------------

tPOINTF CClanWarRoom::GetATeamWarpPos()
{
	return m_ptATeamZoneWarpPos;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetBTeamWarpPos( FLOAT fPosX, FLOAT fPosY )
{
	m_ptBTeamZoneWarpPos.x = fPosX;
	m_ptBTeamZoneWarpPos.y = fPosY;

	return TRUE;
}

//---------------------------------------------------------------------------

tPOINTF CClanWarRoom::GetBTeamWarpPos()
{
	return m_ptBTeamZoneWarpPos;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 귀환 위치
//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetDefaultReturnZoneNo( DWORD dwZoneNo )
{
	m_dwDefaultReturnZoneNo = dwZoneNo;

	return TRUE;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoom::GetDefaultReturnZoneNo()
{
	return m_dwDefaultReturnZoneNo;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetDefaultReturnPos( FLOAT fPosX, FLOAT fPosY )
{
	m_ptDefaultReturnPos.x = fPosX;
	m_ptDefaultReturnPos.y = fPosY;

	return TRUE;
}

//---------------------------------------------------------------------------

tPOINTF CClanWarRoom::GetDefaultReturnPos()
{
	return m_ptDefaultReturnPos;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// << 클랜전 결과 >>
// 0 : 무승부
// 1 : A 팀 승리
// 2 : B 팀 승리
//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetWarResult( DWORD dwWarResult )
{
	m_dwClanWarResult = dwWarResult;

	return TRUE;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoom::GetWarResult()
{
	return m_dwClanWarResult;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 드레곤 HP 갱신 시간 및 갱신시 HP
//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetBossHPUpdateTime( DWORD dwUpdateTime )
{
	m_dwBossHPUpdateTime = dwUpdateTime;

	return TRUE;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoom::GetBossHPUpdateTime()
{
	return m_dwBossHPUpdateTime;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetATeamBossHP( DWORD dwBossHP )
{
	m_dwATeamBossHP = dwBossHP;

	return TRUE;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoom::GetATeamBossHP()
{
	return m_dwATeamBossHP;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetBTeamBossHP( DWORD dwBossHP )
{
	m_dwBTeamBossHP = dwBossHP;

	return TRUE;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoom::GetBTeamBossHP()
{
	return m_dwBTeamBossHP;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 게이트 열림/닫힘 상태
//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetGateFlag( BOOL bGateOpen )
{
	m_bGateOpen = bGateOpen;

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoom::GetGateFlag()
{
	return m_bGateOpen;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// A, B 팀 클랜 포인터
//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetATeam( CClanFamily* pClanFamily )
{
	m_pATeam = pClanFamily;

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetBTeam( CClanFamily* pClanFamily )
{
	m_pBTeam = pClanFamily;

	return TRUE;
}

//---------------------------------------------------------------------------

CClanFamily* CClanWarRoom::GetATeam()
{
	return m_pATeam;
}

//---------------------------------------------------------------------------

CClanFamily* CClanWarRoom::GetBTeam()
{
	return m_pBTeam;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 신청한 클라이언트에서 전달된 클랜워 타입
//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetATeamWarType( DWORD dwWarType )
{
	m_dwATeamWarType = dwWarType;

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoom::SetBTeamWarType( DWORD dwWarType )
{
	m_dwBTeamWarType = dwWarType;

	return TRUE;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoom::GetATeamWarType()
{
	return m_dwATeamWarType;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoom::GetBTeamWarType()
{
	return m_dwBTeamWarType;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 최상위 등급자 리턴 (기본:클랜 마스터)
//---------------------------------------------------------------------------

CClanMember* CClanWarRoom::GetATeamTopGradeMember( BOOL bCheckClanWarZone )
{
	int iCount = 0;
	ZList* pMemberList = NULL;
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;

	
	// A 팀
	if( (pClanFamily = GetATeam()) != NULL )
	{
		if( (pClanMember = pClanFamily->GetTopMember( GetZoneNo(), bCheckClanWarZone )) != NULL )
			return pClanMember;
	}

	return NULL;
}

//---------------------------------------------------------------------------

CClanMember* CClanWarRoom::GetBTeamTopGradeMember( BOOL bCheckClanWarZone )
{
	int iCount = 0;
	ZList* pMemberList = NULL;
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;


	// B 팀
	if( (pClanFamily = GetBTeam()) != NULL )
	{
		if( (pClanMember = pClanFamily->GetTopMember( GetZoneNo(), bCheckClanWarZone )) != NULL )
			return pClanMember;
	}

	return NULL;
}

//---------------------------------------------------------------------------
// A,B팀 클랜원 수
//---------------------------------------------------------------------------

DWORD CClanWarRoom::GetATeamMemberCount( BOOL bCheckClanWarZone )
{
	if( m_pATeam )
		return m_pATeam->GetMemberCount( bCheckClanWarZone, m_dwZoneNo );

	return -1;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoom::GetBTeamMemberCount( BOOL bCheckClanWarZone )
{
	if( m_pBTeam )
		return m_pBTeam->GetMemberCount( bCheckClanWarZone, m_dwZoneNo );

	return -1;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 맵에 리젠된 몹 (CObjMOB*)
//---------------------------------------------------------------------------

BOOL CClanWarRoom::AddATeamMOB( CObjMOB* pMOB )
{
	m_zlATeamMOB.Add( pMOB );

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoom::AddBTeamMOB( CObjMOB* pMOB )
{
	m_zlBTeamMOB.Add( pMOB );

	return TRUE;
}

//---------------------------------------------------------------------------

ZList* CClanWarRoom::GetATeamMOBList()
{
	return &m_zlATeamMOB;
}

//---------------------------------------------------------------------------

ZList* CClanWarRoom::GetBTeamMOBList()
{
	return &m_zlBTeamMOB;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 몹 데이타 초기화
//---------------------------------------------------------------------------

BOOL CClanWarRoom::ClearATeamMOB()
{
	int iCount = 0;
	CObjMOB* pMOB = NULL;


	//iCount = m_zlATeamMOB.Count();
	//for( int i = 0; i < iCount; i++ )
	//{
	//	if( (pMOB = (CObjMOB*)m_zlATeamMOB.IndexOf(i)) != NULL )
	//	{
	//		pMOB->Dead( NULL );
	//	}
	//}
	m_zlATeamMOB.Clear();

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoom::ClearBTeamMOB()
{
	int iCount = 0;
	CObjMOB* pMOB = NULL;


	//iCount = m_zlBTeamMOB.Count();
	//for( int i = 0; i < iCount; i++ )
	//{
	//	if( (pMOB = (CObjMOB*)m_zlBTeamMOB.IndexOf(i)) != NULL )
	//	{
	//		pMOB->Dead( NULL );
	//	}
	//}
	m_zlBTeamMOB.Clear();

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoom::ClearAllTeamMOB()
{
	ClearATeamMOB();
	ClearBTeamMOB();

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 어느팀 멤버인지 구별
//---------------------------------------------------------------------------

BOOL CClanWarRoom::IsATeamMember( classUSER* pMember )
{
	if( m_pATeam )
		return m_pATeam->IsMember( pMember );

	return FALSE;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoom::IsBTeamMember( classUSER* pMember )
{
	if( m_pBTeam )
		return m_pBTeam->IsMember( pMember );

	return FALSE;
}

//---------------------------------------------------------------------------
// 현 시점에 클랜전 존에 있는 유저를 리스트로 작성한다.
//---------------------------------------------------------------------------

ZString CClanWarRoom::GetATeamClanWarJoinList()
{
	int iCount = 0;
	ZString zJoinList;
	ZList* pMemberList = NULL;
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;


	if( (pClanFamily = GetATeam()) != NULL )
	{
		// 클랜 멤버가 클랜전에 참여했는가?
		if( (pMemberList = pClanFamily->GetMemberList()) != NULL )
		{
			iCount = pMemberList->Count();
			for( int i = 0; i < iCount; i++ )
			{
				if( (pClanMember = (CClanMember*)pMemberList->IndexOf(i)) != NULL )
				{
					if( pClanMember->IsUse() && pClanMember->IsWarZone( GetZoneNo() ) )
					{
						zJoinList += " ";
						zJoinList += pClanMember->GetName();
					}
				}
			}
		}

		if( zJoinList.IsEmpty() == FALSE )
			return zJoinList;
	}

	return "";
}

//---------------------------------------------------------------------------

ZString CClanWarRoom::GetBTeamClanWarJoinList()
{
	int iCount = 0;
	ZString zJoinList;
	ZList* pMemberList = NULL;
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;


	if( (pClanFamily = GetBTeam()) != NULL )
	{
		// 클랜 멤버가 클랜전에 참여했는가?
		if( (pMemberList = pClanFamily->GetMemberList()) != NULL )
		{
			iCount = pMemberList->Count();
			for( int i = 0; i < iCount; i++ )
			{
				if( (pClanMember = (CClanMember*)pMemberList->IndexOf(i)) != NULL )
				{
					if( pClanMember->IsUse() && pClanMember->IsWarZone( GetZoneNo() ) )
					{
						zJoinList += " ";
						zJoinList += pClanMember->GetName();
					}
				}
			}
		}

		if( zJoinList.IsEmpty() == FALSE )
			return zJoinList;
	}

	return "";
}

//---------------------------------------------------------------------------
// 클랜전 진입 타임아웃 (클라이언트에 떠있는 클랜전 참가관련 메시지창을 닫게한다.)


BOOL CClanWarRoom::StartTimeOut()
{
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;


	// A 팀 진입 타임아웃
	if( (pClanFamily = GetATeam()) != NULL )
	{
		if( pClanFamily->IsUse() )	
		{
			pClanFamily->StartTimeOut();

			// 클랜전 밖에 있는 마스터 변수값 초기화
			if( (pClanMember = pClanFamily->GetTopMember( GetZoneNo() )) != NULL )
			{
				pClanMember->SetClanWarValue( 0, 0 );
			}
		}
	}

	// B 팀 진입 타임아웃
	if( (pClanFamily = GetBTeam()) != NULL )
	{
		if( pClanFamily->IsUse() )	
		{
			pClanFamily->StartTimeOut();

			// 클랜전 밖에 있는 마스터 변수값 초기화
			if( (pClanMember = pClanFamily->GetTopMember( GetZoneNo() )) != NULL )
			{
				pClanMember->SetClanWarValue( 0, 0 );
			}
		}
	}

	return TRUE;
}

//---------------------------------------------------------------------------
// 클랜전 포기한 메시지 전달
//---------------------------------------------------------------------------

BOOL CClanWarRoom::ClanWarRejoinGiveUp( DWORD dwWinTeam )
{
	switch( dwWinTeam )
	{
	case CWTT_A_TEAM:

		// CWAR_OPPONENT_MASTER_GIVEUP
		// 974 : [클랜전] 상대방 클랜 마스터가 기권하여 클랜전에 승리하였습니다.
		if( m_pATeam )
		{
			m_pATeam->ClanWarRejoinGiveUp( CWAR_OPPONENT_MASTER_GIVEUP );
		}

		// CWAR_MASTER_GIVEUP
		// 975 : [클랜전] 클랜 마스터가 기권하여 클랜전에 패배하였습니다.
		if( m_pBTeam )
		{
#if defined(__JAPAN_EVO)
			m_pBTeam->ClanWarRejoinGiveUp( CWAR_MASTER_GIVEUP );
#elif defined(__KOREA_EVO)
			m_pBTeam->ClanWarRejoinGiveUp( CWAR_MASTER_GIVEUP );
#endif
		}
		break;


	case CWTT_B_TEAM:

		// CWAR_MASTER_GIVEUP
		// 975 : [클랜전] 클랜 마스터가 기권하여 클랜전에 패배하였습니다.
		if( m_pATeam )
		{
#if defined(__JAPAN_EVO)
			m_pATeam->ClanWarRejoinGiveUp( CWAR_MASTER_GIVEUP );
#elif defined(__KOREA_EVO)
			m_pATeam->ClanWarRejoinGiveUp( CWAR_MASTER_GIVEUP );
#endif
		}

		// CWAR_OPPONENT_MASTER_GIVEUP
		// 974 : [클랜전] 상대방 클랜 마스터가 기권하여 클랜전에 승리하였습니다.
		if( m_pBTeam )
		{
			m_pBTeam->ClanWarRejoinGiveUp( CWAR_OPPONENT_MASTER_GIVEUP );
		}
		break;

	default:;
	}


	return TRUE;
}

//---------------------------------------------------------------------------
// 보상 포인트 현황 m_zClanWarResult에 기록

ZString& CClanWarRoom::ClanWarResult()
{
	int iCount = 0;
	ZList* pMemberList = NULL;
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;
	ZString zATeam, zBTeam;


	// A 팀
	if( (pClanFamily = GetATeam()) != NULL )
	{
		// A 팀 멤버들
		if( (pMemberList = pClanFamily->GetMemberList()) != NULL )
		{
			// A 팀 클랜 참여자 보상 포인트 리스트 작성
			zATeam.Format( "[A(%d):%s] ", pClanFamily->GetIDX(), pClanFamily->GetName().c_str() );

			iCount = pMemberList->Count();
			for( int i = 0; i < iCount; i++ )
			{
				if( (pClanMember = (CClanMember*)pMemberList->IndexOf(i)) != NULL )
				{
					// A 팀 멤버의 클랜전 참여자 보상 포인트 기록
					if( pClanMember->IsUse() && pClanMember->IsWarZone( GetZoneNo() ) )
					{
						zATeam.Format( "%s%s(%d) ", zATeam.c_str(), pClanMember->GetName(), pClanMember->GetRewardPoint() );						
					}
				}
			}
		}

	} // End of if( (pClanFamily = pClanWarRoom->GetATeam()) != NULL )		


	// B 팀
	if( (pClanFamily = GetBTeam()) != NULL )
	{
		// B 팀 멤버들
		if( (pMemberList = pClanFamily->GetMemberList()) != NULL )
		{
			// B 팀 클랜 참여자 보상 포인트 리스트 작성
			zBTeam.Format( "[B(%d):%s] ", pClanFamily->GetIDX(), pClanFamily->GetName().c_str() );

			iCount = pMemberList->Count();
			for( int i = 0; i < iCount; i++ )
			{
				if( (pClanMember = (CClanMember*)pMemberList->IndexOf(i)) != NULL )
				{
					// B 팀 멤버의 클랜전 참여자 보상 포인트 기록
					if( pClanMember->IsUse() && pClanMember->IsWarZone( GetZoneNo() ) )
					{
						zBTeam.Format( "%s%s(%d) ", zBTeam.c_str(), pClanMember->GetName(), pClanMember->GetRewardPoint() );
					}
				}
			}
		}

	} // End of if( (pClanFamily = pClanWarRoom->GetATeam()) != NULL )	

	m_zClanWarResult.Format( "%s  %s", zATeam.c_str(), zBTeam.c_str() );

	return m_zClanWarResult;
}

//---------------------------------------------------------------------------


