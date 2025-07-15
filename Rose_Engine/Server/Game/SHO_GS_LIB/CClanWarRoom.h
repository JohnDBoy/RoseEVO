/*---------------------------------------------------------------------------
Make:		2007.02.21
Name:		CClanWarRoom.h
Author:     ����ö.����

Desc:		Ŭ���� �� ����Ÿ Ŭ����
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __CCLANWARROOM_H__
#define __CCLANWARROOM_H__

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------

// << Ŭ���� ���� ���� >>
// 0 : Ŭ���� ���� ��
// 1 : Ŭ���� ��û (ù��° Ŭ��)
// 2 : Ŭ���� ��û (�ι�° Ŭ��)
// 3,4 : Ŭ���� �����ڵ� �ش� Ŭ�� ������ ����
// 5 : ���� ���� �ð�
enum CW_PLAY_TYPE 
{
	CWPT_BEFORE_CLANWAR = 0,
	CWPT_TEAM_A_APPLY,
	CWPT_TEAM_B_APPLY,
	CWPT_WARP_TEAM_A,
	CWPT_WARP_TEAM_B,
	CWPT_END_CLANWAR
};

//---------------------------------------------------------------------------

// << Ŭ���� ���� ���� >>
// 0 : 1000 �ٸ���
// 1 : 100�� �ٸ���
// 2 : 1000�� �ٸ���
enum CW_WAR_TYPE
{
	CWWT_WAR_TYPE_A = 0,
	CWWT_WAR_TYPE_B,
	CWWT_WAR_TYPE_C,

	MAX_CWWT_WAR_TYPE
};

//---------------------------------------------------------------------------

// << Ŭ���� ��� >>
// 0 : ���º�
// 1 : A �� �¸�
// 2 : B �� �¸�
enum CW_WAR_RESULT
{
	CWWR_WAR_RESULT_DRAW = 0,
	CWWR_WAR_RESULT_AWIN,
	CWWR_WAR_RESULT_BWIN
};

//---------------------------------------------------------------------------

class CClanWarRoom
{
private:
	// << ���� ���� ����Ÿ >>

	// Ŭ���� ������?
	BOOL m_bIsUse;

	// ����� ��
	BOOL m_bIsATeamGiveUp;
	BOOL m_bIsBTeamGiveUp;

	// ���° �� ������ ���� Ŭ�����ΰ�?
	// Ex) MAP_FEDION_H.ini --> 100 ~ ���������� ��ȣ����(Ex 50) = 100 ~ 150
	DWORD m_dwZoneSeedNo;

	// Ŭ���� ��û�� ���� NPC
	CObjNPC*	m_pNPC;

	// << Ŭ���� ���� ���� >>
	// 0 : Ŭ���� ���� ��
	// 1 : Ŭ���� ��û (ù��° Ŭ��)
	// 2 : Ŭ���� ��û (�ι�° Ŭ��)
	// 3,4 : Ŭ���� �����ڵ� �ش� Ŭ�� ������ ����
	// 5 : ���� ���� �ð�
	DWORD m_dwPlayType;

	// << Ŭ���� ���� ���� >>
	// 0 : 1000 �ٸ���
	// 1 : 100�� �ٸ���
	// 2 : 1000�� �ٸ���
	DWORD m_dwWarType;

	// ���� ���� ���� �ð� ( 1 = 10��, 1�� = 6 ) Ex) 20�� = 20 * 6 = 120
	DWORD m_dwElapsedPlayTime;

	// ���� �ݾ�
	INT64 m_i64BatJuly;

	// Ŭ���� �� ��ȣ
	DWORD m_dwZoneNo;
	tPOINTF m_ptATeamZoneWarpPos;
	tPOINTF m_ptBTeamZoneWarpPos;

	// �⺻ ��ȯ ��ġ
	DWORD m_dwDefaultReturnZoneNo;
	tPOINTF m_ptDefaultReturnPos;

	// << Ŭ���� ��� >>
	// 0 : ���º�
	// 1 : A �� �¸�
	// 2 : B �� �¸�
	DWORD m_dwClanWarResult;

	// �巹�� HP ���� �ð� �� ���Ž� HP
	DWORD m_dwBossHPUpdateTime;
	DWORD m_dwATeamBossHP;
	DWORD m_dwBTeamBossHP;

	// Ŭ���� ����Ʈ ����/���� ����
	BOOL m_bGateOpen;

	// A, B �� Ŭ�� ������
	CClanFamily* m_pATeam;
	CClanFamily* m_pBTeam;
	// ��û�� Ŭ���̾�Ʈ���� ���޵� Ŭ���� Ÿ��
	DWORD m_dwATeamWarType;
	DWORD m_dwBTeamWarType;
    
	// A, B �� ���� �ο� - ��� ���ҵ�... ��.�� �����ϰ� ������!
	//DWORD m_dwATeamMemberCount;
	//DWORD m_dwBTeamMemberCount;

	// �ʿ� ������ �� (CObjMOB*)
	ZList m_zlATeamMOB;
	ZList m_zlBTeamMOB;

	// (20070523:����ö) : Ŭ���� ���� ����Ʈ �� ��� ����
	ZString m_zClanWarResult;

public:
	CClanWarRoom();
	~CClanWarRoom();

	// �� û��! : ������ ����Ÿ ���� �ʱ�ȭ �Ѵ�.
	BOOL DataClear();
	
	// << ���� ���� ����Ÿ ���� >>

	// Ŭ���� ���� ��� ���� Set/Ȯ��
	BOOL SetUseFlag( BOOL bUse );
	BOOL IsUse();

	// ����� ��(������)
	BOOL SetTeamGiveUp( classUSER* pMaster );
	BOOL GetATeamGiveUp();
	BOOL GetBTeamGiveUp();


	// ���° �� ������ ���� Ŭ�����ΰ�?
	BOOL SetZoneSeedNo( DWORD dwRoomTemplateID );
	DWORD GetZoneSeedNo();

	// Ŭ���� NPC
	BOOL SetNPC( CObjNPC* pNpc );
	CObjNPC* GetNPC();

	// << Ŭ���� ���� ���� >>
	// 0 : Ŭ���� ���� ��
	// 1 : Ŭ���� ��û (ù��° Ŭ��)
	// 2 : Ŭ���� ��û (�ι�° Ŭ��)
	// 3,4 : Ŭ���� �����ڵ� �ش� Ŭ�� ������ ����
	// 5 : ���� ���� �ð�
	BOOL SetPlayType( DWORD dwPlayType );
	DWORD GetPlayType();

	// << Ŭ���� Ÿ�� >>
	// 0 : 1000 �ٸ���
	// 1 : 100�� �ٸ���
	// 2 : 1000�� �ٸ���
	BOOL SetWarType( DWORD dwWarType );
	DWORD GetWarType();

	// �÷��� �ð� ��� (Ŭ���� ���� ���¿� ���� �ٸ���.)
	BOOL SetElapsedPlayTime( DWORD dwElapsedPlayTime );
	DWORD GetElapsedPlayTime();

	// �ٸ� ����
	BOOL SetBatJuly( INT64 i64BatJuly );
	INT64 GetBatJuly();

	// Ŭ���� �� ��ȣ
	BOOL SetZoneNo( DWORD dwZoneNo );
	DWORD GetZoneNo();

	// Ŭ���� ���� ��ġ
	BOOL SetATeamWarpPos( FLOAT fPosX, FLOAT fPosY );
	tPOINTF GetATeamWarpPos();
	BOOL SetBTeamWarpPos( FLOAT fPosX, FLOAT fPosY );
	tPOINTF GetBTeamWarpPos();

	// ��ȯ ��ġ
	BOOL SetDefaultReturnZoneNo( DWORD dwZoneNo );
	DWORD GetDefaultReturnZoneNo();
	BOOL SetDefaultReturnPos( FLOAT fPosX, FLOAT fPosY );
	tPOINTF GetDefaultReturnPos();

	// << Ŭ���� ��� >>
	// 0 : ���º�
	// 1 : A �� �¸�
	// 2 : B �� �¸�
	BOOL SetWarResult( DWORD dwWarResult );
	DWORD GetWarResult();

	// �巹�� HP ���� �ð� �� ���Ž� HP
	BOOL SetBossHPUpdateTime( DWORD dwUpdateTime );
	DWORD GetBossHPUpdateTime();
	BOOL SetATeamBossHP( DWORD dwBossHP );
	DWORD GetATeamBossHP();
	BOOL SetBTeamBossHP( DWORD dwBossHP );
	DWORD GetBTeamBossHP();

	// Ŭ���� ����Ʈ ����/���� ����
	BOOL SetGateFlag( BOOL bGateOpen );
	BOOL GetGateFlag();

	// A, B �� Ŭ�� ������
	BOOL SetATeam( CClanFamily* pClanFamily );
	BOOL SetBTeam( CClanFamily* pClanFamily );
	CClanFamily* GetATeam();
	CClanFamily* GetBTeam();

	// ��û�� Ŭ���̾�Ʈ���� ���޵� Ŭ���� Ÿ��
	BOOL SetATeamWarType( DWORD dwWarType );
	BOOL SetBTeamWarType( DWORD dwWarType );
	DWORD GetATeamWarType();
	DWORD GetBTeamWarType();

	// �ֻ��� ����� ���� (�⺻:Ŭ�� ������)
	CClanMember* GetATeamTopGradeMember( BOOL bCheckClanWarZone = FALSE );
	CClanMember* GetBTeamTopGradeMember( BOOL bCheckClanWarZone = FALSE );
	// A,B�� Ŭ���� ��
	DWORD GetATeamMemberCount( BOOL bCheckClanWarZone = FALSE );
	DWORD GetBTeamMemberCount( BOOL bCheckClanWarZone = FALSE );

	// �ʿ� ������ �� (CObjMOB*)
	BOOL AddATeamMOB( CObjMOB* pMOB );
	BOOL AddBTeamMOB( CObjMOB* pMOB );
	ZList* GetATeamMOBList();
	ZList* GetBTeamMOBList();

	// �� ����Ÿ �ʱ�ȭ
	BOOL ClearATeamMOB();
	BOOL ClearBTeamMOB();
	BOOL ClearAllTeamMOB();

	// ����� ������� ����
	BOOL IsATeamMember( classUSER* pMember );
	BOOL IsBTeamMember( classUSER* pMember );

	// �� ������ Ŭ���� ���� �ִ� ���� ����Ʈ�� ��Ʈ������ �ۼ��Ѵ�.
	ZString GetATeamClanWarJoinList();
	ZString GetBTeamClanWarJoinList();

	// Ŭ���� ���� Ÿ�Ӿƿ� (Ŭ���̾�Ʈ�� ���ִ� Ŭ���� �������� �޽���â�� �ݰ��Ѵ�.)
	BOOL StartTimeOut();

	// Ŭ���� ������ �޽��� ����
	BOOL ClanWarRejoinGiveUp( DWORD dwWinTeam );

	// ���� ����Ʈ ��Ȳ zClanWarResult�� ���
	ZString& ClanWarResult();
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
