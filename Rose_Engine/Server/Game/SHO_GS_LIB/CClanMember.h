/*---------------------------------------------------------------------------
Make:		2007.02.28
Name:		CClanMember.h
Author:     남병철.레조

Desc:		클랜원의 개인 정보
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __CCLANMEMBER_H__
#define __CCLANMEMBER_H__

//---------------------------------------------------------------------------

class CClanMember
{
private:
	// 클랜 정보로 사용중인가? 아니라면 다른 클랜이 추가될때 여기에 덮어쓴다.
	// 클랜원이 한 사람이라도 접속해 있으면 TRUE
	BOOL			m_bIsUse;

	// 클랜전에 참여한 멤버 표시
	BOOL			m_bIsWar;

	// 유저 정보 (포인터만 보관)
	classUSER*		m_pMember;

	// 클랜전 시작할때 자신이 있던 맵 번호 (클랜전 종류후 복귀할 맵으로 사용)
	DWORD			m_dwReturnZoneNo;


public:
	CClanMember();
	~CClanMember();

	// 방 청소! : 기존의 데이타 값을 초기화 한다.
	BOOL DataClear();

	// 사용중인 멤버 데이타인가? (비사용중:DataClear후 다시 사용)
	BOOL SetUseFlag( BOOL bUse );
	BOOL IsUse();

	// 클랜전에 참여한 클랜원인가?
	BOOL SetUseWarFlag( BOOL bUse );
	BOOL IsWarZone( DWORD dwClanWarZoneNo );

	// 클랜원 세팅
	BOOL SetMember( classUSER* pMember );
	BOOL IsMaster();
	classUSER* GetMember();

	// 클랜전 복귀 존 번호 (자신이 있던곳)
	BOOL SetReturnZoneNo( DWORD dwReturnZoneNo );
	DWORD GetReturnZoneNo();

	// 클랜 변수
	BOOL SetClanWarValue( DWORD dwIDX, WORD wValue );
	WORD GetClanWarValue( DWORD dwIDX );

	// 클랜내 멤버 등급
	DWORD GetGrade();

	// 이름
	char* GetName();

	// 계정 이름
	char* GetAccountName();

	// 클랜전 진입 타임아웃 (클라이언트에 떠있는 클랜전 참가관련 메시지창을 닫게한다.)
	BOOL StartTimeOut();

	// 클랜전 포기한 메시지 전달
	BOOL ClanWarRejoinGiveUp( DWORD dwEnumErrMsg );

	// 보상 포인트 가져오기
	DWORD GetRewardPoint();
};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
