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
	// ���� ����Ʈ
	CClanMember* pClanMember = NULL;
	int iCount = m_zlMemberList.Count();
	for( int i = 0; i < iCount; i++ )
	{
		// ������ ����� �ִ� Ŭ���̸� ����� ��� NOT USE ���·� �ʱ�ȭ
		if( (pClanMember = (CClanMember*)m_zlMemberList.IndexOf(i)) != NULL )
		{
			SAFE_DELETE( pClanMember );
		}
	}
}

//---------------------------------------------------------------------------


											 
//---------------------------------------------------------------------------
// �� û��! : ������ ����Ÿ ���� �ʱ�ȭ �Ѵ�.
//---------------------------------------------------------------------------

BOOL CClanFamily::DataClear()
{
	int i = 0;	// for ���� ī��Ʈ
	int iCount = 0; // ZList Count() ����


	// ������ΰ�?
	m_bIsUse = FALSE;

	// Ŭ�� ����
	ZeroMemory( &m_CLAN, sizeof(tag_MY_CLAN) );

	// Ŭ�� �̸�
	m_zClanName = "";

	// ���� ����Ʈ
	CClanMember* pClanMember = NULL;
	iCount = m_zlMemberList.Count();
	for( i = 0; i < iCount; i++ )
	{
		// ������ ����� �ִ� Ŭ���̸� ����� ��� NOT USE ���·� �ʱ�ȭ
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
// ������� ��� ����Ÿ�ΰ�? (������:DataClear�� �ٽ� ���)
//---------------------------------------------------------------------------

BOOL CClanFamily::SetUseFlag( BOOL bUse )
{
	int iMemberCount = 0;
	CClanMember* pClanMember = NULL;

	m_bIsUse = bUse;
	iMemberCount = m_zlMemberList.Count();

	// ��� ����� FALSE�� ����
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
// Ŭ�� �̸� ����
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
// ��� �߰�
//---------------------------------------------------------------------------

BOOL CClanFamily::AddMember( CClanMember* pMember )
{
	// << pMember�� new�� �����Ǿ ���°��̹Ƿ� ZList�� �߰��� ���ش�. >>

	// ���� �����̸� Ŭ�� ������ ����Ѵ�.
	if( m_bIsUse == FALSE )
	{
		memcpy( &m_CLAN, &(pMember->GetMember()->m_CLAN), sizeof(tag_MY_CLAN) );
		m_zClanName = pMember->GetMember()->GetClanNAME(); // m_ClanNAME.Get();
	}

	// Ŭ�� ��������� ����
	m_bIsUse = TRUE;

	// �Ϲ� Ŭ���� �߰�
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
				// ������ �����̸� Ŭ�� �йи��� Ŭ�� ���� ������Ʈ
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
			// ������ �ΰ�?
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
	// �ְ� ��� ��� ����
	DWORD dwTopGrade = 0;
	CClanMember* pTopMember = NULL;


	// �����
	if( (pMemberList = GetMemberList()) != NULL )
	{
		iCount = pMemberList->Count();
		for( int i = 0; i < iCount; i++ )
		{
			if( (pClanMember = (CClanMember*)pMemberList->IndexOf(i)) != NULL )
			{
				if( pClanMember->IsUse() )
				{
					// Ŭ���� ���� ����ΰ�?
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

		// ������ Ȥ�� �ְ� �����
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
// ����ִ� ��� ����Ÿ�� �ִ°�?
// ��� ������ �ε��ؼ� ����ϴٰ� ������ �ʾ���(m_bIsUse==FALSE) ����Ÿ
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
			// �ε��� �йи� �����߿� ������� �ʴ� �޸𸮰� �ִ°�?
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
// Ŭ�� ����
//---------------------------------------------------------------------------

DWORD CClanFamily::GetLevel()
{
	return (DWORD)m_CLAN.m_btClanLEVEL;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Ŭ�� ����ΰ�?
//---------------------------------------------------------------------------

BOOL CClanFamily::IsMember( classUSER* pMember )
{
	int iCount = 0;
	CClanMember* pClanMember = NULL;

	
	// ����ΰ�?
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
// Ŭ���� �� (bClanWar : Ŭ���� ���� ���� ����� äũ�� ���ΰ�?)
// dwZoneNo : bClanWar == TRUE �϶��� ���
//---------------------------------------------------------------------------

DWORD CClanFamily::GetMemberCount( BOOL bClanWar, DWORD dwZoneNo )
{
	DWORD iCount = 0;
	DWORD iMemberCount = 0;
	CClanMember* pClanMember = NULL;


	// Ŭ���� ���¸� äũ�ϴ°�?
	if( bClanWar )
	{
		// ��� ī��Ʈ
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
		// ��� ī��Ʈ
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
// ����� �α׾ƿ� (Ŭ���� ��� �α׾ƿ��� Ŭ�� �йи� UseFlag ����)
//---------------------------------------------------------------------------

BOOL CClanFamily::SetLogOutMember( classUSER* pMember )
{
	int iCount = 0;
	CClanMember* pClanMember = NULL;


	// Ŭ���� �α׾ƿ� ����
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

	// Ŭ���� äũ (������ Family Use Flag ����)
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

	// Ŭ������ ��� �������� +_+ �йи� ���!
	if( iMemberCount < 1 )
		this->SetUseFlag( FALSE );


	return TRUE;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// ��� ��� äũ (Ŭ���� ���� ������� �����Ͱ� �ٲ��� �ٽ� äũ�Ͽ� ������ �ٷ���´�.)
//---------------------------------------------------------------------------
/*
BOOL CClanFamily::CheckGradeOrder()
{
	int iCount = 0;
	CClanMember* pClanMember = NULL;
	CClanMember* pTempMember = NULL;


	// (20070323:����ö)
	// �����Ҷ� ����(Ŀ�ǵ��̻�)��� ����� �����ͷ� ���� �ٲٰ� �̰����� �ѹ� ���´�. �̶��� ����
	// �����͸� ������(Ŀ�ǵ��̻�)�� �ٲٰ��� �̰����� �ٽ� ���� �׶� �Ʒ� ��ƾ�� ���� ������ �ݿ��Ѵ�.
	if( m_pMaster && m_pMaster->IsUse() && m_pMaster->IsMaster() == FALSE )
	{
		// 1. �����͸� ����� �߰�
		if( (pClanMember = CheckVacantMemberMemory( FALSE )) != NULL )
		{
			// �����͸� ����� ����
			pClanMember->DataClear();
			pClanMember->SetUseFlag( TRUE );
			pClanMember->SetUseWarFlag( FALSE );
			pClanMember->SetMember( m_pMaster->GetMember() );

			// ������ �ʱ�ȭ
			m_pMaster->DataClear();
		}
		// ������ ������ �̰����� ���� ���� ������...
		else if( (pClanMember = new CClanMember()) != NULL )
		{
			pClanMember->DataClear();
			pClanMember->SetUseFlag( TRUE );
			pClanMember->SetUseWarFlag( FALSE );
			pClanMember->SetMember( m_pMaster->GetMember() );
			AddMember( pClanMember );

			// ������ �ʱ�ȭ			
			m_pMaster->DataClear();
		}


		// 2. ����� �����͸� �����ͷ� ��ȯ
		iCount = m_zlMemberList.Count();
		for( int i = 0; i < iCount; i++ )
		{
			if( (pClanMember = (CClanMember*)m_zlMemberList.IndexOf(i)) != NULL )
			{
				// �������ΰ�?
				if( pClanMember->IsMaster() && pClanMember->IsUse() )
				{
					// �� �Լ� ó���� �����͸� �ʱ�ȭ ����
					if( (pTempMember = CheckVacantMemberMemory( TRUE )) != NULL )
					{
						pTempMember->DataClear();
						pTempMember->SetUseFlag( TRUE );
						pTempMember->SetUseWarFlag( FALSE );
						pTempMember->SetMember( pClanMember->GetMember() );

						// ��� �ʱ�ȭ
						pClanMember->DataClear();
					}
					else if( (pTempMember = new CClanMember()) != NULL )
					{
						pTempMember->DataClear();
						pTempMember->SetUseFlag( TRUE );
						pTempMember->SetUseWarFlag( FALSE );
						pTempMember->SetMember( pClanMember->GetMember() );

						// ����� �����ͷ�
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
// Ŭ���� ���� Ÿ�Ӿƿ� (Ŭ���̾�Ʈ�� ���ִ� Ŭ���� �������� �޽���â�� �ݰ��Ѵ�.)
//---------------------------------------------------------------------------

BOOL CClanFamily::StartTimeOut()
{
	int iCount = 0;
	CClanMember* pClanMember = NULL;


	// Ŭ���� �α׾ƿ� ����
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
// Ŭ���� ������ �޽��� ����
//---------------------------------------------------------------------------

BOOL CClanFamily::ClanWarRejoinGiveUp( DWORD dwEnumErrMsg )
{
	int iCount = 0;
	CClanMember* pClanMember = NULL;


	// Ŭ���� �α׾ƿ� ����
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


