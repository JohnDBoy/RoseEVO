/*---------------------------------------------------------------------------
Make:		2007.02.27
Name:		CClanWarRoomTemplate.h
Author:     ����ö.����

Desc:		Ŭ���� ������ �ε��Ѵ�.
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __CCLANWARROOMTEMPLATE_H__
#define __CCLANWARROOMTEMPLATE_H__

//---------------------------------------------------------------------------

class CClanWarRoomTemplate
{
private: 
	// << �ε� ����Ÿ >>

	// ���° �� ������ ���� Ŭ�����ΰ�?
	// Ex) MAP_FEDION_H.ini --> 100 ~ ���������� ��ȣ����(Ex 50) = 100 ~ 150
	DWORD m_dwZoneSeedNo;

	// Ŭ���� ��û�� ���� NPC
	CObjNPC*	m_pNPC;

	// ���� ��ġ (tPOINT*)
	tPOINTF m_ptATeamStartPos;	// A ��
	tPOINTF m_ptBTeamStartPos;	// B ��

	// ��Ȱ ��ġ (tPOINTF*)
	ZList m_zlATeamRevivalPos;	// A ��
	ZList m_zlBTeamRevivalPos;	// B ��

	// �⺻ ��ȯ ��ġ
	DWORD m_dwDefaultReturnZoneNo;
	tPOINTF m_ptDefaultReturnPos;

	// Ŭ���� ���� �ð�
	DWORD m_dwGateTime;
	DWORD m_dwPlayTime;
	DWORD m_dwEndTime;

	// Ŭ���� WarType�� ���� ���� (DWORD*)
	ZList m_zlWarTypeRestrictLevel;

	// Ŭ���� WarType�� ���� �ݾ� (INT64*)
	ZList m_zlWarTypeBatJuly;

	// Ŭ���� ���� �ִ� �ο� �� (�⺻ : 15 (���� �ִ밪 : 50))
	DWORD m_dwMAX_ClanWarJoiningNumberPerTeam;

	// Ŭ���� �ִ� �� (Ŭ���� �ο� ���� 50�� ��� 25, 15�� ��� 50) - �ִ� ������ 2500�� ���� ����
	DWORD m_dwMAX_ClanWarNumber;

	// Ŭ���� ���� ���� ���� ( stClanWarMOB* )
	ZList m_zlATeamWarTypeMOB;		// ��Ÿ�� ����Ʈ (ZList*)
	ZList m_zlBTeamWarTypeMOB;

	// Ŭ���� ���� �����۵�
	// Ŭ���� Ÿ�Կ� ���� ���� ������ ����Ʈ (ZList* �κ����ϸ� �� ZList�� stClanWarItem*�̴�)
	ZList m_zlRewardItems;


public:
	CClanWarRoomTemplate();
	~CClanWarRoomTemplate();

	// �� û��! : ������ ����Ÿ ���� �ʱ�ȭ �Ѵ�.
	BOOL DataClear();

	// << �ε� ����Ÿ ���� >>

	// ���° �� ������ ���� Ŭ�����ΰ�?
	BOOL SetZoneSeedNo( DWORD dwZoneSeedNo );
	DWORD GetZoneSeedNo();

	// Ŭ���� NPC
	BOOL SetNPC( CObjNPC* pNpc );
	CObjNPC* GetNPC();

	// ���� ��ġ
	BOOL SetStartPos( FLOAT fATeamX, FLOAT fATeamY, FLOAT fBTeamX, FLOAT fBTeamY );
	tPOINTF GetATeamStartPos();
	tPOINTF GetBTeamStartPos();

	// ��Ȱ ��ġ
	BOOL AddATeamRevivalPos( FLOAT fPosX, FLOAT fPosY );
	BOOL AddBTeamRevivalPos( FLOAT fPosX, FLOAT fPosY );
	tPOINTF GetATeamShortestRevivalPos( FLOAT fDeadPosX, FLOAT fDeadPosY );
	tPOINTF GetBTeamShortestRevivalPos( FLOAT fDeadPosX, FLOAT fDeadPosY );

	// ��ȯ ��ġ
	BOOL SetDefaultReturnZoneNo( DWORD dwZone );
	DWORD GetDefaultReturnZoneNo();
	BOOL SetDefaultReturnPos( FLOAT fPosX, FLOAT fPosY );
	tPOINTF GetDefaultReturnPos();

	// Ŭ���� ���� �ð�
	BOOL SetGateTime( DWORD dwGateTime );
	BOOL SetPlayTime( DWORD dwPlayTime );
	BOOL SetEndTime( DWORD dwEndTime );
	DWORD GetGateTime();
	DWORD GetPlayTime();
	DWORD GetEndTime();

	// Ŭ���� WarType�� ���� ����
	BOOL AddWarTypeRestrictLevel( DWORD dwRestrictLevel );
	DWORD GetWarTypeRestrictLevel( DWORD dwWarType );

	// Ŭ���� WarType�� ���� �ݾ�
	BOOL AddWarTypeBatJuly( INT64 i64BatJuly );
	INT64 GetWarTypeBatJuly( DWORD dwWarType );

	// Ŭ���� ���� �ִ� �ο� �� (�⺻ : 15 (���� �ִ밪 : 50))
	BOOL SetMAX_ClanWarJoiningNumberPerTeam( DWORD dwMaxNumber );
	DWORD GetMAX_ClanWarJoiningNumberPerTeam();

	// Ŭ���� �ִ� �� (Ŭ���� �ο� ���� 50�� ��� 25, 15�� ��� 50) - �ִ� ������ 2500�� ���� ����
	BOOL SetMAX_ClanWarNumber( DWORD dwMaxNumber );
	DWORD GetMAX_ClanWarNumber();

	// Ŭ���� �� ����
	BOOL AddATeamMOBInfo( ZList* pzlMobData );
	BOOL AddBTeamMOBInfo( ZList* pzlMobData );
	ZList* GetATeamMOBInfo( DWORD dwWarType );
	ZList* GetBTeamMOBInfo( DWORD dwWarType );

	// Ŭ���� ������ ����
	BOOL AddWinningRewardItem( ZList* pzlItemData );
	ZList* GetWinningRewardItem( DWORD dwWarType );
};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
