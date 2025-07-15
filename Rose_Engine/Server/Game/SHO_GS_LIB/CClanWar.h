/*---------------------------------------------------------------------------
Make:		2007.02.21
Name:		CClanWar.h
Author:     ����ö.����

Desc:		CClanWar Ŭ����
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __CCLANWAR_H__
#define __CCLANWAR_H__

//---------------------------------------------------------------------------
// BOOL CClanWar::SendProcessMessage( CClanWarRoom* pClanWarRoom, DWORD dwProcessType, DWORD dwWarpType = 0 );
// << Ŭ���� ���� �޽��� ���� >>
// 1 : �����Ϳ��� Ŭ���� ���� �޽��� ����
// 2 : Ŭ�����鿡�� Ŭ���� ���� �޽��� ����
// 7 : ������ �����鿡�� Ŭ���� ���� ��ġ ���� ����
//10 : ������ ������ ������. (�⺻:�ֳ����� ����)
enum CW_MSG_PROCESS_TYPE
{
	CWMPT_MASTER_MSG = 1,
	CWMPT_MEMBER_MSG = 2,
	CWMPT_TIME_MSG = 7,
	CWMPT_RETURN_WARP_MSG = 10
};

//---------------------------------------------------------------------------
// BOOL CClanWar::SendProcessMessage( CClanWarRoom* pClanWarRoom, DWORD dwProcessType, DWORD dwWarpType = 0 );
// << Ŭ���� ���� ���� �޽��� ���� >>
// 1 : A �� Ŭ������ Ŭ���� ������ ����
// 2 : B �� Ŭ������ Ŭ���� ������ ����
enum CW_MSG_WARP_TYPE
{
	CWMWT_NO_WARP = 0,
	CWMWT_A_TEAM_WARP = 1,
	CWMWT_B_TEAM_WARP = 2
};

//---------------------------------------------------------------------------
// BOOL classUSER::Send_gsv_CLANWAR_OK(DWORD p_Sub_Type,DWORD p_Clan_dw,WORD p_Zone_w,WORD p_Team_w);
// p_Sub_Type
// << Ŭ���� ���� ���� ��� >>
// 1 : ������ ����
// 2 : ��� ����
enum CW_WARP_TYPE
{
	CWWT_NO_WARP = 0,
	CWWT_MASTER_WARP = 1,
	CWWT_MEMBER_WARP = 2
};

//---------------------------------------------------------------------------
// BOOL classUSER::Send_gsv_CLANWAR_OK(DWORD p_Sub_Type,DWORD p_Clan_dw,WORD p_Zone_w,WORD p_Team_w);
// p_Team_w
// << Ŭ���� �� ���� >>
// 0	: ��� ��
// 1000 : A ��
// 2000 : B ��
enum CW_TEAM_TYPE
{
	CWTT_ALL = 0,
	CWTT_A_TEAM = 1000,
	CWTT_B_TEAM = 2000
};

//---------------------------------------------------------------------------
// BOOL Send_gsv_CLANWAR_CLIENT_PROGRESS( CClanWarRoom* pClanWarRoom, DWORD dwClientProgress );
// dwClinetProgress
// << Ŭ���� Ŭ���̾�Ʈ ���� �޽��� >>
// 1	: Ŭ���� ���� �޽���
// 2	: A �� �¸� �޽���
// 3	: B �� �¸� �޽���
// 4	: ���º� �޽���
enum CW_CLIENT_PROGRESS_MSG
{
	CWCPM_CLANWAR_START = 1,
	CWCPM_CLANWAR_A_TEAM_WIN = 2,
	CWCPM_CLANWAR_B_TEAM_WIN = 3,
	CWCPM_CLANWAR_TEAM_DRAW = 4
};

//---------------------------------------------------------------------------

class CClanWar : public ZMultiThreadSync< CClanWar >
{
private:
	//CRITICAL_SECTION	m_General_LOCK;
	//CRITICAL_SECTION	m_DB_LOCK;

	// Ŭ���� ���� �������� �ʱ�ȭ�� �� ����Ÿ (CClanWarRoomTemplate*)
	// (Ŭ���� ������ �ʱ�ȭ������ ���)
	ZList m_zlWarRoomTemplate;

	// Ŭ���� ���� ���� (CClanWarRoom*)
	ZList m_zlWarRoom;

	// Ŭ������ ������ Ŭ�� ����Ʈ (CClanFamily*)
	ZList m_zlClanFamily;

protected: 

	// << ũ��Ƽ�� ���� ���ο��� ��� �Ǿ���Ѵ�. >> //
	// Ex) BOOL ApplyClanWar( CObjNPC* pNPC, classUSER* pApplyUser, DWORD dwClientClanWarType );

	// NPC�� �ش��ϴ� �� ����Ÿ ���ø� or ���ø� �ε����� ��´�.
	CClanWarRoomTemplate* GetWarRoomTemplate( CObjNPC* pNPC );

	// Ŭ���� ���� ���� ���� äũ
	// (NPC�� ����ϴ� Ŭ���� ������ ��Ÿ�Ժ� ���� ���ѿ� �����ϴ°�?)
	BOOL CheckUserLevel( CObjNPC* pNPC, DWORD dwWarType, DWORD dwUserLevel );

	// Ŭ���̾�Ʈ Ŭ���� Ÿ�� -> ���� Ŭ���� Ÿ������ ��ȯ
	DWORD ChangeClientToServerClanWarType( DWORD dwClientClanWarType );

	// Ŭ���� ���� ���� äũ
	CClanWarRoom* CheckWarRoomReservation( CObjNPC* pNPC, DWORD dwClanWarType );

	// ���� ���� �ִ��� äũ
	CClanWarRoom* CheckVacantWarRoom( CObjNPC* pNPC, DWORD& dwRoomIDX /*OUT*/ );

	// �̹� �ε��� �йи��� �ִ°�?
	CClanFamily* CheckTheSameFamily( classUSER* pMember );

	// ����ִ� �йи� �޸𸮰� �ִ°�?
	// �йи� ������ �ε��ؼ� ����ϴٰ� ������ �ʾ���(m_bIsUse==FALSE) ����Ÿ
	// (20070427:����ö) 0427 ���� �ּ� : ������� �ʴ� �йи� ����Ÿ�� �ִ°�?
	// (20070427:����ö) 0427 ���� �ּ� : ������� �ʴ� �йи��� �Ͽ��̾��°�?
	CClanFamily* CheckVacantFamilyMemory( classUSER* pMember );
	// A, B �� Ŭ�� �йи� ����
	BOOL SetATeam( CClanWarRoom* pWarRoom, ZString zClanName );
	BOOL SetBTeam( CClanWarRoom* pWarRoom, ZString zClanName );

	// Ŭ���� ���࿡ �ʿ��� �޽��� ����
	// dwProcessType [01:�����Ϳ��� Ŭ���� ���� �޽��� ����]
	//               [02:Ŭ�����鿡�� Ŭ���� ���� �޽��� ����]
	//               [07:������ �����鿡�� Ŭ���� ���� ��ġ ���� ����]
	//               [10:������ ������ ������. (�⺻:�ֳ����� ����)]
	// dwWarpType [1:A �� Ŭ�����鿡�� Ŭ���� ���� �޽��� ����]
	//            [2:B �� Ŭ�����鿡�� Ŭ���� ���� �޽��� ����]
	BOOL SendProcessMessage( CClanWarRoom* pClanWarRoom, DWORD dwProcessType, DWORD dwWarpType = 0 );

	// Ŭ���� ����
	BOOL RegenMOB( CClanWarRoomTemplate* pClanWarRoomTemplate, CClanWarRoom* pClanWarRoom );

	// ������ ���� (ZoneNo, Team����)
	// << �� ���� >>
	// CWTT_ALL = 0,
	// CWTT_A_TEAM = 1000,
	// CWTT_B_TEAM = 2000
	BOOL RemoveMOB( CClanWarRoom* pClanWarRoom, DWORD dwTeamType );

	// �¸� ���� ���� (�ٸ�, ������)
	BOOL WinningRewardJulyItemSetting( CClanWarRoom* pClanWarRoom, DWORD dwWinTeamType, BOOL bItemReward );
	// ������ Ŭ�� ���� ���� (�ٸ�, ������ ����)
	BOOL ChangeMasterClanValue( CClanFamily* pClanFamily, WORD wJuly, BOOL bRewardItem, DWORD dwWarType, DWORD dwNPC_NO = 0 );


	// Ŭ���� ������ ����
	BOOL Send_gsv_CLANWAR_MASTER_WARP( CClanWarRoom* pClanWarRoom, DWORD dwTeamType );
	// Ŭ���� ��� ����
	BOOL Send_gsv_CLANWAR_MEMBER_WARP( CClanWarRoom* pClanWarRoom, DWORD dwTeamType );
	// ��� �� ������� Ŭ���� ���� �ð� ����(�ð� ����ȭ��)
	BOOL Send_gsv_CLANWAR_TIME_STATUS( CClanWarRoom* pClanWarRoom );
	// ��� �� ������� Ŭ���� ���� �޽��� ����
	BOOL Send_gsv_CLANWAR_RETURN_WARP( CClanWarRoom* pClanWarRoom );


	// �¸� ���� (����Ʈ)
	BOOL WinningRewardQuest( CClanWarRoom* pClanWarRoom, DWORD dwTeamType );
	// Ŭ���� �������� �����Ͱ� ������ ����Ʈ ����
	BOOL ChangeMasterQuest( CClanWarRoom* pClanWarRoom, DWORD dwTeamType, DWORD dwAddQuestIDX, DWORD dwRemoveQuestIDX );
	// Ŭ���� ����Ʈ ����
	BOOL RemoveQuest( CClanMember* pClanMember, DWORD dwQuestIDX );
	// Ŭ���� ����Ʈ ���
	BOOL AddQuest( CClanMember* pClanMember, DWORD dwQuestIDX );
	// Ŭ���� Ŭ���̾�Ʈ ���� �޽���
	BOOL Send_gsv_CLANWAR_CLIENT_PROGRESS( CClanWarRoom* pClanWarRoom, DWORD dwClientProgress );


	// ��ŷ ����Ʈ ����
	BOOL SetRankPoint( CClanWarRoom* pClanWarRoom );
	// ��ŷ ����Ʈ ���� ��� (B �� ��ŷ ����Ʈ = 1.f - A �� ��ŷ ����Ʈ)
	FLOAT GetATeamRankPointRate( DWORD dwATeamRankPoint, DWORD dwBTeamRankPoint );
	// ���� ����Ʈ ����
	BOOL AddRewardPoint( CClanWarRoom* pClanWarRoom );


	// �� ���� ����
	BOOL Send_gsv_GateStatus( CClanWarRoom* pClanWarRoom, classUSER* pUser, BOOL bIsGateOpen );
	// �� HP ����
	BOOL ResetTeamHP( CClanWarRoom* pClanWarRoom, DWORD dwTeamType );


	// Ŭ������ ��������
	CClanWarRoom* GetClanWarRoomByZoneNo( DWORD dwZoneNo );
	// Ŭ������ ��������
	CClanWarRoom* GetClanWarRoomByClanID( DWORD dwClanID );
	// ����ó��
	BOOL DropClanWar( CClanWarRoom* pClanWarRoom, DWORD dwDropTeamType );


	// ������ �Ҽӵ� Ŭ���� ����
	CClanFamily* GetFamily( classUSER* pUser );

	// Ŭ���� �������� �� ����
	DWORD GetLiveClanWarRoom();


public:

	// << Ÿ�̸� �Լ� >>

	// Ÿ�� ó�� �Լ�
	BOOL TimeProc();
		// ���� ���� Ÿ�� ����
		BOOL PlayTime( CClanWarRoom* pClanWarRoom, CClanWarRoomTemplate* pClanWarRoomTemplate );
		// ���� ���� Ÿ�� ����
		BOOL EndTime( CClanWarRoom* pClanWarRoom );


	// �¸� ���� (�ٸ�, ������) - Ŭ���� �� "�¸��� ¡ǥ" ����Ʈ�� ���� ����
	BOOL WinningRewardJulyItem( classUSER* pRewardUser );
	// Ŭ���� ���� ���ΰ�?
	BOOL IsBoss( DWORD dwBossIDX );
	// Ŭ���� �������� ���� HP ���� ����
	BOOL NotifyBossHP( DWORD dwZoneNo );
	// Ŭ���� ������ ��(��)�ΰ�?
	BOOL IsCreatedClanWarRoom( DWORD dwZoneNo );
	DWORD IsCreatedClanWarRoom( classUSER* pUser );
	// ��Ȱ ��ġ ���
	BOOL GetRevivePos( classUSER* pUser, tPOINTF ptDeadPos, tPOINTF& ptRevivePos /*OUT*/ );
	// Ŭ�����̸� ����Ʈ ���� ����
	BOOL CheckGateStatus( classUSER* pUser );
	// Ŭ���� ������ �������� �Ǿ����� ù��° Ŭ�� �ʿ� �����ϴ� �⺻ ���� ��ġ���� ����
	BOOL CheckDefaultReturnZone( classUSER* pUser /*IN,OUT*/ );
	// ������ ����� Ŭ���� �Ҽ����� Ȯ��
	DWORD CheckClanWarUser( classUSER* pUser );
	// Ŭ���� �������� �̵�
	DWORD WarpUserToClanWar( classUSER* pUser, DWORD dwTeamType );
	// Ŭ�� �������ΰ�?
	BOOL IsClanJoined( classUSER* pMember );
	// PK ��� äũ�� ó��
	BOOL IsPKMode( CObjCHAR* pUser );

	// Ŭ�� ������ ����Ʈ ǥ��
	// (�Ʒ��� ũ��Ƽ�� ����ó�� �Լ� �������� ����Ұ�)
	BOOL UpdateClanInfoList();

	// Ŭ�� ���� ������ ����Ʈ�� ǥ��
	// (�Ʒ��� ũ��Ƽ�� ����ó�� �Լ� �������� ����Ұ�)
	BOOL UpdateClanWarList( CClanWarRoom* pWarRoom, int i );




public: // Critical Section ó��
	CClanWar();
	~CClanWar();

	static CClanWar* GetInstance();


	// << äũ �� ��Ƽ���� �Լ� >>

	// ������ �׾��°�?
	BOOL IsBossKilled( CObjMOB* pMOB );


	BOOL ReturnBatJulyOnly( classUSER* pUser );

	// Ŭ���� ����
	BOOL DeclareMasterGiveUp( classUSER* pUser );
	BOOL GiveUpClanWar( classUSER* pUser );

	// Ŭ���� ���࿡ �ʿ��� �޽��� ����
	// dwProcessType [01:�����Ϳ��� Ŭ���� ���� �޽��� ����]
	//               [02:Ŭ�����鿡�� Ŭ���� ���� �޽��� ����]
	//               [07:������ �����鿡�� Ŭ���� ���� ��ġ ���� ����]
	//               [10:������ ������ ������. (�⺻:�ֳ����� ����)]
	// dwWarpType [1:A �� Ŭ�����鿡�� Ŭ���� ���� �޽��� ����]
	//            [2:B �� Ŭ�����鿡�� Ŭ���� ���� �޽��� ����]
	BOOL SendProcessMessage( classUSER* pUser, DWORD dwProcessType, DWORD dwWarpType = 0 );



public:
	
	// (20070509:����ö) : ��Ŷ �ݺ� ���� äũ�� ����Ұ��̳�?
	DWORD m_dwUSE;
	DWORD m_dwElapsedInvalidateTime;
	DWORD m_dwInvalidateMAXCount;

	// (20070511:����ö) : �� ������ �ð��� ��Ŷ Ƚ��
	DWORD m_dwElapsedRecvPacketMAXTime;
	// (20070514:����ö) : ��Ŷ �ʰ� ��� ����
	DWORD m_dwWarningPacketCount;
	// (20070514:����ö) : ��Ŷ �ʰ� ������� ��Ŀ��
	DWORD m_dwDisconnectPacketCount;



	// �ʱ�ȭ
	BOOL Init();
		BOOL InitWarTypeMOB( ZIniFile& zMapINI, int iTeamType/*0:A��, 1:B��*/, 
							 ZString zClanWarTypeName, CClanWarRoomTemplate* pClanWarRoomTemplate );
		BOOL InitWarTypeRewardItem( ZIniFile& zMapINI, ZString zClanWarTypeName, CClanWarRoomTemplate* pClanWarRoomTemplate );


	// ���α׷��� ������ �����ϹǷ� ����Ÿ ���� ����
	//BOOL DataClear();

	// Ŭ�� ��� ���
	// bFirstAdd : ó�� �����̸�
	BOOL AddClanMember( classUSER* pMember );
	// Ŭ�� ��� ���� ������Ʈ
	BOOL UpdateClanMember( classUSER* pMember );

	// Ŭ���� ��û
	BOOL ApplyClanWar( CObjNPC* pNPC, classUSER* pApplyUser, DWORD dwClientClanWarType );
	// Ŭ���� ��û ������ Ȯ��
	BOOL IsApplyClanWar( classUSER* pMember );

	// ���� �α׾ƿ�
	BOOL LogOut( classUSER* pUser );

	// Ŭ���� ������ �޽��� ����
	BOOL ClanWarRejoinGiveUp( classUSER* pUser, DWORD dwWinTeam );
};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

