//---------------------------------------------------------------------------

#include "ClanWar.h"

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// CClanWarRoom
//---------------------------------------------------------------------------

CClanWarRoomTemplate::CClanWarRoomTemplate()
{
	DataClear();
}

//---------------------------------------------------------------------------

CClanWarRoomTemplate::~CClanWarRoomTemplate()
{
	DataClear();
}

//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// 방 청소! : 기존의 데이타 값을 초기화 한다.
//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::DataClear()
{
	// << 로딩 데이타 >>
	int i = 0;
	int iCount = 0;

	// 클랜전 신청을 받은 NPC
	m_pNPC = NULL;

	// 시작 위치 (tPOINT*)
	m_ptATeamStartPos.x = m_ptATeamStartPos.y = 0.f; // A 팀
	m_ptBTeamStartPos.x = m_ptBTeamStartPos.y = 0.f; // B 팀

	// 부활 위치 (tPOINTF*)
	// A 팀
	iCount = m_zlATeamRevivalPos.Count();
	tPOINTF* pRevivePos = NULL;
	for( i = 0; i < iCount; i++ )
	{
		pRevivePos = (tPOINTF*)(m_zlATeamRevivalPos.IndexOf(i));
		SAFE_DELETE( pRevivePos );
	}
	m_zlATeamRevivalPos.Clear();
	// B 팀
	iCount = m_zlBTeamRevivalPos.Count();
	for( i = 0; i < iCount; i++ )
	{
		pRevivePos = (tPOINTF*)(m_zlBTeamRevivalPos.IndexOf(i));
		SAFE_DELETE( pRevivePos );
	}
	m_zlBTeamRevivalPos.Clear();

	// 기본 귀환 위치
	m_dwDefaultReturnZoneNo = 0;
	m_ptDefaultReturnPos.x = m_ptDefaultReturnPos.y = 0.f;

	// 클랜전 진행 시간
	m_dwGateTime = 0;
	m_dwPlayTime = 0;
	m_dwEndTime = 0;

	// 클랜전 WarType별 레벨 제한 (DWORD*)
	iCount = m_zlWarTypeRestrictLevel.Count();
	DWORD* pRestrictLevel = NULL;
	for( i = 0; i < iCount; i++ )
	{
		pRestrictLevel = (DWORD*)(m_zlWarTypeRestrictLevel.IndexOf(i));
		SAFE_DELETE( pRestrictLevel );
	}
	m_zlWarTypeRestrictLevel.Clear();

	// 클랜전 WarType별 배팅 금액 (INT64*)
	iCount = m_zlWarTypeBatJuly.Count();
	INT64* pBatJuly = NULL;
	for( i = 0; i < iCount; i++ )
	{
		pBatJuly = (INT64*)(m_zlWarTypeBatJuly.IndexOf(i));
		SAFE_DELETE( pBatJuly );
	}
	m_zlWarTypeBatJuly.Clear();

	// 클랜전 팀당 최대 인원 수 (기본 : 15 (설정 최대값 : 50))
	m_dwMAX_ClanWarJoiningNumberPerTeam = 0;

	// 클랜전 최대 수 (클랜전 인원 제한 50일 경우 25, 15일 경우 50) - 최대 접속자 2500명 제한 기준
	m_dwMAX_ClanWarNumber = 0;

	// 클랜전 몬스터 설정 정보 ( stClanWarMOB* )
	// A 팀 몹
	iCount = m_zlATeamWarTypeMOB.Count();
	stClanWarMOB* pMOB = NULL;
	for( i = 0; i < iCount; i++ )
	{
		pMOB = (stClanWarMOB*)(m_zlATeamWarTypeMOB.IndexOf(i));
		SAFE_DELETE( pMOB );
	}
	m_zlATeamWarTypeMOB.Clear();
	// B 팀 몹
	iCount = m_zlBTeamWarTypeMOB.Count();
	for( i = 0; i < iCount; i++ )
	{
		pMOB = (stClanWarMOB*)(m_zlBTeamWarTypeMOB.IndexOf(i));
		SAFE_DELETE( pMOB );
	}
	m_zlBTeamWarTypeMOB.Clear();

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 몇번째 맵 정보를 가진 클랜전인가?
//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::SetZoneSeedNo( DWORD dwZoneSeedNo )
{
	m_dwZoneSeedNo = dwZoneSeedNo;

	return TRUE;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoomTemplate::GetZoneSeedNo()
{
	return m_dwZoneSeedNo;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 NPC
//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::SetNPC( CObjNPC* pNPC )
{
	m_pNPC = pNPC;

	return TRUE;
}

//---------------------------------------------------------------------------

CObjNPC* CClanWarRoomTemplate::GetNPC()
{
	return m_pNPC;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 시작 위치 SET/GET
//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::SetStartPos( const FLOAT fATeamX, const FLOAT fATeamY, 
							   const FLOAT fBTeamX, const FLOAT fBTeamY )
{
	m_ptATeamStartPos.x = fATeamX;
	m_ptATeamStartPos.y = fATeamY;
	m_ptBTeamStartPos.x = fBTeamX;
	m_ptBTeamStartPos.y = fBTeamY;

	return TRUE;
}

//---------------------------------------------------------------------------

tPOINTF CClanWarRoomTemplate::GetATeamStartPos()
{
	return m_ptATeamStartPos;
}

//---------------------------------------------------------------------------

tPOINTF CClanWarRoomTemplate::GetBTeamStartPos()
{
	return m_ptBTeamStartPos;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 부활 위치 세팅
//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::AddATeamRevivalPos( FLOAT fPosX, FLOAT fPosY )
{
	tPOINTF* pRevivalPos = NULL;

	if( (pRevivalPos = new tPOINTF) == NULL )
		return FALSE;

	pRevivalPos->x = fPosX;
	pRevivalPos->y = fPosY;

	if( m_zlATeamRevivalPos.Add( (void*)pRevivalPos ) == FALSE )
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::AddBTeamRevivalPos( FLOAT fPosX, FLOAT fPosY )
{
	tPOINTF* pRevivalPos = NULL;

	if( (pRevivalPos = new tPOINTF) == NULL )
		return FALSE;

	pRevivalPos->x = fPosX;
	pRevivalPos->y = fPosY;

	if( m_zlBTeamRevivalPos.Add( (void*)pRevivalPos ) == FALSE )
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------

tPOINTF CClanWarRoomTemplate::GetATeamShortestRevivalPos( FLOAT fDeadPosX, FLOAT fDeadPosY )
{
	FLOAT fDistance = INFINITY;
	tPOINTF ptShortestRevivalPos;
	int iCount = m_zlATeamRevivalPos.Count();

	switch( iCount )
	{
	case 0: // 데이타가 없다. (0으로 초기화)
		ptShortestRevivalPos.x = 0.f;
		ptShortestRevivalPos.y = 0.f;
		break;

	case 1: // 부활 포인트 1개
		ptShortestRevivalPos = *((tPOINTF*)m_zlATeamRevivalPos.First());
		break;

	default: // 부활 포인트 1개 이상
		{
			FLOAT fTempDistance = 0;
			tPOINTF ptTempPos;

			// 부활 포인트가 1개 이상일 경우만 처리
			if( iCount > 1 )
			{
				for( int i = 0; i < iCount; i++ )
				{
					ptTempPos = *((tPOINTF*)m_zlATeamRevivalPos.IndexOf(i));
					fTempDistance = sqrt( ((fDeadPosX - ptTempPos.x)*(fDeadPosX - ptTempPos.x) + 
						(fDeadPosY - ptTempPos.y)*(fDeadPosY - ptTempPos.y)) );
					// 최소 거리 및 위치 보관
					if( fTempDistance < fDistance )
					{
						fDistance = fTempDistance;
						ptShortestRevivalPos = ptTempPos;
					}
				}
			}
			else
			{
				ptShortestRevivalPos.x = 0.f;
				ptShortestRevivalPos.y = 0.f;
			}
		}
	}

	return ptShortestRevivalPos;
}

//---------------------------------------------------------------------------

tPOINTF CClanWarRoomTemplate::GetBTeamShortestRevivalPos( FLOAT fDeadPosX, FLOAT fDeadPosY )
{
	FLOAT fDistance = INFINITY;
	tPOINTF ptShortestRevivalPos;
	int iCount = m_zlBTeamRevivalPos.Count();

	switch( iCount )
	{
	case 0: // 데이타가 없다. (0으로 초기화)
		ptShortestRevivalPos.x = 0.f;
		ptShortestRevivalPos.y = 0.f;
		break;

	case 1: // 부활 포인트 1개
		ptShortestRevivalPos = *((tPOINTF*)m_zlBTeamRevivalPos.First());
		break;

	default: // 부활 포인트 1개 이상
		{
			FLOAT fTempDistance = 0;
			tPOINTF ptTempPos;

			// 부활 포인트가 1개 이상일 경우만 처리
			if( iCount > 1 )
			{
				for( int i = 0; i < iCount; i++ )
				{
					ptTempPos = *((tPOINTF*)m_zlBTeamRevivalPos.IndexOf(i));
					fTempDistance = sqrt( ((fDeadPosX - ptTempPos.x)*(fDeadPosX - ptTempPos.x) + 
						(fDeadPosY - ptTempPos.y)*(fDeadPosY - ptTempPos.y)) );
					// 최소 거리 및 위치 보관
					if( fTempDistance < fDistance )
					{
						fDistance = fTempDistance;
						ptShortestRevivalPos = ptTempPos;
					}
				}
			}
			else
			{
				ptShortestRevivalPos.x = 0.f;
				ptShortestRevivalPos.y = 0.f;
			}
		}
	}

	return ptShortestRevivalPos;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 귀환 위치
//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::SetDefaultReturnZoneNo( DWORD dwZoneNo )
{
	m_dwDefaultReturnZoneNo = dwZoneNo;

	return TRUE;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoomTemplate::GetDefaultReturnZoneNo()
{
	return m_dwDefaultReturnZoneNo;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::SetDefaultReturnPos( FLOAT fPosX, FLOAT fPosY )
{
	m_ptDefaultReturnPos.x = fPosX;
	m_ptDefaultReturnPos.y = fPosY;

	return TRUE;
}

//---------------------------------------------------------------------------

tPOINTF CClanWarRoomTemplate::GetDefaultReturnPos()
{
	return m_ptDefaultReturnPos;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 진행 시간
//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::SetGateTime( DWORD dwGateTime )
{
	m_dwGateTime = dwGateTime;

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::SetPlayTime( DWORD dwPlayTime )
{
	m_dwPlayTime = dwPlayTime;

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::SetEndTime( DWORD dwEndTime )
{
	m_dwEndTime = dwEndTime;

	return TRUE;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoomTemplate::GetGateTime()
{
	return m_dwGateTime;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoomTemplate::GetPlayTime()
{
	return m_dwPlayTime;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoomTemplate::GetEndTime()
{
	return m_dwEndTime;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 WarType별 레벨 제한
//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::AddWarTypeRestrictLevel( DWORD dwRestrictLevel )
{
	DWORD* pdwRestrictLevel = NULL;

	if( (pdwRestrictLevel = new DWORD) == NULL )
		return FALSE;

	*pdwRestrictLevel = dwRestrictLevel;

	if( m_zlWarTypeRestrictLevel.Add( (void*)pdwRestrictLevel ) == FALSE )
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoomTemplate::GetWarTypeRestrictLevel( DWORD dwWarType )
{
	DWORD dwRestrictLevel = 0;
	DWORD dwCount = (DWORD)m_zlWarTypeRestrictLevel.Count();

	// 보유 클랜전 범위 초과
	if( dwWarType > dwCount )
		return 0;

	dwRestrictLevel = *((DWORD*)m_zlWarTypeRestrictLevel.IndexOf((int)dwWarType));

	return dwRestrictLevel;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 WarType별 배팅 금액
//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::AddWarTypeBatJuly( INT64 i64BatJuly )
{
	INT64* pi64BatJuly = NULL;

	if( (pi64BatJuly = new INT64) == NULL )
		return FALSE;

	*pi64BatJuly = i64BatJuly;

	if( m_zlWarTypeBatJuly.Add( (void*)pi64BatJuly ) == FALSE )
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------

INT64 CClanWarRoomTemplate::GetWarTypeBatJuly( DWORD dwWarType )
{
	INT64 i64BatJuly = 0;
	DWORD dwCount = (DWORD)m_zlWarTypeBatJuly.Count();

	// 보유 클랜전 범위 초과
	if( dwWarType > dwCount )
		return 0;

	i64BatJuly = *((INT64*)m_zlWarTypeBatJuly.IndexOf((int)dwWarType));

	return i64BatJuly;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 팀당 최대 인원 수 (기본 : 15 (설정 최대값 : 50))
//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::SetMAX_ClanWarJoiningNumberPerTeam( DWORD dwMaxNumber )
{
	m_dwMAX_ClanWarJoiningNumberPerTeam = dwMaxNumber;

	return TRUE;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoomTemplate::GetMAX_ClanWarJoiningNumberPerTeam()
{
	return m_dwMAX_ClanWarJoiningNumberPerTeam;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 최대 수 (클랜전 인원 제한 50일 경우 25, 15일 경우 50) - 최대 접속자 2500명 제한 기준
//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::SetMAX_ClanWarNumber( DWORD dwMaxNumber )
{
	m_dwMAX_ClanWarNumber = dwMaxNumber;

	return TRUE;
}

//---------------------------------------------------------------------------

DWORD CClanWarRoomTemplate::GetMAX_ClanWarNumber()
{
	return m_dwMAX_ClanWarNumber;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 몹 정보
//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::AddATeamMOBInfo( ZList* pzlMobData )
{
	if( m_zlATeamWarTypeMOB.Add( (void*)pzlMobData ) == FALSE )
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::AddBTeamMOBInfo( ZList* pzlMobData )
{
	if( m_zlBTeamWarTypeMOB.Add( (void*)pzlMobData ) == FALSE )
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------

ZList* CClanWarRoomTemplate::GetATeamMOBInfo( DWORD dwWarType )
{
	int iCount = m_zlATeamWarTypeMOB.Count();

	if( dwWarType < (DWORD)iCount )
	{
		return (ZList*)m_zlATeamWarTypeMOB.IndexOf( (int)dwWarType );
	}

	return NULL;
}

//---------------------------------------------------------------------------

ZList* CClanWarRoomTemplate::GetBTeamMOBInfo( DWORD dwWarType )
{
	int iCount = m_zlBTeamWarTypeMOB.Count();

	if( dwWarType < (DWORD)iCount )
	{
		return (ZList*)m_zlBTeamWarTypeMOB.IndexOf( (int)dwWarType );
	}

	return NULL;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜전 아이템 정보
//---------------------------------------------------------------------------

BOOL CClanWarRoomTemplate::AddWinningRewardItem( ZList* pzlItemData )
{
	if( m_zlRewardItems.Add( (void*)pzlItemData ) == FALSE )
		return FALSE;

	return TRUE;	
}

//---------------------------------------------------------------------------

ZList* CClanWarRoomTemplate::GetWinningRewardItem( DWORD dwWarType )
{
	int iCount = m_zlRewardItems.Count();

	if( dwWarType < (DWORD)iCount )
	{
		return (ZList*)m_zlRewardItems.IndexOf( (int)dwWarType );
	}

	return NULL;
}

//---------------------------------------------------------------------------
