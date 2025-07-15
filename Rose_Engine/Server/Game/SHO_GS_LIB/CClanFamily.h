/*---------------------------------------------------------------------------
Make:		2007.02.21
Name:		CClanFamily.h
Author:     남병철.레조

Desc:		클랜원 및 클랜 정보를 관리
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __CCLANFAMILY_H__
#define __CCLANFAMILY_H__

//---------------------------------------------------------------------------

class CClanFamily
{
private:
	// 클랜 정보로 사용중인가? 아니라면 다른 클랜이 추가될때 여기에 덮어쓴다.
	// 클랜원이 한 사람이라도 접속해 있으면 TRUE
	BOOL			m_bIsUse;

/*  NET_Prototype.h

	<< tag_MY_CLAN 정보 >>

	struct tag_CLAN_ID {
		DWORD	m_dwClanID;
		union 
		{
			DWORD	m_dwClanMARK;
			WORD	m_wClanMARK[2];
		} ;
		BYTE	m_btClanLEVEL;		// 클랜 레벨
		BYTE	m_btClanPOS;		// 클랜에서의 직위
	};

	struct tag_MY_CLAN : public tag_CLAN_ID 
	{
		int				m_iClanSCORE;		// 클랜 포인트
		int				m_iClanRATE;		// 창고효율
		__int64			m_biClanMONEY;
		short			m_nMemberCNT;		// 멤버 수
		int				m_iClanCONT;		// 클랜 기여도
		#ifdef	MAX_CLAN_SKILL_SLOT
			tagClanBIN		m_ClanBIN;			// 클랜 바이너리 데이타..
		#endif
		//	김영환 : 클랜 랭크 포인트 추가
		int				m_iRankPoint;
	};
*/

	tag_MY_CLAN			m_CLAN;				// 클랜 정보
	ZString				m_zClanName;		// 클랜 이름
	// (20070323:남병철) : 클랜에서 위임 기능 구현이 적용되지 않아 마스터 구분을 제거함
	//CClanMember*		m_pMaster;			// 클랜 마스터

	// 클랜 멤버 리스트 (CClanMember*) 
	// 마스터 포함
	ZList				m_zlMemberList;


public:
	CClanFamily();
	~CClanFamily();

	// 방 청소! : 기존의 데이타 값을 초기화 한다.
	BOOL DataClear();

	// 사용중인 멤버 데이타인가? (비사용중:DataClear후 다시 사용)
	BOOL SetUseFlag( BOOL bUse );
	BOOL IsUse();

	// 클랜 이름 설정
	BOOL SetName( ZString zClanName );
	ZString GetName();

	// 멤버 추가
	BOOL AddMember( CClanMember* pMember );
	BOOL UpdateMember( classUSER* pMember );
	CClanMember* GetMaster();
	CClanMember* GetTopMember( DWORD dwZoneNo, BOOL bCheckClanWarZone = FALSE );
	ZList* GetMemberList();

	// 비어있는 멤버 데이타가 있는가?
	// 멤버 정보를 로딩해서 사용하다가 지금은 않쓰는(m_bIsUse==FALSE) 데이타
	CClanMember* CheckVacantMemberMemory();

	// 클랜 인덱스
	DWORD GetIDX();

	// 클랜 레벨
	DWORD GetLevel();

	// 클랜 멤버인가?
	BOOL IsMember( classUSER* pMember );

	// 클랜원 수 (bClanWar : 클랜전 참여 중인 사람만 채크할 것인가?)
	// dwZoneNo : bClanWar == TRUE 일때만 사용
	DWORD GetMemberCount( BOOL bClanWar = FALSE, DWORD dwZoneNo = 0 );

	// 사용자 로그아웃 (클랜원 모두 로그아웃시 클랜 패밀리 UseFlag 해제)
	BOOL SetLogOutMember( classUSER* pMember );

	// 멤버 등급 채크 (클랜원 위임 기능으로 마스터가 바뀔경우 다시 채크하여 순서를 바로잡는다.)
	//BOOL CheckGradeOrder();

	// 클랜전 진입 타임아웃 (클라이언트에 떠있는 클랜전 참가관련 메시지창을 닫게한다.)
	BOOL StartTimeOut();

	// 클랜전 포기한 메시지 전달
	BOOL ClanWarRejoinGiveUp( DWORD dwEnumErrMsg );
};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
