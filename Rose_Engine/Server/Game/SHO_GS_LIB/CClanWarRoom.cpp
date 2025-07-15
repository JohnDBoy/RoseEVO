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
// �� û��! : ������ ����Ÿ ���� �ʱ�ȭ �Ѵ�.
//---------------------------------------------------------------------------

BOOL CClanWarRoom::DataClear()
{
	int i = 0;	// for ���� ī��Ʈ
	int iCount = 0; // ZList Count() ����

	// Ŭ���� ������?
	m_bIsUse = FALSE;

	// ����� ��
	m_bIsATeamGiveUp = FALSE;
	m_bIsBTeamGiveUp = FALSE;

	// ���° �� ������ ���� Ŭ�����ΰ�?
	m_dwZoneSeedNo = 0;

	// Ŭ���� ��û�� ���� NPC
	m_pNPC = NULL;

	// << Ŭ���� ���� ���� >>
	// 0 : Ŭ���� ���� ��
	// 1 : Ŭ���� ��û (ù��° Ŭ��)
	// 2 : Ŭ���� ��û (�ι�° Ŭ��)
	// 3,4 : Ŭ���� �����ڵ� �ش� Ŭ�� ������ ����
	// 5 : ���� ���� �ð�
	m_dwPlayType = CWPT_BEFORE_CLANWAR;

	// << Ŭ���� ���� ���� >>
	// 0 : 1000 �ٸ���
	// 1 : 100�� �ٸ���
	// 2 : 1000�� �ٸ���
	m_dwWarType = CWWT_WAR_TYPE_A;

	// ���� ���� ���� �ð� ( 1 = 10��, 1�� = 6 ) Ex) 20�� = 20 * 6 = 120
	m_dwElapsedPlayTime = 0;

	// ���� �ݾ�
	m_i64BatJuly = 0;

	// A, B �� Ŭ�� (CClanFamily*)
	m_pATeam = NULL;
	m_pBTeam = NULL;
	// ��û�� Ŭ���̾�Ʈ���� ���޵� Ŭ���� Ÿ��
	m_dwATeamWarType = 0;
	m_dwBTeamWarType = 0;

	// Ŭ���� �� ��ȣ
	m_dwZoneNo = 0;

	// << Ŭ���� ��� >>
	// 0 : ���º�
	// 1 : A �� �¸�
	// 2 : B �� �¸�
	m_dwClanWarResult = 0;

	// �巹�� HP ���� �ð� �� ���Ž� HP
	m_dwBossHPUpdateTime = 0;
	m_dwATeamBossHP = 0;
	m_dwBTeamBossHP = 0;

	// Ŭ���� ����Ʈ ����/���� ����
	m_bGateOpen = FALSE;

	// A, B �� ���� �ο� - ��� ���ҵ�... ��.�� �����ϰ� ������!
	//DWORD m_dwATeamMemberCount;
	//DWORD m_dwBTeamMemberCount;

	// �ʿ� ������ �� (CObjMOB*)
	//iCount = m_zlATeamMOB.Count();
	//for( i = 0; i < iCount; i++ )
	//	SAFE_DELETE( (CObjMOB*)m_zlATeamMOB.IndexOf(i) );
	// ���� ���� ����Ÿ ������ g_pZoneLIST->Zone_MOB_Kill() ���
	m_zlATeamMOB.Clear();

	//iCount = m_zlBTeamMOB.Count();
	//for( i = 0; i < iCount; i++ )
	//	SAFE_DELETE( (CObjMOB*)m_zlBTeamMOB.IndexOf(i) );
	// ���� ���� ����Ÿ ������ g_pZoneLIST->Zone_MOB_Kill() ���
	m_zlBTeamMOB.Clear();

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Ŭ���� ���� ��� ���� Set/Ȯ��
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
// ����� ��
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
// ���° �� ������ ���� Ŭ�����ΰ�?
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
// Ŭ���� NPC
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
// �÷��� Ÿ��
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
// Ŭ���� Ÿ��
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
// �÷��� �ð� ��� (Ŭ���� ���� ���¿� ���� �ٸ���.)
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
// �ٸ� ����
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
// ����ڰ� �ִ� �� ��ȣ ����
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
// Ŭ���� ���� ��ġ
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
// ��ȯ ��ġ
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
// << Ŭ���� ��� >>
// 0 : ���º�
// 1 : A �� �¸�
// 2 : B �� �¸�
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
// �巹�� HP ���� �ð� �� ���Ž� HP
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
// Ŭ���� ����Ʈ ����/���� ����
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
// A, B �� Ŭ�� ������
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
// ��û�� Ŭ���̾�Ʈ���� ���޵� Ŭ���� Ÿ��
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
// �ֻ��� ����� ���� (�⺻:Ŭ�� ������)
//---------------------------------------------------------------------------

CClanMember* CClanWarRoom::GetATeamTopGradeMember( BOOL bCheckClanWarZone )
{
	int iCount = 0;
	ZList* pMemberList = NULL;
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;

	
	// A ��
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


	// B ��
	if( (pClanFamily = GetBTeam()) != NULL )
	{
		if( (pClanMember = pClanFamily->GetTopMember( GetZoneNo(), bCheckClanWarZone )) != NULL )
			return pClanMember;
	}

	return NULL;
}

//---------------------------------------------------------------------------
// A,B�� Ŭ���� ��
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
// �ʿ� ������ �� (CObjMOB*)
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
// �� ����Ÿ �ʱ�ȭ
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
// ����� ������� ����
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
// �� ������ Ŭ���� ���� �ִ� ������ ����Ʈ�� �ۼ��Ѵ�.
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
		// Ŭ�� ����� Ŭ������ �����ߴ°�?
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
		// Ŭ�� ����� Ŭ������ �����ߴ°�?
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
// Ŭ���� ���� Ÿ�Ӿƿ� (Ŭ���̾�Ʈ�� ���ִ� Ŭ���� �������� �޽���â�� �ݰ��Ѵ�.)


BOOL CClanWarRoom::StartTimeOut()
{
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;


	// A �� ���� Ÿ�Ӿƿ�
	if( (pClanFamily = GetATeam()) != NULL )
	{
		if( pClanFamily->IsUse() )	
		{
			pClanFamily->StartTimeOut();

			// Ŭ���� �ۿ� �ִ� ������ ������ �ʱ�ȭ
			if( (pClanMember = pClanFamily->GetTopMember( GetZoneNo() )) != NULL )
			{
				pClanMember->SetClanWarValue( 0, 0 );
			}
		}
	}

	// B �� ���� Ÿ�Ӿƿ�
	if( (pClanFamily = GetBTeam()) != NULL )
	{
		if( pClanFamily->IsUse() )	
		{
			pClanFamily->StartTimeOut();

			// Ŭ���� �ۿ� �ִ� ������ ������ �ʱ�ȭ
			if( (pClanMember = pClanFamily->GetTopMember( GetZoneNo() )) != NULL )
			{
				pClanMember->SetClanWarValue( 0, 0 );
			}
		}
	}

	return TRUE;
}

//---------------------------------------------------------------------------
// Ŭ���� ������ �޽��� ����
//---------------------------------------------------------------------------

BOOL CClanWarRoom::ClanWarRejoinGiveUp( DWORD dwWinTeam )
{
	switch( dwWinTeam )
	{
	case CWTT_A_TEAM:

		// CWAR_OPPONENT_MASTER_GIVEUP
		// 974 : [Ŭ����] ���� Ŭ�� �����Ͱ� ����Ͽ� Ŭ������ �¸��Ͽ����ϴ�.
		if( m_pATeam )
		{
			m_pATeam->ClanWarRejoinGiveUp( CWAR_OPPONENT_MASTER_GIVEUP );
		}

		// CWAR_MASTER_GIVEUP
		// 975 : [Ŭ����] Ŭ�� �����Ͱ� ����Ͽ� Ŭ������ �й��Ͽ����ϴ�.
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
		// 975 : [Ŭ����] Ŭ�� �����Ͱ� ����Ͽ� Ŭ������ �й��Ͽ����ϴ�.
		if( m_pATeam )
		{
#if defined(__JAPAN_EVO)
			m_pATeam->ClanWarRejoinGiveUp( CWAR_MASTER_GIVEUP );
#elif defined(__KOREA_EVO)
			m_pATeam->ClanWarRejoinGiveUp( CWAR_MASTER_GIVEUP );
#endif
		}

		// CWAR_OPPONENT_MASTER_GIVEUP
		// 974 : [Ŭ����] ���� Ŭ�� �����Ͱ� ����Ͽ� Ŭ������ �¸��Ͽ����ϴ�.
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
// ���� ����Ʈ ��Ȳ m_zClanWarResult�� ���

ZString& CClanWarRoom::ClanWarResult()
{
	int iCount = 0;
	ZList* pMemberList = NULL;
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;
	ZString zATeam, zBTeam;


	// A ��
	if( (pClanFamily = GetATeam()) != NULL )
	{
		// A �� �����
		if( (pMemberList = pClanFamily->GetMemberList()) != NULL )
		{
			// A �� Ŭ�� ������ ���� ����Ʈ ����Ʈ �ۼ�
			zATeam.Format( "[A(%d):%s] ", pClanFamily->GetIDX(), pClanFamily->GetName().c_str() );

			iCount = pMemberList->Count();
			for( int i = 0; i < iCount; i++ )
			{
				if( (pClanMember = (CClanMember*)pMemberList->IndexOf(i)) != NULL )
				{
					// A �� ����� Ŭ���� ������ ���� ����Ʈ ���
					if( pClanMember->IsUse() && pClanMember->IsWarZone( GetZoneNo() ) )
					{
						zATeam.Format( "%s%s(%d) ", zATeam.c_str(), pClanMember->GetName(), pClanMember->GetRewardPoint() );						
					}
				}
			}
		}

	} // End of if( (pClanFamily = pClanWarRoom->GetATeam()) != NULL )		


	// B ��
	if( (pClanFamily = GetBTeam()) != NULL )
	{
		// B �� �����
		if( (pMemberList = pClanFamily->GetMemberList()) != NULL )
		{
			// B �� Ŭ�� ������ ���� ����Ʈ ����Ʈ �ۼ�
			zBTeam.Format( "[B(%d):%s] ", pClanFamily->GetIDX(), pClanFamily->GetName().c_str() );

			iCount = pMemberList->Count();
			for( int i = 0; i < iCount; i++ )
			{
				if( (pClanMember = (CClanMember*)pMemberList->IndexOf(i)) != NULL )
				{
					// B �� ����� Ŭ���� ������ ���� ����Ʈ ���
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


