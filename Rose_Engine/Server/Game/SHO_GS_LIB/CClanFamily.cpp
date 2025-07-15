//---------------------------------------------------------------------------

#include "ClanWar.h"

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// CClanFamily
//---------------------------------------------------------------------------

CClanFamily::CClanFamily()
{
	DataClear();
}

//---------------------------------------------------------------------------

CClanFamily::~CClanFamily()
{
	DataClear();
	// 유저 리스트
	CClanMember* pClanMember = NULL;
	int iCount = m_zlMemberList.Count();
	for( int i = 0; i < iCount; i++ )
	{
		// 기존에 멤버가 있던 클랜이면 멤버를 모두 NOT USE 상태로 초기화
		if( (pClanMember = (CClanMember*)m_zlMemberList.IndexOf(i)) != NULL )
		{
			SAFE_DELETE( pClanMember );
		}
	}
}

//---------------------------------------------------------------------------


											 
//---------------------------------------------------------------------------
// 방 청소! : 기존의 데이타 값을 초기화 한다.
//---------------------------------------------------------------------------

BOOL CClanFamily::DataClear()
{
	int i = 0;	// for 루프 카운트
	int iCount = 0; // ZList Count() 보관


	// 사용중인가?
	m_bIsUse = FALSE;

	// 클랜 정보
	ZeroMemory( &m_CLAN, sizeof(tag_MY_CLAN) );

	// 클랜 이름
	m_zClanName = "";

	// 유저 리스트
	CClanMember* pClanMember = NULL;
	iCount = m_zlMemberList.Count();
	for( i = 0; i < iCount; i++ )
	{
		// 기존에 멤버가 있던 클랜이면 멤버를 모두 NOT USE 상태로 초기화
		if( (pClanMember = (CClanMember*)m_zlMemberList.IndexOf(i)) != NULL )
		{
			pClanMember->SetUseFlag( FALSE );
			pClanMember->SetUseWarFlag( FALSE );
			pClanMember = NULL;
		}
	}

	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 사용중인 멤버 데이타인가? (비사용중:DataClear후 다시 사용)
//---------------------------------------------------------------------------

BOOL CClanFamily::SetUseFlag( BOOL bUse )
{
	int iMemberCount = 0;
	CClanMember* pClanMember = NULL;

	m_bIsUse = bUse;
	iMemberCount = m_zlMemberList.Count();

	// 모든 멤버를 FALSE로 세팅
	if( m_bIsUse == FALSE )
	{
		for( int i = 0; i < iMemberCount; i++ )
		{
			pClanMember = (CClanMember*)m_zlMemberList.IndexOf(i);
			if( pClanMember )
			{
				pClanMember->SetUseFlag( FALSE );
			}
		}
	}

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CClanFamily::IsUse()
{
	return m_bIsUse;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜 이름 설정
//---------------------------------------------------------------------------

BOOL CClanFamily::SetName( ZString zClanName )
{
	m_zClanName = zClanName;

	return TRUE;
}

//---------------------------------------------------------------------------

ZString CClanFamily::GetName()
{
	return m_zClanName;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 멤버 추가
//---------------------------------------------------------------------------

BOOL CClanFamily::AddMember( CClanMember* pMember )
{
	// << pMember는 new로 생성되어서 들어온것이므로 ZList에 추가만 해준다. >>

	// 최초 설정이면 클랜 정보를 기록한다.
	if( m_bIsUse == FALSE )
	{
		memcpy( &m_CLAN, &(pMember->GetMember()->m_CLAN), sizeof(tag_MY_CLAN) );
		m_zClanName = pMember->GetMember()->GetClanNAME(); // m_ClanNAME.Get();
	}

	// 클랜 사용중으로 세팅
	m_bIsUse = TRUE;

	// 일반 클랜원 추가
	m_zlMemberList.Add( pMember );

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL CClanFamily::UpdateMember( classUSER* pMember )
{
	int iCount = 0;
	CClanMember* pClanMember = NULL;


	iCount = m_zlMemberList.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanMember = (CClanMember*)m_zlMemberList.IndexOf(i)) != NULL )
		{
			if( pClanMember->GetMember() == pMember )
			{
				// 동일한 유저이면 클랜 패밀리의 클랜 정보 업데이트
				memcpy( &m_CLAN, &(pMember->m_CLAN), sizeof(tag_MY_CLAN) );
				m_zClanName = pMember->GetClanNAME(); // m_ClanNAME.Get();

				return TRUE;
			}
		}
	}

	return FALSE;
}

//---------------------------------------------------------------------------

CClanMember* CClanFamily::GetMaster()
{
	int iCount = 0;
	CClanMember* pClanMember = NULL;


	iCount = m_zlMemberList.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanMember = (CClanMember*)m_zlMemberList.IndexOf(i)) != NULL )
		{
			// 마스터 인가?
			if( pClanMember->GetMember() && pClanMember->GetMember()->Is_ClanMASTER() )
			{
				return pClanMember;
			}
		}
	}

	return NULL;
}

//---------------------------------------------------------------------------

CClanMember* CClanFamily::GetTopMember( DWORD dwZoneNo, BOOL bCheckClanWarZone )
{
	int iCount = 0;
	ZList* pMemberList = NULL;
	CClanMember* pClanMember = NULL;
	// 최고 등급 멤버 보관
	DWORD dwTopGrade = 0;
	CClanMember* pTopMember = NULL;


	// 멤버들
	if( (pMemberList = GetMemberList()) != NULL )
	{
		iCount = pMemberList->Count();
		for( int i = 0; i < iCount; i++ )
		{
			if( (pClanMember = (CClanMember*)pMemberList->IndexOf(i)) != NULL )
			{
				if( pClanMember->IsUse() )
				{
					// 클랜전 중인 멤버인가?
					if( bCheckClanWarZone )
					{
						if( pClanMember->IsWarZone( dwZoneNo ) )
						{
							if( dwTopGrade < pClanMember->GetGrade() )
							{
								dwTopGrade = pClanMember->GetGrade();
								pTopMember = pClanMember;
							}
						}
					}
					else
					{
						if( dwTopGrade < pClanMember->GetGrade() )
						{
							dwTopGrade = pClanMember->GetGrade();
							pTopMember = pClanMember;
						}
					}
				}
			}
		}

		// 마스터 혹은 최고 등급자
		return pTopMember;
	}

	return NULL;
}

//---------------------------------------------------------------------------

ZList* CClanFamily::GetMemberList()
{
	return &m_zlMemberList;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 비어있는 멤버 데이타가 있는가?
// 멤버 정보를 로딩해서 사용하다가 지금은 않쓰는(m_bIsUse==FALSE) 데이타
//---------------------------------------------------------------------------

CClanMember* CClanFamily::CheckVacantMemberMemory()
{
	int iMemberCount = 0;
	CClanMember* pClanMember = NULL;

	iMemberCount = m_zlMemberList.Count();
	for( int i = 0; i < iMemberCount; i++ )
	{
		if( (pClanMember = (CClanMember*)m_zlMemberList.IndexOf(i)) != NULL )
		{
			// 로딩된 패밀리 정보중에 사용하지 않는 메모리가 있는가?
			if( pClanMember->IsUse() == FALSE )
				return pClanMember;
		}
	}

	return NULL;
}

//---------------------------------------------------------------------------

DWORD CClanFamily::GetIDX()
{
	return m_CLAN.m_dwClanID;
}

//---------------------------------------------------------------------------
// 클랜 레벨
//---------------------------------------------------------------------------

DWORD CClanFamily::GetLevel()
{
	return (DWORD)m_CLAN.m_btClanLEVEL;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜 멤버인가?
//---------------------------------------------------------------------------

BOOL CClanFamily::IsMember( classUSER* pMember )
{
	int iCount = 0;
	CClanMember* pClanMember = NULL;

	
	// 멤버인가?
	iCount = m_zlMemberList.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanMember = (CClanMember*)m_zlMemberList.IndexOf(i)) != NULL )
		{
			if( pClanMember->GetMember() && pClanMember->IsUse() )
				if( pClanMember->GetMember() == pMember )
					return TRUE;
		}
	}

	return FALSE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 클랜원 수 (bClanWar : 클랜전 참여 중인 사람만 채크할 것인가?)
// dwZoneNo : bClanWar == TRUE 일때만 사용
//---------------------------------------------------------------------------

DWORD CClanFamily::GetMemberCount( BOOL bClanWar, DWORD dwZoneNo )
{
	DWORD iCount = 0;
	DWORD iMemberCount = 0;
	CClanMember* pClanMember = NULL;


	// 클랜전 상태를 채크하는가?
	if( bClanWar )
	{
		// 멤버 카운트
		iCount = m_zlMemberList.Count();
		for( int i = 0; i < (int)iCount; i++ )
		{
			if( (pClanMember = (CClanMember*)m_zlMemberList.IndexOf(i)) != NULL )
			{
				if( pClanMember->IsUse() && pClanMember->IsWarZone( dwZoneNo ) )
					iMemberCount++;
			}
		}
	}
	else
	{
		// 멤버 카운트
		iCount = m_zlMemberList.Count();
		for( int i = 0; i < (int)iCount; i++ )
		{
			if( (pClanMember = (CClanMember*)m_zlMemberList.IndexOf(i)) != NULL )
			{
				if( pClanMember->IsUse() )
					iMemberCount++;
			}
		}
	}

	return iMemberCount;
}

//---------------------------------------------------------------------------
// 사용자 로그아웃 (클랜원 모두 로그아웃시 클랜 패밀리 UseFlag 해제)
//---------------------------------------------------------------------------

BOOL CClanFamily::SetLogOutMember( classUSER* pMember )
{
	int iCount = 0;
	CClanMember* pClanMember = NULL;


	// 클랜원 로그아웃 세팅
	iCount = m_zlMemberList.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanMember = (CClanMember*)m_zlMemberList.IndexOf(i)) != NULL )
		{
			if( pClanMember->GetMember() == pMember && pClanMember->IsUse() )
			{
				pClanMember->SetUseFlag( FALSE );
				break;
			}
		}
	}

	// 클랜원 채크 (없으면 Family Use Flag 해제)
	int iMemberCount = 0;

	iCount = m_zlMemberList.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanMember = (CClanMember*)m_zlMemberList.IndexOf(i)) != NULL )
		{
			if( pClanMember->IsUse() )
				iMemberCount++;
		}
	}

	// 클랜원이 모두 나갔으면 +_+ 패밀리 폐쇄!
	if( iMemberCount < 1 )
		this->SetUseFlag( FALSE );


	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 멤버 등급 채크 (클랜원 위임 기능으로 마스터가 바뀔경우 다시 채크하여 순서를 바로잡는다.)
//---------------------------------------------------------------------------
/*
BOOL CClanFamily::CheckGradeOrder()
{
	int iCount = 0;
	CClanMember* pClanMember = NULL;
	CClanMember* pTempMember = NULL;


	// (20070323:남병철)
	// 위임할때 낮은(커맨드이상)등급 멤버를 마스터로 먼저 바꾸고 이곳으로 한번 들어온다. 이때는 무시
	// 마스터를 위임자(커맨드이상)로 바꾸고나서 이곳으로 다시 오며 그때 아래 루틴을 통해 위임을 반영한다.
	if( m_pMaster && m_pMaster->IsUse() && m_pMaster->IsMaster() == FALSE )
	{
		// 1. 마스터를 멤버로 추가
		if( (pClanMember = CheckVacantMemberMemory( FALSE )) != NULL )
		{
			// 마스터를 멤버에 세팅
			pClanMember->DataClear();
			pClanMember->SetUseFlag( TRUE );
			pClanMember->SetUseWarFlag( FALSE );
			pClanMember->SetMember( m_pMaster->GetMember() );

			// 마스터 초기화
			m_pMaster->DataClear();
		}
		// 위임의 순서상 이곳으로 들어올 일은 없을듯...
		else if( (pClanMember = new CClanMember()) != NULL )
		{
			pClanMember->DataClear();
			pClanMember->SetUseFlag( TRUE );
			pClanMember->SetUseWarFlag( FALSE );
			pClanMember->SetMember( m_pMaster->GetMember() );
			AddMember( pClanMember );

			// 마스터 초기화			
			m_pMaster->DataClear();
		}


		// 2. 멤버중 마스터를 마스터로 변환
		iCount = m_zlMemberList.Count();
		for( int i = 0; i < iCount; i++ )
		{
			if( (pClanMember = (CClanMember*)m_zlMemberList.IndexOf(i)) != NULL )
			{
				// 마스터인가?
				if( pClanMember->IsMaster() && pClanMember->IsUse() )
				{
					// 이 함수 처음에 마스터를 초기화 했음
					if( (pTempMember = CheckVacantMemberMemory( TRUE )) != NULL )
					{
						pTempMember->DataClear();
						pTempMember->SetUseFlag( TRUE );
						pTempMember->SetUseWarFlag( FALSE );
						pTempMember->SetMember( pClanMember->GetMember() );

						// 멤버 초기화
						pClanMember->DataClear();
					}
					else if( (pTempMember = new CClanMember()) != NULL )
					{
						pTempMember->DataClear();
						pTempMember->SetUseFlag( TRUE );
						pTempMember->SetUseWarFlag( FALSE );
						pTempMember->SetMember( pClanMember->GetMember() );

						// 멤버를 마스터로
						AddMember( pTempMember );
						pClanMember->SetUseFlag( FALSE );
					}

					return TRUE;
				}
			}
		}
	}

	return TRUE;
}
*/

//---------------------------------------------------------------------------
// 클랜전 진입 타임아웃 (클라이언트에 떠있는 클랜전 참가관련 메시지창을 닫게한다.)
//---------------------------------------------------------------------------

BOOL CClanFamily::StartTimeOut()
{
	int iCount = 0;
	CClanMember* pClanMember = NULL;


	// 클랜원 로그아웃 세팅
	iCount = m_zlMemberList.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanMember = (CClanMember*)m_zlMemberList.IndexOf(i)) != NULL )
		{
			if( pClanMember->IsUse() )
			{
				pClanMember->StartTimeOut();
			}
		}
	}

	return TRUE;
}

//---------------------------------------------------------------------------
// 클랜전 포기한 메시지 전달
//---------------------------------------------------------------------------

BOOL CClanFamily::ClanWarRejoinGiveUp( DWORD dwEnumErrMsg )
{
	int iCount = 0;
	CClanMember* pClanMember = NULL;


	// 클랜원 로그아웃 세팅
	iCount = m_zlMemberList.Count();
	for( int i = 0; i < iCount; i++ )
	{
		if( (pClanMember = (CClanMember*)m_zlMemberList.IndexOf(i)) != NULL )
		{
			if( pClanMember->IsUse() )
			{
				pClanMember->ClanWarRejoinGiveUp( dwEnumErrMsg );
			}
		}
	}

	return TRUE;
}

//---------------------------------------------------------------------------


