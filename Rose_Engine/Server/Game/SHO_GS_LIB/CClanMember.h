/*---------------------------------------------------------------------------
Make:		2007.02.28
Name:		CClanMember.h
Author:     ����ö.����

Desc:		Ŭ������ ���� ����
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __CCLANMEMBER_H__
#define __CCLANMEMBER_H__

//---------------------------------------------------------------------------

class CClanMember
{
private:
	// Ŭ�� ������ ������ΰ�? �ƴ϶�� �ٸ� Ŭ���� �߰��ɶ� ���⿡ �����.
	// Ŭ������ �� ����̶� ������ ������ TRUE
	BOOL			m_bIsUse;

	// Ŭ������ ������ ��� ǥ��
	BOOL			m_bIsWar;

	// ���� ���� (�����͸� ����)
	classUSER*		m_pMember;

	// Ŭ���� �����Ҷ� �ڽ��� �ִ� �� ��ȣ (Ŭ���� ������ ������ ������ ���)
	DWORD			m_dwReturnZoneNo;


public:
	CClanMember();
	~CClanMember();

	// �� û��! : ������ ����Ÿ ���� �ʱ�ȭ �Ѵ�.
	BOOL DataClear();

	// ������� ��� ����Ÿ�ΰ�? (������:DataClear�� �ٽ� ���)
	BOOL SetUseFlag( BOOL bUse );
	BOOL IsUse();

	// Ŭ������ ������ Ŭ�����ΰ�?
	BOOL SetUseWarFlag( BOOL bUse );
	BOOL IsWarZone( DWORD dwClanWarZoneNo );

	// Ŭ���� ����
	BOOL SetMember( classUSER* pMember );
	BOOL IsMaster();
	classUSER* GetMember();

	// Ŭ���� ���� �� ��ȣ (�ڽ��� �ִ���)
	BOOL SetReturnZoneNo( DWORD dwReturnZoneNo );
	DWORD GetReturnZoneNo();

	// Ŭ�� ����
	BOOL SetClanWarValue( DWORD dwIDX, WORD wValue );
	WORD GetClanWarValue( DWORD dwIDX );

	// Ŭ���� ��� ���
	DWORD GetGrade();

	// �̸�
	char* GetName();

	// ���� �̸�
	char* GetAccountName();

	// Ŭ���� ���� Ÿ�Ӿƿ� (Ŭ���̾�Ʈ�� ���ִ� Ŭ���� �������� �޽���â�� �ݰ��Ѵ�.)
	BOOL StartTimeOut();

	// Ŭ���� ������ �޽��� ����
	BOOL ClanWarRejoinGiveUp( DWORD dwEnumErrMsg );

	// ���� ����Ʈ ��������
	DWORD GetRewardPoint();
};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
