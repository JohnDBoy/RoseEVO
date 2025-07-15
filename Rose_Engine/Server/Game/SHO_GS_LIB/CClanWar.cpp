//---------------------------------------------------------------------------

#include "ClanWar.h"

// (20070427:����ö) : Ŭ�� ���� ����Ʈ �߰��� (cpp���� ���)
#include "GF_Define.h"
#include "F_GDI_List.h"
#include "GF_Data.h"

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// CClanWar
//---------------------------------------------------------------------------

CClanWar::CClanWar()
{
	//	ũ��Ƽ�� ���� ����
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



	// Ŭ���� ���� �������� �ʱ�ȭ�� �� ����Ÿ (CClanWarRoomTemplate*)
	// (Ŭ���� ������ �ʱ�ȭ ������ ���)
	iCount = m_zlWarRoomTemplate.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanWarRoomTemplate = (CClanWarRoomTemplate*)m_zlWarRoomTemplate.IndexOf(i)) != NULL )
		{
			SAFE_DELETE( pClanWarRoomTemplate );
		}
	}
	m_zlWarRoomTemplate.Clear();

	// Ŭ���� ���� ���� (CClanWarRoom*)
	iCount = m_zlWarRoom.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanWarRoom = (CClanWarRoom*)m_zlWarRoom.IndexOf(i)) != NULL )
		{
			SAFE_DELETE( pClanWarRoom );
		}
	}
	m_zlWarRoom.Clear();

	// Ŭ������ ������ Ŭ�� ����Ʈ (CClanFamily*)
	iCount = m_zlClanFamily.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanFamily = (CClanFamily*)m_zlClanFamily.IndexOf(i)) != NULL )
		{
			SAFE_DELETE( pClanFamily );
		}
	}
	m_zlClanFamily.Clear();

	//	ũ��Ƽ�� ���� ����
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
// Ÿ�� ó�� �Լ� (10�ʿ� �ѹ��� ���´�.)
//---------------------------------------------------------------------------

BOOL CClanWar::TimeProc()
{
	// 1) ������� ���� ã�´�.
	// 2) ���� ���� ���� Ȯ���ϰ� Ÿ�� ī��Ʈ ���� �Ѵ�.
	// 3) ������ ī��Ʈ�� ����Ѵ�.
	// 4) ���� �ܰ� ó�� �Ѵ�.
	// 5) Ÿ�� �ƿ� �Ǹ� ���� ���� ��� ��Ż ��Ų��.[����]
	// 6) �ش� ���� ���� ���ϰ� ���� �����. [�ٽ� ��� �����ϰ�]

	int iCount = 0;
	CClanWarRoom* pClanWarRoom = NULL;
	CClanWarRoomTemplate* pClanWarRoomTemplate = NULL;


	ZString zTemp;
	ZStringList zslPRoomNo; // �������� Ŭ���� �� ��ȣ
	ZStringList zslPRoomFamilyMemberCount;
	ZStringList zslERoomNo; // �������� Ŭ���� �� ��ȣ
	ZStringList zslERoomFamilyMemberCount;

	// ����ȭ
	//EnterCriticalSection( &m_General_LOCK );

	iCount = m_zlWarRoom.Count();
	for( int i = 0; i < iCount; i++ )
	{
		// ���� ���� ������ �˻��Ѵ�.
		if( (pClanWarRoom = (CClanWarRoom*)m_zlWarRoom.IndexOf(i)) != NULL )
		{
			switch( pClanWarRoom->GetPlayType() )
			{
			case CWPT_TEAM_B_APPLY:
			case CWPT_WARP_TEAM_A:
			case CWPT_WARP_TEAM_B:

#ifdef __USE_ZLOG__
				// ���ȣ
				zTemp.Format( "[PLAY] - TIME(%d sec) RoomNO(%d) ZoneNO(%d) BatJuly(%d)", pClanWarRoom->GetElapsedPlayTime()*10, i, pClanWarRoom->GetZoneNo(), pClanWarRoom->GetBatJuly() );
				zslPRoomNo.Add( zTemp );
				// �� �� �ο� ����
				zTemp.Format( "[PLAY] - A Team(WAR MEMBER:%d), B Team(WAR MEMBER:%d)", 
							  pClanWarRoom->GetATeamMemberCount( TRUE ), 
							  pClanWarRoom->GetBTeamMemberCount( TRUE ) );
				zslPRoomFamilyMemberCount.Add( zTemp );
#endif

				// ���� ���� ���� ��
				pClanWarRoomTemplate = GetWarRoomTemplate( pClanWarRoom->GetNPC() );
				PlayTime( pClanWarRoom, pClanWarRoomTemplate );
				// ���� �����߿� ���� �� HP ���� �Ѵ�.
				NotifyBossHP( pClanWarRoom->GetZoneNo() );
				break;

			case CWPT_END_CLANWAR:

#ifdef __USE_ZLOG__
				// ���ȣ
				zTemp.Format( "[END] - TIME(%d sec) RoomNO(%d) ZoneNO(%d) BatJuly(%d)", pClanWarRoom->GetElapsedPlayTime()*10, i, pClanWarRoom->GetZoneNo(), pClanWarRoom->GetBatJuly() );
				zslERoomNo.Add( zTemp );
				// �� �� �ο� ����
				zTemp.Format( "[END] - A Team(WAR MEMBER:%d), B Team(WAR MEMBER:%d)", 
							  pClanWarRoom->GetATeamMemberCount( TRUE ), 
							  pClanWarRoom->GetBTeamMemberCount( TRUE ) );
				zslERoomFamilyMemberCount.Add( zTemp );
#endif

				// ���� ���� �����
				EndTime( pClanWarRoom );
				// ���� �����߿� ���� �� HP ���� �Ѵ�.
				NotifyBossHP( pClanWarRoom->GetZoneNo() );
				break;
			}

			// 10�ʿ� �ѹ��� Ŭ���� ���� ���� ������Ʈ
			// 10�ʿ� �ѹ� ��� Ŭ���� ������ ������Ʈ �ϹǷ� �� Ŭ������ ���� �ߺ��Ǿ� ������Ʈ �� ���� ����.
			//UpdateClanWarList( pClanWarRoom, i );
		}
	}

#ifdef __USE_ZLOG__

	// Ŭ���� �������� �� ����
	iCount = zslPRoomNo.Count();
	if( iCount > 0 )
		g_WatchData.Log( "<< Ŭ���� ������... >>" );
	for( int i = 0; i < iCount; i++ )
	{
		g_WatchData.Log( zslPRoomNo.IndexOf(i).c_str() );
		g_WatchData.Log( zslPRoomFamilyMemberCount.IndexOf(i).c_str() );
	}

	// Ŭ���� �������� �� ����
	iCount = zslERoomNo.Count();
	if( iCount > 0 )
		g_WatchData.Log( "<< Ŭ���� ������... >>" );
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
// ���� ���� Ÿ�� ����
//---------------------------------------------------------------------------

BOOL CClanWar::PlayTime( CClanWarRoom* pClanWarRoom, CClanWarRoomTemplate* pClanWarRoomTemplate )
{
	DWORD dwElapsedTime = 0;
	DWORD dwPlayTime = 0, dwGateTime = 0, dwEndTime = 0;


	// INI�� ������ Ŭ���� ���� �ð� ����
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


	// Ŭ���� ����
	dwElapsedTime = pClanWarRoom->GetElapsedPlayTime();
	if( dwElapsedTime > 0 )
	{
		// Ÿ�� ����
		dwElapsedTime--;
		// ī��Ʈ ���� �޽���
		SendProcessMessage( pClanWarRoom, CWMPT_TIME_MSG, CWMWT_NO_WARP );
		pClanWarRoom->SetElapsedPlayTime( dwElapsedTime );

		// ���� ���� �޽��� ���� ���ð� ó��
		if( dwElapsedTime == (dwPlayTime - dwGateTime) )
		{
			// (20070417:����ö) : ����Ʈ �������� ��ǽ� äũ
			// ����Ʈ ������ �ƹ��� ������ �ٷ� ����
			// (A,B�� ��� �����ϸ� ���� �ƹ��� ������.. �ʻ��� ���ڸ� äũ�ص� ��� ����� ��찡 ó����)
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



				// �� ����
				RemoveMOB( pClanWarRoom, CWTT_ALL );
				// PK ��� ����
				g_pZoneLIST->Set_PK_FLAG( (short)pClanWarRoom->GetZoneNo(), FALSE );
				// ��� ������ ü���� ȸ��
				ResetTeamHP( pClanWarRoom, CWTT_ALL );
				// ���� ���� ����
				pClanWarRoom->SetPlayType( CWPT_END_CLANWAR );
				pClanWarRoom->SetElapsedPlayTime( 0 );

				// Ŭ���̾�Ʈ Ŭ���� ���� ��ûâ �ݱ�
				pClanWarRoom->StartTimeOut();
			}
			else if( pClanWarRoom->GetBTeamGiveUp() || pClanWarRoom->GetBTeamMemberCount( TRUE ) <= 0 )
			{ // A�� �¸� (��ǽ�)
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



				// Ŭ���� �� ������� ����
				pClanWarRoom->SetWarResult(	CWWR_WAR_RESULT_AWIN );
				DropClanWar( pClanWarRoom, CWTT_B_TEAM );
				WinningRewardQuest( pClanWarRoom, CWTT_A_TEAM );

				// ���� ���� ����
				pClanWarRoom->SetPlayType( CWPT_END_CLANWAR );
				pClanWarRoom->SetElapsedPlayTime( dwEndTime );

				// Ŭ���̾�Ʈ Ŭ���� ���� ��ûâ �ݱ�
				pClanWarRoom->StartTimeOut();
			}
			else if( pClanWarRoom->GetATeamGiveUp() || pClanWarRoom->GetATeamMemberCount( TRUE ) <= 0 )
			{ // B�� �¸� (��ǽ�)
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



				// Ŭ���� �� ������� ����
				pClanWarRoom->SetWarResult(	CWWR_WAR_RESULT_BWIN );
				DropClanWar( pClanWarRoom, CWTT_A_TEAM );
				WinningRewardQuest( pClanWarRoom, CWTT_B_TEAM );

				// ���� ���� ����
				pClanWarRoom->SetPlayType( CWPT_END_CLANWAR );
				pClanWarRoom->SetElapsedPlayTime( dwEndTime );

				// Ŭ���̾�Ʈ Ŭ���� ���� ��ûâ �ݱ�
				pClanWarRoom->StartTimeOut();
			}
			else
			{
				// ���� ���� ó��
				Send_gsv_CLANWAR_CLIENT_PROGRESS( pClanWarRoom, CWCPM_CLANWAR_START );
			}

			// ����Ʈ ����
			pClanWarRoom->SetGateFlag( TRUE );
			Send_gsv_GateStatus( pClanWarRoom, NULL, FALSE );
		}
		else
			Send_gsv_GateStatus( pClanWarRoom, NULL, TRUE );
	}
	// Ŭ���� �ð� ��� ���� (���º�!)
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

		// ���� ���� ����
        pClanWarRoom->SetPlayType( CWPT_END_CLANWAR );
		pClanWarRoom->SetElapsedPlayTime( dwEndTime );
		// �������� ����� ����
		SendProcessMessage( pClanWarRoom, CWMPT_TIME_MSG, CWMWT_NO_WARP );

		// ���º� ó��
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
		// Ŭ���̾�Ʈ Ŭ���� ���� ��ûâ �ݱ�
		pClanWarRoom->StartTimeOut();

		// ���� �¸� �޽���
		Send_gsv_CLANWAR_CLIENT_PROGRESS( pClanWarRoom, CWCPM_CLANWAR_TEAM_DRAW );

		// �� ����
		RemoveMOB( pClanWarRoom, CWTT_ALL );

		// PK ��� ����
		g_pZoneLIST->Set_PK_FLAG( (short)pClanWarRoom->GetZoneNo(), FALSE );

		// ��� ������ ü���� ȸ��
		ResetTeamHP( pClanWarRoom, CWTT_ALL );
	}
	
	return TRUE;
}

//---------------------------------------------------------------------------
// ���� ���� Ÿ�� ����
//---------------------------------------------------------------------------

BOOL CClanWar::EndTime( CClanWarRoom* pClanWarRoom )
{
	CObjNPC* pNPC = NULL;
	DWORD dwElapsedTime = 0;

	dwElapsedTime = pClanWarRoom->GetElapsedPlayTime();

	// Ÿ�Ӱ� Ȯ��
	if( dwElapsedTime > 1 )
	{
		// Ÿ�� ����
		dwElapsedTime--;
		// ī��Ʈ ���� �޽��� ����
		SendProcessMessage( pClanWarRoom, CWMPT_TIME_MSG, CWMWT_NO_WARP );
		// �� ���°�
		Send_gsv_GateStatus( pClanWarRoom, NULL, TRUE );	
		// Ÿ�� ����
		pClanWarRoom->SetElapsedPlayTime( dwElapsedTime );
	}
	// ��� ������ �ֳ����� ����
	else if( dwElapsedTime == 1 )
	{
		// Ÿ�� ����
		dwElapsedTime--;
		// �� ����
		RemoveMOB( pClanWarRoom, CWTT_ALL );
		// ī��Ʈ ���� �޽��� ����
		SendProcessMessage( pClanWarRoom, CWMPT_TIME_MSG, CWMWT_NO_WARP );
		// �ش� Ŭ���� ���� ��� ������ �ֳ����� ����
		SendProcessMessage( pClanWarRoom, CWMPT_RETURN_WARP_MSG, CWMWT_NO_WARP );
		// Ÿ�� ����
		pClanWarRoom->SetElapsedPlayTime( dwElapsedTime );
	}
	// ���� ����
	else
	{
		// �ش� Ŭ���� ���� ��� ������ �ֳ����� ����
		//SendProcessMessage( pClanWarRoom, CWMPT_RETURN_WARP_MSG, CWMWT_NO_WARP );
		g_pZoneLIST->Set_PK_FLAG( (short)pClanWarRoom->GetZoneNo(), TRUE );
		// ���� ���� -> �ʵǳ�.. -_-;
		//RemoveMOB( pClanWarRoom, CWTT_ALL );
		// �� �ʱ�ȭ
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
// PK ��� äũ�� ó��
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
				// AI �ൿ ����
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
// Ŭ���� ���� ���ΰ�?
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
				// A �� ������
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

				// B �� ������
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
// ������ �׾��°�?
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

	// Ŭ���� �����ΰ�?
	dwMobIdx = pKilledMOB->Get_CharNO();
	//dwMobIdx = pKilledMOB->Get_INDEX();
	if( IsBoss( dwMobIdx ) == FALSE )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}


	if( pKilledMOB->GetZONE() )
	{
		// Ŭ������ ��������
		dwZoneNo = pKilledMOB->GetZONE()->Get_ZoneNO();
		if( (pClanWarRoom = GetClanWarRoomByZoneNo(dwZoneNo)) == NULL )
		{
			//LeaveCriticalSection( &m_General_LOCK );
			return FALSE;
		}

		// A �� ���� �� ����Ʈ ��������
		pAMobList = pClanWarRoom->GetATeamMOBList();
		if( pAMobList )
		{
			// ù��° ���� �����̴�.
			pRegenMOB = (CObjMOB*)pAMobList->First();

			// ���� ���� �������� ������?
			if( pRegenMOB == pKilledMOB )
			{
				// (20070423:����ö) : Ŭ���� �α� �߰�
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

				// Ŭ���� �� ������ ������ Ŭ���� ����� �����ϰ� ��� ���� ����Ѵ�.
				pClanWarRoom->SetWarResult(	CWWR_WAR_RESULT_BWIN );
				DropClanWar( pClanWarRoom, CWTT_ALL );
				WinningRewardQuest( pClanWarRoom, CWTT_B_TEAM );
				// Ŭ���̾�Ʈ Ŭ���� ���� ��ûâ �ݱ�
				pClanWarRoom->StartTimeOut();

				//LeaveCriticalSection( &m_General_LOCK );
				return TRUE;
			}
		}

        // B �� ���� �� ����Ʈ ��������
		pBMobList = pClanWarRoom->GetBTeamMOBList();
		if( pBMobList )
		{
			// ù��° ���� �����̴�.
			pRegenMOB = (CObjMOB*)pBMobList->First();

			// ���� ���� �������� ������?
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

				// Ŭ���� �� ������ ������ Ŭ���� ����� �����ϰ� ��� ���� ����Ѵ�.
				pClanWarRoom->SetWarResult(	CWWR_WAR_RESULT_AWIN );
				DropClanWar( pClanWarRoom, CWTT_ALL );
				WinningRewardQuest( pClanWarRoom, CWTT_A_TEAM );
				// Ŭ���̾�Ʈ Ŭ���� ���� ��ûâ �ݱ�
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
// Ŭ���� �������� ���� HP ���� ����
//---------------------------------------------------------------------------

BOOL CClanWar::NotifyBossHP( DWORD dwZoneNo )
{
	BOOL bCheckSendTiming = FALSE;
	DWORD dwCurTick = 0;
	CClanWarRoom* pClanWarRoom = NULL;


	//EnterCriticalSection( &m_General_LOCK );

	// �����ΰ�? (Ŭ���� �������� ���̸� äũ ������ �ּ� �ð����� 1500(1.5��)�̹Ƿ�)
	// Ŭ���� �������� ���ϰ�� pClanWarRoom ȹ���� �����Ѵ�.
	if( (pClanWarRoom = GetClanWarRoomByZoneNo( dwZoneNo )) == NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}

	// Ŭ���� �������ΰ�?
	dwCurTick = GetTickCount();
	if( pClanWarRoom->GetPlayType() == CWPT_WARP_TEAM_B )
	{
		if( pClanWarRoom->GetBossHPUpdateTime() < dwCurTick )
		{
			if( (dwCurTick - pClanWarRoom->GetBossHPUpdateTime()) > 1500 )
			{
				// HP ����! OK~
				bCheckSendTiming = TRUE;

				// ���� äũ Ÿ�̹� ����
				pClanWarRoom->SetBossHPUpdateTime( dwCurTick );
			}
		}
		else
		{
			// Ÿ�̸Ӱ� ȸ���� ���
			bCheckSendTiming = TRUE;
			
			// ���� äũ Ÿ�̹� ����
			pClanWarRoom->SetBossHPUpdateTime( dwCurTick );
		}
	}

	// ������ ����� ��쵵 ����
	if( pClanWarRoom->GetPlayType() == CWPT_END_CLANWAR )
	{
		bCheckSendTiming = TRUE;
	}


	if( bCheckSendTiming )
	{
		// Ŭ���� ���� �ִ� ��� �������� ���� HP���� ����
		char szSendBuffer[512];
		ZeroMemory( szSendBuffer, 512 );
		gsv_cli_CLAN_WAR_BOSS_HP* pSendData = NULL;

		pSendData = (gsv_cli_CLAN_WAR_BOSS_HP*)szSendBuffer;
		pSendData->m_wType = GSV_CLANWAR_BOSS_HP;
		pSendData->m_nSize = sizeof(gsv_cli_CLAN_WAR_BOSS_HP);


		// << �� ���� ���� >>

		// ������ ����� ���
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

			// A �� ���� �� ����Ʈ ��������
			if( (pAMobList = pClanWarRoom->GetATeamMOBList()) != NULL )
			{
				// ù��° ���� �����̴�.
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

			// B �� ���� �� ����Ʈ ��������
			if( (pBMobList = pClanWarRoom->GetBTeamMOBList()) != NULL )
			{
				// ù��° ���� �����̴�.
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
		// �ش� ���� �޽��� ����
		g_pZoneLIST->Send_gsv_All_User_Data( (short)dwZoneNo, (char*)pSendData );
	}

	//LeaveCriticalSection( &m_General_LOCK );

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Ŭ���� ������ ��(��)�ΰ�?
//---------------------------------------------------------------------------

BOOL CClanWar::IsCreatedClanWarRoom( DWORD dwZoneNo )
{
	//EnterCriticalSection( &m_General_LOCK );

	// ������ Ŭ�������ΰ�?
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

	// Ŭ���� ������
	if( pClanWarRoom->GetPlayType() == CWPT_TEAM_B_APPLY || 
		pClanWarRoom->GetPlayType() == CWPT_WARP_TEAM_A || 
		pClanWarRoom->GetPlayType() == CWPT_WARP_TEAM_B )
	{
		if( pClanWarRoom->IsATeamMember( pUser ) )
		{
			if( !pClanWarRoom->GetATeamGiveUp() )
			{
				// ����� ������ ���� ���� ���� ũ��
				if( (DWORD)pUser->Get_LEVEL() > dwRestrictLevel )
					// ���� ���� ���� �Ұ�
					dwReturnValue = CWTT_ALL;
				else
					dwReturnValue = CWTT_A_TEAM;
			}
		}
		else if( pClanWarRoom->IsBTeamMember( pUser ) )
		{
			if( !pClanWarRoom->GetBTeamGiveUp() )
			{
				// ����� ������ ���� ���� ���� ũ��
				if( (DWORD)pUser->Get_LEVEL() > dwRestrictLevel )
					// ���� ���� ���� �Ұ�
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

	//	���ú��� �ʱ�ȭ
	pUser->m_Quests.m_nClanWarVAR[0] = 0;	// Ŭ���� ��û Ÿ�� (1000�ٸ�... ���)
	pUser->m_Quests.m_nClanWarVAR[6] = 0;	// ä�� ��ȣ
	pUser->m_Quests.m_nClanWarVAR[5] = 0;	// ���� �ݾ�
	pUser->m_Quests.m_nClanWarVAR[7] = 0;

	////	������ ���� ����Ʈ ���� ���� ����
	pUser->Send_gsv_Quest_VAR();

	//LeaveCriticalSection( &m_General_LOCK );

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Ŭ�����̸� ����Ʈ ���� ����
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
// ��Ȱ ��ġ ���
//---------------------------------------------------------------------------

BOOL CClanWar::GetRevivePos( classUSER* pUser, tPOINTF ptDeadPos, tPOINTF& ptRevivePos /*OUT*/ )
{
	DWORD dwZoneNo = 0;
	CZoneTHREAD* pZoneThread = NULL;
	CClanWarRoom* pClanWarRoom = NULL;
	CClanWarRoomTemplate* pClanWarRoomTemplate = NULL;


	//EnterCriticalSection( &m_General_LOCK );

	// �� ���ϱ�
	if( (pZoneThread = pUser->GetZONE()) == NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}

	dwZoneNo = pZoneThread->Get_ZoneNO();
	// Ŭ������ ���ϱ�
	if( (pClanWarRoom = GetClanWarRoomByZoneNo( dwZoneNo )) == NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}

	// Ŭ���� ���ø� ���ϱ�
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
// Ŭ���� ������ �������� �Ǿ����� ù��° Ŭ�� �ʿ� �����ϴ� �⺻ ���� ��ġ���� ����
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
				// �ֱ� ���� ���Ḧ Ŭ�� �ʿ��� �ߴ�.
				// ù��° ���� �⺻ ��ġ���� ��Ȱ��Ų��.
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
// ������ ����� Ŭ���� �Ҽ����� Ȯ��
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
			// Ŭ���� ����
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
				// Ŭ���� ����� ���� ������ �� ����� Ŭ���� ���� ����
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
				// Ŭ���� ����� ���� ������ �� ����� Ŭ���� ���� ����
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
// Ŭ���� �������� �̵�
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

	// ����� ������ ���� ���� ���� ũ��
	if( (DWORD)pUser->Get_LEVEL() > dwRestrictLevel )
	{
		// ���� ��Ű�� �ʴ´�.
		pUser->Send_gsv_CLANWAR_Err( CWAR_CLAN_MASTER_LEVEL_RESTRICTED );
		//LeaveCriticalSection( &m_General_LOCK );
		return 1;
	}

	switch( dwTeamType )
	{
	case CWTT_A_TEAM:

		// ���� �ο� ����
		if( dwRestrictJoinCount <= pClanWarRoom->GetATeamMemberCount( TRUE ) )
		{
			pUser->Send_gsv_CLANWAR_Err( CWAR_TOO_MANY_CWAR_PROGRESS );
			//LeaveCriticalSection( &m_General_LOCK );
			return 2;
		}

		// ü���� 0�� ��� ����
		if( pUser->Get_HP() <= 0 )
		{
			pUser->Set_HP( pUser->Get_MaxHP() );
			pUser->Send_CHAR_HPMP_INFO( FALSE );
		}

		// ��Ƽ ����
		pUser->Party_Out();
		// Ŭ���� �̵�
		pUser->Reward_WARP( pClanWarRoom->GetZoneNo(), pClanWarRoom->GetATeamWarpPos() );
		// �ش� ���� �� ��ȣ ����
		pUser->SetCur_TeamNO( CWTT_A_TEAM );
		pUser->Set_TeamNoFromClanIDX( CWTT_A_TEAM );

		break;

	case CWTT_B_TEAM:

		// ���� �ο� ����
		if( dwRestrictJoinCount <= pClanWarRoom->GetBTeamMemberCount( TRUE ) )
		{
			pUser->Send_gsv_CLANWAR_Err( CWAR_TOO_MANY_CWAR_PROGRESS );
			//LeaveCriticalSection( &m_General_LOCK );
			return 2;
		}

		// ü���� 0�� ��� ����
		if( pUser->Get_HP() <= 0 )
		{
			pUser->Set_HP( pUser->Get_MaxHP() );
			pUser->Send_CHAR_HPMP_INFO( FALSE );
		}

		// ��Ƽ ����
		pUser->Party_Out();
		// Ŭ���� �̵�
		pUser->Reward_WARP( pClanWarRoom->GetZoneNo(), pClanWarRoom->GetBTeamWarpPos() );
		// �ش� ���� �� ��ȣ ����
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
// Ŭ���� ����
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
	g_WatchData.Log( "ClanID:%d, ���� ����ȣ : %d", pUser->GetClanID(), pClanWarRoom->GetZoneNo() );
#endif

	// << A, B �� ��� ���������� Ŭ������ ���� >>
	if( pClanWarRoom->GetATeamGiveUp() && pClanWarRoom->GetBTeamGiveUp() )
	{
#ifdef __USE_ZLOG__
		g_WatchData.Log( "[�������]ClanID:%d, ���� ����ȣ : %d", pUser->GetClanID(), pClanWarRoom->GetZoneNo() );
#endif

		// ���� ���� ����
		pClanWarRoom->SetPlayType( CWPT_END_CLANWAR );
		pClanWarRoom->SetElapsedPlayTime( 0 );
		// �� ����
		RemoveMOB( pClanWarRoom, CWTT_ALL );
		// PK ��� ����
		g_pZoneLIST->Set_PK_FLAG( (short)pClanWarRoom->GetZoneNo(), FALSE );
		// ��� ������ ü���� ȸ��
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

	// Ŭ�� �������̸� Ŭ���� ���� ����
	if( pUser->Is_ClanMASTER() )
	{
		if( pClanWarRoom->IsATeamMember( pUser ) )
		{
			// A ��
			if( DropClanWar( pClanWarRoom, CWTT_A_TEAM ) )
			{
				// ����Ʈ ���� ó��
				WinningRewardQuest( pClanWarRoom, CWTT_A_TEAM );

				// B �� ������ or �ְ� ����ڿ��� �޽��� ������
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
			// B ��
			if( DropClanWar( pClanWarRoom, CWTT_B_TEAM ) )
			{
				// ����Ʈ ���� ó��
				WinningRewardQuest( pClanWarRoom, CWTT_B_TEAM );

				// A �� ������ or �ְ� ����ڿ��� �޽��� ������
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
// Ŭ���� ���࿡ �ʿ��� �޽��� ����
// dwProcessType [01:�����Ϳ��� Ŭ���� ���� �޽��� ����]
//               [02:Ŭ�����鿡�� Ŭ���� ���� �޽��� ����]
//               [07:������ �����鿡�� Ŭ���� ���� ��ġ ���� ����]
//               [10:������ ������ ������. (�⺻:�ֳ����� ����)]
// dwWarpType [1:A �� Ŭ�����鿡�� Ŭ���� ���� �޽��� ����]
//            [2:B �� Ŭ�����鿡�� Ŭ���� ���� �޽��� ����]
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
// �¸� ���� (�ٸ�, ������) - Ŭ���� �� "�¸��� ¡ǥ" ����Ʈ�� ���� ����
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

	//	���� �ݾ� ����..
	if(pRewardUser->m_Quests.m_nClanWarVAR[5] > 0)
	{
		pRewardUser->m_Inventory.m_i64Money = pRewardUser->m_Inventory.m_i64Money + pRewardUser->m_Quests.m_nClanWarVAR[5]*1000;
		pRewardUser->Quest_SetClanWarVAR( 5, 0 );	// Ŭ���� ���� �ݾ� (1/1000 ����)
		pRewardUser->Quest_SetClanWarVAR( 6, 0 );	// ä�� ��ȣ
		pRewardUser->Send_gsv_SET_MONEYONLY(GSV_SET_MONEY_ONLY);
	}

	//	���� ������ ����
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
							g_WatchData.Log( "���� ������ ��ȣ:%d, ����:%d", pItem->dwItemNo, pItem->dwItemDup );
							pRewardUser->In_Out_Item( TRUE, pItem->dwItemNo, (WORD)pItem->dwItemDup );
						}						
					}
				}
			}
		}

		pRewardUser->Quest_SetClanWarVAR( 7, 0 );	// Ŭ���� Ÿ��
		pRewardUser->Quest_SetClanWarVAR( 8, 0 );	// Ŭ���� ��� NPC ��ȣ
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

	// DB INI���� ����
	zSetupINI.GetValue( "SETUP", "CLAN_WAR_DB", zDBFileName );

	// (20070509:����ö) : ������ �б�
	m_dwUSE = 0;
	m_dwElapsedInvalidateTime = 0;
	m_dwInvalidateMAXCount = 0;
	zSetupINI.GetValue( "CHECK_HACKER", "USE", m_dwUSE );
	zSetupINI.GetValue( "CHECK_HACKER", "ELAPSED_TIME", m_dwElapsedInvalidateTime );
	zSetupINI.GetValue( "CHECK_HACKER", "COUNT", m_dwInvalidateMAXCount );

	// (20070511:����ö) : �� ������ �ð��� ��Ŷ Ƚ��
	m_dwElapsedRecvPacketMAXTime = 0;
	zSetupINI.GetValue( "CHECK_HACKER", "ELAPSED_RECV_PACKET_CHECK_TIME", m_dwElapsedRecvPacketMAXTime );

	// (20070514:����ö) : ��Ŷ �ʰ� ��� ����
	m_dwWarningPacketCount = 0;
	zSetupINI.GetValue( "CHECK_HACKER", "WARNING_COUNT", m_dwWarningPacketCount );

	// (20070514:����ö) : ��Ŷ �ʰ� ������� ��Ŀ��
	m_dwDisconnectPacketCount = 0;
	zSetupINI.GetValue( "CHECK_HACKER", "CUT_OFF_COUNT", m_dwDisconnectPacketCount );



	// Ŭ�� �� INI ���� ���� �� �� ����Ÿ ���ø� ����
	int iCount = 0;
	CClanWarRoomTemplate* pClanWarRoomTemplate = NULL;
	do
	{
		zValueName = "MAP";
		zValueName += ZString(iCount);
		zSetupINI.GetValue( "SETUP", zValueName, zMAPFileName );

		if( zMAPFileName.IsEmpty() == FALSE )
		{
			// Ŭ�� �� ����Ÿ ���ø� ����
			zMapINI.Open( zMAPFileName );

			if( (pClanWarRoomTemplate = new CClanWarRoomTemplate()) == NULL )
				return FALSE;


			// ���ø��� Ŭ�� NPC ����
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


			// ���ø� �� ���� �ѹ�
			// (STB�� ������ �� ���� ��ȣ)
			DWORD dwZoneSeedNo = 0;
			zMapINI.GetValue( "CLANWAR_ZONE_SEED_NO", "SEED_NO", dwZoneSeedNo );
			// ����
			pClanWarRoomTemplate->SetZoneSeedNo( dwZoneSeedNo );


			// ���� ���� ��ġ
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
			// ����
			pClanWarRoomTemplate->SetStartPos( fATeamPosX, fATeamPosY, fBTeamPosX, fBTeamPosY );


			// ��Ȱ ��ġ
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
				// ����
				pClanWarRoomTemplate->AddATeamRevivalPos( fATeamPosX, fATeamPosY );

				zTempRevival = "BTEAM_POS_X";
				zTempRevival += ZString(iTempRevival);
				zMapINI.GetValue( "CLANWAR_ZONE_REVIVAL_POSITION", zTempRevival, zValue );
				if( zValue.IsEmpty() ) break;
				fBTeamPosX = zValue.ToFloat();

				zTempRevival = "BTEAM_POS_Y";
				zTempRevival += ZString(iTempRevival);
				zMapINI.GetValue( "CLANWAR_ZONE_REVIVAL_POSITION", zTempRevival, fBTeamPosY );
				// ����
				pClanWarRoomTemplate->AddBTeamRevivalPos( fBTeamPosX, fBTeamPosY );

				iTempRevival++;

			} while(1);


			// �⺻ ��ȯ ��ġ
			DWORD dwZoneNo = 0;
			FLOAT fPosX=0.f, fPosY=0.f;

			zMapINI.GetValue( "DEFAULT_CLANWAR_ZONE_RETURN_POSITION", "ZONE_NO", dwZoneNo );
			pClanWarRoomTemplate->SetDefaultReturnZoneNo( dwZoneNo );
			zMapINI.GetValue( "DEFAULT_CLANWAR_ZONE_RETURN_POSITION", "POS_X", fPosX );
			zMapINI.GetValue( "DEFAULT_CLANWAR_ZONE_RETURN_POSITION", "POS_Y", fPosY );
			pClanWarRoomTemplate->SetDefaultReturnPos( fPosX, fPosY );


			// Ŭ���� ���� �ð�
			DWORD dwClanWarTime = 0;

			zMapINI.GetValue( "CLANWAR_TIME", "GATE_TIME", dwClanWarTime );
			pClanWarRoomTemplate->SetGateTime( dwClanWarTime );
			zMapINI.GetValue( "CLANWAR_TIME", "PLAY_TIME", dwClanWarTime );
			pClanWarRoomTemplate->SetPlayTime( dwClanWarTime );
			zMapINI.GetValue( "CLANWAR_TIME", "END_TIME", dwClanWarTime );
			pClanWarRoomTemplate->SetEndTime( dwClanWarTime );


			// Ŭ���� ���� ����
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


			// Ŭ���� ���� �ݾ�
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


			// Ŭ���� ���� �ִ� �ο� �� (�⺻ : 15 (���� �ִ밪 : 50))
			DWORD dwMax_ClanWarJoiningNumberPerTeam = 0;

			zMapINI.GetValue( "MAX_CLANWAR_JOINING_NUMBER_PER_TEAM", "MAX_CLANWAR_JOINING_NUMBER_PER_TEAM", dwMax_ClanWarJoiningNumberPerTeam );
			pClanWarRoomTemplate->SetMAX_ClanWarJoiningNumberPerTeam( dwMax_ClanWarJoiningNumberPerTeam );


			// Ŭ���� �ִ� �� (Ŭ���� �ο� ���� 50�� ��� 25, 15�� ��� 50) : �ִ� ������ 2500�� ���� ����
			DWORD dwMAX_ClanWarNumber = 0;

			zMapINI.GetValue( "MAX_CLANWAR_NUMBER", "MAX_CLANWAR_NUMBER", dwMAX_ClanWarNumber );
			pClanWarRoomTemplate->SetMAX_ClanWarNumber( dwMAX_ClanWarNumber );


			// Ŭ���� ���� ���� ����
			int iClanWarTypeCount = 0;
			ZString zClanWarTypeName;
			ZString zMobCheck;		// ���� ������ �ִ��� äũ

			do 
			{
				// A �� ���� �б�
				zClanWarTypeName = "WAR_TYPE";
				zClanWarTypeName += ZString(iClanWarTypeCount);
				zClanWarTypeName += "_ATEAM_MOB_SETUP";

				// �� ������ �� �о��°�?
				zMapINI.GetValue( zClanWarTypeName, "MOB_0", zMobCheck );
				if( zMobCheck.IsEmpty() ) break;

				// A �� WarType�� �´� ���� �ε�
				InitWarTypeMOB( zMapINI, 0, zClanWarTypeName, pClanWarRoomTemplate );


				// B �� ���� �б�
				zClanWarTypeName = "WAR_TYPE";
				zClanWarTypeName += ZString(iClanWarTypeCount);
				zClanWarTypeName += "_BTEAM_MOB_SETUP";

				// �� ������ �� �о��°�?
				zMapINI.GetValue( zClanWarTypeName, "MOB_0", zMobCheck );
				if( zMobCheck.IsEmpty() ) break;

				// B �� WarType�� �´� ���� �ε�
				InitWarTypeMOB( zMapINI, 1, zClanWarTypeName, pClanWarRoomTemplate );

				// ī��Ʈ ����
                iClanWarTypeCount++;

			} while(1);


			// Ŭ���� �¸� ���� ������ �б�
			iClanWarTypeCount = 0;
			zClanWarTypeName = "";
			ZString zRewardItemCheck;	// ������ ������ �ִ��� äũ

			do 
			{
				// INI Ÿ��Ʋ ����
				zClanWarTypeName = "CW_REWARD_ITEM_";
				zClanWarTypeName += ZString( iClanWarTypeCount );

				// ������ ������ �� �о��°�?
				zMapINI.GetValue( zClanWarTypeName, "CW_REWARD_ITEM_0", zRewardItemCheck );
				if( zRewardItemCheck.IsEmpty() ) break;

				// Ŭ���� Ÿ�Ժ� ������ �б�
				InitWarTypeRewardItem( zMapINI, zClanWarTypeName, pClanWarRoomTemplate );

				// ī��Ʈ ����
				iClanWarTypeCount++;
				
			} while(1);


			// Ŭ���� �� ����Ÿ ���ø� ���
			m_zlWarRoomTemplate.Add( (void*)pClanWarRoomTemplate );
		}
		else break;

		// �� ����Ÿ ���� ��ȣ
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
	ZString zMobCheck;		// ���� ������ �ִ��� äũ
	stClanWarMOB* pClanWarMobInfo = NULL;
	ZList* pzlMobData = NULL;



	// ù ����Ÿ äũ�� �ϰ� ���Ա� ������ �ѹ��� �� ���δ�. (��, DELETE ����)
	if( (pzlMobData = new ZList()) == NULL )
		return FALSE;

	do 
	{
		// �� ��ȣ
		zClanWarMobName = "MOB_";
		zClanWarMobName += ZString( iClanWarMobCount );
		zMapINI.GetValue( zClanWarTypeName, zClanWarMobName, zMobCheck );
		if( zMobCheck.IsEmpty() ) break;
		if( (pClanWarMobInfo = new stClanWarMOB()) == NULL )
			return FALSE;
		pClanWarMobInfo->dwMOB_IDX = (DWORD)zMobCheck.ToInt();

		// �� ��ġ
		zClanWarMobName = "POS_X";
		zClanWarMobName += ZString( iClanWarMobCount );
		zMapINI.GetValue( zClanWarTypeName, zClanWarMobName, pClanWarMobInfo->fPosX );

		zClanWarMobName = "POS_Y";
		zClanWarMobName += ZString( iClanWarMobCount );
		zMapINI.GetValue( zClanWarTypeName, zClanWarMobName, pClanWarMobInfo->fPosY );

		// �� ���� ��
		zClanWarMobName = "REGEN_NUMBER";
		zClanWarMobName += ZString( iClanWarMobCount );
		zMapINI.GetValue( zClanWarTypeName, zClanWarMobName, pClanWarMobInfo->dwRegenNumber );

		// �� ���� �ݰ�
		zClanWarMobName = "REGEN_RANGE";
		zClanWarMobName += ZString( iClanWarMobCount );
		zMapINI.GetValue( zClanWarTypeName, zClanWarMobName, pClanWarMobInfo->dwRegenRange );

		// �� ���� ���� ���
		if( (pzlMobData->Add( pClanWarMobInfo )) == FALSE )
			return FALSE;

		iClanWarMobCount++;

	} while(1);

	// �� ���� ���
	if( iTeamType == 0 ) // A ��
		pClanWarRoomTemplate->AddATeamMOBInfo( pzlMobData );
	else // B ��
		pClanWarRoomTemplate->AddBTeamMOBInfo( pzlMobData );

	return TRUE;
}

//---------------------------------------------------------------------------
// Ŭ���� Ÿ�Ժ� �¸� ���� ������
//---------------------------------------------------------------------------

BOOL CClanWar::InitWarTypeRewardItem( ZIniFile& zMapINI, ZString zClanWarTypeName, CClanWarRoomTemplate* pClanWarRoomTemplate )
{
	int iItemCount = 0;
	ZString zItemData;
	ZString zItemCheck;
	stClanWarRewardItem* pClanWarRewardItem = NULL;
	ZList* pzlItemData = NULL;



	// ù ����Ÿ äũ�� �ϰ� ���Ա� ������ �ѹ��� �� ���δ�. (��, DELETE ����)
	if( (pzlItemData = new ZList()) == NULL )
		return FALSE;

	do 
	{
		// ������ ��ȣ
		zItemData = "CW_REWARD_ITEM_";
		zItemData += ZString( iItemCount );
		zMapINI.GetValue( zClanWarTypeName, zItemData, zItemCheck );
		if( zItemCheck.IsEmpty() ) break;
		if( (pClanWarRewardItem = new stClanWarRewardItem()) == NULL )
			return FALSE;
		pClanWarRewardItem->dwItemNo = (DWORD)zItemCheck.ToInt();

		// ������ �ߺ� ����
		zItemData = "CW_REWARD_DUP_";
		zItemData += ZString( iItemCount );
		zMapINI.GetValue( zClanWarTypeName, zItemData, pClanWarRewardItem->dwItemDup );

		// ���� ������ ���
		if( (pzlItemData->Add( pClanWarRewardItem )) == FALSE )
			return FALSE;

		iItemCount++;

	} while(1);

	// ������ ���� ���
	pClanWarRoomTemplate->AddWinningRewardItem( pzlItemData );

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Ŭ�� ��� ���
//---------------------------------------------------------------------------

BOOL CClanWar::AddClanMember( classUSER* pMember )
{
	ZThreadSync Sync;

	int iFamilyCount = 0;
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;

	// ����ȭ
	//EnterCriticalSection( &m_General_LOCK );

	// ó���̸� CClanFamily ����
	iFamilyCount = m_zlClanFamily.Count();
	if( iFamilyCount < 1 )
	{
		// << ���� Ŭ���� �߰� >>
		
		// Ŭ��&��� ����
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

		// ������� ����
		pClanMember->DataClear();				// �ʱ�ȭ
		pClanMember->SetUseFlag( TRUE );		// ������� ����Ÿ�ΰ�?
		pClanMember->SetUseWarFlag( FALSE );	// Ŭ������ �������ΰ�?
		pClanMember->SetMember( pMember );		// Ŭ�� ��� ����

		// �йи��� �߰�
		pClanFamily->DataClear();
		//pMember->m_CLAN.m_iRankPoint
		DWORD dwClanID = pMember->GetClanID();
		char* pClanName = pMember->GetClanNAME();
		pClanFamily->AddMember( pClanMember );

		// �йи� ����Ʈ�� �߰�
		m_zlClanFamily.Add( pClanFamily );

#ifdef __USE_ZLOG__
		g_WatchData.Log( "[ m_zlClanFamily�� �ű� Ŭ�� �߰� ] - ����" );
		if( pMember->Is_ClanMASTER() )
			g_WatchData.Log( " ** ���� Ŭ���� �߰�(������) : %s", pMember->Get_NAME() );
		else
			g_WatchData.Log( " ** ���� Ŭ���� �߰� : %s", pMember->Get_NAME() );
#endif

	}
	else
	{
		// << Ŭ���� �߰� >>
		
		// �̹� �ε��� Ŭ���йи��� �Ͽ��ΰ�?
		if( (pClanFamily = CheckTheSameFamily( pMember )) != NULL )
		{
			// Ŭ�� ���&�йи� ��� �ε��Ǿ� ����
			if( (pClanMember = pClanFamily->CheckVacantMemberMemory()) != NULL )
			{
				// ������� ����
				pClanMember->DataClear();				// �ʱ�ȭ
				pClanMember->SetUseFlag( TRUE );		// ������� ����Ÿ�ΰ�?
				pClanMember->SetUseWarFlag( FALSE );	// Ŭ������ �������ΰ�?
				pClanMember->SetMember( pMember );		// Ŭ�� ��� ����

				// �йи� ����Ÿ ������Ʈ
				pClanFamily->SetUseFlag( TRUE );
				pClanFamily->UpdateMember( pMember );

#ifdef __USE_ZLOG__
				if( pMember->Is_ClanMASTER() )
					g_WatchData.Log( " ** Ŭ���� �߰�[�йи�, ��� ����](������) : %s", pMember->Get_NAME() );
				else
					g_WatchData.Log( " ** Ŭ���� �߰�[�йи�, ��� ����] : %s", pMember->Get_NAME() );
#endif

			}
			// ��� �߰�
			else
			{
				// Ŭ�� ��� �ε��� �����Ƿ�
				if( (pClanMember = new CClanMember()) == NULL )
				{
					//LeaveCriticalSection( &m_General_LOCK );
					return FALSE;
				}

				// ������� ����
				pClanMember->DataClear();				// �ʱ�ȭ
				pClanMember->SetUseFlag( TRUE );		// ������� ����Ÿ�ΰ�?
				pClanMember->SetUseWarFlag( FALSE );	// Ŭ������ �������ΰ�?
				pClanMember->SetMember( pMember );		// Ŭ�� ��� ����

				// �йи��� �߰�
				pClanFamily->AddMember( pClanMember );

#ifdef __USE_ZLOG__
				if( pMember->Is_ClanMASTER() )
					g_WatchData.Log( " ** Ŭ���� �߰�[�йи� ����, ����߰�](������) : %s", pMember->Get_NAME() );
				else
					g_WatchData.Log( " ** Ŭ���� �߰�[�йи� ����, ����߰�] : %s", pMember->Get_NAME() );
#endif

			}

			// << �йи� ����Ʈ�� �߰��� �ʿ� ���� >>

		}
		else
		{
			// (20070427:����ö) 0427 ���� �ּ� : ������� �ʴ� �йи� ����Ÿ�� �ִ°�?
			// (20070427:����ö) 0427 ���� �ּ� : ������� �ʴ� �йи��� �Ͽ��̾��°�?
			if( (pClanFamily = CheckVacantFamilyMemory( pMember )) != NULL )
			{
				// Ŭ�� ���&�йи� ��� �ε��Ǿ� ����
				if( (pClanMember = pClanFamily->CheckVacantMemberMemory()) != NULL )
				{
					// ������� ����
					pClanMember->DataClear();				// �ʱ�ȭ
					pClanMember->SetUseFlag( TRUE );		// ������� ����Ÿ�ΰ�?
					pClanMember->SetUseWarFlag( FALSE );	// Ŭ������ �������ΰ�?
					pClanMember->SetMember( pMember );		// Ŭ�� ��� ����

					// �йи� ����Ÿ ������Ʈ
					pClanFamily->SetUseFlag( TRUE );
					pClanFamily->UpdateMember( pMember );

#ifdef __USE_ZLOG__
					if( pMember->Is_ClanMASTER() )
						g_WatchData.Log( " ** Ŭ���� �߰�[�йи�, ��� ����](������) : %s", pMember->Get_NAME() );
					else
						g_WatchData.Log( " ** Ŭ���� �߰�[�йи�, ��� ����] : %s", pMember->Get_NAME() );
#endif

				}
				// ��� �߰�
				else
				{
					if( (pClanMember = new CClanMember()) == NULL )
					{
						//LeaveCriticalSection( &m_General_LOCK );
						return FALSE;
					}

					// ������� ����
					pClanMember->DataClear();				// �ʱ�ȭ
					pClanMember->SetUseFlag( TRUE );		// ������� ����Ÿ�ΰ�?
					pClanMember->SetUseWarFlag( FALSE );	// Ŭ������ �������ΰ�?
					pClanMember->SetMember( pMember );		// Ŭ�� ��� ����

					// �йи��� �߰�
					pClanFamily->AddMember( pClanMember );

#ifdef __USE_ZLOG__
					if( pMember->Is_ClanMASTER() )
						g_WatchData.Log( " ** Ŭ���� �߰�[�йи� ����, ����߰�](������) : %s", pMember->Get_NAME() );
					else
						g_WatchData.Log( " ** Ŭ���� �߰�[�йи� ����, ����߰�] : %s", pMember->Get_NAME() );
#endif

				}

				// << �йи� ����Ʈ�� �߰��� �ʿ� ���� >>

			}
			else
			{
				// ó�� �����ϴ� Ŭ���̹Ƿ� ���� ����
				// Ŭ�� �йи��� �ε����� �ʾ����Ƿ� CClanFamily�� CClanMember ��� ����
				// (���� Ŭ���� �߰��� ����)

				// Ŭ��&��� ����
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

				// ������� ����
				pClanMember->DataClear();				// �ʱ�ȭ
				pClanMember->SetUseFlag( TRUE );		// ������� ����Ÿ�ΰ�?
				pClanMember->SetUseWarFlag( FALSE );	// Ŭ������ �������ΰ�?
				pClanMember->SetMember( pMember );		// Ŭ�� ��� ����

				// �йи��� �߰�
				pClanFamily->DataClear();
				pClanFamily->AddMember( pClanMember );

				// �йи� ����Ʈ�� �߰�
				m_zlClanFamily.Add( pClanFamily );

#ifdef __USE_ZLOG__
				g_WatchData.Log( "[ m_zlClanFamily�� �ű� Ŭ�� �߰� ]" );
				if( pMember->Is_ClanMASTER() )
					g_WatchData.Log( " ** Ŭ���� �߰�[�йи�, ��� ����](������) : %s", pMember->Get_NAME() );
				else
					g_WatchData.Log( " ** Ŭ���� �߰�[�йи�, ��� ����] : %s", pMember->Get_NAME() );
#endif

			}
		}
	}

	// Ŭ�� ���� ���� ������Ʈ
	//UpdateClanInfoList();

#ifdef __USE_ZLOG__
	int iClanCount = 0;
	int iLoginMemberCount = 0;
	ZList* pMemberList = NULL;


	g_WatchData.Log( " " );
	g_WatchData.Log( "<< �α��� Ŭ���� >>" );
	iFamilyCount = m_zlClanFamily.Count();
	g_WatchData.Log( " - Ŭ�� ��:%d", iFamilyCount );
	for( int i = 0; i < iFamilyCount; i++ )
	{
		if( (pClanFamily = (CClanFamily*)m_zlClanFamily.IndexOf(i)) != NULL )
		{
			if( pClanFamily->IsUse() )
				g_WatchData.Log( " -*- Ŭ����:%s - USE:TRUE", (pClanFamily->GetName()).c_str() );
			else
				g_WatchData.Log( " -*- Ŭ����:%s - USE:false", (pClanFamily->GetName()).c_str() );


			if( (pMemberList = pClanFamily->GetMemberList()) != NULL )
			{
				iLoginMemberCount = pMemberList->Count();
				for( int j = 0; j < iLoginMemberCount; j++ )
				{
					if( (pClanMember = (CClanMember*)pMemberList->IndexOf(j)) != NULL )
					{
						if( pClanMember->IsUse() )
							g_WatchData.Log( " --- %02d:���(%d):%s - USE:TRUE", j+1, pClanMember->GetGrade(), pClanMember->GetName() );
						else
							g_WatchData.Log( " --- %02d:���(%d):%s - USE:false", j+1, pClanMember->GetGrade(), pClanMember->GetName() );

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
// Ŭ�� ��� ���� ������Ʈ
//---------------------------------------------------------------------------

BOOL CClanWar::UpdateClanMember( classUSER* pMember )
{
	//ZThreadSync Sync;

	CClanFamily* pClanFamily = NULL;

	// ����ȭ
	//EnterCriticalSection( &m_General_LOCK );
	//if( (pClanFamily = CheckTheSameFamily( pMember )) != NULL )
	//{
	//	pClanFamily->UpdateMember( pMember );
	//}
	//LeaveCriticalSection( &m_General_LOCK );


	return TRUE;
}

//---------------------------------------------------------------------------
// Ŭ�� �������ΰ�?
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
// Ŭ���� ��û
//---------------------------------------------------------------------------

BOOL CClanWar::ApplyClanWar( CObjNPC* pNPC, classUSER* pApplyUser, DWORD dwClientClanWarType )
{
	ZThreadSync Sync;

	if( pNPC == NULL ) return FALSE;
	if( pApplyUser == NULL ) return FALSE;
	if( dwClientClanWarType == 0 ) return FALSE; // Ŭ���̾�Ʈ������ 1,2, 31,32, 61,62�� �����ش�.

	DWORD dwWarType = 0;
	DWORD dwUserLevel = 0;
	CClanWarRoomTemplate* pWarRoomTemplate = NULL;

	// ����ȭ
	//EnterCriticalSection( &m_General_LOCK );

#ifdef __USE_ZLOG__
	g_WatchData.Log( __FUNCTION__ );
#endif

	// �������ΰ�?
	if( pApplyUser->Is_ClanMASTER() == FALSE )
		return FALSE;

	// (20070503:����ö) : �̹� Ŭ���� ���� ���� ����� ��û�ϸ� �����Ѵ�.
	if( IsApplyClanWar( pApplyUser ) )
		return FALSE;

	// ���� Ŭ���� �ʿ� �´� ����Ÿ �ε�
	if( (pWarRoomTemplate = GetWarRoomTemplate( pNPC )) == NULL )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}

	// ����� 0,1,2 ��, 3���� Ÿ�Ը� �����
	dwWarType = ChangeClientToServerClanWarType( dwClientClanWarType );
	dwUserLevel = pApplyUser->Get_LEVEL();
	if( dwUserLevel < 0 )
	{
		//LeaveCriticalSection( &m_General_LOCK );
		return FALSE;
	}

	// Ŭ���� ���� ���� ���� äũ
	if( CheckUserLevel( pNPC, dwWarType, dwUserLevel ) == FALSE )
	{
		//	���� �޽��� ����
		pApplyUser->Send_gsv_CLANWAR_Err( CWAR_CLAN_MASTER_LEVEL_RESTRICTED );
		//LeaveCriticalSection(&m_General_LOCK);
		return FALSE;
	}

	// �ٸ� ���� äũ
	if( pApplyUser->Get_MONEY() < pWarRoomTemplate->GetWarTypeBatJuly( dwWarType ) )
	{
		//	���� �޽��� ����
		//LeaveCriticalSection(&m_General_LOCK);
		return FALSE;
	}

	// Ŭ���� ���� ���� äũ
	// (��û�ϴ� ���ǿ� �����ϴ� Ŭ���� ������ ������ ��������ش�.)
	CClanWarRoom* pWarRoom = NULL;
	if( (pWarRoom = CheckWarRoomReservation( pNPC, dwWarType )) != NULL )
	{
		// << �̹� ��û�Ǿ��ִ� Ŭ���� ��� ���� >>
		
		if( pWarRoom->GetPlayType() == CWPT_TEAM_A_APPLY )
		{
			INT64 i64BatJuly = 0;
			INT64 i64Temp = 0;
			FLOAT fPosX = 0.f, fPosY = 0.f;


			// << Ŭ���� ���� ���� >>
			// 2 : Ŭ���� ��û (�ι�° Ŭ��)
			pWarRoom->SetPlayType( CWPT_TEAM_B_APPLY );

			// �÷��� �ð� ��� (Ŭ���� ���� ���¿� ���� �ٸ���.)
			pWarRoom->SetElapsedPlayTime( pWarRoomTemplate->GetPlayTime() );

			// ���� �ٸ�
			i64BatJuly = pWarRoom->GetBatJuly();
			i64BatJuly += pWarRoomTemplate->GetWarTypeBatJuly( dwWarType );
			pWarRoom->SetBatJuly( i64BatJuly );
			// ���� �ٸ� ����
			i64Temp = pApplyUser->GetCur_MONEY();
			i64Temp = i64Temp - pWarRoomTemplate->GetWarTypeBatJuly( dwWarType );
			if( i64Temp >= 0 )
			{
				pApplyUser->SetCur_MONEY( i64Temp );
				pApplyUser->Send_gsv_SET_MONEYONLY( GSV_SET_MONEY_ONLY );
			}

			// Ŭ���� B �� �� ���� ��ǥ
			fPosX = pWarRoomTemplate->GetBTeamStartPos().x;
			fPosY = pWarRoomTemplate->GetBTeamStartPos().y;
			pWarRoom->SetBTeamWarpPos( fPosX, fPosY );

			// B �� Ŭ�� ����
			SetBTeam( pWarRoom, pApplyUser->GetClanNAME() );

			// B �� Ŭ���� Ÿ�� ����
			pWarRoom->SetBTeamWarType( dwWarType );

			// << Ŭ���� B �� ��û DB ���� >>
			GF_DB_CWar_Insert( 2, 0, i64BatJuly, 0, pApplyUser->GetClanID(), NULL );

			// Ŭ���忡�� ���� �޽��� ������.
			SendProcessMessage( pWarRoom, CWMPT_MASTER_MSG, CWMWT_NO_WARP );

			// << Ŭ���� �������� ���� DB ���� >>
			// CheckWarRoomReservation ���⼭ A, B ���� ������ ���� �Ǿ ����
			GF_DB_CWar_Insert( 3, 0, pWarRoom->GetBatJuly(), pWarRoom->GetATeam()->GetIDX(), pWarRoom->GetBTeam()->GetIDX(), NULL );

			// (20070423:����ö) : Ŭ���� �α� �߰�
			g_LOG.CS_ODS( 0xffff, "\n[B TEAM] ZONE(%d) WAR_TYPE(%d) CLANID(%d):%s MASTERID(%s):%s",
						  pWarRoom->GetZoneNo(), pWarRoom->GetWarType(), pApplyUser->GetClanID(), pApplyUser->GetClanNAME(),
						  pApplyUser->Get_ACCOUNT(), pApplyUser->Get_NAME() );



#ifdef __USE_ZLOG__
			g_WatchData.Log( "[Ŭ���� B �� ��û]" );
#endif
		}
		else
		{
#ifdef __USE_ZLOG__
			g_WatchData.Log( "[ERROR] B �� ��û�� ������ CWPT_TEAM_A_APPLY ���� �ƴ�(ZoneNO:%d)(PlayType:%d)", pWarRoom->GetZoneNo(), pWarRoom->GetPlayType() );
#endif
			//LeaveCriticalSection(&m_General_LOCK);
			return FALSE;
		}
	}
	else
	{
		// << Ŭ���� ��û >>
		DWORD dwWarRoomIDX = 0;

		// �� �� �������� (������ ����ϴ��� ������!)
		if( (pWarRoom = CheckVacantWarRoom( pNPC, dwWarRoomIDX )) != NULL )
		{
			// �� �濡 ��û ����

			INT64 i64BatJuly = 0;
			INT64 i64Temp = 0;
			DWORD dwZoneNo = 0;
			FLOAT fPosX = 0.f, fPosY = 0.f;

			pWarRoom->SetUseFlag( TRUE );				// ������� ������ ����
			pWarRoom->SetNPC( pNPC );					// NPC ����
			pWarRoom->SetPlayType( CWPT_TEAM_A_APPLY );	// Ŭ���� ���� Ÿ�� ����
			pWarRoom->SetWarType( dwWarType );			// Ŭ���� Ÿ��
			pWarRoom->SetElapsedPlayTime( 0 );			// ���� ���� �ð�

			// ���� �ٸ�
			i64BatJuly = pWarRoomTemplate->GetWarTypeBatJuly( dwWarType );
			pWarRoom->SetBatJuly( i64BatJuly );
			// ���� �ٸ� ����
			i64Temp = pApplyUser->GetCur_MONEY();
			i64Temp = i64Temp - pWarRoomTemplate->GetWarTypeBatJuly( dwWarType );
			if( i64Temp >= 0 )
			{
				pApplyUser->SetCur_MONEY( i64Temp );
				pApplyUser->Send_gsv_SET_MONEYONLY( GSV_SET_MONEY_ONLY );
			}

			// Ŭ���� �� ��ȣ ����
			pWarRoom->SetZoneNo( pWarRoomTemplate->GetZoneSeedNo() + dwWarRoomIDX );

			// Ŭ���� A �� �� ���� ��ǥ
			fPosX = pWarRoomTemplate->GetATeamStartPos().x;
			fPosY = pWarRoomTemplate->GetATeamStartPos().y;
			pWarRoom->SetATeamWarpPos( fPosX, fPosY );

			// �⺻ ���� ��ǥ ����
			pWarRoom->SetDefaultReturnZoneNo( pWarRoomTemplate->GetDefaultReturnZoneNo() );
			fPosX = pWarRoomTemplate->GetDefaultReturnPos().x;
			fPosY = pWarRoomTemplate->GetDefaultReturnPos().y;
			pWarRoom->SetDefaultReturnPos( fPosX, fPosY );

			// �⺻ ���º� ����	
			pWarRoom->SetWarResult( CWWR_WAR_RESULT_DRAW );

			// �巹�� HP ���� �ð� (HP ������ �� �����Ҷ�)
			pWarRoom->SetBossHPUpdateTime( 10 ); // 10��

			// �� ���� (A, B�� ������)
			pWarRoom->SetGateFlag( FALSE );

			// A �� Ŭ�� ����
			SetATeam( pWarRoom, pApplyUser->GetClanNAME() );

			// A �� Ŭ���� Ÿ�� ����
			pWarRoom->SetATeamWarType( dwWarType );

			// << �� ������ ���� �����Ҷ�! >>

			// << Ŭ���� A �� ��û DB ���� >>
			GF_DB_CWar_Insert( 1, 0, i64BatJuly, pApplyUser->GetClanID(), 0, NULL );

			// (20070423:����ö) : Ŭ���� �α� �߰�
			g_LOG.CS_ODS( 0xffff, "\n[A TEAM] ZONE(%d) WAR_TYPE(%d) CLANID(%d):%s MASTERID(%s):%s",
				pWarRoom->GetZoneNo(), pWarRoom->GetWarType(), pApplyUser->GetClanID(), pApplyUser->GetClanNAME(),
				pApplyUser->Get_ACCOUNT(), pApplyUser->Get_NAME() );


#ifdef __USE_ZLOG__
			g_WatchData.Log( "[Ŭ���� A �� ��û]" );
#endif
	
		}
		else
		{
#ifdef __USE_ZLOG__
			g_WatchData.Log( "[ERROR] A �� ��û�� ������ �����ϴ�.", pWarRoom->GetZoneNo() );
#endif
			// ��û�� ���� ����!!
			pApplyUser->Send_gsv_CLANWAR_Err( CWAR_TOO_MANY_CWAR_PROGRESS );
			//LeaveCriticalSection(&m_General_LOCK);
			return FALSE;
		}
	}

	// << Ŭ���� ���� ���� �������� ���� ���� >>

	//LeaveCriticalSection(&m_General_LOCK);

	return TRUE;
}

//---------------------------------------------------------------------------
// Ŭ���� ��û ������ Ȯ��
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
			// ��û�� ������� Ŭ������ ���
			if( pClanWarRoom->IsUse() )
			{
				// A �� ����ΰ�?
				if( pClanWarRoom->GetATeam() )
				{
					if( pClanWarRoom->GetATeam()->IsUse() )
						if( pClanWarRoom->GetATeam()->IsMember( pMember ) )
							return TRUE;
				}

				// B �� ����ΰ�?
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
// ���� �α׾ƿ�
//---------------------------------------------------------------------------

BOOL CClanWar::LogOut( classUSER* pUser )
{
	ZThreadSync Sync;

	CClanFamily* pClanFamily = NULL;


	// ����ȭ
	//EnterCriticalSection( &m_General_LOCK );
	
	if( (pClanFamily = GetFamily( pUser )) != NULL )
	{
		pClanFamily->SetLogOutMember( pUser );
	}

	// Ŭ�� ���� ���� ������Ʈ
	//UpdateClanInfoList();

#ifdef __USE_ZLOG__
	int iClanCount = 0;
	int iFamilyCount = 0;
	int iLoginMemberCount = 0;
	CClanMember* pClanMember = NULL;
	ZList* pMemberList = NULL;


	g_WatchData.Log( " " );
	g_WatchData.Log( "<< �α��� Ŭ���� >>" );
	iFamilyCount = m_zlClanFamily.Count();
	g_WatchData.Log( " - Ŭ�� ��:%d", iFamilyCount );
	for( int i = 0; i < iFamilyCount; i++ )
	{
		if( (pClanFamily = (CClanFamily*)m_zlClanFamily.IndexOf(i)) != NULL )
		{
			if( pClanFamily->IsUse() )
				g_WatchData.Log( " -*- Ŭ����:%s - USE:TRUE", (pClanFamily->GetName()).c_str() );
			else
				g_WatchData.Log( " -*- Ŭ����:%s - USE:false", (pClanFamily->GetName()).c_str() );


			if( (pMemberList = pClanFamily->GetMemberList()) != NULL )
			{
				iLoginMemberCount = pMemberList->Count();
				for( int j = 0; j < iLoginMemberCount; j++ )
				{
					if( (pClanMember = (CClanMember*)pMemberList->IndexOf(j)) != NULL )
					{
						if( pClanMember->IsUse() )
							g_WatchData.Log( " --- %02d:���(%d):%s - USE:TRUE", j+1, pClanMember->GetGrade(), pClanMember->GetName() );
						else
							g_WatchData.Log( " --- %02d:���(%d):%s - USE:false", j+1, pClanMember->GetGrade(), pClanMember->GetName() );
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
// Ŭ���� ������ �޽��� ����
//---------------------------------------------------------------------------

BOOL CClanWar::ClanWarRejoinGiveUp( classUSER* pUser, DWORD dwWinTeam )
{
	CClanWarRoom* pClanWarRoom = NULL;


	pClanWarRoom = GetClanWarRoomByClanID( pUser->GetClanID() );

	if( pClanWarRoom )
	{
		// Ŭ���� ���� ���̸� ����� �¸�
		if( dwWinTeam == CWTT_A_TEAM )
		{
			// B�� ���� --> A�� �¸� //
			pClanWarRoom->SetWarResult(	CWWR_WAR_RESULT_AWIN );
			DropClanWar( pClanWarRoom, CWTT_ALL );
			// A �� ����Ʈ �¸� ó��
			WinningRewardQuest( pClanWarRoom, CWTT_A_TEAM );
			// Ŭ���̾�Ʈ Ŭ���� ���� ��ûâ �ݱ�
			pClanWarRoom->StartTimeOut();
			// �������� ����� �˷��ش�.
			pClanWarRoom->ClanWarRejoinGiveUp( CWTT_A_TEAM );
		}
		else if( dwWinTeam == CWTT_B_TEAM )
		{
			// A�� ���� --> B�� �¸� //
			pClanWarRoom->SetWarResult(	CWWR_WAR_RESULT_BWIN );
			DropClanWar( pClanWarRoom, CWTT_ALL );
			// A �� ����Ʈ �¸� ó��
			WinningRewardQuest( pClanWarRoom, CWTT_B_TEAM );
			// Ŭ���̾�Ʈ Ŭ���� ���� ��ûâ �ݱ�
			pClanWarRoom->StartTimeOut();
			// �������� ����� �˷��ش�.
			pClanWarRoom->ClanWarRejoinGiveUp( CWTT_B_TEAM );
		}
	}

	return TRUE;
}

//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
// NPC�� �ش��ϴ� �� ����Ÿ ���ø��� ��´�.
//---------------------------------------------------------------------------

CClanWarRoomTemplate* CClanWar::GetWarRoomTemplate( CObjNPC* pNPC )
{
	int iClientNpcNo = 0;	// Ŭ���̾�Ʈ���� ��û�� NPC ��ȣ
	int iWarRoomCount = 0;
	int iServerNpcNo = 0;	// ���� ���� ���ø� NPC ��ȣ
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

				// Ŭ���̾�Ʈ NPC ��ȣ�� ���� NPC ��ȣ�� ��ġ�ϴ°�?
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

	// NPC�� �ش��ϴ� �� ����Ÿ ���ø��� ��´�.
	pWarRoomTemplate = GetWarRoomTemplate( pNPC );
	if( pWarRoomTemplate )
	{
		DWORD dwRestrictLevel = 0;

		dwRestrictLevel = pWarRoomTemplate->GetWarTypeRestrictLevel( dwWarType );
		// ��� ������ �����ΰ�?
		if( dwRestrictLevel >= dwUserLevel )
			return TRUE;
	}

	return FALSE;
}

//---------------------------------------------------------------------------
// Ŭ���̾�Ʈ Ŭ���� Ÿ�� -> ���� Ŭ���� Ÿ������ ��ȯ

DWORD CClanWar::ChangeClientToServerClanWarType( DWORD dwClientClanWarType )
{
	switch( dwClientClanWarType )
	{
	case 1:
	case 2:
		//	���ú� Ŭ����
		return 0;
	case 31:
	case 32:
		//	���ú� Ŭ����
		return 1;
	case 61:
	case 62:
		//	���ú� Ŭ����
		return 2;

	default:
		return 0;
/*
	case 3:
	case 4:
		//	������ Ŭ����
		return 3;
	case 33:
	case 34:
		//	������ Ŭ����
		return 4;
	case 63:
	case 64:
		//	������ Ŭ����
		return 5;
*/
	}

	return 0;
}

//---------------------------------------------------------------------------
// Ŭ������ ��û�ϰ� ��ٸ��� ���� �ִ��� äũ

CClanWarRoom* CClanWar::CheckWarRoomReservation( CObjNPC* pNPC, DWORD dwClanWarType )
{
	CClanWarRoom* pWarRoom = NULL;
	int iRoomCount = m_zlWarRoom.Count();

	// ���� ���� �ִ°�?
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
					// ������ NPC�� ���� ���ΰ�?
					if( pRoomNPC->Get_CharNO() == pNPC->Get_CharNO() )
					{
						// ������ Ÿ���� Ŭ�����ΰ�?
						if( pWarRoom->GetWarType() == dwClanWarType )
						{
							// �̹� ��û�� A ���� �ְ� A �� �����Ͱ� ������ ������ �����϶� Ŭ���� ��Ī
							if( pWarRoom->GetATeam() && pWarRoom->GetATeam()->IsUse() && 
								pWarRoom->GetATeam()->GetMaster() && pWarRoom->GetATeam()->GetMaster()->IsUse() )
							{
								// �������� ���ΰ�?
								if( pWarRoom->GetPlayType() == CWPT_TEAM_A_APPLY )
								{
									// ������ ���̴�!!!
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
// ���� ���� �ִ��� äũ ������ ������ ������ �ش�.

CClanWarRoom* CClanWar::CheckVacantWarRoom( CObjNPC* pNPC, DWORD& dwRoomIDX )
{
	int iRoomCount = 0;
	CClanWarRoom* pWarRoom = NULL;
	CClanWarRoomTemplate* pWarRoomTemplate = NULL;

	// �������� ���� ���ø� ���� ��������
	pWarRoomTemplate = GetWarRoomTemplate( pNPC );
	if( pWarRoomTemplate )
	{
		// �ִ� ����� �ʰ��ߴ��� äũ
		if( GetLiveClanWarRoom() < (int)pWarRoomTemplate->GetMAX_ClanWarNumber() )
		{
			// ���� ���� ã�Ƽ� �����ش�.
			iRoomCount = m_zlWarRoom.Count();
			for( int i = 0; i < iRoomCount; i++ )
			{
				// pWarRoom�� �� ã�⿡ ��Ȱ��
				pWarRoom = (CClanWarRoom*)m_zlWarRoom.IndexOf(i);
				if( pWarRoom )
				{
					// ��������� ���� �� �߰�!
					if( pWarRoom->IsUse() == FALSE )
					{
#ifdef __USE_ZLOG__
						g_WatchData.Log( "%s : ������ �ʱ�ȭ(ROOM_IDX:%d)", __FUNCTION__, i );
#endif
						// �� û��!
						pWarRoom->DataClear();
						dwRoomIDX = i; // �� ��ȣ ������ ���� �� �ε��� ����

						return pWarRoom;
					}
				}
			}

			// �� �߰�!
			if( (pWarRoom = new CClanWarRoom()) == NULL )
				return NULL;
			pWarRoom->DataClear();
			m_zlWarRoom.Add( pWarRoom );
			// �� ��ȣ ������ ���� �� �ε��� ����
			//dwRoomIDX = m_zlWarRoom.Count() - 1;
			// Ex) 101�� ���� SEED��ȣ�̸� 0���� �� �ε����� ���� ��, 101�� ���� ù��° Ŭ���� ���� �ȴ�.
			dwRoomIDX = iRoomCount; // ���ٰ� ���� �ǹ�

#ifdef __USE_ZLOG__
			g_WatchData.Log( "%s : �� �� �ʱ�ȭ(ROOM_IDX:%d)", __FUNCTION__, iRoomCount );
#endif

			return pWarRoom;
		}
	}

	return NULL;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// �̹� �ε��� �йи��� �ִ°�?
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
				// ������ Ŭ���� �ΰ�?
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
// ����ִ� �йи� ����Ÿ�� �ִ°�?
// �йи� ������ �ε��ؼ� ����ϴٰ� ������ �ʾ���(m_bIsUse==FALSE) ����Ÿ
// (20070427:����ö) 0427 ���� �ּ� : ������� �ʴ� �йи� ����Ÿ�� �ִ°�?
// (20070427:����ö) 0427 ���� �ּ� : ������� �ʴ� �йи��� �Ͽ��̾��°�?
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
				// (20070427:����ö) : ���� Ŭ���� ��������� äũ
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
// A, B �� Ŭ�� �йи� ����
//---------------------------------------------------------------------------

BOOL CClanWar::SetATeam( CClanWarRoom* pWarRoom, ZString zClanName )
{
	int iClanCount = 0;
	CClanFamily* pClanFamily = NULL;

	iClanCount = m_zlClanFamily.Count();
	for( int i = 0; i < iClanCount; i++ )
	{
		// ���� Ŭ������ ������ ������ ���� �� ����
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

	// ���� Ŭ���� ����? -0-; ���� Ŭ������ ��û�Ѱɱ�... -_-;
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
		// ���� Ŭ������ ������ ������ ���� �� ����
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

	// ���� Ŭ���� ����? -0-; ���� Ŭ������ ��û�Ѱɱ�... -_-;
	return FALSE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Ŭ���� ���࿡ �ʿ��� �޽��� ����
// dwProcessType [01:�����Ϳ��� Ŭ���� ���� �޽��� ����]
//               [02:Ŭ�����鿡�� Ŭ���� ���� �޽��� ����]
//               [07:������ �����鿡�� Ŭ���� ���� ��ġ ���� ����]
//               [10:������ ������ ������. (�⺻:�ֳ����� ����)]
// dwWarpType [1:A �� Ŭ�����鿡�� Ŭ���� ���� �޽��� ����]
//            [2:B �� Ŭ�����鿡�� Ŭ���� ���� �޽��� ����]
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

		// Ŭ������ ������ �� ���� ��� �����ؾ��Ѵ�.
		if( (pAClanFamily = pClanWarRoom->GetATeam()) == NULL )
			return FALSE;
		if( (pBClanFamily = pClanWarRoom->GetBTeam()) == NULL )
			return FALSE;
		// �Ѵ� ��� �����ϸ� Ŭ���� ����
		if( pAClanFamily->IsUse() == FALSE || pBClanFamily->IsUse() == FALSE )
			return FALSE;


		// Ŭ�� �������� ���翩�ο� ���� Ŭ���� ���� ���¸� �����Ѵ�.
        if( (pAClanMaster = pAClanFamily->GetMaster()) == NULL )
			return FALSE;
		if( (pBClanMaster = pBClanFamily->GetMaster()) == NULL )
			return FALSE;

		// 1. Ŭ�� ������ ��� ����
		if( pAClanMaster->IsUse() == FALSE && pBClanMaster->IsUse() == FALSE )
		{
			// Ŭ���� ROOM �ʱ�ȭ
			pClanWarRoom->DataClear();

			// �޽��� ó�� ����
			return FALSE;
		}
		// 2. Ŭ�� ������ ��� ����
		else if( pAClanMaster->IsUse() && pBClanMaster->IsUse() )
		{
			// �� �����Ϳ��� Ŭ���� ���� ���� ����
			// A ��
			Send_gsv_CLANWAR_MASTER_WARP( pClanWarRoom, CWTT_A_TEAM );
			// B ��
			Send_gsv_CLANWAR_MASTER_WARP( pClanWarRoom, CWTT_B_TEAM );

			// Ŭ���� ���� ��ȯ
			pClanWarTemplate = GetWarRoomTemplate( pClanWarRoom->GetNPC() );
			if( pClanWarTemplate )
			{
				// �� ���� ���� û�� -> �ʵǳ� -_-;
				//RemoveMOB( pClanWarRoom, CWTT_ALL );
				// �� ����
				RegenMOB( pClanWarTemplate, pClanWarRoom );
			}
		}
		// 3. A�� Ŭ�� ������ ����
		else if( pAClanMaster->IsUse() && pBClanMaster->IsUse() == FALSE )
		{
			// A �� ������� �¸� ó��
			WinningRewardJulyItemSetting( pClanWarRoom, CWTT_A_TEAM, FALSE );

			// A �� ����Ʈ �¸� ó��
			WinningRewardQuest( pClanWarRoom, CWTT_A_TEAM );

			// Ŭ���� ROOM �ʱ�ȭ
			pClanWarRoom->DataClear();
		}		
		// 4. B�� Ŭ�� ������ ����
		else if( pAClanMaster->IsUse() == FALSE && pBClanMaster->IsUse() )
		{
			// B �� ������� �¸� ó��
			WinningRewardJulyItemSetting( pClanWarRoom, CWTT_B_TEAM, FALSE );

            // B �� ����Ʈ �¸� ó��
			WinningRewardQuest( pClanWarRoom, CWTT_B_TEAM );

			// Ŭ���� ROOM �ʱ�ȭ
			pClanWarRoom->DataClear();
		}
		// ����!!
		else
		{
			// ���� ������?
		}
		break;


	case CWMPT_MEMBER_MSG:

		// Ŭ������ ������ �� ���� ��� �����ؾ��Ѵ�.
		if( (pAClanFamily = pClanWarRoom->GetATeam()) == NULL )
			return FALSE;
		if( (pBClanFamily = pClanWarRoom->GetBTeam()) == NULL )
			return FALSE;
		// �Ѵ� ��� �����ϸ� Ŭ���� ����
		if( pAClanFamily->IsUse() == FALSE || pBClanFamily->IsUse() == FALSE )
			return FALSE;

	
		// Ŭ�� ����鿡�� ���� �޽��� ����
		switch( dwWarpType )
		{
		case CWMWT_A_TEAM_WARP:

			// A �� ��� ����
			Send_gsv_CLANWAR_MEMBER_WARP( pClanWarRoom, CWTT_A_TEAM );

			//	�÷��� Ÿ�� ����
			if( pClanWarRoom->GetPlayType() == CWPT_TEAM_B_APPLY )
				pClanWarRoom->SetPlayType( CWPT_WARP_TEAM_A );
			else if( pClanWarRoom->GetPlayType() == CWPT_WARP_TEAM_A )
				pClanWarRoom->SetPlayType( CWPT_WARP_TEAM_B );

			break;

		case CWMWT_B_TEAM_WARP:

			// B �� ��� ����
			Send_gsv_CLANWAR_MEMBER_WARP( pClanWarRoom, CWTT_B_TEAM );

			//	�÷��� Ÿ�� ����
			if( pClanWarRoom->GetPlayType() == CWPT_TEAM_B_APPLY )
				pClanWarRoom->SetPlayType( CWPT_WARP_TEAM_A );
			else if( pClanWarRoom->GetPlayType() == CWPT_WARP_TEAM_A )
				pClanWarRoom->SetPlayType( CWPT_WARP_TEAM_B );

			break;

		default:;
		}

		break;


	case CWMPT_TIME_MSG:

		// Ŭ������ ������ �� ���� ��� �����ؾ��Ѵ�.
		if( (pAClanFamily = pClanWarRoom->GetATeam()) == NULL )
			return FALSE;
		if( (pBClanFamily = pClanWarRoom->GetBTeam()) == NULL )
			return FALSE;
		// ���� �ϳ��� ������ OK
		if( pAClanFamily->IsUse() == FALSE && pBClanFamily->IsUse() == FALSE )
			return FALSE;


		// �����ϸ� Ŭ���� �������� ������Ը� Ÿ�� �޽��� ���� (������ & ��� ���)
		Send_gsv_CLANWAR_TIME_STATUS( pClanWarRoom );

		break;


	case CWMPT_RETURN_WARP_MSG:

		// Ŭ������ ������ �� ���� ��� �����ؾ��Ѵ�.
		if( (pAClanFamily = pClanWarRoom->GetATeam()) == NULL )
			return FALSE;
		if( (pBClanFamily = pClanWarRoom->GetBTeam()) == NULL )
			return FALSE;
		// ���� �ϳ��� ������ OK
		if( pAClanFamily->IsUse() == FALSE && pBClanFamily->IsUse() == FALSE )
			return FALSE;


		// ��� �� ������� Ŭ���� ���� �޽��� ����
		Send_gsv_CLANWAR_RETURN_WARP( pClanWarRoom );

		break;
	}

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Ŭ���� ����
//---------------------------------------------------------------------------

BOOL CClanWar::RegenMOB( CClanWarRoomTemplate* pClanWarRoomTemplate, CClanWarRoom* pClanWarRoom )
{
	DWORD dwWarType = pClanWarRoom->GetWarType();
	ZList* pzlMobList = NULL;
	int iMobCount = 0;
	stClanWarMOB* pClanWarMOB = NULL;
	CObjMOB* pMob = NULL;
	
	// �� Ŭ���� Ÿ�Կ� ���� ���� ����
	// A �� �� ����
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

	// B �� �� ����
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

	// PK ��� ON
	g_pZoneLIST->Set_PK_FLAG( (short)pClanWarRoom->GetZoneNo(), TRUE );

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// ������ ���� (ZoneNo, Team����)
// << �� ����:dwTeamType >>
// CWTT_ALL = 0,
// CWTT_A_TEAM = 1000,
// CWTT_B_TEAM = 2000
//---------------------------------------------------------------------------
BOOL CClanWar::RemoveMOB( CClanWarRoom* pClanWarRoom, DWORD dwTeamType )
{
	// << Ŭ���� �� ���� >>
	// 0	: ��� ��
	// 1000 : A ��
	// 2000 : B ��
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
// �¸� ����
//---------------------------------------------------------------------------

BOOL CClanWar::WinningRewardJulyItemSetting( CClanWarRoom* pClanWarRoom, DWORD dwWinTeamType, BOOL bRewardItem )
{
	////	��쿡 ���� ���� ����.
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
	case CWTT_ALL: // ���º�

		// ���� �ݾ��� 30%�� ���ϰ� ������ (�� �����ʹ� 15%�� �پ�� �ݾ��� ��������)
		i64RewardJuly = i64RewardJuly - (3 * (i64RewardJuly / 10));
		// ������� �ѷ� ������.
		i64RewardJuly = i64RewardJuly / 2;
		wJuly = (WORD)(i64RewardJuly/1000);

		// ������ ������ Ŭ���� ���� ���� (�ٸ�, ������ ����)
		// A �� ���º� ����
		pClanFamily = pClanWarRoom->GetATeam();
		if( pClanFamily )
			ChangeMasterClanValue( pClanFamily, wJuly, bRewardItem, pClanWarRoom->GetWarType() );
		// B �� ���º� ����
		pClanFamily = pClanWarRoom->GetBTeam();
		if( pClanFamily )
			ChangeMasterClanValue( pClanFamily, wJuly, bRewardItem, pClanWarRoom->GetWarType() );

		break;

	case CWTT_A_TEAM: // A �� �¸�

		// ���� �ݾ��� 10%�� ���ϰ� ������ (�¸����� ��� ����)
		i64RewardJuly = i64RewardJuly - i64RewardJuly / 10;
		wJuly = (WORD)(i64RewardJuly/1000);

		// A �� �¸� ����
		pClanFamily = pClanWarRoom->GetATeam();
		if( pClanFamily )
			ChangeMasterClanValue( pClanFamily, wJuly, bRewardItem, pClanWarRoom->GetWarType(), dwNPC_NO );

		break;

	case CWTT_B_TEAM: // B �� �¸�

		// ���� �ݾ��� 10%�� ���ϰ� ������ (�¸����� ��� ����)
		i64RewardJuly = i64RewardJuly - i64RewardJuly / 10;
		wJuly = (WORD)(i64RewardJuly/1000);

		// B �� �¸� ����
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
// ������ Ŭ�� ���� ���� (�ٸ�, ������ ����)
//---------------------------------------------------------------------------

BOOL CClanWar::ChangeMasterClanValue( CClanFamily* pClanFamily, WORD wJuly, BOOL bRewardItem, DWORD dwWarType, DWORD dwNPC_NO )
{
	CClanMember* pClanMaster = NULL;

	// << �����Ͱ� ���� ��� Ŭ�� ������ ����� ���� ���� ������� ���� ���� �ʿ� >>
	if( (pClanMaster = pClanFamily->GetTopMember( TRUE )) != NULL )
	{
		// 1. �����Ϳ��� July�ݾ�(1/1000) ����
		pClanMaster->SetClanWarValue( 5, wJuly );

		// 2. ������ ����
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
// Ŭ���� ������ ����
//---------------------------------------------------------------------------

BOOL CClanWar::Send_gsv_CLANWAR_MASTER_WARP( CClanWarRoom* pClanWarRoom, DWORD dwTeamType )
{
	classUSER* pUser = NULL;
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;


	switch( dwTeamType )
	{
	case CWTT_A_TEAM:

		// A �� ������ ����
		if( (pClanFamily = pClanWarRoom->GetATeam()) == NULL )
			return FALSE;
		if( (pClanMember = pClanFamily->GetMaster()) == NULL )
			return FALSE;
		if( (pUser = pClanMember->GetMember()) == NULL )
			return FALSE;

		pUser->Send_gsv_CLANWAR_OK( CWWT_MASTER_WARP, pClanFamily->GetIDX(), (WORD)pClanWarRoom->GetZoneNo(), CWTT_A_TEAM );

		break;

	case CWTT_B_TEAM:

		// B �� ������ ����
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
// Ŭ���� ��� ����
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
			// A �� ��� ����
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
							// ����
							pUser->Send_gsv_CLANWAR_OK( CWWT_MEMBER_WARP, pClanFamily->GetIDX(), (WORD)pClanWarRoom->GetZoneNo(), (WORD)dwTeamType );
						}
					}
				}
			}
		}
		break;

	case CWTT_B_TEAM:
		{
			// B �� ��� ����
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
							// ����
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
// ��� ��(A,B��) ������� Ŭ���� ���� �ð� ����(����ȭ)
//---------------------------------------------------------------------------

BOOL CClanWar::Send_gsv_CLANWAR_TIME_STATUS( CClanWarRoom* pClanWarRoom )
{
	int iCount = 0;
	classUSER* pUser = NULL;
	ZList* pMemberList = NULL;
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;


	// A ���� �ð� ����
	if( (pClanFamily = pClanWarRoom->GetATeam()) != NULL )
	{
		// A �� ����鿡�� ����
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


	// B �� ������� �ð� ����
	if( (pClanFamily = pClanWarRoom->GetBTeam()) != NULL )
	{
		// B �� ����鿡�� ����
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
// ��� �� ������� Ŭ���� ���� �޽��� ����
//---------------------------------------------------------------------------

BOOL CClanWar::Send_gsv_CLANWAR_RETURN_WARP( CClanWarRoom* pClanWarRoom )
{
	int iCount = 0;
	classUSER* pUser = NULL;
	ZList* pMemberList = NULL;
	CClanFamily* pClanFamily = NULL;
	CClanMember* pClanMember = NULL;


	// A �� ����
	if( (pClanFamily = pClanWarRoom->GetATeam()) != NULL )
	{
		// A �� �����
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
							// Ŭ���� ����
							pClanMember->SetUseWarFlag( FALSE );

							// ��Ƽ ����
							pUser->Party_Out();
							// ü���� 0�ΰ�� ���� ä���.
							if( pUser->Get_HP() <= 0 )
							{
								pUser->Set_HP( pUser->Get_MaxHP() );
								pUser->Send_CHAR_HPMP_INFO(FALSE);
							}

							// �ӽ� ��ġ�� ���� (����� �ֳ�)
							tPOINTF ptTempPos;
							ptTempPos.x = pClanWarRoom->GetDefaultReturnPos().x + (rand()%50) - 25;
							ptTempPos.y = pClanWarRoom->GetDefaultReturnPos().y + (rand()%50) - 25;
							// �ش� ���� Ŭ���� ���� ��ġ�� ����
							pUser->Reward_WARP( pClanWarRoom->GetDefaultReturnZoneNo(), ptTempPos );
							// �ش� ���� �� ��ȣ ����
							pUser->SetCur_TeamNO( 2 );
							pUser->Set_TeamNoFromClanIDX( 0 );
						}
					}
				}
			}
		}

	} // End of if( (pClanFamily = pClanWarRoom->GetATeam()) != NULL )


	// B �� ����
	if( (pClanFamily = pClanWarRoom->GetBTeam()) != NULL )
	{
		// B �� �����
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
							// Ŭ���� ����
							pClanMember->SetUseWarFlag( FALSE );

							// ��Ƽ ����
							pUser->Party_Out();
							// ü���� 0�ΰ�� ���� ä���.
							if( pUser->Get_HP() <= 0 )
							{
								pUser->Set_HP( pUser->Get_MaxHP() );
								pUser->Send_CHAR_HPMP_INFO(FALSE);
							}

							// �ӽ� ��ġ�� ���� (����� �ֳ�)
							tPOINTF ptTempPos;
							ptTempPos.x = pClanWarRoom->GetDefaultReturnPos().x + (rand()%50) - 25;
							ptTempPos.y = pClanWarRoom->GetDefaultReturnPos().y + (rand()%50) - 25;
							// �ش� ���� Ŭ���� ���� ��ġ�� ����
							pUser->Reward_WARP( pClanWarRoom->GetDefaultReturnZoneNo(), ptTempPos );
							// �ش� ���� �� ��ȣ ����
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
// �¸� ���� (����Ʈ)
//---------------------------------------------------------------------------

BOOL CClanWar::WinningRewardQuest( CClanWarRoom* pClanWarRoom, DWORD dwTeamType )
{
	switch( dwTeamType )
	{
	case CWTT_ALL: // ���º�
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

	case CWTT_A_TEAM: // A �� �¸� ó��
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

	case CWTT_B_TEAM: // B �� �¸� ó��
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
// Ŭ���� �������� �����Ͱ� ������ ����Ʈ ���� (���º�/�¸�/�й�)
//---------------------------------------------------------------------------

BOOL CClanWar::ChangeMasterQuest( CClanWarRoom* pClanWarRoom, DWORD dwTeamType, DWORD dwAddQuestIDX, DWORD dwRemoveQuestIDX )
{
	CClanMember* pClanMaster = NULL;


	// << �����Ͱ� ���� ��� Ŭ�� ������ ����� ���� ���� ������� ����Ʈ ���� �ʿ� >>
	switch( dwTeamType )
	{
	case CWTT_ALL: // ���º�!!
		{
			// A �� �������� ����Ʈ ����
			if( (pClanMaster = pClanWarRoom->GetATeamTopGradeMember( TRUE )) != NULL )
			{
				// �ְ� ����ڸ� �����鼭 Ŭ���� ���� �ִ��� äũ ����
				if( pClanMaster->IsUse() )
				{
					// ����Ʈ ����
					if( pClanMaster->IsMaster() )
						if( dwRemoveQuestIDX > 0 )
							RemoveQuest( pClanMaster, dwRemoveQuestIDX );
					// ���º� ����Ʈ ���
					if( dwAddQuestIDX > 0 )
						AddQuest( pClanMaster, dwAddQuestIDX );
				}
			}

			// B �� �������� ����Ʈ ����
			if( (pClanMaster = pClanWarRoom->GetBTeamTopGradeMember( TRUE )) != NULL )
			{
				// �ְ� ����ڸ� �����鼭 Ŭ���� ���� �ִ��� äũ ����
				if( pClanMaster->IsUse() )
				{
					// ����Ʈ ����
					if( pClanMaster->IsMaster() )
						if( dwRemoveQuestIDX > 0 )
							RemoveQuest( pClanMaster, dwRemoveQuestIDX );
					// ���º� ����Ʈ ���
					if( dwAddQuestIDX > 0 )
						AddQuest( pClanMaster, dwAddQuestIDX );
				}
			}
		}
		break;

	case CWTT_A_TEAM:
		{
			// A �� �������� ����Ʈ ����
			if( (pClanMaster = pClanWarRoom->GetATeamTopGradeMember( TRUE )) != NULL )
			{
				// �ְ� ����ڸ� �����鼭 Ŭ���� ���� �ִ��� äũ ����
				if( pClanMaster->IsUse() )
				{
					// ����Ʈ ����
					if( pClanMaster->IsMaster() )
						if( dwRemoveQuestIDX > 0 )
							RemoveQuest( pClanMaster, dwRemoveQuestIDX );
					// A �� �¸� ����Ʈ ���
					if( dwAddQuestIDX > 0 )
						AddQuest( pClanMaster, dwAddQuestIDX );
				}
			}

			// B �� �������� ����Ʈ ����
			if( (pClanMaster = pClanWarRoom->GetBTeamTopGradeMember( TRUE )) != NULL )
			{
				// �ְ� ����ڸ� �����鼭 Ŭ���� ���� �ִ��� äũ ����
				if( pClanMaster->IsUse() )
				{
					// ����Ʈ ����
					if( pClanMaster->IsMaster() )
						if( dwRemoveQuestIDX > 0 )
							RemoveQuest( pClanMaster, dwRemoveQuestIDX );
				}
			}
		}
		break;

	case CWTT_B_TEAM:
		{
			// B �� �������� ����Ʈ ����
			if( (pClanMaster = pClanWarRoom->GetBTeamTopGradeMember( TRUE )) != NULL )
			{
				// �ְ� ����ڸ� �����鼭 Ŭ���� ���� �ִ��� äũ ����
				if( pClanMaster->IsUse() )
				{
					// ����Ʈ ����
					if( pClanMaster->IsMaster() )
						if( dwRemoveQuestIDX > 0 )
							RemoveQuest( pClanMaster, dwRemoveQuestIDX );
					// B �� �¸� ����Ʈ ���
					if( dwAddQuestIDX > 0 )
						AddQuest( pClanMaster, dwAddQuestIDX );
				}
			}

			// A �� �������� ����Ʈ ����
			if( (pClanMaster = pClanWarRoom->GetATeamTopGradeMember( TRUE )) != NULL )
			{
				// �ְ� ����ڸ� �����鼭 Ŭ���� ���� �ִ��� äũ ����
				if( pClanMaster->IsUse() )
				{
					// ����Ʈ ����
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
// Ŭ���� ����Ʈ ����
//---------------------------------------------------------------------------

BOOL CClanWar::RemoveQuest( CClanMember* pClanMember, DWORD dwQuestIDX )
{
	DWORD dwQuestPos = 0;
	classUSER* pUser = NULL;


	if( (pUser = pClanMember->GetMember()) != NULL )
	{
		// ������ ����Ʈ�� ��ġ ���
		dwQuestPos = (DWORD)pUser->Quest_GetRegistered( (int)dwQuestIDX );
		if( dwQuestPos < QUEST_PER_PLAYER )
		{
			// ����Ʈ ����
			pUser->Quest_Delete( (int)dwQuestIDX );
			// �������� �뺸
			pUser->Send_gsv_QUEST_REPLY( RESULT_QUEST_REPLY_DEL_SUCCESS, (BYTE)dwQuestPos, (int)dwQuestIDX );
		}
	}
	else return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------
// Ŭ���� ����Ʈ ���
//---------------------------------------------------------------------------

BOOL CClanWar::AddQuest( CClanMember* pClanMember, DWORD dwQuestIDX )
{
	DWORD dwQuestPos = 0;
	classUSER* pUser = NULL;


	if( (pUser = pClanMember->GetMember()) != NULL )
	{
		// ����Ʈ ���
		dwQuestPos = pUser->Quest_Append( (int)dwQuestIDX );
		// �������� �뺸
		pUser->Send_gsv_QUEST_REPLY( RESULT_QUEST_REPLY_ADD_SUCCESS, (BYTE)dwQuestPos, (int)dwQuestIDX );
	}
	else return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------
// Ŭ���� Ŭ���̾�Ʈ ���� �޽���
//---------------------------------------------------------------------------

BOOL CClanWar::Send_gsv_CLANWAR_CLIENT_PROGRESS( CClanWarRoom* pClanWarRoom, DWORD dwClientProgress )
{
	ZString zReward;


	// DB ���
	switch( dwClientProgress )
	{
	case CWCPM_CLANWAR_A_TEAM_WIN: // A �� �¸�
		// (20070523:����ö) : Ŭ���� ���� ĳ������ ���� ����Ʈ �ο��� ��ġ ���
		zReward = pClanWarRoom->ClanWarResult();
		GF_DB_CWar_Insert( 4, 2, pClanWarRoom->GetBatJuly(), pClanWarRoom->GetATeam()->GetIDX(), pClanWarRoom->GetBTeam()->GetIDX(), zReward.c_str() );
		break;

	case CWCPM_CLANWAR_B_TEAM_WIN: // B �� �¸�
		// (20070523:����ö) : Ŭ���� ���� ĳ������ ���� ����Ʈ �ο��� ��ġ ���
		zReward = pClanWarRoom->ClanWarResult();
		GF_DB_CWar_Insert( 4, 3, pClanWarRoom->GetBatJuly(), pClanWarRoom->GetATeam()->GetIDX(), pClanWarRoom->GetBTeam()->GetIDX(), zReward.c_str() );
		break;

	case CWCPM_CLANWAR_TEAM_DRAW: // ���º�
		// (20070523:����ö) : Ŭ���� ���� ĳ������ ���� ����Ʈ �ο��� ��ġ ���
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


	// A �� ����
	if( (pClanFamily = pClanWarRoom->GetATeam()) != NULL )
	{
		// A �� �����
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
							// Ŭ���̾�Ʈ�� ���� ���� ����
							pUser->Send_gsv_CLANWAR_Progress( (WORD)dwClientProgress );

							// �������� ���
							if( pUser->Is_ClanMASTER() )
								pClanMember->SetClanWarValue( 6, 0 );

						}
					}
				}
			}
		}

	} // End of if( (pClanFamily = pClanWarRoom->GetATeam()) != NULL )


	// B �� ����
	if( (pClanFamily = pClanWarRoom->GetBTeam()) != NULL )
	{
		// B �� �����
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
							// Ŭ���̾�Ʈ�� ���� ���� ����
							pUser->Send_gsv_CLANWAR_Progress( (WORD)dwClientProgress );

							// �������� ���
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
// ��ŷ ����Ʈ ����
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

	// ��ŷ ����Ʈ ����
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

	// ��ũ ����Ʈ ����
	FLOAT fRateA = GetATeamRankPointRate( dwARankPoint, dwBRankPoint );
	FLOAT fRateB = 1.f - fRateA;

	// ����Ʈ ��� �����
	FLOAT fWin = 0.f, fLost = 0.f;

	// Ŭ���� ����(������)�� ���� ���� ����
	FLOAT fWinRate = 1.f, fLostRate = 1.f;

	// ��ũ ����Ʈ ��갪 (�������� ��)
	int iAddRate = 0;

	// Ŭ���� ���� ������ ���� ���� ����
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


	// ��ŷ ����Ʈ ���� ó��
	switch( pClanWarRoom->GetWarResult() )
	{
	case CWWR_WAR_RESULT_AWIN:
		{
			fLost = 30.f * fRateB + ((FLOAT)dwBRankPoint) * 0.01f; // ������ �ս� ����
			fWin = fLost * (1.f - fRateA) + 1.f; // �̱����� �ս� ����

			if( pAUser != NULL )
			{
				iAddRate = (int)(fWin * fWinRate);
				dwARankPoint = dwARankPoint + iAddRate;
				pAUser->m_CLAN.m_iRankPoint = (int)dwARankPoint;
				if( bNewA )
				{
					pAUser->SetClanRank( (int)dwARankPoint );
					// (20070430:����ö) : ��ŷ ����Ʈ �α�
					g_LOG.CS_ODS( 0xffff, "\n[A] RANK_POINT(%d) Clan:(%d)(%s)", (int)dwARankPoint, pAUser->GetClanID(), pAUser->GetClanNAME() );
				}
				else
				{
					pAUser->SetClanRank( iAddRate );
					// (20070430:����ö) : ��ŷ ����Ʈ �α�
					g_LOG.CS_ODS( 0xffff, "\n[A] RANK_POINT(%d) Clan:(%d)(%s)", iAddRate, pAUser->GetClanID(), pAUser->GetClanNAME() );
				}
			}

			if( pBUser != NULL )
			{
				iAddRate = - (int)(fLost * fLostRate);
				if( (dwBRankPoint * iAddRate) < 0 )
				{
					// 1�� �����.
					iAddRate = -1 * dwBRankPoint + 1;
					dwBRankPoint = 1;
				}
				else
					dwBRankPoint = dwBRankPoint + iAddRate;
				pBUser->m_CLAN.m_iRankPoint = (int)dwBRankPoint;
				if( bNewB )
				{
					pBUser->SetClanRank( (int)dwBRankPoint );
					// (20070430:����ö) : ��ŷ ����Ʈ �α�
					g_LOG.CS_ODS( 0xffff, "\n[B] RANK_POINT(%d) Clan:(%d)(%s)", (int)dwBRankPoint, pBUser->GetClanID(), pBUser->GetClanNAME() );
				}
				else
				{
					pBUser->SetClanRank( iAddRate );
					// (20070430:����ö) : ��ŷ ����Ʈ �α�
					g_LOG.CS_ODS( 0xffff, "\n[B] RANK_POINT(%d) Clan:(%d)(%s)", iAddRate, pBUser->GetClanID(), pBUser->GetClanNAME() );
				}
			}
		}
		break;

	case CWWR_WAR_RESULT_BWIN:
		{
			fLost = 30.f * fRateA + ((FLOAT)dwARankPoint) * 0.01f; // ������ �ս� ����
			fWin = fLost * (1.f - fRateB) + 1.f; // �̱����� �ս� ����

			if( pBUser != NULL )
			{
				iAddRate = (int)(fWin * fWinRate);
				dwBRankPoint = dwBRankPoint + iAddRate;
				pBUser->m_CLAN.m_iRankPoint = dwBRankPoint;
				if( bNewB )
				{
					pBUser->SetClanRank( (int)dwBRankPoint );
					// (20070430:����ö) : ��ŷ ����Ʈ �α�
					g_LOG.CS_ODS( 0xffff, "\n[B] RANK_POINT(%d) Clan:(%d)(%s)", (int)dwBRankPoint, pBUser->GetClanID(), pBUser->GetClanNAME() );
				}
				else
				{
					pBUser->SetClanRank( iAddRate );
					// (20070430:����ö) : ��ŷ ����Ʈ �α�
					g_LOG.CS_ODS( 0xffff, "\n[B] RANK_POINT(%d) Clan:(%d)(%s)", iAddRate, pBUser->GetClanID(), pBUser->GetClanNAME() );
				}
			}

			if( pAUser != NULL )
			{
				iAddRate = - (int)(fLost * fLostRate);
				if( (dwARankPoint * iAddRate) < 0 )
				{
					// 1�� �����.
					iAddRate = -1 * dwARankPoint + 1;
					dwARankPoint = 1;
				}
				else
					dwARankPoint = dwARankPoint + iAddRate;
				pAUser->m_CLAN.m_iRankPoint = dwARankPoint;
				if( bNewA )
				{
					pAUser->SetClanRank( (int)dwARankPoint );
					// (20070430:����ö) : ��ŷ ����Ʈ �α�
					g_LOG.CS_ODS( 0xffff, "\n[A] RANK_POINT(%d) Clan:(%d)(%s)", (int)dwARankPoint, pAUser->GetClanID(), pAUser->GetClanNAME() );
				}
				else
				{
					pAUser->SetClanRank( iAddRate );
					// (20070430:����ö) : ��ŷ ����Ʈ �α�
					g_LOG.CS_ODS( 0xffff, "\n[A] RANK_POINT(%d) Clan:(%d)(%s)", iAddRate, pAUser->GetClanID(), pAUser->GetClanNAME() );
				}
			}
		}
		break;

	case CWWR_WAR_RESULT_DRAW:
		{
			// ��ŷ ����Ʈ ȹ�� ����!
		}
		break;

	default:;
	}

	return TRUE;
}

//---------------------------------------------------------------------------
// ��ŷ ����Ʈ ���� ���
//---------------------------------------------------------------------------

FLOAT CClanWar::GetATeamRankPointRate( DWORD dwATeamRankPoint, DWORD dwBTeamRankPoint )
{
	//	A���� �������� ó���Ѵ�.
	int	iRate = (int)(dwBTeamRankPoint - dwATeamRankPoint);


	//	���� ó���Ѵ�.
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
// ���� ����Ʈ ����
//---------------------------------------------------------------------------
#ifdef __CLAN_REWARD_POINT
BOOL CClanWar::AddRewardPoint( CClanWarRoom* pClanWarRoom )
{
	classUSER* pUser = NULL;
	CClanMember* pClanMember = NULL;
	int iATeamRewardPoint = 0, iBTeamRewardPoint = 0;
	ZString zClanWarJoinList;	// Ŭ���� ������ ����Ʈ


	switch( pClanWarRoom->GetWarResult() )
	{
	case CWWR_WAR_RESULT_DRAW:
		// ���� ����Ʈ ����
		break;

	case CWWR_WAR_RESULT_AWIN:
		{
			// (20070319:����ö) : Ŭ���� ���� ����Ʈ ����
			switch( pClanWarRoom->GetWarType() )
			{
			case CWWT_WAR_TYPE_A: // 1000 �ٸ�
				iATeamRewardPoint = 5;
				iBTeamRewardPoint = 2;
				break;

			case CWWT_WAR_TYPE_B: // 100�� �ٸ�
				iATeamRewardPoint = 50;
				iBTeamRewardPoint = 10;
				break;

			case CWWT_WAR_TYPE_C: // 1000�� �ٸ�
				iATeamRewardPoint = 100;
				iBTeamRewardPoint = 10;
				break;
			}
		}
		break;

	case CWWR_WAR_RESULT_BWIN:
		{
			// (20070319:����ö) : Ŭ���� ���� ����Ʈ ����
			switch( pClanWarRoom->GetWarType() )
			{
			case CWWT_WAR_TYPE_A: // 1000 �ٸ�
				iATeamRewardPoint = 2;
				iBTeamRewardPoint = 5;
				break;

			case CWWT_WAR_TYPE_B: // 100�� �ٸ�
				iATeamRewardPoint = 10;
				iBTeamRewardPoint = 50;
				break;

			case CWWT_WAR_TYPE_C: // 1000�� �ٸ�
				iATeamRewardPoint = 10;
				iBTeamRewardPoint = 100;
				break;
			}
		}
		break;

	default:;
	}

	// A �� Ŭ���� ���� ����Ʈ ����
	if( (pClanMember = pClanWarRoom->GetATeamTopGradeMember( TRUE )) != NULL )
	{
		// A �� Ŭ���� ������ ����Ʈ �ۼ�
		zClanWarJoinList = pClanWarRoom->GetATeamClanWarJoinList();

		if( (pUser = pClanMember->GetMember()) != NULL )
		{
			// (20070430:����ö) : ���� ����Ʈ �α�
			g_LOG.CS_ODS( 0xffff, "\n[A] REWARD(%d) Clan:(%d)(%s) : %s", iATeamRewardPoint, pUser->GetClanID(), 
																	   pUser->GetClanNAME(), zClanWarJoinList.c_str() );
			pUser->AddClanRewardPoint( iATeamRewardPoint, zClanWarJoinList );
		}
	}

	// B ��
	if( (pClanMember = pClanWarRoom->GetBTeamTopGradeMember( TRUE )) != NULL )
	{
		// B �� Ŭ���� ������ ����Ʈ �ۼ�
		zClanWarJoinList = pClanWarRoom->GetBTeamClanWarJoinList();

		if( (pUser = pClanMember->GetMember()) != NULL )
		{
			// (20070430:����ö) : ���� ����Ʈ �α�
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
// �� ���� ����
//---------------------------------------------------------------------------

BOOL CClanWar::Send_gsv_GateStatus( CClanWarRoom* pClanWarRoom, classUSER* pUser, BOOL bIsGateOpen )
{
	if( pUser == NULL )
	{
		char szSendBuffer[512];
		gsv_cli_USER_MSG_1LV* pSendData = NULL;

        ZeroMemory( szSendBuffer, 512 );
		pSendData = (gsv_cli_USER_MSG_1LV*)szSendBuffer;

		// �޽��� ����Ÿ�� �ִ´�.
		pSendData->m_wType = GSV_USER_MSG_1LV;
		pSendData->m_nSize = sizeof( gsv_cli_USER_MSG_1LV );
		pSendData->m_Sub_Type = 100;	// �� ��Ʈ�� ����... (����ö:����? -_-;)

		if( bIsGateOpen )
		{
			if( pClanWarRoom->GetGateFlag() )
				pSendData->m_Data = 2; // �����ִ� ���� ���
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
// �� HP ����
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
// Ŭ������ �������� by Zone No
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
				// ������� ���� �� ��ȣ Ŭ���� ���� �ִ�!!
				if( pClanWarRoom->GetZoneNo() == dwZoneNo )
					return pClanWarRoom;
			}
		}

	}

	return NULL;
}

//---------------------------------------------------------------------------
// Ŭ������ �������� by Clan ID
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
				// A ���� Ŭ�� ID�� ������?
				if( (pATeam = pClanWarRoom->GetATeam()) != NULL )
				{
					if( pATeam->GetIDX() == dwClanID )
					{
						return pClanWarRoom;
					}
				}

				// B ���� Ŭ�� ID�� ������?
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
// ����ó��
//---------------------------------------------------------------------------

BOOL CClanWar::DropClanWar( CClanWarRoom* pClanWarRoom, DWORD dwDropTeamType )
{
	switch( dwDropTeamType )
	{
	case CWTT_ALL:

		// Ŭ���� �� ������ ���� ��� DropTeamType = 0���� ó���Ѵ�.
		// DropClanWar äũ�ϱ��� Ŭ���� ����� ������ �д�.
		switch( pClanWarRoom->GetWarResult() )
		{
		case CWWR_WAR_RESULT_DRAW: // ���º�
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


	case CWTT_A_TEAM: // A �� ���
		WinningRewardJulyItemSetting( pClanWarRoom, CWTT_B_TEAM, FALSE );
		pClanWarRoom->SetWarResult( CWWR_WAR_RESULT_BWIN );
		break;

	case CWTT_B_TEAM: // B �� ���
		WinningRewardJulyItemSetting( pClanWarRoom, CWTT_A_TEAM, FALSE );
		pClanWarRoom->SetWarResult( CWWR_WAR_RESULT_AWIN );
		break;

	default:
		return FALSE;
	}


	// ���� ���� Ÿ������ ��ȯ
	CClanWarRoomTemplate* pClanWarRoomTemplate = NULL;
	DWORD dwGateTime = 0, dwPlayTime = 0, dwEndTime = 0;

	// INI�� ������ Ŭ���� ���� �ð� ����
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

	// �������� ����� �˸�
	SendProcessMessage( pClanWarRoom, CWMPT_TIME_MSG, CWMWT_NO_WARP );

	// ���� ����
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

	// PK ��� ����
	g_pZoneLIST->Set_PK_FLAG( (short)pClanWarRoom->GetZoneNo(), FALSE );

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// ������ �Ҽӵ� Ŭ���� ����
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
// Ŭ���� �������� �� ����
//---------------------------------------------------------------------------

DWORD CClanWar::GetLiveClanWarRoom()
{
	int iRoomCount = 0;
	int iLiveCount = 0;
	CClanWarRoom* pClanWarRoom = NULL;


	// �ִ� ����� �ʰ��ߴ��� äũ
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
// Ŭ�� ������ ���� ����Ʈ �ڽ��� ǥ��
// (ũ��Ƽ�� ����ó�� �Լ� �������� ����Ұ�)
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
// Ŭ�� ���� ������ ����Ʈ�� ǥ��
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

