/*---------------------------------------------------------------------------
Make:		2007.02.21
Name:		CClanFamily.h
Author:     ����ö.����

Desc:		Ŭ���� �� Ŭ�� ������ ����
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __CCLANFAMILY_H__
#define __CCLANFAMILY_H__

//---------------------------------------------------------------------------

class CClanFamily
{
private:
	// Ŭ�� ������ ������ΰ�? �ƴ϶�� �ٸ� Ŭ���� �߰��ɶ� ���⿡ �����.
	// Ŭ������ �� ����̶� ������ ������ TRUE
	BOOL			m_bIsUse;

/*  NET_Prototype.h

	<< tag_MY_CLAN ���� >>

	struct tag_CLAN_ID {
		DWORD	m_dwClanID;
		union 
		{
			DWORD	m_dwClanMARK;
			WORD	m_wClanMARK[2];
		} ;
		BYTE	m_btClanLEVEL;		// Ŭ�� ����
		BYTE	m_btClanPOS;		// Ŭ�������� ����
	};

	struct tag_MY_CLAN : public tag_CLAN_ID 
	{
		int				m_iClanSCORE;		// Ŭ�� ����Ʈ
		int				m_iClanRATE;		// â��ȿ��
		__int64			m_biClanMONEY;
		short			m_nMemberCNT;		// ��� ��
		int				m_iClanCONT;		// Ŭ�� �⿩��
		#ifdef	MAX_CLAN_SKILL_SLOT
			tagClanBIN		m_ClanBIN;			// Ŭ�� ���̳ʸ� ����Ÿ..
		#endif
		//	�迵ȯ : Ŭ�� ��ũ ����Ʈ �߰�
		int				m_iRankPoint;
	};
*/

	tag_MY_CLAN			m_CLAN;				// Ŭ�� ����
	ZString				m_zClanName;		// Ŭ�� �̸�
	// (20070323:����ö) : Ŭ������ ���� ��� ������ ������� �ʾ� ������ ������ ������
	//CClanMember*		m_pMaster;			// Ŭ�� ������

	// Ŭ�� ��� ����Ʈ (CClanMember*) 
	// ������ ����
	ZList				m_zlMemberList;


public:
	CClanFamily();
	~CClanFamily();

	// �� û��! : ������ ����Ÿ ���� �ʱ�ȭ �Ѵ�.
	BOOL DataClear();

	// ������� ��� ����Ÿ�ΰ�? (������:DataClear�� �ٽ� ���)
	BOOL SetUseFlag( BOOL bUse );
	BOOL IsUse();

	// Ŭ�� �̸� ����
	BOOL SetName( ZString zClanName );
	ZString GetName();

	// ��� �߰�
	BOOL AddMember( CClanMember* pMember );
	BOOL UpdateMember( classUSER* pMember );
	CClanMember* GetMaster();
	CClanMember* GetTopMember( DWORD dwZoneNo, BOOL bCheckClanWarZone = FALSE );
	ZList* GetMemberList();

	// ����ִ� ��� ����Ÿ�� �ִ°�?
	// ��� ������ �ε��ؼ� ����ϴٰ� ������ �ʾ���(m_bIsUse==FALSE) ����Ÿ
	CClanMember* CheckVacantMemberMemory();

	// Ŭ�� �ε���
	DWORD GetIDX();

	// Ŭ�� ����
	DWORD GetLevel();

	// Ŭ�� ����ΰ�?
	BOOL IsMember( classUSER* pMember );

	// Ŭ���� �� (bClanWar : Ŭ���� ���� ���� ����� äũ�� ���ΰ�?)
	// dwZoneNo : bClanWar == TRUE �϶��� ���
	DWORD GetMemberCount( BOOL bClanWar = FALSE, DWORD dwZoneNo = 0 );

	// ����� �α׾ƿ� (Ŭ���� ��� �α׾ƿ��� Ŭ�� �йи� UseFlag ����)
	BOOL SetLogOutMember( classUSER* pMember );

	// ��� ��� äũ (Ŭ���� ���� ������� �����Ͱ� �ٲ��� �ٽ� äũ�Ͽ� ������ �ٷ���´�.)
	//BOOL CheckGradeOrder();

	// Ŭ���� ���� Ÿ�Ӿƿ� (Ŭ���̾�Ʈ�� ���ִ� Ŭ���� �������� �޽���â�� �ݰ��Ѵ�.)
	BOOL StartTimeOut();

	// Ŭ���� ������ �޽��� ����
	BOOL ClanWarRejoinGiveUp( DWORD dwEnumErrMsg );
};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
