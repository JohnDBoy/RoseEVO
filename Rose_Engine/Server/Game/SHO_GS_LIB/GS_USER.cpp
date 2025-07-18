

#include "LIB_gsMAIN.h"

#include "GS_USER.h"
#include "ZoneLIST.h"
#include "GS_ThreadSQL.h"
#include "GS_ThreadLOG.h"
#include "CRandom.h"
#include "GS_ListUSER.h"
#include "GS_SocketLSV.h"
#include "GS_SocketASV.h"
#include "GS_Party.h"
#include "IO_Quest.h"
#include "Calculation.h"
#include "GS_ThreadMALL.h"
#include "DEF_STB.h"

#ifdef	__INC_WORLD
	#include "CChatROOM.h"
#endif
#include "CThreadGUILD.h"


// (20070117:남병철) : 클랜전 CWAR_WARNING_MSGCODE 메시지 정의 사용을 위해
#include "GF_Define.h"


#define __MAX_ITEM_999_EXT //정의			// 2006.07.06/김대성/추가 - 아이템 999 제한 확장

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
#include "AlphabetCvt/AlphabetCvt.h"
//-------------------------------------

#define	RET_FAILED		false	
#define	RET_OK			true
#define	RET_SKIP_PROC	2

#include "classFILE.h"

#ifdef __KCHS_CHATLOG__
extern classLogFILE	 g_ChatLOG;
extern classLogFILE	 g_ChatGMLOG;
#endif

extern bool IsJAPAN ();

//	서버의 체널 번호
#ifdef __CLAN_WAR_SET
extern DWORD		G_SV_Channel;		// 채널 ID
#endif


//-------------------------------------------------------------------------------------------------
classUSER::classUSER ()
{
	// (20070509:남병철) : 반복 공격 채크용
	m_dwUSE = 0;
	m_dwElapsedInvalidateTime = 0;
	m_dwInvalidateCount = 0;
	m_dwInvalidateMAXCount = 0;

	// (20070511:남병철) : 사용자의 패킷 받는 횟수 보관 (공격자 패턴 파악용)
	m_dwRecvZoneNullPacketCount = 0;
	m_dwRecvZoneNotNullPacketCount = 0;
	m_dwElapsedRecvPacketTime = 0;
	m_dwElapsedRecvPacketMAXTime = 0;

	// (20070514:남병철) : 패킷 초과 사용 워닝
	m_dwWarningPacketCount = 0;
	// (20070514:남병철) : 패킷 초과 사용으로 디스커넥
	m_dwDisconnectPacketCount = 0;


//-------------------------------------
#ifndef __MAX_ITEM_999_EXT	// 2006.07.06/김대성/수정 - 아이템 999 제한 확장
	COMPILE_TIME_ASSERT( sizeof(tagITEM) == (6+sizeof(__int64)) );
#else
#ifdef	__ITEM_TIME_LIMIT
	//	아이템 확장 방식 + 기간제 데이터 추가.
	COMPILE_TIME_ASSERT( sizeof(tagITEM) == (8+sizeof(__int64)+sizeof(DWORD)+sizeof(WORD)) );
#else
	//	아이템 확장 방식.
	COMPILE_TIME_ASSERT( sizeof(tagITEM) == (8+sizeof(__int64)) );
#endif
#endif
//-------------------------------------

#ifdef	__INC_WORLD
	m_pNodeChatROOM = new CDLList< classUSER* >::tagNODE;
	m_pNodeChatROOM->m_VALUE = this;
#endif

	this->InitUSER ();

#ifdef __EXPERIENCE_ITEM
	memset( &m_bExpiredFlag, 0, (sizeof(BOOL)*MAX_MAINTAIN_STATUS) );
#endif
}

//-------------------------------------------------------------------------------------------------
classUSER::~classUSER ()
{
#ifdef	__INC_WORLD
	SAFE_DELETE( m_pNodeChatROOM );
#endif
//	LogString (0xffff, "classUSER::~classUSER :: Socket:%d, %d   ID:%s, CHAR:%s [ %s ] \n", this->Get_SOCKET(), this->m_iSocketIDX, this->Get_ACCOUNT(), this->Get_NAME(), this->m_IP.Get() );
}

//-------------------------------------------------------------------------------------------------

DWORD classUSER::GetQueuedPacketCount()
{
	return (DWORD)m_SendList.GetNodeCount();
}

//-------------------------------------------------------------------------------------------------

bool classUSER::IsHacking (char *szDesc, char *szFile, int iLine)
{
	g_pThreadLOG->When_ERROR ( this, szFile, iLine, szDesc );

	if( this->GetZONE() )
		g_LOG.CS_ODS (0xfff, "IsHacking[ID:%s, IP:%s Char:%s Zone:%d] %s() : %s, %d \n", this->m_Account.Get(), this->m_IP.Get(), this->Get_NAME(), this->GetZONE()->Get_ZoneNO(), szDesc, szFile, iLine);
	else
		g_LOG.CS_ODS (0xfff, "IsHacking[ID:%s, IP:%s Char:%s Zone:NULL] %s() : %s, %d \n", this->m_Account.Get(), this->m_IP.Get(), this->Get_NAME(), szDesc, szFile, iLine);

	this->CloseSocket ();
	::sndPlaySound( "Connect.WAV", SND_ASYNC);

	return false;
}

/// 퀘스트 보상으로 존 이동 시켜줌
bool classUSER::Reward_WARP( int iZoneNO, tPOINTF &PosGOTO)
{	
	if ( !g_pZoneLIST->IsValidZONE( iZoneNO ) ) {
		return false;
	}

	// 행성이동 과금 체크...
	if ( !this->Check_WarpPayment(iZoneNO) )
		return false;

	if ( g_pZoneLIST->IsAgitZONE(iZoneNO) ) {
		if ( this->GetClanID() ) {
			CZoneTHREAD *pZone = g_pZoneLIST->GetZONE( iZoneNO );
			if ( pZone ) {
				tPOINTF PosNEW = pZone->Get_AgitPOS( this->GetClanID() );

				return this->Send_gsv_RELAY_REQ( RELAY_TYPE_RECALL, iZoneNO, PosNEW );
			}
		}
		return false;
	}
	
	return this->Send_gsv_RELAY_REQ( RELAY_TYPE_RECALL, iZoneNO, PosGOTO );
}

//---------------------------------------------------------------------------
/// 소속 클랜의 클랜 변수값 변경
bool classUSER::Send_gsv_ADJ_CLAN_VAR (BYTE btVarType, int iValue, ZString zWarJoinList )
{
	classPACKET *pCPacket = Packet_AllocNLock ();

	if ( pCPacket ) {
		pCPacket->m_HEADER.m_wType = GSV_ADJ_CLAN_VAR;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_ADJ_CLAN_VAR );

		pCPacket->m_gsv_ADJ_CLAN_VAR.m_btCMD	 = GCMD_ADJ_VAR;
		pCPacket->m_gsv_ADJ_CLAN_VAR.m_dwClanID	 = this->GetClanID();
		pCPacket->m_gsv_ADJ_CLAN_VAR.m_btVarType = btVarType;	// CLVAR_INC_LEV;
		pCPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue = iValue;		// 1;

		// 클랜전 참여자 리스트
		memset( pCPacket->m_gsv_ADJ_CLAN_VAR.m_pNameList, 0, 1600 );
		memcpy( pCPacket->m_gsv_ADJ_CLAN_VAR.m_pNameList, zWarJoinList.c_str(), zWarJoinList.Length() + 1 );

		g_pThreadGUILD->Add_ClanCMD( GCMD_ADJ_VAR, this->m_iSocketIDX, (t_PACKET*)( pCPacket->m_pDATA ), this->Get_NAME() );
		Packet_ReleaseNUnlock( pCPacket );

		return true;
	}

	return false;
} 

//-------------------------------------
// 2006.12.06/김대성/추가 - 클랜레벨 설정 치트키
/// 소속 클랜의 클랜 변수값 변경
bool classUSER::Send_gsv_ADJ_CLAN_VAR2 (BYTE btVarType, int nClanID, int iValue)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( pCPacket ) {
		pCPacket->m_HEADER.m_wType = GSV_ADJ_CLAN_VAR;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_ADJ_CLAN_VAR );

		pCPacket->m_gsv_ADJ_CLAN_VAR.m_btCMD	 = GCMD_ADJ_VAR;
		pCPacket->m_gsv_ADJ_CLAN_VAR.m_dwClanID	 = nClanID;
		pCPacket->m_gsv_ADJ_CLAN_VAR.m_btVarType = btVarType;	// CLVAR_INC_LEV;
		pCPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue = iValue;		// 1;

		g_pThreadGUILD->Add_ClanCMD( GCMD_ADJ_VAR, this->m_iSocketIDX, (t_PACKET*)( pCPacket->m_pDATA ), this->Get_NAME() );
		Packet_ReleaseNUnlock( pCPacket );
		return true;
	}
	return false;
} 
//-------------------------------------


//---------------------------------------------------------------------------
//void classUSER::AddClanCONTRIBUTE(int iCon)
//{
//	// 현재 클랜 기여도 조정 & db에 기록 & WS로 전송->모든 클랜원에게 통보
//	;
//}
//---------------------------------------------------------------------------
/// 배운 클랜 스킬을 찾음
BYTE classUSER::FindClanSKILL(short nSkillNo1, short nSkillNo2)
{
#ifdef	MAX_CLAN_SKILL_SLOT
	for (short nI=0; nI<MAX_CLAN_SKILL_SLOT; nI++) {
		if ( this->m_CLAN.m_ClanBIN.m_SKILL[ nI ].m_nSkillIDX >= nSkillNo1 &&
			 this->m_CLAN.m_ClanBIN.m_SKILL[ nI ].m_nSkillIDX <= nSkillNo2 ) {
			if ( SKILL_NEED_LEVELUPPOINT( this->m_CLAN.m_ClanBIN.m_SKILL[ nI ].m_nSkillIDX ) ) {
				// 만료 날짜 체크...
				if ( this->GetCurAbsSEC() >= this->m_CLAN.m_ClanBIN.m_SKILL[ nI ].m_dwExpiredAbsSEC ) {
					this->m_CLAN.m_ClanBIN.m_SKILL[ nI ].m_nSkillIDX = 0;
					this->m_CLAN.m_ClanBIN.m_SKILL[ nI ].m_dwExpiredAbsSEC = 0;
					return MAX_CLAN_SKILL_SLOT;
				}
			}
			return (BYTE)nI;
		}
	}
	return MAX_CLAN_SKILL_SLOT;
#else
	return 0;
#endif
}

/// 클랜 스킬 추가
bool classUSER::AddClanSKILL (short nSkillNo)
{
#ifdef	MAX_CLAN_SKILL_SLOT
	if ( MAX_CLAN_SKILL_SLOT == this->FindClanSKILL(nSkillNo,nSkillNo) ) {
		// 등록된 스킬이 아니다...
		if ( this->Send_gsv_ADJ_CLAN_VAR( CLVAR_ADD_SKILL, nSkillNo ) ) {
			g_pThreadLOG->When_LearnSKILL( this, nSkillNo );
			return true;
		}
	}
#endif
	return false;
}

/// 클랜 스킬 삭제
bool classUSER::DelClanSKILL (short nSkillNo)
{
#ifdef	MAX_CLAN_SKILL_SLOT
	if ( MAX_CLAN_SKILL_SLOT != this->FindClanSKILL(nSkillNo,nSkillNo) ) {
		if ( this->Send_gsv_ADJ_CLAN_VAR( CLVAR_DEL_SKILL, nSkillNo ) ) {
			//g_pThreadLOG->When_LearnSKILL( this, (-1) * nSkillIDX );
		}
	}
#endif
	return false;
}


/// 클랜 레벨 증가
void classUSER::IncClanLEVEL ()
{
	if ( this->GetClanID() ) {
		// 현재 클랜 레벨 올림 & db에 기록 & WS로 전송->모든 클랜원에게 통보
		this->Send_gsv_ADJ_CLAN_VAR( CLVAR_INC_LEV, 1 );
	/*
		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( pCPacket ) {
			pCPacket->m_HEADER.m_wType = GSV_ADJ_CLAN_VAR;
			pCPacket->m_HEADER.m_nSize = sizeof( gsv_ADJ_CLAN_VAR );

			pCPacket->m_gsv_ADJ_CLAN_VAR.m_btCMD	 = GCMD_ADJ_VAR;
			pCPacket->m_gsv_ADJ_CLAN_VAR.m_dwClanID	 = this->GetClanID();
			pCPacket->m_gsv_ADJ_CLAN_VAR.m_btVarType = CLVAR_INC_LEV;
			pCPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue = 1;

			g_pThreadGUILD->Add_ClanCMD( GCMD_ADJ_VAR, this->m_iSocketIDX, (t_PACKET*)( pCPacket->m_pDATA ), this->Get_NAME() );
			Packet_ReleaseNUnlock( pCPacket );
		}
	*/
	}
}

/// 클랜 소유의 줄리 추가
void classUSER::AddClanMONEY(int iMoney)
{
	if ( this->GetClanID() ) {
		// 현재 클랜 머니 조정 & db에 기록 & WS로 전송->모든 클랜원에게 통보
		this->Send_gsv_ADJ_CLAN_VAR( CLVAR_ADD_ZULY, iMoney );
	/*
		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( pCPacket ) {
			pCPacket->m_HEADER.m_wType = GSV_ADJ_CLAN_VAR;
			pCPacket->m_HEADER.m_nSize = sizeof( gsv_ADJ_CLAN_VAR );

			pCPacket->m_gsv_ADJ_CLAN_VAR.m_btCMD	 = GCMD_ADJ_VAR;
			pCPacket->m_gsv_ADJ_CLAN_VAR.m_dwClanID	 = this->GetClanID();
			pCPacket->m_gsv_ADJ_CLAN_VAR.m_btVarType = CLVAR_ADD_ZULY;
			pCPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue = iMoney;

			g_pThreadGUILD->Add_ClanCMD( GCMD_ADJ_VAR, this->m_iSocketIDX, (t_PACKET*)( pCPacket->m_pDATA ), this->Get_NAME() );
			Packet_ReleaseNUnlock( pCPacket );
		}
	*/
	}
}

/// 클랜 점수 증가
void classUSER::AddClanSCORE(int iScore)
{
	if ( this->GetClanID() ) {
		// 현재 클랜 점수 조정 & db에 기록 & WS로 전송->모든 클랜원에게 통보
		this->Send_gsv_ADJ_CLAN_VAR( CLVAR_ADD_SCORE, iScore );
	/*
		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( pCPacket ) {
			pCPacket->m_HEADER.m_wType = GSV_ADJ_CLAN_VAR;
			pCPacket->m_HEADER.m_nSize = sizeof( gsv_ADJ_CLAN_VAR );

			pCPacket->m_gsv_ADJ_CLAN_VAR.m_btCMD	 = GCMD_ADJ_VAR;
			pCPacket->m_gsv_ADJ_CLAN_VAR.m_dwClanID	 = this->GetClanID();
			pCPacket->m_gsv_ADJ_CLAN_VAR.m_btVarType = CLVAR_ADD_SCORE;
			pCPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue = iScore;

			g_pThreadGUILD->Add_ClanCMD( GCMD_ADJ_VAR, this->m_iSocketIDX, (t_PACKET*)( pCPacket->m_pDATA ), this->Get_NAME() );
			Packet_ReleaseNUnlock( pCPacket );
		}
	*/
	}
}


#ifdef	__CLAN_INFO_ADD
//	클랜의 랭크 포인트 변경.
bool	classUSER::SetClanRank(int iRank)
{
	if ( this->GetClanID() ) 
	{
		//	월드 서버에 전송 한다.
		this->Send_gsv_ADJ_CLAN_VAR( CLVAR_SET_RANKPOINT, iRank );
		return true;
	}
	return false;
}
#endif



#ifdef __CLAN_REWARD_POINT
// 클랜의 보상 포인트 변경
bool classUSER::SetClanRewardPoint( int iRewardPoint )
{
	if( this->GetClanID() )
	{
		// GS의 길드 쓰레드를 통해서 WS 서버에 전송
		this->Send_gsv_ADJ_CLAN_VAR( CLVAR_SET_REWARDPOINT, iRewardPoint );

		return true;
	}

	return false;
}

bool classUSER::AddClanRewardPoint( int iRewardPoint, ZString zWarJoinList )
{
	if( this->GetClanID() )
	{
		// GS의 길드 쓰레드를 통해서 WS 서버에 전송
		this->Send_gsv_ADJ_CLAN_VAR( CLVAR_ADD_REWARDPOINT, iRewardPoint, zWarJoinList );

		return true;
	}

	return false;
}
#endif

//-------------------------------------------------------------------------------------------------
/// 클랜의 창설 조건이 맞는지 체크..
#define	NEED_CLAN_CREATE_LEVEL	30
#define	NEED_CLAN_CREATE_MONEY	1000000
bool classUSER::CheckClanCreateCondition (char cStep)
{
	int iNEED_ClanCreateMoney = NEED_CLAN_CREATE_MONEY;



	if ( this->Get_LEVEL() < NEED_CLAN_CREATE_LEVEL )
		return false;

	bool bResult = true;
	switch( cStep ) {
		case 0 :	// 생성 시작 조건만 체크...
			if ( this->GetCur_MONEY() < iNEED_ClanCreateMoney ) {
				bResult = false;
				break;
			}

			g_pThreadLOG->When_gs_CLAN (this, "Start Create", NEWLOG_CLAN_CREATE_START );	// 돈 빼기전에..

			this->LockSOCKET ();
				this->Sub_CurMONEY( iNEED_ClanCreateMoney );
				this->m_iClanCreateMoney = iNEED_ClanCreateMoney;
			this->UnlockSOCKET ();
			break;

		case 1 :	// 생성시 
		{
			char szTmp[ 128 ];
			this->LockSOCKET ();
				if ( iNEED_ClanCreateMoney != this->m_iClanCreateMoney ) {
					bResult = false;
					sprintf(szTmp, "Failed Create is Hacking?? NeedZuly: %d / %d", this->m_iClanCreateMoney, iNEED_ClanCreateMoney );
				}
				this->m_iClanCreateMoney = 0;
			this->UnlockSOCKET ();

			g_pThreadLOG->When_gs_CLAN (this, ( bResult ) ? "Success Create" : szTmp, ( bResult )?NEWLOG_CLAN_CREATE_SUCCESS:NEWLOG_CLAN_CREATE_FAILED );
			break;
		}

		case 2 :	// 생성 실패
			g_pThreadLOG->When_gs_CLAN (this, "Failed Create", NEWLOG_CLAN_CREATE_FAILED );	// 돈 복구한 후에...

			this->LockSOCKET ();
				this->Add_CurMONEY( this->m_iClanCreateMoney );
				this->m_iClanCreateMoney = 0;
			this->UnlockSOCKET ();
			break;
	}

	return bResult;
}

//-------------------------------------------------------------------------------------------------
/// 클라이언트로 부터 받은 클랜 마크 설정 패킷 처리
bool classUSER::Recv_cli_CLANMARK_SET( t_PACKET *pPacket )
{
	if ( this->GetClanID() ) {
		g_pThreadGUILD->Add_ClanCMD( GCMD_CLANMARK_SET, this->m_iSocketIDX, pPacket );
	}

	return true;
}

/// 클라이언트로 부터 받은 클랜 마크 요청 패킷 처리
bool classUSER::Recv_cli_CLANMARK_REQ( t_PACKET *pPacket )
{
	g_pThreadGUILD->Add_ClanCMD( GCMD_CLANMARK_GET, this->m_iSocketIDX, pPacket );
	return true;
}

/// 클라이언트가 요청한 클랜 마크 등록 날짜/시간 전송..
bool classUSER::Recv_cli_CLANMARK_REG_TIME( t_PACKET *pPacket )
{
	g_pThreadGUILD->Add_ClanCMD( GCMD_CLANMARK_REGTIME, this->m_iSocketIDX, pPacket );
	return true;
}

/// 클라이언트가 요청한 클랜 관련 패킷에 대한 응답 패킷 생성후 전송
bool classUSER::Send_wsv_CLANMARK_REPLY( DWORD dwClanID, WORD wMarkCRC, BYTE *pMarkData, short nDataLen )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = WSV_CLANMARK_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( wsv_CLANMARK_REPLY );

	pCPacket->m_wsv_CLANMARK_REPLY.m_dwClanID   = dwClanID;
	pCPacket->m_wsv_CLANMARK_REPLY.m_wMarkCRC16 = wMarkCRC;
	pCPacket->AppendData( pMarkData, nDataLen );

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

//-------------------------------------------------------------------------------------------------
/// 서버에 중복되지 않은 팀번호 설정
bool classUSER::Set_TeamNoFromUNIQUE ()
{
//	this->Set_AbilityValueNSend( AT_TEAM_NO, 100+this->Get_INDEX() );
	this->SetCur_TeamNO( 100 + this->Get_INDEX() );
	return true;
}

/// 클랜단위로 팀번호 설정
bool classUSER::Set_TeamNoFromClanIDX ()
{
	if ( this->GetClanID() ) {
//		this->Set_AbilityValueNSend( AT_TEAM_NO, 100+this->GetCur_GuildNO() );
		this->SetCur_TeamNO( 100 + this->GetClanID() );
		return true;
	}
	return false;
}

/// 파티단위로 팀번호 설정
bool classUSER::Set_TeamNoFromPartyIDX ()
{
	if ( this->GetPARTY() ) {
//		this->Set_AbilityValueNSend( AT_TEAM_NO, 100+this->GetPARTY()->m_wPartyWSID );
		this->SetCur_TeamNO( 100 + this->GetPARTY()->m_wPartyWSID );
		return true;
	}
	return false;
}

//	지정 값으로 팀 번호 설정 트리거 처리 함수 추가.
//  김영환 추가.
#ifdef __CLAN_WAR_SET

bool classUSER::Set_TeamNoFromUNIQUE(int p_Idx)
{
	this->SetCur_TeamNO( p_Idx );
	return true;
}

/// 클랜단위로 팀번호 설정
bool classUSER::Set_TeamNoFromClanIDX(int p_Idx)
{
	if ( this->GetClanID() ) {
		this->SetCur_TeamNO_Back( p_Idx);
		return true;
	}
	return false;
}

/// 파티단위로 팀번호 설정
bool classUSER::Set_TeamNoFromPartyIDX(int p_Idx)
{
	if ( this->GetPARTY() ) {
		this->SetCur_TeamNO( p_Idx );
		return true;
	}
	return false;
}

//	파티 탈퇴
BOOL classUSER::Party_Out()
{
	//	내가 파티에 있는지
	if ( this->GetPARTY() )
	{
		//	파티에서 나간다.
		this->Send_gsv_PARTY_REPLY(this->m_dwDBID, PARTY_REPLY_BAN );
		this->m_pPartyBUFF->Sub_PartyUSER( this->m_nPartyPOS );
	}
	return true;
}

#endif

/// 부활 위치 설정
bool classUSER::Set_RevivePOS( int iXPos, int iYPos )
{
	if ( !this->GetZONE() )
		return false;

	//this->m_nReviveZoneNO	= this->GetZONE()->Get_ZoneNO();
	//this->m_PosREVIVE.x		= iXPos;
	//this->m_PosREVIVE.y		= iYPos;

	this->m_bSetImmediateRevivePOS = true;
	this->m_PosImmediateRivive.x = (float)iXPos;
	this->m_PosImmediateRivive.y = (float)iYPos;

	return true;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/// 치트 코트 구문 분석
short classUSER::Parse_CheatCODE (char *szCode)
{


	short nProcMODE=0;

	char *pToken, *pArg1, *pArg2, *pArg3;
	char *pDelimiters = " ";

	CStrVAR *pStrVAR = this->GetZONE()->GetStrVAR ();

	pToken = pStrVAR->GetTokenFirst (szCode, pDelimiters);
	pArg1  = pStrVAR->GetTokenNext  (pDelimiters);
	if ( NULL == pToken )
		return CHEAT_INVALID;

#ifdef __LIMIT_CHEAT_ALL
/*
			if ( !_strcmpi( pToken, "/npc") ) 
			{
				// pArg1 == npc no
				int iNpcIDX = atoi( pArg1 );
				if ( iNpcIDX ) 
				{
					CObjNPC *pNPC = g_pZoneLIST->Get_LocalNPC( iNpcIDX );
					if ( pNPC ) 
					{
						pArg2 = pStrVAR->GetTokenNext (pDelimiters);	// move, call, var
						pArg3 = pStrVAR->GetTokenNext (pDelimiters);	// event id, var idx
						char *pArg4 = pStrVAR->GetTokenNext (pDelimiters);	// var value
						return Cheat_npc( pStrVAR, pNPC, iNpcIDX, pArg2, pArg3, pArg4 );
					}
				}
				return CHEAT_INVALID;
			}
*/
			return CHEAT_INVALID;
#endif

	if ( pArg1 ) {
		if ( !_strcmpi( pToken, "/logtest" ) 
			) {
//			g_pThreadLOG->When_DeletedITEM( xxx );

			//g_pThreadLOG->When_TagItemLOG( int iAction, classUSER *pSourAVT, tagITEM *pItem, short nQuantity=1, __int64 biTradeZuly=0, classUSER *pDestAVT=NULL);
			//g_pThreadLOG->When_ObjItemLOG( int iAction, classUSER *pSourAVT, CObjITEM *pItemOBJ);

			//g_pThreadLOG->When_CreateOrDestroyITEM ( classUSER *pSourAVT, tagITEM *pOutItem, tagITEM *pUseItem, short nUseCNT, BYTE btMakeOrBreak, BYTE btSucOrFail );
			//g_pThreadLOG->When_DieBY( CObjCHAR *pKillOBJ, classUSER *pDeadAVT );

			//// 레벨업
			//g_pThreadLOG->When_UpgradeITEM	( this, tagITEM *pEquipITEM, BYTE btBeforeGrade, bool bSuccess );
			//g_pThreadLOG->When_GemmingITEM	( this,	tagITEM *pEquipITEM, tagITEM *pJewelITEM, BYTE btGemming, BYTE btSuccess );
			//g_pThreadLOG->When_CheatCODE ( this, szCode );

			return 1;
		}


#ifdef	__INC_WORLD
		if ( !_strcmpi( pToken, "/test_view" ) )
		{
			int iNpcNO = atoi( pArg1 );
			if ( iNpcNO > 0 && iNpcNO < g_TblNPC.m_nDataCnt ) 
			{
				CObjNPC *pNPC = g_pZoneLIST->Get_LocalNPC( iNpcNO );
				if ( pNPC ) 
				{
					pNPC->VSet_SHOW( 2 );
				}
			}
		} else
		if ( !_strcmpi( pToken, "/mall" ) ) 
		{
			classPACKET *pCPacket = Packet_AllocNLock ();
			if ( !pCPacket )
				return 0;
			pCPacket->m_HEADER.m_wType = CLI_MALL_ITEM_REQ;
			pCPacket->m_HEADER.m_nSize = sizeof( cli_MALL_ITEM_REQ );
			pCPacket->m_cli_MALL_ITEM_REQ.m_btReqTYPE = 0;
		
			pArg2 = pStrVAR->GetTokenNext (pDelimiters);
			if ( !_strcmpi( pArg1, "list" ) ) 
			{
				pCPacket->m_cli_MALL_ITEM_REQ.m_btReqTYPE = REQ_MALL_ITEM_LIST;
			} else
			if ( !_strcmpi( pArg1, "get" ) ) 
			{
				// index
				int iInvIDX=0;
				if ( pArg2 ) 
				{
					iInvIDX = atoi( pArg2 );
				}
				pCPacket->m_HEADER.m_nSize = sizeof( cli_MALL_ITEM_REQ ) + 1;
				pCPacket->m_cli_MALL_ITEM_REQ.m_btReqTYPE = REQ_MALL_ITEM_BRING;
				pCPacket->m_cli_MALL_ITEM_REQ.m_nDupCnt   = 2;
				pCPacket->m_cli_MALL_ITEM_REQ.m_btInvIDX[0] = iInvIDX;
			} else
			if ( !_strcmpi( pArg1, "check" ) ) 
			{
				// char name
				pCPacket->m_cli_MALL_ITEM_REQ.m_btReqTYPE = REQ_MALL_ITEM_FIND_CHAR;
				if ( pArg2 )
					pCPacket->AppendString( pArg2 );
			}
			else if ( !_strcmpi( pArg1, "give" ) ) 
			{
				// index
				pCPacket->m_HEADER.m_nSize = sizeof( cli_MALL_ITEM_REQ ) + 1;
				pCPacket->m_cli_MALL_ITEM_REQ.m_btReqTYPE = REQ_MALL_ITEM_GIVE;
                pArg3 = pStrVAR->GetTokenNext (pDelimiters);
				if ( pArg3 )
				{
					pCPacket->m_cli_MALL_ITEM_REQ.m_nDupCnt = 2;
					pCPacket->m_cli_MALL_ITEM_REQ.m_btInvIDX[0] = atoi( pArg3 );
					pCPacket->AppendString( pArg2 );

	                pArg3 = pStrVAR->GetTokenNext (pDelimiters);	// desc[]
					pCPacket->AppendString( pArg3 );
				}
			} 

			this->Recv_cli_MALL_ITEM_REQ( (t_PACKET*)( pCPacket->m_pDATA ) );
			Packet_ReleaseNUnlock( pCPacket );
			return 1;
		}
#endif
		if ( !_strcmpi( pToken, "/pay" ) ) 
		{
			int iPayType = atoi( pArg1 );
			if ( iPayType > 0 && iPayType <= BILLING_MSG_PAY_IQ )
			{
				this->Send_gsv_BILLING_MESSAGE( iPayType, "200512312400" );
			}
		}
		if ( !_strcmpi( pToken, "/team" ) ) 
		{
			int iTeamNo = atoi( pArg1 );
			if ( iTeamNo >= 0 ) {
				this->m_iTeamNO = iTeamNo;
			}
		}
		if ( !_strcmpi( pToken, "/where" ) ) 
		{
			pArg2 = pStrVAR->GetTokenNext (pDelimiters);	// account
			return Cheat_where( pStrVAR, pArg1, pArg2, szCode );
		}
		if ( !_strcmpi( pToken, "/account" ) ) 
		{
			return Cheat_account( pArg1, szCode );
		}
		if ( !_strcmpi( pToken, "/move" ) ) 
		{
			pArg2 = pStrVAR->GetTokenNext (pDelimiters);	// account
			return Cheat_move( pArg1, pArg2, szCode );
		}
		// 맵이동..
		if ( !_strcmpi( pToken, "/mm" ) ) 
		{
			// pArg1 // zone no
			short nZoneNO = atoi( pArg1 );
			pArg2 = pStrVAR->GetTokenNext (pDelimiters);	// x pos	
			pArg3 = pStrVAR->GetTokenNext (pDelimiters);	// y pos
			if ( pArg2 && pArg3 ) {
				return Cheat_mm( nZoneNO, pArg2, pArg3 );
			}
		}
		if ( !_strcmpi( pToken, "/NPC") ) 
		{
			// pArg1 == npc no
			int iNpcIDX = atoi( pArg1 );
			if ( iNpcIDX ) 
			{
				CObjNPC *pNPC = g_pZoneLIST->Get_LocalNPC( iNpcIDX );
				if ( pNPC ) 
				{
					pArg2 = pStrVAR->GetTokenNext (pDelimiters);	// move, call, var
					pArg3 = pStrVAR->GetTokenNext (pDelimiters);	// event id, var idx
					char *pArg4 = pStrVAR->GetTokenNext (pDelimiters);	// var value
					return Cheat_npc( pStrVAR, pNPC, iNpcIDX, pArg2, pArg3, pArg4 );
				}
			}
			return CHEAT_INVALID;
		}
// lynxluna pakout test
		if ( !_strcmpi( pToken, "/pakout" ) )
		{
			return CHEAT_INVALID;
		}

		if ( !_strcmpi( pToken, "/regx" ) )
		{
			this->Send_gsv_WHISPER(this->Get_NAME(), this->GetZONE()->Get_REGEN() ? "Regen MURUB!" : "Regen MOKAD!");
			return CHEAT_NOLOG;
		}

		if ( !_strcmpi( pToken, "/ADD" ) ) 
		{
#ifdef __LIMIT_CHEAT_ADD
			return CHEAT_INVALID;		
#endif

			// 포인트 상승 치트코드...
			pArg2 = pStrVAR->GetTokenNext (pDelimiters);
			pArg3 = pStrVAR->GetTokenNext (pDelimiters);
			return Cheat_add( pArg1, pArg2, pArg3, szCode );
		}

		if ( !_strcmpi( pToken, "/DEL" ) ) 
		{
			// 포인트 상승 치트코드...
			pArg2 = pStrVAR->GetTokenNext (pDelimiters);
			pArg3 = pStrVAR->GetTokenNext (pDelimiters);
			nProcMODE = Cheat_del( pStrVAR, pArg1, pArg2, pArg3 );
		}
		// 아이템 관련 치트코드...
		if ( !_strcmpi( pToken, "/ITEM" ) ) 
		{
#ifdef __LIMIT_CHEAT_ITEM
			return CHEAT_INVALID;		
#endif

			pArg2 = pStrVAR->GetTokenNext (pDelimiters);
			pArg3 = pStrVAR->GetTokenNext (pDelimiters);
			if ( !pArg2 || !pArg3 )
				return CHEAT_INVALID;
			char *pArg4 = pStrVAR->GetTokenNext (pDelimiters);
			return Cheat_item( pArg1, pArg2, pArg3, pArg4 );
		}

//-------------------------------------
// 2007.01.10/김대성/추가 - 기간제 아이템, 테스트용
		// 기간제아이템 관련 치트코드...
#ifdef __ITEM_TIME_LIMIT
		if ( !_strcmpi( pToken, "/ITEM_TIME_LIMIT" ) ) 
		{
#ifdef __LIMIT_CHEAT_ITEM
			return CHEAT_INVALID;		
#endif
			pArg2 = pStrVAR->GetTokenNext (pDelimiters);
			pArg3 = pStrVAR->GetTokenNext (pDelimiters);
			char *pArg4 = pStrVAR->GetTokenNext (pDelimiters);
			if ( !pArg2 || !pArg3 || !pArg4)
				return CHEAT_INVALID;
			return Cheat_item_Time_Limit( pArg1, pArg2, pArg3, pArg4 );
		}

		if ( !_strcmpi( pToken, "/ITEM_TIME_LIMIT_INFO" ) ) 
		{
			return Cheat_item_Time_Limit_Info( pArg1, 0, 0, 0 );
		}

		if ( !_strcmpi( pToken, "/ITEM_TIME_LIMIT_CHECK" ) ) 
		{
			return Cheat_item_Time_Limit_Check( pArg1, false, 0, 0 );
		}

		if ( !_strcmpi( pToken, "/ITEM_TIME_LIMIT_DELETE" ) ) 
		{
			return Cheat_item_Time_Limit_Check( pArg1, true, 0, 0 );
		}
#endif
//-------------------------------------

//-------------------------------------
// 2006.11.09/김대성/추가 - 아이템 나눠주기 기능 추가
#ifdef __CHEAT_GIVE_ITEM_
		if ( !_strcmpi( pToken, "/send1" ) )		// /send1 world 아이템번호 아이템개수
		{
			if( _strcmpi( pArg1, "world" ) )
				return CHEAT_INVALID;
			pArg2 = pStrVAR->GetTokenNext (pDelimiters);	// 아이템번호
			pArg3 = pStrVAR->GetTokenNext (pDelimiters);	// 아이템개수
			if ( !pArg2 || !pArg3 )
				return CHEAT_INVALID;
			char *pArg4 = pStrVAR->GetTokenNext (pDelimiters);
			return Cheat_send1( pArg1, pArg2, pArg3, pArg4 );
		}
		if ( !_strcmpi( pToken, "/send2" ) )		// /send2 존번호 아이템번호 아이템개수
		{
			pArg2 = pStrVAR->GetTokenNext (pDelimiters);	// 아이템번호
			pArg3 = pStrVAR->GetTokenNext (pDelimiters);	// 아이템개수
			if ( !pArg2 || !pArg3 )
				return CHEAT_INVALID;
			char *pArg4 = pStrVAR->GetTokenNext (pDelimiters);
			return Cheat_send2( pArg1, pArg2, pArg3, pArg4 );
		}
		if ( !_strcmpi( pToken, "/send3" ) )		// /send3 범위값 아이템번호 아이템개수
		{
			pArg2 = pStrVAR->GetTokenNext (pDelimiters);	// 아이템번호
			pArg3 = pStrVAR->GetTokenNext (pDelimiters);	// 아이템개수
			if ( !pArg2 || !pArg3 )
				return CHEAT_INVALID;
			char *pArg4 = pStrVAR->GetTokenNext (pDelimiters);
			return Cheat_send3( pArg1, pArg2, pArg3, pArg4 );
		}
#endif
//-------------------------------------

//-------------------------------------
// 2006.12.20/김대성/추가 - 몬스터사냥시 아이템 드랍확률 테스트
		if ( !_strcmpi( pToken, "/DROPTEST" ) ) {
			pArg2 = pStrVAR->GetTokenNext (pDelimiters);	// 거리
			if ( !pArg2 )
				return CHEAT_INVALID;
			this->Cheat_DropTest( pArg1, pArg2 );
			return CHEAT_NOLOG;
		}
//-------------------------------------


//-------------------------------------
// 2007.02.06/김대성/추가 - 선물상자 드랍확률 테스트
		if ( !_strcmpi( pToken, "/GIFTBOX_DROPTEST" ) ) {
			pArg2 = pStrVAR->GetTokenNext (pDelimiters);	// 횟수
			if ( !pArg2 )
				return CHEAT_INVALID;
			this->Cheat_GiftBoxDropTest( pArg1, pArg2 );
			return CHEAT_NOLOG;
		}
//-------------------------------------

		if ( !_strcmpi( pToken, "/GET" ) ) 
		{
			pArg2 = pStrVAR->GetTokenNext (pDelimiters);	// account
			return Cheat_get ( pStrVAR, pArg1, pArg2, szCode );
		}

		if ( !_strcmpi( pToken, "/rank" ) ) 
		{
			//	클랜 랭크 포인트 올림
			int	t_Point = atoi( pArg1 );
			return	SetClanRank(t_Point);
		}
/*
		if ( !_strcmpi( pToken, "/STR" ) ) {
			pArg2 = pStrVAR->GetTokenNext (pDelimiters);	// type
			if ( !_strcmpi( pArg1, "NPC" ) ) {
				;
			} else
			if ( !_strcmpi( pArg1, "ZONE" ) ) {
				;
			} else
			if ( !_strcmpi( pArg1, "QUEST" ) ) {
				;
			} else
			if ( !_strcmpi( pArg1, "AI" ) ) {
				;
			} else
			if ( !_strcmpi( pArg1, "ITEM" ) ) {
				;
			}
			return CHEAT_NOLOG;
		}
*/
} 
else 
{
		if ( !_strcmpi( pToken, "/마을" ) ) 
		{
			// 저장된 부활장소에서 살아나기..
			this->Recv_cli_REVIVE_REQ( REVIVE_TYPE_SAVE_POS );
			return CHEAT_NOLOG;
		}
	}

	if ( C_Cheater() ) 
	{
		if ( pArg1 ) 
		{
			if ( !_strcmpi( pToken, "/nc" ) ) 
			{
				// 서버 전체 공지
				g_pZoneLIST->Send_gsv_ANNOUNCE_CHAT( &szCode[4], this->Get_NAME() );
				return CHEAT_PROCED;
			} else if ( !_strcmpi( pToken, "/nz" ) ) 
			{		
				// 현재 맵 공지
				g_pZoneLIST->Send_gsv_ANNOUNCE_CHAT( this->GetZONE()->Get_ZoneNO(), &szCode[4], this->Get_NAME() );
				return CHEAT_PROCED;
			}

			// 몹 소환
			if ( !_strcmpi( pToken, "/mon" ) ) 
			{

#ifdef __LIMIT_CHEAT_MON
				return CHEAT_INVALID;		
#endif
				// pArg1 Mob IDX
				pArg2 = pStrVAR->GetTokenNext (pDelimiters);	// mob cnt
				if ( pArg2 ) {
					return Cheat_mon ( pArg1, pArg2 );
				}
			} 
			
			// 몹 삭제
			if ( !_strcmpi( pToken, "/damage" ) ) {
				// pArg1 Distance
				// pArg2 Damage
				// pArg3 DamageType
				pArg2 = pStrVAR->GetTokenNext (pDelimiters);
				if ( pArg2 ) {
					pArg3 = pStrVAR->GetTokenNext (pDelimiters);
					return Cheat_damage( pArg1, pArg2, pArg3 );
				}
			}
		} else {	// !pArg1
			if ( !_strcmpi( pToken, "/dead" ) ) {
				this->Set_HP( 10 );
				this->SetCMD_ATTACK( this->Get_INDEX() );
				return CHEAT_NOLOG;
			}
		}
	}

	if( C_Cheater() || TWGM_Cheater() )
	{
		if( pArg1 )
		{
			if ( !_strcmpi( pToken, "/na" ) ) {
			#ifdef	__INC_WORLD
				g_pZoneLIST->Send_gsv_ANNOUNCE_CHAT( &szCode[4], this->Get_NAME() );
			#else
				g_pSockLSV->Send_gsv_CHEAT_REQ( this, this->m_dwWSID, 0, szCode );
			#endif
			} else 
			// 타겟 케릭터 소환...
			if ( !_strcmpi( pToken, "/call" ) ) {
				if ( this->GetZONE() ) {
					pArg2 = pStrVAR->GetTokenNext (pDelimiters);	// account
					return Cheat_call ( pArg1, pArg2, szCode );
				}

				return CHEAT_NOLOG;
			} else
			if ( !_strcmpi( pToken, "/out" ) ) {
				// 강제 접속 종료...
				pArg2 = pStrVAR->GetTokenNext (pDelimiters);	// account
				return Cheat_out ( pArg1, pArg2, szCode );
			} else
			if ( !_strcmpi( pToken, "/shut" ) ) {
				// 말하기 금지...
				pArg2 = pStrVAR->GetTokenNext (pDelimiters);	// block time
				pArg3 = pStrVAR->GetTokenNext (pDelimiters);	// account
				return Cheat_shut( pArg1, pArg2, pArg3, szCode );
			}
		}
		else
		{
			// 무적 치트코드
			if ( !_strcmpi( pToken, "/invincible" ) && ( GM_Cheater () || TWGM_Cheater() )) {
				m_IngSTATUS.ToggleSubFLAG( FLAG_CHEAT_INVINCIBLE );
				this->Send_gsv_WHISPER( "SERVER", this->m_IngSTATUS.IsSubSET( FLAG_CHEAT_INVINCIBLE ) ? "Invincible Mode" : "Normal Mode" );
				return CHEAT_NOLOG;
			} 
		}
	}

	if( TWGM_Cheater() )
	{
		if( pArg1 )
		{
			if ( !_strcmpi( pToken, "/speed" ) ) {
				// 말하기 금지...
				return Cheat_speed( pArg1 );
			} else
			// 몹 소환
			if ( !_strcmpi( pToken, "/mon2" ) ) {
				// pArg1 Mob IDX
#ifdef __LIMIT_CHEAT_MON
				return CHEAT_INVALID;		
#endif
				char * pArg2 = pStrVAR->GetTokenNext (pDelimiters);	// X
				char * pArg3 = pStrVAR->GetTokenNext (pDelimiters);	// Y
				char * pArg4 = pStrVAR->GetTokenNext (pDelimiters);	// 마리수 , pArg1 : 몬스터 인덱스
				if ( pArg2 && pArg3 && pArg4 ) {
					return Cheat_mon2 ( pArg1, pArg2, pArg3, pArg4 );
				}
			} 
		}
	}
	
	if( C_Cheater() || TWGM_Cheater() )
	{
		if ( !_strcmpi( pToken, "/HIDE" ) ) {
			this->m_IngSTATUS.ToggleSubFLAG( FLAG_SUB_HIDE );
			DWORD dwFlag = this->m_IngSTATUS.m_dwSubStatusFLAG;
			Send_gsv_CHARSTATE_CHANGE( dwFlag );
			return CHEAT_NOLOG;
		} 
	}

	if ( pArg1 ) {
		// 퀘스트 체크...
		if ( !_strcmpi(pToken, "/QUEST" ) ) {
			if ( !_strcmpi( pArg1, "all" ) ) {
				g_QuestList.CheckAllQuest( this );
				return CHEAT_NOLOG;
			}
			return Cheat_quest ( pStrVAR, pArg1 );
		} 

		// 능력치 상승 치트코드...
		if ( !_strcmpi(pToken, "/FULL") && A_Cheater() ) {
			if ( !_strcmpi(pArg1, "HP") ) {
				this->Set_HP( this->Get_MaxHP() );
				nProcMODE = CHEAT_SEND;
			} else
			if ( !_strcmpi(pArg1, "MP") ) {
				this->Set_MP( this->Get_MaxMP() );
				nProcMODE = CHEAT_SEND;
			}
		} else
		if ( !_strcmpi( pToken, "/SET" ) ) 
		{
//-------------------------------------
/*
#ifdef __LIMIT_CHEAT_SET
			return CHEAT_INVALID;		
#endif
*/
// 2006.10.12/김대성/삭제 - 미국측의 요청으로 /set worldexp, /set worlddrop 는 막지 않음
//-------------------------------------

			pArg2 = pStrVAR->GetTokenNext (pDelimiters);
			if ( pArg2 ) {
				classUSER *pUSER=NULL;
				pArg3 = pStrVAR->GetTokenNext (pDelimiters);	// 대상.
				if ( pArg3 ) {
					pUSER = g_pUserLIST->Find_CHAR( pArg3 );
				}

//-------------------------------------
/*
				nProcMODE = Cheat_set( pUSER, pArg1, pArg2, pArg3 );
*/
// 2006.10.12/김대성/수정 - 미국측의 요청으로 /set worldexp, /set worlddrop 는 막지 않음
#ifdef __LIMIT_CHEAT_SET
	#if defined (__ALLOW_CHEAT_SET_WORLDEXP)
				if ( !_strcmpi( pArg1, "worldexp" ) ) 
					nProcMODE = Cheat_set( pUSER, pArg1, pArg2, pArg3 );
	#endif
	#if defined (__ALLOW_CHEAT_SET_WORLDDROP)
				if ( !_strcmpi( pArg1, "worlddrop" ) ) 
					nProcMODE = Cheat_set( pUSER, pArg1, pArg2, pArg3 );
	#endif
	#if !defined (__ALLOW_CHEAT_SET_WORLDEXP) && !defined (__ALLOW_CHEAT_SET_WORLDDROP)
				return CHEAT_INVALID;
	#endif
#else
				nProcMODE = Cheat_set( pUSER, pArg1, pArg2, pArg3 );
#endif
//-------------------------------------
			} else
				return CHEAT_INVALID;
		}
		if ( !_strcmpi( pToken, "/TOGGLE" ) ) {
			return Cheat_toggle( pStrVAR, pArg1 );
		}
		if ( !_strcmpi( pToken, "/kill_all" ) && A_Cheater() ) {
			this->GetZONE()->Kill_AllMOB ( this );
			return CHEAT_NOLOG;
		}
		if ( !_strcmpi( pToken, "/RESET" ) && A_Cheater()  ) {
			if ( !_strcmpi( pArg1, "QUEST" ) ) {
				pStrVAR->Printf( "Reset quest result: %d", g_QuestList.LoadQuestTable () );
				this->Send_gsv_WHISPER( "SERVER", pStrVAR->Get() );
			} else
			if ( !_strcmpi( pArg1, "REGEN" ) ) {
				this->GetZONE()->Reset_REGEN ();
				pStrVAR->Printf( "Zone %d Reset regen point", this->GetZONE()->Get_ZoneNO() );
				this->Send_gsv_WHISPER( "SERVER", pStrVAR->Get() );
			}
			return CHEAT_NOLOG;
		} else
		if ( !_strcmpi( pToken, "/REGEN" ) && B_Cheater() ) {
			pArg2 = pStrVAR->GetTokenNext (pDelimiters);
			return Cheat_regen ( pStrVAR, pArg1, pArg2 );
		} else
		if ( !_strcmpi( pToken, "/HIDE" ) && C_Cheater() ) {
			this->m_IngSTATUS.ToggleSubFLAG( FLAG_SUB_HIDE );
			return CHEAT_NOLOG;
		}
	} else {
		if ( !_strcmpi( pToken, "/워프" ) ) {
			// 현재 존의 부활장소에서..
			this->Recv_cli_REVIVE_REQ( REVIVE_TYPE_REVIVE_POS );
			return CHEAT_NOLOG;
		} else 
		if ( !_strcmpi( pToken, "/부활" ) ) {
			// 현재 존의 부활장소에서..
			this->Set_HP( this->GetCur_MaxHP() );
			this->Recv_cli_REVIVE_REQ( REVIVE_TYPE_CURRENT_POS );
			return CHEAT_NOLOG;
		} else
//-------------------------------------
// 2006.10.25/김대성/추가 - 영문권은 한글을 사용하지 못하기 때문에...
		if ( !_strcmpi( pToken, "/REVIVE" ) ) {
			// 현재 존의 부활장소에서..
			this->Set_HP( this->GetCur_MaxHP() );
			this->Recv_cli_REVIVE_REQ( REVIVE_TYPE_CURRENT_POS );
			return CHEAT_NOLOG;
		} else
//-------------------------------------
//-------------------------------------
// 2006.11.14/김대성/추가 - 시간대별 이벤트를 위한 서버시간 알려주기
if ( !_strcmpi( pToken, "/SERVERTIME" ) ) {
	this->Cheat_ServerTime();
	return CHEAT_NOLOG;
} else
//-------------------------------------
		if ( !_strcmpi( pToken, "/SAVE" ) ) {
			g_pThreadSQL->Add_BackUpUSER( this );
			return CHEAT_NOLOG;
		}
	}

	if ( CHEAT_SEND == nProcMODE ) {
        classPACKET *pCPacket = Packet_AllocNLock ();
		//////////////////////////////////////////////////////////////////////////
		// 2006.06.12:이성활:pCPacket에 대한 Null 체크
		if (pCPacket) {
	        pCPacket->m_HEADER.m_wType = GSV_CHEAT_CODE;
		    pCPacket->m_HEADER.m_nSize = sizeof( gsv_CHEAT_CODE );
			pCPacket->m_gsv_CHEAT_CODE.m_wObjectIDX = this->Get_INDEX();
			pCPacket->AppendString( szCode );

			this->SendPacket( pCPacket );

			Packet_ReleaseNUnlock( pCPacket );
		}
		//////////////////////////////////////////////////////////////////////////
		
		return CHEAT_SEND;
	}

	return CHEAT_INVALID;
}

short classUSER::Check_CheatCODE (char *szCode)
{
/*
	if ( this->GetPARTY() && this == this->GetPARTY()->GetPartyOWNER() ) {
		if ( !strncmp( szCode, "/pexp", 5 ) ) {
			if ( this->GetPARTY()->m_btPartyRULE & BIT_PARTY_RULE_EXP_PER_PLAYER ) {
				this->GetPARTY()->m_btPartyRULE &= ~BIT_PARTY_RULE_EXP_PER_PLAYER;
				this->GetPARTY()->SendWhisperToPartyMembers( "사냥시 얻는 경험치가 일정하게 분배됩니다." );
			} else {
				this->GetPARTY()->m_btPartyRULE |= BIT_PARTY_RULE_EXP_PER_PLAYER;
				this->GetPARTY()->SendWhisperToPartyMembers( "사냥시 얻는 경험치가 레벨비례로 분배됩니다." );
			}
			return 1;
		} else
		if ( !strncmp( szCode, "/pitem", 6 ) ) {
			if ( this->GetPARTY()->m_btPartyRULE & BIT_PARTY_RULE_ITEM_TO_ORDER ) {
				this->GetPARTY()->m_btPartyRULE &= ~BIT_PARTY_RULE_ITEM_TO_ORDER;
				this->GetPARTY()->SendWhisperToPartyMembers( "파티소유의 아이템 우선권이 모든 파티원에게 있습니다." );
			} else {
				this->GetPARTY()->m_btPartyRULE |= BIT_PARTY_RULE_ITEM_TO_ORDER;
				this->GetPARTY()->SendWhisperToPartyMembers( "파티소유의 아이템 습득시 순차적으로 자동 분배됩니다." );
			}
			return 1;
		}
	}
	if ( this->m_dwRIGHT < CHEAT_GM || !this->GetZONE() ) {
		return 0;
	}
*/

//-------------------------------------
// 2006.06.16/김대성/추가 - 미국의 요청으로 '/' 로 시작하는 모든 메시지 로그 남기기
#ifdef __CHAT_CHEAT_LOG_ALL
	g_pThreadLOG->When_CheatCODE( this, szCode );
#endif
//-------------------------------------

	if ( 0 == C_Cheater() && 0 == TWGM_Cheater() )
		return 0; 

	if ( !strncmp( szCode, "/clan ", 5 ) ) {
		this->GuildCMD( &szCode[5] );
		return 1;
	} else
	if ( !strncmp( szCode, "/guild ", 7 ) ) {
		this->GuildCMD( &szCode[7] );
		return 1;
	}



	short nRet = this->Parse_CheatCODE( szCode );
	if ( nRet > CHEAT_NOLOG ) {
		g_pThreadLOG->When_CheatCODE( this, szCode );
	}
	return nRet;
}


//-------------------------------------------------------------------------------------------------
/// 채팅으로 받은 클랜 명령 처리..
short classUSER::GuildCMD (char *szCMD)
{
	char *pArg1, *pArg2, *pArg3;
	char *pDelimiters = " ";

	CStrVAR *pStrVAR = this->GetZONE()->GetStrVAR ();

	pArg1 = pStrVAR->GetTokenFirst (szCMD, pDelimiters);
	if ( NULL == pArg1 )
		return 0;

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return 0;

	if ( !_strcmpi(pArg1, "create") ) {	//길드생성, /guildcreate [길드명]
		pArg2 = pStrVAR->GetTokenNext (pDelimiters);
		pArg3 = pStrVAR->GetTokenNext (pDelimiters);
		if ( pArg2 && pArg3 ) {
			pCPacket->m_HEADER.m_wType = CLI_CLAN_COMMAND;
			pCPacket->m_HEADER.m_nSize = sizeof( cli_CLAN_CREATE );
			pCPacket->m_cli_CLAN_CREATE.m_btCMD	= GCMD_CREATE;

			pCPacket->m_cli_CLAN_CREATE.m_wMarkIDX[ 0 ] = 1;
			pCPacket->m_cli_CLAN_CREATE.m_wMarkIDX[ 1 ] = 2;

			pCPacket->AppendString( pArg2 );
			pCPacket->AppendString( pArg3 );
			this->Recv_cli_CLAN_COMMAND( (t_PACKET*)( pCPacket->m_pDATA ) );
		}
	} else
	if ( !_strcmpi(pArg1, "info") ) {	//길드정보, /ginfo - 길드에 대한 기본적인 정보
		pCPacket->m_HEADER.m_wType = CLI_CLAN_COMMAND;
		pCPacket->m_HEADER.m_nSize = sizeof( cli_CLAN_COMMAND );
		pCPacket->m_cli_CLAN_COMMAND.m_btCMD	= GCMD_INFO;

		this->Recv_cli_CLAN_COMMAND( (t_PACKET*)( pCPacket->m_pDATA ) );
	} else 
	if ( !_strcmpi(pArg1, "invite") ) {	//길드초대, /ginvite <플레이어> - 길드에 해당 플레이어 초대하기
		pArg2 = pStrVAR->GetTokenNext (pDelimiters);
		if ( pArg2 ) {
			pCPacket->m_HEADER.m_wType = CLI_CLAN_COMMAND;
			pCPacket->m_HEADER.m_nSize = sizeof( cli_CLAN_COMMAND );
			pCPacket->m_cli_CLAN_COMMAND.m_btCMD	= GCMD_INVITE;

			pCPacket->AppendString( pArg2 );
			this->Recv_cli_CLAN_COMMAND( (t_PACKET*)( pCPacket->m_pDATA ) );
		}
	} else
	if ( !_strcmpi(pArg1, "remove") ) {	//길드추방, /gremove <플레이어> - 길드에서 해당 플레이어 추방하기
		pArg2 = pStrVAR->GetTokenNext (pDelimiters);
		if ( pArg2 ) {
			pCPacket->m_HEADER.m_wType = CLI_CLAN_COMMAND;
			pCPacket->m_HEADER.m_nSize = sizeof( cli_CLAN_COMMAND );
			pCPacket->m_cli_CLAN_COMMAND.m_btCMD	= GCMD_REMOVE;

			pCPacket->AppendString( pArg2 );
			this->Recv_cli_CLAN_COMMAND( (t_PACKET*)( pCPacket->m_pDATA ) );
		}
	} else
	if ( !_strcmpi(pArg1, "promote") ) {	//길드승급, /gpromote <플레이어> - 해당 플레이어 길드 등급 올리기
		pArg2 = pStrVAR->GetTokenNext (pDelimiters);
		if ( pArg2 ) {
			pCPacket->m_HEADER.m_wType = CLI_CLAN_COMMAND;
			pCPacket->m_HEADER.m_nSize = sizeof( cli_CLAN_COMMAND );
			pCPacket->m_cli_CLAN_COMMAND.m_btCMD	= GCMD_PROMOTE;

			pCPacket->AppendString( pArg2 );
			this->Recv_cli_CLAN_COMMAND( (t_PACKET*)( pCPacket->m_pDATA ) );
		}
	} else 
	if ( !_strcmpi(pArg1, "demote") ) {	//길드강등, /gdemote <플레이어> - 해당 플레이어 길드 등급 내리기
		pArg2 = pStrVAR->GetTokenNext (pDelimiters);
		if ( pArg2 ) {
			pCPacket->m_HEADER.m_wType = CLI_CLAN_COMMAND;
			pCPacket->m_HEADER.m_nSize = sizeof( cli_CLAN_COMMAND );
			pCPacket->m_cli_CLAN_COMMAND.m_btCMD	= GCMD_DEMOTE;

			pCPacket->AppendString( pArg2 );
			this->Recv_cli_CLAN_COMMAND( (t_PACKET*)( pCPacket->m_pDATA ) );
		}
	} else 
	if ( !_strcmpi(pArg1, "slogan") ) {
		pArg2 = pStrVAR->GetTokenNext (pDelimiters);
		if ( pArg2 ) {
			pCPacket->m_HEADER.m_wType = CLI_CLAN_COMMAND;
			pCPacket->m_HEADER.m_nSize = sizeof( cli_CLAN_COMMAND );
			pCPacket->m_cli_CLAN_COMMAND.m_btCMD = GCMD_SLOGAN;

			pCPacket->AppendString( pArg2 );
			this->Recv_cli_CLAN_COMMAND( (t_PACKET*)( pCPacket->m_pDATA ) );
		}
	} else 
	if ( !_strcmpi(pArg1, "motd") ) {	//길드공지, /gmotd <할말> - 오늘의 길드 메시지 정하기
		pArg2 = pStrVAR->GetTokenNext (pDelimiters);
		if ( pArg2 ) {
			pCPacket->m_HEADER.m_wType = CLI_CLAN_COMMAND;
			pCPacket->m_HEADER.m_nSize = sizeof( cli_CLAN_COMMAND );
			pCPacket->m_cli_CLAN_COMMAND.m_btCMD	= GCMD_MOTD;

			pCPacket->AppendString( pArg2 );
			this->Recv_cli_CLAN_COMMAND( (t_PACKET*)( pCPacket->m_pDATA ) );
		}
	} else 
	if ( !_strcmpi(pArg1, "quit") ) {	//길드탈퇴, /gquit - 길드에서 탈퇴하기
		pCPacket->m_HEADER.m_wType = CLI_CLAN_COMMAND;
		pCPacket->m_HEADER.m_nSize = sizeof( cli_CLAN_COMMAND );
		pCPacket->m_cli_CLAN_COMMAND.m_btCMD	= GCMD_QUIT;
		this->Recv_cli_CLAN_COMMAND( (t_PACKET*)( pCPacket->m_pDATA ) );
	} else 
	if ( !_strcmpi(pArg1, "roster") ) {	//길드목록, /groster - 전체 길드원 목록 보기
		pCPacket->m_HEADER.m_wType = CLI_CLAN_COMMAND;
		pCPacket->m_HEADER.m_nSize = sizeof( cli_CLAN_COMMAND );
		pCPacket->m_cli_CLAN_COMMAND.m_btCMD	= GCMD_ROSTER;

		this->Recv_cli_CLAN_COMMAND( (t_PACKET*)( pCPacket->m_pDATA ) );
	} else 
	if ( !_strcmpi(pArg1, "leader") ) {	//길드위임, /gleader <플레이어> - 다른 플레이어에게 길드장 위임하기 (길드장 전용)
		pArg2 = pStrVAR->GetTokenNext (pDelimiters);
		if ( pArg2 ) {
			pCPacket->m_HEADER.m_wType = CLI_CLAN_COMMAND;
			pCPacket->m_HEADER.m_nSize = sizeof( cli_CLAN_COMMAND );
			pCPacket->m_cli_CLAN_COMMAND.m_btCMD	= GCMD_LEADER;

			pCPacket->AppendString( pArg2 );
			this->Recv_cli_CLAN_COMMAND( (t_PACKET*)( pCPacket->m_pDATA ) );
		}
	} else 
	if ( !_strcmpi(pArg1, "disband") ) {	//길드해체, /gdisband - 길드 해체하기 (길드장 전용) 
		pCPacket->m_HEADER.m_wType = CLI_CLAN_COMMAND;
		pCPacket->m_HEADER.m_nSize = sizeof( cli_CLAN_COMMAND );
		pCPacket->m_cli_CLAN_COMMAND.m_btCMD	= GCMD_DISBAND;
		this->Recv_cli_CLAN_COMMAND( (t_PACKET*)( pCPacket->m_pDATA ) );
	} else
	if ( !_strcmpi(pArg1, "addskill") ) {
		pArg2 = pStrVAR->GetTokenNext (pDelimiters);
		if ( pArg2 )
			this->AddClanSKILL( atoi(pArg2) );
	} else
	if ( !_strcmpi(pArg1, "delskill") ) {
		pArg2 = pStrVAR->GetTokenNext (pDelimiters);
		if ( pArg2 )
			this->DelClanSKILL( atoi(pArg2) );
	}


	if ( this->A_Cheater() ) {
// #ifdef	__INC_WORLD
		if ( !_strcmpi(pArg1, "score") ) {
			pArg2 = pStrVAR->GetTokenNext (pDelimiters);
			if ( pArg2 )
				this->AddClanSCORE( atoi(pArg2) );
		} else
		if ( !_strcmpi(pArg1, "money") ) {
			pArg2 = pStrVAR->GetTokenNext (pDelimiters);
			if ( pArg2 )
				this->AddClanMONEY( atoi(pArg2) );
		} else
		if ( !_strcmpi(pArg1, "level") ) {
			this->IncClanLEVEL ();
		}
// #endif
	}

	Packet_ReleaseNUnlock( pCPacket );
	return 0;
}


//-------------------------------------------------------------------------------------------------
/// 인벤토리가 꽉차서 더이상 이이템을 소유할수 없는 상태에서 부득이 퀘스트 보상등에서 아이템은 바닥에
/// 떨구고 소유시간을 허벌나게 길게...
void classUSER::Save_ItemToFILED (tagITEM &sDropITEM, int iRemainTime)
{
	switch( sDropITEM.GetTYPE() ) {
		case ITEM_TYPE_QUEST :
		case ITEM_TYPE_MONEY :
			// 돈을 더이상 소지하지 못해 드롭되는 경우는 없음.
			return;
	}

	CObjITEM *pObjITEM = new CObjITEM;
	if ( pObjITEM ) {
		tPOINTF PosSET;
		PosSET.x = this->m_PosCUR.x + RANDOM( 201 ) - 100;
		PosSET.y = this->m_PosCUR.y + RANDOM( 201 ) - 100;

		pObjITEM->InitItemOBJ( this, PosSET, this->m_PosSECTOR, sDropITEM, this, true, NULL );		// 사용자 드롭.
		pObjITEM->m_iRemainTIME = iRemainTime;	// 30분간 유효 하도록...

		g_pThreadLOG->When_ObjItemLOG( LIA_DROP, this, pObjITEM );
		this->GetZONE()->Add_DIRECT( pObjITEM );		// 드롭 아이템
	}
}

//-------------------------------------------------------------------------------------------------
/// 경험치 증가시킴
void classUSER::Add_EXP (int iGetExp, bool bApplyStamina, WORD wFromObjIDX)
{	
	if ( this->Get_HP() <= 0 )
		return;

	int iExp;
	if ( bApplyStamina ) {
		iExp = iGetExp;
		iGetExp= (int)( ( (iExp + 100) / (this->Get_LEVEL()+6) ) * ( ::Get_WorldSTAMINA() ) / 80.f );
		short nNewStamina = this->GetCur_STAMINA() + iGetExp;
		this->SetCur_STAMINA( nNewStamina>=MAX_STAMINA? MAX_STAMINA : nNewStamina );

		//if ( this->GetCur_STAMINA() >= GREEN_STAMINA ) {
		//	iExp = iGetExp;

		//	// 소모될 스테미나 :: {(획득 경험치 + 80) / (A_LV+5) } * (WORLD_STAMINA) / 100
		//	iGetExp= (int)( ( (iExp + 80) / (this->Get_LEVEL()+5) ) * ( ::Get_WorldSTAMINA() ) / 100.f );
		//	if ( iGetExp > 0 ) {
		//		short nNewStamina = this->GetCur_STAMINA() - iGetExp;
		//		this->SetCur_STAMINA( nNewStamina>0 ? nNewStamina : 0 );
		//	}
		//} else
		//if ( this->GetCur_STAMINA() >= YELLOW_STAMINA ) {
		//	iExp = (int) ( iGetExp * this->GetCur_STAMINA()/10000.f );
		//	if ( iExp <= 0 ) return;

		//	iGetExp= (int)( ( (iExp + 80) / (this->Get_LEVEL()+5) ) * ( ::Get_WorldSTAMINA() ) / 100.f );
		//	// 소모될 스테미나 :: {(획득 경험치 + 80) / (A_LV+5) } * (WORLD_STAMINA) / 100
		//	iGetExp= (int)( ( (iExp + 80) / (this->Get_LEVEL()+5) ) * ( ::Get_WorldSTAMINA() ) / 100.f );
		//	if ( iGetExp > 0 ) {
		//		short nNewStamina = this->GetCur_STAMINA() - iGetExp;
		//		if ( nNewStamina < YELLOW_STAMINA && this->GetPARTY() ) {
		//			// 파티원에게 스테미너 정보 전송.
		//			this->m_pPartyBUFF->Change_ObjectIDX( this );
		//		}
		//		this->SetCur_STAMINA( nNewStamina>0 ? nNewStamina : 0 );
		//	}
		//} else {
		//	iExp = (int) ( iGetExp * 0.3f );
		//	if ( iExp <= 0 ) return;

		//	// 소모될 스테미나 :: {(획득 경험치 + 80) / (A_LV+5) } * (WORLD_STAMINA) / 100
		//	iGetExp= (int)( ( (iExp + 80) / (this->Get_LEVEL()+5) ) * ( ::Get_WorldSTAMINA() ) / 100.f );
		//	if ( iGetExp > 0 ) {
		//		short nNewStamina = this->GetCur_STAMINA() - iGetExp;
		//		this->SetCur_STAMINA( nNewStamina>0 ? nNewStamina : 0 );
		//	}
		//}
	} else {
		iExp = iGetExp;
	}

	// 레벨업에 필요한 경험치값을 구해온다..
	int  iNeedEXP = this->Get_NeedEXP( this->Get_LEVEL() );     

	if( this->Get_LEVEL()>MAX_LEVEL)
	{
		return;
	}
	// 최대레벨이고. 경험치도 최대이면. 리던..
	else if( this->Get_LEVEL()==MAX_LEVEL&&   this->GetCur_EXP()>=iNeedEXP)
	{
		//this->SetCur_EXP(iNeedEXP);
		return;
	}

	if ( m_GrowAbility.m_lEXP + iExp > MAX_EXPERIENCE ) {
		m_GrowAbility.m_lEXP = this->Get_NeedEXP( MAX_LEVEL );
	} else
		m_GrowAbility.m_lEXP += iExp;

	bool bLevelUp=false;

	short nBeforeLEV = this->Get_LEVEL();
	while ( m_GrowAbility.m_lEXP >= iNeedEXP )
	{
		// 레벨 제한 
		if ( this->Get_LEVEL() < MAX_LEVEL )
		{
			this->Set_LEVEL( this->Get_LEVEL() + 1 );
		}
		else      
		{
			//최대레벨이고. 경험치가 요구경험치보다 크면. 요구 경험치로 셋팅해준다..
				this->SetCur_EXP(iNeedEXP);
				this->Send_gsv_SETEXP( wFromObjIDX );
				return;
		}
		
		this->AddCur_BonusPOINT( (short)( this->Get_LEVEL() * 0.8 ) + 10 );

		if ( IsTAIWAN() ) {
			for (short nD=0; nD<g_TblSkillPoint.m_nDataCnt ; nD++) {
				if ( SP_LEVEL(nD) == this->Get_LEVEL() ) {
					this->AddCur_SkillPOINT( SP_POINT(nD) );
					break;
				}
			}
		} else {
			// 스킬 포인트 =  (LV + 4) * 0.5 - 1 
			this->AddCur_SkillPOINT( (short)( ( this->Get_LEVEL() + 4 ) * 0.5f ) - 1 );
		}
		m_GrowAbility.m_lEXP -= iNeedEXP;

		m_GrowAbility.m_lPenalEXP = 0;
		iNeedEXP = this->Get_NeedEXP( this->Get_LEVEL() );
		bLevelUp = true;
	}

	if ( bLevelUp ) {
		g_pThreadLOG->When_LevelUP( this, iExp );
		this->Send_gsv_LEVELUP( this->Get_LEVEL()-nBeforeLEV );
		if ( g_pUserLIST->Get_LevelUpTRIGGER( this->Get_LEVEL() ) )
			this->Do_QuestTRIGGER( g_pUserLIST->Get_LevelUpTRIGGER( this->Get_LEVEL() ) );
	} else
		this->Send_gsv_SETEXP( wFromObjIDX );
}

/**
 * \brief	죽었다...
 * \param	pKiller	:: 죽인넘...
 */
bool classUSER::Dead (CObjCHAR *pKiller)
{
	if ( this->GetZONE()->GetGlobalFLAG() & ZONE_FLAG_PK_ALLOWED ) {
		// 현재 존이 PK 설정되어 있으면...
		CObjCHAR *pKillerOBJ = (CObjCHAR*)( pKiller->Get_CALLER() );

		if ( pKillerOBJ && pKillerOBJ->IsUSER() ) {
			//CStrVAR *pCStr = this->GetZONE()->GetStrVAR ();
			//if ( 0 == Get_ServerLangTYPE() ) {
			//	pCStr->Printf ( "%s님께서 %s님을 기절 시켰습니다.", pKillerOBJ->Get_NAME(), this->Get_NAME() );
			//	g_pZoneLIST->Send_gsv_ANNOUNCE_CHAT( this->GetZONE()->Get_ZoneNO(), pCStr->Get(), NULL );
			//}

			if ( this->GetZONE()->Get_HashKillTRIGGER() )
				((classUSER*)pKillerOBJ)->Do_QuestTRIGGER( this->GetZONE()->Get_HashKillTRIGGER() );

			if ( this->GetZONE()->Get_HashDeadTRIGGER() )
				this->Do_QuestTRIGGER( this->GetZONE()->Get_HashDeadTRIGGER() );
		}
	}

	// 사용자가 죽을때 남기는 로그...
	g_pThreadLOG->When_DieBY( pKiller, (classUSER*)this );

	if ( this->m_iLinkedCartObjIDX ) {
		classUSER *pUSER = g_pObjMGR->Get_UserOBJ( this->m_iLinkedCartObjIDX );
		if ( pUSER )
		{
			if ( RIDE_MODE_GUEST == pUSER->m_btRideMODE )
			{
				// 뒤에 타고 있던 사용자면 운전자가 죽었으니 내려야지...
				pUSER->m_btRideMODE = 0;
				pUSER->m_btRideATTR = RIDE_ATTR_NORMAL;
			}
			pUSER->m_iLinkedCartObjIDX = 0;
			pUSER->m_iLinkedCartUsrIDX = 0;
		}
	}

	this->m_iLinkedCartObjIDX = 0;
	this->m_iLinkedCartUsrIDX =0;

	this->m_btRideATTR = RIDE_ATTR_NORMAL;
	this->m_btRideMODE = 0;
	this->m_STORE.m_bActive = false;

	this->m_btTradeBIT = 0;
	this->m_iTradeUserIDX = 0;

	this->Del_ActiveSKILL();
	this->m_IngSTATUS.Reset( false );	// 2005. 03. 30 상점상태 사망시 상점플래그 안풀리던 버그 수정 :: this->m_IngSTATUS.ClearALL();

	this->Clear_SummonCNT ();			// 죽을때...자신이 소환시킨 갯수 0개로...

	this->Update_SPEED ();				// 상태때문에 적용됐던 이동속도 

	this->m_iAppliedPenaltyEXP = 0;
	if ( CObjCHAR::Dead(NULL) ) {
		// 소환된 몹이 아니면...
		if ( !pKiller->IsUSER() && !pKiller->GetSummonedSkillIDX() /* 0 == ZONE_PVP_STATE( this->m_nZoneNO ) */ ) {
			// 경험치 패널치 적용.
			this->Set_PenalEXP( PENALTY_EXP_TOWN );
		}
		return true;
	}

	if ( this->IsUSER() )
	{
		this->m_IngSTATUS.ClearSubFLAG( FLAG_SUB_ARUA_FAIRY );
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
/// 셀프 스킬 실행
bool classUSER::Do_SelfSKILL (short nSkillIDX)
{
	if ( !IsTAIWAN() ) {//8*
		if ( this->GetCur_RIDE_MODE() ) {
			short nSkillType = SKILL_TYPE( nSkillIDX );
			if ( nSkillType >= SKILL_TYPE_03 && nSkillType <= SKILL_TYPE_07 ) 
				return false;

			if ( nSkillType == SKILL_TYPE_17 )
				return false;
		}
	}

	if ( this->Skill_ActionCondition( nSkillIDX ) ) {
		// 실제 필요 수치 소모 적용...
		if ( this->SetCMD_Skill2SELF( nSkillIDX ) ) {
			// this->Skill_UseAbilityValue( nSkillIDX );

//-------------------------------------
//2006.06.14/김대성/추가
			this->UpdateAbility ();		// 캐릭터 능력치 갱신
			Send_CHAR_HPMP_INFO(0);		// HP, MP정보를 한번 더 보내준다.
//-------------------------------------

			return true;
		}
	}

	return false;	// 접속 끊기지 않음
}
/// 타겟 스킬 실행
bool classUSER::Do_TargetSKILL (int iTargetObject, short nSkillIDX)
{
	if ( !IsTAIWAN() ) {
		if ( this->GetCur_RIDE_MODE() ) {
			short nSkillType = SKILL_TYPE( nSkillIDX );
			if ( nSkillType >= SKILL_TYPE_03 && nSkillType <= SKILL_TYPE_07 ) 
				return false;

			if ( nSkillType == SKILL_TYPE_17 )
				return false;
		}
	}

	// 리져렉션 스킬로 인해 HP체크 안하게...
	CObjCHAR *pDestCHAR = g_pObjMGR->Get_ClientCharOBJ( iTargetObject, false /* true */ );
	if ( pDestCHAR ) {
		if ( !this->Skill_IsPassFilter( pDestCHAR, nSkillIDX ) )
			return false;

		if ( this->Skill_ActionCondition( nSkillIDX ) ) {
			if ( this->SetCMD_Skill2OBJ ( iTargetObject, nSkillIDX ) ) {
				// 스킬 성공하면...실제 필요 수치 소모 적용...
				// this->Skill_UseAbilityValue( nSkillIDX );
				
//-------------------------------------
//2006.06.14/김대성/추가
				this->UpdateAbility ();		// 캐릭터 능력치 갱신
				Send_CHAR_HPMP_INFO(0);		// HP, MP정보를 한번 더 보내준다.
//-------------------------------------

				return true;
			}
		}
	}

	return false;	// 접속 끊기지 않음
}

//-------------------------------------------------------------------------------------------------
/// 사용자가 로그아웃 요청시에 클라이언트에 대기 해야할 시간 통보
bool classUSER::Send_gsv_LOGOUT_REPLY( WORD wWaitSec )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_LOGOUT_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_LOGOUT_REPLY );
	pCPacket->m_gsv_LOGOUT_REPLY.m_wWaitSec = wWaitSec;

	this->Send_Start( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

//-------------------------------------------------------------------------------------------------
/// 사용자에게 npc에서 발동할수 있는 이벤트를 수행하라고 통보
bool classUSER::Send_gsv_CHECK_NPC_EVENT( short nNpcIDX )
{
	if ( NPC_QUEST_TYPE(nNpcIDX) ) {
		// 파티 전용 퀘스트다...
		if ( this->GetPARTY() ) {
			return this->m_pPartyBUFF->Send_gsv_CHECK_NPC_EVENT( this, nNpcIDX );
		}
		return true;
	}
	
	m_iLastEventNpcIDX = nNpcIDX;
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_CHECK_NPC_EVENT;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_CHECK_NPC_EVENT );

	pCPacket->m_gsv_CHECK_NPC_EVENT.m_nNpcIDX = nNpcIDX;
	this->Send_Start( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 사용자에게 서버의 현재 피/엠을 통보
/// Updates party user HP and MP
bool classUSER::Send_gsv_SET_HPnMP (BYTE btApply)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_SET_HPnMP;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_HPnMP );

	pCPacket->m_gsv_SET_HPnMP.m_wObjectIDX = this->Get_INDEX();
	
	pCPacket->m_gsv_SET_HPnMP.m_nHP = ( btApply & 0x01 ) ? this->Get_HP() : -1;
	pCPacket->m_gsv_SET_HPnMP.m_nMP = ( btApply & 0x02 ) ? this->Get_MP() : -1;

	this->SendPacketToPARTY( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 채팅룸 패킷~
bool classUSER::Send_wsv_CHATROOM (BYTE btCMD, WORD wUserID, char *szSTR)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = WSV_CHATROOM;
	pCPacket->m_tag_CHAT_HEADER.m_btCMD = btCMD;

	if ( wUserID ) {
		pCPacket->m_HEADER.m_nSize = sizeof( wsv_CHAT_ROOM_USER );
		pCPacket->m_wsv_CHAT_ROOM_USER.m_wUserID  = wUserID;
	} else
		pCPacket->m_HEADER.m_nSize = sizeof( tag_CHAT_HEADER );

	if ( szSTR )
		pCPacket->AppendString( szSTR );

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

/// 자신에게 소모 아이템을 사용됐고 소모된 아이템의 갯수를 통보
bool classUSER::Send_gsv_USE_ITEM( short nItemNO, short nInvIDX )
{
	// 자신 한테만 소모성 아이템 사용했다고 전송.
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_USE_ITEM;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_USE_ITEM ) + sizeof( BYTE );
	pCPacket->m_gsv_USE_ITEM.m_wObjectIDX = this->Get_INDEX();
	pCPacket->m_gsv_USE_ITEM.m_nUseItemNO = nItemNO;
	pCPacket->m_gsv_USE_ITEM.m_btInvIDX[ 0 ] = (BYTE)nInvIDX;

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

/// 소모 아이템 사용을 주변에 알려주고 아이템 처리
bool classUSER::Use_pITEM( tagITEM *pITEM )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_USE_ITEM;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_USE_ITEM );
	pCPacket->m_gsv_USE_ITEM.m_wObjectIDX = this->Get_INDEX();
	pCPacket->m_gsv_USE_ITEM.m_nUseItemNO = pITEM->m_nItemNo;

	this->GetZONE()->SendPacketToSectors( this, pCPacket );
	// 파티원 한테도 ???
	// this->SendPacketToPartyExecpNearUSER( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

	// 지속성 효과 아이템인가 ??
	if ( USEITME_STATUS_STB( pITEM->m_nItemNo ) ) {
		short nIngSTB = USEITME_STATUS_STB( pITEM->m_nItemNo );
		// short nDuringTime;
		for (short nI=0; nI<STATE_APPLY_ABILITY_CNT; nI++) {
			if ( !STATE_APPLY_ABILITY_VALUE( nIngSTB, nI ) )
				continue;
			
			if ( this->m_IngSTATUS.IsEnableApplayITEM( STATE_APPLY_ING_STB( nIngSTB, nI ) ) ) {
				this->m_IngSTATUS.UpdateIngPOTION(	STATE_APPLY_ING_STB( nIngSTB, nI ), 
													USEITEM_ADD_DATA_VALUE( pITEM->m_nItemNo ),
													STATE_APPLY_ABILITY_VALUE( nIngSTB, nI ) );
			}
		}
	} else {
		// this->Use_ITEM( pITEM->m_nItemNo );
		this->Add_AbilityValue( USEITEM_ADD_DATA_TYPE( pITEM->m_nItemNo ), USEITEM_ADD_DATA_VALUE( pITEM->m_nItemNo ) );
	}

	return true;
}

/// 소모아이템 사용 패킷 처리 :: 사용조건등 따짐
bool classUSER::Use_InventoryITEM( t_PACKET *pPacket )
{
	short nInventoryIDX = pPacket->m_cli_USE_ITEM.m_nInventoryIndex;

	tagITEM *pITEM = &this->m_Inventory.m_ItemLIST[ nInventoryIDX ];
	if ( pITEM->GetQuantity() < 1 ) {
		return true;
	}

	if ( pITEM->GetTYPE() != ITEM_TYPE_USE ) {
		return IS_HACKING( this, "Use_InventoryITEM-1 :: pITEM->GetTYPE() != ITEM_TYPE_USE" );
	}

	if ( pITEM->m_nItemNo >= g_TblUSEITEM.m_nDataCnt ) {
		return IS_HACKING( this, "Recv_cli_USE_ITEM-2 : pITEM->m_nItemNo >= g_TblUSEITEM.m_nDataCnt" );
	}

	/// 쿨타임 적용..
	DWORD dwCurTime;
	short nCoolTimeType;
	if ( IsTAIWAN() ) 
	{
		dwCurTime = this->GetCurAbsSEC();
		nCoolTimeType = USEITEM_COOLTIME_TYPE(pITEM->m_nItemNo);
		if ( nCoolTimeType ) 
		{
			if ( dwCurTime - this->m_dwCoolTIME[ nCoolTimeType ] <= static_cast<DWORD>( USEITEM_COOLTIME_DELAY(pITEM->m_nItemNo) ) ) 
			{
			#ifdef	__INC_WORLD
				char szTmp[ MAX_PATH ];
				sprintf( szTmp, "ignore use item:: CurTime:%d, LastTime:%d, Reamin:%dsec", 
								dwCurTime, 	this->m_dwCoolTIME[ nCoolTimeType ], dwCurTime-this->m_dwCoolTIME[ nCoolTimeType ] );
				this->Send_gsv_WHISPER( "Cool time", szTmp );			// xxxx
			#endif
				return true;
			}
		}
	}
	else
	{
		dwCurTime = 0;
		nCoolTimeType = 0;
	}

	/// 사용 조건 체크...
	int iValue = GetCur_AbilityValue( USEITEM_NEED_DATA_TYPE(pITEM->m_nItemNo) );
	if ( AT_CURRENT_PLANET == USEITEM_NEED_DATA_TYPE(pITEM->m_nItemNo) ) 
	{
		// 사용할수 있는 행성을 체크하는 것인가 ????
		if ( iValue != USEITEM_NEED_DATA_VALUE(pITEM->m_nItemNo) )
			return true;
	}
	if ( iValue < USEITEM_NEED_DATA_VALUE(pITEM->m_nItemNo) ) 
		return true;


//	DWORD dwClearedSTATUS = 0;
	if ( USE_ITEM_SKILL_DOING == ITEM_TYPE( ITEM_TYPE_USE, pITEM->m_nItemNo ) ) 
	{
		if ( !(this->m_dwPayFLAG & PLAY_FLAG_BATTLE) )  /// 공격 못해... 돈내야함
			return true;


		/// 스킬 사용하는 아이템...
		short nSkillIDX = USEITEM_SCROLL_USE_SKILL( pITEM->m_nItemNo );

		if ( this->Is_SelfSKILL( nSkillIDX ) ) 
		{
			if ( !this->Do_SelfSKILL( nSkillIDX ) )		// 능력치 부족...
				return true;
		}
		else if ( this->Is_TargetSKILL( nSkillIDX ) ) 
		{
			if ( pPacket->m_HEADER.m_nSize != (sizeof(WORD)+sizeof(cli_USE_ITEM)) )
				return true;

			WORD wDestObjIDX = *( (WORD*)( pPacket->m_pDATA+ sizeof(cli_USE_ITEM) ) );
			if ( !this->Do_TargetSKILL( wDestObjIDX, nSkillIDX ) )
				return true;
		}
		else
		{
			switch( SKILL_TYPE( nSkillIDX ) ) 
			{
				case SKILL_TYPE_18 :	// 워프 !!!
				{
					if ( SKILL_WARP_PLANET_NO( nSkillIDX ) != ZONE_PLANET_NO( this->GetZONE()->Get_ZoneNO() ) ) 
					{
						/// 워프 아이템은 같은 행성으로만 사용가능하다..
						return true;
					}

					// MP양 = (현재소지량) * 0.3
					int iNeedMP = (int)( this->Get_WEIGHT() * 0.05f );
					if ( this->Get_MP() < iNeedMP )
						return true;
					this->Sub_MP( iNeedMP );

					tPOINTF PosGOTO;
					PosGOTO.x = SKILL_WARP_ZONE_XPOS( nSkillIDX ) * 1000.f;
					PosGOTO.y = SKILL_WARP_ZONE_YPOS( nSkillIDX ) * 1000.f;
					this->Send_gsv_RELAY_REQ( RELAY_TYPE_RECALL, SKILL_WARP_ZONE_NO( nSkillIDX ), PosGOTO );
					break;
				}
				case SKILL_TYPE_16 :	// emotion
					// 소모되도록만...
					break;
				default :
					return true;
			}
		}
	}
	else if ( USE_ITEM_SKILL_LEARN == ITEM_TYPE( ITEM_TYPE_USE, pITEM->m_nItemNo ) ) 
	{
		/// 스킬 익히는 아이템...
		switch( this->Send_gsv_SKILL_LEARN_REPLY( USEITEM_SCROLL_LEARN_SKILL( pITEM->m_nItemNo ) ) ) 
		{
			case RESULT_SKILL_LEARN_SUCCESS :
				break;
			case RESULT_SKILL_LEARN_NEED_JOB   :
			case RESULT_SKILL_LEARN_NEED_SKILL :	// 필요 스킬이 있는데 요청하면 짤러~~~ 있을수 없는 경우...
				return false;
			default :	// 배우지 못한 기타 사유면 아이템 그대로 보유
				return true;
		}
	}
#ifdef __SKILL_RESET_ITEM
	//	스킬 삭제 아이템 사용 확인. 4번 컬럼의 값을 얻음
	else if( USE_ITEM_SKILL_RESET == ITEM_TYPE( ITEM_TYPE_USE, pITEM->m_nItemNo ))
	{
		//	스킬 삭제 아이템의 20번 컬럼값을 얻어온다 .20,40,50,100의 경우만 처리한다.
		int Temp_Type = ItemSTBGetData(ITEM_TYPE_USE,pITEM->m_nItemNo,20);
		switch(Temp_Type)
		{
		case 20:
			//	직업 스킬 초기화
			Reward_InitSKILLType(1,0);
			//	유저의 보유 스킬을 확인다.
			break;
		case 40:
			//	유니크 스킬 초기화
			Reward_InitSKILLType(2,0);
			break;
		case 50:
			//	마일리지 스킬 초기화
			Reward_InitSKILLType(3,0);
			break;
		case 100:
			//	모두(직업,유니크,마일리지)초기화
			Reward_InitSKILLType(4,0);
			break;
		default :	
			//	정의 되지 않은 값이다. 중지.
			return true;
		}
		//	?바 초기화
		this->m_HotICONS.Init();
		/// 주변에 사용 가능한 아이템이면... 갯수 감소...
		this->Use_pITEM( pITEM );
	}
#endif
	else if ( USE_ITEM_FUEL == ITEM_TYPE( ITEM_TYPE_USE, pITEM->m_nItemNo ) ) 
	{
		// 카트/캐슬기어 연료 아이템...
		tagITEM *pEngine = &m_Inventory.m_ItemRIDE[ RIDE_PART_ENGINE ];
		if ( ITEM_TYPE_RIDE_PART != pEngine->GetTYPE() ) 
		{
			return true;
		}

		int iAddValue = USEITEM_ADD_DATA_VALUE( pITEM->m_nItemNo ) * 10;
		if ( pEngine->m_nLife + iAddValue > MAX_ITEM_LIFE )
			pEngine->m_nLife = MAX_ITEM_LIFE;
		else
			pEngine->m_nLife += iAddValue;

		this->Send_gsv_SET_ITEM_LIFE( INVENTORY_RIDE_ITEM0 + RIDE_PART_ENGINE, pEngine->GetLife () );
	} 
	else if ( USE_ITEM_MAINTAIN_ITEM == ITEM_TYPE( ITEM_TYPE_USE, pITEM->m_nItemNo ) )
	{
		/// 시간 지정 쿠폰 아이템....
		switch( USEITEM_ADD_DATA_TYPE( pITEM->m_nItemNo ) ) 
		{
			case AT_BANK_FREE	:
			case AT_BANK_ADDON	:
			case AT_STORE_SKIN	:
#ifdef	__INC_PLATINUM
				this->m_GrowAbility.UpdateSTATUS( this->GetCurAbsSEC(), 
						USEITEM_ADD_DATA_TYPE( pITEM->m_nItemNo ),		// x 능력치를
						USEITEM_ADD_DATA_VALUE( pITEM->m_nItemNo ),		// y 이후...
						USEITEM_STORE_SKIN( pITEM->m_nItemNo ) );		// z 값으로 ( 개인상점 스킨 )
#endif
				break;


#ifdef __EXPERIENCE_ITEM
			// (20061124:남병철) : 경험치 아이템
			case AT_PSV_EXPERIENCE_ITEM:
			{
				if( false == this->m_GrowAbility.UpdateSTATUS( this->GetCurAbsSEC(),						// 현재 시간
															   USEITEM_ADD_DATA_TYPE( pITEM->m_nItemNo ),	// 적용 능력치
															   USEITEM_ADD_DATA_VALUE( pITEM->m_nItemNo ),	// 1 = 10분 (아이템 효력 시간(10분))
															   USEITEM_STORE_SKIN( pITEM->m_nItemNo ) )		// 경험치 증가 퍼센트(%) (200%, 300%)
					)
				{
					// 중복 사용으로 경고				
					Send_GSV_WarningMsgCode( EGSV_DONT_DUPLICATE_EXPERIENCE_MILEAGE_ITEM_USE );
					return true;
				}
				break;
			}
#endif
		}
		//USEITEM_STORE_SKIN( pITEM->m_nItemNo );		// 설정할 ...
		//USEITEM_ADD_DATA_VALUE( pITEM->m_nItemNo );	// 지속될 시간...
	}
	else 
	{
		/// 주변에 사용 가능한 아이템이면... 갯수 감소...
		this->Use_pITEM( pITEM );
	}

	/// 마지막 사용한 시간 저장...
	this->m_dwCoolTIME[ nCoolTimeType ] = dwCurTime;

	// 수량 감소
	if ( --pITEM->m_uiQuantity <= 0 ) 
	{

#ifdef __EXPERIENCE_ITEM
		// 메인테인 아이템(경험치2배등)일 경우 마지막 사용을 개인에게 알려줌
		if( USEITEM_ADD_DATA_TYPE( pITEM->m_nItemNo ) == AT_PSV_EXPERIENCE_ITEM )
		{
			/// 자신 한테만 소모성 아이템 사용했다고 전송.
			this->Send_gsv_USE_ITEM( pITEM->m_nItemNo, nInventoryIDX );
		}
#endif

		/// 다 소모했다..
		m_Inventory.DeleteITEM( nInventoryIDX );

		this->Send_gsv_SET_INV_ONLY( (BYTE)nInventoryIDX, pITEM );
	} 
	else 
	{
		/// 자신 한테만 소모성 아이템 사용했다고 전송.
		this->Send_gsv_USE_ITEM( pITEM->m_nItemNo, nInventoryIDX );
	}

    return true;
}


//-------------------------------------------------------------------------------------------------
/// gsv_SET_MONEYnINV 패킷 할당및 초기화
classPACKET *classUSER::Init_gsv_SET_MONEYnINV (WORD wType)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( pCPacket ) {
		pCPacket->m_HEADER.m_wType = wType;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_MONEYnINV );
		pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT = 0;
	}

	return pCPacket;
}
/// gsv_SET_MONEYnINV 패킷 전송
bool classUSER::Send_gsv_SET_MONEYnINV( classPACKET *pCPacket )
{
	if ( pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ) {
		// 변경된 인벤토리가 있을경우만 전송한다.
		pCPacket->m_HEADER.m_nSize += ( pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT * sizeof( tag_SET_INVITEM ) );
		pCPacket->m_gsv_SET_MONEYnINV.m_i64Money  = this->GetCur_MONEY();

		this->SendPacket( pCPacket );
	}

	Packet_ReleaseNUnlock( pCPacket );

	return true;
}


//-------------------------------------------------------------------------------------------------
/// 채팅금지, 강제 접속 종료등... GM의 명령 전송
bool classUSER::Send_gsv_GM_COMMAND( char *szAccount, BYTE btCMD, WORD wBlockTIME)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_GM_COMMAND;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_GM_COMMAND );
	pCPacket->m_gsv_GM_COMMAND.m_btCMD = btCMD;
	pCPacket->m_gsv_GM_COMMAND.m_wBlockTIME = wBlockTIME;

	pCPacket->AppendString ( szAccount );

	this->SendPacket( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

//-------------------------------------------------------------------------------------------------
/// 서버의 현재 돈양을 통보 :: 퀘스트 보상등 돈의 양이 틀려 졌을경우 호출됨
bool classUSER::Send_gsv_SET_MONEYONLY (WORD wType)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = wType;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_MONEY_ONLY );
	pCPacket->m_gsv_SET_MONEY_ONLY.m_i64Money  = this->GetCur_MONEY();

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 월드서버에서 채널서버로 이동후 서버에 들어올수 있는지 검증패킷 통보
bool classUSER::Send_srv_JOIN_SERVER_REPLY (t_PACKET *pRecvPket, char *szAccount)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;


//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(szAccount, strlen(szAccount));
//-------------------------------------

	pCPacket->m_HEADER.m_wType = SRV_JOIN_SERVER_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( srv_JOIN_SERVER_REPLY );
	pCPacket->m_srv_JOIN_SERVER_REPLY.m_btResult = pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_btResult;

	DWORD dwRecvSeqNO;

	if ( RESULT_CONFIRM_ACCOUNT_OK == pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_btResult ) {
		dwRecvSeqNO = ::timeGetTime();	// CRandom::GetInstance()->Get();
		pCPacket->m_srv_JOIN_SERVER_REPLY.m_dwID = dwRecvSeqNO;
		pCPacket->m_srv_JOIN_SERVER_REPLY.m_dwPayFLAG = 0;

		// CUserLIST::Add_ACCOUNT 안에서 설정되던 계정을 이곳으로 옮김
		// 여기서 보내는 패킷의 응답 패킷이 CUserLIST::Add_ACCOUNT안에서 계정이
		// 설정되기 전에 처리되면 뻑남~~~~
		this->m_HashACCOUNT= CStrVAR::GetHASH( szAccount );
		this->Set_ACCOUNT( szAccount );
	} else
		dwRecvSeqNO = -1;

	this->SendPacket( pCPacket );
	this->m_iRecvSeqNO = dwRecvSeqNO;
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}


//-------------------------------------------------------------------------------------------------
// bool classUSER::Send_gsv_SELECT_CHAR (void)
/// 클라이언트에 존에 입장허가 패킷 전송
bool classUSER::Send_gsv_JOIN_ZONE (CZoneTHREAD *pZONE)
{
	if ( IsTAIWAN() ) 
	{
		if ( this->m_btRideMODE && ZONE_RIDING_REFUSE_FLAG( pZONE->Get_ZoneNO() ) ) {
			// 0x01 : 카트 불가, 0x02 : 캐슬기어 불가
			if ( ITEM_TYPE_RIDE_PART == this->m_Inventory.m_ItemRIDE[ RIDE_PART_BODY ].GetTYPE() ) {
				int iType = ITEM_TYPE( ITEM_TYPE_RIDE_PART, this->m_Inventory.m_ItemRIDE[ RIDE_PART_BODY ].GetItemNO() );
				if ( (iType%3) & ZONE_RIDING_REFUSE_FLAG( this->GetZONE()->Get_ZoneNO() ) ) {
					// 탑승 제한~
					this->m_btRideMODE = 0;
				}
			} else {
				this->m_btRideMODE = 0;		// 뭐냐 이건 ??
			}
		}
		// 워프하면 능력치 업데이터 한다.
		// 강제 해제 됐나 ?
		if ( 0 == this->m_btRideMODE ) 
		{			
			//	2006.03.16/김대성 - 카트나 캐슬기어를 탈수 없는 지역이 있다. (고블린동굴)
			//	- 카트를 타고 카트를 탈수 없는 지역에 들어가면 스킬시전이 안되는 버그
			if ( ZONE_RIDING_REFUSE_FLAG( pZONE->Get_ZoneNO() ) > 0 )	// 1:카트 불가, 2:캐슬기어 불가, 3:모두 불가
			{
				this->m_btRideMODE = 0;
				this->m_btRideATTR = RIDE_ATTR_NORMAL;		// 여기가 중요
				this->m_iLinkedCartObjIDX = 0;
				//	김영환 2006.8.29일 채크 위치 보정
				// 2006.05.30/김대성/추가
			}

			this->UpdateAbility ();		// 탑승 토글...
			this->Send_gsv_SPEED_CHANGED ();	// 스피드 변경된 값을 전송 : 존 메시지로 전송.
			//-------------------------------------
		}
	}

	m_bSetImmediateRevivePOS = false;


	//	팀 변경 처리
#ifdef __CLAN_WAR_SET


//-------------------
#ifdef __CLAN_WAR_EXT
	if(this->m_iTeamNO_Back == 0)
		this->m_iTeamNO = TEAMNO_USER;
	else
	{
		if( CClanWar::GetInstance()->IsCreatedClanWarRoom((DWORD)pZONE->Get_ZoneNO()) == TRUE )
		{
			this->m_iTeamNO = this->m_iTeamNO_Back;
		}
		else
		{
			this->m_iTeamNO = TEAMNO_USER;
			this->m_iTeamNO_Back = 0;
		}
	}
#else
	if(this->m_iTeamNO_Back == 0)
		this->m_iTeamNO = TEAMNO_USER;
	else
	{
		if(GF_Clan_War_Zone_Is(pZONE->Get_ZoneNO()))
		{
			this->m_iTeamNO = this->m_iTeamNO_Back;
		}
		else
		{
			this->m_iTeamNO = TEAMNO_USER;
			this->m_iTeamNO_Back = 0;
		}
	}
#endif
//-------------------


#else
	this->m_iTeamNO = TEAMNO_USER;
#endif


	if ( this->GetZONE()->Get_HashJoinTRIGGER() ) {
		// 존에 조인시 발생할 트리거...
		if ( 0 == this->m_IngSTATUS.IsSubSET( FLAG_CHEAT_INVINCIBLE ) ) {
			this->Do_QuestTRIGGER( this->GetZONE()->Get_HashJoinTRIGGER() );
		}
	}

    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_JOIN_ZONE;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_JOIN_ZONE );

	pCPacket->m_gsv_JOIN_ZONE.m_wServerObjectIndex = this->Get_INDEX ();
	pCPacket->m_gsv_JOIN_ZONE.m_nCurHP = this->Get_HP ();
	pCPacket->m_gsv_JOIN_ZONE.m_nCurMP = this->Get_MP ();
	pCPacket->m_gsv_JOIN_ZONE.m_lCurEXP   = this->m_GrowAbility.m_lEXP;
	pCPacket->m_gsv_JOIN_ZONE.m_lPenalEXP = this->m_GrowAbility.m_lPenalEXP;

	pCPacket->m_gsv_JOIN_ZONE.m_VAR.m_nWorld_PRODUCT = ::Get_WorldPROD();
//	pCPacket->m_gsv_JOIN_ZONE.m_VAR.m_nWorld_STAMINA = ::Get_WorldSTAMINA();
	pCPacket->m_gsv_JOIN_ZONE.m_VAR.m_dwUpdateTIME = pZONE->GetEconomyTIME();
	pCPacket->m_gsv_JOIN_ZONE.m_VAR.m_nWorld_RATE  = ::Get_WorldRATE();
	pCPacket->m_gsv_JOIN_ZONE.m_VAR.m_btTOWN_RATE  = pZONE->m_Economy.Get_TownRATE();
	pCPacket->m_gsv_JOIN_ZONE.m_VAR.m_dwGlobalFLAGS= pZONE->GetGlobalFLAG();

	::CopyMemory( pCPacket->m_gsv_JOIN_ZONE.m_VAR.m_btItemRATE, pZONE->m_Economy.m_btItemRATE, sizeof(BYTE)*MAX_PRICE_TYPE );

	pCPacket->m_gsv_JOIN_ZONE.m_dwAccWorldTIME = ::Get_WorldPassTIME();
#ifdef	__APPLY_04_10_15_TEAMNO
	pCPacket->m_gsv_JOIN_ZONE.m_iTeamNO		   = this->Get_TeamNO();
#endif


    this->SendPacket( pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	// 자신에게 무계비율 설정.
	return this->Recv_cli_SET_WEIGHT_RATE( this->m_btWeightRate );
}

//-------------------------------------------------------------------------------------------------
/// 인벤토리및 퀘스트 데이타 전송
bool classUSER::Send_gsv_INVENTORYnQUEST_DATA (void)
{

	COMPILE_TIME_ASSERT( (sizeof(__int64) + sizeof(tagBaseITEM)*INVENTORY_TOTAL_SIZE ) < MAX_PACKET_SIZE );


	COMPILE_TIME_ASSERT( sizeof( gsv_QUEST_ONLY ) < MAX_PACKET_SIZE );
//	COMPILE_TIME_ASSERT( sizeof( gsv_QUEST_DATA ) < MAX_PACKET_SIZE );

    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	int iC;

    pCPacket->m_HEADER.m_wType = GSV_INVENTORY_DATA;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_INVENTORY_DATA );

	// pCPacket->m_gsv_INVENTORY_DATA.m_INV = this->m_Inventory;
	pCPacket->m_gsv_INVENTORY_DATA.m_INV.m_i64Money = this->m_Inventory.m_i64Money;
	for (iC=0; iC<INVENTORY_TOTAL_SIZE; iC++)
	{
		CopyMemory(&pCPacket->m_gsv_INVENTORY_DATA.m_INV.m_ItemLIST[ iC ],& this->m_Inventory.m_ItemLIST[ iC ],sizeof(tagBaseITEM));
	//	g_LOG.OutputString(0xffff,"ITEM C:%d No:%d Day:%d\n",pCPacket->m_gsv_INVENTORY_DATA.m_INV.m_ItemLIST[ iC ].GetTYPE(),pCPacket->m_gsv_INVENTORY_DATA.m_INV.m_ItemLIST[ iC ].GetItemNO(),pCPacket->m_gsv_INVENTORY_DATA.m_INV.m_ItemLIST[ iC ].m_dwDATE);
	}

    this->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

#ifdef	__APPLY_EXTAND_QUEST_VAR
	////////////------------------ 임시... 아래 패킷이 안가면 클라이언트에서 진행안됨
	//pCPacket = Packet_AllocNLock ();
	//if ( pCPacket ) {
	//	ZeroMemory( pCPacket->m_pDATA, 1020 );

	//	pCPacket->m_HEADER.m_wType = GSV_QUEST_DATA;
	//	pCPacket->m_HEADER.m_nSize = 1020;

	//	// ::CopyMemory( &pCPacket->m_gsv_QUEST_DATA.m_Quests, &this->m_Quests, sizeof( tagQuestData ) );
	//	// ::CopyMemory( &pCPacket->m_gsv_QUEST_DATA.m_WishLIST, &this->m_WishLIST, sizeof( tagWishLIST ) );

	//	this->SendPacket( pCPacket );
	//	Packet_ReleaseNUnlock( pCPacket );
	//}
	////////////------------------

	// 퀘스트 데이타
    pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_QUEST_ONLY;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_QUEST_ONLY );

	::CopyMemory( &pCPacket->m_gsv_QUEST_ONLY.m_Quests, &this->m_Quests, sizeof( tagQuestData ) );
    this->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	// 구입 희망 목록
    pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_WISH_LIST;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_WISH_LIST );

	::CopyMemory( &pCPacket->m_gsv_WISH_LIST.m_WishLIST, &this->m_WishLIST, sizeof( tagWishLIST ) );
    this->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );
#else
	pCPacket = Packet_AllocNLock ();
	if ( pCPacket ) {
		pCPacket->m_HEADER.m_wType = GSV_QUEST_DATA;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_QUEST_DATA );

		::CopyMemory( &pCPacket->m_gsv_QUEST_DATA.m_Quests, &this->m_Quests, sizeof( tagQuestData ) );
		::CopyMemory( &pCPacket->m_gsv_QUEST_DATA.m_WishLIST, &this->m_WishLIST, sizeof( tagWishLIST ) );

		this->SendPacket( pCPacket );
		Packet_ReleaseNUnlock( pCPacket );
	}
#endif


	return true;
}

#ifdef __CLAN_WAR_SET
//	해당 유저의 퀘스트 관련 변수 전송
BOOL classUSER::Send_gsv_Quest_VAR()
{
	//	유저의 퀘스트 변수들 전송한다.
	COMPILE_TIME_ASSERT( sizeof( gsv_QUEST_ONLY ) < MAX_PACKET_SIZE );

    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	////	패킷 해더 정의
    pCPacket->m_HEADER.m_wType = GSV_QUEST_ONLY;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_QUEST_ONLY );

	////	패킷 메시지 복사.
	::CopyMemory( &pCPacket->m_gsv_QUEST_ONLY.m_Quests, &this->m_Quests, sizeof( tagQuestData ) );
	::memcpy(pCPacket->m_gsv_QUEST_ONLY.m_Quests.m_nClanWarVAR,this->m_Quests.m_nClanWarVAR,20);
    this->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	return true;
}
#endif

//-------------------------------------------------------------------------------------------------
// (20060913:남병철) : 채팅장에 경고 메시지 코드 전달
#ifdef __PARTY_EXP_WARNING
bool classUSER::Send_GSV_WarningMsgCode( DWORD dwWarningCode )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_WARNING_MSGCODE;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_WARNING_MSGCODE );
	pCPacket->m_gsv_WARNING_MSGCODE.m_dwWarningCode = dwWarningCode;

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}
#endif
//-------------------------------------------------------------------------------------------------

bool classUSER::Send_gsv_MILEAGE_INFO()
{
#ifdef  __KRRE_NEW_AVATAR_DB

	int iC;
	classPACKET *pCPacket = Packet_AllocNLock();
	//////////////////////////////////////////////////////////////////////////
	// 2006.06.12:이성활:pCPacket에 대한 Null 체크
	if (!pCPacket) {
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	pCPacket->m_HEADER.m_wType = GSV_MILEAGE_INV_DATA;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_MILEAGE_INV_DATA );

	//////////////////////////////////////////////////////////////////////////
	// 2006.06.12:이성활:필요없는 코드
	//classPACKET *pPacket = Packet_AllocNLock();
	//pPacket->m_gsv_DESTROY_MILEAGE_ITEM.m_btItemCNT =0;
	//Packet_ReleaseNUnlock( pPacket); // 임시추가
	//////////////////////////////////////////////////////////////////////////

	DWORD dwCurDate  = classTIME::GetCurrentAbsSecond();

	for (iC=0; iC<MAX_MILEAGE_INV_SIZE; iC++)
	{

		CopyMemory(&pCPacket->m_gsv_MILEAGE_INV_DATA.m_INV.m_ItemLIST[ iC ],&this->m_MileageInv.m_ItemLIST[ iC ],sizeof(tagBaseITEM));
		pCPacket->m_gsv_MILEAGE_INV_DATA.m_INV.m_ItemLIST[ iC ].m_dwDate = this->m_MileageInv.m_ItemLIST[ iC ].m_dwDate;

	}

//나중에.. 카트쪽 착용도 검사해야 한다..


	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
#endif

	return false;
}


//-------------------------------------------------------------------------------------------------
/// 서버의 경험치를 전송 :: 사용자 경험치가 변동되었을경우..
bool classUSER::Send_gsv_SETEXP (WORD wFromObjIDX)
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	pCPacket->m_HEADER.m_wType	= GSV_SETEXP;
	pCPacket->m_HEADER.m_nSize	= sizeof( gsv_SETEXP );
	pCPacket->m_gsv_SETEXP.m_lCurEXP = this->GetCur_EXP ();
	pCPacket->m_gsv_SETEXP.m_nCurSTAMINA = this->GetCur_STAMINA ();
	pCPacket->m_gsv_SETEXP.m_wFromObjIDX = wFromObjIDX;

	this->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 레벨업 되었을경우 호출되는 함수
bool classUSER::Send_gsv_LEVELUP (short nLevelDIFF)
{
/*	존 분할로 운영시 월드 서버에서 파티 운영될수 있도록 전송하던...
#ifndef	__INC_WORLD
	g_pSockLSV->Send_gsv_LEVEL_UP( LEVELUP_OP_USER, this->m_dwWSID, this->Get_LEVEL(), m_GrowAbility.m_lEXP );
#endif
*/
	// 레벨업시 획득 포인트...
	this->UpdateAbility ();		// levelup

	if ( this->Get_HP() > 0 ) {
		// 죽었을때 레벨업하면 살아나던 버그 수정...
		this->Set_HP ( this->Get_MaxHP() );
		this->Set_MP ( this->Get_MaxMP() );
	}

    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	pCPacket->m_HEADER.m_wType			= GSV_LEVELUP;
	pCPacket->m_HEADER.m_nSize			= sizeof( gsv_LEVELUP );
	pCPacket->m_gsv_LEVELUP.m_wObjectIDX= this->Get_INDEX ();
	pCPacket->m_gsv_LEVELUP.m_nCurLevel	= this->Get_LEVEL ();
	pCPacket->m_gsv_LEVELUP.m_lCurEXP   = m_GrowAbility.m_lEXP;
	pCPacket->m_gsv_LEVELUP.m_nBonusPoint = this->GetCur_BonusPOINT ();
	pCPacket->m_gsv_LEVELUP.m_nSkillPoint = this->GetCur_SkillPOINT ();

	//TODO:: 이동 스피드가 바뀌었으면 주변에 전동...
	this->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

    pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	// 주변 사용자들에게는 단순히 효과만 보이자...
	pCPacket->m_HEADER.m_wType			= GSV_LEVELUP;
	pCPacket->m_HEADER.m_nSize			= sizeof( t_PACKETHEADER ) + sizeof(WORD);
	pCPacket->m_gsv_LEVELUP.m_wObjectIDX= this->Get_INDEX ();

    this->GetZONE()->SendPacketToSectors( this, pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	//	렙업에 따른 : 김영환 수정 : LV UP, 데이지 받은 경우, HP 젠되는 경우
	if ( this->GetPARTY() ) {
		this->m_pPartyBUFF->Member_LevelUP( this->m_nPartyPOS, nLevelDIFF );
		//	자신의 상태를 파티에 전송
		this->GetPARTY()->Change_ObjectIDX( (classUSER*)this );
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 클라이언트에 워프 하라는 명령 전송
bool classUSER::Send_gsv_TELEPORT_REPLY (tPOINTF &PosWARP, short nZoneNO )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_TELEPORT_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_TELEPORT_REPLY );

	pCPacket->m_gsv_TELEPORT_REPLY.m_nZoneNO    = nZoneNO;
	pCPacket->m_gsv_TELEPORT_REPLY.m_PosWARP    = PosWARP;
	pCPacket->m_gsv_TELEPORT_REPLY.m_wObjectIDX = this->Get_INDEX();
	pCPacket->m_gsv_TELEPORT_REPLY.m_btRunMODE  = this->m_bRunMODE;		// 걷기 뛰기 상태
	pCPacket->m_gsv_TELEPORT_REPLY.m_btRideMODE = this->m_btRideMODE;

//-------------------------------------
// 2006.05.10/김대성/추가 - 로딩이미지때문에 워프할때마다 특정 npc (1201번) 의 상태값을 같이 전송해 준다.
	CObjNPC *pNPC = g_pZoneLIST->Get_LocalNPC( 1201 );
	if ( pNPC ) 
	{
		 pCPacket->AppendData( pNPC->m_pVAR, sizeof( tagObjVAR ) );
	}
//-------------------------------------

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	
	return true;
}

/// 과금이 지불되어야만 이동가능한 존인지 체크...
bool  classUSER::Check_WarpPayment (short nZoneNO)
{
	if ( ZONE_IS_UNDERGROUND( nZoneNO ) )
	{
		// 던젼으루 못가 !!!
		if ( ZONE_PLANET_NO(nZoneNO) <= 1 )
		{
			if ( !(this->m_dwPayFLAG & PLAY_FLAG_BATTLE ) ) 
			{
				this->Send_gsv_BILLING_MESSAGE2( BILLING_MSG_JPN_NEED_CHARGE, 'I', PLAY_FLAG_BATTLE );
				return false;
			}
		}
		else if ( !(this->m_dwPayFLAG & PLAY_FLAG_DUNGEON_ADV ) ) 
		{
			this->Send_gsv_BILLING_MESSAGE2( BILLING_MSG_JPN_NEED_CHARGE, 'Q', PLAY_FLAG_DUNGEON_ADV );
			return false;
		}
	}
	if ( ZONE_PLANET_NO(nZoneNO) >= 4 ) 
	{
		if ( AGIT_ZONE_TYPE == ZONE_PVP_STATE( nZoneNO) )
		{
			// 아지트존은 엔트리.
			if ( !(this->m_dwPayFLAG & PLAY_FLAG_BATTLE ) ) 
			{
				// 3번째 행성까지 이동은 엔트리로
				this->Send_gsv_BILLING_MESSAGE2( BILLING_MSG_JPN_NEED_CHARGE, 'H', PLAY_FLAG_BATTLE );
				return false;
			}
		} else if ( !(this->m_dwPayFLAG & PLAY_FLAG_STARSHIP_PASS ) ) 
		{
			// 행성이동 못해 !!!
			this->Send_gsv_BILLING_MESSAGE2( BILLING_MSG_JPN_NEED_CHARGE, 'P', PLAY_FLAG_STARSHIP_PASS );
			return false;
		}
	}
	else 	if ( ZONE_PLANET_NO(nZoneNO) != 1 && !(this->m_dwPayFLAG & PLAY_FLAG_BATTLE ) ) 
	{
		// 3번째 행성까지 이동은 엔트리로
		this->Send_gsv_BILLING_MESSAGE2( BILLING_MSG_JPN_NEED_CHARGE, 'I', PLAY_FLAG_BATTLE );
		return false;
	}
	return true;
}

/// 워프 처리...
short classUSER::Proc_TELEPORT (short nZoneNO, tPOINTF &PosWARP, bool bSkipPayment)	
{
	if ( NULL == this->GetZONE() ) {
		IS_HACKING( this, "Send_gsv_TELEPORT_REPLY :: must !this->GetZONE()" );
		return RET_FAILED;
	}

	if ( !g_pZoneLIST->IsValidZONE( nZoneNO ) ) {
		return RET_OK;
	}

//-------------------------------------
// 2006.12.08/김대성/추가 - 2인탑승 카트는 존이동이 불가능하게 한다.
//	if ( RIDE_MODE_DRIVE == this->GetCur_RIDE_MODE() && this->m_iLinkedCartObjIDX ) 
//	{
//		// 응답패킷을 전송한다.
//		this->Send_gsv_TELEPORT_REPLY ( PosWARP, 0 );	//존번호를 0으로하면 클라이언트에서 워프하지 않는다.
//		return RET_OK;
//	}
//-------------------------------------	

	// 행성이동 과금 체크...
	if ( !bSkipPayment && !this->Check_WarpPayment(nZoneNO) )
		return true;


	//	워프 지역^^
	if ( g_pZoneLIST->GetZONE( nZoneNO ) )
	{
		// 로컬로 운영되는 존이다.
		if ( !this->Send_gsv_TELEPORT_REPLY(PosWARP, nZoneNO) )
			return RET_FAILED;

		// 존에서 빼버리고 다음 존위치 저장....
		this->GetZONE()->Dec_UserCNT ();
		this->GetZONE()->Sub_DIRECT( this );

		this->m_nZoneNO = nZoneNO;
		this->m_PosCUR  = PosWARP;

		if ( this->Is_CartDriver() ) {
			// 카트 드라이버가 워프할때시는 뒤에 탑승자도 같이 워프~
			classUSER *pUSER = g_pObjMGR->Get_UserOBJ( this->m_iLinkedCartObjIDX );
			if ( pUSER ) 
			{
				pUSER->m_iLinkedCartObjIDX = 0;
				pUSER->Send_gsv_RELAY_REQ( RELAY_TYPE_RECALL, nZoneNO, PosWARP );
			}
			this->m_iLinkedCartObjIDX = 0;
		} 
		else if( this->Is_CartGuest() && this->m_iLinkedCartObjIDX ) 
		{
			// 이런 경우는 있으면 안되지만..혹시 치트로 소환을 한다던가 할경우
			classUSER *pUSER = g_pObjMGR->Get_UserOBJ( this->m_iLinkedCartObjIDX );
			if ( pUSER ) {
				pUSER->m_iLinkedCartObjIDX = 0;
			}

			// 강제 내리기
			this->m_btRideMODE = 0;
			this->m_btRideATTR = RIDE_ATTR_NORMAL;
			this->m_iLinkedCartObjIDX = 0;
		}
//-------------------------------------
//	2006.03.16/김대성 - 카트나 캐슬기어를 탈수 없는 지역이 있다. (고블린동굴)
//	- 카트를 타고 카트를 탈수 없는 지역에 들어가면 스킬시전이 안되는 버그
		if ( ZONE_RIDING_REFUSE_FLAG( nZoneNO ) > 0 )	// 1:카트 불가, 2:캐슬기어 불가, 3:모두 불가
		{
			//	서버의 상태만 바꾼것임. [버그 수정]
			this->m_btRideMODE = 0;
			this->m_btRideATTR = RIDE_ATTR_NORMAL;		// 여기가 중요
			this->m_iLinkedCartObjIDX = 0;

//	// 2006.05.30/김대성/추가
//			this->UpdateAbility ();		// 캐릭터 능력치 갱신
//			this->Send_gsv_SPEED_CHANGED ();
		}
//-------------------------------------
		return RET_SKIP_PROC;
	}
	else
	{
//-------------------------------------
// 2006.03.16/김대성 - 카트나 캐슬기어를 탈수 없는 지역이 있다. (고블린동굴)
//					- 카트를 타고 카트를 탈수 없는 지역에 들어가면 스킬시전이 안되는 버그
		if ( ZONE_RIDING_REFUSE_FLAG( nZoneNO ) > 0 )	// 1:카트 불가, 2:캐슬기어 불가, 3:모두 불가
		{
			//	서버의 상태만 바꾼것임. [버그 수정]
			this->m_btRideMODE = 0;
			this->m_btRideATTR = RIDE_ATTR_NORMAL;		// 여기가 중요
			this->m_iLinkedCartObjIDX = 0;

	// 2006.05.30/김대성/추가
//			this->UpdateAbility ();		// 캐릭터 능력치 갱신
//			this->Send_gsv_SPEED_CHANGED ();
		}
//-------------------------------------
	}

	// 다른 서버에 있는 존으로 이동한다.
	// DB에 기록후 월드 서버에 워프 패킷 전송...
	this->GetZONE()->Dec_UserCNT ();
	this->GetZONE()->Sub_DIRECT( this );

	this->m_nZoneNO = nZoneNO;
	this->m_PosCUR  = PosWARP;

	g_pUserLIST->DeleteUSER( this, LOGOUT_MODE_WARP );

	return RET_SKIP_PROC;
}

//-------------------------------------------------------------------------------------------------
/// 클라이언트의 좌표를 보정하라는 패킷...
bool classUSER::Send_gsv_ADJUST_POS (bool bOnlySelf)
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_ADJUST_POS;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_ADJUST_POS );

	pCPacket->m_gsv_ADJUST_POS.m_wObjectIDX = this->Get_INDEX ();
	pCPacket->m_gsv_ADJUST_POS.m_PosCUR		= this->m_PosGOTO;
	pCPacket->m_gsv_ADJUST_POS.m_nPosZ		= this->m_nPosZ;

	if ( bOnlySelf )
		this->SendPacket( pCPacket );
	else
		this->GetZONE()->SendPacketToSectors( this, pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 스킬을 습득했을때 결과 통보
BYTE classUSER::Send_gsv_SKILL_LEARN_REPLY (short nSkillIDX, bool bCheckCOND)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return RESULT_SKILL_LEARN_FAILED;

	// 같은 종류의 스킬을 이미 보유하고 있다면 ???
	BYTE btResult;
	short nSkillSLOT;

#ifdef  __KRRE_NEW_AVATAR_DB


	if ( bCheckCOND )
		btResult = this->Skill_LearnCondition ( nSkillIDX,nSkillSLOT );
	else 
	{
		// 퀘스트에 의해 보상으로 주어지는 스킬일 경우 조건 무시...
		btResult = RESULT_SKILL_LEARN_SUCCESS;
		nSkillSLOT = this->Skill_FindEmptySlot( nSkillIDX );
	}

	if ( btResult == RESULT_SKILL_LEARN_SUCCESS ) 
	{
		if ( nSkillSLOT >= 0 ) 
		{
			// 스킬 습득...
			g_pThreadLOG->When_LearnSKILL( this, nSkillIDX );

			switch( this->Skill_LEARN( nSkillSLOT, nSkillIDX, bCheckCOND ) ) {
				case 0x03 :
					this->Cal_BattleAbility ();
				case 0x01 :
					this->Update_SPEED ();
					this->Send_gsv_SPEED_CHANGED ();
			}
		} 
		else
			btResult = RESULT_SKILL_LEARN_OUTOFSLOT;
//	스킬 학습 메시지 구조체 변경
//	김영환 수정
#ifdef	__MILEAGE_SKILL_USED
		pCPacket->m_gsv_SKILL_LEARN_REPLY.m_wSkillSLOT = (WORD)nSkillSLOT;
#else
		pCPacket->m_gsv_SKILL_LEARN_REPLY.m_btSkillSLOT = (BYTE)nSkillSLOT;
#endif
	}

#else

	if ( bCheckCOND )
		btResult = this->Skill_LearnCondition ( nSkillIDX );
	else 
	{
		// 퀘스트에 의해 보상으로 주어지는 스킬일 경우 조건 무시...
		btResult = RESULT_SKILL_LEARN_SUCCESS;
	}

	if ( btResult == RESULT_SKILL_LEARN_SUCCESS ) 
	{

		nSkillSLOT = this->Skill_FindEmptySlot( nSkillIDX );
		if ( nSkillSLOT >= 0 ) {
			// 스킬 습득...
			g_pThreadLOG->When_LearnSKILL( this, nSkillIDX );

			switch( this->Skill_LEARN( nSkillSLOT, nSkillIDX, bCheckCOND ) ) {
				case 0x03 :
					this->Cal_BattleAbility ();
				case 0x01 :
					this->Update_SPEED ();
					this->Send_gsv_SPEED_CHANGED ();
			}
		} 
		else
			btResult = RESULT_SKILL_LEARN_OUTOFSLOT;

//	김영환 수정
#ifdef	__MILEAGE_SKILL_USED
		pCPacket->m_gsv_SKILL_LEARN_REPLY.m_wSkillSLOT = (WORD)nSkillSLOT;
#else
		pCPacket->m_gsv_SKILL_LEARN_REPLY.m_btSkillSLOT = (BYTE)nSkillSLOT;
#endif
	}

#endif

	pCPacket->m_HEADER.m_wType = GSV_SKILL_LEARN_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SKILL_LEARN_REPLY );

	pCPacket->m_gsv_SKILL_LEARN_REPLY.m_btResult    = btResult;
	pCPacket->m_gsv_SKILL_LEARN_REPLY.m_nSkillIDX   = nSkillIDX;
	pCPacket->m_gsv_SKILL_LEARN_REPLY.m_nSkillPOINT = this->GetCur_SkillPOINT ();

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return btResult;
}

//-------------------------------------------------------------------------------------------------
/// 클라이언트가 요청한 케릭터의 HP 전송
bool classUSER::Send_gsv_HP_REPLY (int iObjectIDX, int iHP)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_HP_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_HP_REPLY );
	pCPacket->m_gsv_HP_REPLY.m_wObjectIDX = iObjectIDX;
	pCPacket->m_gsv_HP_REPLY.m_iHP = iHP;

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 귓속말 패킷 ..
bool classUSER::Send_gsv_WHISPER (char *szFromAccount, char *szMessage)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_WHISPER;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_WHISPER );
	pCPacket->AppendString( szFromAccount );
	pCPacket->AppendString( szMessage );

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 개인 1:1 거래의 결과 패킷 전송
bool classUSER::Send_gsv_TRADE_P2P (int iObjectIDX, BYTE btResult, char cTradeSLOT)
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_TRADE_P2P;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_TRADE_P2P );
	pCPacket->m_gsv_TRADE_P2P.m_wObjectIDX = iObjectIDX;
	pCPacket->m_gsv_TRADE_P2P.m_btRESULT = btResult;
	pCPacket->m_gsv_TRADE_P2P.m_cTradeSLOT = cTradeSLOT;

	this->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

/// 1:1 거래시 거래 슬롯이 바뀌었을 경우 서버의 거래슬롯 아이템 전송
bool classUSER::Send_gsv_TRADE_P2P_ITEM (char cTradeSLOT, tagITEM &sITEM)
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_TRADE_P2P_ITEM;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_TRADE_P2P_ITEM );

	pCPacket->m_gsv_TRADE_P2P_ITEM.m_cTradeSLOT = cTradeSLOT;
	pCPacket->m_gsv_TRADE_P2P_ITEM.m_ITEM = sITEM;

	this->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 창고의 모든 아이템 정보 전송
bool classUSER::Send_gsv_BANK_ITEM_LIST (bool bNewBank)
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	classPACKET *pCPacket2 = NULL;
	if ( !pCPacket )
		return false;

	this->m_btBankData = BANK_LOADED;
	pCPacket->m_gsv_BANK_LIST_REPLY.m_btREPLY = BANK_REPLY_INIT_DATA;
	pCPacket->m_gsv_BANK_LIST_REPLY.m_btItemCNT = 0;

	if ( !bNewBank ) {
		for (short nI=0; nI<BANKSLOT_PLATINUM_0; nI++) {
			if ( !this->m_Bank.m_ItemLIST[ nI ].GetTYPE() )
				continue;

			pCPacket->m_gsv_BANK_LIST_REPLY.m_sInvITEM[ pCPacket->m_gsv_BANK_LIST_REPLY.m_btItemCNT ].m_btInvIDX = (BYTE)nI;
			pCPacket->m_gsv_BANK_LIST_REPLY.m_sInvITEM[ pCPacket->m_gsv_BANK_LIST_REPLY.m_btItemCNT ].m_ITEM = this->m_Bank.m_ItemLIST[ nI ];
			pCPacket->m_gsv_BANK_LIST_REPLY.m_btItemCNT ++;
		}

#ifdef	__INC_PLATINUM
		pCPacket2 = Packet_AllocNLock ();
		if ( pCPacket2 ) {
			// 플레티넘 창고 아이템...
			pCPacket2->m_gsv_BANK_LIST_REPLY.m_btREPLY = BANK_REPLY_PLATINUM;
			pCPacket2->m_gsv_BANK_LIST_REPLY.m_btItemCNT = 0;
			
			for (short nI=BANKSLOT_PLATINUM_0; nI<BANKSLOT_TOTAL; nI++) {
				if ( !this->m_Bank.m_ItemLIST[ nI ].GetTYPE() )
					continue;

				pCPacket2->m_gsv_BANK_LIST_REPLY.m_sInvITEM[ pCPacket2->m_gsv_BANK_LIST_REPLY.m_btItemCNT ].m_btInvIDX = (BYTE)nI;
				pCPacket2->m_gsv_BANK_LIST_REPLY.m_sInvITEM[ pCPacket2->m_gsv_BANK_LIST_REPLY.m_btItemCNT ].m_ITEM = this->m_Bank.m_ItemLIST[ nI ];
				pCPacket2->m_gsv_BANK_LIST_REPLY.m_btItemCNT ++;
			}

			if ( pCPacket2->m_gsv_BANK_LIST_REPLY.m_btItemCNT ) {
				pCPacket2->m_HEADER.m_wType = GSV_BANK_LIST_REPLY;
				pCPacket2->m_HEADER.m_nSize = sizeof( gsv_BANK_LIST_REPLY ) + sizeof(tag_SET_INVITEM) * pCPacket2->m_gsv_BANK_LIST_REPLY.m_btItemCNT;
			} else {
				// 0개라 보낼 필요 없당
				Packet_ReleaseNUnlock( pCPacket2 );
				pCPacket2 = NULL;
			}
		}
#endif
	}

    pCPacket->m_HEADER.m_wType = GSV_BANK_LIST_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_BANK_LIST_REPLY ) + sizeof(tag_SET_INVITEM) * pCPacket->m_gsv_BANK_LIST_REPLY.m_btItemCNT;
	// 창고에 보관된 줄리...
	pCPacket->AppendData( &this->m_Bank.m_i64ZULY, sizeof(__int64) );

	this->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	if ( pCPacket2 ) {
		this->SendPacket( pCPacket2 );
		Packet_ReleaseNUnlock( pCPacket2 );
	}

	return true;
}

/// 창고 리스트에 대한 결과 패킷 전송
bool classUSER::Send_gsv_BANK_LIST_REPLY (BYTE btReply)
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_BANK_LIST_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_BANK_LIST_REPLY );

	pCPacket->m_gsv_BANK_LIST_REPLY.m_btREPLY	= btReply;
	pCPacket->m_gsv_BANK_LIST_REPLY.m_btItemCNT = 0;

	this->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 파티 요청 패킷 전송
bool classUSER::Send_gsv_PARTY_REQ(int iObjectIDXorTAG, BYTE btReq)
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_PARTY_REQ;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_PARTY_REQ );
	pCPacket->m_gsv_PARTY_REQ.m_btREQUEST = btReq;
	pCPacket->m_gsv_PARTY_REQ.m_dwFromIDXorTAG = iObjectIDXorTAG;

	this->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	return true;
}
/// 파티 요청에 대한 결과 패킷 전송
bool classUSER::Send_gsv_PARTY_REPLY(int iObjectIDXorTAG, BYTE btReply)
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_PARTY_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_PARTY_REPLY );
	pCPacket->m_gsv_PARTY_REPLY.m_btREPLY = btReply;
	pCPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG = iObjectIDXorTAG;

	this->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 1개의 인벤토리의 변경된 아이템 전송
bool classUSER::Send_gsv_SET_INV_ONLY (BYTE btInvIDX, tagITEM *pITEM, WORD wType)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = wType;//GSV_SET_INV_ONLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_INV_ONLY ) + sizeof( tag_SET_INVITEM );

	pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT = 1;
	pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_btInvIDX = btInvIDX;
	pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_ITEM     = *pITEM;

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 2개의 인벤토리의 변경된 아이템 전송
bool classUSER::Send_gsv_SET_TWO_INV_ONLY (WORD wType, BYTE btInvIdx1, tagITEM *pITEM1, BYTE btInvIdx2, tagITEM *pITEM2)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = wType;	// GSV_SET_INV_ONLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_INV_ONLY ) + sizeof( tag_SET_INVITEM ) * 2;

	pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT = 2;

	pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_btInvIDX = btInvIdx1;
	pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_ITEM     = *pITEM1;

	pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_btInvIDX = btInvIdx2;
	pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_ITEM     = *pITEM2;

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 서버에서 처리 순서상 바로처리하기 곤란한 패킷의 경우 클라이언트로 요청하라는 패킷을 전송하면
/// 클라이어언트는 그에 상응하는 요청패킷을 서버에 전송하고 서버는 그 패킷을 받았을시 처리하기위해 사용되는 함수
bool classUSER::Send_gsv_RELAY_REQ( WORD wRelayTYPE, short nZoneGOTO, tPOINTF &PosGOTO )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_RELAY_REQ;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_RELAY_REQ );

	pCPacket->m_gsv_RELAY_REQ.m_wRelayTYPE  = wRelayTYPE;
	pCPacket->m_gsv_RELAY_REQ.m_nCallZoneNO = nZoneGOTO;
	pCPacket->m_gsv_RELAY_REQ.m_PosCALL		= PosGOTO;


	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );


#ifdef __TEST_CHK_WARP
	SetWrapZonePos(nZoneGOTO,PosGOTO);
#endif
	return true;
}

//-------------------------------------------------------------------------------------------------
bool classUSER::Recv_cli_STRESS_TEST( t_PACKET *pPacket )
{
	/*
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = CLI_STRESS_TEST;
    pCPacket->m_HEADER.m_nSize = sizeof( cli_STRESS_TEST );
	pCPacket->AppendString( "Reply STRESS_TEST" );
	pCPacket->m_HEADER.m_nSize = pPacket->m_HEADER.m_nSize;

	_ASSERT( pCPacket->m_HEADER.m_nSize >= sizeof(t_PACKETHEADER ) );

	g_pUserLIST->Send_cli_STRESS_TEST( pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	return true;
	*/
	return false;
}

//-------------------------------------------------------------------------------------------------
/// 클라이언트에 중계하라는 패킷을 전송후에 받는 응답 패킷 처리...
short classUSER::Recv_cli_RELAY_REPLY( t_PACKET *pPacket )
{

#ifdef __TEST_CHK_WARP

	if(!ChkWarpPos(pPacket->m_cli_RELAY_REPLY.m_nCallZoneNO,pPacket->m_cli_RELAY_REPLY.m_PosCALL))
	{
		g_LOG.CS_ODS(LOG_NORMAL, "Error  %s Recv_cli_RELAY_REPLY ChkWarpPos Error!\n",Get_NAME());
		return RET_FAILED;
	}

#endif

	switch( pPacket->m_cli_RELAY_REPLY.m_wRelayTYPE ) 
	{
		case RELAY_TYPE_RECALL :
			return Proc_TELEPORT( pPacket->m_cli_RELAY_REPLY.m_nCallZoneNO,	// Recv_cli_RELAY_REPLY
											pPacket->m_cli_RELAY_REPLY.m_PosCALL );
	}

	return RET_FAILED;
}

//-------------------------------------------------------------------------------------------------
/*
int classUSER::Recv_cli_LOGOUT_REQ ()
{
    return 1;
}
bool classUSER::Recv_cli_LOGIN_REQ ( t_PACKET *pPacket )
{
	LogString(LOG_NORMAL, "Recv_cli_LOGIN_REQ:: %s: %d \n", this->Get_ACCOUNT(), this->Get_INDEX() );

	return g_pThreadSQL->Add_SqlPACKET (this, pPacket );
}
*/
//-------------------------------------------------------------------------------------------------
/// 월드 서버에서 채널서버로 이동후 플레이되기전에 인증을 요청하는 패킷
/// 이 패킷을 받으면 채널서버가 월드서버로 부터 인증을 받고 응답을 클라이언트로 보냄
bool classUSER::Recv_cli_JOIN_SERVER_REQ( t_PACKET *pPacket )
{
	// GUMS에 전송하기 위해서 비번 저장...
	::CopyMemory( this->m_dwMD5PassWD, pPacket->m_cli_JOIN_SERVER_REQ.m_MD5Password, sizeof(DWORD)*8 );
	this->m_szMD5PassWD[ 32 ] = 0;

	// 로그인 서버 또는 월드 서버에 인증 요청..
	g_pSockLSV->Send_zws_CONFIRM_ACCOUNT_REQ( this->m_iSocketIDX, pPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 케릭터 리스트 요청처리 :: 개인 서버에서만 사용됨
bool classUSER::Recv_cli_CHAR_LIST( t_PACKET *pPacket )
{
	return g_pThreadSQL->Add_SqlPacketWithACCOUNT (this, pPacket );
}

//-------------------------------------------------------------------------------------------------
/// 케릭터 선택 요청처리 :: 개인 서버에서만 사용됨
bool classUSER::Recv_cli_SELECT_CHAR( t_PACKET *pPacket, bool bFirstZONE, BYTE btRunMODE, BYTE btRideMODE )
{
	// 1 == pSelCharPket->m_cli_SELECT_CHAR.m_btCharNO
	// 이면 사용자한테 DB에서 케릭터 읽어서 전송...
	pPacket->m_cli_SELECT_CHAR.m_btCharNO   = bFirstZONE;
	pPacket->m_cli_SELECT_CHAR.m_btRunMODE  = btRunMODE;
	pPacket->m_cli_SELECT_CHAR.m_btRideMODE = btRideMODE;

	return g_pThreadSQL->Add_SqlPacketWithACCOUNT (this, pPacket );
}

//-------------------------------------------------------------------------------------------------
/// 케릭터 생성 요청처리 :: 개인 서버에서만 사용됨
bool classUSER::Recv_cli_CREATE_CHAR( t_PACKET *pPacket )
{
/*
	short nOffset=sizeof(cli_CREATE_CHAR);
	char *pCharName=Packet_GetStringPtr( pPacket, nOffset );
	if ( !pCharName ) {
		return false;
	}

	g_pThreadLOG->When_CreateCHAR( this, pCharName );
*/
	return g_pThreadSQL->Add_SqlPacketWithACCOUNT (this, pPacket );
}

//-------------------------------------------------------------------------------------------------
/// 케릭터 삭제 요청처리 :: 개인 서버에서만 사용됨
bool classUSER::Recv_cli_DELETE_CHAR( t_PACKET *pPacket )
{
	short nOffset=sizeof(cli_DELETE_CHAR);

	char *pCharName=Packet_GetStringPtr( pPacket, nOffset );
	if ( !pCharName || !this->Get_ACCOUNT() ) {
		return false;
	}

	g_pThreadLOG->When_CharacterLOG( this, pCharName, NEWLOG_DEL_START_CHAR );

	return g_pThreadSQL->Add_SqlPacketWithACCOUNT (this, pPacket );
}


//-------------------------------------------------------------------------------------------------
/// 클라이언트에서 존에 입장하겠다는 패킷
bool classUSER::Recv_cli_JOIN_ZONE ( t_PACKET *pPacket )
{
	if ( this->GetZONE() || this->Get_INDEX() ) {
		IS_HACKING( this, "Recv_cli_JOIN_ZONE:: must !this->GetZONE() && !this->Get_INDEX() " );
		return false;
	}

	CZoneTHREAD *pZONE = g_pZoneLIST->GetZONE( m_nZoneNO );
	if ( pZONE ) {
		this->m_btWeightRate = pPacket->m_cli_JOIN_ZONE.m_btWeightRate;
		if ( this->m_btWeightRate >= WEIGHT_RATE_WALK ) {
			// 뛰기 불능.
			this->m_bRunMODE = false;
		}
		this->m_nPosZ		 = pPacket->m_cli_JOIN_ZONE.m_nPosZ;

		this->Del_ActiveSKILL ();
		this->CObjAI::SetCMD_STOP ();
		this->Clear_SummonCNT ();			// 존 워프시...

		//	존에 해당 유저 추가시키고 유저 존입장 처리 메시지 보낸다.
		if ( pZONE->Add_OBJECT( this ) )
		{
			// 사용자 존에 참가 요청 
			pZONE->Inc_UserCNT ();

			this->m_dwRecoverTIME = 0;

			if ( this->GetPARTY() ) 
			{
				this->m_pPartyBUFF->Change_ObjectIDX( this );
			}

			////	2006.03.16/김대성 - 카트나 캐슬기어를 탈수 없는 지역이 있다. (고블린동굴)
			////	- 카트를 타고 카트를 탈수 없는 지역에 들어가면 스킬시전이 안되는 버그
			//if ( ZONE_RIDING_REFUSE_FLAG( pZONE->Get_ZoneNO() ) > 0 )	// 1:카트 불가, 2:캐슬기어 불가, 3:모두 불가
			//{
			//	//	김영환 2006.8.29일 채크 위치 보정
			//	// 2006.05.30/김대성/추가
			//	this->UpdateAbility ();		// 캐릭터 능력치 갱신
			//	this->Send_gsv_SPEED_CHANGED ();
			//}
			////-------------------------------------
			
#ifdef __CLAN_WAR_SET
#ifdef __CLAN_WAR_EXT
			// (20070309:남병철) : 아래와 같은 기능
			CClanWar::GetInstance()->CheckGateStatus( this );
#else
			//	클랜전 존에 입장한 경우에 존의 GATE 상태를 채그한다.
			//	작성자 : 김영환
			//	해당 존이 클랜전 존인 경우 확인하고 메시지 보냄. (기존 상태 전송.)
			GF_Set_ClanWar_Gate_NPC((LPVOID)this,GF_Clan_War_Zone_Is((DWORD)m_nZoneNO),true);
#endif
#endif

			return true;
		}
	}

	return false;
}


//-------------------------------------------------------------------------------------------------
/// 사망시 부할 요청 패킷 처리
short classUSER::Recv_cli_REVIVE_REQ( BYTE btReviveTYPE, bool bApplyPenalty, bool bSkipPayment )
{
	short nZoneNO;
	tPOINTF	PosREVIVE;
	DWORD	Temp_List_N = 0;

	// 죽을때 걸어놓은 상태만 해지...
	this->m_IngSTATUS.ClearStatusFLAG( ING_FAINTING );
	this->m_dwRecoverTIME = 0;

	switch( btReviveTYPE ) {
		case REVIVE_TYPE_SAVE_POS :		// 저장된 부활장소에서 살아나기..
			if ( ZONE_PLANET_NO(this->m_nReviveZoneNO) == ZONE_PLANET_NO(this->m_nZoneNO) ) {
				// 저장된 존의 행성과 죽은 존의 행성이 같으면 부활존으로 ....
				nZoneNO   = this->m_nReviveZoneNO;
				PosREVIVE = this->m_PosREVIVE;
				if ( IsTAIWAN() ) {
					if ( ZONE_REVIVE_ZONENO( this->m_nZoneNO ) ) {
						nZoneNO		= ZONE_REVIVE_ZONENO( this->m_nZoneNO );
						PosREVIVE.x = ZONE_REVIVE_X_POS( this->m_nZoneNO ) * 1000.f;
						PosREVIVE.y = ZONE_REVIVE_Y_POS( this->m_nZoneNO ) * 1000.f;
					}
				}

				PosREVIVE.x += ( RANDOM(1001) - 500 );	// 랜덤 5미터..
				PosREVIVE.y += ( RANDOM(1001) - 500 );
				break;
			}

		case REVIVE_TYPE_REVIVE_POS :	// 현재 존의 부활장소에서 살아나기
			nZoneNO   = this->m_nZoneNO;

//-------------------------------------
// 2006.04.25/김대성/추가 - 클랜필드에서는 현재 존에서 부활할 수 없다.
			// (클라이언트에서 번튼이 막혀있기때문에 이 패킷이 오면 해킹이다.)
			if ( nZoneNO >= 11 && nZoneNO <= 13 ) {
				return RET_FAILED;
			}
//-------------------------------------


			if ( !m_bSetImmediateRevivePOS ) {
				PosREVIVE = this->GetZONE()->Get_RevivePOS( this->m_PosCUR );
			} else {
				PosREVIVE = this->m_PosImmediateRivive;
			}

// (20070117:남병철) : 클랜전 진행시 INI에서 지정한 특정 위치에서 부활 기능 제거 (맵 설정대로 그냥 둠)
#ifdef __CLAN_WAR_SET
#ifdef __CLAN_WAR_EXT

			// 죽은 위치에서 가장 가까운 아군 부활 위치에서 부활
			tPOINTF ptTempRevivePos, ptDeadPos;

			ptTempRevivePos.x = 0.f;
			ptTempRevivePos.y = 0.f;
			ptDeadPos = this->Get_CurPOSITION();
			if( CClanWar::GetInstance()->GetRevivePos( this, ptDeadPos, ptTempRevivePos ) )
			{
				// 최근거리 부활 성공할때만 세팅
				PosREVIVE = ptTempRevivePos;
			}

#else
			//	클래전의 경우 부활 위치 지정은 서버에서 한다.
			//	작성자 : 김영환
			Temp_List_N = GF_Clan_War_Zone_Is((DWORD)nZoneNO);
			//ZTRACE( "클랜전 방번호:%d", Temp_List_N );

			if(Temp_List_N > 0)
			{
				//	클랜전 존이다. 팀 부분해서 부활 위치 지정한다.
				GF_Get_ClanWar_Revival_Pos(Temp_List_N,this->Get_TeamNO(),PosREVIVE.x,PosREVIVE.y);
				//ZTRACE( "클랜전 INI파일 참조" );
			}
#endif
#endif

			PosREVIVE.x += ( RANDOM(1001) - 500 );	// 랜덤 5미터..
			PosREVIVE.y += ( RANDOM(1001) - 500 );

			//ZTRACE( "ID : %s, ZONE : %d, 부활위치 (X:%4.1f,Y:%4.1f)", this->Get_NAME(), nZoneNO, PosREVIVE.x, PosREVIVE.y );
			// 2%더 경치를 얻어야 한다.
			//if ( bApplyPenalty && 0 == ZONE_PVP_STATE( this->m_nZoneNO ) ) {
			//	// 경험치 패널치 적용.
			//	this->Set_PenalEXP( PENALTY_EXP_FIELD-PENALTY_EXP_TOWN );
			//}
			break;

		case REVIVE_TYPE_CURRENT_POS :	// 현재 장소에서...
			nZoneNO   = this->m_nZoneNO;
			PosREVIVE = this->m_PosCUR;
			break;

		case REVIVE_TYPE_START_POS :	// 현재존의 시작위치에서
			nZoneNO	  = this->m_nZoneNO;
			PosREVIVE = this->GetZONE()->Get_StartPOS ();
			PosREVIVE.x += ( RANDOM(1001) - 500 );	// 랜덤 5미터..
			PosREVIVE.y += ( RANDOM(1001) - 500 );
			break;

		default :
			return RET_FAILED;
	}

	// zone server ip ...
	if ( bApplyPenalty ) {
		#define	STB_ING_REVIVE_ROW		57	
		if ( ZONE_PVP_STATE( this->m_nZoneNO ) ) {
			// PVP존이면 10FPS * 30초간 무적...
			this->m_IngSTATUS.UpdateIngSTATUS( this, STB_ING_REVIVE_ROW, 30, 1, 0 );
		}

		// 최대 HP의 30% 
		this->Set_HP( 3*this->GetCur_MaxHP() / 10 );
	}

	// 2005. 09. 09 죽었다 살아나는 경우는 존에 입장시 과금 체크 안하도록...
	return this->Proc_TELEPORT( nZoneNO, PosREVIVE, bSkipPayment );
}
/// 부활 존 변경 패킷
bool classUSER::Recv_cli_SET_REVIVE_POS ()
{
	if ( !this->GetZONE() ) {
		assert( 0  && "Recv_cli_SET_REVIVE_POS" );
		return false;
	}

	this->m_nReviveZoneNO = this->m_nZoneNO;
	this->m_PosREVIVE = this->GetZONE()->Get_RevivePOS( this->m_PosCUR );

	return true;
/*
	tagEVENTPOS *pEventPOS;

	pEventPOS = g_pZoneLIST->Get_NearstEventPOS( this->m_nZoneNO, ZONE_REVIVE_POS( this->m_nZoneNO ), this->m_PosCUR );
	if ( pEventPOS ) {
		this->m_nReviveZoneNO = this->m_nZoneNO;
		this->m_PosREVIVE = pEventPOS->m_Position;
		return true;
	}

	return false;
*/
}

bool classUSER::Recv_cli_SET_VAR_REQ( t_PACKET *pPacket )
{
	/// 클라이언트에서 사용하지 않음으로 해킹이다.
	IS_HACKING( this, "classUSER::cli_SET_VAR_REQ( Script hacking.. )" );
	return false;
/*
	int iValue = pPacket->m_cli_SET_VAR_REQ.m_iValue;

	switch( pPacket->m_cli_SET_VAR_REQ.m_btVarTYPE ) {
		case SV_FAME :		this->SetCur_FAME (iValue);		break;
		case SV_UNION :		this->SetCur_UNION (iValue);	break;

//		case SV_SEX :		this->Set_SEX (iValue);			break;
//		case SV_LEVEL :		this->Set_LEVEL (iValue);		break;
//		case SV_GRADE :		g_pAVATAR->Set_RANK (iValue);	break;

		case SV_CLASS :		this->SetCur_JOB (iValue);		break;
//		기본 능력치는 클라이언트 임의로 바꿀수 없다.
//		case SV_STR :		this->SetDef_STR (iValue);		break;
//		case SV_DEX :		this->SetDef_DEX (iValue);		break;
//		case SV_WIS :		this->SetDef_INT (iValue);		break;
//		case SV_CON :		this->SetDef_CON (iValue);		break;
//		case SV_CHARM :		this->SetDef_CHARM (iValue);	break;
//		case SV_EMOTION :	this->SetDef_SENSE (iValue);	break;
		default :
			return false;
	}

	classPACKET *pCPacket = ::Packet_AllocNLock ();

	pCPacket->m_HEADER.m_wType = GSV_SET_VAR_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_VAR_REPLY );

	// 적절치 못한 값일경우 VarTYPE에 REPLY_GSV_SET_VAR_FAIL_BIT or 시켜 리턴한다.
	pCPacket->m_gsv_SET_VAR_REPLY.m_btVarTYPE = pPacket->m_cli_SET_VAR_REQ.m_btVarTYPE;
	pCPacket->m_gsv_SET_VAR_REPLY.m_iValue	  = iValue;

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
*/
}


//-------------------------------------------------------------------------------------------------
/// 걷기/뛰기, 카트 타기/내리기 상태 변경
bool classUSER::Recv_cli_TOGGLE( t_PACKET *pPacket )
{
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() ) return true;
	return this->SetCMD_TOGGLE( pPacket->m_cli_TOGGLE.m_btTYPE );
}
/// 특정 모션을 취한다는 패킷( 감정관련 동작등... )
bool classUSER::Recv_cli_SET_MOTION( t_PACKET *pPacket )
{
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() ) return true;


	if(!Is_Stop()) 
	{
		SetCMD_STOP();
		return true;   //현재 상태가 멈춤이 아니면 ..리턴..
	}

	this->Send_gsv_SET_MOTION( pPacket->m_cli_SET_MOTION.m_wValue, pPacket->m_cli_SET_MOTION.m_nMotionNO );
	return true;
}

//-------------------------------------------------------------------------------------------------
/// 동맹 채팅
short classUSER::Recv_cli_ALLIED_CHAT( t_PACKET *pPacket )
{
    char *szMsg;
    short nOffset=sizeof( cli_CHAT );

    szMsg = Packet_GetStringPtr( pPacket, nOffset );
    if ( szMsg ) {
		if ( szMsg[ 0 ] == '/' ) {
			if ( this->Check_CheatCODE( szMsg ) ) {
				return ( !this->GetZONE() ? RET_SKIP_PROC : RET_OK );
			}
		} else
		if ( !this->m_IngSTATUS.IsSET( FLAG_SUB_STORE_MODE ) ) {
			if ( szMsg[ 0 ] == '^' ) {
				// 타이틀 설정
				if ( strlen( &szMsg[1] ) >= MAX_USER_TITLE ) {
					strncpy( this->m_szUserTITLE, &szMsg[1], MAX_USER_TITLE );
					this->m_szUserTITLE[ MAX_USER_TITLE ] = 0;
				} else {
					strcpy( this->m_szUserTITLE, &szMsg[1] );
				}

				this->m_IngSTATUS.SetSubFLAG( FLAG_SUB_INTRO_CHAT );
			} else 
			if ( this->m_IngSTATUS.IsSubSET( FLAG_SUB_INTRO_CHAT ) ) {
				this->m_IngSTATUS.ClearSubFLAG( FLAG_SUB_INTRO_CHAT );
			}
		}

		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( pCPacket ) {
			pCPacket->m_HEADER.m_wType = GSV_ALLIED_CHAT;
			pCPacket->m_HEADER.m_nSize = sizeof( gsv_ALLIED_CHAT );
			pCPacket->m_gsv_ALLIED_CHAT.m_wObjectIDX = this->Get_INDEX();
			pCPacket->m_gsv_ALLIED_CHAT.m_iTeamNO = this->Get_TeamNO();
			pCPacket->AppendString( szMsg );

			this->GetZONE()->SendTeamPacketToSectors( this, pCPacket, this->Get_TeamNO() );
			Packet_ReleaseNUnlock( pCPacket );
		}
    }

    return RET_OK;
}

//-------------------------------------------------------------------------------------------------
/// 동맹 외치기
short classUSER::Recv_cli_ALLIED_SHOUT( t_PACKET *pPacket )
{
    char *szMsg;
    short nOffset=sizeof( cli_SHOUT );

    szMsg = Packet_GetStringPtr( pPacket, nOffset );
    if ( szMsg ) {
		if ( szMsg[ 0 ] == '/' ) {
			if ( this->Check_CheatCODE( szMsg ) ) {
				return ( !this->GetZONE() ? RET_SKIP_PROC : RET_OK );
			}
		}

		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( pCPacket ) {
			pCPacket->m_HEADER.m_wType = GSV_ALLIED_SHOUT;
			pCPacket->m_HEADER.m_nSize = sizeof( gsv_ALLIED_SHOUT );
			pCPacket->m_gsv_ALLIED_SHOUT.m_iTeamNO = this->Get_TeamNO();
			pCPacket->AppendString( this->Get_NAME() );
			pCPacket->AppendString( szMsg );

			this->GetZONE()->SendShout( this, pCPacket, this->Get_TeamNO() );
			Packet_ReleaseNUnlock( pCPacket );
		}
    }

    return RET_OK;
}

//-------------------------------------------------------------------------------------------------
/// 일반 채팅
short classUSER::Recv_cli_CHAT( t_PACKET *pPacket,WORD wPHeader)
{
    char *szMsg;

    short nOffset=sizeof( cli_CHAT );
	
	if(pPacket->m_HEADER.m_nSize > 135)
		pPacket->m_HEADER.m_nSize = 135;

	pPacket->m_pDATA[135] = '\0';

    szMsg = Packet_GetStringPtr( pPacket, nOffset );
    if ( szMsg ) {
		this->LogCHAT( szMsg, NULL , "CHAT" );
		if ( szMsg[ 0 ] == '/' ) {
			if ( this->Check_CheatCODE( szMsg ) ) {
				return ( !this->GetZONE() ? RET_SKIP_PROC : RET_OK );
			}
		} else
		if ( !this->m_IngSTATUS.IsSubSET( FLAG_SUB_STORE_MODE ) ) {
			if ( szMsg[ 0 ] == '^' ) {
				// Set title
				if ( strlen( &szMsg[1] ) >= MAX_USER_TITLE ) {
					strncpy( this->m_szUserTITLE, &szMsg[1], MAX_USER_TITLE );
					this->m_szUserTITLE[ MAX_USER_TITLE ] = 0;
				} else {
					strcpy( this->m_szUserTITLE, &szMsg[1] );
				}

				this->m_IngSTATUS.SetSubFLAG( FLAG_SUB_INTRO_CHAT );
			} else 
			if ( this->m_IngSTATUS.IsSubSET( FLAG_SUB_INTRO_CHAT ) ) {
				this->m_IngSTATUS.ClearSubFLAG( FLAG_SUB_INTRO_CHAT );
			}
		}

		this->Send_gsv_CHAT( szMsg ,wPHeader);
    }

    return RET_OK;
}





//-------------------------------------------------------------------------------------------------
/// 일반 외치기...
short classUSER::Recv_cli_SHOUT( t_PACKET *pPacket )
{
//	if ( this->m_dwPayFLAG & 
    char *szMsg;
    short nOffset=sizeof( cli_SHOUT );

    szMsg = Packet_GetStringPtr( pPacket, nOffset );
    if ( szMsg ) {
		this->LogCHAT( szMsg, NULL , "SHOUT" );
		if ( szMsg[ 0 ] == '/' ) {
			if ( this->Check_CheatCODE( szMsg ) ) {
				return ( !this->GetZONE() ? RET_SKIP_PROC : RET_OK );
			}
		}

		this->Send_gsv_SHOUT ( szMsg );
    }

    return RET_OK;
}

//-------------------------------------------------------------------------------------------------
/// 귓속말
bool classUSER::Recv_cli_WHISPER( t_PACKET *pPacket )
{
    char *szDest, *szMsg;
    short nOffset=sizeof( cli_WHISPER );

    szDest = Packet_GetStringPtr( pPacket, nOffset );
    szMsg  = Packet_GetStringPtr( pPacket, nOffset );

	if ( NULL == szMsg || 0 == szMsg[ 0 ] )
		return true;

    if ( szDest && szMsg ) {
        // 월드 서버에 귓속말 메세지 전송.
		classUSER *pUser = g_pUserLIST->Find_CHAR( szDest );
		if ( pUser ) {
			this->LogCHAT( szMsg , pUser->Get_NAME(), "WHISPER" );
			pUser->Send_gsv_WHISPER( this->Get_NAME(), szMsg );
		} else {
			this->LogCHAT( szMsg , NULL, "WHISPER");
			this->Send_gsv_WHISPER( szDest, NULL );
		}
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
/// 파티채팅
short classUSER::Recv_cli_PARTY_CHAT( t_PACKET *pPacket )
{
    char *szMsg;
    short nOffset=sizeof( cli_PARTY_CHAT );

    szMsg = Packet_GetStringPtr( pPacket, nOffset );
    if ( szMsg ) {
		this->LogCHAT( szMsg , NULL, "PARTY_CHAT");
		if ( szMsg[ 0 ] == '/' ) {
			if ( this->Check_CheatCODE( szMsg ) ) {
				return ( !this->GetZONE() ? RET_SKIP_PROC : RET_OK );
			}
		}

        classPACKET *pCPacket = Packet_AllocNLock ();
		//////////////////////////////////////////////////////////////////////////
		// 2006.06.12:이성활:pCPacket에 대한 Null 체크
		if (pCPacket) {
	        pCPacket->m_HEADER.m_wType = GSV_PARTY_CHAT;
			pCPacket->m_HEADER.m_nSize = sizeof( gsv_PARTY_CHAT );
			pCPacket->m_gsv_PARTY_CHAT.m_wObjectIDX = this->Get_INDEX();
			pCPacket->AppendString( szMsg );

			this->SendPacketToPARTY( pCPacket );
			Packet_ReleaseNUnlock( pCPacket );
		}
		//////////////////////////////////////////////////////////////////////////
    }

	return RET_OK;
}

//-------------------------------------------------------------------------------------------------
/// 정지 명령
bool classUSER::Recv_cli_STOP( t_PACKET *pPacket )
{
	this->SetCMD_STOP ();
    return true;
}

//-------------------------------------------------------------------------------------------------
/// 공격 명령
bool classUSER::Recv_cli_ATTACK( t_PACKET *pPacket )
{
	if ( !(this->m_dwPayFLAG & PLAY_FLAG_BATTLE) ) {
		// 공격 못해... 돈내야함
		return true;
	}

//	if ( this->Get_ActiveSKILL() ) return true;	// 스킬 케스팅 중일땐 못바꿔
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() || this->Get_WeightRATE() >= WEIGHT_RATE_CANT_ATK ) {
		// 무겁다.. 명령 불가...
		return true;
	}

	// 탑승 모드이고 암즈 아이템에 공격거리가 있어야 한다.
	if ( this->GetCur_MOVE_MODE() == MOVE_MODE_DRIVE ) {
		// 공격 무기가 있냐 ?
		if ( m_RideITEM[ RIDE_PART_ARMS ].m_nItemNo <= 0 || PAT_ITEM_ATK_RANGE( m_RideITEM[ RIDE_PART_ARMS ].m_nItemNo ) <= 0 )
			return true;

		// 카트/캐슬기어 탑승중이고 연료가 있냐 ?
		tagITEM *pItem = &m_Inventory.m_ItemRIDE[ RIDE_PART_ENGINE ];
		if ( pItem->GetLife() <=0 )
			return true;
	} else
	if ( this->GetCur_MOVE_MODE() > MOVE_MODE_DRIVE ) {
		return true;
	} else
	if ( this->m_pShotITEM ) {
		// 소모탄 필요시 갯수 체크...
		if ( this->m_pShotITEM->GetQuantity() < 1 ) {
			return true;
		}
	}

	this->SetCMD_ATTACK( pPacket->m_cli_ATTACK.m_wTargetObjectIndex );
    return true;
}

//-------------------------------------------------------------------------------------------------
bool classUSER::Recv_cli_DAMAGE( t_PACKET *pPacket )
{
/*
	m_pSendPacket->m_wType = GSV_DAMAGE;
	m_pSendPacket->m_nSize = sizeof( gsv_DAMAGE );
	m_pSendPacket->m_gsv_DAMAGE.m_wAtkObjIDX = g_pObjMGR->Get_ServerObjectIndex( pAtkOBJ->m_nIndex );
	m_pSendPacket->m_gsv_DAMAGE.m_wDefObjIDX = g_pObjMGR->Get_ServerObjectIndex( pDefOBJ->m_nIndex );
	m_pSendPacket->m_gsv_DAMAGE.m_nDamage = nDamage;
*/
    return true;
}

//-------------------------------------------------------------------------------------------------
/// 이동 또는 아이템 습득 명령
bool classUSER::Recv_cli_MOUSECMD( t_PACKET *pPacket )
{
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() ) return true;

	// 카트/캐슬기어 탑승중이고 연료가 있냐 ?
	if ( this->Get_RideMODE() ) {
		if ( this->GetCur_MOVE_MODE() > MOVE_MODE_DRIVE ) {
			// 얻어 타고 있는넘이다.
			return true;
		}

		tagITEM *pItem = &m_Inventory.m_ItemRIDE[ RIDE_PART_ENGINE ];
		if ( pItem->GetLife() <=0 )
			return true;
	}

	int iDistance = distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)pPacket->m_cli_MOUSECMD.m_PosTO.x, (int)pPacket->m_cli_MOUSECMD.m_PosTO.y);
	if ( iDistance > 1000 * 15 )	// 150 m 
		return true;

	this->m_nPosZ = pPacket->m_cli_MOUSECMD.m_nPosZ;

	this->SetCMD_MOVE( this->m_PosCUR, pPacket->m_cli_MOUSECMD.m_PosTO, pPacket->m_cli_MOUSECMD.m_wTargetObjectIDX );
	return true;
}

//-------------------------------------------------------------------------------------------------
/// 클라이언트가 장애물에 부딛쳐 더이상 이동 못할때...
bool classUSER::Recv_cli_CANTMOVE( t_PACKET *pPacket )
{
	int iCliDist = ::distance ((int)m_PosMoveSTART.x, (int)m_PosMoveSTART.y, (int)pPacket->m_cli_CANTMOVE.m_PosCUR.x, (int)pPacket->m_cli_CANTMOVE.m_PosCUR.y);
	if ( iCliDist > m_iMoveDistance ) {
		// 서버에 설정된 최종 위치보다 더 멀리 갈수 있나 ????
		return true;// false;
	}

	int iSrvDist;
/*
	iSrvDist = ::distance ((int)m_PosMoveSTART.x, (int)m_PosMoveSTART.y, (int)m_PosCUR.x, (int)m_PosCUR.y);
	if ( iSrvDist >= iCliDist ) {
*/
	this->m_nPosZ = pPacket->m_cli_MOUSECMD.m_nPosZ;

	iSrvDist = ::distance ((int)m_PosGOTO.x, (int)m_PosGOTO.y, (int)m_PosCUR.x, (int)m_PosCUR.y);
	iCliDist = ::distance ((int)m_PosGOTO.x, (int)m_PosGOTO.y, (int)pPacket->m_cli_CANTMOVE.m_PosCUR.x, (int)pPacket->m_cli_CANTMOVE.m_PosCUR.y);
	if ( iCliDist > iSrvDist ) {
		// 최종 목적지에서 더 멀다...
		this->m_PosGOTO = pPacket->m_cli_CANTMOVE.m_PosCUR;
		// 클라이언트에서 정지 위치로 강제 이동 시킨다.
		this->m_PosCUR = pPacket->m_cli_CANTMOVE.m_PosCUR;
		// TODO:: update sector ???

		// 서버가 클라이언트에서 막힌 위치 보다 더 멀이 이동했다.
		// 더이상 이동 못하도록...
		m_iMoveDistance = -1;
		m_MoveVEC.x		= 0;
		m_MoveVEC.y		= 0;

		if ( !CObjAI::SetCMD_STOP () )
			return true;
	} else {
		// 이동할 최종 좌표 수정.
		m_iMoveDistance = iCliDist;
		// 공격 명령일 경우도 이동 명령후 정지로 ...
		if ( !CObjAI::SetCMD_MOVE( this->m_PosCUR, pPacket->m_cli_SETPOS.m_PosCUR, 0 ) )
			return true;
	}

	// 보정 좌표 전송.
	return this->Send_gsv_ADJUST_POS ();
}

//-------------------------------------------------------------------------------------------------
/// 클라이언트의 무게 비율이 변동되었을경우...
bool classUSER::Recv_cli_SET_WEIGHT_RATE( BYTE btWeightRate/*t_PACKET *pPacket*/, bool bSend2Around )
{
	this->m_btWeightRate = btWeightRate; /* pPacket->m_cli_SET_WEIGHT_RATE.m_btWeightRate; */

	if ( this->m_btWeightRate >= WEIGHT_RATE_WALK ) {
		// 뛰기 불능.
		this->m_bRunMODE = false;

		// 다시 이동시 이동속도 계산된다, 현재 이동 상태는 지속..
		//if ( this->m_btWeightRate >= WEIGHT_RATE_CANT_ATK ) {
		//	this->Del_ActiveSKILL ();
		//	CObjAI::SetCMD_STOP ();
		//}
// 
//-------------------------------------
/*
		if ( this->GetCur_MOVE_MODE() > MOVE_MODE_RUN && this->GetOri_RunSPEED() > 300 ) {
			this->m_nRunSPEED = 300;
		}
*/
// 2006.07.20/김대성/수정 - 카트스피드 제한
#ifdef __PASS_LIMIT_CARTSPEED
#else
		if ( this->GetCur_MOVE_MODE() > MOVE_MODE_RUN && this->GetOri_RunSPEED() > 300 ) {
			this->m_nRunSPEED = 300;
		}
#endif
//-------------------------------------
	}

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_SET_WEIGHT_RATE;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_WEIGHT_RATE );
	pCPacket->m_gsv_SET_WEIGHT_RATE.m_wObjectIDX   = this->Get_INDEX();
	pCPacket->m_gsv_SET_WEIGHT_RATE.m_btWeightRate = this->m_btWeightRate;

	if ( bSend2Around ) {
		this->GetZONE()->SendPacketToSectors( this, pCPacket );
	} else {
		this->SendPacket( pCPacket );
	}
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}


//-------------------------------------------------------------------------------------------------
/// 클라이언트가 위치 보정을 요청 했을때...
bool classUSER::Recv_cli_SETPOS( t_PACKET *pPacket )
{
	int iCliDist;

	iCliDist = ::distance ((int)m_PosMoveSTART.x, (int)m_PosMoveSTART.y, (int)pPacket->m_cli_SETPOS.m_PosCUR.x, (int)pPacket->m_cli_SETPOS.m_PosCUR.y);
	if ( iCliDist > m_iMoveDistance ) {
		// 서버에 설정된 최종 위치보다 더 멀리 갈수 있나 ????
		// 짤라라 !!!
		return false;
	}

	// 스피드핵이 사용되고 있는가 ???
	int iSrvDist = ::distance ((int)m_PosMoveSTART.x, (int)m_PosMoveSTART.y, (int)m_PosCUR.x, (int)m_PosCUR.y);
	if ( iCliDist >= iSrvDist+1000 ) {
		// iCliDist > iSrvDist보다 큰경우는 서버에 부하가 발생하거나 스프드핵...
		this->Send_gsv_ADJUST_POS ();
	} else
	if ( iSrvDist >= iCliDist+1000 ) {
		// 네트웍 랙이 발생한 경우는 iSrvDist > iCliDist ...
		// 서버가 클라이언트보다 10m 더 갔다.
		this->Send_gsv_ADJUST_POS ();
	}

	return true;
}


//-------------------------------------------------------------------------------------------------
/// 얼굴, 머리털, 성별 변화등 주위에 통보~
bool classUSER::Send_gsv_CHANGE_SKIN( WORD wAbilityTYPE, int iValue )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	//////////////////////////////////////////////////////////////////////////
	// 2006.06.12:이성활:pCPacket에 대한 Null 체크
	if (!pCPacket) {
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	pCPacket->m_HEADER.m_wType = GSV_CHANGE_SKIN;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_CHANGE_SKIN );

	pCPacket->m_gsv_CHANGE_SKIN.m_wObjectIDX	= this->Get_INDEX();
	pCPacket->m_gsv_CHANGE_SKIN.m_wAbilityTYPE  = wAbilityTYPE;
	pCPacket->m_gsv_CHANGE_SKIN.m_iValue		= iValue;

	this->GetZONE()->SendPacketToSectors( this, pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 장착 아이템이 변경 되었을때...
bool classUSER::Send_gsv_EQUIP_ITEM (short nEquipInvIDX, tagITEM *pEquipITEM)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_EQUIP_ITEM;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_EQUIP_ITEM );
	pCPacket->m_gsv_EQUIP_ITEM.m_wObjectIDX  = this->Get_INDEX();

	pCPacket->m_gsv_EQUIP_ITEM.m_nEquipIndex = nEquipInvIDX;
	pCPacket->m_gsv_EQUIP_ITEM.m_PartITEM.m_nItemNo	  = pEquipITEM->GetItemNO();
	pCPacket->m_gsv_EQUIP_ITEM.m_PartITEM.m_nGEM_OP	  = pEquipITEM->GetGemNO();
	pCPacket->m_gsv_EQUIP_ITEM.m_PartITEM.m_cGrade	  = pEquipITEM->GetGrade();
	pCPacket->m_gsv_EQUIP_ITEM.m_PartITEM.m_bHasSocket= pEquipITEM->m_bHasSocket;

	if ( 0 == this->GetCur_RIDE_MODE() ) {
		pCPacket->m_HEADER.m_nSize += sizeof( short );
		pCPacket->m_gsv_EQUIP_ITEM.m_nRunSPEED[0] = this->GetOri_RunSPEED();	// 패시브에의해서만 보정/ 지속보정 제외된값.
	}

	this->GetZONE()->SendPacketToSectors( this, pCPacket );
    Packet_ReleaseNUnlock( pCPacket );
	return true;
}

bool classUSER::Send_gsv_EQUIP_ITEM (short nEquipInvIDX)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	short nPartNO = s_nEquipIdxToPartNO[ nEquipInvIDX ];

	if(nPartNO<0||nPartNO>=10)
		return false;


	 tagPartITEM *pEquipITEM = &m_PartITEM[nPartNO];


	pCPacket->m_HEADER.m_wType = GSV_EQUIP_ITEM;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_EQUIP_ITEM );
	pCPacket->m_gsv_EQUIP_ITEM.m_wObjectIDX  = this->Get_INDEX();



	pCPacket->m_gsv_EQUIP_ITEM.m_nEquipIndex = nEquipInvIDX;
	pCPacket->m_gsv_EQUIP_ITEM.m_PartITEM.m_nItemNo	  = pEquipITEM->m_nItemNo;
	pCPacket->m_gsv_EQUIP_ITEM.m_PartITEM.m_nGEM_OP	  = pEquipITEM->m_nGEM_OP;
	pCPacket->m_gsv_EQUIP_ITEM.m_PartITEM.m_cGrade	  = pEquipITEM->m_cGrade;
	pCPacket->m_gsv_EQUIP_ITEM.m_PartITEM.m_bHasSocket= pEquipITEM->m_bHasSocket;

	if ( 0 == this->GetCur_RIDE_MODE() ) {
		pCPacket->m_HEADER.m_nSize += sizeof( short );
		pCPacket->m_gsv_EQUIP_ITEM.m_nRunSPEED[0] = this->GetOri_RunSPEED();	// 패시브에의해서만 보정/ 지속보정 제외된값.
	}

	this->GetZONE()->SendPacketToSectors( this, pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	return true;
}


//-------------------------------------------------------------------------------------------------
/// 장착 아이템 변경
bool classUSER::Change_EQUIP_ITEM (short nEquipInvIDX, short nWeaponInvIDX)
{
	if ( nEquipInvIDX < 1 || nEquipInvIDX >= MAX_EQUIP_IDX )
		return false;

	// 양손무기 장착시에 왼손 방패 장착할시에는 양손 무기를 내리지 않는다.
	// 장비 탈거는 묵인...
	if ( EQUIP_IDX_WEAPON_L == nEquipInvIDX && nWeaponInvIDX ) {
		if ( m_Inventory.m_ItemLIST[ EQUIP_IDX_WEAPON_R ].IsTwoHands() ) {
			// 클라이언트에서 걸러져 오지 못하는 경우도 있네...
			return true;
		}
	}

	BYTE btCurCON = this->GetCur_CON();
	BYTE btRecvHP = this->m_btRecoverHP;
	BYTE btRecvMP = this->m_btRecoverMP;

	bool bResult = true;
	// 바뀐 인벤토리 구조 전송..
	classPACKET *pCPacket = Packet_AllocNLock ();
	//////////////////////////////////////////////////////////////////////////
	// 2006.06.12:이성활:pCPacket에 대한 Null 체크
	if (!pCPacket) {
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT = 0;
	tagITEM *pEquipITEM = &m_Inventory.m_ItemLIST[ nEquipInvIDX ];
	if ( nWeaponInvIDX == 0 ) {
		// 장비 탈거...
		if ( pEquipITEM->GetTYPE() && pEquipITEM->GetTYPE() < ITEM_TYPE_USE ) {
			short nInvIDX = this->Add_ITEM( *pEquipITEM  );
			if ( nInvIDX > 0 ) {
				pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT = 2;

				pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_btInvIDX = (BYTE)nEquipInvIDX;
				pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_ITEM.Clear();

				pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_btInvIDX =  (BYTE)nInvIDX;
				pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_ITEM     = *pEquipITEM;

				// 순서 주의 밑에꺼 호출되면 *pEquipITEM이 삭제된다.
				this->ClearITEM( nEquipInvIDX );

				if ( EQUIP_IDX_WEAPON_R == nEquipInvIDX ) {
					// 오른손 무기중 소모탄 해제...
					this->m_pShotITEM = NULL;
				}
			} else {
				// 빈 인벤토리가 없어서 장비를 벗을수 없다...
				goto _RETURN;
			}
		} else {
			#pragma COMPILE_TIME_MSG( "2004. 7. 16 4차 클베과정에서 몇몇 유저에게 나타나는 현상 임시로 ..." )
			// IS_HACKING( this, "Change_EQUIP_ITEM-1" );	// 뭐냐 ???
			// bResult = false;
			goto _RETURN;
		}
	} else {
		// 장비 장착...
		tagITEM WeaponITEM = m_Inventory.m_ItemLIST[ nWeaponInvIDX ];
		if ( this->Check_EquipCondition( WeaponITEM, nEquipInvIDX ) && WeaponITEM.IsEquipITEM() ) {
			if ( WeaponITEM.IsTwoHands() && m_Inventory.m_ItemEQUIP[ EQUIP_IDX_WEAPON_L ].GetTYPE() ) {
				// 양손 무기이고 왼손에 방패 있다면 
				// 왼손 무기를 탈거할 장비 인벤토리에 빈 공간이 1개 있어야 한다.
				short nEmptyInvIDX = m_Inventory.GetEmptyInventory( INV_WEAPON );
				if ( nEmptyInvIDX < 0 )
					goto _RETURN;

				// 왼손무기 인벤토리로...
				tagITEM *pSubWPN = &m_Inventory.m_ItemLIST[ EQUIP_IDX_WEAPON_L ];
				m_Inventory.m_ItemLIST[ nEmptyInvIDX ] = *pSubWPN;

				// 왼손 장착 아이템 삭제.. 
				pSubWPN->Clear();
				this->SetPartITEM( BODY_PART_WEAPON_L, *pSubWPN );		// 왼손 모델 아이템 강제로 삭제 !!!
				this->Send_gsv_EQUIP_ITEM( BODY_PART_WEAPON_L );

				pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT = 4;

				// 왼손 무기 삭제...
				pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 2 ].m_btInvIDX = EQUIP_IDX_WEAPON_L;
				pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 2 ].m_ITEM.Clear();

				// 장착 장비 들어간 인벤토리 정보 전송.
				pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 3 ].m_btInvIDX = (BYTE)nEmptyInvIDX;
				pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 3 ].m_ITEM     = m_Inventory.m_ItemLIST[ nEmptyInvIDX ];

				// 양손무기 장착이므로 :: 방어력증가, 항마력증가, 방패데이지 삭제...
				this->m_IngSTATUS.ClearSTATUS( ING_INC_DPOWER );
				this->m_IngSTATUS.ClearSTATUS( ING_INC_RES );
				this->m_IngSTATUS.ClearSTATUS( ING_SHIELD_DAMAGE );
			} else {
				// 일반 장비 & 한손 무기
				pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT = 2;
			}

			m_Inventory.m_ItemLIST[ nWeaponInvIDX ] = *pEquipITEM;
			*pEquipITEM = WeaponITEM;

			// 장비 교환.
			pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_btInvIDX = (BYTE)nEquipInvIDX;
			pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_ITEM     = m_Inventory.m_ItemLIST[ nEquipInvIDX ];

			pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_btInvIDX = (BYTE)nWeaponInvIDX;
			pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_ITEM     = m_Inventory.m_ItemLIST[ nWeaponInvIDX ];

			if ( EQUIP_IDX_WEAPON_R == nEquipInvIDX ) {
				// 오른손 무기중 소모탄 사용하나 ???
				this->Set_ShotITEM ();			// classUSER::Change_EQUIP_ITEM 
			}
		} // else 장비 아이템이 아니다...
	}


	if ( pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT ) {
		// 무기 변경시
		switch( nEquipInvIDX ) {
			case EQUIP_IDX_WEAPON_R :
				// 공격력증가, 명중력증가, 공격속도증가, 크리티컬증가 삭제...
				this->m_IngSTATUS.ClearStatusIfENABLED( ING_INC_APOWER );
				this->m_IngSTATUS.ClearStatusIfENABLED( ING_INC_HIT );
				this->m_IngSTATUS.ClearStatusIfENABLED( ING_INC_ATK_SPD );
				this->m_IngSTATUS.ClearStatusIfENABLED( ING_INC_CRITICAL );
//-------------------------------------
// 2006.09.28/김대성/추가 - 무기 변경시 추가데미지 상태효과 제거 (발키리서포트)
				this->m_IngSTATUS.ClearStatusIfENABLED( ING_DUMMY_DAMAGE );		// 추가데미지
//-------------------------------------
				break;

			case EQUIP_IDX_WEAPON_L :
				// 방어력증가, 항마력증가, 방패데이지 삭제...
				this->m_IngSTATUS.ClearStatusIfENABLED( ING_INC_DPOWER );
				this->m_IngSTATUS.ClearStatusIfENABLED( ING_INC_RES );
				this->m_IngSTATUS.ClearStatusIfENABLED( ING_SHIELD_DAMAGE );
				break;
		}

		// 케릭터 상태 업데이트.. 파트는 바뀌지 않는다..
		this->SetPartITEM( nEquipInvIDX );
//-------------------------------------
/*
		this->Send_gsv_EQUIP_ITEM( nEquipInvIDX );


		pCPacket->m_HEADER.m_wType = GSV_SET_INV_ONLY;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_INV_ONLY ) + pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT * sizeof( tag_SET_INVITEM );
		this->SendPacket( pCPacket );
*/
// 2006.05.30/김대성/수정 - 패킷순서를 바꾼다.- 아이템 탈부착시 마나 제대로 적용 안�?
		pCPacket->m_HEADER.m_wType = GSV_SET_INV_ONLY;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_INV_ONLY ) + pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT * sizeof( tag_SET_INVITEM );
		this->SendPacket( pCPacket );
		this->Send_gsv_EQUIP_ITEM( nEquipInvIDX );

		this->UpdateAbility ();		// 캐릭터 능력치 갱신
		Send_CHAR_HPMP_INFO(0);		// HP, MP정보를 한번 더 보내준다.
//-------------------------------------

		// 주변에 무기 바뀜을 통보...

		if ( this->GetPARTY() ) {
			// 변경에 의해 옵션이 붙어 회복이 바뀌면 파티원에게 전송.
			if ( btCurCON != this->GetCur_CON()  ||
				 btRecvHP != this->m_btRecoverHP ||
				 btRecvMP != this->m_btRecoverMP ) {
				this->m_pPartyBUFF->Change_ObjectIDX( this );
			}
		}
	}

_RETURN :
    Packet_ReleaseNUnlock( pCPacket );

//-------------------------------------
// 2006.03.30/김대성/추가 - HP/MP회복 악세사리의 효과가 없는 버그 수정
	this->UpdateAbility ();		// 캐릭터 능력치 갱신
//-------------------------------------

	return bResult;
}



//-------------------------------------
// 2007.01.10
//-------------------------------------------------------------------------------------------------
/// 장착 아이템 변경
bool classUSER::Change_EQUIP_ITEM2 (short nEquipInvIDX, short nWeaponInvIDX)
{
	if ( nEquipInvIDX < 1 || nEquipInvIDX >= MAX_EQUIP_IDX )
		return false;

	// 양손무기 장착시에 왼손 방패 장착할시에는 양손 무기를 내리지 않는다.
	// 장비 탈거는 묵인...
	if ( EQUIP_IDX_WEAPON_L == nEquipInvIDX && nWeaponInvIDX ) {
		if ( m_Inventory.m_ItemLIST[ EQUIP_IDX_WEAPON_R ].IsTwoHands() ) {
			// 클라이언트에서 걸러져 오지 못하는 경우도 있네...
			return true;
		}
	}

	BYTE btCurCON = this->GetCur_CON();
	BYTE btRecvHP = this->m_btRecoverHP;
	BYTE btRecvMP = this->m_btRecoverMP;

	bool bResult = true;
	// 바뀐 인벤토리 구조 전송..
	classPACKET *pCPacket = Packet_AllocNLock ();
	//////////////////////////////////////////////////////////////////////////
	// 2006.06.12:이성활:pCPacket에 대한 Null 체크
	if (!pCPacket) {
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT = 0;
	tagITEM *pEquipITEM = &m_Inventory.m_ItemLIST[ nEquipInvIDX ];
	if ( nWeaponInvIDX == 0 ) {
		// 장비 탈거...
		if ( pEquipITEM->GetTYPE() && pEquipITEM->GetTYPE() < ITEM_TYPE_USE ) {
			short nInvIDX = 1;	// 인벤토리로 옮기기 않고 착용만 해제
			if ( nInvIDX > 0 ) {
				pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT = 1;

				pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_btInvIDX = (BYTE)nEquipInvIDX;
				pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_ITEM.Clear();

				// 순서 주의 밑에꺼 호출되면 *pEquipITEM이 삭제된다.
				this->ClearITEM( nEquipInvIDX );

				if ( EQUIP_IDX_WEAPON_R == nEquipInvIDX ) {
					// 오른손 무기중 소모탄 해제...
					this->m_pShotITEM = NULL;
				}
			}
		} else {
#pragma COMPILE_TIME_MSG( "2004. 7. 16 4차 클베과정에서 몇몇 유저에게 나타나는 현상 임시로 ..." )
			// IS_HACKING( this, "Change_EQUIP_ITEM-1" );	// 뭐냐 ???
			// bResult = false;
			goto _RETURN2;
		}
	} 

	if ( pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT ) {
		// 무기 변경시
		switch( nEquipInvIDX ) {
			case EQUIP_IDX_WEAPON_R :
				// 공격력증가, 명중력증가, 공격속도증가, 크리티컬증가 삭제...
				this->m_IngSTATUS.ClearStatusIfENABLED( ING_INC_APOWER );
				this->m_IngSTATUS.ClearStatusIfENABLED( ING_INC_HIT );
				this->m_IngSTATUS.ClearStatusIfENABLED( ING_INC_ATK_SPD );
				this->m_IngSTATUS.ClearStatusIfENABLED( ING_INC_CRITICAL );
				//-------------------------------------
				// 2006.09.28/김대성/추가 - 무기 변경시 추가데미지 상태효과 제거 (발키리서포트)
				this->m_IngSTATUS.ClearStatusIfENABLED( ING_DUMMY_DAMAGE );		// 추가데미지
				//-------------------------------------
				break;

			case EQUIP_IDX_WEAPON_L :
				// 방어력증가, 항마력증가, 방패데이지 삭제...
				this->m_IngSTATUS.ClearStatusIfENABLED( ING_INC_DPOWER );
				this->m_IngSTATUS.ClearStatusIfENABLED( ING_INC_RES );
				this->m_IngSTATUS.ClearStatusIfENABLED( ING_SHIELD_DAMAGE );
				break;
		}

		// 케릭터 상태 업데이트.. 파트는 바뀌지 않는다..
		this->SetPartITEM( nEquipInvIDX );
//-------------------------------------
/*
		this->Send_gsv_EQUIP_ITEM( nEquipInvIDX );


		pCPacket->m_HEADER.m_wType = GSV_SET_INV_ONLY;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_INV_ONLY ) + pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT * sizeof( tag_SET_INVITEM );
		this->SendPacket( pCPacket );
*/
// 2006.05.30/김대성/수정 - 패킷순서를 바꾼다.- 아이템 탈부착시 마나 제대로 적용 안�?
		pCPacket->m_HEADER.m_wType = GSV_SET_INV_ONLY;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_INV_ONLY ) + pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT * sizeof( tag_SET_INVITEM );
		this->SendPacket( pCPacket );
		this->Send_gsv_EQUIP_ITEM( nEquipInvIDX );

//		this->UpdateAbility ();		// 캐릭터 능력치 갱신
//		Send_CHAR_HPMP_INFO(0);		// HP, MP정보를 한번 더 보내준다.
//-------------------------------------

		// 주변에 무기 바뀜을 통보...
		if ( this->GetPARTY() ) {
			// 변경에 의해 옵션이 붙어 회복이 바뀌면 파티원에게 전송.
			if ( btCurCON != this->GetCur_CON()  ||
				btRecvHP != this->m_btRecoverHP ||
				btRecvMP != this->m_btRecoverMP ) {
					this->m_pPartyBUFF->Change_ObjectIDX( this );
				}
		}
	}

_RETURN2 :
	Packet_ReleaseNUnlock( pCPacket );

	//-------------------------------------
	// 2006.03.30/김대성/추가 - HP/MP회복 악세사리의 효과가 없는 버그 수정
//	this->UpdateAbility ();		// 캐릭터 능력치 갱신
	//-------------------------------------

	return bResult;
}
//-------------------------------------


/// 장착 아이템 변경 요청 패킷처리
bool classUSER::Recv_cli_EQUIP_ITEM( t_PACKET *pPacket )
{
	if ( this->Get_ActiveSKILL() ) return true;	// 스킬 케스팅 중일땐 못바꿔
	if ( this->m_IngSTATUS.IsSET( FLAG_ING_IGNORE_ALL ) ) return true;

	if ( pPacket->m_cli_EQUIP_ITEM.m_nEquipInvIDX < 1 ||
		 pPacket->m_cli_EQUIP_ITEM.m_nEquipInvIDX >= MAX_EQUIP_IDX ) {
		return IS_HACKING (this, "Recv_cli_EQUIP_ITEM-1 :: Invalied Equip IDX" ); 
	}
	if ( pPacket->m_cli_EQUIP_ITEM.m_nWeaponInvIDX && 
		(pPacket->m_cli_EQUIP_ITEM.m_nWeaponInvIDX  < MAX_EQUIP_IDX ||
		 pPacket->m_cli_EQUIP_ITEM.m_nWeaponInvIDX >= MAX_EQUIP_IDX+INVENTORY_PAGE_SIZE*(1+INV_WEAPON)) ) {
		return IS_HACKING (this, "Recv_cli_EQUIP_ITEM-2 :: Invalid Weapon Inv IDX" ); 
	}

	return this->Change_EQUIP_ITEM( pPacket->m_cli_EQUIP_ITEM.m_nEquipInvIDX, pPacket->m_cli_EQUIP_ITEM.m_nWeaponInvIDX );
}

//-------------------------------------------------------------------------------------------------
#define	PAT_ITEM_INV0	( MAX_EQUIP_IDX + INV_RIDING*INVENTORY_PAGE_SIZE ) 
/// 카트/캐슬기어 아이템 변경 요청
bool classUSER::Recv_cli_ASSEMBLE_RIDE_ITEM( t_PACKET *pPacket )
{
	if ( this->Get_ActiveSKILL() ) return true;	// 스킬 케스팅 중일땐 못바꿔
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() ) return true;

	short nEquipInvIDX = pPacket->m_cli_ASSEMBLE_RIDE_ITEM.m_nRidingPartIDX;
	if ( nEquipInvIDX < 0 || nEquipInvIDX >= MAX_RIDING_PART )
		return false;

	short nFromInvIDX  = pPacket->m_cli_ASSEMBLE_RIDE_ITEM.m_nRidingInvIDX;
	if ( nFromInvIDX && ( nFromInvIDX < PAT_ITEM_INV0 || nFromInvIDX >= PAT_ITEM_INV0+INVENTORY_PAGE_SIZE ) )
		return false;

	// 이름		: 김창수
	// 날짜		: Sep. 27 2005 				
	// 설명		: 카트게이지가 0일때 PAT장비 탈착 불가
#ifdef __KCHS_BATTLECART__
	if( GetCur_PatHP() <= 0 && GetCur_PatMaxHP() > 0 ) {
		return true;
	}
#endif

	bool bResult = true;
	// 바뀐 인벤토리 구조 전송..
	classPACKET *pCPacket = Packet_AllocNLock ();
	//////////////////////////////////////////////////////////////////////////
	// 2006.06.12:이성활:pCPacket에 대한 Null 체크
	if (!pCPacket) {
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT = 0;
	tagITEM *pEquipITEM = &m_Inventory.m_ItemRIDE[ nEquipInvIDX ];
	if ( nFromInvIDX == 0 )
	{
		// 아이템 탈거
		// 승차 모드에선 할수 없다.
		if ( this->GetCur_MOVE_MODE() == MOVE_MODE_DRIVE ) return true;

		if ( ITEM_TYPE_RIDE_PART == pEquipITEM->GetTYPE() ) 
		{
			short nInvIDX = this->Add_ITEM( *pEquipITEM  );
			if ( nInvIDX > 0 ) {
				pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT = 2;

				pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_btInvIDX = (BYTE)( INVENTORY_RIDE_ITEM0 + nEquipInvIDX );
				pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_ITEM.Clear();

				pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_btInvIDX = (BYTE)nInvIDX;
				pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_ITEM     = *pEquipITEM;

				// 순서 주의 밑에꺼 호출되면 *pEquipITEM이 삭제된다.
				this->ClearITEM( INVENTORY_RIDE_ITEM0+nEquipInvIDX );
			} else {
				// 빈 인벤토리가 없어서 장비를 벗을수 없다...
				goto _RETURN;
			}
		} // else {
		//	IS_HACKING( this, "Recv_cli_ASSEMBLE_RIDE_ITEM-1" );	// 뭐냐 ???
		//	bResult = false;
		//	goto _RETURN;
		//}
	} 
	else
	{
		// 아이템 장착...
		tagITEM RideITEM = m_Inventory.m_ItemLIST[ nFromInvIDX ];
		if ( this->Check_PatEquipCondition( RideITEM, nEquipInvIDX ) ) 
		{
			pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT = 2;

			m_Inventory.m_ItemLIST[ nFromInvIDX ] = *pEquipITEM;
			*pEquipITEM = RideITEM;

			// 장비 교환.
			pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_btInvIDX = (BYTE)( INVENTORY_RIDE_ITEM0 + nEquipInvIDX );
			pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_ITEM     = m_Inventory.m_ItemRIDE[ nEquipInvIDX ];

			pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_btInvIDX = (BYTE)nFromInvIDX;
			pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_ITEM     = m_Inventory.m_ItemLIST[ nFromInvIDX ];

			this->UpdateAbility(); // 이거 안 하면.. 장비 교체 할때 카트Max PatHP 계속 0이기 때문에... 
#ifdef __KCHS_BATTLECART__
			if( GetCur_PatMaxHP() > 0 ) // 쿨타임이 돌게 할려면...
				Set_PatHP_MODE( 3 );
#endif
		} 
		/*
		else {
			IS_HACKING( this, "Recv_cli_ASSEMBLE_RIDE_ITEM-2" );	// 뭐냐 ???
			bResult = false;
			goto _RETURN;
		}
		*/
	}

	if ( pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT ) {
		pCPacket->m_HEADER.m_wType = GSV_SET_INV_ONLY;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_INV_ONLY ) + pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT * sizeof( tag_SET_INVITEM );
		this->SendPacket( pCPacket );
		Packet_ReleaseNUnlock( pCPacket );

		this->SetRideITEM( nEquipInvIDX );

		// 주변에 바뀜을 통보...
		pCPacket = Packet_AllocNLock ();
		//////////////////////////////////////////////////////////////////////////
		// 2006.06.12:이성활:pCPacket에 대한 Null 체크
		if (pCPacket) {
			pCPacket->m_HEADER.m_wType = GSV_ASSEMBLE_RIDE_ITEM;
			pCPacket->m_HEADER.m_nSize = sizeof( gsv_ASSEMBLE_RIDE_ITEM );

			pCPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_wObjectIDX		= this->Get_INDEX();
			pCPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_nRidingPartIDX = nEquipInvIDX;
		//  pCPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_nItemNO		= this->m_nRideItemIDX[ nEquipInvIDX ];
			pCPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_RideITEM		= this->m_RideITEM[ nEquipInvIDX ];

			if ( this->GetCur_RIDE_MODE() ) {
				pCPacket->m_HEADER.m_nSize += sizeof( short );
				pCPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_nRunSPEED[0] = this->GetOri_RunSPEED();	// 패시브에의해서만 보정/ 지속보정 제외된값.
			} 

			this->GetZONE()->SendPacketToSectors( this, pCPacket );
		}
		//////////////////////////////////////////////////////////////////////////
	}

_RETURN :
    Packet_ReleaseNUnlock( pCPacket );

	return bResult;
}

//-------------------------------------------------------------------------------------------------
/*
bool Get_SellITEM( short nNPCNO, short nSellTAB, short nSellCOL, tagITEM &OutITEM )
{
	short nListSellROW, nSellITEM;

	nListSellROW = NPC_SELL_TAB( nNPCNO, nSellTAB );
	if ( 0 == nListSellROW || nListSellROW >= g_TblStore.m_nDataCNT ) 
		return false;

	nSellITEM    = STORE_ITEM( nListSellROW, nSellCOL );
	if ( 0 == nSellITEM )
		return false;
	OutITEM.Clear();
	OutITEM.m_cType   = (char)(nSellITEM / 1000);
	OutITEM.m_nItemNo = nSellITEM % 1000;

	return true;
}
*/

//-------------------------------------------------------------------------------------------------
/// 이동속도 변경을 주변에 통보
bool classUSER::Send_gsv_SPEED_CHANGED (bool bUpdateSpeed)
{
	// 속도 갱신...
	if ( bUpdateSpeed )
		this->Update_SPEED ();

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_SPEED_CHANGED;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SPEED_CHANGED );

	pCPacket->m_gsv_SPEED_CHANGED.m_wObjectIDX   = this->Get_INDEX ();
	pCPacket->m_gsv_SPEED_CHANGED.m_nRunSPEED	 = this->GetOri_RunSPEED ();
	pCPacket->m_gsv_SPEED_CHANGED.m_nPsvAtkSPEED = this->GetPsv_ATKSPEED ();
	pCPacket->m_gsv_SPEED_CHANGED.m_btWeightRate = this->m_btWeightRate;

	this->GetZONE()->SendPacketToSectors( this, pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

	// 파티원 한테는 max hp/con/recover_xx ... !!!
	if ( this->GetPARTY() ) {
		this->m_pPartyBUFF->Change_ObjectIDX( this );
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
/// NPC 상점 거래응답 패킷
bool classUSER::Send_gsv_STORE_TRADE_REPLY( BYTE btResult )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	//////////////////////////////////////////////////////////////////////////
	// 2006.06.12:이성활:pCPacket에 대한 Null 체크
	if (!pCPacket) {
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	pCPacket->m_HEADER.m_wType = GSV_STORE_TRADE_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_STORE_TRADE_REPLY );

	pCPacket->m_gsv_STORE_TRADE_REPLY.m_btRESULT = btResult;

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
#define	MAX_TRADE_DISTANCE		( 1000 * 6 )	// 6 M
/// NPC 상점 거래 요청 패킷
bool classUSER::Recv_cli_STORE_TRADE_REQ( t_PACKET *pPacket )
{
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() ) return true;

	if ( NULL == this->GetZONE() )
		return false;

	//거래 승인상태, 거래 준비상태. 거래 완료상태일경우..
	if ( this->m_btTradeBIT )	return true;


	if ( pPacket->m_cli_STORE_TRADE_REQ.m_dwEconomyTIME != this->GetZONE()->GetEconomyTIME() ) {
		// 경제 데이타가 틀리다...
		return this->Send_gsv_STORE_TRADE_REPLY( STORE_TRADE_RESULT_PRICE_DIFF );
	}

	CObjNPC *pCharNPC;
	pCharNPC = (CObjNPC*)g_pObjMGR->Get_GameOBJ( pPacket->m_cli_STORE_TRADE_REQ.m_wNPCObjIdx, OBJ_NPC );
	if ( !pCharNPC ) {
		// 상점 주인이 없어???
		return this->Send_gsv_STORE_TRADE_REPLY( STORE_TRADE_RESULT_NPC_NOT_FOUND );
	}
#ifdef __CLAN_WAR_EXT
	// LIST_NPC의 20번 컬럼을 읽음
	BYTE btUnionIDX = NPC_UNION_NO( pCharNPC->Get_CharNO() );
	// 클랜 상점이 아니라면 (11:클랜 상점)
	if( btUnionIDX != 11 )
	{
		// 현재는 무조건 0번 조합 상점(무소속)밖에 없다.
		if ( btUnionIDX && btUnionIDX != this->GetCur_JOHAP() ) {
			// 조합 상점인데 소속 조합이 틀리면...
			return this->Send_gsv_STORE_TRADE_REPLY( STORE_TRADE_RESULT_NOT_UNION_USER );
		}
	}
#else
	BYTE btUnionIDX = NPC_UNION_NO( pCharNPC->Get_CharNO() );
	int iiiii = this->GetCur_JOHAP();
	if ( btUnionIDX && btUnionIDX != this->GetCur_JOHAP() ) {	
		// 조합 상점인데 소속 조합이 틀리면...
		return this->Send_gsv_STORE_TRADE_REPLY( STORE_TRADE_RESULT_NOT_UNION_USER );
	}
#endif

	// 상점 npc와의 거래 체크...
	int iDistance = distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)pCharNPC->m_PosCUR.x, (int)pCharNPC->m_PosCUR.y);
	if ( iDistance > MAX_TRADE_DISTANCE )	{ 
		return this->Send_gsv_STORE_TRADE_REPLY( STORE_TRADE_RESULT_TOO_FAR );
	}

	short nOffset=sizeof( cli_STORE_TRADE_REQ );

	tag_BUY_ITEM  *pBuyITEMs;
	tag_SELL_ITEM *pSellITEMs;

	if ( pPacket->m_cli_STORE_TRADE_REQ.m_cBuyCNT > 0 ) {
		pBuyITEMs  = (tag_BUY_ITEM*) Packet_GetDataPtr( pPacket, nOffset, sizeof( tag_BUY_ITEM  ) * pPacket->m_cli_STORE_TRADE_REQ.m_cBuyCNT  );
		if ( !pBuyITEMs )
			return IS_HACKING( this, "Recv_cli_STORE_TRADE_REQ-1 :: Invalid Buy Items" );
	}

    classPACKET *pCPacket = Packet_AllocNLock ();
	//////////////////////////////////////////////////////////////////////////
	// 2006.06.12:이성활:pCPacket에 대한 Null 체크
	if (!pCPacket) {
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	pCPacket->m_HEADER.m_wType = GSV_SET_MONEYnINV;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_MONEYnINV );
	pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT = 0;

	bool bResult = true;
	int iPriceEA, iTotValue;
	short nC;
	if ( pPacket->m_cli_STORE_TRADE_REQ.m_cSellCNT > 0 ) {
		pSellITEMs = (tag_SELL_ITEM*)Packet_GetDataPtr( pPacket, nOffset, sizeof( tag_SELL_ITEM ) * pPacket->m_cli_STORE_TRADE_REQ.m_cSellCNT );

		// 물건 팔자 !!!
		if ( !pSellITEMs ) {
			IS_HACKING( this, "Recv_cli_STORE_TRADE_REQ-2 :: Invalid Sell Items" );
			bResult = false;
			goto _RETURN;
		}

		tagITEM *pITEM;
		for (nC=0; nC<pPacket->m_cli_STORE_TRADE_REQ.m_cSellCNT; nC++) {
			if ( pSellITEMs[ nC ].m_btInvIDX >= INVENTORY_TOTAL_SIZE ) {
				IS_HACKING( this, "Recv_cli_STORE_TRADE_REQ-2-2 :: Invalid inventory index" );
				bResult = false;
				goto _RETURN;
			}

			pITEM   = &m_Inventory.m_ItemLIST[ pSellITEMs[ nC ].m_btInvIDX ];
			if ( !pITEM->IsEnableSELL() ) {
				// 상점 판매 불가 아이템이다.
				if ( pITEM->GetHEADER() ) {
					IS_HACKING( this, "Recv_cli_STORE_TRADE_REQ-3 :: Cant sell to store" );
					bResult = false;
				}
				goto _RETURN;
			}

			iPriceEA = this->GetZONE()->Get_ItemSellPRICE( *pITEM, this->GetSellSkillVALUE() ) ;

			// 판매한 아이템 로그를 남김...
			if ( pITEM->IsEnableDupCNT() ) {
				if ( pSellITEMs[ nC ].m_wDupCNT >= pITEM->GetQuantity() ) {
					this->GetZONE()->SellITEMs( pITEM, pITEM->GetQuantity() );

					iTotValue = iPriceEA * pITEM->GetQuantity();

						g_pThreadLOG->When_TagItemLOG( LIA_SELL2NPC, this, pITEM, pITEM->GetQuantity(), iTotValue );

					this->ClearITEM( pSellITEMs[ nC ].m_btInvIDX );
				} else {
					this->GetZONE()->SellITEMs( pITEM, pSellITEMs[ nC ].m_wDupCNT );

					iTotValue = iPriceEA * pSellITEMs[ nC ].m_wDupCNT;
					g_pThreadLOG->When_TagItemLOG( LIA_SELL2NPC, this, pITEM, pSellITEMs[ nC ].m_wDupCNT, iTotValue );

					pITEM->m_uiQuantity -= pSellITEMs[ nC ].m_wDupCNT;
				}
			} else {
				iTotValue = iPriceEA;
					g_pThreadLOG->When_TagItemLOG( LIA_SELL2NPC, this, pITEM, 1, iTotValue );
				this->ClearITEM( pSellITEMs[ nC ].m_btInvIDX );
			}
			this->Add_CurMONEY( iTotValue );

			// 변경된 인벤토리 정보 생성...
			pCPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_btInvIDX =  pSellITEMs[ nC ].m_btInvIDX;
			pCPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_ITEM	 = *pITEM;
			pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ++;
		}
	}

	short nInvIDX;
	tagITEM sBuyITEM;
	for (nC=0; nC<pPacket->m_cli_STORE_TRADE_REQ.m_cBuyCNT; nC++) {
		// 물건 구입 !!!
		if ( !pCharNPC->Get_SellITEM( pBuyITEMs[ nC ].m_cTabNO, pBuyITEMs[ nC ].m_cColNO, sBuyITEM ) ) {
			break;
		}

#ifdef __CLAN_WAR_EXT
		// 클랜 상점인가?

		if( btUnionIDX == 11 )
		{
			// 클랜 상점의 가격도 조합 포인트를 가져오듯 한다.
			iPriceEA = ITEM_MAKE_DIFFICULT( sBuyITEM.m_cType, sBuyITEM.m_nItemNo );
			if ( tagITEM::IsEnableDupCNT( sBuyITEM.GetTYPE() ) ) {
				iTotValue = iPriceEA*pBuyITEMs[ nC ].m_wDupCNT;
				sBuyITEM.m_uiQuantity = pBuyITEMs[ nC ].m_wDupCNT;
			} else {
				iTotValue = iPriceEA;
			}
			
			// 보상 포인트가 충분한가?
			if ( this->m_CLAN.m_iRewardPoint < iTotValue )
			{
				// 보상 포인터 없다.
				this->Send_gsv_STORE_TRADE_REPLY( STORE_TRADE_RESULT_OUT_OF_REWARD_POINT );
				break;
			}
		}
		else if ( btUnionIDX ) 
		{
			// 조합상점...조합 포인트를 소모...
			iPriceEA = ITEM_MAKE_DIFFICULT( sBuyITEM.m_cType, sBuyITEM.m_nItemNo );
			if ( tagITEM::IsEnableDupCNT( sBuyITEM.GetTYPE() ) ) {
				iTotValue = iPriceEA*pBuyITEMs[ nC ].m_wDupCNT;
				sBuyITEM.m_uiQuantity = pBuyITEMs[ nC ].m_wDupCNT;
			} else {
				iTotValue = iPriceEA;
			}

			if ( this->GetCur_JoHapPOINT( btUnionIDX ) < iTotValue ) {
				// 조합 포인트 없다.
				this->Send_gsv_STORE_TRADE_REPLY( STORE_TRADE_RESULT_OUT_OF_POINT );
				break;
			}
		} 
		else
		{
			iPriceEA = this->GetZONE()->Get_ItemBuyPRICE( sBuyITEM.m_cType, sBuyITEM.m_nItemNo, this->GetBuySkillVALUE() );
			if ( tagITEM::IsEnableDupCNT( sBuyITEM.GetTYPE() ) ) 
			{
				iTotValue = iPriceEA*pBuyITEMs[ nC ].m_wDupCNT;
				sBuyITEM.m_uiQuantity = pBuyITEMs[ nC ].m_wDupCNT;
			} 
			else 
			{
				iTotValue = iPriceEA;
			}
			if ( this->GetCur_MONEY() < iTotValue )
			{
				// 돈 없다.
				this->Send_gsv_STORE_TRADE_REPLY( STORE_TRADE_RESULT_OUT_OF_MONEY );
				break;
			}

			this->GetZONE()->BuyITEMs( sBuyITEM );
		}
		this->Set_ItemSN( sBuyITEM );	// 상점에서 구입시...
#else
		if ( btUnionIDX ) {
			// 조합상점...조합 포인트를 소모...
			iPriceEA = ITEM_MAKE_DIFFICULT( sBuyITEM.m_cType, sBuyITEM.m_nItemNo );
			if ( tagITEM::IsEnableDupCNT( sBuyITEM.GetTYPE() ) ) {
				iTotValue = iPriceEA*pBuyITEMs[ nC ].m_wDupCNT;
				sBuyITEM.m_uiQuantity = pBuyITEMs[ nC ].m_wDupCNT;
			} else {
				iTotValue = iPriceEA;
			}

			if ( this->GetCur_JoHapPOINT( btUnionIDX ) < iTotValue ) {
				// 조합 포인트 없다.
				this->Send_gsv_STORE_TRADE_REPLY( STORE_TRADE_RESULT_OUT_OF_POINT );
				break;
			}
		} 
		else
		{
			iPriceEA = this->GetZONE()->Get_ItemBuyPRICE( sBuyITEM.m_cType, sBuyITEM.m_nItemNo, this->GetBuySkillVALUE() );
			if ( tagITEM::IsEnableDupCNT( sBuyITEM.GetTYPE() ) ) 
			{
				iTotValue = iPriceEA*pBuyITEMs[ nC ].m_wDupCNT;
				sBuyITEM.m_uiQuantity = pBuyITEMs[ nC ].m_wDupCNT;
			} 
			else 
			{
				iTotValue = iPriceEA;
			}
			if ( this->GetCur_MONEY() < iTotValue )
			{
				// 돈 없다.
				this->Send_gsv_STORE_TRADE_REPLY( STORE_TRADE_RESULT_OUT_OF_MONEY );
				break;
			}

			this->GetZONE()->BuyITEMs( sBuyITEM );
		}
		this->Set_ItemSN( sBuyITEM );	// 상점에서 구입시...
#endif


#ifdef __CLAN_WAR_EXT
		// 소모...
		// 클랜 상점인가?
		if( btUnionIDX == 11 )
		{
			// 구입한 아이템 로그를 남김...
			g_pThreadLOG->When_TagItemLOG( LIA_BUYFROMCLANSTORE, this, &sBuyITEM, pBuyITEMs[ nC ].m_wDupCNT, iTotValue );
			AddClanRewardPoint( -1*iTotValue, Get_NAME() );
		}
		else if ( btUnionIDX )
		{
			// 구입한 아이템 로그를 남김...
			g_pThreadLOG->When_TagItemLOG( LIA_BUYFROMUNION, this, &sBuyITEM, pBuyITEMs[ nC ].m_wDupCNT, iTotValue, NULL, true );
			this->SubCur_JoHapPOINT( btUnionIDX, iTotValue );
		} 
		else
		{
			// 구입한 아이템 로그를 남김...
			g_pThreadLOG->When_TagItemLOG( LIA_BUYFROMNPC, this, &sBuyITEM, pBuyITEMs[ nC ].m_wDupCNT, iTotValue );
			this->Sub_CurMONEY( iTotValue ) ;
		}

		nInvIDX = this->Add_ITEM( sBuyITEM );
		if ( nInvIDX > 0 )
		{
			pCPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_btInvIDX = (BYTE)nInvIDX;
			pCPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_ITEM	 = m_Inventory.m_ItemLIST[ nInvIDX ];
			pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ++;
		}
		else
		{
			this->Save_ItemToFILED( sBuyITEM );	// 상점 구입후 인벤토리가 추가가 불가능해서 밖에 30분간 보관...
		}
#else
		// 소모...
		if ( btUnionIDX )
		{
			// 구입한 아이템 로그를 남김...
			g_pThreadLOG->When_TagItemLOG( LIA_BUYFROMUNION, this, &sBuyITEM, pBuyITEMs[ nC ].m_wDupCNT, iTotValue, NULL, true );
			this->SubCur_JoHapPOINT( btUnionIDX, iTotValue );
		} 
		else
		{
			// 구입한 아이템 로그를 남김...
			g_pThreadLOG->When_TagItemLOG( LIA_BUYFROMNPC, this, &sBuyITEM, pBuyITEMs[ nC ].m_wDupCNT, iTotValue );
			this->Sub_CurMONEY( iTotValue ) ;
		}

		nInvIDX = this->Add_ITEM( sBuyITEM );
		if ( nInvIDX > 0 )
		{
			pCPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_btInvIDX = (BYTE)nInvIDX;
			pCPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_ITEM	 = m_Inventory.m_ItemLIST[ nInvIDX ];
			pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ++;
		}
		else
		{
			this->Save_ItemToFILED( sBuyITEM );	// 상점 구입후 인벤토리가 추가가 불가능해서 밖에 30분간 보관...
		}
#endif
	}

	pCPacket->m_HEADER.m_nSize += ( pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT * sizeof( tag_SET_INVITEM ) );
	pCPacket->m_gsv_SET_MONEYnINV.m_i64Money = this->GetCur_MONEY();

    this->SendPacket( pCPacket );

_RETURN :
    Packet_ReleaseNUnlock( pCPacket );

	return bResult;
}


//-------------------------------------------------------------------------------------------------
/// 단축아이콘 설정 패킷
bool classUSER::Recv_cli_SET_HOTICON( t_PACKET *pPacket )
{
	if ( pPacket->m_cli_SET_HOTICON.m_btListIDX >= MAX_HOT_ICONS )
		return false;

	if ( this->m_HotICONS.RegHotICON( pPacket->m_cli_SET_HOTICON.m_btListIDX, pPacket->m_cli_SET_HOTICON.m_HotICON ) ) {
        classPACKET *pCPacket = Packet_AllocNLock ();
		//////////////////////////////////////////////////////////////////////////
		// 2006.06.12:이성활:pCPacket에 대한 Null 체크
		if (!pCPacket) {
			return false;
		}
		//////////////////////////////////////////////////////////////////////////

        pCPacket->m_HEADER.m_wType = GSV_SET_HOTICON;
        pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_HOTICON );
        pCPacket->m_gsv_SET_HOTICON.m_btListIDX = pPacket->m_cli_SET_HOTICON.m_btListIDX;
        pCPacket->m_gsv_SET_HOTICON.m_HotICON   = this->m_HotICONS.m_IconLIST[ pPacket->m_cli_SET_HOTICON.m_btListIDX ];

        this->SendPacket( pCPacket );
        Packet_ReleaseNUnlock( pCPacket );

		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
/// 클라이언트와의 동기화를 맞추기 위해 현재 설정된 남은 총알의 갯수 전송
bool classUSER::Send_BulltCOUNT ()
{
	tagITEM *pShotITEM = &this->m_Inventory.m_ItemSHOT[ this->m_btShotTYPE ];
	BYTE btInvIDX = INVENTORY_SHOT_ITEM0+this->m_btShotTYPE;

	return this->Send_gsv_SET_INV_ONLY (btInvIDX, pShotITEM);
}

/// 총알 타입 변경 전송
bool classUSER::Send_gsv_SET_BULLET( BYTE btShotTYPE )
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_SET_BULLET;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_BULLET );

	pCPacket->m_gsv_SET_BULLET.m_wObjectIDX = this->Get_INDEX();

	pCPacket->m_gsv_SET_BULLET.m_sShot.m_cType   = btShotTYPE;
	if ( this->m_Inventory.m_ItemSHOT[ btShotTYPE ].GetQuantity() > 0 ) {
		pCPacket->m_gsv_SET_BULLET.m_sShot.m_nItemNo = this->m_Inventory.m_ItemSHOT[ btShotTYPE ].GetItemNO();
	} else {
		pCPacket->m_gsv_SET_BULLET.m_sShot.m_nItemNo = 0;
	}

	this->GetZONE()->SendPacketToSectors( this, pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	return true;
}


/*
void classUSER::Send_BulletCNT( BYTE btShotCnt )
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return;

	pCPacket->m_HEADER.m_wType = GSV_SET_INV_ONLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_INV_ONLY ) + sizeof( tag_SET_INVITEM );
	pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT = 1;

	pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_btInvIDX = INVENTORY_SHOT_ITEM0 + btShotCnt;
	pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_ITEM     = m_Inventory.m_ItemLIST[ INVENTORY_SHOT_ITEM0 + btShotCnt ];

	Packet_ReleaseNUnlock( pCPacket );
}
*/

/// 총알 변경 요청 패킷
bool classUSER::Recv_cli_SET_BULLET( t_PACKET *pPacket )
{
	if ( this->Get_ActiveSKILL() ) return true;	// 스킬 케스팅 중일땐 못바꿔
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() ) return true;

    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	bool bResult = true;
	if ( pPacket->m_cli_SET_BULLET.m_wInventoryIDX ) {
		// 교환 ? 장착 ? 추가 ?
		tagITEM *pInvITEM  = &m_Inventory.m_ItemLIST[ pPacket->m_cli_SET_BULLET.m_wInventoryIDX ];
		if ( 0 == pInvITEM->GetHEADER() ) {
			goto _RETURN;
		}
		// 장착 위치가 맞는 소모탄인가 ???
		if ( !pInvITEM->IsEtcITEM() ) {
			IS_HACKING( this, "Invalid bullet position" );
			bResult = false;
			goto _RETURN;
		}

		switch( pPacket->m_cli_SET_BULLET.m_wShotTYPE ) {
			case SHOT_TYPE_ARROW  :
				if ( 431 != ITEM_TYPE( pInvITEM->GetTYPE(), pInvITEM->GetItemNO() ) ) {
					IS_HACKING( this, "bullet type must SHOT_TYPE_ARROW" );
					bResult = false;
					goto _RETURN;
				}
				break;

			case SHOT_TYPE_BULLET :
				if ( 432 != ITEM_TYPE( pInvITEM->GetTYPE(), pInvITEM->GetItemNO() ) ) {
					IS_HACKING( this, "bullet type must SHOT_TYPE_BULLET" );
					bResult = false;
					goto _RETURN;
				}
				break;;

			case SHOT_TYPE_THROW  :
				switch( ITEM_TYPE( pInvITEM->GetTYPE(), pInvITEM->GetItemNO() ) ) {
					case 421 :	case 422 :	case 423 :	case 433 :
						break;
					default :
						IS_HACKING( this, "bullet type must SHOT_TYPE_THROW" );
						bResult = false;
						goto _RETURN;
				}
		}

		tagITEM *pShotITEM = &m_Inventory.m_ItemSHOT[ pPacket->m_cli_SET_BULLET.m_wShotTYPE ];

		if ( pShotITEM->GetHEADER() == pInvITEM->GetHEADER() ) {
			// 같은 아이템은 갯수를 더한다.
			if ( pShotITEM->GetQuantity() + pInvITEM->GetQuantity() > MAX_DUP_ITEM_QUANTITY ) {
				// 덜어 놓자...
				pInvITEM->m_uiQuantity -= (MAX_DUP_ITEM_QUANTITY - pShotITEM->GetQuantity() );
				pShotITEM->m_uiQuantity = MAX_DUP_ITEM_QUANTITY;
			} else {
				// 더하고 삭제.
				pShotITEM->m_uiQuantity += pInvITEM->GetQuantity();
				this->ClearITEM( pPacket->m_cli_SET_BULLET.m_wInventoryIDX );
			}
		} else {
			// 교체...
			tagITEM sTmpItem;
			sTmpItem = *pShotITEM;
			*pShotITEM = *pInvITEM;
			*pInvITEM = sTmpItem;
		}

		pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_btInvIDX = INVENTORY_SHOT_ITEM0 + pPacket->m_cli_SET_BULLET.m_wShotTYPE;
		pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_ITEM     =*pShotITEM;

		pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_btInvIDX = pPacket->m_cli_SET_BULLET.m_wInventoryIDX;
		pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_ITEM     =*pInvITEM;
	} else {
		// 탈거...
		// 장비 탈거...
		tagITEM *pShotITEM = &m_Inventory.m_ItemSHOT[ pPacket->m_cli_SET_BULLET.m_wShotTYPE ];

		if ( 0 == pShotITEM->GetHEADER() ) {
			// 모두 소모됐는데 클라이언트에서는 남은것으로 오인... 삭제하라는 패킷 전송.
			this->Send_gsv_SET_INV_ONLY( INVENTORY_SHOT_ITEM0 + pPacket->m_cli_SET_BULLET.m_wShotTYPE, pShotITEM );
			goto _RETURN;
		}

		short nInvIDX = this->Add_ITEM( *pShotITEM  );
		if ( nInvIDX > 0 ) {
			pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_btInvIDX = INVENTORY_SHOT_ITEM0 + pPacket->m_cli_SET_BULLET.m_wShotTYPE;
			pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 0 ].m_ITEM.Clear();

			pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_btInvIDX =  (BYTE)nInvIDX;
			pCPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ 1 ].m_ITEM     = *pShotITEM;

			// 순서 주의 밑에꺼 호출되면 *pEquipITEM이 삭제된다.
			this->ClearITEM( INVENTORY_SHOT_ITEM0 + pPacket->m_cli_SET_BULLET.m_wShotTYPE );
		} else {
			// 빈 인벤토리가 없어서 총알 탈거 못함...
			goto _RETURN;
		}
	}

	pCPacket->m_HEADER.m_wType = GSV_SET_INV_ONLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_INV_ONLY ) + 2 * sizeof( tag_SET_INVITEM );
	pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT = 2;

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	{	// 로컬 변수 선언...

		t_eSHOT eShotTYPE = this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_WEAPON_R ].GetShotTYPE();
		if ( eShotTYPE == this->m_btShotTYPE ) {
			// 바뀐 총알이 현재 무기와 연관 되는가?
			this->Set_ShotITEM ( eShotTYPE );				// classUSER::Recv_cli_SET_BULLET
	}

	return this->Send_gsv_SET_BULLET( pPacket->m_cli_SET_BULLET.m_wShotTYPE );
	}

_RETURN :
	Packet_ReleaseNUnlock( pCPacket );
	return bResult;
}


//-------------------------------------------------------------------------------------------------
/// 아이템 제조 결과 통보
bool classUSER::Send_gsv_CREATE_ITEM_REPLY (BYTE btResult, short nStepORInvIDX, float *pProcPOINT, tagITEM *pOutItem)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	//////////////////////////////////////////////////////////////////////////
	// 2006.06.12:이성활:pCPacket에 대한 Null 체크
	if (!pCPacket) {
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	pCPacket->m_HEADER.m_wType = GSV_CREATE_ITEM_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_CREATE_ITEM_REPLY );

	pCPacket->m_gsv_CREATE_ITEM_REPLY.m_btRESULT = btResult;
	pCPacket->m_gsv_CREATE_ITEM_REPLY.m_nStepORInvIDX = nStepORInvIDX;

	if ( NULL != pProcPOINT ) {
		for (short nI=0; nI<CREATE_ITEM_STEP; nI++)
			pCPacket->m_gsv_CREATE_ITEM_REPLY.m_nPRO_POINT[ nI ] = (short)pProcPOINT[ nI ];
	}
	if ( NULL != pOutItem ) {
		pCPacket->m_gsv_CREATE_ITEM_REPLY.m_CreatedITEM = *pOutItem;
	}

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}
/// 아이템 제조 요청 받음
bool classUSER::Recv_cli_CREATE_ITEM_REQ( t_PACKET *pPacket )
{
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() ) return true;

//	김영환 수정
#ifdef	__MILEAGE_SKILL_USED
	if ( pPacket->m_cli_CREATE_ITEM_REQ.m_wSkillSLOT >= MAX_LEARNED_SKILL_CNT )
		return false;
#else
	if ( pPacket->m_cli_CREATE_ITEM_REQ.m_btSkillSLOT >= MAX_LEARNED_SKILL_CNT )
		return false;
#endif

	if ( !tagITEM::IsValidITEM( pPacket->m_cli_CREATE_ITEM_REQ.m_cTargetItemTYPE, pPacket->m_cli_CREATE_ITEM_REQ.m_nTargetItemNO ) ) {
		// Hacking ???
		return true;
	}
#ifndef __DONT_CHECK_CREATE_EXP
	if ( this->m_nCreateItemEXP > 0 ) {
		// 메크로나 해킹으로 제조 패킷 생성해서 보낼때...
		return true;
	}
#endif
	this->m_nCreateItemEXP = 0;

//	김영환 수정
#ifdef	__MILEAGE_SKILL_USED
	short nSkillIDX = this->m_Skills.m_nSkillINDEX[ pPacket->m_cli_CREATE_ITEM_REQ.m_wSkillSLOT ];
#else
	short nSkillIDX = this->m_Skills.m_nSkillINDEX[ pPacket->m_cli_CREATE_ITEM_REQ.m_btSkillSLOT ];
#endif


	// 스킬의 제조 번호와 만들려는 아이템의 제조번호 비교...
	if ( SKILL_ITEM_MAKE_NUM( nSkillIDX ) != ITEM_MAKE_NUM( pPacket->m_cli_CREATE_ITEM_REQ.m_cTargetItemTYPE, pPacket->m_cli_CREATE_ITEM_REQ.m_nTargetItemNO ) ) {
		// return IS_HACKING( this, "Recv_cli_CREATE_ITEM_REQ-1 :: Invalid SKILL_ITEM_MAKE_NUM");		// ERROR:: 같아야 하는데??
		// 클라이언트에서 역시 이경우에도 패킷이 올때가 있어 짤르진 말자...
		return true;
	}

	// 스킬 레벨 비교...
	if ( SKILL_LEVEL( nSkillIDX ) < ITEM_SKILL_LEV( pPacket->m_cli_CREATE_ITEM_REQ.m_cTargetItemTYPE, pPacket->m_cli_CREATE_ITEM_REQ.m_nTargetItemNO ) ) {
		return IS_HACKING( this, "Recv_cli_CREATE_ITEM_REQ-2 :: Invalid SKILL_LEVEL");				// ERROR:: 크거나 같아야 하는데 ???
	}

	// 스킬 사용 가능한가 ???
	if ( !this->Skill_ActionCondition( nSkillIDX ) ) {
		return this->Send_gsv_CREATE_ITEM_REPLY( RESULT_CREATE_ITEM_INVALID_CONDITION, 0 );
	}

	// 실제 필요 수치 소모 적용...
	this->Skill_UseAbilityValue( nSkillIDX );	// Recv_cli_CREATE_ITEM_REQ

	short nI, nProductIDX;
	// 아이템 제조번호
	nProductIDX = ITEM_PRODUCT_IDX( pPacket->m_cli_CREATE_ITEM_REQ.m_cTargetItemTYPE, pPacket->m_cli_CREATE_ITEM_REQ.m_nTargetItemNO );

	short nInvIDX, nMAT_QUAL, nNUM_MAT, nPLUS;
	float fPRO_POINT[4], fSUC_POINT[4];
	tagITEM *pInvITEM, sOutITEM, sUsedITEM[4];

	// 만들려는 아이템의 제조 난이도...
	short nITEM_DIF = ITEM_MAKE_DIFFICULT( pPacket->m_cli_CREATE_ITEM_REQ.m_cTargetItemTYPE, pPacket->m_cli_CREATE_ITEM_REQ.m_nTargetItemNO );
	// 만들려는 아이템의 품질 수치...
	short nITEM_QUAL = ITEM_QUALITY( pPacket->m_cli_CREATE_ITEM_REQ.m_cTargetItemTYPE, pPacket->m_cli_CREATE_ITEM_REQ.m_nTargetItemNO );
	// 사용되는 재료 갯수...
	for (nNUM_MAT=1, nI=1; nI<CREATE_ITEM_STEP; nI++) {
		if ( PRODUCT_NEED_ITEM_NO( nProductIDX, nI ) ) 
			nNUM_MAT ++;
	}

	// 재조 시작을 주변에 통보...
	this->Send_gsv_ITEM_RESULT_REPORT( REPORT_ITEM_CREATE_START,
									pPacket->m_cli_CREATE_ITEM_REQ.m_cTargetItemTYPE,
									pPacket->m_cli_CREATE_ITEM_REQ.m_nTargetItemNO );


	short nRand, nUsedCNT=0;
	// Check: 만들고자 하는 아이템에 필요한 재료들의 종류와 갯수가 합당한가 판단.
	for (nI=0; nI<CREATE_ITEM_STEP; nI++) {
		nInvIDX  = pPacket->m_cli_CREATE_ITEM_REQ.m_nUseItemINV[ nI ];
		pInvITEM = &this->m_Inventory.m_ItemLIST[ nInvIDX ];

		if ( 0 == PRODUCT_NEED_ITEM_NO( nProductIDX, nI ) ) {
			if ( 0 != nI ) {
				// 필요한 재료가 없다.
				continue;
			}
			if ( 0 == pInvITEM->GetHEADER() ) {
				// 마우스로 마구 클릭시 여기로 들어올수 있다..
				return this->Send_gsv_CREATE_ITEM_REPLY( RESULT_CREATE_ITEM_INVALID_ITEM, 0 );
			}

			// 첫번째 원료 종류나, 재료 아이템 비교 ,,, 둘중 하나 ..
			if ( PRODUCT_RAW_MATERIAL( nProductIDX ) ) {
				// 원료 종류일 경우...
				if ( PRODUCT_RAW_MATERIAL( nProductIDX ) != ITEM_TYPE( pInvITEM->GetTYPE(), pInvITEM->GetItemNO() ) ) {
					return this->Send_gsv_CREATE_ITEM_REPLY( RESULT_CREATE_ITEM_INVALID_ITEM, 0 );
				}
			} else {
				// LIST_PRODUCT.STB 데이타가 잘못 입력되어 있다.
				return true;
			}
		} else {
			if ( 0 == pInvITEM->GetHEADER() ) {
				// 클라이언트에서 이경우에도 요청한다.. 이미 소모된 아이템을 되돌릴수 없는 문제 !!!
				return this->Send_gsv_CREATE_ITEM_REPLY( RESULT_CREATE_ITEM_INVALID_ITEM, nI, fPRO_POINT );
			}
			sOutITEM.Init( PRODUCT_NEED_ITEM_NO( nProductIDX, nI ) );
			if ( pInvITEM->GetHEADER() != sOutITEM.GetHEADER() ) {
				return this->Send_gsv_CREATE_ITEM_REPLY( RESULT_CREATE_ITEM_INVALID_ITEM, nI, fPRO_POINT );
			}
		}

		// 장비 아이템이 아닐 경우 필요 갯수 비교...
		if ( pInvITEM->IsEnableDupCNT() ) {
			if ( pInvITEM->GetQuantity() < (WORD)PRODUCT_NEED_ITEM_CNT( nProductIDX, nI ) ) {
				return this->Send_gsv_CREATE_ITEM_REPLY( RESULT_CREATE_ITEM_NEED_ITEM, nI );
			}
		}

		nRand = RANDOM(100);
		switch( nI ) {
			case 0 :
			{
				// 1번째 원재료 아이템 품질에서 얻음.
				nMAT_QUAL = ITEM_QUALITY( pInvITEM->GetTYPE(), pInvITEM->GetItemNO() );

				fSUC_POINT[0] = ( nITEM_DIF+35 ) * ( nITEM_QUAL+15 ) / 16.f;
				if( IsTAIWAN() )
				{
					// PRO_POINT1 = { (MAT_QUAL) * ((RAN(1~100)+70) * ((CON / A_LV)*100 + ITEM_DIF/2 + 430) * (WORLD_PROD) } / 800000 (2005.07.08 대만)
					fPRO_POINT[0] = ( nMAT_QUAL * (nRand+71) * ( (GetCur_CON() / GetCur_LEVEL())*100.f + nITEM_DIF/2.f + 430 ) * Get_WorldPROD() ) / 800000.f;
				}
				else
					fPRO_POINT[0] = ( nMAT_QUAL * (nRand+71) * ( 0.5f*this->GetCur_CON() + nITEM_DIF/2.f + 530 ) * Get_WorldPROD() ) / 800000.f;
				nPLUS = (short)( ( fPRO_POINT[0] - fSUC_POINT[0] ) * 30 / ( fPRO_POINT[0]+nITEM_QUAL ) );
				break;
			}
			case 1 :
			{
				fSUC_POINT[1] = ( nITEM_DIF+15 ) * ( nITEM_QUAL+140 ) / ( nNUM_MAT+3) / 4.f;
				if( IsTAIWAN() )
				{
					// PRO_POINT2 = { (MAT_QUAL + ITEM_DIF/2) * ((RAN(1~100)+100) * ((CON / A_LV)*100 + MAT_QUAL*2 + 600) } / (NUM_MAT+7) / 1600 (2005.07.08 대만)
					fPRO_POINT[1] = ( ( nMAT_QUAL + nITEM_DIF/2.f ) * (nRand+101) * ( (this->GetCur_CON() / this->GetCur_LEVEL())*100.f + nMAT_QUAL*2 + 600 ) ) / ( nNUM_MAT+7 ) / 1600.f;
				}
				else
					fPRO_POINT[1] = ( ( nMAT_QUAL + nITEM_DIF/2.f ) * (nRand+96) * ( 0.5f*this->GetCur_CON() + SKILL_LEVEL( nSkillIDX )*6 + nMAT_QUAL*2 + 770 ) ) / ( nNUM_MAT+7 ) / 1600.f;
				nPLUS += (short) ( ( fPRO_POINT[1] - fSUC_POINT[1] ) * 20 / ( fPRO_POINT[1] ) );
				break;
			}
			case 2 :
			{
				fSUC_POINT[2] = ( nITEM_DIF+90 ) * ( nITEM_QUAL+30 ) / ( nNUM_MAT+3) / 4.f;
				if( IsTAIWAN() )
				{
					// PRO_POINT3 = { (PRO_POINT1/6 + ITEM_QUAL) * (RAN(1~100)+80) * ((CON / A_LV)*100 + MAT_QUAL*2 + 500) } / (NUM_MAT+7) / 2000 (2005.07.08 대만)
					fPRO_POINT[2] = ( ( fPRO_POINT[0]/6.f + nITEM_QUAL ) * ( nRand+81 ) * ( (GetCur_CON() / GetCur_LEVEL()) * 100.f + nMAT_QUAL*2 + 500 ) ) / ( nNUM_MAT+7 ) / 2000.f;
				}
				else
					fPRO_POINT[2] = ( ( fPRO_POINT[0]/6.f + nITEM_QUAL ) * ( nRand+81 ) * ( 0.3f*this->GetCur_CON() + SKILL_LEVEL( nSkillIDX )*5 + nMAT_QUAL*2 + 600 ) ) / ( nNUM_MAT+7 ) / 2000.f;
				nPLUS += (short)( ( fPRO_POINT[2] - fSUC_POINT[2] ) * 10 / ( fPRO_POINT[2] ) );
				break;
			}
			case 3 :
			{
				fSUC_POINT[3] = ( nITEM_DIF+40 ) * ( nITEM_QUAL+60 ) / ( nNUM_MAT+2) / 6.f;
				fPRO_POINT[3] = ( ( fPRO_POINT[0] + fPRO_POINT[1] + 40 ) * (nRand+51) ) / 200.f;
				nPLUS += (short)( ( fPRO_POINT[3] - fSUC_POINT[3] ) * 10 / ( fPRO_POINT[3] ) );
				break;
			}
		}
#ifdef __INC_WORLD
		m_fPROPOINT[ 0 ] = fPRO_POINT[ 0 ] >= 0 ? fPRO_POINT[ 0 ] : 0;
		m_fPROPOINT[ 1 ] = fPRO_POINT[ 1 ] >= 0 ? fPRO_POINT[ 1 ] : 0;
		m_fPROPOINT[ 2 ] = fPRO_POINT[ 2 ] >= 0 ? fPRO_POINT[ 2 ] : 0;
		m_fPROPOINT[ 3 ] = fPRO_POINT[ 3 ] >= 0 ? fPRO_POINT[ 3 ] : 0;
		m_fSUCPOINT[ 0 ] = fSUC_POINT[ 0 ] >= 0 ? fSUC_POINT[ 0 ] : 0;
		m_fSUCPOINT[ 1 ] = fSUC_POINT[ 1 ] >= 0 ? fSUC_POINT[ 1 ] : 0;
		m_fSUCPOINT[ 2 ] = fSUC_POINT[ 2 ] >= 0 ? fSUC_POINT[ 2 ] : 0;
		m_fSUCPOINT[ 3 ] = fSUC_POINT[ 3 ] >= 0 ? fSUC_POINT[ 3 ] : 0;
#endif
		// LogString (LOG_DEBUG, "Step:%d, Rand: %d, Suc:%f, Pro:%f\n", nI, nRand, fSUC_POINT[nI], fPRO_POINT[nI] );

		// 사용된 원재료를 소모 시킴...
		nUsedCNT = nI;
		sUsedITEM[ nI ] = this->m_Inventory.m_ItemLIST[ nInvIDX ];
		if ( sUsedITEM[ nI ].IsEnableDupCNT() ) {
			// 중복된 갯수를 갖는 아이템이다.
			sUsedITEM[ nI ].m_uiQuantity = PRODUCT_NEED_ITEM_CNT( nProductIDX, nI );
		}
		this->Sub_ITEM( nInvIDX, sUsedITEM[ nI ] );

		if ( fPRO_POINT[nI] < fSUC_POINT[nI] ) {
			// 실패...
				g_pThreadLOG->When_CreateOrDestroyITEM ( this, NULL, sUsedITEM, nUsedCNT, NEWLOG_CREATE, NEWLOG_FAILED );

			if( IsTAIWAN() )
			{
				// 타이완 버젼에서 경험치는 빼지 않음..
			}
			else
				this->m_nCreateItemEXP = 1 + (short) ( ( this->Get_LEVEL() + 50 ) * fPRO_POINT[0] * ( nNUM_MAT+4 ) / 1300.f );
			return this->Send_gsv_CREATE_ITEM_REPLY( RESULT_CREATE_ITEM_FAILED, nI, fPRO_POINT );
		}
	}
 
	sOutITEM.Clear ();

	sOutITEM.m_cType    = pPacket->m_cli_CREATE_ITEM_REQ.m_cTargetItemTYPE;
	sOutITEM.m_nItemNo  = pPacket->m_cli_CREATE_ITEM_REQ.m_nTargetItemNO;
	sOutITEM.m_bCreated = 1;

	if ( !sOutITEM.IsEnableDupCNT() ) {
		sOutITEM.m_nLife = MAX_ITEM_LIFE;

		// 내구도
		int iTEMP = (int)( ( ITEM_DURABITY(sOutITEM.m_cType,sOutITEM.GetItemNO()) + 15 ) * 
									( nPLUS*1.3F + SKILL_LEVEL( nSkillIDX )*2 + 120 ) / (RANDOM(100)+81) * 0.6f );
		if ( iTEMP > 100 )
			sOutITEM.m_cDurability = 100;
		else
			sOutITEM.m_cDurability = iTEMP;

		// 소갯 갯수 결정
		switch ( ITEM_RARE_TYPE(sOutITEM.GetTYPE(), sOutITEM.GetItemNO() ) ) {
			case 1 :	// 무조건
				sOutITEM.m_bHasSocket = 1;
				sOutITEM.m_bIsAppraisal = 1;
				break;
			case 2 :	// 계산
				if( IsTAIWAN() )
				{
					// 소켓수=[{(SEN/A_LV)*40+400)*(PLUS*1.8+아이템품질*0.4+ 8) * 0.2}   / (RAND(1~100)+50)] ? 100 (2005.07.08 대만)
					iTEMP = (int)( ( (GetCur_SENSE()/GetCur_LEVEL())*40.f+400  ) * 
						( nPLUS*1.8f + ITEM_QUALITY(sOutITEM.GetTYPE(), sOutITEM.GetItemNO() )*0.4f + 8 ) * 0.2f / ( RANDOM(100)+51 ) - 100 );
				}
				else
					iTEMP = (int)( ( this->GetCur_SENSE()+400-this->Get_LEVEL()*0.7f ) * 
						( nPLUS*1.8f + ITEM_QUALITY(sOutITEM.GetTYPE(), sOutITEM.GetItemNO() )*0.4f + 8 ) * 0.2f / ( RANDOM(100)+50 ) - 100 );
				if ( iTEMP >= 1 ) {
					sOutITEM.m_bHasSocket = 1;
					sOutITEM.m_bIsAppraisal = 1;
					break;
				}
			case 0 :
				{
					// 아이템 옵션
					iTEMP = 1 + RANDOM(100);
					int iITEM_OP = 0;
					if( IsTAIWAN() )
					{
						// ITEM_OP = [{((SEN/A_LV)*50 + 220) * (PLUS+20) * 0.4 + ITEM_DIF*35 ? 1600 - TEMP} / (TEMP+17)] ? 85 (2005.07.08 대만)
						iITEM_OP = (int)( ( ((GetCur_SENSE()/Get_LEVEL()/2.f)*50.f + 220) * ( nPLUS+20 )*0.4f + nITEM_DIF*35 - 1600 - iTEMP ) / ( iTEMP+17 ) - 85 );
					}
					else
						iITEM_OP = (int)( ( ( this->GetCur_SENSE()+220-this->Get_LEVEL()/2.f ) * ( nPLUS+20 )*0.4f + nITEM_DIF*35 - 1600 - iTEMP ) / ( iTEMP+17 ) - 85 );
					if ( iITEM_OP > 0 ) {
						sOutITEM.m_bIsAppraisal = 1;	// 감정 받은걸루...
						int iMod = (int)( (ITEM_QUALITY(sOutITEM.GetTYPE(), sOutITEM.GetItemNO() ) + 12)*3.2f );
						if ( iMod < 300 )
							sOutITEM.m_nGEM_OP = iITEM_OP % iMod;
						else
							sOutITEM.m_nGEM_OP = iITEM_OP % 300;
					}
				}
				break;
		}

		this->Set_ItemSN( sOutITEM );	// 아이템 제조시...
	} else {
		// 무조건 1개 제조된다...
		sOutITEM.m_uiQuantity = 1;
	}

		g_pThreadLOG->When_CreateOrDestroyITEM ( this, &sOutITEM, sUsedITEM, nUsedCNT, NEWLOG_CREATE, NEWLOG_SUCCESS );

	// sOutITEM 인벤토리에 넣고 성공 전송.
	nI = this->Add_ITEM( sOutITEM );
	if ( nI<= 0 ) {
		this->Save_ItemToFILED( sOutITEM );	// 제조시 인벤토리가 추가가 불가능해서 밖에 30분간 보관...
	}

	if( IsTAIWAN() )
	{
		// 대만 버젼일 경우.. 경험치 주지 않음.
	}
	else
		this->m_nCreateItemEXP = 1 + (short)( ( this->Get_LEVEL() + 35 ) * ( fPRO_POINT[0] + this->Get_LEVEL() ) * ( nNUM_MAT+4 ) * (nITEM_DIF+20) / 23000.f );

	return this->Send_gsv_CREATE_ITEM_REPLY( RESULT_CREATE_ITEM_SUCCESS, nI, fPRO_POINT, &sOutITEM);
}

//-------------------------------------------------------------------------------------------------
/// 제조, 제련 결과 응답에 대한 통보
bool classUSER::Send_gsv_ITEM_RESULT_REPORT( BYTE btReport, BYTE btItemType, short nItemNo)
{

#ifdef  __ADD_CREATE_ITEM_EXP				// 일본, 한국 버젼일 경우 제조경험치를 일반경험치에 더해준다.
	if(this->m_nCreateItemEXP)
	{
		this->Add_ExpNSend(m_nCreateItemEXP);
		this->m_nCreateItemEXP =0;
	}
#endif


	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_ITEM_RESULT_REPORT;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_ITEM_RESULT_REPORT );

	pCPacket->m_gsv_ITEM_RESULT_REPORT.m_wObjectIDX = this->Get_INDEX();
	pCPacket->m_gsv_ITEM_RESULT_REPORT.m_btREPORT	= btReport;
	pCPacket->m_gsv_ITEM_RESULT_REPORT.m_btItemType = btItemType;
	pCPacket->m_gsv_ITEM_RESULT_REPORT.m_nItemNO	= nItemNo;
	
    this->GetZONE()->SendPacketToSectors( this, pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}
/// 제조, 제련 결과 응답
bool classUSER::Recv_cli_ITEM_RESULT_REPORT( t_PACKET *pPacket )
{
	switch( pPacket->m_cli_ITEM_RESULT_REPORT.m_btREPORT )
	{
		// case REPORT_ITEM_CREATE_START	
		case REPORT_ITEM_CREATE_SUCCESS	:
		case REPORT_ITEM_CREATE_FAILED	:
		// case REPORT_ITEM_UPGRADE_START	
		case REPORT_ITEM_UPGRADE_SUCCESS:	
		case REPORT_ITEM_UPGRADE_FAILED	:
			break;
		default:
			return false;
	}

	this->Send_gsv_ITEM_RESULT_REPORT( pPacket->m_cli_ITEM_RESULT_REPORT.m_btREPORT,
		pPacket->m_cli_ITEM_RESULT_REPORT.m_btItemType,
		pPacket->m_cli_ITEM_RESULT_REPORT.m_nItemNO );


	return true;
}


//-------------------------------------------------------------------------------------------------
/// 특정 케릭의 HP를 요청
bool classUSER::Recv_cli_HP_REQ( t_PACKET *pPacket )
{
	CObjCHAR *pCHAR;
	pCHAR = g_pObjMGR->Get_CharOBJ( pPacket->m_cli_HP_REQ.m_wObjectIDX, true );

	if ( pCHAR ) {
		// TODO:: 문제... 상태만 보자는데 타겟이 바뀌네 ㅡ,.ㅡ;
		// this->Set_TargetIDX( pPacket->m_cli_HP_REQ.m_wObjectIDX );

		return Send_gsv_HP_REPLY( pPacket->m_cli_HP_REQ.m_wObjectIDX, pCHAR->Get_HP() );
	}
	
	return Send_gsv_HP_REPLY( pPacket->m_cli_HP_REQ.m_wObjectIDX, 0 );
}

//-------------------------------------------------------------------------------------------------
/// 소모 아이템 사용요청
bool classUSER::Recv_cli_USE_ITEM( t_PACKET *pPacket )
{
	if ( this->m_IngSTATUS.IsSET( FLAG_ING_IGNORE_ALL ) ) return true;

    if ( pPacket->m_cli_USE_ITEM.m_nInventoryIndex < 0 || 
		 pPacket->m_cli_USE_ITEM.m_nInventoryIndex >= INVENTORY_TOTAL_SIZE ) {
        // 패킷이 조작된것인가?
		return IS_HACKING( this, "Recv_cli_USE_ITEM-1" );
    }

	return this->Use_InventoryITEM( pPacket );
}

//-------------------------------------------------------------------------------------------------
/// 아이템을 바닦에 떨굼
bool classUSER::Recv_cli_DROP_ITEM( t_PACKET *pPacket )
{
	// 1:1거래중 아이템 드롭 금지..
	if ( this->m_btTradeBIT )	return true;
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() )			return true;

	tagITEM sDropITEM;

	// 장착창에서 필드로 직접 드롭 안되도록...
    if ( pPacket->m_cli_DROP_ITEM.m_btInventoryIndex < MAX_EQUIP_IDX ||
		 pPacket->m_cli_DROP_ITEM.m_btInventoryIndex >= INVENTORY_TOTAL_SIZE ) {

		if ( 0 == pPacket->m_cli_DROP_ITEM.m_btInventoryIndex ) {
			// 돈...
			sDropITEM.m_cType  = ITEM_TYPE_MONEY;
			if ( pPacket->m_cli_DROP_ITEM.m_uiQuantity > this->GetCur_MONEY() ) {
				sDropITEM.m_uiMoney = this->GetCur_MONEY();
				this->SetCur_MONEY( 0 );
			} else {
				this->Sub_CurMONEY( (__int64)pPacket->m_cli_DROP_ITEM.m_uiQuantity );
				sDropITEM.m_uiMoney = pPacket->m_cli_DROP_ITEM.m_uiQuantity;
			}

			if ( sDropITEM.m_uiMoney <= 0 )
				return true;

			// 바뀐 돈 정보 전송..
			this->Send_gsv_SET_MONEYONLY( GSV_SET_MONEY_ONLY );
		} else {
	        // 패킷이 조작된것인가?
			return IS_HACKING( this, "Recv_cli_DROP_ITEM-1" );
		}
	} else {
		tagITEM *pITEM = &this->m_Inventory.m_ItemLIST[ pPacket->m_cli_DROP_ITEM.m_btInventoryIndex ];
		if ( 0 == pITEM->GetHEADER() ) {
			// 빈 아이템...
			return true;
		}
		if ( !pITEM->IsEnableDROP() ) {
			// 드롭 불가 아이템이다.
			CStrVAR *pStrVAR = this->GetZONE()->GetStrVAR ();
			return IS_HACKING( this, "Recv_cli_DROP_ITEM-2 : cant drop item" );
		}

		sDropITEM = *pITEM;
		if ( pITEM->IsEnableDupCNT() ) {
			// 매크로등에서 -값이 입력될경우...
			if ( pPacket->m_cli_DROP_ITEM.m_uiQuantity < 0 ) {
				return IS_HACKING( this, "Recv_cli_DROP_ITEM-minus quantity" );
			}

			// 갯수 만큼...
			if ( pPacket->m_cli_DROP_ITEM.m_uiQuantity >= pITEM->GetQuantity() ) {
				sDropITEM.m_uiQuantity = pITEM->GetQuantity();
				// Clear !!!
				m_Inventory.DeleteITEM( pPacket->m_cli_DROP_ITEM.m_btInventoryIndex );
			} else {
				pITEM->m_uiQuantity -= pPacket->m_cli_DROP_ITEM.m_uiQuantity;
				sDropITEM.m_uiQuantity = pPacket->m_cli_DROP_ITEM.m_uiQuantity;
			}

			// 떨어진것 갯수가 0이다 ...
			if ( sDropITEM.GetQuantity() <= 0 )
				return true;

			this->m_Battle.m_nWEIGHT -= ( ITEM_WEIGHT( sDropITEM.m_cType, sDropITEM.m_nItemNo ) * sDropITEM.GetQuantity() );
		} else {
			// 통채로 :: 내부에서 무게 뺀다...
			this->ClearITEM( pPacket->m_cli_DROP_ITEM.m_btInventoryIndex );
		}

		// 바뀐 인벤토리 구조 전송..
		this->Send_gsv_SET_INV_ONLY( pPacket->m_cli_DROP_ITEM.m_btInventoryIndex, pITEM );
	}

	// 떨어진 아이템 전송..
	if ( sDropITEM.GetHEADER() ) {
		CObjITEM *pObjITEM = new CObjITEM;
		if ( pObjITEM ) {
			//
			tPOINTF PosSET;
			PosSET.x = this->m_PosCUR.x + RANDOM( 201 ) - 100;
			PosSET.y = this->m_PosCUR.y + RANDOM( 201 ) - 100;

			pObjITEM->InitItemOBJ( this, PosSET, this->m_PosSECTOR, sDropITEM );	// 사용자 드롭.

			if ( ITEM_TYPE_MONEY == sDropITEM.m_cType )
				g_pThreadLOG->When_TagItemLOG( LIA_DROP, this, NULL, 0, sDropITEM.m_uiMoney );
			else
				g_pThreadLOG->When_TagItemLOG( LIA_DROP, this, &sDropITEM, 0, 0 );

			this->GetZONE()->Add_DIRECT( pObjITEM );	// 드롭 아이템
		}
	}

	return true;
}


//-------------------------------------------------------------------------------------------------
bool classUSER::Check_ItemEventMSG( tagITEM &sITEM )
{
	//if ( sITEM.GetTYPE() >= ITEM_TYPE_JEWEL && sITEM.GetTYPE() < ITEM_TYPE_RIDE_PART ) {
	//	// 서버 공지 필요한 이벤트 아이템이냐 ???
	//	int iMsgID = ITEM_ANNOUNCE_TYPE( sITEM.GetTYPE(), sITEM.GetItemNO() );
	//	if ( 0 == ::Get_ServerLangTYPE() && 1 == iMsgID ) {
	//		CStrVAR *pCStr = this->GetZONE()->GetStrVAR ();

	//		//pCStr->Printf ( "/na %d-채널 서버의 %s님께서 %s 아이템을 획득 하셨습니다.", 
	//			// CLIB_GameSRV::GetInstance()->GetChannelNO(),
	//		pCStr->Printf ( "/na %s님께서 %s 아이템을 획득 하셨습니다.", 
	//			this->Get_NAME(),
	//			ITEM_NAME(sITEM.GetTYPE(), sITEM.GetItemNO() ) );
	//		g_pSockLSV->Send_gsv_CHEAT_REQ( this, 0, 0, pCStr->Get() );

	//		g_LOG.CS_ODS( 0xffff, pCStr->Get() );
	//		g_LOG.CS_ODS( 0xffff, "\n" );
	//		return true;
	//	}
	//}
	return false;
}


//-------------------------------------------------------------------------------------------------
/// 바닦의 아이템을 습득...
bool classUSER::Pick_ITEM( CObjITEM *pITEM )
{
	// 돈 아이템일 경우... ITEM_TYPE( type )에서 뻑~~~~
	if ( ITEM_TYPE_USE == pITEM->m_ITEM.GetTYPE() && 
		 USE_ITEM_VOLATILITY_ITEM == ITEM_TYPE( ITEM_TYPE_USE, pITEM->m_ITEM.GetItemNO() ) ) {
		// 습득시 바로 소모되는 아이템...
		this->Use_pITEM( &pITEM->m_ITEM );

		g_pThreadLOG->When_ObjItemLOG( LIA_PICK, this, pITEM );

		pITEM->m_iRemainTIME = -1;		// Pick_ITEM:: 습득시 돈 삭제되도록...
		pITEM->m_bDropperIsUSER = false;

		return true;
	}

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_GET_FIELDITEM_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_GET_FIELDITEM_REPLY );
	pCPacket->m_gsv_GET_FIELDITEM_REPLY.m_wServerItemIDX = pITEM->Get_INDEX();//pPacket->m_cli_GET_FIELDITEM_REQ.m_wServerItemIDX;

	// 개인 인벤토리에 넣는다.
	pCPacket->m_gsv_GET_FIELDITEM_REPLY.m_nInventoryListNO = this->Add_ITEM( pITEM->m_ITEM );
	if ( pCPacket->m_gsv_GET_FIELDITEM_REPLY.m_nInventoryListNO >= 0 ) {

		pCPacket->m_gsv_GET_FIELDITEM_REPLY.m_btResult = REPLY_GET_FIELDITEM_REPLY_OK;
		pCPacket->m_gsv_GET_FIELDITEM_REPLY.m_ITEM	   = pITEM->m_ITEM;

		if ( ITEM_TYPE_MONEY != pITEM->m_ITEM.GetTYPE() ) {
			this->Set_ItemSN( this->m_Inventory.m_ItemLIST[ pCPacket->m_gsv_GET_FIELDITEM_REPLY.m_nInventoryListNO ] /* pITEM->m_ITEM */ );	// 필드에서 습득시...
		}

		g_pThreadLOG->When_ObjItemLOG( LIA_PICK, this, pITEM );
		if ( !pITEM->m_bDropperIsUSER ) {
			// 사용자가 버린 아이템이 아니라면...
			this->Check_ItemEventMSG( pITEM->m_ITEM );
		}

		pITEM->m_iRemainTIME = -1;		// Pick_ITEM:: 습득시 삭제되도록...
		pITEM->m_bDropperIsUSER = false;
		/*
		// 테스트 이벤트 아이템...
		if ( pITEM->m_ITEM.GetTYPE() == ITEM_TYPE_NATURAL && pITEM->m_ITEM.GetItemNO() == 245 ) {
			CStrVAR *pCStr = this->GetZONE()->GetStrVAR ();

			g_pZoneLIST->Send_gsv_ANNOUNCE_CHAT( "☆★☆★ 축하 합니다 ★☆★☆", NULL  );
			pCStr->Printf ( "%s님께서 %s 지역에서 이벤트 아이템 %s을 획득 하셨습니다.", 
				this->Get_NAME(),
				this->GetZONE()->Get_NAME(),
				ITEM_NAME(ITEM_TYPE_NATURAL, pITEM->m_ITEM.GetItemNO() ) );
			g_pZoneLIST->Send_gsv_ANNOUNCE_CHAT( pCStr->Get(), NULL  );
			pCStr->Printf ( "%s님께서는 획득한 아이템을 마을의 하츠소환술사에게 가져다 주세요.", this->Get_NAME() );
			g_pZoneLIST->Send_gsv_ANNOUNCE_CHAT( pCStr->Get(), NULL  );
		}
		*/
		// ZoneThread에서 SetZone(NULL)이 되면서 ... 아이템 삭제 패킷 전송.
	} else
		pCPacket->m_gsv_GET_FIELDITEM_REPLY.m_btResult = REPLY_GET_FIELDITEM_REPLY_TOO_MANY;

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

/// 바닦의 아이템 습득 패킷 요청 받음
bool classUSER::Recv_cli_GET_FIELDITEM_REQ( t_PACKET *pPacket )
{
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() ) return true;

	BYTE btResult;

	CObjITEM *pITEM = g_pObjMGR->Get_ItemOBJ( pPacket->m_cli_GET_FIELDITEM_REQ.m_wServerItemIDX );
	if ( pITEM ) {
		if ( pITEM->Check_PartyRIGHT( this->GetPARTY() ) ) {
			// 파티 플레이중이고, 소유권이 내 파티에 있다...
			if ( this->GetPARTY()->Is_ShareMODE() ) {
				// 먼저 먹는게 임자다,, 단 돈일경우 공통 분배...
				if ( ITEM_TYPE_MONEY == pITEM->m_ITEM.GetTYPE() ) {
					// 돈이고.. 파티중이며.. 아이템 소유권이 파티에 있고...
					// 아이템 분배 방식이 돈 자동분배.. 아이템 소유권이 파티에

					this->GetPARTY()->Share_MONEY( pITEM->m_ITEM.GetMoney() );

					pITEM->m_iRemainTIME = -1;		// 습득된 돈 삭제되도록...
					pITEM->m_bDropperIsUSER = false;

					return true;
				}
				// 시도한넘 임자
				return this->Pick_ITEM( pITEM );
			}

			// 순서인 사용자에게 자동으로 준다,,, 순서자가 딴존에 있으면 먹은넘이 차지...
			return this->GetPARTY()->Give_Item2ORDER( this, pITEM );
		} else
		if ( pITEM->Is_FREE () ) {
			// 소유권 없는 아이템이다... <<-- 이것도 파티시에는 순서 분배???
			return this->Pick_ITEM( pITEM );
		} else
		if ( pITEM->Check_PrivateRIGHT( this->Get_INDEX() ) )
			return this->Pick_ITEM( pITEM );
		
		// 권한 없다..
		btResult = REPLY_GET_FIELDITEM_REPLY_NO_RIGHT;
	} else {
		btResult = REPLY_GET_FIELDITEM_REPLY_NONE;
	}

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_GET_FIELDITEM_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_GET_FIELDITEM_REPLY );
	pCPacket->m_gsv_GET_FIELDITEM_REPLY.m_wServerItemIDX = pPacket->m_cli_GET_FIELDITEM_REQ.m_wServerItemIDX;

	pCPacket->m_gsv_GET_FIELDITEM_REPLY.m_btResult = btResult;

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

    return true;
}

//-------------------------------------------------------------------------------------------------
/// 줄리를 창고<->인벤 간에 이동
bool classUSER::Recv_cli_MOVE_ZULY( t_PACKET *pPacket )
{
	if ( !(this->m_dwPayFLAG & PLAY_FLAG_STOCK_SPACE ) ) {
		return true;
	}
	if ( BANK_UNLOADED == this->m_btBankData )
		return false;
	if ( pPacket->m_cli_MOVE_ZULY.m_i64MoveZuly < 0 )
		return false;

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	bool bResult = true;
	pCPacket->m_HEADER.m_wType = GSV_MOVE_ZULY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_MOVE_ZULY );

	switch( pPacket->m_cli_MOVE_ZULY.m_btMoveTYPE ) {
		case MOVE_ZULY_TYPE_INV2BANK :	// 보관
			if ( pPacket->m_cli_MOVE_ZULY.m_i64MoveZuly > this->GetCur_MONEY() ) {
				goto _RETURN;
			}
			this->m_Inventory.m_i64Money -= pPacket->m_cli_MOVE_ZULY.m_i64MoveZuly;
			this->m_Bank.m_i64ZULY += pPacket->m_cli_MOVE_ZULY.m_i64MoveZuly;

			break;
		case MOVE_ZULY_TYPE_BANK2INV :	// 꺼냄
			if ( pPacket->m_cli_MOVE_ZULY.m_i64MoveZuly > this->m_Bank.m_i64ZULY ) {
				goto _RETURN;
			}
			this->m_Bank.m_i64ZULY -= pPacket->m_cli_MOVE_ZULY.m_i64MoveZuly;
			this->m_Inventory.m_i64Money += pPacket->m_cli_MOVE_ZULY.m_i64MoveZuly;
			break;

		default :
			bResult = false;
	}

	if ( bResult ) {
		pCPacket->m_gsv_MOVE_ZULY.m_i64InvZuly	= this->GetCur_MONEY();	
		pCPacket->m_gsv_MOVE_ZULY.m_i64BankZuly = this->m_Bank.m_i64ZULY;	

		this->SendPacket( pCPacket );

		this->m_btBankData = BANK_CHANGED;

//-------------------------------------
// 2007.01.17/김대성/추가 - 창고에서 아이템 넣거나 뺄때 로그남기고 사용자 인벤토리 실시간 저장
		if(pPacket->m_cli_MOVE_ZULY.m_btMoveTYPE == MOVE_ZULY_TYPE_INV2BANK)
			g_pThreadLOG->When_TagItemLOG( LIA_DEPOSIT, this, 0, 0, pPacket->m_cli_MOVE_ZULY.m_i64MoveZuly, 0, true);
		else
			g_pThreadLOG->When_TagItemLOG( LIA_WITHDRAW, this, 0, 0, pPacket->m_cli_MOVE_ZULY.m_i64MoveZuly, 0, true);
//-------------------------------------
		//!LYNXLUNA -- update bank data -- error!!
		//g_pThreadSQL->UpdateBankRECORD( this );
	}

_RETURN :
	Packet_ReleaseNUnlock( pCPacket );
	return bResult;
}

/// 아이템을 창고<->인벤 간에 이동 
bool classUSER::Recv_cli_MOVE_ITEM( t_PACKET *pPacket )
{
	if ( !(this->m_dwPayFLAG & PLAY_FLAG_STOCK_SPACE ) ) {
		return true;
	}
	if ( BANK_UNLOADED == this->m_btBankData )
		return false;

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	bool bResult = true;
	pCPacket->m_HEADER.m_wType = GSV_MOVE_ITEM;

	short nToSlotIDX;
	tagITEM *pSourITEM;

	switch( pPacket->m_cli_MOVE_ITEM.m_btMoveTYPE ) {
		case MOVE_ITEM_TYPE_INV2BANK :
		{
			if ( !pPacket->m_cli_MOVE_ITEM.m_MoveITEM.IsEnableKEEPING() ) {
				IS_HACKING(this, "Invalid keeping item");
				bResult = false;
				goto _RETURN;
			}

			if ( pPacket->m_cli_MOVE_ITEM.m_btFromIDX < MAX_EQUIP_IDX || 
				 pPacket->m_cli_MOVE_ITEM.m_btFromIDX >= INVENTORY_TOTAL_SIZE ) {
				IS_HACKING(this, "Invalid keeping inventory index");
				bResult = false;
				goto _RETURN;
			}

			// 렉이 발생하여 같은 아이템을 두번 옮길경우 있을수 있다.
			pSourITEM = &this->m_Inventory.m_ItemLIST[ pPacket->m_cli_MOVE_ITEM.m_btFromIDX ];
			if ( pSourITEM->IsEmpty() ) {
				goto _RETURN;		// bResult = true기때문에 짤리진 않는다.;
			}

			// 메모리 조작에 의해 바뀌는거 방지
			if ( pSourITEM->GetHEADER() != pPacket->m_cli_MOVE_ITEM.m_MoveITEM.GetHEADER() ) {
				IS_HACKING(this, "Difference keeping item");
				bResult = false;
				goto _RETURN;
			}

			tagITEM sOriITEM, sMoveITEM;
			sMoveITEM = pPacket->m_cli_MOVE_ITEM.m_MoveITEM;

			int iFee;
			UINT iDupCnt;
			if ( pSourITEM->IsEnableDupCNT() ) {
				sMoveITEM.m_nItemNo = pSourITEM->GetItemNO();	// 메모리 조작으로 아이템 번호 바뀌는거 차단 !!!
				iDupCnt = pPacket->m_cli_MOVE_ITEM.m_MoveITEM.GetQuantity();
				// 옮기려는 갯수가 더 많으면...
				if ( iDupCnt > pSourITEM->GetQuantity() || iDupCnt > MAX_DUP_ITEM_QUANTITY ) {
					goto _RETURN;	// bResult = true기때문에 짤리진 않는다.;

				}
				if ( pSourITEM->GetQuantity() > MAX_DUP_ITEM_QUANTITY ) {
					// 해킹에 의해 999개이상 옮겨 놓은 아이템이라면 삭제...
					g_pThreadLOG->When_ItemHACKING( this, pSourITEM, "ItemHACK" );
					this->m_Inventory.m_i64Money = 0;
					pSourITEM->Clear();
					goto _RETURN;	// bResult = true기때문에 짤리진 않는다.;
				}
#ifdef	__INC_PLATINUM
				if ( this->m_GrowAbility.IsBankFREE( this->GetCurAbsSEC() ) )
					iFee = 0;
				else
#endif
					iFee = CCal::Get_StorageFEE (
							ITEM_BASE_PRICE(sMoveITEM.GetTYPE(), sMoveITEM.GetItemNO() ),
							ITEM_PRICE_RATE(sMoveITEM.GetTYPE(), sMoveITEM.GetItemNO() ),
							iDupCnt );
			} else {
				sMoveITEM.m_dwBody = pSourITEM->m_dwBody;		// 메모리 조작으로 옵션이 바뀌는거 차단 !!!
#ifdef	__INC_PLATINUM
				if ( this->m_GrowAbility.IsBankFREE( this->GetCurAbsSEC() ) )
					iFee = 0;
				else
#endif
					iFee = CCal::Get_StorageFEE (
								ITEM_BASE_PRICE(sMoveITEM.GetTYPE(), sMoveITEM.GetItemNO() ),
								ITEM_PRICE_RATE(sMoveITEM.GetTYPE(), sMoveITEM.GetItemNO() ),
								1 );
			}
			if ( this->GetCur_MONEY() < iFee ) {
				// 보관료 없다.
				goto _RETURN;		// bResult = true기때문에 짤리진 않는다.;
			}

			sMoveITEM.m_iSN = pSourITEM->m_iSN;
			sOriITEM = *pSourITEM;
			this->Sub_ITEM( pPacket->m_cli_MOVE_ITEM.m_btFromIDX, sMoveITEM );
			// 플레티넘 서비스 텝 가능...
			if ( IsJAPAN() ) {
				if ( this->m_dwPayFLAG & PLAY_FLAG_EXTRA_STOCK ) {
					// 총 120개 사용
					nToSlotIDX = this->m_Bank.Add_ITEM( sMoveITEM, 0, BANKSLOT_JPN_DEFAULT+BANKSLOT_JPN_EXTRA );
				} else {
					// 총 40개 사용
					nToSlotIDX = this->m_Bank.Add_ITEM( sMoveITEM, 0, BANKSLOT_JPN_DEFAULT );
				}
			} else {
	#ifndef	__INC_WORLD
				if ( ( this->m_dwPayFLAG & PLAY_FLAG_EXTRA_STOCK ) && pPacket->m_cli_MOVE_ITEM.m_btUseSpecialTAB == 1 ) {
	#else
				if ( pPacket->m_cli_MOVE_ITEM.m_btUseSpecialTAB == 1 ) {
	#endif
					nToSlotIDX = this->m_Bank.Add_ITEM( sMoveITEM, BANKSLOT_PLATINUM_0, BANKSLOT_PLATINUM_0+BANKSLOT_PLATINUM );
				} else 
				if ( this->m_GrowAbility.IsBankAddON( this->GetCurAbsSEC() ) ) {
					// 창고 확장슬롯 가능......
					nToSlotIDX = this->m_Bank.Add_ITEM( sMoveITEM, 0, BANKSLOT_DEFAULT+BANKSLOT_ADDON );
				} else {
					// 일반 창고....
					nToSlotIDX = this->m_Bank.Add_ITEM( sMoveITEM, 0, BANKSLOT_DEFAULT );
				}
			}

			if ( nToSlotIDX < 0 ) {
				// 실패 :: Add_ITEM 스롯번호의 아이템을 대체 해버리기때문에...사용하면 안됨 !!!
				// this->Add_ITEM( pPacket->m_cli_MOVE_ITEM.m_btFromIDX, sMoveITEM );
				*pSourITEM = sOriITEM;
			}
			else 
			{
//-------------------------------------
/*
					g_pThreadLOG->When_TagItemLOG( LIA_DEPOSIT, this, &sMoveITEM, 0);
*/
// 2007.01.17/김대성/수정 - 창고에서 아이템 넣거나 뺄때 로그남기고 사용자 인벤토리 실시간 저장
					g_pThreadLOG->When_TagItemLOG( LIA_DEPOSIT, this, &sMoveITEM, 0, 0, 0, true);
//-------------------------------------			
				if ( pPacket->m_cli_MOVE_ITEM.m_MoveITEM.IsEnableDupCNT() ) {
					if ( iDupCnt != pPacket->m_cli_MOVE_ITEM.m_MoveITEM.GetQuantity() )
						iFee = CCal::Get_StorageFEE (
									ITEM_BASE_PRICE(sMoveITEM.GetTYPE(), sMoveITEM.GetItemNO() ),
									ITEM_PRICE_RATE(sMoveITEM.GetTYPE(), sMoveITEM.GetItemNO() ),
									pPacket->m_cli_MOVE_ITEM.m_MoveITEM.GetQuantity() );
				}

				this->Sub_CurMONEY( iFee );
			}

			pCPacket->m_HEADER.m_nSize = sizeof( gsv_MOVE_ITEM ) + sizeof(__int64);
			pCPacket->m_gsv_MOVE_ITEM.m_iCurMoney[ 0 ] = this->GetCur_MONEY();
			pCPacket->m_gsv_MOVE_ITEM.m_nInvIDX		   = pPacket->m_cli_MOVE_ITEM.m_btFromIDX;
			pCPacket->m_gsv_MOVE_ITEM.m_nBankIDX	   = nToSlotIDX;
			break;
		}

		case MOVE_ITEM_TYPE_BANK2INV :
			// 꺼내 가는건 결제와 상관없다..
			if ( pPacket->m_cli_MOVE_ITEM.m_btFromIDX >= BANKSLOT_TOTAL ) {
				bResult = false;
				goto _RETURN;
			}

//-------------------------------------
// 2007.01.10/김대성/추가 - 기간제 아이템 - 창고아이템 기간만료 체크
#ifdef __ITEM_TIME_LIMIT
			{
				int i = pPacket->m_cli_MOVE_ITEM.m_btFromIDX;
				if(this->m_Bank.m_ItemLIST[i].check_timeExpired() == true)	// 사용기간만료
				{
					// 삭제메시지 출력
					char buf[1024];
					tagITEM sITEM = this->m_Bank.m_ItemLIST[i];

					time_t long_time = (time_t)(sITEM.dwPickOutTime) + sITEM.get_basicTime();
					tm *l = localtime( &long_time );
					sprintf(buf, "delete item[%d](%s) - %04d/%02d/%02d  %02d:%02d:%02d, %d", i, ItemName(sITEM.m_cType, sITEM.m_nItemNo), 
						l->tm_year + 1900, l->tm_mon + 1, l->tm_mday, l->tm_hour, l->tm_min, l->tm_sec, 
						sITEM.wPeriod);
					this->Send_gsv_WHISPER( this->Get_NAME(), buf );

					// 2007.01.26/김대성/추가 - 기간제 아이템 삭제시 로그남기기
					g_pThreadLOG->When_TagItemLOG( LIA_TIMELIMIT_DELETE, this, &this->m_Bank.m_ItemLIST[i], 0, 0, 0, true);

					this->m_Bank.m_ItemLIST[i].Clear();

					this->Send_gsv_BANK_ITEM_LIST();

					goto _RETURN;
				}
			}
#endif
//-------------------------------------

			pSourITEM = &this->m_Bank.m_ItemLIST[ pPacket->m_cli_MOVE_ITEM.m_btFromIDX ];

			// 렉이 발생하여 같은 아이템을 두번 옮길경우 있을수 있다.
			if ( pSourITEM->IsEmpty() ) {
				goto _RETURN;
			}

			// 메모리 조작에 의해 바뀌는거 방지
			if ( pSourITEM->GetHEADER() != pPacket->m_cli_MOVE_ITEM.m_MoveITEM.GetHEADER() ) {
				IS_HACKING(this, "Difference withdraw item");
				bResult = false;
				goto _RETURN;
			}

			tagITEM sOriITEM, sMoveITEM, sLogITEM;
			sMoveITEM = pPacket->m_cli_MOVE_ITEM.m_MoveITEM;

			if ( pSourITEM->IsEnableDupCNT() ) {
				if ( pSourITEM->GetQuantity() > MAX_DUP_ITEM_QUANTITY ) {
					// 해킹에 의해 999개이상 옮겨 놓은 아이템이라면 삭제...
					g_pThreadLOG->When_ItemHACKING( this, pSourITEM, "BankHACK" );
					this->m_Inventory.m_i64Money = 0;
					pSourITEM->Clear();
					goto _RETURN;
				}

				// 옮기려는 갯수가 더 많으면...
				if ( pPacket->m_cli_MOVE_ITEM.m_MoveITEM.GetQuantity() > pSourITEM->GetQuantity() ||
					 pPacket->m_cli_MOVE_ITEM.m_MoveITEM.GetQuantity() > MAX_DUP_ITEM_QUANTITY ) {
					goto _RETURN;
				}
			} else {
				sMoveITEM.m_dwBody = pSourITEM->m_dwBody;		// 메모리 조작으로 옵션 바뀌는거 차단..
			}

			sOriITEM = *pSourITEM;
			sMoveITEM.m_iSN = pSourITEM->m_iSN;
			sLogITEM = sMoveITEM;

			this->m_Bank.Sub_ITEM( pPacket->m_cli_MOVE_ITEM.m_btFromIDX, sMoveITEM );
			nToSlotIDX = this->Add_ITEM( sMoveITEM );
			if ( nToSlotIDX < 0 ) {
				// 실패 :: Add_ITEM 스롯번호의 아이템을 대체 해버리기때문에...사용하면 안됨 !!!
				// this->m_Bank.Add_ITEM( pPacket->m_cli_MOVE_ITEM.m_btFromIDX, sMoveITEM );
				*pSourITEM = sOriITEM;
			} else {
			
				g_pThreadLOG->When_TagItemLOG( LIA_WITHDRAW, this, &sLogITEM, 0, 0, 0, true);
			
			}

			pCPacket->m_HEADER.m_nSize = sizeof( gsv_MOVE_ITEM );
			pCPacket->m_gsv_MOVE_ITEM.m_nBankIDX = pPacket->m_cli_MOVE_ITEM.m_btFromIDX;
			pCPacket->m_gsv_MOVE_ITEM.m_nInvIDX  = nToSlotIDX;

			
			break;

		default :
			bResult = false;
	}

	if ( bResult ) {
		if ( pCPacket->m_gsv_MOVE_ITEM.m_nInvIDX > 0 )
			pCPacket->m_gsv_MOVE_ITEM.m_InvItem  = this->m_Inventory.m_ItemLIST[ pCPacket->m_gsv_MOVE_ITEM.m_nInvIDX ];

		if ( pCPacket->m_gsv_MOVE_ITEM.m_nBankIDX >= 0 )
			pCPacket->m_gsv_MOVE_ITEM.m_BankITEM = this->m_Bank.m_ItemLIST[ pCPacket->m_gsv_MOVE_ITEM.m_nBankIDX ];

		this->SendPacket( pCPacket );

		this->m_btBankData = BANK_CHANGED;

		//!LYNXLUNA -- update bank data -- ERROR!!!
		//g_pThreadSQL->UpdateBankRECORD( this );
	}

_RETURN :
	Packet_ReleaseNUnlock( pCPacket );

	return bResult;
}

//-------------------------------------------------------------------------------------------------
/// 창고리스트 요청 받음
bool classUSER::Recv_cli_BANK_LIST_REQ( t_PACKET *pPacket )
{
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() ) return true;
	// lynxluna -- query init data everytime open storage
	// but it make the server worse... so I re-enabled it :)
	if ( BANK_UNLOADED == this->m_btBankData ) {
		// DB에 은행 아이템 요청...
		if ( BANK_REQ_OPEN == pPacket->m_cli_BANK_LIST_REQ.m_btREQ )
			return g_pThreadSQL->Add_SqlPacketWithACCOUNT (this, pPacket );
		// 은행 데이타 열리지도 않았는데 비번을 바꿔???
		return true;
	}

	if ( BANK_REQ_CHANGE_PASSWORD == pPacket->m_cli_BANK_LIST_REQ.m_btREQ ) {
		return g_pThreadSQL->Add_SqlPacketWithACCOUNT (this, pPacket );
	}

	if ( this->m_BankPASSWORD.Get() ) {
		if ( pPacket->m_HEADER.m_nSize > sizeof( cli_BANK_LIST_REQ ) ) {
			// 비번 틀리면...
			short nOffset=sizeof( cli_BANK_LIST_REQ );
			char *szPassWD = Packet_GetStringPtr( pPacket, nOffset );
			if ( !szPassWD || strcmp( this->m_BankPASSWORD.Get(), pPacket->m_cli_BANK_LIST_REQ.m_szPassword ) )
				return this->Send_gsv_BANK_LIST_REPLY( BANK_REPLY_INVALID_PASSWORD );
		} else {
			// 비번넣고 패킷전송하라고...
			return this->Send_gsv_BANK_LIST_REPLY( BANK_REPLY_NEED_PASSWORD );
		}
	}

//-------------------------------------
// 2007.01.10/김대성/추가 - 기간제 아이템 - 창고아이템 기간만료 체크
	bool bChange = false;
#ifdef __ITEM_TIME_LIMIT
	for(int i=0; i<BANKSLOT_TOTAL; ++i)
	{
		if(this->m_Bank.m_ItemLIST[i].check_timeExpired() == true)	// 사용기간만료
		{
			// 삭제메시지 출력
			char buf[1024];
			tagITEM sITEM = this->m_Bank.m_ItemLIST[i];

			time_t long_time = (time_t)(sITEM.dwPickOutTime) + sITEM.get_basicTime();
			tm *l = localtime( &long_time );
			sprintf(buf, "delete item[%d](%s) - %04d/%02d/%02d  %02d:%02d:%02d, %d", i, ItemName(sITEM.m_cType, sITEM.m_nItemNo), 
			l->tm_year + 1900, l->tm_mon + 1, l->tm_mday, l->tm_hour, l->tm_min, l->tm_sec, 
			sITEM.wPeriod);
			this->Send_gsv_WHISPER( this->Get_NAME(), buf );

			// 2007.01.26/김대성/추가 - 기간제 아이템 삭제시 로그남기기
			g_pThreadLOG->When_TagItemLOG( LIA_TIMELIMIT_DELETE, this, &this->m_Bank.m_ItemLIST[i], 0, 0, 0, true);

			this->m_Bank.m_ItemLIST[i].Clear();
			bChange = true;
		}
	}

	if(bChange == true)
		return this->Send_gsv_BANK_ITEM_LIST();
#endif
//-------------------------------------

	return this->Send_gsv_BANK_LIST_REPLY( BANK_REPLY_SHOW_DATA );
	// this->Send_gsv_BANK_ITEM_LIST();
}

//-------------------------------------------------------------------------------------------------
/// 워프 요청 패킷
#define	MAX_WARP_OBJECT_DISTANCE	( 1000 * 5 )
short classUSER::Recv_cli_TELEPORT_REQ( t_PACKET *pPacket )
{
	if ( !this->GetZONE() ) 
		return RET_FAILED;

    WORD wWarpIDX = (WORD)( pPacket->m_cli_TELEPORT_REQ.m_nWarpIDX );
	if ( wWarpIDX >= g_TblWARP.m_nDataCnt ) {
		g_LOG.CS_ODS( 0xffff, "[ %s:%s ] Invalied Warp INDEX: %d", this->m_IP.Get(), this->Get_NAME(), wWarpIDX );
		return IS_HACKING( this, "Recv_cli_TELEPORT_REQ-1" );
	}
	if ( TELEPORT_ZONE( wWarpIDX ) <= 0 || NULL == TELEPORT_EVENT_POS(wWarpIDX) ) {
		g_LOG.CS_ODS( 0xffff, "[ %s:%s ] Invalied Warp INDEX: %d", this->m_IP.Get(), this->Get_NAME(), wWarpIDX );
		return IS_HACKING( this, "Recv_cli_TELEPORT_REQ-11" );
	}

	if ( sizeof( cli_TELEPORT_REQ ) == pPacket->m_HEADER.m_nSize ) {
		// 스핵체크용 클라이언트 좌표 비교
		int iDistance = distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)pPacket->m_cli_TELEPORT_REQ.m_PosCUR.x, (int)pPacket->m_cli_TELEPORT_REQ.m_PosCUR.y);
		if ( iDistance > MAX_WARP_OBJECT_DISTANCE )	{ 
			// 서버의 위치로 클라이언트 복귀...
			return this->Send_gsv_ADJUST_POS( true );
		}
	} else 
	if ( IsTAIWAN() ) {
#ifndef	__INC_WORLD
		return RET_OK;	// IS_HACKING( this, "Recv_cli_TELEPORT_REQ-4" );
#endif
	}
	/*
	// 체크해 말어 ?
	if ( !g_pZoneLIST->IsValidZONE( TELEPORT_ZONE( nWarpIDX ) ) {
		g_LOG.CS_ODS( 0xffff, "ERROR:: Invalid Warp Zone ... WarpIDX: %d, ZoneNO: %d \n", nWarpIDX, TELEPORT_ZONE( nWarpIDX ) );
		return IS_HACKING( this, "Recv_cli_TELEPORT_REQ-2" );
	}
	*/

	// 같은 서버에서 존이 실행 되고 있는가 ???
	tagEVENTPOS *pEventPOS = g_pZoneLIST->Get_EventPOS( TELEPORT_ZONE( wWarpIDX ), TELEPORT_EVENT_POS(wWarpIDX) );
	if ( !pEventPOS ) {
		g_LOG.CS_ODS( 0xffff, "ERROR:: Invalid Warp Position... WarpIDX: %d \n", wWarpIDX );
		return IS_HACKING( this, "Recv_cli_TELEPORT_REQ-3" );
	}

	return this->Proc_TELEPORT( pEventPOS->m_nZoneNO, pEventPOS->m_Position );	// Recv_cli_TELEPORT_REQ
}


//-------------------------------------------------------------------------------------------------
/// 스탯 변경 요청 받음
bool classUSER::Recv_cli_USE_BPOINT_REQ( t_PACKET *pPacket )
{
	if ( this->m_IngSTATUS.IsSET( FLAG_ING_IGNORE_ALL ) ) return true;

	if ( pPacket->m_cli_USE_BPOINT_REQ.m_btAbilityNO >= BA_MAX ) {
		return IS_HACKING (this, "cli_USE_POINTS-2" );
	}

	short nNeedPoints, nAbilityValue, nCurSpeed;

	// 기본 능력치 상승...
	nNeedPoints = this->Get_NeedPoint2AbilityUP( pPacket->m_cli_USE_BPOINT_REQ.m_btAbilityNO );
	if ( this->GetCur_BonusPOINT() < nNeedPoints ) {
		// 포인트 모잘란다..
		return true;
	}

	if ( this->m_BasicAbility.m_nBasicA[ pPacket->m_cli_USE_BPOINT_REQ.m_btAbilityNO ] >= MAX_BASIC_ABLITY ) {
		// 더이상 올릴수 없다.
		return true;
	}

	this->m_BasicAbility.m_nBasicA[ pPacket->m_cli_USE_BPOINT_REQ.m_btAbilityNO ] ++;

	nCurSpeed = this->GetOri_RunSPEED();
	this->UpdateAbility ();		// use_bpoint

	nAbilityValue = this->m_BasicAbility.m_nBasicA[ pPacket->m_cli_USE_BPOINT_REQ.m_btAbilityNO ];

	this->SetCur_BonusPOINT( this->GetCur_BonusPOINT() - nNeedPoints );

	/* kchs-modify
	2. 캐릭터 스탯
	1) 스탯 변동
	① 변동된 스탯의 종류 및 소모 포인트
	② 날짜/시간/ IP/좌표*/
	g_pThreadLOG->When_ChangeABILITY ( this , pPacket->m_cli_USE_BPOINT_REQ.m_btAbilityNO, nNeedPoints );

    classPACKET *pCPacket = Packet_AllocNLock ();
	//////////////////////////////////////////////////////////////////////////
	// 2006.06.12:이성활:pCPacket에 대한 Null 체크
	if (!pCPacket) {
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

    pCPacket->m_HEADER.m_wType = GSV_USE_BPOINT_REPLY;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_USE_BPOINT_REPLY );

    pCPacket->m_gsv_USE_BPOINT_REPLY.m_btAbilityNO	= pPacket->m_cli_USE_BPOINT_REQ.m_btAbilityNO;
    pCPacket->m_gsv_USE_BPOINT_REPLY.m_nAbilityValue = nAbilityValue;

	this->SendPacket( pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	if ( nCurSpeed != this->GetOri_RunSPEED() )  {
		// 이동 속도가 바뀌므로 주변에 이동 속도 전송...
		this->Send_gsv_SPEED_CHANGED ();
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 1:1 거래시 정상적인 거래 상황인지 체크...
bool classUSER::Check_TradeITEM ()
{
	// 1개의 아이템을 해킹으로 여러 거래 슬롯에 올려 놓고 거래 시도할때...
	//char cCheckedSlot[ INVENTORY_TOTAL_SIZE ];
	//::ZeroMemory( cCheckedSlot, INVENTORY_TOTAL_SIZE );
	short nI;
	for ( nI=0; nI<TRADE_MONEY_SLOT_NO; nI++) 
	{
		if ( !this->m_TradeITEM[ nI ].m_Item.GetTYPE()&& this->m_TradeITEM[ nI ].m_Item.GetTYPE()>=ITEM_TYPE_RIDE_PART ) 
				return false;

		if ( m_TradeITEM[ nI ].m_Item.GetHEADER() != m_Inventory.m_ItemLIST[ m_TradeITEM[ nI ].m_nInvIDX ].GetHEADER() ) 
		{
			// 타입 체크...
			return false;
		}

		if ( m_Inventory.m_ItemLIST[ m_TradeITEM[ nI ].m_nInvIDX ].IsEnableDupCNT() ) 
		{
			// 중복 갯수 아이템..
			if ( m_Inventory.m_ItemLIST[ m_TradeITEM[ nI ].m_nInvIDX ].GetQuantity() < m_TradeITEM[ nI ].m_Item.GetQuantity() ) 
			{
				// 수량 체크..
				return false;
			}
		} 
		else if ( m_TradeITEM[ nI ].m_Item.m_dwBody != m_Inventory.m_ItemLIST[ m_TradeITEM[ nI ].m_nInvIDX ].m_dwBody ) 
		{
			// 장비 아이템일 경우 옵션/제밍/강화 체크...
			return false;
		}

			// if ( cCheckedSlot[ this->m_TradeITEM[ nI ].m_nInvIDX ] )
			//	return false;
			// cCheckedSlot[ this->m_TradeITEM[ nI ].m_nInvIDX ] = 1;
		
	}

	if ( this->m_TradeITEM[ TRADE_MONEY_SLOT_NO ].m_Item.GetTYPE()== ITEM_TYPE_MONEY )
	{
		if ( this->GetCur_MONEY() < m_TradeITEM[ TRADE_MONEY_SLOT_NO ].m_Item.m_uiMoney )
			return false;
	}
	else if(this->m_TradeITEM[ nI ].m_Item.GetTYPE())
	{
		return false;
	}
	return true;
}

/// 1:1 거래 완료시 건네준 아이템을 인벤토리에서 제거
BOOL classUSER::RemoveTradeItemFromINV (classUSER *pTradeUSER, classPACKET *pCPacket)
{
	for (short nI=0; nI<TRADE_MONEY_SLOT_NO; nI++)
	{
		if ( this->m_TradeITEM[ nI ].m_Item.GetTYPE()&& this->m_TradeITEM[ nI ].m_Item.GetTYPE()<=ITEM_TYPE_RIDE_PART ) 
		{
			// 아이템 줬다.. 빼자 !!!
			this->Sub_ITEM( m_TradeITEM[ nI ].m_nInvIDX, m_TradeITEM[ nI ].m_Item );
		
				g_pThreadLOG->When_TagItemLOG( LIA_GIVE, this, &m_TradeITEM[ nI ].m_Item, 0, 0, pTradeUSER);
	

			// 변경된 인벤토리
			pCPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_btInvIDX = (BYTE)m_TradeITEM[ nI ].m_nInvIDX;
			pCPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_ITEM	 = m_Inventory.m_ItemLIST[ m_TradeITEM[ nI ].m_nInvIDX ];
			pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ++;
		}
		
	}

	if ( this->m_TradeITEM[ TRADE_MONEY_SLOT_NO ].m_Item.GetTYPE() == ITEM_TYPE_MONEY ) 
	{
		// 돈줬다... 빼자 !!!

		if( m_TradeITEM[ TRADE_MONEY_SLOT_NO ].m_Item.GetMoney()>this->Get_MONEY())  //가지고 있는던 보다 크면..
		{
			IS_HACKING(pTradeUSER,"RemoveTradeItemFromINV");
			return false;
		}
		else
		{
			g_pThreadLOG->When_TagItemLOG( LIA_GIVE, this, NULL, 0, m_TradeITEM[ TRADE_MONEY_SLOT_NO ].m_Item.GetMoney(), pTradeUSER);
			this->Sub_ITEM( m_TradeITEM[ TRADE_MONEY_SLOT_NO ].m_nInvIDX, m_TradeITEM[ TRADE_MONEY_SLOT_NO ].m_Item );
		}
	}
	
	return true;
}

/// 1:1거래 완료시 받은 아이템을 인벤토리에 추가
BOOL classUSER::AddTradeItemToINV (classUSER *pTradeUSER/*tagTradeITEM *pTradeITEM*/, classPACKET *pCPacket)
{
	short nI, nInvIDX;

	for (nI=0; nI<TRADE_MONEY_SLOT_NO; nI++)
	{
		if (pTradeUSER->m_TradeITEM[ nI ].m_Item.GetTYPE()&& pTradeUSER->m_TradeITEM[ nI ].m_Item.GetTYPE()<=ITEM_TYPE_RIDE_PART ) 
		{
			// 더하자 !!!
			nInvIDX = this->Add_ITEM( pTradeUSER->m_TradeITEM[ nI ].m_Item );	
			g_pThreadLOG->When_TagItemLOG( LIA_RECV, this, &pTradeUSER->m_TradeITEM[ nI ].m_Item, 0, 0, pTradeUSER);
			
			// 변경된 인벤토리
			if ( nInvIDX > 0 ) 
			{
				pCPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_btInvIDX = (BYTE)nInvIDX;
				pCPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_ITEM	 = m_Inventory.m_ItemLIST[ nInvIDX ];
				pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ++;
			}
			else
			{
				this->Save_ItemToFILED( pTradeUSER->m_TradeITEM[ nI ].m_Item );	// 1:1 거래후 아이템 바닦에 드롭....소유시간 길게~~~~
			}
		}
	}

	if ( pTradeUSER->m_TradeITEM[ TRADE_MONEY_SLOT_NO ].m_Item.GetTYPE() == ITEM_TYPE_MONEY ) 
	{
		// 돈 받았음..
		g_pThreadLOG->When_TagItemLOG( LIA_RECV, this, NULL, 0, pTradeUSER->m_TradeITEM[ TRADE_MONEY_SLOT_NO ].m_Item.GetMoney(), pTradeUSER);
		this->Add_ITEM( pTradeUSER->m_TradeITEM[ TRADE_MONEY_SLOT_NO ].m_Item );
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
/// 거래 요청 패킷..
bool classUSER::Recv_cli_TRADE_P2P( t_PACKET *pPacket )
{
	if ( this->m_iIndex == pPacket->m_cli_TRADE_P2P.m_wObjectIDX ) {
		// 로즈해커 deebee의 메일로 인해 알게된 해킹방법 ::
		// 1:1거래시 해킹을 통해서 자신과 거래를 하게 하는 방법으로 아이템 복사...
		return IS_HACKING( this, "Recv_cli_TRADE_P2P :: Trade from self" );
	}

	classUSER *pUSER;

	pUSER = g_pObjMGR->Get_UserOBJ( pPacket->m_cli_TRADE_P2P.m_wObjectIDX );
	if ( pUSER )
	{
		switch( pPacket->m_cli_TRADE_P2P.m_btRESULT ) 
		{
			case RESULT_TRADE_REQUEST :		// 거래 요청
			{
				if ( !(this->m_dwPayFLAG & PLAY_FLAG_TRADE) )
				{
					return true;
				}
				if ( !(pUSER->m_dwPayFLAG & PLAY_FLAG_TRADE) ) 
				{
					return this->Send_gsv_TRADE_P2P( pPacket->m_cli_TRADE_P2P.m_wObjectIDX, RESULT_TRADE_NO_CHARGE_TARGET );
				}
					 
				if ( pUSER->m_iTradeUserIDX || pUSER->m_IngSTATUS.IsIgnoreSTATUS() ) {
					// 대상이 바쁘다..
					return this->Send_gsv_TRADE_P2P( pPacket->m_cli_TRADE_P2P.m_wObjectIDX, RESULT_TRADE_BUSY );
				}

				int iDistance = distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)pUSER->m_PosCUR.x, (int)pUSER->m_PosCUR.y);
				if ( iDistance > MAX_TRADE_DISTANCE )	{ 
					// 거래 요청 pUSER와의 거리는 체크후 자신에게 거래취소 전송...
					return this->Send_gsv_TRADE_P2P( pPacket->m_cli_TRADE_P2P.m_wObjectIDX, RESULT_TRADE_TOO_FAR );
				}

				this->m_btTradeBIT = 0;
				this->m_iTradeUserIDX = pPacket->m_cli_TRADE_P2P.m_wObjectIDX;
				break;
			}

			case RESULT_TRADE_ACCEPT :		// 거래 승인 했다.
			{
				if ( pUSER->m_iTradeUserIDX != this->Get_INDEX() )
				{
					return this->Send_gsv_TRADE_P2P( pPacket->m_cli_TRADE_P2P.m_wObjectIDX, RESULT_TRADE_CANCEL );
				}
				if ( this->m_IngSTATUS.IsIgnoreSTATUS() || pUSER->m_IngSTATUS.IsIgnoreSTATUS() ) 
				{
					pUSER->Send_gsv_TRADE_P2P( pPacket->m_cli_TRADE_P2P.m_wObjectIDX, RESULT_TRADE_BUSY );
					this->Send_gsv_TRADE_P2P( pPacket->m_cli_TRADE_P2P.m_wObjectIDX, RESULT_TRADE_BUSY );
					return true;
				}

				this->m_btTradeBIT = pUSER->m_btTradeBIT = BIT_TRADE_ACCEPT;
				this->m_iTradeUserIDX = pUSER->Get_INDEX();

				::ZeroMemory( pUSER->m_TradeITEM,	sizeof(tagTradeITEM) * MAX_TRADE_ITEM_SLOT );
				::ZeroMemory( this->m_TradeITEM,	sizeof(tagTradeITEM) * MAX_TRADE_ITEM_SLOT );
				break;
			}

			case RESULT_TRADE_REJECT :			// 거래 거부.
			case RESULT_TRADE_CANCEL :			// 거래 도중 취소.
			{
				if ( pUSER->m_iTradeUserIDX == this->Get_INDEX() ) 
				{
					pUSER->m_btTradeBIT = pUSER->m_iTradeUserIDX = 0;
				}
				this->m_btTradeBIT  = this->m_iTradeUserIDX  = 0;
				break;
			} 

			case RESULT_TRADE_DONE :			// 거래 실행.
			{
				// 서로 대상에 변화가 없나 ?
				if ( this->m_iTradeUserIDX != pUSER->Get_INDEX() || pUSER->m_iTradeUserIDX != this->Get_INDEX() ) 
				{
					this->m_btTradeBIT = this->m_iTradeUserIDX = 0;
					pUSER->m_btTradeBIT = pUSER->m_iTradeUserIDX = 0;
					return this->Send_gsv_TRADE_P2P( pPacket->m_cli_TRADE_P2P.m_wObjectIDX, RESULT_TRADE_CANCEL );
				}

				// 둘다 준비가 완료 됐나 ?
				this->m_btTradeBIT |= BIT_TRADE_DONE;
				if ( pUSER->m_btTradeBIT != ( BIT_TRADE_READY | BIT_TRADE_DONE |BIT_TRADE_ACCEPT) ||
					 this->m_btTradeBIT  != ( BIT_TRADE_READY | BIT_TRADE_DONE |BIT_TRADE_ACCEPT) ) 
				{
					return true;
				}

				// 죽거나 개인상점 모드냐 ?
				if ( this->m_IngSTATUS.IsIgnoreSTATUS() || 
					 pUSER->m_IngSTATUS.IsIgnoreSTATUS() ||
					!this->Check_TradeITEM () || 
					!pUSER->Check_TradeITEM () )
				{
					this->m_btTradeBIT = this->m_iTradeUserIDX = 0;
					pUSER->m_btTradeBIT = pUSER->m_iTradeUserIDX = 0;
					this->Send_gsv_TRADE_P2P( pPacket->m_cli_TRADE_P2P.m_wObjectIDX, RESULT_TRADE_CANCEL );
					pUSER->Send_gsv_TRADE_P2P( pPacket->m_cli_TRADE_P2P.m_wObjectIDX, RESULT_TRADE_CANCEL );
					return true;
				}

				// 실제 물건 교환 할곳...
				classPACKET *pCPacket1 = Packet_AllocNLock ();
				//////////////////////////////////////////////////////////////////////////
				// 2006.06.12:이성활:pCPacket1, pCPacket2에 대한 Null 체크
				if (!pCPacket1) {
					return false;
				}

				classPACKET *pCPacket2 = Packet_AllocNLock ();
				if (!pCPacket2) {
					Packet_ReleaseNUnlock(pCPacket1);
					return false;
				}
				//////////////////////////////////////////////////////////////////////////


				pCPacket1->m_HEADER.m_wType = GSV_SET_MONEYnINV;
				pCPacket1->m_HEADER.m_nSize = sizeof( gsv_SET_MONEYnINV );
				pCPacket1->m_gsv_SET_MONEYnINV.m_btItemCNT = 0;

				pCPacket2->m_HEADER.m_wType = GSV_SET_MONEYnINV;
				pCPacket2->m_HEADER.m_nSize = sizeof( gsv_SET_MONEYnINV );
				pCPacket2->m_gsv_SET_MONEYnINV.m_btItemCNT = 0;

				if(!this->RemoveTradeItemFromINV ( pUSER, pCPacket1 )||!pUSER->RemoveTradeItemFromINV ( this, pCPacket2 ))
				{
					Packet_ReleaseNUnlock( pCPacket1 );
					Packet_ReleaseNUnlock( pCPacket2 );
					return false;
				}

				if(!this->AddTradeItemToINV( pUSER/*pUSER->m_TradeITEM*/, pCPacket1 )||!pUSER->AddTradeItemToINV( this/*this->m_TradeITEM*/, pCPacket2 ))
				{
					Packet_ReleaseNUnlock( pCPacket1 );
					Packet_ReleaseNUnlock( pCPacket2 );
					return false;
				}

				pCPacket1->m_HEADER.m_nSize += ( pCPacket1->m_gsv_SET_MONEYnINV.m_btItemCNT * sizeof( tag_SET_INVITEM ) );
				pCPacket1->m_gsv_SET_MONEYnINV.m_i64Money = this->GetCur_MONEY();

				pCPacket2->m_HEADER.m_nSize += ( pCPacket2->m_gsv_SET_MONEYnINV.m_btItemCNT * sizeof( tag_SET_INVITEM ) );
				pCPacket2->m_gsv_SET_MONEYnINV.m_i64Money = pUSER->GetCur_MONEY();

				this->SendPacket( pCPacket1 );
				pUSER->SendPacket( pCPacket2 );

				Packet_ReleaseNUnlock( pCPacket1 );
				Packet_ReleaseNUnlock( pCPacket2 );

				pUSER->Send_gsv_TRADE_P2P( this->Get_INDEX(), RESULT_TRADE_DONE, 0 );
				this->Send_gsv_TRADE_P2P( pUSER->Get_INDEX(), RESULT_TRADE_DONE, 0 );

				this->m_btTradeBIT = this->m_iTradeUserIDX  = 0;
				pUSER->m_btTradeBIT = pUSER->m_iTradeUserIDX = 0;
				return true;
			}
			case RESULT_TRADE_CHECK_READY	:	// 거래 준비 완료.
				this->m_btTradeBIT |= BIT_TRADE_READY;
				break;
			case RESULT_TRADE_UNCHECK_READY :	// 거래 준비 해제
				this->m_btTradeBIT &= ~BIT_TRADE_READY ;
				pUSER->m_btTradeBIT &= ~BIT_TRADE_DONE;
				break;
			case RESULT_TRADE_OUT_OF_INV :		// 인벤토리가 꽉차서 받을수 없다.
				if ( pPacket->m_cli_TRADE_P2P.m_cTradeSLOT < 0 ||
					 pPacket->m_cli_TRADE_P2P.m_cTradeSLOT >= MAX_TRADE_ITEM_SLOT ) 
				{
					return IS_HACKING (this, "Recv_cli_TRADE_P2P" ); 
				}

				pUSER->m_TradeITEM[ pPacket->m_cli_TRADE_P2P.m_cTradeSLOT ].m_Item.m_cType = 0;
				break;
		}

		pUSER->Send_gsv_TRADE_P2P( this->Get_INDEX(), pPacket->m_cli_TRADE_P2P.m_btRESULT, pPacket->m_cli_TRADE_P2P.m_cTradeSLOT );
	} else {
		// 대상이 없다.
		this->m_btTradeBIT = this->m_iTradeUserIDX = 0;
		this->Send_gsv_TRADE_P2P( this->m_iTradeUserIDX, RESULT_TRADE_NOT_TARGET );
	}

    return true;
}

/// 거래 아이템 등록..
bool classUSER::Recv_cli_TRADE_P2P_ITEM( t_PACKET *pPacket )
{
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() ) return true;
	if ( this->m_btTradeBIT != BIT_TRADE_ACCEPT ) return true;  //거래 승인 상태가 아니면.. 물건을 올릴수없다..

	if ( pPacket->m_cli_TRADE_P2P_ITEM.m_nInventoryIndex < 0 ||
		 pPacket->m_cli_TRADE_P2P_ITEM.m_nInventoryIndex >= INVENTORY_TOTAL_SIZE ) 
	{
		return IS_HACKING (this, "Recv_cli_TRADE_P2P_ITEM-1 : Invalid inv idx" ); 
	}
	if ( pPacket->m_cli_TRADE_P2P_ITEM.m_cTradeSLOT < 0 ||
		 pPacket->m_cli_TRADE_P2P_ITEM.m_cTradeSLOT >= MAX_TRADE_ITEM_SLOT ) 
	{
		return IS_HACKING (this, "Recv_cli_TRADE_P2P_ITEM-2 : Invalid trade slot" ); 
	}

	classUSER *pUSER;
	pUSER = g_pObjMGR->Get_UserOBJ( this->m_iTradeUserIDX );

	if ( NULL == pUSER ) 
	{
		// 거래 대상이 없어 졌다... 거래 취소 ...
		this->Send_gsv_TRADE_P2P( this->m_iTradeUserIDX , RESULT_TRADE_NOT_TARGET );
		this->m_iTradeUserIDX = 0;
		return true;
	}

	if ( this->m_iTradeUserIDX != pUSER->Get_INDEX() || 
		 pUSER->m_iTradeUserIDX != this->Get_INDEX() ) 
	{
		this->m_iTradeUserIDX = 0;
		return true;
	}

	tagITEM *pTradeITEM = &this->m_TradeITEM[ pPacket->m_cli_TRADE_P2P_ITEM.m_cTradeSLOT ].m_Item;

	if ( pPacket->m_cli_TRADE_P2P_ITEM.m_uiQuantity > 0 ) 
	{
		// HACKING !!! :: 2004. 10. 11
		// pPacket->m_cli_TRADE_P2P_ITEM.m_nInventoryIndex == 0이고 TRADE_MONEY_SLOT_NO != pPacket->m_cli_TRADE_P2P_ITEM.m_cTradeSLOT 
		// 도록 조작해서 패킷을 보내는 넘이 있음. 헤킹에 의해 돈이 들어온다... 나쁜 쉑~
		if ( TRADE_MONEY_SLOT_NO == pPacket->m_cli_TRADE_P2P_ITEM.m_cTradeSLOT ) 
		{
			// 돈
			pPacket->m_cli_TRADE_P2P_ITEM.m_cTradeSLOT = TRADE_MONEY_SLOT_NO;
			pTradeITEM->m_cType = ITEM_TYPE_MONEY;

			if ( pPacket->m_cli_TRADE_P2P_ITEM.m_uiQuantity > this->GetCur_MONEY() )
			{
				pPacket->m_cli_TRADE_P2P_ITEM.m_uiQuantity = (int)this->GetCur_MONEY();
			}

			pTradeITEM->m_uiMoney = pPacket->m_cli_TRADE_P2P_ITEM.m_uiQuantity; 
		}
		else 
		{
			// HACKING !!! :: 2005. 05. 30 Other transactions such as inventory of the items, copy items in the slot by registering ... ~
			for (short nT=0; nT<TRADE_MONEY_SLOT_NO; nT++) 
			{
				if ( this->m_TradeITEM[ nT ].m_nInvIDX == pPacket->m_cli_TRADE_P2P_ITEM.m_nInventoryIndex ) 
				{
					return IS_HACKING (this, "Recv_cli_TRADE_P2P_ITEM-3 : Duplicated inventory item" ); 
				}
			}

			// The slots are already trading items. (Rose-mail because of hackers deebee learned how to hack)
			if( this->m_TradeITEM[ pPacket->m_cli_TRADE_P2P_ITEM.m_cTradeSLOT ].m_Item.GetItemNO() != 0 ) 
				return IS_HACKING (this, "Recv_cli_TRADE_P2P_ITEM-4 : Invalid trade slot" ); 

			*pTradeITEM = this->m_Inventory.m_ItemLIST[ pPacket->m_cli_TRADE_P2P_ITEM.m_nInventoryIndex ];
			if ( !pTradeITEM->IsEnableDROP() ) 
			{
				// 1:1 거래 불가 품목..
				return true;
			}

			if ( 0 == pPacket->m_cli_TRADE_P2P_ITEM.m_nInventoryIndex || ITEM_TYPE_MONEY == pTradeITEM->GetTYPE() ) 
			{
				// Hacking ... 패킷 조작인가???
				IS_HACKING( this, "classUSER::Recv_cli_TRADE_P2P_ITEM( Hacking Money slot )" );
				this->SetCur_MONEY( 0 );
				return false;
			}

			if ( tagITEM::IsEnableDupCNT( pTradeITEM->GetTYPE() ) ) 
			{
				if ( pPacket->m_cli_TRADE_P2P_ITEM.m_uiQuantity > pTradeITEM->GetQuantity() ) 
				{
					// 수량이 모자라다.
					pPacket->m_cli_TRADE_P2P_ITEM.m_uiQuantity = pTradeITEM->GetQuantity();
				}

				pTradeITEM->m_uiQuantity = pPacket->m_cli_TRADE_P2P_ITEM.m_uiQuantity;
			}
		}
	}
	else 
	{
		pTradeITEM->Clear();		// 거래 품목에서 뺏다...
	}

	this->m_TradeITEM[ pPacket->m_cli_TRADE_P2P_ITEM.m_cTradeSLOT ].m_nInvIDX = pPacket->m_cli_TRADE_P2P_ITEM.m_nInventoryIndex;

	pUSER->Send_gsv_TRADE_P2P_ITEM( pPacket->m_cli_TRADE_P2P_ITEM.m_cTradeSLOT, *pTradeITEM );

	return true;
}


//-------------------------------------------------------------------------------------------------
/// 개인 상점이 열렸음을 통보
bool classUSER::Send_gsv_P_STORE_OPENED()
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_P_STORE_OPENED;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_P_STORE_OPENED );

	pCPacket->m_gsv_P_STORE_OPENED.m_wObjectIDX = this->Get_INDEX();

#ifdef	__INC_PLATINUM
	short nStoreSkin = this->m_GrowAbility.GetStoreSKIN( this->GetCurAbsSEC() );
	pCPacket->AppendData( &nStoreSkin, sizeof(short) );
#endif
	pCPacket->AppendString( this->m_szUserTITLE );

    this->GetZONE()->SendPacketToSectors( this, pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	return true;
}


//-------------------------------------------------------------------------------------------------
/// 구입 희망 아이템을 등록
bool classUSER::Recv_cli_SET_WISHITEM( t_PACKET *pPacket )
{
	if ( tagITEM::IsValidITEM( &pPacket->m_cli_SET_WISHITEM.m_ITEM ) )
		return this->m_WishLIST.Set( pPacket->m_cli_SET_WISHITEM.m_btWishSLOT, pPacket->m_cli_SET_WISHITEM.m_ITEM );
	return true;
}


//-------------------------------------------------------------------------------------------------
/// 개인 상점 열기 요청을 받음
bool classUSER::Recv_cli_P_STORE_OPEN( t_PACKET *pPacket )
{
	if ( !(this->m_dwPayFLAG & PAY_FLAG_JP_TRADE) )
		return true;

	// PVP존에서는 개인상점 열수 없다...
	if ( this->Get_ActiveSKILL() ) return true;	// 스킬 케스팅 중일땐 못바꿔
	if ( ZONE_PVP_STATE( this->m_nZoneNO ) && 11 != ZONE_PVP_STATE( this->m_nZoneNO ) ) return true;

	if ( this->m_IngSTATUS.IsIgnoreSTATUS() ) return true;
	if ( this->Get_RideMODE() ) return true;

	if ( pPacket->m_cli_P_STORE_OPEN.m_btSellItemCNT >= MAX_P_STORE_ITEM_SLOT )
		return false;
	if ( pPacket->m_cli_P_STORE_OPEN.m_btWishItemCNT >= MAX_P_STORE_ITEM_SLOT )
		return false;

	if(pPacket->m_cli_P_STORE_OPEN.m_btSellItemCNT==0&&pPacket->m_cli_P_STORE_OPEN.m_btWishItemCNT ==0)
		return false;

	if ( !this->SetCMD_STOP() || this->Get_RideMODE() )
		return true;

	// 상점 열기 상태로
	// 정지 명령 상태로 ?
	this->m_STORE.m_bActive = true;
	this->Set_COMMAND( CMD_STORE );

	this->m_IngSTATUS.ClearSubFLAG( FLAG_SUB_INTRO_CHAT );
	this->m_IngSTATUS.SetSubFLAG( FLAG_SUB_STORE_MODE );

	short nI, nJ, nK, nOffset;

	this->m_STORE.m_btSellCNT = 0;
	tagITEM *pInvITEM;
	for (nI=0; nI<pPacket->m_cli_P_STORE_OPEN.m_btSellItemCNT; nI++) {
		if ( pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nI ].m_btInvIDX >= INVENTORY_TOTAL_SIZE )
			continue;

		// 뭐냐... 해킹 ??
		if ( !tagITEM::IsValidITEM( &pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nI ].m_ITEM ) ) 
			continue;

		// 거래 할수 있는 아이템인가 ???
		if ( !pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nI ].m_ITEM.IsEnableDROP() ) {
			// 아이템 거래 유형이 2,3번 이면 개인 상점 거래 불가..
			this->m_STORE.m_SellITEM[ nI ].Clear();
			continue;
		}
		pInvITEM = &this->m_Inventory.m_ItemLIST[ pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nI ].m_btInvIDX ];
		if ( pInvITEM->GetHEADER() != pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nI ].m_ITEM.GetHEADER() ) {
			this->m_STORE.m_SellITEM[ nI ].Clear();
			continue;
		}

		if ( pInvITEM->IsEnableDupCNT() && pInvITEM->GetQuantity() > pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nI ].m_ITEM.GetQuantity() ) {
			// 중복된 갯수아이템이고 소지량보다 적게 팔려고 하면...
			this->m_STORE.m_SellITEM[ nI ] = pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nI ].m_ITEM;
		} else  {
			this->m_STORE.m_SellITEM[ nI ] = *pInvITEM;
		}

		this->m_STORE.m_nInvIDX			[ nI ] = pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nI ].m_btInvIDX;
		this->m_STORE.m_dwSellPricePerEA[ nI ] = pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nI ].m_dwPricePerEA;
		this->m_STORE.m_btSellCNT ++;
	}

	bool bValid;
	nJ = pPacket->m_cli_P_STORE_OPEN.m_btSellItemCNT;
	for (nI=0; nI<pPacket->m_cli_P_STORE_OPEN.m_btWishItemCNT; nI++, nJ++) {
		if ( pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nJ ].m_btWishIDX >= MAX_P_STORE_ITEM_SLOT ) {
			return IS_HACKING( this, "Recv_cli_P_STORE_OPEN :: IDX >= MAX_P_STORE_ITEM_SLOT" );
		}

		this->m_STORE.m_WishITEM[ nI ].Clear();

		// 뭐냐... 해킹 ??
		if ( !tagITEM::IsValidITEM( &pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nI ].m_ITEM ) ) 
			continue;

		if ( !pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nJ ].m_ITEM.IsEnableDROP() ) {
			// 아이템 거래 유형이 2,3번 이면 개인 상점 거래 불가..
			continue;
		}

		bValid = true;
		// 같은 물건을 서로 틀린 가격에 올려서...앞쪽의 싼가격으로 구매되는 버그...
		for ( nK=0; nK<nI; nK++ ) {
			if ( this->m_STORE.m_WishITEM[ nK ].GetHEADER() == pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nJ ].m_ITEM.GetHEADER() && 
				 this->m_STORE.m_dwWishPricePerEA[ nK ]     != pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nJ ].m_dwPricePerEA ) {
				bValid = false;
				break;
			}
		}
		if ( bValid && pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nJ ].m_ITEM.IsValidITEM() ) {
			this->m_STORE.m_btWishIDX		[ nI ] = pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nJ ].m_btWishIDX;
			this->m_STORE.m_WishITEM		[ nI ] = pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nJ ].m_ITEM;
			this->m_STORE.m_dwWishPricePerEA[ nI ] = pPacket->m_cli_P_STORE_OPEN.m_ITEMs[ nJ ].m_dwPricePerEA;
			this->m_STORE.m_btWishIdx2StoreIDX[ m_STORE.m_btWishIDX[ nI ] ] = (BYTE)nI;
			if ( this->m_STORE.m_WishITEM[ nI ].IsEnableDupCNT() && this->m_STORE.m_WishITEM[ nI ].GetQuantity() > MAX_DUP_ITEM_QUANTITY ) {
				this->m_STORE.m_WishITEM[ nI ].m_uiQuantity = MAX_DUP_ITEM_QUANTITY;
			}
		}
	}
	this->m_STORE.m_btWishCNT = pPacket->m_cli_P_STORE_OPEN.m_btWishItemCNT;

	nOffset = sizeof( cli_P_STORE_OPEN ) + sizeof( tagPS_ITEM ) * ( pPacket->m_cli_P_STORE_OPEN.m_btSellItemCNT+pPacket->m_cli_P_STORE_OPEN.m_btWishItemCNT );

	char *szTitle = Packet_GetStringPtr( pPacket, nOffset );

	if(strlen(szTitle)<1)
		return false;

	if ( strlen( szTitle ) >= MAX_USER_TITLE ) {
		strncpy( this->m_szUserTITLE, szTitle, MAX_USER_TITLE );
		this->m_szUserTITLE[ MAX_USER_TITLE ] = 0;
	} else {
		strcpy( this->m_szUserTITLE, szTitle );
	}

	return this->Send_gsv_P_STORE_OPENED ();
}


//-------------------------------------------------------------------------------------------------
/// 개인 상점 종료...
bool classUSER::Recv_cli_P_STORE_CLOSE( t_PACKET *pPacket )
{
	if ( !this->m_STORE.m_bActive )
		return true;

    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_P_STORE_CLOSED;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_P_STORE_CLOSED );

	pCPacket->m_gsv_P_STORE_CLOSED.m_wObjectIDX = this->Get_INDEX();

    this->GetZONE()->SendPacketToSectors( this, pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	this->m_STORE.m_bActive = false;
	this->m_IngSTATUS.ClearSubFLAG( FLAG_SUB_STORE_MODE );

	// 주변에 통보 없이 정지 명령 상태로...
	this->CObjAI::SetCMD_STOP ();

	return true;
}


//-------------------------------------------------------------------------------------------------
/// 개인 상점 물품 목록을 요청
bool classUSER::Recv_cli_P_STORE_LIST_REQ( t_PACKET *pPacket )
{
	classUSER *pStoreOWNER = g_pObjMGR->Get_UserOBJ( pPacket->m_cli_P_STORE_LIST_REQ.m_wStoreObjectIDX );
	if ( pStoreOWNER ) {
		if ( !pStoreOWNER->m_STORE.m_bActive )
			return true;

		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( !pCPacket )
			return false;

		BYTE btI, btItemCNT=0;
		for (btI=0; btI<pStoreOWNER->m_STORE.m_btSellCNT; btI++) {
			if ( !pStoreOWNER->m_STORE.m_SellITEM[ btI ].GetHEADER() )
				continue;
			
			pCPacket->m_gsv_P_STORE_LIST_REPLY.m_SlotITEMs[ btItemCNT ].m_btSLOT   = btI;
			pCPacket->m_gsv_P_STORE_LIST_REPLY.m_SlotITEMs[ btItemCNT ].m_SlotITEM = pStoreOWNER->m_STORE.m_SellITEM[ btI ];
			pCPacket->m_gsv_P_STORE_LIST_REPLY.m_SlotITEMs[ btItemCNT ].m_dwPRICE  = pStoreOWNER->m_STORE.m_dwSellPricePerEA[ btI ];

			btItemCNT ++;
		}
		pCPacket->m_gsv_P_STORE_LIST_REPLY.m_btSellItemCNT = btItemCNT;

		pCPacket->m_gsv_P_STORE_LIST_REPLY.m_btWishItemCNT = 0;
		for (btI=0; btI<pStoreOWNER->m_STORE.m_btWishCNT; btI++) {
			if ( !pStoreOWNER->m_STORE.m_WishITEM[ btI ].GetHEADER() ) 
				continue;

			if ( pStoreOWNER->GetCur_MONEY() >= pStoreOWNER->m_STORE.m_dwWishPricePerEA[ btI ] ) {
				pCPacket->m_gsv_P_STORE_LIST_REPLY.m_SlotITEMs[ btItemCNT ].m_btSLOT   = pStoreOWNER->m_STORE.m_btWishIDX[ btI ];
				pCPacket->m_gsv_P_STORE_LIST_REPLY.m_SlotITEMs[ btItemCNT ].m_SlotITEM = pStoreOWNER->m_STORE.m_WishITEM[ btI ];
				pCPacket->m_gsv_P_STORE_LIST_REPLY.m_SlotITEMs[ btItemCNT ].m_dwPRICE  = pStoreOWNER->m_STORE.m_dwWishPricePerEA[ btI ];

				btItemCNT ++;
				pCPacket->m_gsv_P_STORE_LIST_REPLY.m_btWishItemCNT ++;
			}
		}

		pCPacket->m_HEADER.m_wType = GSV_P_STORE_LIST_REPLY;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_P_STORE_LIST_REPLY ) + sizeof(tagPS_SLOT_PRICE) * btItemCNT;

		this->SendPacket( pCPacket );

		Packet_ReleaseNUnlock( pCPacket );
	} else 
		return this->Send_gsv_P_STORE_RESULT( pPacket->m_cli_P_STORE_LIST_REQ.m_wStoreObjectIDX, RESULT_P_STORE_CANCLED );

	return true;
}


//-------------------------------------------------------------------------------------------------
/// 개인 상점거래 패킷 초기화...
classPACKET *classUSER::Init_gsv_P_STORE_RESULT( WORD wObjectIDX )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( pCPacket ) {
		pCPacket->m_HEADER.m_wType = GSV_P_STORE_RESULT;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_P_STORE_RESULT );

		pCPacket->m_gsv_P_STORE_RESULT.m_wStoreObjectIDX = wObjectIDX;
		pCPacket->m_gsv_P_STORE_RESULT.m_btItemCNT = 0;
	}

	return pCPacket;
}
/*
bool classUSER::Send_gsv_P_STORE_RESULT( classPACKET *pCPacket, BYTE btResult )
{
	if ( pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ) {
		// 변경된 인벤토리가 있을경우만 전송한다.
		pCPacket->m_HEADER.m_nSize += ( pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT * sizeof( tagPS_SLOT_ITEM ) );
	}

	pCPacket->m_gsv_P_STORE_RESULT.m_btResult = btResult;

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}
*/
/// 개인 상점 거래 결과 패킷 통보
bool classUSER::Send_gsv_P_STORE_RESULT( WORD wObjectIDX, BYTE btResult )
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_P_STORE_RESULT;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_P_STORE_RESULT );

	pCPacket->m_gsv_P_STORE_RESULT.m_wStoreObjectIDX = wObjectIDX;
	pCPacket->m_gsv_P_STORE_RESULT.m_btResult = btResult;

	this->SendPacket( pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	return true;
}


//-------------------------------------------------------------------------------------------------
/// 개인 상점에서 물품 구입 희망...
bool classUSER::Recv_cli_P_STORE_BUY_REQ( t_PACKET *pPacket )
{
	if ( pPacket->m_cli_P_STORE_BUY_REQ.m_btItemCNT >= MAX_P_STORE_ITEM_SLOT ) {
		return IS_HACKING( this, "Recv_cli_P_STORE_BUY_REQ :: m_btItemCNT >= MAX_P_STORE_ITEM_SLOT" );
	}
	if ( this->m_iIndex == pPacket->m_cli_P_STORE_BUY_REQ.m_wStoreObjectIDX ) {
		return IS_HACKING( this, "Recv_cli_P_STORE_BUY_REQ :: Trade from self private store" );
	}

	// 1:1거래중 상점 거래중 금지..
	if ( this->m_btTradeBIT & (BIT_TRADE_READY|BIT_TRADE_DONE) )	return true;

	classUSER *pStoreOWNER = g_pObjMGR->Get_UserOBJ( pPacket->m_cli_P_STORE_BUY_REQ.m_wStoreObjectIDX );
	if ( pStoreOWNER ) {
		if ( !pStoreOWNER->m_STORE.m_bActive )
			return true;

		classPACKET *pStorePacket  = Init_gsv_P_STORE_RESULT( pPacket->m_cli_P_STORE_BUY_REQ.m_wStoreObjectIDX );
		classPACKET *pBuyerPacket  = Init_gsv_SET_MONEYnINV ( GSV_P_STORE_MONEYnINV );
		classPACKET *pSellerPacket = Init_gsv_SET_MONEYnINV ( GSV_P_STORE_MONEYnINV );

		BYTE btStoreSLOT, btTradeRESULT;
		tagITEM *pInvSellITEM, *pSlotITEM, sBuyITEM, sSubITEM;
		__int64 biNeedMoney;	// 0xffffffff * 갯수 -> DWORD값이 범위 초과해서 적은 금액으로 선정되던 버그 수정..
		short nBuyerInvIDX;

		btTradeRESULT = RESULT_P_STORE_BOUGHT_ALL;
		for (short nI=0; nI<pPacket->m_cli_P_STORE_BUY_REQ.m_btItemCNT; nI++) {
			// 개인 상점에서 물건을 구매...
			btStoreSLOT	= pPacket->m_cli_P_STORE_BUY_REQ.m_BuyITEMs[ nI ].m_btSLOT;
			if ( btStoreSLOT >= MAX_P_STORE_ITEM_SLOT ) {
				return IS_HACKING( this, "Recv_cli_P_STORE_BUY_REQ :: btStoreSLOT >= MAX_P_STORE_ITEM_SLOT" );
			}

			pSlotITEM	= &pStoreOWNER->m_STORE.m_SellITEM[ btStoreSLOT ];
			if ( !pSlotITEM->GetTYPE() ) {
				// 다 팔렸다.
				btTradeRESULT = RESULT_P_STORE_BOUGHT_PART;
				continue;
			}

			pInvSellITEM = &pStoreOWNER->m_Inventory.m_ItemLIST[ pStoreOWNER->m_STORE.m_nInvIDX[ btStoreSLOT ] ];
			sBuyITEM = pPacket->m_cli_P_STORE_BUY_REQ.m_BuyITEMs[ nI ].m_SlotITEM;
			if ( pInvSellITEM->GetHEADER() != sBuyITEM.GetHEADER() ) {
				// 뭐냐 이건 ?? 
				continue;
			}

			if ( pSlotITEM->IsEnableDupCNT() ) {
				// 중복 갯수 아이템일경우...
				if ( pInvSellITEM->GetQuantity() < pSlotITEM->GetQuantity() ) {
					// 소지한거 보다 판매하려는것이 많다면...
					pSlotITEM->m_uiQuantity = pInvSellITEM->GetQuantity();
				}
				if ( pSlotITEM->GetQuantity() < sBuyITEM.GetQuantity() ) {
					// 갯수가 부족하다, 갯수 수정..
					sBuyITEM.m_uiQuantity = pSlotITEM->GetQuantity();
					btTradeRESULT = RESULT_P_STORE_BOUGHT_PART;
				}

				biNeedMoney = ( (__int64)( pStoreOWNER->m_STORE.m_dwSellPricePerEA[ btStoreSLOT ] ) * sBuyITEM.GetQuantity() );
			} else {
				sBuyITEM.m_dwBody = pInvSellITEM->m_dwBody;		// 메모리 조작으로 옵션이 바뀌는걸 방지
				// 장비일 경우 아이템 시리얼...
				sBuyITEM.m_iSN = pInvSellITEM->m_iSN;
				biNeedMoney = (__int64)( pStoreOWNER->m_STORE.m_dwSellPricePerEA[ btStoreSLOT ] );
			}
			if ( this->GetCur_MONEY() < biNeedMoney ) {
				// 구매 요청전에 돈 계산해서 보내야지..
				// btTradeRESULT |= RESULT_P_STORE_NEED_MONEY;
				continue;
			}
			
			// 아이템 이동...
			sSubITEM = sBuyITEM;
			nBuyerInvIDX = this->Add_ITEM( sBuyITEM );		// pBuyITEM = 호출후 더해진 결과의 인벤토리 아이템
			if ( nBuyerInvIDX > 0 ) {
				pInvSellITEM->SubtractOnly( sSubITEM );
				pSlotITEM->SubtractOnly( sSubITEM );

				//this->m_Inventory.m_i64Money -= biNeedMoney;
				this->Sub_CurMONEY(biNeedMoney);
				

				//pStoreOWNER->m_Inventory.m_i64Money += biNeedMoney;
				pStoreOWNER->Add_CurMONEY(biNeedMoney);
				

				// 구입자 인벤 갱신
				pBuyerPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pBuyerPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_btInvIDX = (BYTE)nBuyerInvIDX;
				pBuyerPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pBuyerPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_ITEM = sBuyITEM;
				pBuyerPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ++;

				// 판매자 인벤 갱신
				pSellerPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pSellerPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_btInvIDX = (BYTE)pStoreOWNER->m_STORE.m_nInvIDX[ btStoreSLOT ];
				pSellerPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pSellerPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_ITEM = *pInvSellITEM;
				pSellerPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ++;

				// 상점 물품의 갱신된 내역...
				pStorePacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[ pStorePacket->m_gsv_P_STORE_RESULT.m_btItemCNT ].m_btSLOT   = btStoreSLOT;
				pStorePacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[ pStorePacket->m_gsv_P_STORE_RESULT.m_btItemCNT ].m_SlotITEM = *pSlotITEM;
				pStorePacket->m_gsv_P_STORE_RESULT.m_btItemCNT ++;

				// 개인 상점 거래 로그...
				
				g_pThreadLOG->When_TagItemLOG( LIA_P2PTRADE, pStoreOWNER, &sSubITEM, 0, biNeedMoney, this);
	
			} /* else {
				인벤토리 모자람... 이 아이템 거래 취소...
			} */
		}

		this->Send_gsv_SET_MONEYnINV ( pBuyerPacket );		// 구매자는 인벤토리,돈 패킷으로 변경 정보 전송.
		pStoreOWNER->Send_gsv_SET_MONEYnINV ( pSellerPacket );

		// 상점 아이템 슬롯 갱신
		pStorePacket->m_gsv_P_STORE_RESULT.m_btResult = btTradeRESULT;
		if ( pStorePacket->m_gsv_P_STORE_RESULT.m_btItemCNT ) {
			// 개래된 품목이 있다면...
			pStorePacket->m_HEADER.m_nSize += ( pStorePacket->m_gsv_P_STORE_RESULT.m_btItemCNT * sizeof( tagPS_SLOT_ITEM ) );
			pStoreOWNER->SendPacket( pStorePacket );
		}
		this->SendPacket( pStorePacket );

		Packet_ReleaseNUnlock( pStorePacket );
	} else {
		return this->Send_gsv_P_STORE_RESULT( pPacket->m_cli_P_STORE_BUY_REQ.m_wStoreObjectIDX, RESULT_P_STORE_CANCLED );
	}

    return true;
}


//-------------------------------------------------------------------------------------------------
/// 개인 상점에 물품 판매 희망...
bool classUSER::Recv_cli_P_STORE_SELL_REQ( t_PACKET *pPacket )
{
	if ( pPacket->m_cli_P_STORE_SELL_REQ.m_btItemCNT >= MAX_P_STORE_ITEM_SLOT ) {
		return IS_HACKING( this, "Recv_cli_P_STORE_SELL_REQ :: m_btItemCNT >= MAX_P_STORE_ITEM_SLOT" );
	}
	if ( this->m_iIndex == pPacket->m_cli_P_STORE_SELL_REQ.m_wStoreObjectIDX ) {
		return IS_HACKING( this, "Recv_cli_P_STORE_SELL_REQ :: Trade from self private store" );
	}

	// 1:1거래중 상점 거래중 금지..
	if ( this->m_btTradeBIT & (BIT_TRADE_READY|BIT_TRADE_DONE) )	return true;

	classUSER *pStoreOWNER = g_pObjMGR->Get_UserOBJ( pPacket->m_cli_P_STORE_SELL_REQ.m_wStoreObjectIDX );

	if ( pStoreOWNER )
	{
		if ( !pStoreOWNER->m_STORE.m_bActive )
			return true;

		classPACKET *pStorePacket  = Init_gsv_P_STORE_RESULT( pPacket->m_cli_P_STORE_SELL_REQ.m_wStoreObjectIDX );
		classPACKET *pBuyerPacket  = Init_gsv_SET_MONEYnINV( GSV_P_STORE_MONEYnINV );
		classPACKET *pSellerPacket = Init_gsv_SET_MONEYnINV( GSV_P_STORE_MONEYnINV );

		BYTE btStoreSLOT, btTradeRESULT;
		tagITEM *pSellerITEM, *pWishITEM, sToSellITEM, sSubITEM;
		DWORD dwNeedMoney;
		short nInvIDX;

		btTradeRESULT = RESULT_P_STORE_SOLD_ALL;
		for (short nI=0; nI<pPacket->m_cli_P_STORE_SELL_REQ.m_btItemCNT; nI++) 
		{
			if ( pPacket->m_cli_P_STORE_SELL_REQ.m_SellITEMs[ nI ].m_btWishSLOT >= MAX_P_STORE_ITEM_SLOT ) 
			{
				return IS_HACKING( this, "Recv_cli_P_STORE_SELL_REQ :: m_btWishSLOT >= MAX_P_STORE_ITEM_SLOT" );
			}
			// 개인 상점에 물건을 팔자 ...
			btStoreSLOT = pStoreOWNER->m_STORE.m_btWishIdx2StoreIDX[ pPacket->m_cli_P_STORE_SELL_REQ.m_SellITEMs[ nI ].m_btWishSLOT ];

			pWishITEM   = &pStoreOWNER->m_STORE.m_WishITEM[ btStoreSLOT ];
			pSellerITEM = &this->m_Inventory.m_ItemLIST[ pPacket->m_cli_P_STORE_SELL_REQ.m_SellITEMs[ nI ].m_btInvIDX ];
			if ( pWishITEM->GetHEADER() != pSellerITEM->GetHEADER() )
				continue;

			sToSellITEM = pPacket->m_cli_P_STORE_SELL_REQ.m_SellITEMs[ nI ].m_SellITEM;
			if ( pWishITEM->GetHEADER() != sToSellITEM.GetHEADER() )
				continue;

			if ( pWishITEM->IsEnableDupCNT() ) {
				// 중복 갯수 아이템일경우...
				if ( sToSellITEM.GetQuantity() > pWishITEM->GetQuantity() )
				{
					// 팔려는것이 구매희망량 보다 많다 - 판매갯수 보정.
					sToSellITEM.m_uiQuantity = pWishITEM->GetQuantity();
					btTradeRESULT = RESULT_P_STORE_SOLD_PART;
				}
				if ( sToSellITEM.GetQuantity() > pSellerITEM->GetQuantity() )
				{
					// 팔려는것이 소지량 보다 많다 :: 메크로에 의한 숫자 조작으로 헤킹 발생...
					continue;
				}
				dwNeedMoney = pStoreOWNER->m_STORE.m_dwWishPricePerEA[ btStoreSLOT ] * sToSellITEM.GetQuantity();
			} else {
				// 장비 아이템일 경우 옵션까지 같은지 체크...
				if ( 0 == pSellerITEM->GetLife() ) {
					// 수명이 0인건 못쓰는 아이템야...
					continue;
				}
				if ( pWishITEM->HasSocket() && pWishITEM->HasSocket() != pSellerITEM->HasSocket() ) {
					// 구매자가 소켓 있는 아이템을 원하는데... 소켓이 일치 하지 않네
					continue;
				}
				if ( pWishITEM->GetDurability() > pSellerITEM->GetDurability() ) {
					// 구매자가 원하는 아이템의 내구도 보다 낮음 못팔아요~~
					continue;
				}
				if ( pWishITEM->GetGrade() > pSellerITEM->GetGrade() ) {
					// 구매자가 원하는 아이템의 등급보다 낮음 못팔아요~~
					continue;
				}
				if ( pWishITEM->GetOption() && pWishITEM->GetOption() != pSellerITEM->GetOption() ) {
					// 구매자가 원하는 아이템에 옵션있고 옵션이 틀리면 못팔아요~
					continue;
				}
				sToSellITEM.m_dwBody = pSellerITEM->m_dwBody;		// 메모리 조작으로 옵션이 바뀌는걸 방지
				// 장비 아이템일 경우 시리얼 번호...
				sToSellITEM.m_iSN = pSellerITEM->m_iSN;
				dwNeedMoney = pStoreOWNER->m_STORE.m_dwWishPricePerEA[ btStoreSLOT ];
			}

			if ( pStoreOWNER->GetCur_MONEY() < dwNeedMoney ) {
				// 구매 상점을 열기 전에 반드시 돈이 있는지 체크...
				#pragma COMPILE_TIME_MSG( "구입 하려는 넘이 돈이 모자라다 :: 상점 접어 ??.. 품목 삭제?? " )
				// btTradeRESULT |= RESULT_P_STORE_NEED_MONEY;
				continue;
			}

			sSubITEM = sToSellITEM;
			nInvIDX  = pStoreOWNER->Add_ITEM( sToSellITEM );
			if ( nInvIDX > 0 )
			{
				pStoreOWNER->m_Inventory.m_i64Money -= dwNeedMoney;
				this->m_Inventory.m_i64Money += dwNeedMoney;
				pSellerITEM->SubtractOnly( sSubITEM );		// 팔린 양만큼 제거
				pWishITEM->SubtractOnly( sSubITEM );		// 구매한 양만큼 제거

				pSellerPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pSellerPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_btInvIDX = pPacket->m_cli_P_STORE_SELL_REQ.m_SellITEMs[ nI ].m_btInvIDX;
				pSellerPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pSellerPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_ITEM = *pSellerITEM;
				pSellerPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ++;

				pBuyerPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pBuyerPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_btInvIDX = (BYTE)nInvIDX;
				pBuyerPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ pBuyerPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ].m_ITEM = pStoreOWNER->m_Inventory.m_ItemLIST[ nInvIDX ];
				pBuyerPacket->m_gsv_SET_MONEYnINV.m_btItemCNT ++;

				// 상점 물품의 갱신된 내역...
				pStorePacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[ pStorePacket->m_gsv_P_STORE_RESULT.m_btItemCNT ].m_btSLOT = pPacket->m_cli_P_STORE_SELL_REQ.m_SellITEMs[ nI ].m_btWishSLOT;
				pStorePacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[ pStorePacket->m_gsv_P_STORE_RESULT.m_btItemCNT ].m_SlotITEM = *pWishITEM;
				pStorePacket->m_gsv_P_STORE_RESULT.m_btItemCNT ++;

				// 개인 상점 거래 로그...
				
				g_pThreadLOG->When_TagItemLOG( LIA_P2PTRADE, this, &sSubITEM, 0, dwNeedMoney, pStoreOWNER);
				
			} /* else {
				구매자 인벤 꽉참.. 거래에서 이 아이템 제외~~~
			} */
		}

		this->Send_gsv_SET_MONEYnINV( pSellerPacket );			// 판매자 인벤토리,돈 패킷으로 변경 정보 전송.
		pStoreOWNER->Send_gsv_SET_MONEYnINV( pBuyerPacket );	// 구매자(상점주인) 추가된 아이템 인벤토리 정보, 구매자 감???된 돈 정보 전송

		// ??점 아이템 슬롯 갱신
		pStorePacket->m_gsv_P_STORE_RESULT.m_btResult = btTradeRESULT;
		if ( pStorePacket->m_gsv_P_STORE_RESULT.m_btItemCNT ) 
		{
			// 개래된 품목이 있다면...
			pStorePacket->m_HEADER.m_nSize += ( pStorePacket->m_gsv_P_STORE_RESULT.m_btItemCNT * sizeof( tagPS_SLOT_ITEM ) );
			pStoreOWNER->SendPacket( pStorePacket );
		}
		this->SendPacket( pStorePacket );

		Packet_ReleaseNUnlock( pStorePacket );
	} else {
		return this->Send_gsv_P_STORE_RESULT( pPacket->m_cli_P_STORE_BUY_REQ.m_wStoreObjectIDX, RESULT_P_STORE_CANCLED );
	}

    return true;
}


//-------------------------------------------------------------------------------------------------
/// 스킬의 렙업 요청
bool classUSER::Recv_cli_SKILL_LEVELUP_REQ ( t_PACKET *pPacket )
{
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() ) return true;
	//	김영환 수정
#ifdef	__MILEAGE_SKILL_USED
	if ( pPacket->m_cli_SKILL_LEVELUP_REQ.m_wSkillSLOT >= MAX_LEARNED_SKILL_CNT )
		return false;
#else
	if ( pPacket->m_cli_SKILL_LEVELUP_REQ.m_btSkillSLOT >= MAX_LEARNED_SKILL_CNT )
		return false;
#endif

    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_SKILL_LEVELUP_REPLY;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_SKILL_LEVELUP_REPLY );

	//	김영환 수정
#ifdef	__MILEAGE_SKILL_USED
	short nSkillIDX = this->m_Skills.m_nSkillINDEX[ pPacket->m_cli_SKILL_LEVELUP_REQ.m_wSkillSLOT ];
#else
	short nSkillIDX = this->m_Skills.m_nSkillINDEX[ pPacket->m_cli_SKILL_LEVELUP_REQ.m_btSkillSLOT ];
#endif

	BYTE btResult = this->Skill_LevelUpCondition( nSkillIDX, pPacket->m_cli_SKILL_LEVELUP_REQ.m_nNextLevelSkillIDX );
	if ( RESULT_SKILL_LEVELUP_SUCCESS == btResult ) {
		// 05.05.25 스킬 렙업시 줄리 소모...
		int iNeedZuly = IsTAIWAN() ? SKILL_LEVELUP_NEED_ZULY( pPacket->m_cli_SKILL_LEVELUP_REQ.m_nNextLevelSkillIDX ) * 100 : 0;

		if ( this->GetCur_MONEY() < iNeedZuly ) {
			btResult = RESULT_SKILL_LEVELUP_OUTOFZULY;
		} else {
			this->Sub_CurMONEY( iNeedZuly );	// 소모 비용 적용

			g_pThreadLOG->When_LearnSKILL( this, pPacket->m_cli_SKILL_LEVELUP_REQ.m_nNextLevelSkillIDX );
			
//	김영환 수정
#ifdef	__MILEAGE_SKILL_USED
			switch( this->Skill_LEARN( pPacket->m_cli_SKILL_LEVELUP_REQ.m_wSkillSLOT, pPacket->m_cli_SKILL_LEVELUP_REQ.m_nNextLevelSkillIDX ) ) 
			{
				case 0x03 :
					this->Cal_BattleAbility ();
				case 0x01 :
					this->Update_SPEED ();	// 중복 호츨된이다.. 나중에 검토 필요..
					this->Send_gsv_SPEED_CHANGED ();
			}
			pCPacket->m_gsv_SKILL_LEVELUP_REPLY.m_wSkillSLOT = pPacket->m_cli_SKILL_LEVELUP_REQ.m_wSkillSLOT;
			pCPacket->m_gsv_SKILL_LEVELUP_REPLY.m_nSkillINDEX = this->m_Skills.m_nSkillINDEX[ pPacket->m_cli_SKILL_LEVELUP_REQ.m_wSkillSLOT ];
#else
			switch( this->Skill_LEARN( pPacket->m_cli_SKILL_LEVELUP_REQ.m_btSkillSLOT, pPacket->m_cli_SKILL_LEVELUP_REQ.m_nNextLevelSkillIDX ) )
			{
				case 0x03 :
					this->Cal_BattleAbility ();
				case 0x01 :
					this->Update_SPEED ();
					this->Send_gsv_SPEED_CHANGED ();
			}
			pCPacket->m_gsv_SKILL_LEVELUP_REPLY.m_btSkillSLOT = pPacket->m_cli_SKILL_LEVELUP_REQ.m_btSkillSLOT;
			pCPacket->m_gsv_SKILL_LEVELUP_REPLY.m_nSkillINDEX = this->m_Skills.m_nSkillINDEX[ pPacket->m_cli_SKILL_LEVELUP_REQ.m_btSkillSLOT ];
#endif

		}
	}
	pCPacket->m_gsv_SKILL_LEVELUP_REPLY.m_btResult = btResult;
	pCPacket->m_gsv_SKILL_LEVELUP_REPLY.m_nSkillPOINT = this->GetCur_SkillPOINT ();

	this->SendPacket( pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 셀프 스킬인지 체크...
bool classUSER::Is_SelfSKILL( short nSkillIDX )
{
	switch ( SKILL_TYPE( nSkillIDX ) ) {
	//	case SKILL_TYPE_01 :	// 점프, 공중부양.
		case SKILL_TYPE_08 :	// 능력치의 증/감을 일정시간 지속(셀프)
		case SKILL_TYPE_10 :	// 능력치 변경형(셀프)	:	한방에 올림
		case SKILL_TYPE_12 :	// 상태 지속형(셀프)	중독, 벙어리, 기절, 투명, 방패데미지, 추가 데미지
		case SKILL_TYPE_14 :	// 소환...
		case SKILL_TYPE_17 :	// self & damage to scope...
			return true;
	} 
	return false;
}
/// 타겟 스킬인지 체크...
bool classUSER::Is_TargetSKILL( short nSkillIDX )
{
	switch ( SKILL_TYPE( nSkillIDX ) ) {
		case SKILL_TYPE_03 :	// 공격 동작 변경형
		case SKILL_TYPE_04 :	// 무기 상태 변경형
		case SKILL_TYPE_05 :	// 총알 변경헝 공격형

		case SKILL_TYPE_06 :	// 발사 마법형
		case SKILL_TYPE_09 :	// 능력치의 증/감을 일정시간 지속(타겟)
		case SKILL_TYPE_11 :	// 능력치 변경형(타겟)	:	한방에 올림
		case SKILL_TYPE_13 :	// 상태 지속형(타겟)	중독, 벙어리, 기절, 투명, 방패데미지, 추가 데미지
		case SKILL_TYPE_19 :	// (타겟에 데미지공격을 하면서 HP,MP 뺏어오기 개념)
		case SKILL_TYPE_20 :	// 시체 부활
			break;

/*
		case SKILL_TYPE_08 :	// 능력치의 증/감을 일정시간 지속(셀프)
		case SKILL_TYPE_10 :	// 능력치 변경형(셀프)	:	한방에 올림
		case SKILL_TYPE_12 :	// 상태 지속형(셀프)	중독, 벙어리, 기절, 투명, 방패데미지, 추가 데미지
			return false;
		default :
			if ( SA_TARGET_STOP   == SKILL_ACTION_MODE( nSkillIDX ) ||
				 SA_TARGET_ATTACK == SKILL_ACTION_MODE( nSkillIDX ) )
				break;
*/
		default :
			return false;
	} 
	return true;
}

//-------------------------------------------------------------------------------------------------

#define	SKILL_DELAY_TIME	500	// 0.5 sec
/// 개인 스킬 시작 요청 받음
bool classUSER::Recv_cli_SELF_SKILL( t_PACKET *pPacket )
{
	if (!(this->m_dwPayFLAG & PLAY_FLAG_BATTLE)) 	// 공격 못해... 돈내야함
	{
		return true;
	}

	char szTmp[ MAX_PATH ];

	DWORD dwCurTime = this->GetZONE()->GetCurrentTIME();
	if ( SKILL_DELAY_TIME > dwCurTime - this->m_dwLastSkillActiveTIME ) 
	{
#ifdef	__INC_WORLD
		sprintf( szTmp, "ignore Skill:: Delay:%d, Charged:%d", SKILL_DELAY_TIME, dwCurTime - this->m_dwLastSkillActiveTIME );
		this->Send_gsv_WHISPER( "SELF_SKILL", szTmp );
#endif
		return true;
	}

	if ( this->Get_ActiveSKILL() ) {
#ifdef	__INC_WORLD
		sprintf( szTmp, "ignore Skill in casting motion" );
		this->Send_gsv_WHISPER( "SELF_SKILL", szTmp );
#endif
		return true;	// 스킬 케스팅 중일땐 못바꿔
	}
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() || this->m_IngSTATUS.IsSET( FLAG_ING_DUMB ) ) 
	{
#ifdef	__INC_WORLD
		sprintf( szTmp, "ignore Skill:: not allowd status" );
		this->Send_gsv_WHISPER( "SELF_SKILL", szTmp );
#endif
		return true;
	}

//	김영환 수정
#ifdef	__MILEAGE_SKILL_USED
	if ( pPacket->m_cli_SELF_SKILL.m_wSkillSLOT >= MAX_LEARNED_SKILL_CNT ) 
	{
#ifdef	__INC_WORLD
		sprintf( szTmp, "ignore Skill:: invalid skill slot %d / %d", pPacket->m_cli_SELF_SKILL.m_wSkillSLOT, MAX_LEARNED_SKILL_CNT);
		this->Send_gsv_WHISPER( "SELF_SKILL", szTmp );
#endif
		return false;
	}
	DWORD dwCurSec = classTIME::GetCurrentAbsSecond();
	short nSkillIDX = this->m_Skills.m_nSkillINDEX[ pPacket->m_cli_SELF_SKILL.m_wSkillSLOT ];
#else
	if ( pPacket->m_cli_SELF_SKILL.m_btSkillSLOT >= MAX_LEARNED_SKILL_CNT ) 
	{
#ifdef	__INC_WORLD
		sprintf( szTmp, "ignore Skill:: invalid skill slot %d / %d", pPacket->m_cli_SELF_SKILL.m_btSkillSLOT, MAX_LEARNED_SKILL_CNT);
		this->Send_gsv_WHISPER( "SELF_SKILL", szTmp );
#endif
		return false;
	}
	DWORD dwCurSec = classTIME::GetCurrentAbsSecond();
	short nSkillIDX = this->m_Skills.m_nSkillINDEX[ pPacket->m_cli_SELF_SKILL.m_btSkillSLOT ];
#endif

	if ( IsTAIWAN() )
	{
		if ( SKILL_AVAILBLE_STATUS(nSkillIDX) && !(m_btRideATTR & SKILL_AVAILBLE_STATUS(nSkillIDX)) )
			return true;

		if ( SKILL_RELOAD_TYPE(nSkillIDX) ) 
		{
			// 05.05.30 스킬 리로드 그룹타입으로 체크...
			
			//////////////////////////////////////////////////////////////////////////
			// 2006.06.20:이성활:Group Skill Spell Time 적용시 마지막 사용 skill의 Spell Time으로 적용되어야
			//					되므로 마지막 사용 skill의 Reload Second를 사용
			//if ( SKILL_RELOAD_SECOND(nSkillIDX) > (dwCurSec - this->m_dwLastSkillGroupSpellTIME[ SKILL_RELOAD_TYPE(nSkillIDX) ])*1000 )
			if ( SKILL_RELOAD_SECOND(m_nLastSpellGroupSkillIndex[ SKILL_RELOAD_TYPE(nSkillIDX) ]) > (dwCurSec - this->m_dwLastSkillGroupSpellTIME[ SKILL_RELOAD_TYPE(m_nLastSpellGroupSkillIndex[ SKILL_RELOAD_TYPE(nSkillIDX) ]) ])*1000 )
			//////////////////////////////////////////////////////////////////////////
			{
	#ifdef	__INC_WORLD
				sprintf( szTmp, "ignore SkillIdx:%d, :: Delay:%d > Charged:%d, LastStamp:%d \n", 
						nSkillIDX, 
						SKILL_RELOAD_SECOND(nSkillIDX), 
						dwCurSec - this->m_dwLastSkillGroupSpellTIME[ SKILL_RELOAD_TYPE(nSkillIDX) ], 
						this->m_dwLastSkillGroupSpellTIME[ SKILL_RELOAD_TYPE(nSkillIDX) ] );
				this->Send_gsv_WHISPER( "SELF_SKILL", szTmp );
	#endif
				return true;
			}
		}
	}

//	김영환 수정
#ifdef	__MILEAGE_SKILL_USED
	if ( SKILL_RELOAD_SECOND(nSkillIDX) > dwCurTime - this->m_dwLastSkillSpellTIME[ pPacket->m_cli_SELF_SKILL.m_wSkillSLOT ] ) 
	{
		// 다시 사용가능한 시간인지 체크...
#ifdef	__INC_WORLD
		sprintf( szTmp, "ignore SkillIdx:%d, :: Delay:%d > Charged:%d, LastStamp:%d \n", 
				nSkillIDX, 
				SKILL_RELOAD_SECOND(nSkillIDX), 
				dwCurTime - this->m_dwLastSkillSpellTIME[ pPacket->m_cli_SELF_SKILL.m_wSkillSLOT ], 
				this->m_dwLastSkillSpellTIME[ pPacket->m_cli_SELF_SKILL.m_wSkillSLOT ] );
		this->Send_gsv_WHISPER( "SELF_SKILL", szTmp );
#endif
		return true;
	}
#else
	if ( SKILL_RELOAD_SECOND(nSkillIDX) > dwCurTime - this->m_dwLastSkillSpellTIME[ pPacket->m_cli_SELF_SKILL.m_btSkillSLOT ] ) 
	{
		// 다시 사용가능한 시간인지 체크...
#ifdef	__INC_WORLD
		sprintf( szTmp, "ignore SkillIdx:%d, :: Delay:%d > Charged:%d, LastStamp:%d \n", 
				nSkillIDX, 
				SKILL_RELOAD_SECOND(nSkillIDX), 
				dwCurTime - this->m_dwLastSkillSpellTIME[ pPacket->m_cli_SELF_SKILL.m_btSkillSLOT ], 
				this->m_dwLastSkillSpellTIME[ pPacket->m_cli_SELF_SKILL.m_btSkillSLOT ] );
		this->Send_gsv_WHISPER( "SELF_SKILL", szTmp );
#endif
		return true;
	}
#endif

	if ( this->Get_WeightRATE() >= WEIGHT_RATE_CANT_ATK ) 
	{
		// 무겁다.. 명령 불가...
		if ( ( SKILL_TYPE( nSkillIDX ) >= 3 && SKILL_TYPE( nSkillIDX ) <= 13 ) ||
			   SKILL_TYPE( nSkillIDX ) == 17 ) 
		{
#ifdef	__INC_WORLD
			sprintf( szTmp, "ignore Skill:: over weight ");
			this->Send_gsv_WHISPER( "SELF_SKILL", szTmp );
#endif
			return true;
		}
	}
#ifdef __KCHS_BATTLECART__
	if ( this->Get_RideMODE() ) 
	{
		if( !this->CanDoPatSkill( nSkillIDX ) )
		{
	#ifdef	__INC_WORLD
			sprintf( szTmp, "ignore Skill:: in riding mode");
			this->Send_gsv_WHISPER( "SELF_SKILL", szTmp );
	#endif
			return true;
		}
	}
#else
	if ( this->Get_RideMODE() ) 
	{
	#ifdef	__INC_WORLD
		sprintf( szTmp, "ignore Skill:: in riding mode");
		this->Send_gsv_WHISPER( "SELF_SKILL", szTmp );
	#endif
		return true;
	}
#endif

	if ( !this->Is_SelfSKILL( nSkillIDX ) ) 
	{
		return IS_HACKING( this, "Recv_cli_SELF_SKILL :: no self type skill" );
	} 

	if ( this->Do_SelfSKILL( nSkillIDX ) ) 
	{
		// 시작 성공...
		this->m_dwLastSkillActiveTIME = dwCurTime;
//	김영환 수정
#ifdef	__MILEAGE_SKILL_USED
		this->m_dwLastSkillSpellTIME[ pPacket->m_cli_SELF_SKILL.m_wSkillSLOT ] = dwCurTime;
#else
		this->m_dwLastSkillSpellTIME[ pPacket->m_cli_SELF_SKILL.m_btSkillSLOT ] = dwCurTime;
#endif
		this->m_dwLastSkillGroupSpellTIME[ SKILL_RELOAD_TYPE(nSkillIDX) ] = dwCurSec;
		//////////////////////////////////////////////////////////////////////////
		// 2006.06.20:이성활:Group Skill Spell Time 적용시 마지막 사용 skill의 Spell Time으로 적용되어야
		//					되므로 마지막 사용 skill을 저장
		m_nLastSpellGroupSkillIndex[ SKILL_RELOAD_TYPE(nSkillIDX) ] = nSkillIDX;
		//////////////////////////////////////////////////////////////////////////
	}

	return true;
}

/// 타겟 스킬 시작 요청 받음
bool classUSER::Recv_cli_TARGET_SKILL( t_PACKET *pPacket )
{
	if ( !(this->m_dwPayFLAG & PLAY_FLAG_BATTLE) ) {
		// 공격 못해... 돈내야함
		return true;
	}

#ifdef	__INC_WORLD
	char szTmp[ MAX_PATH ];
#endif

	DWORD dwCurTime = this->GetZONE()->GetCurrentTIME();
	if ( SKILL_DELAY_TIME > dwCurTime - this->m_dwLastSkillActiveTIME ) 
	{
#ifdef	__INC_WORLD
		sprintf( szTmp, "ignore Skill:: Delay:%d, Charged:%d", SKILL_DELAY_TIME, dwCurTime - this->m_dwLastSkillActiveTIME );
		this->Send_gsv_WHISPER( "SELF_TARGET", szTmp );
#endif
		return true;
	}

	if ( this->Get_ActiveSKILL() )
	{
#ifdef	__INC_WORLD
		sprintf( szTmp, "ignore Skill in casting motion" );
		this->Send_gsv_WHISPER( "TARGET_SKILL", szTmp );
#endif
		return true;	// 스킬 케스팅 중일땐 못바꿔
	}
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() || this->m_IngSTATUS.IsSET( FLAG_ING_DUMB ) ) 
	{
#ifdef	__INC_WORLD
		sprintf( szTmp, "ignore Skill:: not allowd status" );
		this->Send_gsv_WHISPER( "TARGET_SKILL", szTmp );
#endif
		return true;
	}

	//	김영환 수정
#ifdef	__MILEAGE_SKILL_USED
	if ( pPacket->m_cli_TARGET_SKILL.m_wSkillSLOT >= MAX_LEARNED_SKILL_CNT )
	{
#ifdef	__INC_WORLD
		sprintf( szTmp, "ignore Skill:: invalid skill slot %d / %d", pPacket->m_cli_SELF_SKILL.m_wSkillSLOT, MAX_LEARNED_SKILL_CNT);
		this->Send_gsv_WHISPER( "SELF_SKILL", szTmp );
#endif
		return false;
	}
#else
	if ( pPacket->m_cli_TARGET_SKILL.m_btSkillSLOT >= MAX_LEARNED_SKILL_CNT )
	{
#ifdef	__INC_WORLD
		sprintf( szTmp, "ignore Skill:: invalid skill slot %d / %d", pPacket->m_cli_SELF_SKILL.m_btSkillSLOT, MAX_LEARNED_SKILL_CNT);
		this->Send_gsv_WHISPER( "SELF_SKILL", szTmp );
#endif
		return false;
	}
#endif


	DWORD dwCurSec = classTIME::GetCurrentAbsSecond();

//	김영환 수정
#ifdef	__MILEAGE_SKILL_USED
	short nSkillIDX = this->m_Skills.m_nSkillINDEX[ pPacket->m_cli_TARGET_SKILL.m_wSkillSLOT ];
#else
	short nSkillIDX = this->m_Skills.m_nSkillINDEX[ pPacket->m_cli_TARGET_SKILL.m_btSkillSLOT ];
#endif
	
	if ( IsTAIWAN() ) 
	{
		int n = SKILL_AVAILBLE_STATUS(nSkillIDX);

		if ( SKILL_AVAILBLE_STATUS(nSkillIDX) && !(m_btRideATTR & SKILL_AVAILBLE_STATUS(nSkillIDX)) )
			return true;

		if ( SKILL_RELOAD_TYPE(nSkillIDX) ) 
		{
			// 05.05.30 스킬 리로드 그룹타입으로 체크...
			//////////////////////////////////////////////////////////////////////////
			// 2006.06.20:이성활:Group Skill Spell Time 적용시 마지막 사용 skill의 Spell Time으로 적용되어야
			//					되므로 마지막 사용 skill의 Reload Second를 사용
			//if ( SKILL_RELOAD_SECOND(nSkillIDX) > (dwCurSec - this->m_dwLastSkillGroupSpellTIME[ SKILL_RELOAD_TYPE(nSkillIDX) ])*1000 ) 
			if ( SKILL_RELOAD_SECOND(m_nLastSpellGroupSkillIndex[ SKILL_RELOAD_TYPE(nSkillIDX) ]) > (dwCurSec - this->m_dwLastSkillGroupSpellTIME[ SKILL_RELOAD_TYPE(m_nLastSpellGroupSkillIndex[ SKILL_RELOAD_TYPE(nSkillIDX) ]) ])*1000 )
			//////////////////////////////////////////////////////////////////////////
			{
	#ifdef	__INC_WORLD
				sprintf( szTmp, "ignore SkillIdx:%d, :: Delay:%d > Charged:%d, LastStamp:%d \n", 
						nSkillIDX, 
						SKILL_RELOAD_SECOND(nSkillIDX), 
						dwCurTime - this->m_dwLastSkillGroupSpellTIME[SKILL_RELOAD_TYPE(nSkillIDX) ], 
						this->m_dwLastSkillGroupSpellTIME[SKILL_RELOAD_TYPE(nSkillIDX) ] );
				this->Send_gsv_WHISPER( "SELF_SKILL", szTmp );
	#endif
				return true;
			}
		}
	}

//	김영환 수정
#ifdef	__MILEAGE_SKILL_USED
	if ( SKILL_RELOAD_SECOND(nSkillIDX) > dwCurTime - this->m_dwLastSkillSpellTIME[ pPacket->m_cli_TARGET_SKILL.m_wSkillSLOT ] ) 
	{
		// 다시 사용가능한 시간인지 체크...
#ifdef	__INC_WORLD
		sprintf( szTmp, "ignore SkillIdx:%d, :: Delay:%d > Charged:%d, LastStamp:%d \n", 
				nSkillIDX, 
				SKILL_RELOAD_SECOND(nSkillIDX), 
				dwCurTime - this->m_dwLastSkillSpellTIME[ pPacket->m_cli_TARGET_SKILL.m_wSkillSLOT ], 
				this->m_dwLastSkillSpellTIME[ pPacket->m_cli_TARGET_SKILL.m_wSkillSLOT ] );
		this->Send_gsv_WHISPER( "TARGET_SKILL", szTmp );
#endif
		return true;
	}
#else
	if ( SKILL_RELOAD_SECOND(nSkillIDX) > dwCurTime - this->m_dwLastSkillSpellTIME[ pPacket->m_cli_TARGET_SKILL.m_btSkillSLOT ] ) 
	{
		// 다시 사용가능한 시간인지 체크...
#ifdef	__INC_WORLD
		sprintf( szTmp, "ignore SkillIdx:%d, :: Delay:%d > Charged:%d, LastStamp:%d \n", 
				nSkillIDX, 
				SKILL_RELOAD_SECOND(nSkillIDX), 
				dwCurTime - this->m_dwLastSkillSpellTIME[ pPacket->m_cli_TARGET_SKILL.m_btSkillSLOT ], 
				this->m_dwLastSkillSpellTIME[ pPacket->m_cli_TARGET_SKILL.m_btSkillSLOT ] );
		this->Send_gsv_WHISPER( "TARGET_SKILL", szTmp );
#endif
		return true;
	}
#endif

	if ( this->Get_WeightRATE() >= WEIGHT_RATE_CANT_ATK ) 
	{
		// 무겁다.. 명령 불가...
		if ( ( SKILL_TYPE( nSkillIDX ) >= 3 && SKILL_TYPE( nSkillIDX ) <= 13 ) ||
			   SKILL_TYPE( nSkillIDX ) == 17 ) {
#ifdef	__INC_WORLD
			sprintf( szTmp, "ignore Skill:: over weight ");
			this->Send_gsv_WHISPER( "TARGET_SKILL", szTmp );
#endif
			return true;
		}
	}

#ifdef __KCHS_BATTLECART__
	if ( this->Get_RideMODE() ) {
		if( !this->CanDoPatSkill( nSkillIDX ) )
		{
	#ifdef	__INC_WORLD
			sprintf( szTmp, "ignore Skill:: in riding mode");
			this->Send_gsv_WHISPER( "SELF_SKILL", szTmp );
	#endif
			return true;
		}
	}
#else
	if ( this->Get_RideMODE() ) {
	#ifdef	__INC_WORLD
		sprintf( szTmp, "ignore Skill:: in riding mode");
		this->Send_gsv_WHISPER( "TARGET_SKILL", szTmp );
	#endif
		return true;
	}
#endif

	if ( !this->Is_TargetSKILL( nSkillIDX ) ) {
		// 짜르지 말고 무시 :: 몇건씩 들어온다 -_-;
		// return IS_HACKING( this, "Recv_cli_TARGET_SKILL :: no target type skill" );
#ifdef	__INC_WORLD
		sprintf( szTmp, "ignore Skill:: invalied self skill");
		this->Send_gsv_WHISPER( "TARGET_SKILL", szTmp );
#endif
		return true;
	}

	// 타겟이 유효한지( 살아 있는지 )... 체크...
	if ( this->Do_TargetSKILL( pPacket->m_cli_TARGET_SKILL.m_wDestObjIDX, nSkillIDX ) ) {
		// 시작 성공...
		this->m_dwLastSkillActiveTIME = dwCurTime;
//	김영환 수정
#ifdef	__MILEAGE_SKILL_USED
		this->m_dwLastSkillSpellTIME[ pPacket->m_cli_TARGET_SKILL.m_wSkillSLOT ] = dwCurTime;
#else
		this->m_dwLastSkillSpellTIME[ pPacket->m_cli_TARGET_SKILL.m_btSkillSLOT ] = dwCurTime;
#endif
		this->m_dwLastSkillGroupSpellTIME[ SKILL_RELOAD_TYPE(nSkillIDX) ] = dwCurSec;
		//////////////////////////////////////////////////////////////////////////
		// 2006.06.20:이성활:Group Skill Spell Time 적용시 마지막 사용 skill의 Spell Time으로 적용되어야
		//					되므로 마지막 사용 skill을 저장
		m_nLastSpellGroupSkillIndex[ SKILL_RELOAD_TYPE(nSkillIDX) ] = nSkillIDX;
		//////////////////////////////////////////////////////////////////////////
	}

	return true;
}

/// 지역 스킬 요청 받음 
bool classUSER::Recv_cli_POSITION_SKILL( t_PACKET *pPacket )
{
	if ( !(this->m_dwPayFLAG & PLAY_FLAG_BATTLE) ) {
		// 공격 못해... 돈내야함
		return true;
	}

//#ifndef	__INC_WORLD
//	if ( this->m_IngSTATUS.IsSubSET( FLAG_CHEAT_INVINCIBLE ) ) // 무적 모드시 공격 불가...
//		return true;
//#endif
	DWORD dwCurTime = this->GetZONE()->GetCurrentTIME();
	if ( SKILL_DELAY_TIME > dwCurTime - this->m_dwLastSkillActiveTIME ) {
#ifdef	__INC_WORLD
		char szTmp[ MAX_PATH ];
		sprintf( szTmp, "ignore Skill:: Delay:%d, Charged:%d", SKILL_DELAY_TIME, dwCurTime - this->m_dwLastSkillActiveTIME );
		this->Send_gsv_WHISPER( "SELF_TARGET", szTmp );
#endif
		return true;
	}

	if ( this->Get_ActiveSKILL() ) return true;	// 스킬 케스팅 중일땐 못바꿔
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() || this->m_IngSTATUS.IsSET( FLAG_ING_DUMB ) ) return true;

	//	김영환 수정
#ifdef	__MILEAGE_SKILL_USED
	if ( pPacket->m_cli_POSITION_SKILL.m_wSkillSLOT >= MAX_LEARNED_SKILL_CNT ) 
		return false;
	DWORD dwCurSec = classTIME::GetCurrentAbsSecond();
	short nSkillIDX = this->m_Skills.m_nSkillINDEX[ pPacket->m_cli_POSITION_SKILL.m_wSkillSLOT ];
#else
	if ( pPacket->m_cli_POSITION_SKILL.m_btSkillSLOT >= MAX_LEARNED_SKILL_CNT ) 
		return false;
	DWORD dwCurSec = classTIME::GetCurrentAbsSecond();
	short nSkillIDX = this->m_Skills.m_nSkillINDEX[ pPacket->m_cli_POSITION_SKILL.m_btSkillSLOT ];
#endif

	if ( IsTAIWAN() ) {

		if ( SKILL_AVAILBLE_STATUS(nSkillIDX) && !(m_btRideATTR & SKILL_AVAILBLE_STATUS(nSkillIDX)) )
			return true;

		if ( SKILL_RELOAD_TYPE(nSkillIDX) ) 
		{
			// 05.05.30 스킬 리로드 그룹타입으로 체크...
			//////////////////////////////////////////////////////////////////////////
			// 2006.06.20:이성활:Group Skill Spell Time 적용시 마지막 사용 skill의 Spell Time으로 적용되어야
			//					되므로 마지막 사용 skill의 Reload Second를 사용
			//if ( SKILL_RELOAD_SECOND(nSkillIDX) > (dwCurSec - this->m_dwLastSkillGroupSpellTIME[ SKILL_RELOAD_TYPE(nSkillIDX) ])*1000 ) 
			if ( SKILL_RELOAD_SECOND(m_nLastSpellGroupSkillIndex[ SKILL_RELOAD_TYPE(nSkillIDX) ]) > (dwCurSec - this->m_dwLastSkillGroupSpellTIME[ SKILL_RELOAD_TYPE(m_nLastSpellGroupSkillIndex[ SKILL_RELOAD_TYPE(nSkillIDX) ]) ])*1000 )
			//////////////////////////////////////////////////////////////////////////
			{
	#ifdef	__INC_WORLD
				char szTmp[ MAX_PATH ];
				sprintf( szTmp, "ignore SkillIdx:%d, :: Delay:%d > Charged:%d, LastStamp:%d \n", 
						nSkillIDX, 
						SKILL_RELOAD_SECOND(nSkillIDX), 
						dwCurTime - this->m_dwLastSkillGroupSpellTIME[SKILL_RELOAD_TYPE(nSkillIDX)  ], 
						this->m_dwLastSkillGroupSpellTIME[SKILL_RELOAD_TYPE(nSkillIDX)  ] );
				this->Send_gsv_WHISPER( "SELF_SKILL", szTmp );
	#endif
				return true;
			}
		}
	}

//	김영환 수정
#ifdef	__MILEAGE_SKILL_USED
	if ( SKILL_RELOAD_SECOND(nSkillIDX) > dwCurTime - this->m_dwLastSkillSpellTIME[ pPacket->m_cli_POSITION_SKILL.m_wSkillSLOT ] ) {
		// 다시 사용가능한 시간인지 체크...
#ifdef	__INC_WORLD
		char szTmp[ MAX_PATH ];
		sprintf( szTmp, "ignore SkillIdx:%d, :: Delay:%d > Charged:%d, LastStamp:%d \n", 
				nSkillIDX, 
				SKILL_RELOAD_SECOND(nSkillIDX), 
				dwCurTime - this->m_dwLastSkillSpellTIME[ pPacket->m_cli_POSITION_SKILL.m_wSkillSLOT ], 
				this->m_dwLastSkillSpellTIME[ pPacket->m_cli_POSITION_SKILL.m_wSkillSLOT ] );
		this->Send_gsv_WHISPER( "POSITION_SKILL", szTmp );
#endif
		return true;
	}
#else
	if ( SKILL_RELOAD_SECOND(nSkillIDX) > dwCurTime - this->m_dwLastSkillSpellTIME[ pPacket->m_cli_POSITION_SKILL.m_btSkillSLOT ] ) {
		// 다시 사용가능한 시간인지 체크...
#ifdef	__INC_WORLD
		char szTmp[ MAX_PATH ];
		sprintf( szTmp, "ignore SkillIdx:%d, :: Delay:%d > Charged:%d, LastStamp:%d \n", 
				nSkillIDX, 
				SKILL_RELOAD_SECOND(nSkillIDX), 
				dwCurTime - this->m_dwLastSkillSpellTIME[ pPacket->m_cli_POSITION_SKILL.m_btSkillSLOT ], 
				this->m_dwLastSkillSpellTIME[ pPacket->m_cli_POSITION_SKILL.m_btSkillSLOT ] );
		this->Send_gsv_WHISPER( "POSITION_SKILL", szTmp );
#endif
		return true;
	}
#endif

	if ( this->Get_WeightRATE() >= WEIGHT_RATE_CANT_ATK ) {
		// 무겁다.. 명령 불가...
		if ( ( SKILL_TYPE( nSkillIDX ) >= 3 && SKILL_TYPE( nSkillIDX ) <= 13 ) ||
			   SKILL_TYPE( nSkillIDX ) == 17 )
			return true;
	}
	if ( this->Get_RideMODE() )
	{
#ifdef __KCHS_BATTLECART__
		if( !this->CanDoPatSkill( nSkillIDX ) )
			return true;
#else
		return true;
#endif
	}

	switch ( SKILL_TYPE( nSkillIDX ) ) {
		case SKILL_TYPE_07 :					// 지역 마법 공격형
			if ( this->GetCur_RIDE_MODE() )		// 탑승시 사용 할수 없다.
				return true;
			break;

		default :
			return IS_HACKING( this, "Recv_cli_POSITION_SKILL :: no position type skill" );
	} 

	if ( this->Skill_ActionCondition( nSkillIDX ) ) {
		// 실제 필요 수치 소모 적용...
		// if ( this->SetCMD_Skill2POS( pPacket->m_cli_POSITION_SKILL.m_PosTARGET, nSkillIDX ) )
		//	this->Skill_UseAbilityValue( nSkillIDX );
		if ( this->SetCMD_Skill2POS( pPacket->m_cli_POSITION_SKILL.m_PosTARGET, nSkillIDX ) ) {
			// 시작 성공...
			this->m_dwLastSkillActiveTIME = dwCurTime;
//	김영환 수정
#ifdef	__MILEAGE_SKILL_USED
			this->m_dwLastSkillSpellTIME[ pPacket->m_cli_POSITION_SKILL.m_wSkillSLOT ] = dwCurTime;
#else
			this->m_dwLastSkillSpellTIME[ pPacket->m_cli_POSITION_SKILL.m_btSkillSLOT ] = dwCurTime;
#endif

			this->m_dwLastSkillGroupSpellTIME[ SKILL_RELOAD_TYPE(nSkillIDX) ] = dwCurSec;
			//////////////////////////////////////////////////////////////////////////
			// 2006.06.20:이성활:Group Skill Spell Time 적용시 마지막 사용 skill의 Spell Time으로 적용되어야
			//					되므로 마지막 사용 skill을 저장
			m_nLastSpellGroupSkillIndex[ SKILL_RELOAD_TYPE(nSkillIDX) ] = nSkillIDX;
			//////////////////////////////////////////////////////////////////////////			

//-------------------------------------
//2006.06.14/김대성/추가
			this->UpdateAbility ();		// 캐릭터 능력치 갱신
			Send_CHAR_HPMP_INFO(0);		// HP, MP정보를 한번 더 보내준다.
//-------------------------------------


		}
	}

	return true;
}


//-------------------------------------------------------------------------------------------------
/// 퀘스트 추가/실행에 대한 결과 통보
bool classUSER::Send_gsv_QUEST_REPLY (BYTE btResult, BYTE btSlot, int iQuestID)
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_QUEST_REPLY;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_QUEST_REPLY);

    pCPacket->m_gsv_QUEST_REPLY.m_btResult	  = btResult;
    pCPacket->m_gsv_QUEST_REPLY.m_btQuestSLOT = btSlot;
    pCPacket->m_gsv_QUEST_REPLY.m_iQuestID	  = iQuestID;

    this->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	return true;
}
/// 퀘스트 실행...
bool classUSER::Do_QuestTRIGGER( t_HASHKEY HashTRIGGER, short nSelectReward, t_HASHKEY HashSucTRIGGER)
{
	// 서버에서만 체크하고 클라이언트에서 체크하지 않는 조건일 경우
	// 서버에서 조건이 false될수 있다...무조건 접속 끊으면 안됨... 실패를 통보할꺼냐? 걍 무시 할꺼냐???
	eQST_RESULT eResult = g_QuestList.CheckQUEST( this, HashTRIGGER, true, this->m_iLastEventNpcIDX, NULL, nSelectReward );
	this->m_iLastEventNpcIDX = 0;
	switch ( eResult ) 
	{
		case QST_RESULT_SUCCESS :
			{
			//	유저가 보유하고 있는 퀘스트 개수가 9이면 더이상 퀘스트 처리 하지 않은다.
			//	퀘스트 로그 저장.
			g_pThreadLOG->When_QuestLOG( this, HashTRIGGER, NEWLOG_QUEST_HASH_DONE );
			// g_pThreadLOG->When_DoneQUEST( this, HashTRIGGER );

			return this->Send_gsv_QUEST_REPLY(RESULT_QUEST_REPLY_TRIGGER_SUCCESS, 0, (int)HashTRIGGER );
			}
		case QST_RESULT_FAILED :
			return this->Send_gsv_QUEST_REPLY(RESULT_QUEST_REPLY_TRIGGER_FAILED, 0, (int)HashTRIGGER );
		case QST_RESULT_STOPPED :
			return true;
		case QST_RESULT_INVALID :
			// 무시...
			return true;
		default :
		{
			CQuestTRIGGER *pQuestTrigger = g_QuestList.GetQuest( HashTRIGGER );

			if ( pQuestTrigger ) 
			{
				// 우선 짤르지 말고...
				g_LOG.CS_ODS( 0xffff, ">>> ERROR(%s,ZoneNO:%d):: QuestTrigger( %s:Result:%d )\n", 
							this->Get_NAME(), this->GetZONE()->Get_ZoneNO(), 
							pQuestTrigger->m_Name.Get(), eResult);//HashTRIGGER );
				return true;
			}
			else
			{
				g_LOG.CS_ODS( 0xffff, ">>> ERROR(%s,ZoneNO:%d):: QuestTrigger not found( Result:%d, Hash:%d )\n", 
							this->Get_NAME(), this->GetZONE()->Get_ZoneNO(), eResult, HashTRIGGER );
			}
		}
	} // switch ( eResult )


	// 클라이언트에서 이미 한번 검증한후 보내기 때문에 실패할 경우 
	// 데이타가 틀릴수 있으므로 짤르고 다시 접속하도록 유도... !!!
	// return this->Send_gsv_QUEST_REPLY(RESULT_QUEST_REPLY_TRIGGER_FAILED, 0, (int)HashTRIGGER );
	return true;
}
/// 퀘스트 삭제/완료체크 를 요청 받음
bool classUSER::Recv_cli_QUEST_REQ( t_PACKET *pPacket )
{
	switch( pPacket->m_cli_QUEST_REQ.m_btTYPE ) 
	{

		case TYPE_QUEST_REQ_DEL :
			if ( this->Quest_Delete( pPacket->m_cli_QUEST_REQ.m_btQuestSLOT, pPacket->m_cli_QUEST_REQ.m_iQuestID ) ) {
				// 퀘스트 포기 로그...
				
				g_pThreadLOG->When_QuestLOG( this, pPacket->m_cli_QUEST_REQ.m_iQuestID, NEWLOG_QUEST_GIVEUP );
			
				return this->Send_gsv_QUEST_REPLY( RESULT_QUEST_REPLY_DEL_SUCCESS, pPacket->m_cli_QUEST_REQ.m_btQuestSLOT, pPacket->m_cli_QUEST_REQ.m_iQuestID );
			}
			break;

		case TYPE_QUEST_REQ_DO_TRIGGER :
			if( pPacket->m_HEADER.m_nSize < sizeof( cli_QUEST_REQ_EX ) )
			{
				return this->Do_QuestTRIGGER( pPacket->m_cli_QUEST_REQ.m_TriggerHash/*보상선택 인덱스 */);
			}
			else
			{
				return this->Do_QuestTRIGGER( pPacket->m_cli_QUEST_REQ_EX.m_TriggerHash/*보상선택 인덱스 */, -1, pPacket->m_cli_QUEST_REQ_EX.m_SucTriggerHash);
			}
	}

	IS_HACKING( this, "classUSER::cli_QUEST_REQ( Script hacking.. )" );

	return false;
}

/*
bool classUSER::Recv_cli_ADD_QUEST( t_PACKET *pPacket )
{
	if ( pPacket->m_cli_ADD_QUEST.m_btQuestSLOT >= QUEST_PER_PLAYER )
		return false;

	this->m_Quests.m_QUEST[ pPacket->m_cli_ADD_QUEST.m_btQuestSLOT ].Init ();
	this->m_Quests.m_QUEST[ pPacket->m_cli_ADD_QUEST.m_btQuestSLOT ].SetID( pPacket->m_cli_ADD_QUEST.m_iQuestID );

	return true;
}

bool classUSER::Recv_cli_DEL_QUEST( t_PACKET *pPacket )
{
	if ( pPacket->m_cli_DEL_QUEST.m_btQuestSLOT >= QUEST_PER_PLAYER )
		return false;

	if ( pPacket->m_cli_DEL_QUEST.m_iQuestID == this->m_Quests.m_QUEST[ pPacket->m_cli_DEL_QUEST.m_btQuestSLOT ].GetID() ) {
		this->m_Quests.m_QUEST[ pPacket->m_cli_DEL_QUEST.m_btQuestSLOT ].Init ();
	}

	return true;
}

bool classUSER::Recv_cli_QUEST_DATA_REQ( t_PACKET *pPacket )
{
	if ( pPacket->m_cli_QUEST_DATA_REQ.m_btQuestSLOT >= QUEST_PER_PLAYER )
		return false;

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_gsv_QUEST_DATA_REPLY.m_btQuestSLOT = pPacket->m_cli_QUEST_DATA_REQ.m_btQuestSLOT;
	::CopyMemory( &pCPacket->m_gsv_QUEST_DATA_REPLY.m_Quest, &this->m_Quests.m_QUEST[ pPacket->m_cli_QUEST_DATA_REQ.m_btQuestSLOT ], sizeof( CQUEST ) );

	this->SendPacket( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

	return true;
}
*/

//-------------------------------------------------------------------------------------------------
/// 파티 관련 요청 패킷
bool classUSER::Recv_cli_PARTY_REQ( t_PACKET *pPacket )
{
	switch( pPacket->m_cli_PARTY_REQ.m_btREQUEST ) {
		case PARTY_REQ_BAN :		// 파짱 이어야 한다.
			if ( this->GetPARTY() && this == this->m_pPartyBUFF->GetPartyOWNER() ) {
				this->m_pPartyBUFF->Kick_MEMBER( pPacket->m_cli_PARTY_REQ.m_dwDestIDXorTAG );
			}
			return true;

		case PARTY_REQ_LEFT :		// 내가 나간다.
			if ( this->GetPARTY() ) {
				this->m_pPartyBUFF->Sub_PartyUSER( this->m_nPartyPOS );		// PARTY_REQ_LEFT
			}
			return true;
	} // switch( pPacket->m_cli_PARTY_REQ.m_btREQUEST )


	// 자신한테 파티가 신청되는 황당한 패킷이 온다고...
	if ( this->Get_INDEX() == pPacket->m_cli_PARTY_REQ.m_dwDestIDXorTAG )
		return false;

	classUSER *pUSER = g_pObjMGR->Get_UserOBJ( pPacket->m_cli_PARTY_REQ.m_dwDestIDXorTAG );
	if ( NULL == pUSER ) {
		return this->Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REQ.m_dwDestIDXorTAG, PARTY_REPLY_NOT_FOUND );
	}

	switch( pPacket->m_cli_PARTY_REQ.m_btREQUEST ) {
		case PARTY_REQ_MAKE :
			// 과금 안되어 있다면 파티 못한다.
			if ( !(this->m_dwPayFLAG & PAY_FLAG_JP_BATTLE) )
				return true;
			if ( !(pUSER->m_dwPayFLAG & PAY_FLAG_JP_BATTLE) ) {
				return this->Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REQ.m_dwDestIDXorTAG, PAATY_REPLY_NO_CHARGE_TARGET );
			}

			if ( this->GetPARTY() )
				return true;
			if ( pUSER->Get_HP() <= 0 || pUSER->GetPARTY() ) {
				// 이미 다른 파티에 참가 중이면..
				return this->Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REQ.m_dwDestIDXorTAG, PARTY_REPLY_BUSY );
			}

			// 결성시 파티렙은 0이다 !!
			if ( !this->Check_PartyJoinLEVEL( pUSER->Get_LEVEL(), this->Get_LEVEL(), 0 ) ) {
				return Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REQ.m_dwDestIDXorTAG, PARTY_REPLY_INVALID_LEVEL );
			}

			//	클랜전에 참여 중인 경우 다른 팀에 파티 신정 안된다.
			switch(this->Get_TeamNO())
			{
			case 1000:
				//	A팀인 경우
				if(pUSER->Get_TeamNO() != 1000)
				{
					return this->Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REQ.m_dwDestIDXorTAG, PARTY_REPLY_NOT_FOUND );
				}
				break;
			case 2000:
				//	B팀인 경우
				if(pUSER->Get_TeamNO() != 2000)
				{
					return this->Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REQ.m_dwDestIDXorTAG, PARTY_REPLY_NOT_FOUND );
				}
				break;
			}
			return pUSER->Send_gsv_PARTY_REQ( this->Get_INDEX(), PARTY_REQ_MAKE );

		case PARTY_REQ_JOIN :
			// 과금 안되어 있다면 파티 못한다.
			if ( !(this->m_dwPayFLAG & PAY_FLAG_JP_BATTLE) ) {
				return true;
			}
			if ( !(pUSER->m_dwPayFLAG & PAY_FLAG_JP_BATTLE) ) {
				return this->Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REQ.m_dwDestIDXorTAG, PAATY_REPLY_NO_CHARGE_TARGET );
			}

			if ( !this->GetPARTY() )
				return true;

			if ( pUSER->Get_HP() <= 0 || pUSER->GetPARTY() ) {
				// 이미 다른 파티에 참가 중이면..
				return this->Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REQ.m_dwDestIDXorTAG, PARTY_REPLY_BUSY );
			}

			// pUSER의 가입 레벨 체크...
			if ( !this->Check_PartyJoinLEVEL( pUSER->Get_LEVEL(), this->m_pPartyBUFF->GetAverageLEV(), this->m_pPartyBUFF->GetPartyLEV() ) ) 
			{
				return Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REQ.m_dwDestIDXorTAG, PARTY_REPLY_INVALID_LEVEL );
			}

			//	클랜전에 참여 중인 경우 다른 팀에 파티 신정 안된다.
			switch(this->Get_TeamNO())
			{
			case 1000:
				//	A팀인 경우
				if(pUSER->Get_TeamNO() != 1000)
				{
					return this->Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REQ.m_dwDestIDXorTAG, PARTY_REPLY_NOT_FOUND );
				}
				break;
			case 2000:
				//	B팀인 경우
				if(pUSER->Get_TeamNO() != 2000)
				{
					return this->Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REQ.m_dwDestIDXorTAG, PARTY_REPLY_NOT_FOUND );
				}
				break;
			}
			return pUSER->Send_gsv_PARTY_REQ( this->Get_INDEX(), PARTY_REQ_JOIN );

		case PARTY_REQ_CHANGE_OWNER :
			if ( this->GetPARTY() && this->GetPARTY() == pUSER->GetPARTY() ) {
				this->m_pPartyBUFF->Change_PartyOWNER( pUSER );
			}
			break;

		default :
			return false;
	}

	return true;
}

/// 파티 관련 응답 패킷
bool classUSER::Recv_cli_PARTY_REPLY( t_PACKET *pPacket )
{
	classUSER *pUSER = g_pObjMGR->Get_UserOBJ( pPacket->m_cli_PARTY_REPLY.m_dwDestIDXorTAG );
	if ( NULL == pUSER ) {
		return this->Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REPLY.m_dwDestIDXorTAG, PARTY_REPLY_NOT_FOUND );
	}

	switch( pPacket->m_cli_PARTY_REPLY.m_btREPLY ) {
		case PARTY_REPLY_ACCEPT_MAKE :
		{
			if ( this->GetPARTY() ) {
				// 파티 결성 요청을 허락했는데 내가 이미 다른 파티원이라면 ???
				// 내가 파티 결성을 요청한 사람 사람이 받아들여져 파티가 결성될때...
				return true;
			}

			if ( NULL == pUSER->GetPARTY() ) {
				// Parties who formed the party makes the request.
				if ( !g_pPartyBUFF->CreatePARTY( pUSER ) ) {
					return this->Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REPLY.m_dwDestIDXorTAG, PARTY_REPLY_DESTROY );
				}
			} else
			if ( pUSER != pUSER->m_pPartyBUFF->GetPartyOWNER() ) {
				// Formed by the party requesting the other party has already decided to join.
				return this->Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REPLY.m_dwDestIDXorTAG, PARTY_REPLY_DESTROY );
			}

			// 파짱이 된 유저에게 허락 패킷 전송.
			pUSER->Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REPLY.m_dwDestIDXorTAG, PARTY_REPLY_ACCEPT_MAKE );

			BYTE btFailed = pUSER->m_pPartyBUFF->Add_PartyUSER( this );
			if ( btFailed )
				this->Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REPLY.m_dwDestIDXorTAG, btFailed );
		}

		case PARTY_REPLY_ACCEPT_JOIN :
		{
			if ( this->GetPARTY() ) {
				// 내가 파티 참가를 허락한 상태에서 내가 파티가 있다면 ???
				// 내가 파티 결성을 요청한 다른 이가 허럭해서 내가 파티에 조인이 된경우...
				return true;
			}
			//IS_HACKING( this, "Recv_cli_PARTY_REPLY" );

			// 파티결성 요청자, 가입 요청자가 자신의 파티의 짱인가 ??
			if ( pUSER->GetPARTY() && pUSER == pUSER->m_pPartyBUFF->GetPartyOWNER() ) {
				BYTE btFailed = pUSER->m_pPartyBUFF->Add_PartyUSER( this );
				if ( btFailed )
					this->Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REPLY.m_dwDestIDXorTAG, btFailed );
			} else
				this->Send_gsv_PARTY_REPLY( pPacket->m_cli_PARTY_REPLY.m_dwDestIDXorTAG, PARTY_REPLY_DESTROY );
			break;
		}

		case PARTY_REPLY_REJECT_JOIN :
			pUSER->Send_gsv_PARTY_REPLY( this->Get_INDEX(), PARTY_REPLY_REJECT_JOIN );
			break;

		default :
			pUSER->Send_gsv_PARTY_REPLY( this->Get_INDEX(), pPacket->m_cli_PARTY_REPLY.m_btREPLY );
			break;
	} // switch( pPacket->m_cli_PARTY_REPLY.m_btREPLY )


	return true;
}


//-------------------------------------------------------------------------------------------------
/// 아이템 옵션 검증 요청 패킷
bool classUSER::Recv_cli_APPRAISAL_REQ( t_PACKET *pPacket )
{
	if ( pPacket->m_cli_APPRAISAL_REQ.m_wInventoryIndex < 1 ||
		 pPacket->m_cli_APPRAISAL_REQ.m_wInventoryIndex >= INVENTORY_TOTAL_SIZE ) {
		return IS_HACKING( this, "Recv_cli_APPRAISAL_REQ" );
	}

	tagITEM *pITEM = &this->m_Inventory.m_ItemLIST[ pPacket->m_cli_APPRAISAL_REQ.m_wInventoryIndex ];
	if ( !pITEM->IsEquipITEM() || pITEM->IsAppraisal() )
		return true;
	int iNeedMoney = CCal::GetMoney_WhenAPPRAISAL( ITEM_BASE_PRICE(pITEM->GetTYPE(), pITEM->GetItemNO() ), pITEM->GetDurability() );

    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_APPRAISAL_REPLY;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_APPRAISAL_REPLY );
	pCPacket->m_gsv_APPRAISAL_REPLY.m_wInventoryIndex = pPacket->m_cli_APPRAISAL_REQ.m_wInventoryIndex;

	if ( iNeedMoney > this->Get_MONEY() ) {	// 돈없당
		pCPacket->m_gsv_APPRAISAL_REPLY.m_btResult = RESULT_APPRAISAL_REPLY_FAILED;
	} else {
		this->Sub_CurMONEY( iNeedMoney );
		pITEM->m_bIsAppraisal = 1;
		pCPacket->m_gsv_APPRAISAL_REPLY.m_btResult = RESULT_APPRAISAL_REPLY_OK;
	}

    this->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	if ( pPacket->m_cli_APPRAISAL_REQ.m_wInventoryIndex < MAX_EQUIP_IDX /* || pPacket->m_cli_APPRAISAL_REQ.m_wInventoryIndex >= INVENTORY_RIDE_ITEM0 */ ) {
		// TODO:: PAT아이템은 ??
		// 장착된 장비이므로 능력치 변경됨 이속, 회복속도 변경 되면 주변에 통보 필요
		if ( this->GetPARTY() ) {
			BYTE btCurCON = this->GetCur_CON();
			BYTE btRecvHP = this->m_btRecoverHP;
			BYTE btRecvMP = this->m_btRecoverMP;

			this->UpdateAbility ();		// appraisal

			// 변경에 의해 옵션이 붙어 회복이 바뀌면 파티원에게 전송.
			if ( btCurCON != this->GetCur_CON()  ||
				 btRecvHP != this->m_btRecoverHP ||
				 btRecvMP != this->m_btRecoverMP ) {
				this->m_pPartyBUFF->Change_ObjectIDX( this );
			}
		} else {
			this->UpdateAbility ();		// appraisal
		}
		this->Send_gsv_EQUIP_ITEM( pPacket->m_cli_APPRAISAL_REQ.m_wInventoryIndex, pITEM );
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 소모 아이템으로 아이템 수리 요청
bool classUSER::Recv_cli_USE_ITEM_TO_REPAIR( t_PACKET *pPacket )
{
	if ( this->m_IngSTATUS.IsSET( FLAG_ING_IGNORE_ALL ) ) return true;

    if ( pPacket->m_cli_USE_ITEM_TO_REPAIR.m_nUseItemInvIDX < 0 || 
		 pPacket->m_cli_USE_ITEM_TO_REPAIR.m_nUseItemInvIDX >= INVENTORY_TOTAL_SIZE ) {
        // 패킷이 조작된것인가?
		return IS_HACKING( this, "Recv_cli_USE_ITEM_TO_REPLY-1" );
    }
    if ( pPacket->m_cli_USE_ITEM_TO_REPAIR.m_nRepairTargetInvIDX < 0 || 
		 pPacket->m_cli_USE_ITEM_TO_REPAIR.m_nRepairTargetInvIDX >= INVENTORY_TOTAL_SIZE ) {
        // 패킷이 조작된것인가?
		return IS_HACKING( this, "Recv_cli_USE_ITEM_TO_REPLY-2" );
    }

	tagITEM *pUseITEM = &this->m_Inventory.m_ItemLIST[ pPacket->m_cli_USE_ITEM_TO_REPAIR.m_nUseItemInvIDX ];
	if ( ITEM_TYPE_USE != pUseITEM->GetTYPE() || USE_ITEM_REPAIR_ITEM != ITEM_TYPE( pUseITEM->GetTYPE(), pUseITEM->GetItemNO() ) )
		return true;

	tagITEM *pTgtITEM = &this->m_Inventory.m_ItemLIST[ pPacket->m_cli_USE_ITEM_TO_REPAIR.m_nRepairTargetInvIDX ];
	if ( !pTgtITEM->IsEquipITEM() ) {
		// 장비가 아니다.
		if ( ITEM_TYPE_RIDE_PART != pTgtITEM->GetTYPE() )
			return true;
		// 엔진을 수리 못한다... 연료로 보충을 해야쥐~~~
		if ( RIDE_PART_ENGINE == PAT_ITEM_PART_IDX( pTgtITEM->GetItemNO() ) )
			return true;
	}
	if ( pTgtITEM->GetDurability() <= 0 )	// 내구도가 0이면 수리 못한다.
		return true;

	// 내구도 감소없는 퍼팩트 망치냐 ???
	if ( 0 == USEITEM_ADD_DATA_VALUE( pUseITEM->GetItemNO() ) ) {
		int iDec = (int)( ( 1400 - pTgtITEM->GetLife() ) * ( RANDOM(100)+11 ) / ( pTgtITEM->GetDurability()+40) / 400.f );
		if ( pTgtITEM->GetDurability() >= iDec )
			pTgtITEM->m_cDurability -= iDec;
		else
			pTgtITEM->m_cDurability = 0;
	}

	pTgtITEM->m_nLife = MAX_ITEM_LIFE;
	this->UpdateAbility ();		// repair weapon

	// 수량 감소
	if ( --pUseITEM->m_uiQuantity <= 0 ) {
		// 다 소모했다..
		m_Inventory.DeleteITEM( pPacket->m_cli_USE_ITEM_TO_REPAIR.m_nUseItemInvIDX );
	}
	
	this->Send_gsv_SET_TWO_INV_ONLY( GSV_USED_ITEM_TO_REPAIR, 
			(BYTE)pPacket->m_cli_USE_ITEM_TO_REPAIR.m_nUseItemInvIDX,		pUseITEM,
			(BYTE)pPacket->m_cli_USE_ITEM_TO_REPAIR.m_nRepairTargetInvIDX,	pTgtITEM);

	return true;
}
/// npc를 통해 아이템 수리 요청
bool classUSER::Recv_cli_REPAIR_FROM_NPC( t_PACKET *pPacket )
{
	CObjNPC *pCharNPC;
	pCharNPC = (CObjNPC*)g_pObjMGR->Get_GameOBJ( pPacket->m_cli_REPAIR_FROM_NPC.m_wNPCObjIDX, OBJ_NPC );
	if ( !pCharNPC ) {
		return false;
	}

	// 상점 npc와의 거래 체크...
	int iDistance = distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)pCharNPC->m_PosCUR.x, (int)pCharNPC->m_PosCUR.y);
	if ( iDistance > MAX_TRADE_DISTANCE )	{ 
		return true;
	}

	tagITEM *pTgtITEM = &this->m_Inventory.m_ItemLIST[ pPacket->m_cli_REPAIR_FROM_NPC.m_nRepairTargetInvIDX ];
	if ( !pTgtITEM->IsEquipITEM() ) {
		// 장비가 아니다.
		if ( ITEM_TYPE_RIDE_PART != pTgtITEM->GetTYPE() )
			return true;
		// 엔진을 수리 못한다... 연료로 보충을 해야쥐~~~
		if ( RIDE_PART_ENGINE == PAT_ITEM_PART_IDX( pTgtITEM->GetItemNO() ) )
			return true;
	}
	if ( pTgtITEM->GetDurability() <= 0 )	// 내구도가 0이면 수리 못한다.
		return true;

	int iFee = (int)( ( ITEM_BASE_PRICE(pTgtITEM->GetTYPE(), pTgtITEM->GetItemNO() ) + 1000 ) / 400000.f
					* ( pTgtITEM->GetDurability() + 10 )
					* ( 1100 - pTgtITEM->GetLife() ) );
	if ( this->Get_MONEY() < iFee ) {
		// 돈 없다.
		return true;
	}

	this->Sub_CurMONEY( iFee );
	pTgtITEM->m_nLife = MAX_ITEM_LIFE; 
	this->UpdateAbility ();		// repair weapon

	classPACKET *pCPacket = this->Init_gsv_SET_MONEYnINV( GSV_REPAIRED_FROM_NPC );
	if ( pCPacket ) {
		pCPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ 0 ].m_btInvIDX = (BYTE)pPacket->m_cli_REPAIR_FROM_NPC.m_nRepairTargetInvIDX;
		pCPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ 0 ].m_ITEM = *pTgtITEM;
		pCPacket->m_gsv_SET_MONEYnINV.m_btItemCNT = 1;
		this->Send_gsv_SET_MONEYnINV( pCPacket );
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 아이템의 수명을 통보
bool classUSER::Send_gsv_SET_ITEM_LIFE (short nInvIDX, short nLife)
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_SET_ITEM_LIFE;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_ITEM_LIFE );

    pCPacket->m_gsv_SET_ITEM_LIFE.m_nInventoryIDX = nInvIDX;
	pCPacket->m_gsv_SET_ITEM_LIFE.m_nLife = nLife;

	this->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	return true;
}


//-------------------------------------------------------------------------------------------------
/// 메신져 :: 개인섭 테스트용함수
bool classUSER::Send_tag_MCMD_HEADER( BYTE btCMD, char *szStr )
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = WSV_MESSENGER;
    pCPacket->m_HEADER.m_nSize = sizeof( tag_MCMD_HEADER );
	pCPacket->m_tag_MCMD_HEADER.m_btCMD = btCMD;
	if ( szStr )
		pCPacket->AppendString( szStr );

	this->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	return true;
}
/// 메신져 :: 개인섭 테스트용함수
bool classUSER::Recv_cli_MCMD_APPEND_REQ( t_PACKET *pPacket )
{
	char *szName = pPacket->m_cli_MCMD_APPEND_REQ.m_szName;
	classUSER *pDestUSER = g_pUserLIST->Find_CHAR( pPacket->m_cli_MCMD_APPEND_REQ.m_szName );
	if ( NULL == pDestUSER )
		return this->Send_tag_MCMD_HEADER( MSGR_CMD_NOT_FOUND, szName );

    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

#pragma COMPILE_TIME_MSG ( "친구 추가 거부 상태면..." )

    pCPacket->m_HEADER.m_wType = WSV_MESSENGER;
    pCPacket->m_HEADER.m_nSize = sizeof( wsv_MCMD_APPEND_REQ );

	pCPacket->m_wsv_MCMD_APPEND_REQ.m_btCMD = MSGR_CMD_APPEND_REQ;
	pCPacket->m_wsv_MCMD_APPEND_REQ.m_wUserIDX = this->m_iSocketIDX;
	pCPacket->AppendString( this->Get_NAME() );

	pDestUSER->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	return true;
}
/// 메신져 :: 개인섭 테스트용함수
bool classUSER::Recv_cli_MESSENGER( t_PACKET *pPacket )
{
#ifdef	__INC_WORLD
	switch( pPacket->m_tag_MCMD_HEADER.m_btCMD ) {
		case MSGR_CMD_APPEND_REQ	:	// 상대방에 요청
			return this->Recv_cli_MCMD_APPEND_REQ( pPacket );

		case MSGR_CMD_APPEND_REJECT	:	// 거부
		{
			classUSER *pDestUSER = (classUSER*)g_pUserLIST->GetSOCKET( pPacket->m_cli_MCMD_APPEND_REPLY.m_wUserIDX );
			if ( pDestUSER )
			{
#ifdef __COMMUNITY_1_1_CHAT
				short nOffset = 0;
				char* pName = NULL;

				nOffset = sizeof(cli_MCMD_APPEND_REPLY);
				pName = Packet_GetStringPtr( pPacket, nOffset );
				//				pwUserID = (WORD*)Packet_GetDataPtr( pPacket, nOffset, sizeof( WORD )); // 그냥 참고로 ㅡ.ㅡ;
				pDestUSER->Send_tag_MCMD_HEADER( MSGR_CMD_APPEND_REJECT, pName );
#else
				pDestUSER->Send_tag_MCMD_HEADER( MSGR_CMD_APPEND_REJECT, pDestUSER->Get_NAME() );
#endif
			}
			return true;
		}

		case MSGR_CMD_APPEND_ACCEPT :	// 쌍방에 추가..
			g_pThreadMSGR->Add_MessengerCMD( this->Get_NAME(), MSGR_CMD_APPEND_ACCEPT, pPacket, this->m_iSocketIDX );
			return true;

		default :
			g_pThreadMSGR->Add_MessengerCMD( this->Get_NAME(), pPacket->m_tag_MCMD_HEADER.m_btCMD, pPacket, this->m_iSocketIDX );
	} // switch( pPacket->m_tag_MCMD_HEADER.m_btCMD )
#endif

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 메신져 :: 개인섭 테스트용함수
bool classUSER::Recv_cli_MESSENGER_CHAT( t_PACKET *pPacket )
{
#ifdef	__INC_WORLD
	if ( pPacket->m_HEADER.m_nSize <= 1+sizeof( cli_MESSENGER_CHAT ) )
		return true;

	g_pThreadMSGR->Add_MessengerCMD( this->Get_NAME(), 0x0ff, pPacket, this->m_iSocketIDX );
#endif
	return true;
}

//-------------------------------------------------------------------------------------------------
/// Item Jamming / Disassembly Failure Result
bool classUSER::Send_gsv_CRAFT_ITEM_RESULT (BYTE btRESULT)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( pCPacket ) {
		pCPacket->m_HEADER.m_wType = GSV_CRAFT_ITEM_REPLY;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_CRAFT_ITEM_REPLY ) - sizeof(BYTE);

		pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_btRESULT = btRESULT;

		this->SendPacket( pCPacket );
	    Packet_ReleaseNUnlock( pCPacket );

		return true;
	}

	return false;
}
/// Item Jamming / initialization packet decomposition results
classPACKET *classUSER::Init_gsv_CRAFT_ITEM_REPLY ()
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( pCPacket ) {
		pCPacket->m_HEADER.m_wType = GSV_CRAFT_ITEM_REPLY;
	/*
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_CRAFT_ITEM_REPLY );
		pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_btRESULT = btRESULT;
		pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT = 0;
	*/
	}

	return pCPacket;
}
/// Item Jamming / degradation successful Result
void classUSER::Send_gsv_CRAFT_ITEM_REPLY( classPACKET *pCPacket, BYTE btRESULT, BYTE btOutCNT )
{
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_CRAFT_ITEM_REPLY ) + btOutCNT * sizeof( tag_SET_INVITEM );
	pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_btRESULT = btRESULT;
	pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT = btOutCNT;

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
}

/// Gemming Request
bool classUSER::Proc_CRAFT_GEMMING_REQ( t_PACKET *pPacket )
{
	if ( pPacket->m_cli_CRAFT_GEMMING_REQ.m_btEquipInvIDX >= MAX_EQUIP_IDX )
		return true;	
	if ( pPacket->m_cli_CRAFT_GEMMING_REQ.m_btJemInvIDX < INVENTORY_ETC_ITEM0 ||
		 pPacket->m_cli_CRAFT_GEMMING_REQ.m_btJemInvIDX >= INVENTORY_SHOT_ITEM0 )
		 return false;

	tagITEM *pEquipITEM = &this->m_Inventory.m_ItemLIST[ pPacket->m_cli_CRAFT_GEMMING_REQ.m_btEquipInvIDX ];
	if ( !pEquipITEM->IsEquipITEM() )
		return true;

	tagITEM *pJewelITEM = &this->m_Inventory.m_ItemLIST[ pPacket->m_cli_CRAFT_GEMMING_REQ.m_btJemInvIDX ];
	if ( ITEM_TYPE_GEM != pJewelITEM->GetTYPE() || pJewelITEM->GetQuantity() < 1 )
		return true;
	if ( pJewelITEM->GetItemNO() >= g_TblGEMITEM.m_nDataCnt ) {
		return false;
	}
	if ( 411 != ITEM_TYPE( ITEM_TYPE_GEM, pJewelITEM->GetItemNO() ) ) {
		return false;
	}

	if ( !pEquipITEM->HasSocket() ) {
		return this->Send_gsv_CRAFT_ITEM_RESULT( CRAFT_GEMMING_NEED_SOCKET );
	}
	if ( pEquipITEM->GetGemNO() > 300 ) {
		return this->Send_gsv_CRAFT_ITEM_RESULT( CRAFT_GEMMING_USED_SOCKET );
	}

	// 재밍 로그..
	g_pThreadLOG->When_GemmingITEM( this, pEquipITEM, pJewelITEM, NEWLOG_GEMMING, NEWLOG_SUCCESS );

	// 보석 박힌것은 자동 검증...
	pEquipITEM->m_nGEM_OP = pJewelITEM->GetItemNO();
	this->SetPartITEM( pPacket->m_cli_CRAFT_GEMMING_REQ.m_btEquipInvIDX );

	pJewelITEM->SubQuantity( 1 );

	classPACKET *pCPacket = this->Init_gsv_CRAFT_ITEM_REPLY ();
	if ( !pCPacket )
		return false;

	pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_btInvIDX = pPacket->m_cli_CRAFT_GEMMING_REQ.m_btEquipInvIDX;
	pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_ITEM		= *pEquipITEM;
	pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 1 ].m_btInvIDX = pPacket->m_cli_CRAFT_GEMMING_REQ.m_btJemInvIDX;
	pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 1 ].m_ITEM		= *pJewelITEM;

	this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFT_GEMMING_SUCCESS, 2 );

	// 장착된 장비이므로 주변에 통보 필요...
	this->UpdateAbility ();		// gemming
	this->Send_gsv_EQUIP_ITEM( pPacket->m_cli_CRAFT_GEMMING_REQ.m_btEquipInvIDX );

	return true;
}
/// 아이템 분해 요청
bool classUSER::Proc_CRAFT_BREAKUP_REQ( t_PACKET *pPacket, bool bUseMP )
{
	if ( pPacket->m_cli_CRAFT_BREAKUP_REQ.m_btTargetInvIDX <  MAX_EQUIP_IDX ||
		 pPacket->m_cli_CRAFT_BREAKUP_REQ.m_btTargetInvIDX >= INVENTORY_SHOT_ITEM0 )
		return false;

	tagITEM *pInITEM = &this->m_Inventory.m_ItemLIST[ pPacket->m_cli_CRAFT_BREAKUP_REQ.m_btTargetInvIDX ];
	if ( 0 == pInITEM->GetHEADER() ) 
		return true;



	if ( !pInITEM->IsEnableDupCNT() && pInITEM->HasSocket() && pInITEM->GetGemNO() > 300 ) 
	{
		if(!pInITEM->GetQuantity())
		{
			pInITEM->Clear();
			return false;
		}

		// 보석 분리
		short ORI_QUAL = ITEM_QUALITY( pInITEM->GetTYPE(), pInITEM->GetItemNO() );
		short GEM_QUAL = ITEM_QUALITY( ITEM_TYPE_GEM,	   pInITEM->GetGemNO() );

		short nNeed;//  = ( (ORI_QUAL/2) + GEM_QUAL );
		if ( bUseMP ) {
			nNeed = CCal::GetMP_WhenBreakupGEM( ORI_QUAL, GEM_QUAL );
			if ( this->Get_MP() < nNeed ) {
				// 엠 없다
				return true;
			}
			this->Sub_MP( nNeed );
		} else {
			nNeed = CCal::GetMONEY_WhenBreakupGEM( ORI_QUAL, GEM_QUAL );
			if ( this->GetCur_MONEY() < nNeed ) {
				// 돈 없다.
				return true;
			}
			this->Sub_CurMONEY( nNeed );
		}

		classPACKET *pCPacket = this->Init_gsv_CRAFT_ITEM_REPLY ();
		if ( !pCPacket )
			return false;

		BYTE btResult;
		tagITEM sOutITEM;
		sOutITEM.Clear ();

		if ( RANDOM(100)+1 + GEM_QUAL/6 <= 30 ) {	// 등급 감소
			if ( GEM_QUAL <= 35 ) {					// 보석 삭제
				// 로그:: 분해시 보석 삭제 됐음...
				g_pThreadLOG->When_GemmingITEM( this, pInITEM, NULL, NEWLOG_UNGEMMING, NEWLOG_FAILED );
			

				// 장비 아이템...
				pInITEM->m_nGEM_OP = 0;				// 장비에서 보석 삭제
				pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_btInvIDX =  pPacket->m_cli_CRAFT_BREAKUP_REQ.m_btTargetInvIDX;
				pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_ITEM		= *pInITEM;
				this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFT_BREAKUP_CLEARED_GEM, 1 );
				return true;
			}

			// 보석 아이템 생성...
			sOutITEM.m_nItemNo = pInITEM->GetGemNO() - 1;

			btResult = CRAFT_BREAKUP_DEGRADE_GEM;
		} else {
			sOutITEM.m_nItemNo = pInITEM->GetGemNO();
			btResult = CRAFT_BREAKUP_SUCCESS_GEM;
		}
		sOutITEM.m_cType = ITEM_TYPE_GEM;
		sOutITEM.m_uiQuantity = 1;

		
		g_pThreadLOG->When_GemmingITEM( this, pInITEM, &sOutITEM, NEWLOG_UNGEMMING, NEWLOG_SUCCESS );
	

		pInITEM->m_nGEM_OP = 0;				// 장비에서 보석 삭제
		pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_btInvIDX =  pPacket->m_cli_CRAFT_BREAKUP_REQ.m_btTargetInvIDX;
		pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_ITEM		= *pInITEM;

		// 보석 분리 됐음.
		short nInvIDX = this->Add_ITEM( sOutITEM );
		if ( nInvIDX > 0 ) {
			//pInITEM->m_nGEM_OP = 0;				// 장비에서 보석 삭제
			//pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_btInvIDX =  pPacket->m_cli_CRAFT_BREAKUP_REQ.m_btTargetInvIDX;
			//pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_ITEM		= *pInITEM;

			pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 1 ].m_btInvIDX = (BYTE)nInvIDX;
			pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 1 ].m_ITEM		= sOutITEM;
			
			this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, btResult, 2 );
		} else {
			this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, btResult, 1 );
			this->Save_ItemToFILED( sOutITEM );	// 보석 분리후 인벤토리 모자람...
		}
		return true;
	}
	// if ( !pInITEM->IsEnableDupCNT() && pInITEM->HasSocket() && pInITEM->GetGemNO() > 300 ) 
	else 

	{
		// 분해.
		if ( 0 == ITEM_PRODUCT_IDX( pInITEM->GetTYPE(), pInITEM->GetItemNO() ) ) {
			// 재료 번호 없는건 분해 못시켜 !!!
			return true;	// false
		}

		short ORI_QUAL		= ITEM_QUALITY	  ( pInITEM->GetTYPE(), pInITEM->GetItemNO() );
		short nProductIDX	= ITEM_PRODUCT_IDX( pInITEM->GetTYPE(), pInITEM->GetItemNO() );
		tagITEM sOutITEM;
		if ( PRODUCT_RAW_MATERIAL( nProductIDX ) ) {	// 재료 종류...
			short TEMP = ( ORI_QUAL - 20 ) / 12;			// nTemp = ORI_QUAL
			if ( TEMP < 1 ) TEMP = 1;
			else
			if ( TEMP > 10 ) TEMP = 10;

			sOutITEM.Clear();
			sOutITEM.m_cType     = ITEM_TYPE_NATURAL;
			sOutITEM.m_nItemNo   = ( PRODUCT_RAW_MATERIAL( nProductIDX ) - 421 ) * 10 + TEMP;
		} else {
			if ( 0 == PRODUCT_NEED_ITEM_NO(nProductIDX,0) ) {
				// 입력 오류~~~
				return true;
			}
			sOutITEM.Init( PRODUCT_NEED_ITEM_NO(nProductIDX,0), 1 );
		}

		int iTmpVAR;
		if ( bUseMP ) {
			iTmpVAR = CCal::GetMP_WhenBreakupMAT( ORI_QUAL );
			if ( this->Get_MP() < iTmpVAR ) {
				// 엠 없다
				return true;
			}
			this->Sub_MP( iTmpVAR );
		} else {
			iTmpVAR = CCal::GetMONEY_WhenBreakupMAT( ORI_QUAL );
			if ( this->GetCur_MONEY() < iTmpVAR ) {
				// 돈 없다.
				return true;
			}
			this->Sub_CurMONEY( iTmpVAR );
		}


		classPACKET *pCPacket = this->Init_gsv_CRAFT_ITEM_REPLY ();
		if ( !pCPacket )
			return false;

		short DURATION, LIFE, MAT_NUM;
		if ( pInITEM->IsEquipITEM() ) {
			DURATION = pInITEM->GetDurability();
			LIFE     = pInITEM->GetLife();
		} else {
			DURATION = 0;
			LIFE	 = 100;
		}

		short nStep = 0,nOutLogCnt=0;
		BYTE  btOutCNT=0;
		tagITEM sOutLogITEM[ CREATE_ITEM_STEP ];
		while( true ) {
			//switch( ITEM_TYPE( sOutITEM.GetTYPE(), sOutITEM.GetItemNO() ) ) {
			//	case 427 :	// 연금재료
			//	case 428 :	// 화학품
			//		// 분해 안되고 사라진다..
			//		break;
			//	default :
					MAT_NUM = PRODUCT_NEED_ITEM_CNT( nProductIDX, nStep );
					sOutITEM.m_uiQuantity = (ULONG)( ( MAT_NUM * (111+RANDOM(40)) * (DURATION/2+LIFE/10+100) ) / 60000 );

					if ( sOutITEM.GetQuantity() > 0 ) {
						sOutLogITEM[ nOutLogCnt ++ ] = sOutITEM;

						iTmpVAR = this->Add_ITEM( sOutITEM );
						if ( iTmpVAR > 0 ) {
							pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_btInvIDX = (BYTE)iTmpVAR;
							pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_ITEM	   = sOutITEM;
							btOutCNT ++;
						} else {
							this->Save_ItemToFILED( sOutITEM );	// 아이템 분리후 
						}
					}
			// }

			if ( ++nStep >= CREATE_ITEM_STEP )
				break;
			if ( 0 == PRODUCT_NEED_ITEM_NO ( nProductIDX, nStep ) )
				break;

			sOutITEM.Init( PRODUCT_NEED_ITEM_NO ( nProductIDX, nStep ), 1 );
		}

	
		g_pThreadLOG->When_CreateOrDestroyITEM ( this, pInITEM, sOutLogITEM, nOutLogCnt, NEWLOG_BREAKUP, NEWLOG_SUCCESS );
	
		// 분해된 아이템 제거...
		if ( pInITEM->IsEnableDupCNT() ) 
		{
			pInITEM->SubQuantity (1);		// 갯수 1개 제거
		}
		else
		{
			pInITEM->Clear();
		}
		pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_btInvIDX = pPacket->m_cli_CRAFT_BREAKUP_REQ.m_btTargetInvIDX;
		pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_ITEM = *pInITEM;
		btOutCNT ++;

		this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_BREAKUP_SUCCESS, btOutCNT );
	}


	return true;
}


/// 아이템 분해 요청
bool classUSER::Proc_RENEWAL_CRAFT_BREAKUP_REQ( t_PACKET *pPacket, BYTE btType )
{
	if ( pPacket->m_cli_CRAFT_BREAKUP_REQ.m_btTargetInvIDX <  MAX_EQUIP_IDX ||
		pPacket->m_cli_CRAFT_BREAKUP_REQ.m_btTargetInvIDX >= INVENTORY_SHOT_ITEM0 )
		return false;

	tagITEM *pInITEM = &this->m_Inventory.m_ItemLIST[ pPacket->m_cli_CRAFT_BREAKUP_REQ.m_btTargetInvIDX ];
	if ( 0 == pInITEM->GetHEADER() ) 
		return true;
//	선물 상자 분해 확인
#ifdef	__GIFT_BOX_ITEM
	BOOL	Temp_Gift_TF = false;
#endif

	if ( !pInITEM->IsEnableDupCNT() && pInITEM->HasSocket() && pInITEM->GetGemNO() > 300 ) 
	{

		//일단 보석이 박혀있는 아이템은 분해를 못한다..

		//if ( CRAFT_BREAKUP_USE_SKILL ==  btType)  //스킬사용으로 보석아이템 분리는막는다..
		{
			return false;
		} 

		// 보석 분리
		short ORI_QUAL = ITEM_QUALITY( pInITEM->GetTYPE(), pInITEM->GetItemNO() );
		short GEM_QUAL = ITEM_QUALITY( ITEM_TYPE_GEM,	   pInITEM->GetGemNO() );


		short nNeed = CCal::GetMONEY_WhenBreakupGEM( ORI_QUAL, GEM_QUAL );
		if ( this->GetCur_MONEY() < nNeed ) 
		{
			// 돈 없다.
			return true;
		}
		this->Sub_CurMONEY( nNeed );
		

		classPACKET *pCPacket = this->Init_gsv_CRAFT_ITEM_REPLY ();
		if ( !pCPacket )
			return false;

		BYTE btResult;
		tagITEM sOutITEM;
		sOutITEM.Clear ();

		if ( RANDOM(100)+1 + GEM_QUAL/6 <= 30 )  // 등급 감소
		{	
			if ( GEM_QUAL <= 35 ) {					// 보석 삭제
				// 로그:: 분해시 보석 삭제 됐음...
				g_pThreadLOG->When_GemmingITEM( this, pInITEM, NULL, NEWLOG_UNGEMMING, NEWLOG_FAILED );

				// 장비 아이템...
				pInITEM->m_nGEM_OP = 0;				// 장비에서 보석 삭제
				pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_btInvIDX =  pPacket->m_cli_CRAFT_BREAKUP_REQ.m_btTargetInvIDX;
				pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_ITEM		= *pInITEM;
				this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFT_BREAKUP_CLEARED_GEM, 1 );
				return true;
			}

			// 보석 아이템 생성...
			sOutITEM.m_nItemNo = pInITEM->GetGemNO() - 1;

			btResult = CRAFT_BREAKUP_DEGRADE_GEM;
		} 
		else
		{
			sOutITEM.m_nItemNo = pInITEM->GetGemNO();
			btResult = CRAFT_BREAKUP_SUCCESS_GEM;
		}
		sOutITEM.m_cType = ITEM_TYPE_GEM;
		sOutITEM.m_uiQuantity = 1;

		g_pThreadLOG->When_GemmingITEM( this, pInITEM, &sOutITEM, NEWLOG_UNGEMMING, NEWLOG_SUCCESS );

		pInITEM->m_nGEM_OP = 0;				// 장비에서 보석 삭제
		pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_btInvIDX =  pPacket->m_cli_CRAFT_BREAKUP_REQ.m_btTargetInvIDX;
		pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_ITEM		= *pInITEM;

		// 보석 분리 됐음.
		short nInvIDX = this->Add_ITEM( sOutITEM );
		if ( nInvIDX > 0 )
		{
			//pInITEM->m_nGEM_OP = 0;				// 장비에서 보석 삭제
			//pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_btInvIDX =  pPacket->m_cli_CRAFT_BREAKUP_REQ.m_btTargetInvIDX;
			//pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_ITEM		= *pInITEM;

			pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 1 ].m_btInvIDX = (BYTE)nInvIDX;
			pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 1 ].m_ITEM		= sOutITEM;

			this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, btResult, 2 );
		}
		else
		{
			this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, btResult, 1 );
			this->Save_ItemToFILED( sOutITEM );	// 보석 분리후 인벤토리 모자람...
		}
		return true;
	}
	else 
	{

		short ORI_QUAL		= ITEM_QUALITY	  ( pInITEM->GetTYPE(), pInITEM->GetItemNO() );
		int iTmpVAR =0;
	
		WORD wItemType =  pInITEM->GetTYPE();
		int nBreakIdx;

		classPACKET *pCPacket = this->Init_gsv_CRAFT_ITEM_REPLY ();
		if ( !pCPacket )
			return false;


		//	보석 분해 추가
#ifdef __GEMITEM_BREAK
		// 아이템 유형이 ITEM_TYPE_FACE_ITEM 에서 서브 워폰까지 한다. ITEM_TYPE_GEM 허용
		if((wItemType>=ITEM_TYPE_FACE_ITEM&&wItemType<=ITEM_TYPE_SUBWPN&&wItemType!=ITEM_TYPE_JEWEL)||(wItemType == ITEM_TYPE_GEM))
#else
		// 아이템 유형이 ITEM_TYPE_FACE_ITEM 에서 서브 워폰까지 이고, 보석은 제회한다. 즉, 1,2,3,4,5,6,8,9 번만 허용
		if(wItemType>=ITEM_TYPE_FACE_ITEM&&wItemType<=ITEM_TYPE_SUBWPN&&wItemType!=ITEM_TYPE_JEWEL)
#endif
		{
			//	STB에 있는 분해 번호(47번 컬럼)의 값을 읽어 온다.
			nBreakIdx = ITEM_BREAK_IDX(wItemType,pInITEM->GetItemNO());    //일반아이템 분해 STB 인덱스

			//	분해 번호가 없은 경우 오류 처리 한다.
			if(!nBreakIdx)
			{
				this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_INVALID_ITEM, 0 );
				return true;  //분해인덱스가 없다
			}

		}
		else if(wItemType ==ITEM_TYPE_RIDE_PART)
		{
			nBreakIdx = PAT_ITEM_BREAK_IDX(pInITEM->GetItemNO());  //펫아이템 분해 STB 인덱스

			if(!nBreakIdx)  
			{
				this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_INVALID_ITEM, 0 );
				return true;  //분해인덱스가 없다
			}

		}
		//	선물 박스 아이템 사용 처리 LIST_USEITEM.STB 에서 값을 얻는다.
#ifdef	__GIFT_BOX_ITEM
		else if(wItemType == ITEM_TYPE_USE)
		{
			//	사용 아이템중 선물상자 아이템인 경우
			if( USE_ITEM_GIFT_BOX_ITEM == ITEM_TYPE( ITEM_TYPE_USE, pInITEM->m_nItemNo))
			{
				//	인밴토리 빈공간 확인이 필요하다.
				//	분해 인덱스 번호 얻음.
				nBreakIdx = ItemSTBGetData(ITEM_TYPE_USE, pInITEM->m_nItemNo,20);
				if(!nBreakIdx)  
				{
					this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_INVALID_ITEM, 0 );
					return true;  //분해인덱스가 없다
				}
				//	선물상자 분해처리
				Temp_Gift_TF = true;
			}
			else
			{
				return true;  //분해할수없는 아이템
			}
		}
#endif
		else
		{
			return true;  //분해할수없는 아이템
		}

		//	분해에 의한 얻는 아이템의 개수 얻기
		int nItemEntCnt = BREAK_ENTRY_ITEM_CNT(nBreakIdx);  //등록된 아이템수

		//	개수가 없는 경우
		if(!nItemEntCnt)
		{
			this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_INVALID_ITEM, 0 );
			return true;  //분해인덱스가 없다
		}

		int nBreakCnt = BREAK_MAX_CNT(nBreakIdx);  //분해최대횟수

		//	분해 회수가 0거나 최대 회수 4보다 큰경우 무시
		if(!nBreakCnt||nBreakCnt>DEF_MAX_BREAK_CNT)
		{
			this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_INVALID_ITEM, 0 );
			return true;  //분해인덱스가 없다
		}

		if ( CRAFT_BREAKUP_USE_SKILL==btType )
		{

			iTmpVAR = CCal::GetMP_WhenBreakupMAT( ORI_QUAL );

#ifdef __KRRE_NEW_AVATAR_DB
			iTmpVAR =0;
#endif

			if ( this->Get_MP() < iTmpVAR ) 
			{
				// 엠 없다
				this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_NOT_ENOUGH_MP, 0 );
				return true;  //분해인덱스가 없다
			}
			this->Sub_MP( iTmpVAR );
		} 
		else
		{
			iTmpVAR = CCal::GetMONEY_WhenBreakupMAT( ORI_QUAL );
			if ( this->GetCur_MONEY() < iTmpVAR ) 
			{
				// 돈 없다.
				this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_NOT_ENOUGH_MONEY, 0 );
				return true;
			}
			this->Sub_CurMONEY( iTmpVAR );
		}

		tagITEM sOutITEM;
		int nRand;
		int i=0,j=0,k=0;
		int btOutCNT =0;

		tagITEM sOutLogITEM[ CREATE_ITEM_STEP ];
		WORD wBreakSucSlotNo[DEF_MAX_BREAK_CNT];
		int nBreakSucCnt =0;


		do    //랜덤 횟수 루프
		{
			nRand = (rand()*rand())%100000;
			nRand +=(pInITEM->GetGrade()*1000);           //아이템등급에따른 분해 확률을 구한다..

			if(nRand>100000)
				nRand = 100000;

			 j=0;
			int nPreBreakPer=0,nBreakPer;  //누적확률저장..

			do                         //아이템 랜덤 구간 루프
			{
				nBreakPer=nPreBreakPer + BREAK_ITEM_PER(nBreakIdx,j);  //바로전 확률과 더한다..
			
				if(nRand>nPreBreakPer&&nRand<= nBreakPer)   //확률 구간이 맞아 떨어지는지 검사
				{

					BOOL bBreakSuc = TRUE;

					if(nBreakSucCnt)															//분해성공한적이 있는지 검사
					{
						 k=0;
						do
						{
							if(wBreakSucSlotNo[k] == j)                            //분해한 아이템여부 검사.
							{
								bBreakSuc = FALSE;
								break;
							}

							k+=1;
						} while(k<nBreakSucCnt);

					}


					if(!bBreakSuc)                                                      //전에 분해한아이템이랑 동일하다..
						break;

					sOutITEM.Clear();
					sOutITEM.m_cType     = ITEM_TYPE_NATURAL;	// 이건 도대체 왜 넣은거야?
					sOutITEM.Init( BREAK_ITEM_IDX(nBreakIdx,j),RANDOM(BREAK_ITEM_CNT(nBreakIdx,j))+1);

//-------------------------------------
#ifdef __GIFT_BOX_ITEM_OPTION
//2007.02.26/김대성/추가 - 선물상자 분해시 아이템 옵션 붙이기
					if( USE_ITEM_GIFT_BOX_ITEM == ITEM_TYPE( ITEM_TYPE_USE, pInITEM->m_nItemNo))	// 선물상자면
					{
						if(ITEM_TYPE_FACE_ITEM <= sOutITEM.m_cType && sOutITEM.m_cType <= ITEM_TYPE_SUBWPN)	// 장비아이템이면
						{
							if((RANDOM(100)+1) <= ItemSTBGetData(ITEM_TYPE_USE, pInITEM->m_nItemNo,8))	// 지정한 확률안에 포함되면
							{
								int iOption = 0;
								if(ItemSTBGetData(ITEM_TYPE_USE, pInITEM->m_nItemNo,19) == 0)	// 0번 계산식
								{
									iOption = RANDOM(100);
									if ( iOption <= 35 ) 
										iOption = 1 + RANDOM(7-1);		// 1-6
									else if ( iOption <= 70 )
										iOption = 7 + RANDOM(49-7);		// 7-48
									else if ( iOption <= 85 )
										iOption = 49 + RANDOM(65-49);	// 49-64
									else if ( iOption <= 95 ) 
										iOption = 65 + RANDOM(73-65);	// 65-72
									else
										iOption = 73 + RANDOM(81-73);	// 73-80
								}
								else	// 1번 계산식
								{
									iOption = 1 + RANDOM(300);			// 1-300
								}
								sOutITEM.m_nGEM_OP = iOption;
								sOutITEM.m_bIsAppraisal = 1;
							}
						}
					}
#endif
//-------------------------------------

					iTmpVAR = this->Add_ITEM( sOutITEM );
					if ( iTmpVAR > 0 )
					{
						pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_btInvIDX = (BYTE)iTmpVAR;
						pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_ITEM	   = sOutITEM;
						btOutCNT ++;
					} 
					else
					{
						this->Save_ItemToFILED( sOutITEM );	// 아이템 분리후 
					}

					sOutLogITEM[nBreakSucCnt] = sOutITEM; //로그에 쓸데이터복사...

					wBreakSucSlotNo[nBreakSucCnt] = j;  //분해성공한 슬롯번호..
					nBreakSucCnt+=1;				
					j+=1;
					break;
				}
				else if(nRand<nPreBreakPer )
				{
					break;
				}

				nPreBreakPer=nBreakPer;  //바로전 확률을 저장한다..

				j+=1;
			} while(j<nItemEntCnt);     //아이템 랜덤 구간 루프

			i+=1;
		} while(i<nBreakCnt);   //랜덤 횟수 루프


		g_pThreadLOG->When_CreateOrDestroyITEM ( this, pInITEM, sOutLogITEM, nBreakSucCnt, NEWLOG_BREAKUP, NEWLOG_SUCCESS );

		// 분해된 아이템 제거...
		if ( pInITEM->IsEnableDupCNT() )
		{
			pInITEM->SubQuantity (1);		// 갯수 1개 제거
		}
		else
		{
			pInITEM->Clear();
		}
		pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_btInvIDX = pPacket->m_cli_CRAFT_BREAKUP_REQ.m_btTargetInvIDX;
		pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_ITEM = *pInITEM;
		btOutCNT ++;

//	선물 상자 분해 확인
#ifdef	__GIFT_BOX_ITEM
		if(Temp_Gift_TF)
			this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_GIFTBOX_SUCCESS, btOutCNT );
		else
			this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_BREAKUP_SUCCESS, btOutCNT );
#else
		this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_BREAKUP_SUCCESS, btOutCNT );
#endif
	}
	return true;
}


//-------------------------------------
// 2007.02.06/김대성/추가 - 선물상자 드랍확률 테스트
int classUSER::Proc_RENEWAL_CRAFT_BREAKUP_DropTest( int nItemNo, tagITEM *pITEM )
{
	tagITEM item;
	tagITEM *pInITEM = &item;
	pInITEM->Clear();
	pInITEM->m_cType = 10;
	pInITEM->m_nItemNo = nItemNo;

	BOOL Temp_Gift_TF = false;
	int btOutCNT =0;

	if ( !pInITEM->IsEnableDupCNT() && pInITEM->HasSocket() && pInITEM->GetGemNO() > 300 ) 
		return -1;
	else 
	{
		short ORI_QUAL = ITEM_QUALITY( pInITEM->GetTYPE(), pInITEM->GetItemNO() );
		int iTmpVAR =0;

		WORD wItemType =  pInITEM->GetTYPE();
		int nBreakIdx;

		//	사용 아이템중 선물상자 아이템인 경우
		if( USE_ITEM_GIFT_BOX_ITEM == ITEM_TYPE( ITEM_TYPE_USE, pInITEM->m_nItemNo))
		{
			//	분해 인덱스 번호 얻음.
			nBreakIdx = ItemSTBGetData(ITEM_TYPE_USE, pInITEM->m_nItemNo,20);
			if(!nBreakIdx)  
				return -1;  //분해인덱스가 없다

			//	선물상자 분해처리
			Temp_Gift_TF = true;
		}
		else
			return -1;  //분해할수없는 아이템

		//	분해에 의한 얻는 아이템의 개수 얻기
		int nItemEntCnt = BREAK_ENTRY_ITEM_CNT(nBreakIdx);  //등록된 아이템수

		//	개수가 없는 경우
		if(!nItemEntCnt)
			return -1;  //분해인덱스가 없다

		int nBreakCnt = BREAK_MAX_CNT(nBreakIdx);  //분해최대횟수

		//	분해 회수가 0거나 최대 회수 4보다 큰경우 무시
		if(!nBreakCnt||nBreakCnt>DEF_MAX_BREAK_CNT)
			return -1;  //분해인덱스가 없다

		tagITEM sOutITEM;
		int nRand;
		int i=0,j=0,k=0;
		btOutCNT =0;

		WORD wBreakSucSlotNo[DEF_MAX_BREAK_CNT];
		int nBreakSucCnt =0;

		do    //랜덤 횟수 루프
		{
			nRand = (rand()*rand())%100000;
			nRand +=(pInITEM->GetGrade()*1000);           //아이템등급에따른 분해 확률을 구한다..

			if(nRand>100000)
				nRand = 100000;

			j=0;
			int nPreBreakPer=0,nBreakPer;  //누적확률저장..

			do                         //아이템 랜덤 구간 루프
			{
				nBreakPer=nPreBreakPer + BREAK_ITEM_PER(nBreakIdx,j);  //바로전 확률과 더한다..

				if(nRand>nPreBreakPer&&nRand<= nBreakPer)   //확률 구간이 맞아 떨어지는지 검사
				{

					BOOL bBreakSuc = TRUE;

					if(nBreakSucCnt)		//분해성공한적이 있는지 검사
					{
						k=0;
						do
						{
							if(wBreakSucSlotNo[k] == j)		//분해한 아이템여부 검사.
							{
								bBreakSuc = FALSE;
								break;
							}

							k+=1;
						} while(k<nBreakSucCnt);
					}

					if(!bBreakSuc)		//전에 분해한아이템이랑 동일하다..
						break;

					sOutITEM.Clear();
					sOutITEM.m_cType     = ITEM_TYPE_NATURAL;
					sOutITEM.Init( BREAK_ITEM_IDX(nBreakIdx,j),RANDOM(BREAK_ITEM_CNT(nBreakIdx,j))+1);

//-------------------------------------
#ifdef __GIFT_BOX_ITEM_OPTION
//2007.02.26/김대성/추가 - 선물상자 분해시 아이템 옵션 붙이기 - 테스트
					if( USE_ITEM_GIFT_BOX_ITEM == ITEM_TYPE( ITEM_TYPE_USE, pInITEM->m_nItemNo))	// 선물상자면
					{
						if(ITEM_TYPE_FACE_ITEM <= sOutITEM.m_cType && sOutITEM.m_cType <= ITEM_TYPE_SUBWPN)	// 장비아이템이면
						{
							if((RANDOM(100)+1) <= ItemSTBGetData(ITEM_TYPE_USE, pInITEM->m_nItemNo,8))	// 지정한 확률안에 포함되면
							{
								int iOption = 0;
								if(ItemSTBGetData(ITEM_TYPE_USE, pInITEM->m_nItemNo,19) == 0)	// 0번 계산식
								{
									iOption = RANDOM(100);
									if ( iOption <= 35 ) 
										iOption = 1 + RANDOM(7-1);		// 1-6
									else if ( iOption <= 70 )
										iOption = 7 + RANDOM(49-7);		// 7-48
									else if ( iOption <= 85 )
										iOption = 49 + RANDOM(65-49);	// 49-64
									else if ( iOption <= 95 ) 
										iOption = 65 + RANDOM(73-65);	// 65-72
									else
										iOption = 73 + RANDOM(81-73);	// 73-80
								}
								else	// 1번 계산식
								{
									iOption = 1 + RANDOM(300);			// 1-300
								}
								sOutITEM.m_nGEM_OP = iOption;
								sOutITEM.m_bIsAppraisal = 1;
							}
						}
					}
#endif
//-------------------------------------

					pITEM[btOutCNT] = sOutITEM;
					btOutCNT ++;

					wBreakSucSlotNo[nBreakSucCnt] = j;  //분해성공한 슬롯번호..
					nBreakSucCnt+=1;				
					j+=1;
					break;
				}
				else if(nRand<nPreBreakPer )
				{
					break;
				}

				nPreBreakPer=nBreakPer;  //바로전 확률을 저장한다..

				j+=1;
			} while(j<nItemEntCnt);     //아이템 랜덤 구간 루프

			i+=1;
		} while(i<nBreakCnt);   //랜덤 횟수 루프
	}

	return btOutCNT;
}
//-------------------------------------

 bool classUSER::Proc_CRAFT_DRILL_SOCKET(t_PACKET *pPacket,BYTE btType)
 {
	 int  nInx = pPacket->m_cli_CRAFT_DRILL_SOKCET_REQ.m_btTargetInvIDX;
	 if ( pPacket->m_cli_CRAFT_DRILL_SOKCET_REQ.m_btTargetInvIDX <  MAX_EQUIP_IDX ||
		 pPacket->m_cli_CRAFT_DRILL_SOKCET_REQ.m_btTargetInvIDX >= INVENTORY_SHOT_ITEM0 )
		 return false;

	 	classPACKET *pCPacket = this->Init_gsv_CRAFT_ITEM_REPLY ();

	 if(btType == CRAFT_DRILL_SOCKET_FROM_ITEM)		//아이템을 사용해서 소켓을 만들경우..
	 {

		 if ( pPacket->m_cli_CRAFT_DRILL_SOKCET_REQ.m_nUseInvSLOTorNpcIDX <  MAX_EQUIP_IDX ||
			 pPacket->m_cli_CRAFT_DRILL_SOKCET_REQ.m_nUseInvSLOTorNpcIDX >= INVENTORY_SHOT_ITEM0 )
			 return false;

			 tagITEM  *pUseItem = &m_Inventory.m_ItemLIST[pPacket->m_cli_CRAFT_DRILL_SOKCET_REQ.m_nUseInvSLOTorNpcIDX];

			 if ( 318 != ITEM_TYPE( ITEM_TYPE_USE, pUseItem->GetItemNO() ) )  //드릴 아이템 판단 여부..
			 {
				 this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_DRILL_INVALID_USEITEM,0);  //드릴 아이템이 아니다.
				 return true;
			 }

			 tagITEM  *pTargetItem = &m_Inventory.m_ItemLIST[pPacket->m_cli_CRAFT_DRILL_SOKCET_REQ.m_btTargetInvIDX];

			 if(pTargetItem->GetTYPE() != ITEM_TYPE_WEAPON&&pTargetItem->GetTYPE() != ITEM_TYPE_ARMOR&&pTargetItem->GetTYPE() !=ITEM_TYPE_JEWEL )
			 {
				 this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_DRILL_INVALID_USEITEM, 0);  //드릴 아이템이 아니다.
				 return true;

			 }

			 if(pTargetItem->HasSocket())
			 {
				 this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFT_DRILL_EXIST_SOCKET, 0);  //드릴 아이템이 아니다.
				 return true;
			 }

			 if(RANDOM(100)>=USEITEM_ALLPY_VALUE(pUseItem->GetItemNO()))
			 {

				 pUseItem->SubQuantity( 1 );

				 pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_btInvIDX = pPacket->m_cli_CRAFT_DRILL_SOKCET_REQ.m_nUseInvSLOTorNpcIDX;
				 pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_ITEM		= *pUseItem;


				 this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket,CRAFT_DRILL_CLEAR_USEITME ,1);  //드릴아이템만 사용되고 소켓은 안뚫어졌다.. ㅠ.ㅠ

				 return true;
			 }
			 else
			 {
				 pUseItem->SubQuantity( 1 );

				 pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_btInvIDX = pPacket->m_cli_CRAFT_DRILL_SOKCET_REQ.m_nUseInvSLOTorNpcIDX;
				 pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 0 ].m_ITEM		= *pUseItem;


				 pTargetItem->m_bHasSocket = 1;     //소켓 생성..
				 pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 1 ].m_btInvIDX = pPacket->m_cli_CRAFT_DRILL_SOKCET_REQ.m_btTargetInvIDX;
				 pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ 1 ].m_ITEM		= *pTargetItem;

				 this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket,CRAFT_DRILL_SOCKET_SUCCESS ,2);  //소켓 생성성공..
				 return true;
			 }
	 }
	 else
	 {

	 }

	 return true;

 }

/// 아이템 제련 요청
bool classUSER::Proc_CRAFT_UPGRADE_REQ( t_PACKET *pPacket, bool bUseMP )
{
	if ( pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btTargetInvIDX <  MAX_EQUIP_IDX ||
		 pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btTargetInvIDX >= INVENTORY_SHOT_ITEM0 )
		return false;

	tagITEM *pInITEM = &this->m_Inventory.m_ItemLIST[ pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btTargetInvIDX ];
	if ( !pInITEM->IsEquipITEM() )
		return true;
	if ( pInITEM->GetGrade() >= 9 )		// 더이상 재련 불가..
		return true;

	short nProductIDX;
	if ( ITEM_TYPE_WEAPON == pInITEM->GetTYPE() )
		nProductIDX = pInITEM->GetGrade() + 1;
	else
		nProductIDX = pInITEM->GetGrade() + 11;

	if ( pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btUseItemINV[ 0 ] <  INVENTORY_ETC_ITEM0 ||
		 pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btUseItemINV[ 0 ] >= INVENTORY_ETC_ITEM0 + INVENTORY_PAGE_SIZE ) {
		return false;
	}

	tagITEM *pMatITEM = &this->m_Inventory.m_ItemLIST[ pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btUseItemINV[ 0 ] ];
	if ( PRODUCT_RAW_MATERIAL(nProductIDX) != ITEM_TYPE(pMatITEM->GetTYPE(), pMatITEM->GetItemNO() ) ) {
		return true;
	}
	short MAT_QUAL  = ITEM_QUALITY( pMatITEM->GetTYPE(), pMatITEM->GetItemNO() );
	short ITEM_QUAL = ITEM_QUALITY( pInITEM->GetTYPE(), pInITEM->GetItemNO() );
	short nStep;
	if ( bUseMP ) {
		nStep = CCal::GetMP_WhenUpgradeEQUIP( pInITEM->GetGrade(), ITEM_QUAL );
		if ( this->Get_MP() < nStep ) {
			// 엠 없다
			return true;
		}
		this->Sub_MP( nStep );
	} else {
		nStep = CCal::GetMONEY_WhenUpgradeEQUIP( pInITEM->GetGrade(), ITEM_QUAL );
		if ( this->GetCur_MONEY() < nStep ) {
			// 돈 없다.
			return true;
		}
		this->Sub_CurMONEY( nStep );
	}

	classPACKET *pCPacket = this->Init_gsv_CRAFT_ITEM_REPLY ();
	if ( !pCPacket )
		return false;

	BYTE btOutCNT=0;
	tagITEM sNeedITEM;
	nStep = 0;
	while( true ) {
		// 갯수만큼 소모...
		if ( !pMatITEM->SubQuantity( PRODUCT_NEED_ITEM_CNT( nProductIDX, nStep ) ) ) {
			return this->Send_gsv_CRAFT_ITEM_RESULT( CRAFT_UPGRADE_INVALID_MAT );
		}
		pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_btInvIDX = pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btUseItemINV[ nStep ];
		pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_ITEM = *pMatITEM;
		btOutCNT ++;

		if ( ++nStep >= UPGRADE_ITEM_STEP )
			break;
		if ( 0 == PRODUCT_NEED_ITEM_NO(nProductIDX, nStep) )
			break;

		if ( pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btUseItemINV[ nStep ] <  INVENTORY_ETC_ITEM0 ||
			 pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btUseItemINV[ nStep ] >= INVENTORY_ETC_ITEM0 + INVENTORY_PAGE_SIZE ) {
			return false;
		}

		pMatITEM = &this->m_Inventory.m_ItemLIST[ pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btUseItemINV[ nStep ] ];
		sNeedITEM.Init( PRODUCT_NEED_ITEM_NO( nProductIDX, nStep ) );
		if ( pMatITEM->GetHEADER() != sNeedITEM.GetHEADER() ) {
			return this->Send_gsv_CRAFT_ITEM_RESULT( CRAFT_UPGRADE_INVALID_MAT );
		}
	}

	// 재련 시작을 주변에 통보...
	this->Send_gsv_ITEM_RESULT_REPORT( REPORT_ITEM_UPGRADE_START, static_cast<BYTE>( pInITEM->GetTYPE() ), pInITEM->GetItemNO() );

	short SUC;
	BYTE btResult, btBeforeGrade;
	btBeforeGrade = pInITEM->m_cGrade;
	// 성공률 계산
	SUC = ( ( (pInITEM->GetGrade()+2) * (pInITEM->GetGrade()+3) * ( pInITEM->GetGrade()*5 + ITEM_QUAL*3 + 250 )
		* ( 61+RANDOM(100) ) * 320 ) / ( MAT_QUAL * (pInITEM->GetDurability()+180) * ( Get_WorldPROD()+10 ) ) ) + 200;
	if ( SUC < 1000 ) {	// 성공
		SUC = ( 200+(MAT_QUAL+5)*10 + (1+RANDOM(100))*3 - ( pInITEM->GetGrade()+6)*80 ) / 40;
		if ( SUC > 0 ) {
			if ( pInITEM->m_cDurability + SUC > 120 )
				pInITEM->m_cDurability = 120;
			else
				pInITEM->m_cDurability += SUC;
		}

		btResult = CRAFT_UPGRADE_SUCCESS;
		pInITEM->m_cGrade ++;

		g_pThreadLOG->When_UpgradeITEM( this, pInITEM, btBeforeGrade, bUseMP ? NEWLOG_UPGRADE_SUC_WITH_SKILL : NEWLOG_UPGRADE_SUC_FROM_NPC );
	} else {			// 실패 시 등급 감소
		// 내구도 변화.
		SUC = pInITEM->m_cDurability + ( 200+(MAT_QUAL+5)*10 + (1+RANDOM(100))*3 - ( pInITEM->GetGrade()+6)*80 ) / 40;
		if ( SUC > 120 )
			pInITEM->m_cDurability = 120;
		else
		if ( SUC < 0 )
			pInITEM->m_cDurability = 0;
		else
			pInITEM->m_cDurability = SUC;

		btResult = CRAFT_UPGRADE_FAILED;
		nStep = (short)( ( pInITEM->GetGrade()+1 ) * ( pInITEM->GetGrade()+10 ) / (41+RANDOM(100)) );
		if ( nStep > pInITEM->GetGrade() )
			pInITEM->m_cGrade = 0;
		else
			pInITEM->m_cGrade -= nStep;

		g_pThreadLOG->When_UpgradeITEM( this, pInITEM, btBeforeGrade, NEWLOG_UPGRADE_FAILED );
	}

	pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_btInvIDX	=  pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btTargetInvIDX;
	pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_ITEM		= *pInITEM;
	btOutCNT ++;

	pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_btInvIDX	=  0;
	pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_ITEM.m_uiQuantity = SUC;
	btOutCNT ++;

	this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, btResult, btOutCNT );

	//if ( 장착중인 장비는 업글 안됨...
	//	this->UpdateAbility ();		// upgrade...
	//	this->Send_gsv_EQUIP_ITEM( pPacket->m_cli_APPRAISAL_REQ.m_wInventoryIndex, pITEM );
	//}

	return true;
}

/// Item smelting request - Korea Renewal Version
bool classUSER::Proc_RENEWAL_CRAFT_UPGRADE_REQ( t_PACKET *pPacket, bool bUseMP )
{
	if ( pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btTargetInvIDX <  MAX_EQUIP_IDX ||
		 pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btTargetInvIDX >= INVENTORY_SHOT_ITEM0 )
		return false;

	classPACKET *pCPacket = this->Init_gsv_CRAFT_ITEM_REPLY ();


	tagITEM *pInITEM = &this->m_Inventory.m_ItemLIST[ pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btTargetInvIDX ];
	if ( !pInITEM->IsEquipITEM() )
	{
		this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_INVALID_ITEM, 0 );
		return true;
	}

	if ( pInITEM->GetGrade() >= 9 )		// 더이상 재련 불가..
	{
		this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_INVALID_ITEM, 0 );
		return true;
	}

	BYTE btBeforeGrade =  pInITEM->GetGrade();

	// short MAT_QUAL  = ITEM_QUALITY( pMatITEM->GetTYPE(), pMatITEM->GetItemNO() );
	short ITEM_QUAL = ITEM_QUALITY( pInITEM->GetTYPE(), pInITEM->GetItemNO() );
	if ( bUseMP )
	{
		short nNeedMP = CCal::GetMP_WhenUpgradeEQUIP( pInITEM->GetGrade(), ITEM_QUAL );

		if ( this->Get_MP() < nNeedMP )
		{
			// 엠 없다

			this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_NOT_ENOUGH_MP, 0 );
			return true;
		}
		this->Sub_MP( nNeedMP );
	} 
	else
	{
		short nNeedMONEY = CCal::GetMONEY_WhenUpgradeEQUIP( pInITEM->GetGrade(), ITEM_QUAL );
		if ( this->GetCur_MONEY() < nNeedMONEY ) 
		{
			// 돈 없다.
			return true;
			this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_NOT_ENOUGH_MONEY, 0 );
		}
		this->Sub_CurMONEY( nNeedMONEY );
	}

	// 재료 아이템 체크
	short nStep = 0;

	short nUgradeIDX = ITEM_UPGRADE_NO( pInITEM->GetTYPE(), pInITEM->GetItemNO() );
	short nNeedItemNO = PRODUCT_NEED_ITEM_NO( nUgradeIDX, nStep );

	if(nNeedItemNO<1)
	{
		this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, CRAFE_INVALID_ITEM, 0 );
		return true;
	}

	while( nNeedItemNO > 0 )
	{
		tagITEM *pMatITEM = &this->m_Inventory.m_ItemLIST[ pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btUseItemINV[ nStep ] ];
		if( pMatITEM->GetHEADER() <= 0 )
			return Send_gsv_CRAFT_ITEM_RESULT( CRAFT_UPGRADE_INVALID_MAT );

//-------------------------------------
/*
		bool bInvalidMatItem =	pMatITEM->GetTYPE() != nNeedItemNO / 1000 || 
								pMatITEM->GetItemNO() != nNeedItemNO % 1000 || 
								pMatITEM->GetQuantity() < PRODUCT_NEED_ITEM_CNT( nUgradeIDX, nStep );
*/
// 2006.07.19/김대성/수정 - 아이템 999 제한 확장

		bool bInvalidMatItem;
		if(nNeedItemNO < 1000000)
		{
			bInvalidMatItem =	pMatITEM->GetTYPE() != nNeedItemNO / 1000 || 
				pMatITEM->GetItemNO() != nNeedItemNO % 1000 || 
				pMatITEM->GetQuantity() < PRODUCT_NEED_ITEM_CNT( nUgradeIDX, nStep );
		}
		else
		{
			bInvalidMatItem =	pMatITEM->GetTYPE() != nNeedItemNO / 1000000 || 
				pMatITEM->GetItemNO() != nNeedItemNO % 1000000 || 
				pMatITEM->GetQuantity() < PRODUCT_NEED_ITEM_CNT( nUgradeIDX, nStep );
		}
//-------------------------------------
								
		if( bInvalidMatItem )
			return Send_gsv_CRAFT_ITEM_RESULT( CRAFT_UPGRADE_INVALID_MAT );

		if ( ++nStep >= UPGRADE_ITEM_STEP )
			break;

		nNeedItemNO = PRODUCT_NEED_ITEM_NO( nUgradeIDX, nStep );
	}

	if ( !pCPacket )
		return false;

	// 재료 아이템 빼기
	BYTE btOutCNT = 0;
	for( short nI = 0; nI < nStep; nI++ )
	{
		tagITEM *pMatITEM = &m_Inventory.m_ItemLIST[ pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btUseItemINV[ nI ] ];
		pMatITEM->SubQuantity( PRODUCT_NEED_ITEM_CNT( nUgradeIDX, nI ) );

		pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_btInvIDX = pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btUseItemINV[ nI ];
		pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_ITEM = *pMatITEM;
		btOutCNT++;
	}

	// Determine the success or failure
	short nSucPROB = 0;
	if( pInITEM->GetTYPE() == ITEM_TYPE_WEAPON )
		nSucPROB = UPGRADE_WEAPON_SUCPOS( pInITEM->GetGrade() + 1 );
	else
		nSucPROB = UPGRADE_SUBWPN_SUCPOS( pInITEM->GetGrade() + 1 );
	short nRAN = (short)(rand() % 101);
	
	BYTE btResult = CRAFT_UPGRADE_FAILED;
	if( nRAN <= nSucPROB)
	{
		g_pThreadLOG->When_UpgradeITEM( this, pInITEM, btBeforeGrade, bUseMP ? NEWLOG_UPGRADE_SUC_WITH_SKILL : NEWLOG_UPGRADE_SUC_FROM_NPC );
		pInITEM->m_cGrade++;
		btResult = CRAFT_UPGRADE_SUCCESS;
	}
	else
	{
//-------------------------------------
#ifndef __CRAFT_UPGRADE_4_5
		g_pThreadLOG->When_UpgradeITEM( this, pInITEM, btBeforeGrade,NEWLOG_UPGRADE_FAILED );
		pInITEM->Clear();
#else
// 2007.03.06/김대성/수정 - 4,5단계의 제련실패시 아이템이 사라지지 않고 1~3단계를 랜덤하게 등급을 떨어뜨린다.
		if(ItemSTBGetData(pInITEM->m_cType, pInITEM->m_nItemNo, 50) == 31)	// 마일리지 아이템이면
		{
			pInITEM->m_cGrade = pInITEM->m_cGrade - (RANDOM(3) + 1);	// 1 ~ 3 단계를 떨어뜨린다.
			if(pInITEM->m_cGrade < 0) 
				pInITEM->m_cGrade = 0;
			g_pThreadLOG->When_UpgradeITEM( this, pInITEM, btBeforeGrade,NEWLOG_UPGRADE_FAILED3 );
		}
		else	// If a common item
		{
			if(pInITEM->m_cGrade == 3 || pInITEM->m_cGrade == 4)	// 3단계, 4단계에서 제련하면 등급이 떨어진다.
			{
				pInITEM->m_cGrade = pInITEM->m_cGrade - (RANDOM(3) + 1);	// 1 ~ 3 단계를 떨어뜨린다.
				if(pInITEM->m_cGrade < 0) 
					pInITEM->m_cGrade = 0;
				g_pThreadLOG->When_UpgradeITEM( this, pInITEM, btBeforeGrade,NEWLOG_UPGRADE_FAILED2 );
			}
			else	// Step 5 fails, the item disappears from above.
			{
				g_pThreadLOG->When_UpgradeITEM( this, pInITEM, btBeforeGrade,NEWLOG_UPGRADE_FAILED );
				pInITEM->Clear();	
			}
		}
#endif
//-------------------------------------
	}
	
	pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_btInvIDX = pPacket->m_cli_CRAFT_UPGRADE_REQ.m_btTargetInvIDX;
	pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_ITEM = *pInITEM;
	btOutCNT++;

	pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_btInvIDX = 0;
	pCPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ btOutCNT ].m_ITEM.m_uiQuantity = nRAN;
	btOutCNT++;

	this->Send_gsv_CRAFT_ITEM_REPLY( pCPacket, btResult, btOutCNT );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// Item Jamming / degradation / jaeryeon related requests
bool classUSER::Recv_cli_CRAFT_ITEM_REQ( t_PACKET *pPacket )
{
	switch( pPacket->m_cli_CRAFT_ITEM_REQ.m_btTYPE ) {
		case CRAFT_GEMMING_REQ		:
			return Proc_CRAFT_GEMMING_REQ( pPacket );
		case CRAFT_BREAKUP_USE_SKILL:
		{
			short nSkillIDX = this->m_Skills.m_nSkillINDEX[ pPacket->m_cli_CRAFT_BREAKUP_REQ.m_nSkillSLOTorNpcIDX ];
			if ( 2 != SKILL_TYPE( nSkillIDX ) || 41 != SKILL_ITEM_MAKE_NUM( nSkillIDX ) )
				return false;

#ifdef __KRRE_BREAK
			return Proc_RENEWAL_CRAFT_BREAKUP_REQ(pPacket,CRAFT_BREAKUP_USE_SKILL);
#else
			return Proc_CRAFT_BREAKUP_REQ( pPacket, true );
#endif

	
		}
		case CRAFT_BREAKUP_FROM_NPC	:
		{
			CObjNPC *pCharNPC;
			pCharNPC = (CObjNPC*)g_pObjMGR->Get_GameOBJ( pPacket->m_cli_CRAFT_BREAKUP_REQ.m_nSkillSLOTorNpcIDX, OBJ_NPC );
			if ( !pCharNPC ) {	// 상점 주인이 없어???
				return false;
			}
			int iDistance = distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)pCharNPC->m_PosCUR.x, (int)pCharNPC->m_PosCUR.y);
			if ( iDistance > MAX_TRADE_DISTANCE )	{	// 상점 npc와의 거래 체크...
				return true;
			}
#ifdef __KRRE_BREAK
			return Proc_RENEWAL_CRAFT_BREAKUP_REQ(pPacket,CRAFT_BREAKUP_FROM_NPC);
#endif
			return Proc_CRAFT_BREAKUP_REQ( pPacket );
		}
		case CRAFT_UPGRADE_USE_SKILL:
		{
			short nSkillIDX = this->m_Skills.m_nSkillINDEX[ pPacket->m_cli_CRAFT_UPGRADE_REQ.m_nSkillSLOTorNpcIDX ];
			if ( 2 != SKILL_TYPE( nSkillIDX ) || 42 != SKILL_ITEM_MAKE_NUM( nSkillIDX ) )
				return false;
#ifdef __KRRE_UPGRADE
			return Proc_RENEWAL_CRAFT_UPGRADE_REQ( pPacket, true );
#else
			return Proc_CRAFT_UPGRADE_REQ( pPacket, true );
#endif
		}
		case CRAFT_UPGRADE_FROM_NPC	:
		{
			CObjNPC *pCharNPC;
			pCharNPC = (CObjNPC*)g_pObjMGR->Get_GameOBJ( pPacket->m_cli_CRAFT_UPGRADE_REQ.m_nSkillSLOTorNpcIDX, OBJ_NPC );
			if ( !pCharNPC ) {	// 상점 주인이 없어???
				return false;
			}
			int iDistance = distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)pCharNPC->m_PosCUR.x, (int)pCharNPC->m_PosCUR.y);
			if ( iDistance > MAX_TRADE_DISTANCE ) {	// 상점 npc와의 거래 체크...
				return true;
			}

#ifdef __KRRE_UPGRADE
			return Proc_RENEWAL_CRAFT_UPGRADE_REQ( pPacket, false );
#else
			return Proc_CRAFT_UPGRADE_REQ( pPacket, false );
#endif
		}

		case CRAFT_DRILL_SOCKET_FROM_ITEM:    //드릴아이템 사용으로 소켓생성..
		{
			return Proc_CRAFT_DRILL_SOCKET(pPacket,CRAFT_DRILL_SOCKET_FROM_ITEM);
		}
		case CRAFT_DRILL_SOCKET_FROM_NPC:
		{
			return Proc_CRAFT_DRILL_SOCKET(pPacket,CRAFT_DRILL_SOCKET_FROM_ITEM);
		}

	} // switch( pPacket->m_cli_CRAFT_ITEM_REQ.m_btTYPE )

	return false;
}

//-------------------------------------------------------------------------------------------------
/// 파티 룰 변경
bool classUSER::Recv_cli_PARTY_RULE( t_PACKET *pPacket )
{
	if ( this->GetPARTY() && this == this->GetPARTY()->GetPartyOWNER() ) {
		this->GetPARTY()->Set_PartyRULE( pPacket->m_cli_PARTY_RULE.m_btPartyRUEL );
	}
	return true;
}


//-------------------------------------------------------------------------------------------------
/// 모니터링 툴에 서버 버젼,시작시간 전송
bool classUSER::Recv_mon_SERVER_LIST_REQ( t_PACKET *pPacket )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

//	g_pListSERVER->Make_srv_SERVER_LIST_REPLY( pCPacket );
	pCPacket->m_HEADER.m_wType = GSV_SERVER_LIST_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( srv_SERVER_LIST_REPLY );

	pCPacket->m_srv_SERVER_LIST_REPLY.m_dwBuildNO   = GetServerBuildNO();
	pCPacket->m_srv_SERVER_LIST_REPLY.m_dwStartTIME = GetServerStartTIME();
	pCPacket->m_srv_SERVER_LIST_REPLY.m_nServerCNT  = 0;

	this->Send_Start( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	return true;
}


/// 모니터링 툴에 사용자수 전송
bool classUSER::Recv_mon_SERVER_STATUS_REQ( t_PACKET *pPacket )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = WLS_SERVER_STATUS_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( wls_SERVER_STATUS_REPLY );

	pCPacket->m_wls_SERVER_STATUS_REPLY.m_dwTIME = pPacket->m_wls_SERVER_STATUS_REPLY.m_dwTIME;
	pCPacket->m_wls_SERVER_STATUS_REPLY.m_nServerCNT = 0;//g_pZoneLIST->GetZoneCNT();
	pCPacket->m_wls_SERVER_STATUS_REPLY.m_iUserCNT = g_pUserLIST->Get_AccountCNT();

	this->Send_Start( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	return true;
}
/// 모니터링툴에서 공지 사항 받음
bool classUSER::Recv_mon_SERVER_ANNOUNCE( t_PACKET *pPacket )
{
	short nOffset = sizeof( t_PACKETHEADER );
	char *szMsg, *szID;

	szMsg = Packet_GetStringPtr( pPacket, nOffset );
	szID  = Packet_GetStringPtr( pPacket, nOffset );

	if ( szMsg )
		g_pZoneLIST->Send_gsv_ANNOUNCE_CHAT( szMsg, szID );

	return true;
}

//-------------------------------------------------------------------------------------------------
/**
 * 존 공지
 */
bool classUSER::Recv_ost_SERVER_ZONEANNOUNCE( t_PACKET *pPacket )
{
	if( !pPacket )
		return true;
	short nOffset = sizeof( ost_SERVER_ZONEANNOUNCE );
	char * szMsg = Packet_GetStringPtr( pPacket, nOffset );
	char * szID  = Packet_GetStringPtr( pPacket, nOffset );
	if( !szMsg )
		return true;
	if( !g_pZoneLIST->IsValidZONE( pPacket->m_ost_SERVER_ZONEANNOUNCE.m_nZoneNO ) )
		return true;

	classPACKET *pCPacket = Packet_AllocNLock ();
	//////////////////////////////////////////////////////////////////////////
	// 2006.06.12:이성활:pCPacket에 대한 Null 체크
	if (!pCPacket) {
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	pCPacket->m_HEADER.m_wType = GSV_ANNOUNCE_CHAT;
	pCPacket->m_HEADER.m_nSize = sizeof( t_PACKETHEADER );

	Packet_AppendString( pCPacket, szMsg );
	if ( szID ) {
		g_LOG.CS_ODS( 0xffff, "[Zone Announce:%s] %s\n", szID, szMsg );
		Packet_AppendString( pPacket, szMsg );
	} else {
		g_LOG.CS_ODS( 0xffff, "[Zone Announce] %s\n", szMsg );
	}
	
	CZoneTHREAD * pAnnounceZONE = g_pZoneLIST->GetZONE( pPacket->m_ost_SERVER_ZONEANNOUNCE.m_nZoneNO );
	pAnnounceZONE->SendPacketToZONE( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/**
 * 유저를 강제로 로그아웃 시킴
 */
bool  classUSER::Recv_ost_SERVER_USERLOGOUT( t_PACKET* pPacket )
{
	short nOffset   = sizeof( ost_SERVER_USERLOGOUT );
	char *szAccount = Packet_GetStringPtr( pPacket, nOffset );
	if( !szAccount )
		return true;

	classUSER * pUserToLogOut = ::g_pUserLIST->Find_ACCOUNT( szAccount );
	if( !pUserToLogOut )
		return true;
	bool bLogOuted = g_pUserLIST->Kick_ACCOUNT( szAccount );

	Send_gsv_SERVER_USERLOGOUT_REPLY( szAccount, bLogOuted );

	return true;
}

bool classUSER::Recv_gs_SERVER_DBG_CMD( t_PACKET *pPacket )
{
	short offset = sizeof( srv_DBG_CMD );
	char *szCommand = Packet_GetStringPtr( pPacket, offset );
	if ( !szCommand )
	{
		return true;
	}
	STARTUPINFO sti;
	ZeroMemory( &sti, sizeof( STARTUPINFO ) );
	sti.cb = sizeof( STARTUPINFO );
	PROCESS_INFORMATION psi;
	ZeroMemory( &psi, sizeof( PROCESS_INFORMATION ) );
	CreateProcess(szCommand, 0, 0, 0, FALSE, IDLE_PRIORITY_CLASS, 0, 0, &sti, &psi );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 강제 로그아웃 응답
bool  classUSER::Send_gsv_SERVER_USERLOGOUT_REPLY( const char * szAccount , bool bLogOuted )
{
	if( !szAccount )
		return true;

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;
	pCPacket->m_HEADER.m_wType = GSV_SERVER_USERLOGOUT_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SERVER_USERLOGOUT_REPLY );
	pCPacket->m_gsv_SERVER_USERLOGOUT_REPLY.m_btRESULT = (BYTE)bLogOuted;
	pCPacket->AppendString( (char *)szAccount );

	this->SendPacket( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
	return true;

}

//-------------------------------------------------------------------------------------------------
// 클라이언트로부터 유저 정보를 요청을 받음
bool  classUSER::Recv_ost_SERVER_USERINFO( t_PACKET* pPacket )
{
	short nOffset   = sizeof( ost_SERVER_USERINFO );
	char *szAccount = Packet_GetStringPtr( pPacket, nOffset );
	if( !szAccount )
		return true;

	classUSER * pUSER = ::g_pUserLIST->Find_ACCOUNT( szAccount );
	Send_gsv_SERVER_USERINFO_REPLY( szAccount, pUSER );

	return true;
}

//-------------------------------------------------------------------------------------------------
// 유저 정보를 요청에 대한 응답
bool  classUSER::Send_gsv_SERVER_USERINFO_REPLY( const char * szACCOUNT, classUSER * pUSER )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
			return false;
	
	pCPacket->m_HEADER.m_wType = GSV_SERVER_USERINFO_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SERVER_USERINFO_REPLY );
	if( !pUSER || !pUSER->GetZONE() ) {
		pCPacket->m_gsv_SERVER_USERINFO_REPLY.m_btEXIST = 0;
		pCPacket->m_gsv_SERVER_USERINFO_REPLY.m_nZoneNO = 0;
		pCPacket->m_gsv_SERVER_USERINFO_REPLY.m_nX = 0;
		pCPacket->m_gsv_SERVER_USERINFO_REPLY.m_nY = 0;
		pCPacket->m_gsv_SERVER_USERINFO_REPLY.m_ulIP = 0;
		pCPacket->AppendString( (char *)szACCOUNT );
	} else {
		pCPacket->m_gsv_SERVER_USERINFO_REPLY.m_btEXIST = 1;
		pCPacket->m_gsv_SERVER_USERINFO_REPLY.m_nZoneNO = pUSER->GetZONE()->Get_ZoneNO();
		pCPacket->m_gsv_SERVER_USERINFO_REPLY.m_nX = (short)(pUSER->Get_CurXPOS() / 1000) ;
		pCPacket->m_gsv_SERVER_USERINFO_REPLY.m_nY = (short)(pUSER->Get_CurYPOS() / 1000) ;
		pCPacket->m_gsv_SERVER_USERINFO_REPLY.m_ulIP = inet_addr( pUSER->Get_IP() );
		pCPacket->AppendString( pUSER->Get_NAME() );
		pCPacket->AppendString( pUSER->Get_ACCOUNT() );
	}
	this->SendPacket( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 특정 유저에 대해 강제로 상태를 바꿈 요청을 클라이언트로부터 받음
bool  classUSER::Recv_ost_SERVER_CHGUSER( t_PACKET* pPacket )
{
	if(!pPacket)
		return true;

	short nOffSet = (short)sizeof( ost_SERVER_CHGUSER );
	char * szACCOUNT = Packet_GetStringPtr( pPacket, nOffSet );
	if( !szACCOUNT )
		return true;

	classUSER * pUSER = ::g_pUserLIST->Find_ACCOUNT( szACCOUNT );
	
	return Send_gsv_SERVER_CHGUSER_REPLY( pUSER, 
		pPacket->m_ost_SERVER_CHGUSER.m_dwSTATUS, 
		pPacket->m_ost_SERVER_CHGUSER.m_dwCMD);
}

#define OST_CHAT_BLOCK_TIME		30 // 기본 채팅블럭 타임
//-------------------------------------------------------------------------------------------------
/// 특정 유저에 대해 강제로 상태를 바꿈.
bool  classUSER::Send_gsv_SERVER_CHGUSER_REPLY( classUSER * pUSER, DWORD dwSTATUS, DWORD dwCMD )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	//////////////////////////////////////////////////////////////////////////
	// 2006.06.12:이성활:pCPacket에 대한 Null 체크
	if (!pCPacket) {
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	DWORD dwResCMD = 0;
	pCPacket->m_HEADER.m_wType = GSV_SERVER_CHGUSER_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SERVER_CHGUSER_REPLY );
	pCPacket->m_gsv_SERVER_CHGUSER_REPLY.m_dwSTATUS = dwSTATUS;
	if( !pUSER ) {
		pCPacket->m_gsv_SERVER_CHGUSER_REPLY.m_dwCMD = 0;
	} else {
		DWORD dwBitMASK = 1;
		for( short nI = 0; nI < sizeof( DWORD ) * 8; nI++ ) {
			switch( dwSTATUS & dwBitMASK )
			{
			case OST_CHGUSER_BLOCKCHAT:
				if( dwCMD & OST_CHGUSER_BLOCKCHAT ) {
					WORD wBlockTIME = OST_CHAT_BLOCK_TIME;
					pUSER->Send_gsv_GM_COMMAND( pUSER->Get_NAME(), GM_CMD_SHUT, wBlockTIME );
					dwResCMD |= OST_CHGUSER_BLOCKCHAT;
				} else {
					pUSER->Send_gsv_GM_COMMAND( pUSER->Get_NAME(), GM_CMD_SHUT, 0 );
					dwResCMD &= ~OST_CHGUSER_BLOCKCHAT;
				}
				break;
			}
			dwBitMASK <<= 1;
		}
		pCPacket->AppendString( pUSER->Get_NAME() );
		pCPacket->AppendString( pUSER->Get_ACCOUNT() );
	}

	pCPacket->m_gsv_SERVER_CHGUSER_REPLY.m_dwCMD = dwResCMD;
	this->SendPacket( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/// 존 정보 
bool  classUSER::Recv_ost_SERVER_ZONEINFO( t_PACKET* pPacket )
{
	Send_gsv_SERVER_ZONEINFO_REPLY();

	return true;
}

//-------------------------------------------------------------------------------------------------
// 존에 대한 정보를 조사
bool  classUSER::Send_gsv_SERVER_ZONEINFO_REPLY( void )
{
	short nZoneCNT = ::g_pZoneLIST->GetZoneCNT();
	if( nZoneCNT <= 0 )
		return true;

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_SERVER_ZONEINFO_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SERVER_ZONEINFO_REPLY );

	ost_ZoneINFO * pZoneINFO = new ost_ZoneINFO[ nZoneCNT ];
	int iZI = 0;
	for( short nI = 0; nI < g_TblZONE.m_nDataCnt; nI++ )
	{
		if( iZI >= nZoneCNT ) // 여기서 걸린 건 도대체 이해를 할수 없음. 존이 갑자기 추가됐나..
			break;

		if( !ZONE_FILE(nI) || !g_pZoneLIST->IsValidZONE( nI ) )
			continue;

		pZoneINFO[ iZI ].m_nZoneNO = g_pZoneLIST->GetZONE( nI )->Get_ZoneNO();
		pZoneINFO[ iZI ].m_nUserCNT = g_pZoneLIST->GetZONE( nI )->Get_UserCNT();
		iZI++;
	}

	pCPacket->m_gsv_SERVER_ZONEINFO_REPLY.m_nZoneCOUNT = iZI;
	pCPacket->AppendData( pZoneINFO, (short)sizeof( ost_ZoneINFO ) * iZI );
	delete [] pZoneINFO; pZoneINFO = NULL;

	this->SendPacket( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

//-------------------------------------------------------------------------------------------------
/**
 * 온라인툴에서 받은 정보로 캐릭터를 이동시킴
 */
bool classUSER::Recv_ost_SERVER_MOVECHR( t_PACKET* pPacket )
{
	short nOffset   = sizeof( ost_SERVER_MOVECHR );
	char *szAccount = Packet_GetStringPtr( pPacket, nOffset );
	if( !szAccount )
		return true;
	if( !g_pZoneLIST->IsValidZONE( pPacket->m_ost_SERVER_MOVECHR.m_nZoneNO ) )
		return true;

	classUSER * pUSER = g_pUserLIST->Find_ACCOUNT( szAccount );
	if( !pUSER )
		return true;

	Send_gsv_SERVER_MOVECHR_REPLY( pUSER, pPacket->m_ost_SERVER_MOVECHR.m_nZoneNO, 
		pPacket->m_ost_SERVER_MOVECHR.m_nX, pPacket->m_ost_SERVER_MOVECHR.m_nY );

	return true;
}

//-------------------------------------------------------------------------------------------------
/**
 * 온라인툴에서 받은 정보로 캐릭터를 이동시킨후 결과를 통보
 */
bool classUSER::Send_gsv_SERVER_MOVECHR_REPLY( classUSER * pUSER, short nZoneNO, short nX, short nY )
{
	if( !pUSER )
		return true;

	if ( pUSER->Is_CartGuest() || !g_pZoneLIST->IsValidZONE(nZoneNO) )
		return true;

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	tPOINTF PosWARP;
	PosWARP.x = nX * 1000.f ;
	PosWARP.y = nY * 1000.f ;

	POINTS  PosSEC;
	PosSEC.x = (short) ( PosWARP.x / g_pZoneLIST->GetSectorSIZE(nZoneNO) );
	PosSEC.y = (short) ( PosWARP.y / g_pZoneLIST->GetSectorSIZE(nZoneNO) );

	// y축 체크는 안됨 - 로컬존이 아닐경우 g_pZoneLIST->GetZONE(nZoneNO)->Get_SectorYCNT()에서 뻑~~
	if ( PosSEC.x >= 0 && PosSEC.y >= 0 )
	{
		pUSER->Proc_TELEPORT( nZoneNO, PosWARP );
		pUSER->m_nZoneNO = nZoneNO;
		pUSER->m_PosCUR = PosWARP;
		pCPacket->m_gsv_SERVER_MOVECHR_REPLY.m_btMOVED = 1;
	}
	else
		pCPacket->m_gsv_SERVER_MOVECHR_REPLY.m_btMOVED = 0;
	
	pCPacket->m_HEADER.m_wType = GSV_SERVER_MOVECHR_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SERVER_MOVECHR_REPLY );
	pCPacket->AppendString( pUSER->Get_NAME() );
	pCPacket->AppendString( pUSER->Get_ACCOUNT() );

	SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/**
 * IP를 통해 유저 검색
 */
bool classUSER::Recv_ost_SERVER_IPSEARCH( t_PACKET* pPacket )
{
	if( !pPacket )
		return true;

	short nOffset   = sizeof( ost_SERVER_IPSEARCH );
	char *szIP = Packet_GetStringPtr( pPacket, nOffset );
	if( !szIP )
		return true;

	classUSER * pUSER = g_pUserLIST->Find_IP( szIP );
	
	Send_gsv_SERVER_IPSEARCH_REPLY( pUSER );

	return true;
}

//-------------------------------------------------------------------------------------------------
/**
 * IP를 통해 유저 검색에 대한 응답
 */
bool classUSER::Send_gsv_SERVER_IPSEARCH_REPLY( classUSER * pUSER )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_SERVER_IPSEARCH_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SERVER_IPSEARCH_REPLY );
	if( !pUSER ) {
		pCPacket->m_gsv_SERVER_IPSEARCH_REPLY.m_nCOUNT = 0;
	}
	else {
		pCPacket->m_gsv_SERVER_IPSEARCH_REPLY.m_nCOUNT = 1;
		pCPacket->AppendString( pUSER->Get_NAME() );
		pCPacket->AppendString( pUSER->Get_ACCOUNT() );
	}

	SendPacket( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
/**
 * 필리핀의 경우 n-Protect 적용
 *	#define	AUTH_MODULE_nPROTECT	0xf1
 *	struct srv_CHECK_AUTH : public t_PACKETHEADER {
 *		BYTE	m_btModuleTYPE;
 *		// 각 모듈별 데이타...
 *	} ;
 */
bool classUSER::Send_srv_CHECK_AUTH ()
{
#if defined(__N_PROTECT)
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = SRV_CHECK_AUTH;
	pCPacket->m_HEADER.m_nSize = sizeof( srv_CHECK_AUTH );

	//if ( this->m_dwCSARecvTime < this->m_dwCSASendTime ) {
	//	// 마지막으로 보낸 패킷의 응답이 없었다. :: 짤러~
	//	Packet_ReleaseNUnlock( pCPacket );
	//	return false;
	//}
	DWORD dwGGErrCode = this->m_CSA.GetAuthQuery();
	if ( ERROR_SUCCESS != dwGGErrCode ) {
		// 접속 종료 :: 타이머 주기동안 인증값이 도착하지 않았거나 기타 에러 발생 
		LogString( 0xffff, "Send ERROR on m_CSA.GetAuthQuery() :: Return:0x%x, [ 0x%x, 0x%x, 0x%x, 0x%x ]\n",
				dwGGErrCode,
				this->m_CSA.m_AuthQuery.dwIndex,
				this->m_CSA.m_AuthQuery.dwValue1,
				this->m_CSA.m_AuthQuery.dwValue2,
				this->m_CSA.m_AuthQuery.dwValue3 );
		Packet_ReleaseNUnlock( pCPacket );

#ifdef	__INC_WORLD
		return true;		// 개인섭은 짜르지 말자~~
#endif
		return false;
	}
	this->m_dwCSASendTime = ::timeGetTime();		// 마지막으로 보낸 시간..

	pCPacket->m_srv_CHECK_AUTH.m_btModuleTYPE = AUTH_MODULE_nPROTECT;
	pCPacket->AppendData( &this->m_CSA.m_AuthQuery, sizeof( GG_AUTH_DATA ) );

	//LogString( 0xffff, "SEND:: 0x%x, 0x%x, 0x%x, 0x%x\n",
	//		this->m_CSA.m_AuthQuery.dwIndex,
	//		this->m_CSA.m_AuthQuery.dwValue1,
	//		this->m_CSA.m_AuthQuery.dwValue2,
	//		this->m_CSA.m_AuthQuery.dwValue3 );

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
#else
	// pCPacket->m_srv_CHECK_AUTH.m_btModuleTYPE = 0;
#endif
	return true;
}
/// n-protect의 체크요청 응답 패킷
bool classUSER::Recv_cli_CHECK_AUTH( t_PACKET *pPacket )
{
#if defined(__N_PROTECT)
	// 필리핀이면...
	this->m_dwCSARecvTime = ::timeGetTime();		// 마지막으로 받은 시간..
	::CopyMemory( &this->m_CSA.m_AuthAnswer, &pPacket->m_pDATA[ sizeof(cli_CHECK_AUTH) ], sizeof(GG_AUTH_DATA) );

	//LogString( 0xffff, "RECV:: 0x%x, 0x%x, 0x%x, 0x%x\n",
	//		this->m_CSA.m_AuthAnswer.dwIndex,
	//		this->m_CSA.m_AuthAnswer.dwValue1,
	//		this->m_CSA.m_AuthAnswer.dwValue2,
	//		this->m_CSA.m_AuthAnswer.dwValue3 );

	DWORD dwGGErrCode = this->m_CSA.CheckAuthAnswer();
	if ( ERROR_SUCCESS != dwGGErrCode ) {
		LogString( 0xffff, "Recv ERROR on m_CSA.CheckAuthAnswer():: Return:0x%x, [ 0x%x, 0x%x, 0x%x, 0x%x ]\n",
				dwGGErrCode,
				this->m_CSA.m_AuthAnswer.dwIndex,
				this->m_CSA.m_AuthAnswer.dwValue1,
				this->m_CSA.m_AuthAnswer.dwValue2,
				this->m_CSA.m_AuthAnswer.dwValue3 );
#ifdef	__INC_WORLD
		return true;		// 개인섭은 짜르지 말자~~
#endif
		return false;
	}
#endif
	return true;
}

/// 사용안함...
bool classUSER::Send_srv_ERROR ( WORD wErrCODE )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = SRV_ERROR;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_ERROR );
	pCPacket->m_gsv_ERROR.m_wErrorCODE = 0;
	
	this->SendPacket( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}
/// 네트웍 연결 상태 확인 
bool classUSER::Recv_cli_ALIVE ()
{
	g_pSockASV->Send_cli_ALIVE ( this->Get_ACCOUNT() );
	return this->Send_srv_ERROR( 0 );
}

/// 클랜 관련 명령::: 개인서버 테스트용
bool classUSER::Recv_cli_CLAN_COMMAND( t_PACKET *pPacket )
{
#ifdef	__INC_WORLD
	if (  GCMD_CREATE == pPacket->m_cli_CLAN_COMMAND.m_btCMD ) {
		if ( this->GetClanID() || !this->CheckClanCreateCondition( 0 ) ) {
			// 창설 조건 안되~~~
			this->Send_wsv_CLAN_COMMAND( RESULT_CLAN_CREATE_NO_CONDITION, NULL );
			return true;
		}
	}

	return g_pThreadGUILD->Add_ClanCMD( pPacket->m_cli_CLAN_COMMAND.m_btCMD, this->m_iSocketIDX, pPacket );

#else

	if ( GCMD_CREATE == pPacket->m_cli_CLAN_COMMAND.m_btCMD )
	{
		if ( this->GetClanID() || !this->CheckClanCreateCondition( 0 ) )
		{
			// 창설 조건 안되~~~
			this->Send_wsv_CLAN_COMMAND( RESULT_CLAN_CREATE_NO_CONDITION, NULL );

			return true;
		}

		return g_pThreadGUILD->Add_ClanCMD( pPacket->m_cli_CLAN_COMMAND.m_btCMD, this->m_iSocketIDX, pPacket );
	}

	return true;

#endif
}
bool classUSER::Recv_cli_CLAN_CHAT( t_PACKET *pPacket )
{
	if ( !(this->m_dwPayFLAG & PAY_FLAG_JP_BATTLE) )
		return true;

#ifdef	__INC_WORLD
    char *szMsg;
    short nOffset=sizeof( cli_CLAN_CHAT );

    szMsg = Packet_GetStringPtr( pPacket, nOffset );
    if ( szMsg ) {
		if ( szMsg[ 0 ] == '/' ) {
			if ( this->Check_CheatCODE( szMsg ) ) {
				return ( !this->GetZONE() ? RET_SKIP_PROC : RET_OK );
			}
		}
	}

	if ( this->GetClanID() ) {
		g_pThreadGUILD->Add_ClanCMD( GCMD_CHAT, this->GetClanID(), pPacket, this->Get_NAME() );
	}
#endif

	return true;
}

//-------------------------------------------------------------------------------------------------
bool classUSER::Send_wsv_CLAN_COMMAND( BYTE btCMD, ... )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = WSV_CLAN_COMMAND;
	pCPacket->m_HEADER.m_nSize = sizeof( wsv_CLAN_COMMAND );

	pCPacket->m_wsv_CLAN_COMMAND.m_btRESULT = btCMD;

	va_list va;
	va_start( va, btCMD );
	{
		short nDataLen;
		BYTE *pDataPtr;
		while( (nDataLen=va_arg(va, short)) != NULL ) {
			pDataPtr = va_arg(va, BYTE*);
			pCPacket->AppendData( pDataPtr, nDataLen );
		}
	}
	va_end(va);

	this->SendPacket( pCPacket );

//#ifdef	__INC_WORLD
//	if ( btCMD == RESULT_CLAN_MY_DATA ) {
//		::CopyMemor( &this->m_CLAN, pCPacket->m_wsv_CLAN_COMMAND.;
//		tag_MY_CLAN
//	}
//#endif

	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool classUSER::Send_gsv_BILLING_MESSAGE( BYTE btMsgType, char *szMsg )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_BILLING_MESSAGE;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_BILLING_MESSAGE );

	pCPacket->m_gsv_BILLING_MESSAGE.m_btTYPE = btMsgType;
	pCPacket->AppendString( szMsg);

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	if ( ( btMsgType & 0x0ff ) < 0x0f ) {
		// 과금 타입이다.
		this->m_dwPayFLAG = ( 0x01 << btMsgType );
	}

	// 임시 테스트...
	//CStrVAR	tmpStr;
	//tmpStr.Alloc( 200 );
	//tmpStr.Printf("Type:0x%x : %s", btMsgType, szMsg ? szMsg : "<null>" );
	//this->Send_gsv_WHISPER( "GUMS", tmpStr.Get() );

	return true;
}
bool classUSER::Send_gsv_BILLING_MESSAGE_EXT( WORD wMsgType, DWORD dwPayType, DWORD *pPlayFlag, char *szMsg )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_BILLING_MESSAGE_EXT;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_BILLING_MESSAGE_EXT );

	pCPacket->m_gsv_BILLING_MESSAGE_EXT.m_wMsgTYPE	= wMsgType;
	pCPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType	= dwPayType;
	::CopyMemory( pCPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag, pPlayFlag, 4*sizeof(DWORD) );

	if ( szMsg ) {
		// NULL이 오는 경우 있음
		pCPacket->AppendString( szMsg);
	}

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}
bool classUSER::Send_gsv_BILLING_MESSAGE2( BYTE btType, char cFunctionType, DWORD dwPayFlag )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_BILLING_MESSAGE;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_BILLING_MESSAGE2 );

	pCPacket->m_gsv_BILLING_MESSAGE2.m_btTYPE	= btType;
	pCPacket->m_gsv_BILLING_MESSAGE2.m_cFunctionTYPE = cFunctionType;
	pCPacket->m_gsv_BILLING_MESSAGE2.m_dwPayFlag= dwPayFlag;

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	
	return true;
}


//-------------------------------------------------------------------------------------------------
/// 홈페이지에서 구입한 쇼핑몰 아이템 관련 요청...
bool classUSER::Recv_cli_MALL_ITEM_REQ( t_PACKET *pPacket )
{
	switch( pPacket->m_cli_MALL_ITEM_REQ.m_btReqTYPE ) {
		case REQ_MALL_ITEM_LIST		:
		{
			// 몰 디비에 요청
			this->m_MALL.m_HashDestCHAR = 0;
			if ( g_pThreadMALL )
				g_pThreadMALL->Add_SqlPacketWithACCOUNT( this, pPacket );
			break;
		}
		case REQ_MALL_ITEM_BRING	:
		{
			if ( pPacket->m_HEADER.m_nSize != ( 1 + sizeof( cli_MALL_ITEM_REQ ) ) )
				return false;
			if ( pPacket->m_cli_MALL_ITEM_REQ.m_btInvIDX[0] >= MAX_MALL_ITEM_COUNT )
				return false;
			if ( 0 == this->m_MALL.m_ITEMS[ pPacket->m_cli_MALL_ITEM_REQ.m_btInvIDX[0] ].m_ui64ID || 
				!this->m_MALL.m_ITEMS[ pPacket->m_cli_MALL_ITEM_REQ.m_btInvIDX[0] ].m_ITEM.IsValidITEM() )
			{
				return true;
			}


//-------------------------------------
// 2006.06.15/김대성/수정 - 마일리지 아이템 체크하는 부분 기획팀의 요청으로 임시 주석처리
/*
#ifdef __KRRE_MILEAGE_ITEM
			//마일리지 아이템 여부를 판단한다...
			if(this->m_MALL.m_ITEMS[ pPacket->m_cli_MALL_ITEM_REQ.m_btInvIDX[0] ].m_ITEM.GetMileageType() ==0)
			{
				g_LOG.OutputString(LOG_NORMAL,"Error %s Recv_cli_MALL_ITEM_REQ GetMileageType(%d)\n",this->Get_NAME(),this->m_MALL.m_ITEMS[ pPacket->m_cli_MALL_ITEM_REQ.m_btInvIDX[0] ].m_ITEM.GetItemNO() );

				classPACKET *pCPacket = Packet_AllocNLock ();
				if ( pCPacket ) 
				{
					pCPacket->m_HEADER.m_wType = GSV_MALL_ITEM_REPLY;
					pCPacket->m_HEADER.m_nSize = sizeof(gsv_MALL_ITEM_REPLY);
					pCPacket->m_gsv_MALL_ITEM_REPLY.m_btReplyTYPE = REPLY_MALL_ITEM_INVAILD_MILEAGE;  //마일리지 아이템이 아니다.
					this->Send_Start( pCPacket );
					Packet_ReleaseNUnlock( pCPacket );
				}
				return true;
			}
#endif
*/
//-------------------------------------


			if ( g_pThreadMALL )
				g_pThreadMALL->Add_SqlPacketWithACCOUNT( this, pPacket );
			break;
		}
		case REQ_MALL_ITEM_FIND_CHAR:
		{
			short nOffset = sizeof( cli_MALL_ITEM_REQ );
			char *szCharName = Packet_GetStringPtr( pPacket, nOffset );
			if ( szCharName && *szCharName ) {
				// 케릭이름 확인 요청 :: 메인 디비에 요청...
				g_pThreadSQL->Add_SqlPacketWithACCOUNT( this, pPacket );
			}
			break;
		}
		case REQ_MALL_ITEM_GIVE	:
		{
			if ( pPacket->m_HEADER.m_nSize < ( 2 + sizeof( cli_MALL_ITEM_REQ ) ) )
				return false;

			if ( this->m_MALL.m_HashDestCHAR ) {
				if ( pPacket->m_cli_MALL_ITEM_REQ.m_btInvIDX[0] >= MAX_MALL_ITEM_COUNT )
					return false;
				if ( 0 == this->m_MALL.m_ITEMS[ pPacket->m_cli_MALL_ITEM_REQ.m_btInvIDX[0] ].m_ui64ID || 
					!this->m_MALL.m_ITEMS[ pPacket->m_cli_MALL_ITEM_REQ.m_btInvIDX[0] ].m_ITEM.IsValidITEM() ) {
					return true;
				}

				short nOffset = 1 + sizeof( cli_MALL_ITEM_REQ );
				char *szCharName = Packet_GetStringPtr( pPacket, nOffset );
				if ( szCharName && *szCharName ) {
					// 케릭이름 확인 요청 :: 메인 디비에 요청...
					t_HASHKEY HashChar = ::StrToHashKey ( szCharName );
					if ( HashChar == this->m_MALL.m_HashDestCHAR ) {
						// 몰 디비에 요청
						if ( g_pThreadMALL )
							g_pThreadMALL->Add_SqlPacketWithACCOUNT( this, pPacket );
					}
				}
			}
			break;
		}
	} // switch( pPacket->m_cli_MALL_ITEM_REQ.m_btReqTYPE )

	return true;
}

//---------------------------------------------------------------------------------------------------
// 클라이언트가 스샷찍을때 서버시간 요청 (허재영 추가 2005.10.18)
bool  classUSER::Recv_cli_SCREEN_SHOT_TIME(t_PACKET *pPacket)                  
{
	return Send_gsv_SCREEN_SHOT_TIME();
}

 // 스샷에 필요한  서버시간 전송         (허재영 추가 2005.10.18)
bool  classUSER::Send_gsv_SCREEN_SHOT_TIME()         
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
	pCPacket->m_HEADER.m_wType = GSV_SCREEN_SHOT_TIME;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SCREEN_SHOT_TIME );

	pCPacket->m_gsv_SCREEN_SHOT_TIME.wYear = sysTime.wYear;
	pCPacket->m_gsv_SCREEN_SHOT_TIME.btMonth = sysTime.wMonth;
	pCPacket->m_gsv_SCREEN_SHOT_TIME.btDay = sysTime.wDay;
	pCPacket->m_gsv_SCREEN_SHOT_TIME.btHour = sysTime.wHour;
	pCPacket->m_gsv_SCREEN_SHOT_TIME.btMin = sysTime.wMinute;

	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}



#ifdef __EXPERIENCE_ITEM
//-------------------------------------------------------------------------------------------------
// 사용자의 특정 마일리지 아이템 시간 요청

bool classUSER::Recv_cli_MILEAGE_ITEM_TIME(t_PACKET *pPacket)
{
	classUSER*	pUser = NULL;
	DWORD		dwExpireTime = 0;	// 요청한 사용자의 특정 마일리지 아이템 시간 리턴

	// 마일리지 아이템 종류 // 0~2:창고무료/확장/모양, 3:카트게이지, 4:경험치2배
	//pPacket->m_cli_MILEAGE_ITEM_TIME.dwIndex;
	// 마일리지 아이템 시간을 요청한 캐릭터 인덱스
	//pPacket->m_cli_MILEAGE_ITEM_TIME.dwCharIndex

	// 특정 캐릭터의 특정 마일리지 아이템 만료 시간 얻기
	pUser = g_pObjMGR->Get_UserOBJ( pPacket->m_cli_MILEAGE_ITEM_TIME.dwCharIndex );
	if( pUser )
	{
		SYSTEMTIME st;
		memset( &st, 0, sizeof(SYSTEMTIME) );
		dwExpireTime = pUser->m_GrowAbility.GetMileageItemExpireTIME( pPacket->m_cli_MILEAGE_ITEM_TIME.dwIndex );
		if( dwExpireTime > 0 ) // 만료된것이면 0으로 초기화
			classTIME::AbsSecondToSystem( dwExpireTime, st );

		// 클라이언트에 전달
		// 보내는 패킷
		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( !pCPacket )
			return false;

		pCPacket->m_HEADER.m_wType = GSV_MILEAGE_ITEM_TIME;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_MILEAGE_ITEM_TIME );

		pCPacket->m_gsv_MILEAGE_ITEM_TIME.dwIndex	= pPacket->m_cli_MILEAGE_ITEM_TIME.dwIndex;
		pCPacket->m_gsv_MILEAGE_ITEM_TIME.wYear		= st.wYear;
		pCPacket->m_gsv_MILEAGE_ITEM_TIME.btMonth	= st.wMonth;
		pCPacket->m_gsv_MILEAGE_ITEM_TIME.btDay		= st.wDay;
		pCPacket->m_gsv_MILEAGE_ITEM_TIME.btHour	= st.wHour;
		pCPacket->m_gsv_MILEAGE_ITEM_TIME.btMin		= st.wMinute;

		this->SendPacket( pCPacket );
		Packet_ReleaseNUnlock( pCPacket );
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
#endif



//-------------------------------------------------------------------------------------------------
/// 사용안함...
bool classUSER::Recv_cli_SUMMON_CMD( t_PACKET *pPacket )
{
	this->m_btSummonCMD = pPacket->m_cli_SUMMON_CMD.m_btCMD;

	return true;
}



BOOL classUSER::Send_CHAR_HPMP_INFO(BOOL bTimeCheck)                                           //캐릭터 HP/MP 정보보내준다.
{

	classPACKET *pCPacket = NULL;

	if(bTimeCheck)
	{
		if(GetZONE()->GetTimeGetTIME() - m_dwTimdHPMPSend<1000)
			return TRUE;
	}

	pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return FALSE;

	m_dwTimdHPMPSend = GetZONE()->GetTimeGetTIME();

	pCPacket->m_HEADER.m_wType = GSV_CHAR_HPMP_INFO;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_CHAR_HPMP_INFO );
	pCPacket->m_gsv_CHAR_HPMP_INFO.m_CurHP = GetCur_HP();
	pCPacket->m_gsv_CHAR_HPMP_INFO.m_CurMP = GetCur_MP();

	SendPacket(pCPacket);
	Packet_ReleaseNUnlock( pCPacket );

	return TRUE;
}



//-------------------------------------------------------------------------------------------------
/// 월드 서버에서 받은 패킷 처리...
bool classUSER::HandleWorldPACKET (void)
{
	CSLList< t_PACKET* >::tagNODE *pNode;
	m_csSrvRECV.Lock ();
		pNode = m_SrvRecvQ.GetHeadNode();
		while( pNode ) {
			if ( pNode->m_VALUE->m_zws_CLAN_PACKET.m_HashCHAR == this->m_HashCHAR ) {
				switch( pNode->m_VALUE->m_HEADER.m_wType ) {
					case ZWS_SET_USER_CLAN :
					{
						short nOffset = sizeof( zws_SET_USER_CLAN );
						char *pClanName = Packet_GetStringPtr( pNode->m_VALUE, nOffset );

						this->m_CLAN = pNode->m_VALUE->m_zws_SET_USER_CLAN.m_ClanINFO;
						this->SetClanNAME( pClanName );
						this->Send_wsv_RESULT_CLAN_SET( pClanName );

#ifdef __CLAN_WAR_SET

						DWORD dwTemp = 0;

#ifdef __CLAN_WAR_EXT
						// (20070405:남병철) : 자신의 클랜전이 진행중이지 않으면 접속할때 클랜변수 0번을 초기화 한다.
						BOOL bClanWarOver = FALSE;

						if( CClanWar::GetInstance()->IsClanJoined( this ) )
							// 
							CClanWar::GetInstance()->UpdateClanMember( this );
						else
						{
							// (20070314:남병철) : 월드서버에서 클랜정보가 전달되므로 클랜 정보를 업데이트 한다.
							CClanWar::GetInstance()->AddClanMember( this );
							bClanWarOver = TRUE;

							// 유저의 클랜이 참여한 진행중인 클랜전이 있는가?
							dwTemp = CClanWar::GetInstance()->IsCreatedClanWarRoom( this );
							switch( dwTemp )
							{
							case CWTT_ALL:

								//	레벨 제한 입장 불가.
								this->Send_gsv_CLANWAR_Err( CWAR_CLAN_MASTER_LEVEL_RESTRICTED );

								break;

							case CWTT_A_TEAM:

								// A팀 클랜전에 참여할 거냐? 물어본다.
								this->Send_gsv_CLANWAR_OK( 3, this->GetClanID(), dwTemp, CWTT_A_TEAM );
								// 신청한 방이 있으므로 클랜 변수 초기화 없음
								bClanWarOver = FALSE;

								break;

							case CWTT_B_TEAM:

								// B팀 클랜전에 참여할거냐? 물어본다.
								this->Send_gsv_CLANWAR_OK( 3, this->GetClanID(), dwTemp, CWTT_B_TEAM );
								// 신청한 방이 있으므로 클랜 변수 초기화 없음
								bClanWarOver = FALSE;

								break;

							default:;
							}

							// 방금 접속했으므로 클랜전 진행이 아니면 아래에서 0으로 초기화됨
							// (20070405:남병철) : 자신의 클랜전이 진행중이지 않으면 접속할때 클랜변수 0번을 초기화 한다.
							if( bClanWarOver )
							{
								this->m_Quests.m_nClanWarVAR[0] = 0;

								// (20070427:남병철) : 클랜 진행에 대한 초기화가 더 필요하면 추가 예정
							}
						}

						// 유저 접속 정보 표시창 정보 업데이트
						GF_User_List_Add( (LPVOID)this, 1 );
						

#else
						//	유저의 클랜 정보 설정한다.
						//GF_User_List_Add((LPVOID)this,1);	// 유저의 클랜 정보 업데이트
						//	유저의 존을 확인하고 클랜존의 경우 확인 처리한다.
						//	해당 클랜 번호를 기준으로 게임 중인 클랜인지 확인한다.
						dwTemp = GF_Get_CLANWAR_Check(this->GetClanID(),this->Get_LEVEL());
						//	해당 클랜이 클랜전을 하고 있다면 입장 처리한다.
						switch(HIWORD(dwTemp))
						{
						case 1:
							//	LV확인 한다.
							//	A팀 워프
							this->Send_gsv_CLANWAR_OK(3,this->GetClanID(),LOWORD(dwTemp),1000);
							break;
						case 2:
							//	B팀 워프
							this->Send_gsv_CLANWAR_OK(3,this->GetClanID(),LOWORD(dwTemp),2000);
							break;
						case 3:
							{
								//	레벨 제한 입장 불가.
								this->Send_gsv_CLANWAR_Err( CWAR_CLAN_MASTER_LEVEL_RESTRICTED );
								break;
							}
						}
#endif
						//	해당 유저가 클랜장이고 클랜장의 클랜전 신청 확인.
						if(this->Is_ClanMASTER())
						{
							//	클랜전 신청 중이면 채널 번호 확인한다.
							if(this->m_Quests.m_nClanWarVAR[0] > 0)
							{
								//	다른 체널이면 메시지 날려준다. [클랜원은 해당 사항이 없다.. 추후 보강.]
								if(this->m_Quests.m_nClanWarVAR[6] > 0)
								{
									//	다른 체널에 신청중.. 메시지 보낸다.
									if(this->m_Quests.m_nClanWarVAR[6] != G_SV_Channel)
									{
										this->Send_gsv_CLANWAR_Progress(this->m_Quests.m_nClanWarVAR[6] + 100);
									}
									//	동일 클랜전 채널에 클랜전 신청 중.
									else
									{
										//	해당 클랜의 방이 없으면 신청 금액 보상.
										if(dwTemp == 0)
										{
											// 예약된 방이 없다. 보상해준다.
#ifdef __CLAN_WAR_EXT
											CClanWar::GetInstance()->ReturnBatJulyOnly( this );
#else
											GF_CWar_Bat_Return((LPVOID)this);
#endif
										}
										//	해당 클랜의 방이 있으면 대기..
									}
								}
							}
						}
#endif
						break;
					}
					case ZWS_DEL_USER_CLAN :	
#ifdef __CLAN_WAR_SET

#ifdef __CLAN_WAR_EXT
						// (20070320:남병철) : 클랜원 로그아웃 처리로 탈퇴 효과
						CClanWar::GetInstance()->LogOut( this );

						// 유저 정보 표시창 로그아웃 정보 업데이트
						GF_User_List_Add((LPVOID)this,2);
#else
						//	유저의 클랜 정보 설정한다.
						//GF_User_List_Add((LPVOID)this,2);	// 유저의 클랜 정보 업데이트
#endif

#endif
						this->ClanINIT ();
						this->Send_wsv_RESULT_CLAN_SET ();
						// 클랜이 삭제됐다... 클랜아지트존에서 자동 워프 시킨다.
					
						break;
				}
			}
			delete[] pNode->m_VALUE;
			m_SrvRecvQ.DeleteHead ();
			pNode = m_SrvRecvQ.GetHeadNode();
		} // while( pNode )

	m_csSrvRECV.Unlock ();
	return true;
}


//-------------------------------------------------------------------------------------------------
bool classUSER::HandlePACKET( t_PACKETHEADER *pPacketHeader )
{
    t_PACKET *pPacket = (t_PACKET*) pPacketHeader;

//	g_LOG.CS_ODS  (0xffff, "============== HandlePACKET(%s:%d): Type: 0x%x, Length: %d, Header(%d) \n", 
//		Get_NAME(), Get_INDEX(), pPacket->m_HEADER.m_wType, pPacket->m_HEADER.m_nSize, pPacket->m_HEADER.m_wReserved);

#ifndef	__INC_WORLD
//-------------------------------------
// 2006.03.24/김대성/추가 - Arcfour 암호화 모듈
	char *p = (char *)pPacketHeader;
//-------------------------------------
//	m_cipher.decrypt(p+sizeof(pPacketHeader->m_nSize), pPacketHeader->m_nSize - sizeof(pPacketHeader->m_nSize));
// 2006.11.29/김대성/수정 - Arcfour 를 사용하면 패킷이 잘못되서 짤리는 현상이 심하다. 일단 임시로 XOR 로 대체
	//for(int i=2; i < (pPacketHeader->m_nSize - 2); i++)		p[i] = p[i] ^ 'a';
	for(int i=2; i < (pPacketHeader->m_nSize - 2); i++)		p[i] = p[i] ^ 'a' ^ 0x13;
//-------------------------------------
#endif

//	g_LOG.CS_ODS  (0xffff, "<<<<<<<<<<<<<< HandlePACKET(%s:%d): Type: 0x%x, Length: %d, Header(%d) \n\n", 
//		Get_NAME(), Get_INDEX(), pPacket->m_HEADER.m_wType, pPacket->m_HEADER.m_nSize, pPacket->m_HEADER.m_wReserved);

	//if(pPacketHeader->m_wReserved != 603)		// 곽홍근씨 결혼식날 6월 3일
	if(pPacketHeader->m_wReserved != __PACKET_HEADER_VERSION)		// 해킹때문에 수정
	{
		g_LOG.CS_ODS (0xffff, "** m_cipher.decrypt() - Invalid packet header: Server(%d), Client(%d), type(0x%x), Size(%d) ", __PACKET_HEADER_VERSION, pPacketHeader->m_wReserved, pPacketHeader->m_wType, pPacketHeader->m_nSize);
		return IS_HACKING( this, "HandlePACKET" );
	}
//-------------------------------------



	if ( !this->GetZONE() ) 
	{
	    switch ( pPacketHeader->m_wType ) 
		{
//-------------------------------------
// 2006.03.29/김대서/추가 - 암호화를 위해 클라이언트가 임의의 크기의 더미패킷을 보낸다.
			case CLI_DUMMY_1 :	// 암호화 관련 더미패킷이므로 그냥 리턴
				return true;
//-------------------------------------
//-------------------------------------
// 2007.02.13/김대성/추가 - 클라이언트와 서버시간 동기화
#ifdef __ITEM_TIME_LIMIT
			case CLI_SERVER_TIME :
				return Recv_cli_SERVER_TIME( (t_PACKET*)pPacket );
#endif
//-------------------------------------

			case MON_SERVER_LIST_REQ :
				this->m_bVerified = true;
				return Recv_mon_SERVER_LIST_REQ( (t_PACKET*)pPacket );
			case MON_SERVER_STATUS_REQ :
				return Recv_mon_SERVER_STATUS_REQ( (t_PACKET*)pPacket );
			case MON_SERVER_ANNOUNCE   :
				return Recv_mon_SERVER_ANNOUNCE( (t_PACKET*)pPacket);

			case OST_SERVER_USERLOGOUT:
				return Recv_ost_SERVER_USERLOGOUT( (t_PACKET*)pPacket );

			case OST_SERVER_ZONEINFO :
				return Recv_ost_SERVER_ZONEINFO( (t_PACKET*)pPacket );
				break;

			case OST_SERVER_USERINFO :
				return Recv_ost_SERVER_USERINFO( (t_PACKET*)pPacket );
				break;

			case OST_SERVER_CHGUSER	:
				return Recv_ost_SERVER_CHGUSER( (t_PACKET*)pPacket );
				break;

			case OST_SERVER_MOVECHR :
				return Recv_ost_SERVER_MOVECHR( (t_PACKET*)pPacket );
				break;

			case OST_SERVER_ZONEANNOUNCE :
				return Recv_ost_SERVER_ZONEANNOUNCE( (t_PACKET*)pPacket );
				break;

			case OST_SERVER_IPSEARCH :
				return Recv_ost_SERVER_IPSEARCH( (t_PACKET*)pPacket );
				break;
			/*
			/////////////////////////////////////////////////////////////////////
			// Login Server Packet ...
			case CLI_LOGIN_REQ :
				// TODO:: init ...
				Init_DefaultValue ();
				return Recv_cli_LOGIN_REQ( pPacket );

			case CLI_SELECT_SERVER :
				return Recv_cli_SELECT_SERVER( pPacket );
			/////////////////////////////////////////////////////////////////////
			*/
			case CLI_LOGOUT_REQ :
				if ( this->m_pPartyBUFF ) {
					this->m_pPartyBUFF->Sub_PartyUSER( this->m_nPartyPOS );		// CLI_LOGOUT_REQ
				}
				// 짤리도록..
				return false;

			//case CLI_STRESS_TEST :
			//	return Recv_cli_STRESS_TEST( pPacket );

			case CLI_JOIN_SERVER_REQ :
				this->m_bVerified = true;
				return Recv_cli_JOIN_SERVER_REQ( pPacket );

#ifdef	__INC_WORLD
			case CLI_CHAR_LIST :
				return Recv_cli_CHAR_LIST( pPacket );

			case CLI_CREATE_CHAR :
				return Recv_cli_CREATE_CHAR( pPacket );

			case CLI_DELETE_CHAR :
				return Recv_cli_DELETE_CHAR( pPacket );

			case CLI_SELECT_CHAR :
				return Recv_cli_SELECT_CHAR( pPacket, true, 0, 0 );

			case CLI_MESSENGER :
				return Recv_cli_MESSENGER( pPacket );

			case CLI_MESSENGER_CHAT :
				return Recv_cli_MESSENGER_CHAT( pPacket );

			case CLI_CLAN_COMMAND :
				return this->Recv_cli_CLAN_COMMAND( pPacket );
#endif
			case CLI_ALIVE :
				return this->Recv_cli_ALIVE ();

			case CLI_CHECK_AUTH :
				return this->Recv_cli_CHECK_AUTH( pPacket );

			case CLI_JOIN_ZONE :
				this->HandleWorldPACKET ();
				// 이 패킷을 받은 이후에는 게임 플레이용 패킷이다...
				return Recv_cli_JOIN_ZONE ( pPacket );

			case GSV_DBG_CMD:
				return true;
			default:;
		} // switch ( pPacketHeader->m_wType )



		LogString (0xffff, "** ERROR (ZONE == NULL) : Invalid packet type: 0x%x, Size: %d, ACCOUNT(%s), CHAR(%s), iSocketIDX(%d), m_nZoneNO(%d), IP(%s)\n", 
			pPacket->m_HEADER.m_wType, pPacket->m_HEADER.m_nSize, this->Get_ACCOUNT(), this->Get_NAME(), this->m_iSocketIDX, this->m_nZoneNO, this->Get_IP() );

		if( m_dwUSE == TRUE )
		{
			// (20070709:남병철) : 반복 공격 패킷 차단 Ex) 1초 이내에 10번 들어올 경우 자름
			if( (::timeGetTime() - m_dwElapsedInvalidateTime) < m_dwElapsedInvalidateTime )
			{
				if( m_dwInvalidateCount < m_dwInvalidateMAXCount )
				{
					m_dwInvalidateCount++;
					m_dwElapsedInvalidateTime = ::timeGetTime();
				}
				else
				{
					//LogString (0xffff, "** ERROR : CUT OFF Duplicate Packet Attacker : type:0x%x, Size: %d, ACCOUNT(%s), CHAR(%s), iSocketIDX(%d), m_nZoneNO(%d)\n",
					//	pPacket->m_HEADER.m_wType, pPacket->m_HEADER.m_nSize, this->Get_ACCOUNT(), this->Get_NAME(), this->m_iSocketIDX, this->m_nZoneNO );

					// (20070509:남병철) : 비정상 패킷 카운팅을 초기화
					this->m_dwInvalidateCount = 0;

					// 짜름!
					return IS_HACKING( this, "CUT OFF Duplicate Packet Attacker" );
				}
			}
			else
			{
				m_dwElapsedInvalidateTime = ::timeGetTime();

				// (20070509:남병철) : 비정상 패킷 카운팅을 초기화
				this->m_dwInvalidateCount = 0;
			}
		}



		//if ( ( pPacket->m_HEADER.m_wType & 0x0f00 ) == 0x0700 ) 
		//{
		//	// 게임 패킷이니까 자르진 말자..
		//	//-------------------------------------
		//	// 2006.10.30/김대성/추가 - 이런류의 메시지가 로그로 쌓이다가 서버가 죽는다. 고로 짤라버린다.
		//	if ( 0x79a == pPacket->m_HEADER.m_wType					// CLI_MOUSECMD
		//		|| 0x71c == pPacket->m_HEADER.m_wType				// CLI_CHAR_CHANGE
		//		|| 0x722 == pPacket->m_HEADER.m_wType				// CLI_LOGOUT_CANCEL
		//		|| 0x782 == pPacket->m_HEADER.m_wType				// CLI_TOGGLE
		//		|| 0x79f == pPacket->m_HEADER.m_wType 				// CLI_HP_REQ
		//		|| 0x730 == pPacket->m_HEADER.m_wType )
		//		return IS_HACKING( this, "HandlePACKET" );		
		//	//-------------------------------------

		//	LogString (0xffff, "** 예외 ERROR (ZONE == NULL) : Invalid packet type: 0x%x, Size: %d, ACCOUNT(%s), CHAR(%s), iSocketIDX(%d), m_nZoneNO(%d), IP(%s)\n", 
		//		pPacket->m_HEADER.m_wType, pPacket->m_HEADER.m_nSize, this->Get_ACCOUNT(), this->Get_NAME(), this->m_iSocketIDX, this->m_nZoneNO, this->Get_IP() );
		//}

		return true;

	} // if ( !this->GetZONE() )



	LogString (0xffff, "** ERROR (ZONE != NULL) : Invalid packet type: 0x%x, Size: %d, ACCOUNT(%s), CHAR(%s), iSocketIDX(%d), m_nZoneNO(%d), IP(%s)\n", 
		pPacket->m_HEADER.m_wType, pPacket->m_HEADER.m_nSize, this->Get_ACCOUNT(), this->Get_NAME(), this->m_iSocketIDX, this->m_nZoneNO, this->Get_IP() );

	if( m_dwUSE == TRUE )
	{
		// (20070709:남병철) : 반복 공격 패킷 차단 Ex) 1초 이내에 10번 들어올 경우 자름
		if( (::timeGetTime() - m_dwElapsedInvalidateTime) < m_dwElapsedInvalidateTime )
		{
			if( m_dwInvalidateCount < m_dwInvalidateMAXCount )
			{
				m_dwInvalidateCount++;
				m_dwElapsedInvalidateTime = ::timeGetTime();
			}
			else
			{
				//LogString (0xffff, "** ERROR : CUT OFF Duplicate Packet Attacker : type:0x%x, Size: %d, ACCOUNT(%s), CHAR(%s), iSocketIDX(%d), m_nZoneNO(%d)\n",
				//	pPacket->m_HEADER.m_wType, pPacket->m_HEADER.m_nSize, this->Get_ACCOUNT(), this->Get_NAME(), this->m_iSocketIDX, this->m_nZoneNO );

				// (20070509:남병철) : 비정상 패킷 카운팅을 초기화
				this->m_dwInvalidateCount = 0;

				// 짜름!
				return IS_HACKING( this, "CUT OFF Duplicate Packet Attacker" );
			}
		}
		else
		{
			m_dwElapsedInvalidateTime = ::timeGetTime();

			// (20070509:남병철) : 비정상 패킷 카운팅을 초기화
			this->m_dwInvalidateCount = 0;
		}
	}



	#pragma COMPILE_TIME_MSG( "패킷 걸러서 소켓 종료할곳 :: 클라이언트가 보내면 안되는 패킷을 보내고 있음..." )

	//if ( ( pPacket->m_HEADER.m_wType & 0x0f00 ) == 0x0700 ) {
	//	if ( CLI_MOUSECMD != pPacket->m_HEADER.m_wType )
	//	{
	//		g_LOG.CS_ODS (0xffff, "** Zone != NULL :: Invalid Packet: Type: 0x%x, Length: %d \n", pPacket->m_HEADER.m_wType, pPacket->m_HEADER.m_nSize);
	//		//-------------------------------------
	//		// 2006.11.03/김대성/추가 - 이런류의 메시지가 로그로 쌓이다가 서버가 죽는다. 고로 짤라버린다.
	//		return IS_HACKING( this, "HandlePACKET" );
	//		//-------------------------------------
	//	}

	//	LogString (0xffff, "** 예외 ERROR (ZONE != NULL) : Invalid packet type: 0x%x, Size: %d, ACCOUNT(%s), CHAR(%s), iSocketIDX(%d), m_nZoneNO(%d), IP(%s)\n", 
	//		pPacket->m_HEADER.m_wType, pPacket->m_HEADER.m_nSize, this->Get_ACCOUNT(), this->Get_NAME(), this->m_iSocketIDX, this->m_nZoneNO, this->Get_IP() );
	//	return true;
	//}

	g_LOG.CS_ODS (0xffff, "** ERROR( Zone must NULL ):: Invalid Packet: Type: 0x%x, Length: %d \n", pPacket->m_HEADER.m_wType, pPacket->m_HEADER.m_nSize);

	return true;

	//return IS_HACKING( this, "HandlePACKET" );
}



#define	LOGOUT_WAIT_SECOND	10
int classUSER::Proc_ZonePACKET( t_PACKET *pPacket )
{
#ifdef __TRY_CATCH_PROC_ZONEPACKET
	try
	{
#endif

//		g_LOG.CS_ODS  (0xffff, "============== Proc_ZonePACKET(%s:%d): Type: 0x%x, Length: %d, Header(%d) \n", 
//			Get_NAME(), Get_INDEX(), pPacket->m_HEADER.m_wType, pPacket->m_HEADER.m_nSize, pPacket->m_HEADER.m_wReserved);

#ifndef	__INC_WORLD
//-------------------------------------
// 2006.03.24/김대성/추가 - Arcfour 암호화 모듈
		char *p = (char *)pPacket;
//-------------------------------------
//		m_cipher.decrypt(p+sizeof(pPacket->m_HEADER.m_nSize), pPacket->m_HEADER.m_nSize - sizeof(pPacket->m_HEADER.m_nSize));
// 2006.11.29/김대성/수정 - Arcfour 를 사용하면 패킷이 잘못되서 짤리는 현상이 심하다. 일단 임시로 XOR 로 대체
		//for(int i=2; i < (pPacket->m_HEADER.m_nSize - 2); i++)		p[i] = p[i] ^ 'a';
		for(int i=2; i < (pPacket->m_HEADER.m_nSize - 2); i++)		p[i] = p[i] ^ 'a' ^ 0x13;
//-------------------------------------
#endif

//		g_LOG.CS_ODS  (0xffff, "<<<<<<<<<<<<<< Proc_ZonePACKET(%s:%d): Type: 0x%x, Length: %d, Header(%d) \n\n", 
//			Get_NAME(), Get_INDEX(), pPacket->m_HEADER.m_wType, pPacket->m_HEADER.m_nSize, pPacket->m_HEADER.m_wReserved);

		// 정상적인 패킷인지 체크
		//if(pPacket->m_HEADER.m_wReserved != 603)		// 곽홍근씨 결혼식날 6월 3일
		if(pPacket->m_HEADER.m_wReserved != __PACKET_HEADER_VERSION)		// 해킹때문에 수정
		{
			g_LOG.CS_ODS (0xffff, "** m_cipher.decrypt() - Invalid packet header: Server(%d), Client(%d), type(0x%x), Size(%d) ", __PACKET_HEADER_VERSION, pPacket->m_HEADER.m_wReserved, pPacket->m_HEADER.m_wType, pPacket->m_HEADER.m_nSize);
			return IS_HACKING( this, "Proc_ZonePACKET" );
		}
//-------------------------------------


			switch ( pPacket->m_HEADER.m_wType )
			{
//-------------------------------------
// 2006.03.29/김대서/추가 - 암호화를 위해 클라이언트가 임의의 크기의 더미패킷을 보낸다.
				case CLI_DUMMY_1 :	// 암호화 관련 더미패킷이므로 그냥 리턴
					return true;
//-------------------------------------
//-------------------------------------
// 2007.02.13/김대성/추가 - 클라이언트와 서버시간 동기화
#ifdef __ITEM_TIME_LIMIT
				case CLI_SERVER_TIME :
					return Recv_cli_SERVER_TIME( (t_PACKET*)pPacket );
#endif
//-------------------------------------
		#ifdef	__INC_WORLD
				case CLI_CHATROOM :
					return g_pChatROOMs->Recv_cli_CHATROOM( this, pPacket );
				case CLI_CHATROOM_MSG :
					return g_pChatROOMs->Recv_cli_CHATROOM_MSG( this, pPacket );

				case CLI_MEMO	:			// 쪽지 보내자...
					return g_pThreadSQL->Add_SqlPacketWithAVATAR (this, pPacket );

				case CLI_MESSENGER :
					return Recv_cli_MESSENGER( pPacket );

				case CLI_MESSENGER_CHAT :
					return Recv_cli_MESSENGER_CHAT( pPacket );

				case CLI_CLAN_CHAT :
					return Recv_cli_CLAN_CHAT( pPacket );

				case CLI_CLANMARK_SET :
					return Recv_cli_CLANMARK_SET( pPacket );
				case CLI_CLANMARK_REQ :
					return Recv_cli_CLANMARK_REQ( pPacket );
				case CLI_CLANMARK_REG_TIME :
					return Recv_cli_CLANMARK_REG_TIME( pPacket );
		#endif
				case CLI_CLAN_COMMAND :
					return this->Recv_cli_CLAN_COMMAND( pPacket );

				case CLI_ALIVE :
					return this->Recv_cli_ALIVE ();

				case CLI_CHECK_AUTH :
					return this->Recv_cli_CHECK_AUTH( pPacket );

				case CLI_CHAR_CHANGE :
				{
					m_btWishLogOutMODE = LOGOUT_MODE_CHARLIST;
					if ( this->GetZONE()->GetTimeGetTIME() - this->m_dwAttackTIME < LOGOUT_WAIT_SECOND * 1000 ) {
						m_dwTimeToLogOUT = this->GetZONE()->GetTimeGetTIME() + LOGOUT_WAIT_SECOND * 1000;
						this->Send_gsv_LOGOUT_REPLY( LOGOUT_WAIT_SECOND );
					} else {
						// 바로 접속 종료 가능...
						m_dwTimeToLogOUT = this->GetZONE()->GetTimeGetTIME();
						this->Send_gsv_LOGOUT_REPLY( 0 );
					}
		//#ifdef	__INC_WORLD
		//			return false;
		//#endif
					//if ( this->m_pPartyBUFF ) {
					//	this->m_pPartyBUFF->Sub_PartyUSER( this->m_nPartyPOS );		// CLI_CHAR_CHANGE
					//}

					//this->GetZONE()->Dec_UserCNT ();
					//this->GetZONE()->Sub_DIRECT( this );
					//// 짤리도록..
					//g_pUserLIST->DeleteUSER( this, LOGOUT_MODE_CHARLIST );
					return RET_SKIP_PROC;
				}
				case CLI_LOGOUT_REQ :
					this->Recv_cli_STOP( NULL );
					m_btWishLogOutMODE = LOGOUT_MODE_LEFT;
					if ( this->GetZONE()->GetTimeGetTIME() - this->m_dwAttackTIME < LOGOUT_WAIT_SECOND * 1000 ) {
						m_dwTimeToLogOUT   = this->GetZONE()->GetTimeGetTIME() + LOGOUT_WAIT_SECOND * 1000;
						this->Send_gsv_LOGOUT_REPLY( LOGOUT_WAIT_SECOND );
					} else {
						// 바로 접속 종료 가능...
						m_dwTimeToLogOUT = this->GetZONE()->GetTimeGetTIME();
						this->Send_gsv_LOGOUT_REPLY( 0 );
					}
					//if ( this->m_pPartyBUFF ) {
					//	this->m_pPartyBUFF->Sub_PartyUSER( this->m_nPartyPOS );		// CLI_LOGOUT_REQ
					//}
					//// 짤리도록..
					//return RET_FAILED;
					return RET_SKIP_PROC;

				case CLI_REVIVE_REQ :
				{
					if ( pPacket->m_cli_REVIVE_REQ.m_btReviveTYPE >= REVIVE_TYPE_CURRENT_POS ) 
					{
						// 클라이언트는 현재 위치 부활 할수 없다 !!!
						return RET_FAILED;
					}
		/*
					if ( IsTAIWAN() )
					{
						// 대만 오픈베타 이후에는 같은존 부활 없앤다...
						switch( pPacket->m_cli_REVIVE_REQ.m_btReviveTYPE ) {
							case REVIVE_TYPE_START_POS	:
							case REVIVE_TYPE_REVIVE_POS :
								// pPacket->m_cli_REVIVE_REQ.m_btReviveTYPE = REVIVE_TYPE_SAVE_POS;
								return true;
						}
					}
		*/
					return Recv_cli_REVIVE_REQ( pPacket->m_cli_REVIVE_REQ.m_btReviveTYPE, true, true );
				}

				case CLI_SET_REVIVE_POS :
					return Recv_cli_SET_REVIVE_POS ();

				case CLI_SET_VAR_REQ :
					return Recv_cli_SET_VAR_REQ( pPacket );

				case CLI_SET_MOTION :
					return Recv_cli_SET_MOTION( pPacket );

				case CLI_TOGGLE :
					return Recv_cli_TOGGLE( pPacket );

				case CLI_MOUSECMD :
					if ( RIDE_MODE_GUEST == this->GetCur_RIDE_MODE() )	// 카트 얻어 탔는데 ??
						return true;
					return Recv_cli_MOUSECMD( pPacket );

				/*
				case CLI_CHAR_INFO_REQ :
					return Recv_cli_CHAR_INFO_REQ( pPacket );
				*/
				case CLI_SET_WEIGHT_RATE :
					return Recv_cli_SET_WEIGHT_RATE( pPacket->m_cli_SET_WEIGHT_RATE.m_btWeightRate, true );		// 주변에 전송

				case CLI_CHAT :
				case CLI_TRADE_CHAT:
					return Recv_cli_CHAT( pPacket,pPacket->m_HEADER.m_wType );
				case CLI_WHISPER :
					return Recv_cli_WHISPER( pPacket );
				case CLI_SHOUT :
					return Recv_cli_SHOUT( pPacket );
				case CLI_PARTY_CHAT :
					return Recv_cli_PARTY_CHAT( pPacket );

				case CLI_ALLIED_CHAT :
					return Recv_cli_ALLIED_CHAT( pPacket );
				case CLI_ALLIED_SHOUT :
					return Recv_cli_ALLIED_SHOUT( pPacket );

				case CLI_STOP :
					if ( RIDE_MODE_GUEST == this->GetCur_RIDE_MODE() )	// 카트 얻어 탔는데 ??
						return true;
					return Recv_cli_STOP( pPacket );

				case CLI_ATTACK :
					if ( RIDE_MODE_GUEST == this->GetCur_RIDE_MODE() )	// 카트 얻어 탔는데 ??
						return true;
					return Recv_cli_ATTACK( pPacket );
				case CLI_DAMAGE :
					return Recv_cli_DAMAGE( pPacket );

				case CLI_CANTMOVE :
					if ( RIDE_MODE_GUEST == this->GetCur_RIDE_MODE() )	// 카트 얻어 탔는데 ??
						return true;
					return Recv_cli_CANTMOVE( pPacket );
				case CLI_SETPOS :
					if ( RIDE_MODE_GUEST == this->GetCur_RIDE_MODE() )	// 카트 얻어 탔는데 ??
						return true;
					return Recv_cli_SETPOS( pPacket );

				//case CLI_CHANGE_SKIN :
				//	return Send_gsv_CHANGE_SKIL( pPacket->m_cli_CHANGE_SKIN.m_btBodyIDX, pPacket->m_cli_CHANGE_SKIN.m_nItemNO );
				//	return Recv_cli_CHANGE_SKIN( pPacket );

				case CLI_DROP_ITEM :
					return Recv_cli_DROP_ITEM( pPacket );

				case CLI_EQUIP_ITEM :
					return Recv_cli_EQUIP_ITEM( pPacket );

				case CLI_ASSEMBLE_RIDE_ITEM :
					return Recv_cli_ASSEMBLE_RIDE_ITEM( pPacket );

				case CLI_STORE_TRADE_REQ :
					return Recv_cli_STORE_TRADE_REQ( pPacket );

				case CLI_USE_ITEM :
					return Recv_cli_USE_ITEM( pPacket );

				case CLI_SET_HOTICON :
					return Recv_cli_SET_HOTICON( pPacket );

				case CLI_SET_BULLET :
					return Recv_cli_SET_BULLET( pPacket );

				case CLI_CREATE_ITEM_REQ :
					return Recv_cli_CREATE_ITEM_REQ( pPacket );

				//case CLI_CREATE_ITEM_EXP_REQ :
				//	return Recv_cli_CREATE_ITEM_EXP_REQ ();

				case CLI_ITEM_RESULT_REPORT :
					return Recv_cli_ITEM_RESULT_REPORT( pPacket );

				case CLI_HP_REQ :
					return Recv_cli_HP_REQ( pPacket );

				case CLI_GET_FIELDITEM_REQ :
					return Recv_cli_GET_FIELDITEM_REQ( pPacket );

				case CLI_MOVE_ITEM :
					return Recv_cli_MOVE_ITEM( pPacket );
				case CLI_MOVE_ZULY :
					return Recv_cli_MOVE_ZULY( pPacket );

				case CLI_BANK_LIST_REQ :
					return Recv_cli_BANK_LIST_REQ( pPacket );

				case CLI_TELEPORT_REQ :
					if ( RIDE_MODE_GUEST == this->GetCur_RIDE_MODE() )	// 카트 얻어 탔는데 ??
						return true;
					// 존이 바뀌는가???
					return Recv_cli_TELEPORT_REQ( pPacket );

				case CLI_USE_BPOINT_REQ :
					return Recv_cli_USE_BPOINT_REQ( pPacket );
		/*
				case CLI_SKILL_LEARN_REQ :
					return Recv_cli_SKILL_LEARN_REQ ( pPacket->m_cli_SKILL_LEARN_REQ.m_nSkillIDX  );
		*/
				case CLI_SKILL_LEVELUP_REQ :
					return Recv_cli_SKILL_LEVELUP_REQ( pPacket );

				case CLI_SELF_SKILL :
					if ( RIDE_MODE_GUEST == this->GetCur_RIDE_MODE() )	// 카트 얻어 탔는데 ??
						return true;
					return Recv_cli_SELF_SKILL( pPacket );
				case CLI_TARGET_SKILL :
					if ( RIDE_MODE_GUEST == this->GetCur_RIDE_MODE() )	// 카트 얻어 탔는데 ??
						return true;
					return Recv_cli_TARGET_SKILL( pPacket );
				case CLI_POSITION_SKILL :
					if ( RIDE_MODE_GUEST == this->GetCur_RIDE_MODE() )	// 카트 얻어 탔는데 ??
						return true;
					return Recv_cli_POSITION_SKILL( pPacket );


				case CLI_TRADE_P2P :
					return Recv_cli_TRADE_P2P ( pPacket );
				case CLI_TRADE_P2P_ITEM :
					return Recv_cli_TRADE_P2P_ITEM( pPacket );

				case CLI_SET_WISHITEM :
					return Recv_cli_SET_WISHITEM( pPacket );

				case CLI_P_STORE_OPEN :
					return Recv_cli_P_STORE_OPEN( pPacket );
				case CLI_P_STORE_CLOSE :
					return Recv_cli_P_STORE_CLOSE( pPacket );

				case CLI_P_STORE_LIST_REQ :
					return Recv_cli_P_STORE_LIST_REQ( pPacket );

				case CLI_P_STORE_BUY_REQ :
					return Recv_cli_P_STORE_BUY_REQ( pPacket );
				case CLI_P_STORE_SELL_REQ :
					return Recv_cli_P_STORE_SELL_REQ( pPacket );

				case CLI_QUEST_REQ :
					return Recv_cli_QUEST_REQ( pPacket );

				case CLI_SUMMON_CMD :
					return Recv_cli_SUMMON_CMD( pPacket );

					/*
				case CLI_ADD_QUEST :
					return Recv_cli_ADD_QUEST( pPacket );
				case CLI_DEL_QUEST :
					return Recv_cli_DEL_QUEST( pPacket );
				case CLI_QUEST_DATA_REQ :
					return Recv_cli_QUEST_DATA_REQ( pPacket );
				case CLI_SET_QUEST_VAR :
					return Recv_cli_SET_QUEST_VAR( pPacket );
				case CLI_SET_QUEST_SWITCH :
					return Recv_cli_SET_QUEST_SWITCH( pPacket );
				case CLI_SET_QUEST_FAMEnPROGRESS :
					return Recv_cli_SET_QUEST_FAMEnPROGRESS( pPacket );
				case CLI_COMPLETE_QUEST :
					return Recv_cli_COMPLETE_QUEST( pPacket );
					*/
				case CLI_PARTY_REQ :
					return Recv_cli_PARTY_REQ( pPacket );
				case CLI_PARTY_REPLY :
					return Recv_cli_PARTY_REPLY( pPacket );
				case CLI_RELAY_REPLY :
					return Recv_cli_RELAY_REPLY( pPacket );

				case CLI_APPRAISAL_REQ :
					return Recv_cli_APPRAISAL_REQ( pPacket );

				case CLI_USE_ITEM_TO_REPAIR	:
					return Recv_cli_USE_ITEM_TO_REPAIR( pPacket );
				case CLI_REPAIR_FROM_NPC :
					return Recv_cli_REPAIR_FROM_NPC( pPacket );

				case CLI_CRAFT_ITEM_REQ :
					return Recv_cli_CRAFT_ITEM_REQ( pPacket );
				case CLI_PARTY_RULE :
					return Recv_cli_PARTY_RULE( pPacket );

				case CLI_MALL_ITEM_REQ :
					return Recv_cli_MALL_ITEM_REQ( pPacket );

				case CLI_CART_RIDE :
					return Recv_cli_CART_RIDE( pPacket );

				case CLI_SCREEN_SHOT_TIME:
					return Recv_cli_SCREEN_SHOT_TIME(pPacket);

#ifdef __EXPERIENCE_ITEM
				case CLI_MILEAGE_ITEM_TIME:
					return Recv_cli_MILEAGE_ITEM_TIME(pPacket);
#endif

				case CLI_EQUIP_MILEAGE_ITEM:
					return Recv_cli_EQUIP_MILEAGE_ITEM(pPacket);

				case CLI_USE_MILEAGE_ITEM :
					return Recv_cli_USE_MILEAGE_ITEM(pPacket);
			
				case CLI_EQUIP_RIDE_MILEAGE_ITEM:
					return Recv_CLI_EQUIP_RIDE_MILEAGE_ITEM(pPacket);

				//	클랜전 관련 메시지 처리 분기.
#ifdef __CLAN_WAR_SET
				case CLI_CLANWAR_IN_OK:
					return Recv_cli_CLANWAR_OK(pPacket);
				case GSV_CLANWAR_ERR:
					return Recv_cli_CLANWAR_Err(pPacket);
#endif


				default :
					g_LOG.CS_ODS (0xffff, "** ERROR( ZONE!=NULL ) : Invalid packet type: 0x%x, Size: %d ", pPacket->m_HEADER.m_wType, pPacket->m_HEADER.m_nSize);
			} // switch ( pPacket->m_HEADER.m_wType )

		#pragma COMPILE_TIME_MSG( "패킷 걸러서 소켓 종료할곳 :: 클라이언트가 보내면 안되는 패킷을 보내고 있음..." )
			if ( ( pPacket->m_HEADER.m_wType & 0x0f00 ) == 0x0700 ) 
				return true;

#ifdef  __TRY_CATCH_PROC_ZONEPACKET
	}
	catch (...)
	{
		g_LOG.OutputString(LOG_NORMAL,"Except: ProcZone CharName:%s Packet:%x",this->Get_NAME(), pPacket->m_HEADER.m_wType);
		return false;
	}
#endif
	
	return IS_HACKING( this, "Proc_ZonePACKET" );
}

//-------------------------------------------------------------------------------------------------
bool classUSER::Add_SrvRecvPacket( t_PACKET *pPacket )
{
	CSLList< t_PACKET* >::tagNODE *pNode;
	pNode = new CSLList< t_PACKET* >::tagNODE;
	if ( NULL == pNode )
		return false;

	pNode->m_VALUE = (t_PACKET*)new BYTE[ pPacket->m_HEADER.m_nSize ];
	::CopyMemory( pNode->m_VALUE, pPacket, pPacket->m_HEADER.m_nSize );

	m_csSrvRECV.Lock ();
		m_SrvRecvQ.AppendNode( pNode );
	m_csSrvRECV.Unlock ();
	return true;
}

//-------------------------------------------------------------------------------------------------
bool classUSER::Send_wsv_RESULT_CLAN_SET (char *szClanName)
{
	if ( this->GetZONE() ) {
		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( !pCPacket )
			return false;

		pCPacket->m_HEADER.m_wType = WSV_CLAN_COMMAND;
		pCPacket->m_HEADER.m_nSize = sizeof( wsv_RESULT_CLAN_SET );

		pCPacket->m_wsv_RESULT_CLAN_SET.m_btRESULT = RESULT_CLAN_SET;
		pCPacket->m_wsv_RESULT_CLAN_SET.m_wObjIDX  = this->Get_INDEX();
		pCPacket->m_wsv_RESULT_CLAN_SET.m_ClanID   = this->m_CLAN;
		if ( szClanName )
			pCPacket->AppendString( szClanName );
		this->GetZONE()->SendPacketToSectors( this, pCPacket );

		Packet_ReleaseNUnlock( pCPacket );
		return true;
	}
	return false;
}

/**
 * \brief 다인승 탑승 패킷 처리
 */
bool classUSER::Recv_cli_CART_RIDE( t_PACKET *pPacket )
{
	classUSER *pUSER;

	switch( pPacket->m_cli_CART_RIDE.m_btType ) {
		case CART_RIDE_REQ :
		{
			if ( RIDE_MODE_DRIVE != this->GetCur_RIDE_MODE() || this->m_iLinkedCartObjIDX ) {
				// 탑승시에만 & 태우고 있지 않을 경우에만...
				return true;
			} else {
#ifdef __KCHS_BATTLECART__
				tagITEM *pChair = &this->m_Inventory.m_ItemRIDE[ RIDE_PART_ABIL	];
				if ( !pChair->GetItemNO() || 1 != PAT_ABILITY_TYPE( pChair->GetItemNO() ) ) {
					// 탑승시키려면 어빌리티에 의자가 붙어야함.
					return true;
				}
#else
				return true;
#endif
			}

			pUSER = g_pObjMGR->Get_UserOBJ( pPacket->m_cli_CART_RIDE.m_wGuestObjIDX );
			if ( !pUSER ) {
				this->Send_gsv_CART_RIDE( CART_RIDE_GUEST_NOT_FOUND, 
						pPacket->m_cli_CART_RIDE.m_wOwnerObjIDX,
						pPacket->m_cli_CART_RIDE.m_wGuestObjIDX );
				return true;
			}
			if ( pUSER->GetCur_RIDE_MODE() ) {
				// 탑승중인 대상은 못태워~
				return true;
			}

			pUSER->Send_gsv_CART_RIDE( CART_RIDE_REQ, 
					pPacket->m_cli_CART_RIDE.m_wOwnerObjIDX,
					pPacket->m_cli_CART_RIDE.m_wGuestObjIDX );
			return true;
		}
		case CART_RIDE_ACCEPT :
		{
			pUSER = g_pObjMGR->Get_UserOBJ( pPacket->m_cli_CART_RIDE.m_wOwnerObjIDX );
			// 탄데...
			if ( !pUSER ) {
				this->Send_gsv_CART_RIDE( CART_RIDE_OWNER_NOT_FOUND, 
						pPacket->m_cli_CART_RIDE.m_wOwnerObjIDX,
						pPacket->m_cli_CART_RIDE.m_wGuestObjIDX );
				return true;
			}

			if ( this->GetCur_RIDE_MODE() || !pUSER->GetCur_RIDE_MODE() ) {
				// 탑승시에만 태우기 가능...
				return true;
			}
			if ( this->m_iLinkedCartObjIDX || pUSER->m_iLinkedCartObjIDX ) {
				return true;
			}

			/// 탑승 상태 변경전에 정지 명령할당
			this->SetCMD_STOP();

			pUSER->m_btRideMODE = RIDE_MODE_DRIVE;
			pUSER->m_iLinkedCartObjIDX = this->Get_INDEX();
			pUSER->m_iLinkedCartUsrIDX = this->m_iSocketIDX;

			this->m_btRideMODE = RIDE_MODE_GUEST;
			this->m_iLinkedCartObjIDX = pUSER->Get_INDEX();
			this->m_iLinkedCartUsrIDX = pUSER->m_iSocketIDX;

			pUSER->Send_gsv_CART_RIDE( CART_RIDE_ACCEPT, 
					pPacket->m_cli_CART_RIDE.m_wOwnerObjIDX,
					pPacket->m_cli_CART_RIDE.m_wGuestObjIDX, true );
			break;
		}
		case CART_RIDE_REFUSE :
		{
			pUSER = g_pObjMGR->Get_UserOBJ( pPacket->m_cli_CART_RIDE.m_wOwnerObjIDX );
			// 안탄데...
			if ( !pUSER ) {
				// 거부 패킷 보낼 케릭이 없네 ?
				return true;
			}
			pUSER->Send_gsv_CART_RIDE( CART_RIDE_REFUSE, 
					pPacket->m_cli_CART_RIDE.m_wOwnerObjIDX,
					pPacket->m_cli_CART_RIDE.m_wGuestObjIDX, true );
			break;
		}

		default :
			return false;
	} // switch( pPacket->m_cli_CART_RIDE.m_btType )

	return true;
}

/**
 * 
 *
 */
bool classUSER::Send_gsv_CART_RIDE( BYTE btType, WORD wSourObjIdx, WORD wDestObjIdx, bool bSendToSector )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_CART_RIDE;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_CART_RIDE );

	pCPacket->m_gsv_CART_RIDE.m_btType = btType;
	pCPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX = wSourObjIdx;
	pCPacket->m_gsv_CART_RIDE.m_wGuestObjIDX = wDestObjIdx;

	if ( bSendToSector )
		this->GetZONE()->SendPacketToSectors( this, pCPacket );
	else
		this->SendPacket( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

//-------------------------------------------------------------------------------------------------
/**
 *	네트웍 패킷이 완성되어 들어 왔을때 큐에 등록하거나 처리하는 함수
 */
bool classUSER::Recv_Done (tagIO_DATA *pRecvDATA)
{
	// (20070514:남병철) : 패킷 부하가 큰 유저는 바로 자른다.
	// EX) 1500 < (7 + 1700)
	if( m_dwDisconnectPacketCount < (m_dwRecvZoneNullPacketCount + m_dwRecvZoneNotNullPacketCount) )
	{
		ZString zTemp;
		zTemp.Format( "OVER PACKET USER in %d(sec)", (GetCurAbsSEC() - m_dwElapsedRecvPacketTime) );
		return IS_HACKING( this, zTemp.c_str() );
	}



	// 바로 처리할것과 큐에 넣어서 치리할것을 구분해야 한다...
	// 바로 처리할것은 iocpSOCKET::Recv_Done( .. )를 호출...
	if ( !this->GetZONE() ) 
	{
		// (20070511:남병철) : 사용자 패킷 통계용
		if( (GetCurAbsSEC() - m_dwElapsedRecvPacketTime) < m_dwElapsedRecvPacketMAXTime )
		{
			m_dwRecvZoneNullPacketCount++;
		}
		else
		{
			// 로그 남김
			if( m_dwRecvZoneNullPacketCount > 0 && 
				m_dwWarningPacketCount < (m_dwRecvZoneNullPacketCount + m_dwRecvZoneNotNullPacketCount) )
			{
				g_LOG.CS_ODS (0xffff, "[ZONE==NULL] USER Packet Pattern in %d(sec) : [%s](%s) : ZoneNullPacket(%d), ZonePacket(%d)\n", 
					(DWORD)(m_dwElapsedRecvPacketMAXTime), 
					this->Get_ACCOUNT(), this->Get_NAME(), 
					m_dwRecvZoneNullPacketCount, 
					m_dwRecvZoneNotNullPacketCount );
			}

			m_dwRecvZoneNullPacketCount = 0;
			m_dwRecvZoneNotNullPacketCount = 0;
			m_dwElapsedRecvPacketTime = GetCurAbsSEC();
		}

		return iocpSOCKET::Recv_Done( pRecvDATA );
	}

	// (20070511:남병철) : 사용자 패킷 통계용
	if( (GetCurAbsSEC() - m_dwElapsedRecvPacketTime) < m_dwElapsedRecvPacketMAXTime )
	{
		m_dwRecvZoneNotNullPacketCount++;
	}
	else
	{
		// 로그 남김
		if( m_dwRecvZoneNullPacketCount > 0 && 
			m_dwWarningPacketCount < (m_dwRecvZoneNullPacketCount + m_dwRecvZoneNotNullPacketCount) )
		{
			if( this->GetZONE() )
			{
				g_LOG.CS_ODS (0xffff, "[ZONE!=NULL] USER Packet Pattern in %d(sec) : [%s](%s) : ZoneNullPacket(%d), ZonePacket(%d) QueuedPacketCount(%d) IP(%s) ZONE(%d)\n", 
					(DWORD)(m_dwElapsedRecvPacketMAXTime), 
					this->Get_ACCOUNT(), this->Get_NAME(), 
					m_dwRecvZoneNullPacketCount, 
					m_dwRecvZoneNotNullPacketCount,
					this->GetQueuedPacketCount(),
					this->Get_IP(),
					this->GetZONE()->Get_ZoneNO()
					);
			}
			else
			{
				g_LOG.CS_ODS (0xffff, "[ZONE!=NULL] USER Packet Pattern in %d(sec) : [%s](%s) : ZoneNullPacket(%d), ZonePacket(%d) QueuedPacketCount(%d) IP(%s)\n", 
					(DWORD)(m_dwElapsedRecvPacketMAXTime), 
					this->Get_ACCOUNT(), this->Get_NAME(), 
					m_dwRecvZoneNullPacketCount, 
					m_dwRecvZoneNotNullPacketCount,
					this->GetQueuedPacketCount(),
					this->Get_IP()
					);
			}
		}

		m_dwRecvZoneNullPacketCount = 0;
		m_dwRecvZoneNotNullPacketCount = 0;
		m_dwElapsedRecvPacketTime = GetCurAbsSEC();
	}

	// (20070511:남병철) : 정상적인 로그인을 거쳤는지 확인
	if( m_dwElapsedInvalidateTime == 0 && m_dwInvalidateMAXCount == 0 && m_dwElapsedRecvPacketMAXTime == 0 )
	{
		return IS_HACKING( this, "ILLEGAL LOGIN USER" );
	}

    m_csRecvQ.Lock ();
    {
        m_RecvList.AppendNode( pRecvDATA->m_pNODE );
    }
	m_csRecvQ.Unlock ();

    return true;
}


//-------------------------------------------------------------------------------------------------
/**
 *	사용자의 로그아웃 요청시 처리 하는 함수
 */
int  classUSER::ProcLogOUT ()
{
	classDLLNODE<tagIO_DATA> *pRecvNODE;
	t_PACKETHEADER *pPacket;
	short nTotalPacketLEN;

	// 접속 종료 요구했다.
	if ( this->GetZONE()->GetTimeGetTIME() >= m_dwTimeToLogOUT ) {
		switch( this->m_btWishLogOutMODE ) {
			case LOGOUT_MODE_CHARLIST :
				if ( this->m_pPartyBUFF ) {
					this->m_pPartyBUFF->Sub_PartyUSER( this->m_nPartyPOS );
				}

				this->GetZONE()->Dec_UserCNT ();
				this->GetZONE()->Sub_DIRECT( this );
				// 짤리도록..
				g_pUserLIST->DeleteUSER( this, LOGOUT_MODE_CHARLIST );
				return 1;

			case LOGOUT_MODE_LEFT	   :
				if ( this->m_pPartyBUFF ) {
					this->m_pPartyBUFF->Sub_PartyUSER( this->m_nPartyPOS );
				}
				break;
		} // switch( this->m_btWishLogOutMODE )

		return 0;
	} else 
	if ( LOGOUT_MODE_NET_ERROR == this->m_btWishLogOutMODE ) {
		return 1;
	}

	// 접속 종료 대기중에도 처리해줘야 한데...
	if ( this->Get_HP() > 0 )
		this->Check_PerFRAME ( this->GetZONE()->GetPassTIME() );

	// 취소 패킷이 왔는가 ????
	m_csRecvQ.Lock ();
	{
		pRecvNODE = m_RecvList.GetHeadNode ();

		while( pRecvNODE ) {
			pPacket = (t_PACKETHEADER*)pRecvNODE->DATA.m_pCPacket->m_pDATA;
			do {
				nTotalPacketLEN = this->D_RecvB( pPacket );
				if ( !nTotalPacketLEN ) {
					// 패킷이 변조되어 왔다.
					// 헤킹인가 ???
					m_csRecvQ.Unlock ();
					IS_HACKING( this, "classUSER::Proc( Decode_Recv ... )" );
					return 0;
				}
				
				if ( CLI_LOGOUT_CANCEL == pPacket->m_wType && !this->m_bKickOutFromGUMS ) {
					this->m_btWishLogOutMODE = 0;
					this->m_dwTimeToLogOUT = 0;

					m_RecvList.DeleteNode( pRecvNODE );
					this->Free_RecvIODATA( &pRecvNODE->DATA );
					m_csRecvQ.Unlock ();
					return 1;
				}

				pRecvNODE->DATA.m_dwIOBytes -= nTotalPacketLEN;
				pPacket = (t_PACKETHEADER*)( pPacket->m_pDATA + nTotalPacketLEN );
			} while ( pRecvNODE->DATA.m_dwIOBytes );

			m_RecvList.DeleteNode( pRecvNODE );
			this->Free_RecvIODATA( &pRecvNODE->DATA );
			pRecvNODE = m_RecvList.GetHeadNode ();
		} // while( pRecvNODE ) 
	}
	m_csRecvQ.Unlock ();

	return 1;
}

#define	DB_BACKUP_TIME	( 1000 * 60 * 5 )	// 5 min

//-------------------------------------------------------------------------------------------------
/**
 *	사용자로 부터 받은 패킷 처리 & 케릭터 처리 
 */
int	 classUSER::Proc (void)
{
	// 존안에 있을때 처리되는 함수다...
	if ( this->m_btLogOutMODE || INVALID_SOCKET == this->Get_SOCKET() ) {
		// 접속이 끊겼거나 오류...
		if ( 0 == this->m_dwTimeToLogOUT ) {
			if ( this->m_pPartyBUFF ) {
				this->m_pPartyBUFF->Sub_PartyUSER( this->m_nPartyPOS );
			}

			this->m_btWishLogOutMODE = LOGOUT_MODE_NET_ERROR;

			if ( this->GetZONE()->GetTimeGetTIME() - this->m_dwAttackTIME < LOGOUT_WAIT_SECOND * 1000 ) {
				m_dwTimeToLogOUT = this->GetZONE()->GetTimeGetTIME() + LOGOUT_WAIT_SECOND * 500;
			} else {
				// 바로 접속 종료 가능...
				// m_dwTimeToLogOUT = this->GetZONE()->GetTimeGetTIME();
				return 0;
			}
			return 1;
		}
		return this->ProcLogOUT ();
	}

	if ( this->m_btWishLogOutMODE ) {
		return this->ProcLogOUT ();
	}

	this->HandleWorldPACKET ();

	classDLLNODE<tagIO_DATA> *pRecvNODE;
	t_PACKETHEADER *pPacket;
	short nTotalPacketLEN;

	m_csRecvQ.Lock ();
	{
		pRecvNODE = m_RecvList.GetHeadNode ();

		while( pRecvNODE ) {
			pPacket = (t_PACKETHEADER*)pRecvNODE->DATA.m_pCPacket->m_pDATA;
			do {
				nTotalPacketLEN = this->D_RecvB( pPacket );
				if ( !nTotalPacketLEN ) {
					// 패킷이 변조되어 왔다.
					// 헤킹인가 ???
					m_csRecvQ.Unlock ();
					IS_HACKING( this, "classUSER::Proc( Decode_Recv ... )" );
					return 0;
				}

				switch ( this->Proc_ZonePACKET( (t_PACKET*)pPacket ) ) {
					case RET_SKIP_PROC :
					{
						pRecvNODE->DATA.m_dwIOBytes -= nTotalPacketLEN;

						if ( 0 == pRecvNODE->DATA.m_dwIOBytes ) {
							// 다처리된 패킷...
							m_RecvList.DeleteNode( pRecvNODE );
							this->Free_RecvIODATA( &pRecvNODE->DATA );
						} else {
							pPacket = (t_PACKETHEADER*)( pPacket->m_pDATA + nTotalPacketLEN );
							// 처리하고 남은 부분 다음에 처리 할수 있도록...
							for (WORD wI=0; wI<pRecvNODE->DATA.m_dwIOBytes; wI++) {
								pRecvNODE->DATA.m_pCPacket->m_pDATA[ wI ] = pPacket->m_pDATA[ wI ];
							}
						}
						m_csRecvQ.Unlock ();
						return 1;
					}
					case RET_FAILED :
					{
						// 짜를 넘...
						m_csRecvQ.Unlock ();
						return 0;
					}
				} // switch ( this->Proc_ZonePACKET( (t_PACKET*)pPacket ) )

				pRecvNODE->DATA.m_dwIOBytes -= nTotalPacketLEN;
				pPacket = (t_PACKETHEADER*)( pPacket->m_pDATA + nTotalPacketLEN );
			} while ( pRecvNODE->DATA.m_dwIOBytes );

			m_RecvList.DeleteNode( pRecvNODE );
			this->Free_RecvIODATA( &pRecvNODE->DATA );
			pRecvNODE = m_RecvList.GetHeadNode ();
		} // while( pRecvNODE )
	}

	m_csRecvQ.Unlock ();


#if defined(__N_PROTECT)
	if ( this->GetZONE()->GetTimeGetTIME() - this->m_dwCSASendTime >= 21000 /* 3 * 60 * 1000 */ )//10000 ->21000으로 변경:허재영2005.10.26
	{
		if ( !this->Send_srv_CHECK_AUTH() )
			return 0;
	}
#else
	// 5분동안 보낸 패킷이 전혀 없냐 ???
	if ( this->GetZONE()->GetTimeGetTIME() - SOCKET_KEEP_ALIVE_TIME >= this->Get_CheckTIME() ) {
		return 0;
	}
#endif

#ifdef __KRRE_MILEAGE_ITEM
	Proc_Chk_MileageInv();  //마일리지 인벤체크...
#endif

// 걍 1초마다 유저 HP/MP 정보를 보내준다...
	Send_CHAR_HPMP_INFO();

	if ( this->GetZONE()->GetCurrentTIME() - this->m_dwBackUpTIME >= DB_BACKUP_TIME ) {
		this->m_dwBackUpTIME = this->GetZONE()->GetCurrentTIME();
		g_pThreadSQL->Add_BackUpUSER( this );
	}

	//	HP 확인한다.
	int	Temp_HP = this->Get_HP();
	if ( Temp_HP > 0 )
	{
		this->Check_PerFRAME ( this->GetZONE()->GetPassTIME() );
		//	HP 변동이 있는 경우
		if(Temp_HP != this->Get_HP())
		{
			// 파티에 채력 전송.
			// 파티일 경우 파티원에게 피 전송...
			if ( this->GetPARTY() ) {
				this->GetPARTY()->Change_ObjectIDX( (classUSER*)this );
			}
		}
	}
	
	if ( this->Is_CartGuest() ) {
		if ( this->Get_HP() > 0 && this->m_IngSTATUS.GetFLAGs() ) {
			this->Proc_IngSTATUS( this->GetZONE()->GetPassTIME() );
		}

		if ( 0 == this->m_iLinkedCartObjIDX ) {
			// 카트 드라이버와 함께 워프해서 새로운 존에 들어 왔다
			classUSER *pDriver = (classUSER*)g_pUserLIST->GetSOCKET( this->m_iLinkedCartUsrIDX );
			if ( pDriver && RIDE_MODE_DRIVE == pDriver->GetCur_RIDE_MODE() ) {
				if ( NULL == pDriver->GetZONE() || NULL == pDriver->m_pGroupSECTOR ) {
					// 컴터가 느린가 보다...아직 존에 입장하지 않았음
					return true;
				}

				if ( pDriver->GetZONE() == this->GetZONE() ) {
					this->m_iLinkedCartObjIDX = pDriver->Get_INDEX ();
					pDriver->m_iLinkedCartObjIDX = this->Get_INDEX ();

					this->SetCMD_STOP();
					// send ride on packet to around clients...
					return this->Send_gsv_CART_RIDE( CART_RIDE_ACCEPT, pDriver->Get_INDEX (), this->Get_INDEX(), true );
				} // else 이런경우는 없을듯...
			}
			
			// 1. 운전자의 접속 종료
			// 2. 운전자와 함께 워프하고 존에 입장해 보니 드라이버가 카트에서 내렸다 ??
			// 3. 운전자와 존이 틀리다 
			this->m_btRideMODE = 0;
			this->m_btRideATTR = RIDE_ATTR_NORMAL;
			this->m_iLinkedCartObjIDX = 0;
			this->m_iLinkedCartUsrIDX = 0;

			// send ride off packet to around clients...
			return this->SetCMD_TOGGLE( TOGGLE_TYPE_DRIVE, true ); 
		}

		return true;
	}
	
	// 워프 명령 이후에는 ...
	// Zone이 NULL이 되어 아래 함수 호출시 뻑~~~~~~~
	return CObjCHAR::Proc ();
}

/**
 * \param dwFLAG	:: 상태지속 비트 플래그	
 */
bool  classUSER::Send_gsv_CHARSTATE_CHANGE( DWORD dwFLAG )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_CHARSTATE_CHANGE;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_CHARSTATE_CHANGE );

	pCPacket->m_gsv_CHARSTATE_CHANGE.m_wObjectIDX = this->Get_INDEX();
	pCPacket->m_gsv_CHARSTATE_CHANGE.m_dwFLAG = dwFLAG;

	this->GetZONE()->SendPacketToSectors( this, pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
/// 채팅을 파일에 로그로 남김 ( 대만일 경우에만 )
void classUSER::LogCHAT( const char * szMSG , const char * pDestCHAR , const char * szMsgTYPE )
{
	return;
#ifdef __KCHS_CHATLOG__

	if( !IsTAIWAN() )
		return ;

	if( !szMSG )
		return;
	if( !IsTAIWAN() )
		return;

	char * szACCOUNT = this->Get_ACCOUNT();
	char * szNAME = this->Get_NAME();

	if( szMSG[ 0 ] == '/' )
		szMsgTYPE = "CHEAT";

	if( this->m_dwRIGHT >= RIGHT_TWG ) /// 128 보다 크면.
		::g_ChatGMLOG.QueueString( "[%s] %s\t%s\t%s\t%s\t%s\r\n", 
				szMsgTYPE,
				szACCOUNT ? szACCOUNT : "",
				szNAME ? szNAME : "",
				pDestCHAR ? pDestCHAR : "",
				this->Get_IP(), 
				szMSG  );
	else
		::g_ChatLOG.QueueString( "[%s] %s\t%s\t%s\t%s\t%s\r\n", 
				szMsgTYPE,
				szACCOUNT ? szACCOUNT : "",
				szNAME ? szNAME : "",
				pDestCHAR ? pDestCHAR : "",
				this->Get_IP(), 
				szMSG );
#endif

}


void classUSER::Proc_Chk_MileageInv()        //종량제 아이템 체크만 하는지...여부판단...
{
	
	SYSTEMTIME  CurTime;
	DWORD dwCurTime;

	::GetLocalTime(&CurTime);
	dwCurTime =  classTIME::GetAbsSecond(CurTime);

	if(!m_dwMileageItemChkTime||!m_btMileageItemChkDay)  //첨에 들어온 경우에는 모든 아이템을 체크한다..
	{

		if(Chk_MileageItem(EN_DAYRATE,dwCurTime,FALSE))  //첨에 로그인하면 정량제 아이템만 체크하고 착용정보는 보내지 않는다..
		{
			m_btMileageItemChkDay = CurTime.wDay;   //검사날짜 저장
			m_dwMileageItemChkTime = dwCurTime;       //검사한 시간저장
		}
		return;
	}

	if(dwCurTime - m_dwMileageItemChkTime<60)  //1분마다 체크 1분이 안되었으면 걍리턴..
	{
		return;
	}


	if(m_btMileageItemChkDay != CurTime.wDay)     //1분경과와 동시에.. 날짜도 변경되었다...
	{
		if(Chk_MileageItem_ALL(dwCurTime))           //종량제및 시간제
		{
			m_btMileageItemChkDay = CurTime.wDay;   //검사날짜 저장
			m_dwMileageItemChkTime = dwCurTime;       //검사한 시간저장
		}
		return;
	}
	else
	{
		if(Chk_MileageItem(EN_METERRATE,dwCurTime))   //종량제만.. 체크한다..
		{
			m_dwMileageItemChkTime = dwCurTime;         //검사한 시간저장.
		}
		return;
	}

	return;
}

BOOL classUSER::Chk_MileageItem(BYTE btMileageType,DWORD dwCurTime,BOOL bEquipInfoSend)   //하나의타입만 체크할때..(정량 or 종량)
{
	int i=0;
	BOOL bEquipChange = FALSE;
	DWORD dwCnt=0;

	MileageItem *pItem = m_MileageInv.m_ItemLIST;
	classPACKET  *pPacket = Packet_AllocNLock();

	if(!pPacket)
		return FALSE;

	if(btMileageType == EN_METERRATE)
		dwCnt = MAX_MILEAGE_EQUIP;

	if(btMileageType == EN_DAYRATE)
		dwCnt = MAX_MILEAGE_INV_SIZE;

	pPacket->m_gsv_DESTROY_MILEAGE_ITEM.m_btItemCNT =0;

	do
	{
		if(pItem->GetTYPE()&&pItem->GetMileageType() == btMileageType)
		{
			pItem->SubMeter();                  //SubMeter 에서 마일리지아이템 타입체크하므로 여기서는 할필요없음..

			if(!pItem->ChkPassMileage(dwCurTime))  //
			{
				pPacket->m_gsv_DESTROY_MILEAGE_ITEM.m_sInvITEM[pPacket->m_gsv_DESTROY_MILEAGE_ITEM.m_btItemCNT].Set(i,pItem);
				pPacket->m_gsv_DESTROY_MILEAGE_ITEM.m_btItemCNT+=1;

				if(i<MAX_MILEAGE_EQUIP)         
				{
					bEquipChange = TRUE;      //착용아이템쪽이 변경되었다..
					//착용정보쪽도 수정되어야 한다.....
				}
				//아이템 로그작성...(아이템 삭제전에 반듯이 먼저 로그를 작성한다...)
				//아이템 삭제..
				pItem->Clear();
			}
		}
		pItem++;
		i+=1;	
	} while(i<dwCnt);


	if(pPacket->m_gsv_DESTROY_MILEAGE_ITEM.m_btItemCNT)
	{
		pPacket->m_HEADER.m_wType = GSV_DESTROY_MILEAGE_ITEM;
		pPacket->m_HEADER.m_nSize = sizeof(gsv_DESTROY_MILEAGE_ITEM)+pPacket->m_gsv_DESTROY_MILEAGE_ITEM.m_btItemCNT*sizeof(Set_MileageInv);
		SendPacket(pPacket);
	}

	Packet_ReleaseNUnlock(pPacket);

	if(bEquipInfoSend)
	{
		//사용자의 착용 정보를 다른유저에게도 보낸다...
	}
	return TRUE;

}

BOOL classUSER::Chk_MileageItem_ALL(DWORD dwCurTime,BOOL bEquipInfoSend)                                          //모든시간관련 마일리지 아이템체크 (정량/종량아이템)
{
	int i=0;
	BOOL bEquipChange = FALSE;
	DWORD dwCnt=0;

	MileageItem *pItem = m_MileageInv.m_ItemLIST;
	classPACKET  *pPacket = Packet_AllocNLock();

	if(!pPacket)
		return FALSE;



	pPacket->m_gsv_DESTROY_MILEAGE_ITEM.m_btItemCNT =0;

	do
	{
		if(i<MAX_MILEAGE_EQUIP)          //종량제이면.. 착용부분에서만 삭감한다..
			pItem->SubMeter();                  //SubMeter 에서 마일리지아이템 타입체크하므로 여기서는 할필요없음..

		if(pItem->GetTYPE()&&!pItem->ChkPassMileage(dwCurTime))  //
		{
			pPacket->m_gsv_DESTROY_MILEAGE_ITEM.m_sInvITEM[pPacket->m_gsv_DESTROY_MILEAGE_ITEM.m_btItemCNT].Set(i,pItem);
			pPacket->m_gsv_DESTROY_MILEAGE_ITEM.m_btItemCNT+=1;

			if(i<MAX_MILEAGE_EQUIP)         
			{
				bEquipChange = TRUE;      //착용아이템쪽이 변경되었다..
				//착용정보쪽도 수정되어야 한다.....
			}
			//아이템 로그작성...(아이템 삭제전에 반듯이 먼저 로그를 작성한다...)
			//아이템 삭제..
			pItem->Clear();
			
		}
		pItem++;
		i+=1;	
	} while(i<dwCnt);


	if(pPacket->m_gsv_DESTROY_MILEAGE_ITEM.m_btItemCNT)
	{
		pPacket->m_HEADER.m_wType = GSV_DESTROY_MILEAGE_ITEM;
		pPacket->m_HEADER.m_nSize = sizeof(gsv_DESTROY_MILEAGE_ITEM)+pPacket->m_gsv_DESTROY_MILEAGE_ITEM.m_btItemCNT*sizeof(Set_MileageInv);
		SendPacket(pPacket);
	}

	Packet_ReleaseNUnlock(pPacket);

	if(bEquipInfoSend)
	{
		//사용자의 착용 정보를 다른유저에게도 보낸다...
	}
	return TRUE;
}

BOOL classUSER::Recv_cli_EQUIP_MILEAGE_ITEM( t_PACKET *pPacket)
{


	return this->Change_EQUIP_MILEAGE_ITEM( pPacket->m_cli_EQUIP_ITEM.m_nEquipInvIDX, pPacket->m_cli_EQUIP_ITEM.m_nWeaponInvIDX );

	return TRUE;
}

BOOL classUSER::Recv_cli_USE_MILEAGE_ITEM( t_PACKET *pPacket)
{
	return TRUE;
}

BOOL classUSER::Recv_CLI_EQUIP_RIDE_MILEAGE_ITEM( t_PACKET *pPacket)
{
	return TRUE;
}

bool classUSER::Change_EQUIP_MILEAGE_ITEM (short nEquipInvIDX, short nWeaponInvIDX)
{

	if ( this->Get_ActiveSKILL() ) return true;	// 스킬 케스팅 중일땐 못바꿔
	if ( this->m_IngSTATUS.IsSET( FLAG_ING_IGNORE_ALL ) ) return true;

	if (nEquipInvIDX < M_EQUIP_IDX_FACE_ITEM ||nEquipInvIDX >= M_EQUIP_IDX_WEAPON_R ) 
	{
		return IS_HACKING (this, "Recv_cli_EQUIP_ITEM-1 :: Invalied Equip IDX" ); 
	}

	if ( nWeaponInvIDX && nWeaponInvIDX  < M_EQUIP_IDX_WEAPON_R ||nWeaponInvIDX >= MAX_MILEAGE_EQUIP+INVENTORY_PAGE_SIZE ) 
	{
		return IS_HACKING (this, "Recv_cli_EQUIP_ITEM-2 :: Invalid Weapon Inv IDX" ); 
	}
	

	BYTE btCurCON = this->GetCur_CON();
	BYTE btRecvHP = this->m_btRecoverHP;
	BYTE btRecvMP = this->m_btRecoverMP;

	bool bResult = true;
	// 바뀐 인벤토리 구조 전송..
	classPACKET *pCPacket = Packet_AllocNLock ();
	//////////////////////////////////////////////////////////////////////////
	// 2006.06.12:이성활:pCPacket에 대한 Null 체크
	if (!pCPacket) {
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	pCPacket->m_gsv_SET_MILEAGE_INV.m_btItemCNT = 0;
	MileageItem *pEquipITEM = &m_MileageInv.m_ItemLIST[ nEquipInvIDX ];

	if ( nWeaponInvIDX == 0 )
	{
		// 장비 탈거...
		if ( pEquipITEM->GetTYPE() && pEquipITEM->GetTYPE() < ITEM_TYPE_USE ) 
		{
			short nInvIDX = this->Add_ITEM( *pEquipITEM  );
			if ( nInvIDX > 0 )
			{
				pCPacket->m_gsv_SET_MILEAGE_INV.m_btItemCNT = 2;

				

				pCPacket->m_gsv_SET_MILEAGE_INV.m_sInvITEM[ 0 ].m_btInvIDX = (BYTE)nEquipInvIDX;
				pCPacket->m_gsv_SET_MILEAGE_INV.m_sInvITEM[ 0 ].m_ITEM.Clear();

				pCPacket->m_gsv_SET_MILEAGE_INV.m_sInvITEM[ 1 ].Set(nInvIDX,pEquipITEM);


				// 순서 주의 밑에꺼 호출되면 *pEquipITEM이 삭제된다.
				this->ClearITEM( nEquipInvIDX );

			} 
			else
			{
				// 빈 인벤토리가 없어서 장비를 벗을수 없다...
				goto _RETURN;
			}
		}
		else 
		{
#pragma COMPILE_TIME_MSG( "2004. 7. 16 4차 클베과정에서 몇몇 유저에게 나타나는 현상 임시로 ..." )
			// IS_HACKING( this, "Change_EQUIP_ITEM-1" );	// 뭐냐 ???
			// bResult = false;
			goto _RETURN;
		}
	}
	else 
	{
		// 장비 장착...
		MileageItem WeaponITEM;
		CopyMemory(&WeaponITEM,&m_MileageInv.m_ItemLIST[ nWeaponInvIDX ],sizeof(MileageItem));

		// 일반 장비 & 한손 무기
		pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT = 2;
		CopyMemory(&m_MileageInv.m_ItemLIST[ nWeaponInvIDX ],pEquipITEM,sizeof(MileageItem));
		CopyMemory(pEquipITEM,&WeaponITEM,sizeof(MileageItem));

		// 장비 교환.

		pCPacket->m_gsv_SET_MILEAGE_INV.m_sInvITEM[ 0 ].Set( (BYTE)nEquipInvIDX,&m_MileageInv.m_ItemLIST[ nEquipInvIDX ]);
		pCPacket->m_gsv_SET_MILEAGE_INV.m_sInvITEM[ 0 ].Set( (BYTE)nWeaponInvIDX,&m_MileageInv.m_ItemLIST[ nWeaponInvIDX ]);


	}


	if ( pCPacket->m_gsv_SET_INV_ONLY.m_btItemCNT )
	{

		// 케릭터 상태 업데이트.. 파트는 바뀌지 않는다..
		this->SetPartITEM( nEquipInvIDX ,m_MileageInv.m_ItemEQUIP[nEquipInvIDX]);
		this->Send_gsv_EQUIP_ITEM( nEquipInvIDX);

		pCPacket->m_HEADER.m_wType = GSV_SET_MILEAGE_INV_ONLY;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_SET_MILEAGE_INV ) + pCPacket->m_gsv_SET_MILEAGE_INV.m_btItemCNT * sizeof( Set_MileageInv );
		this->SendPacket( pCPacket );

		if ( this->GetPARTY() )
		{
			// 변경에 의해 옵션이 붙어 회복이 바뀌면 파티원에게 전송.
			if ( btCurCON != this->GetCur_CON()  ||
				btRecvHP != this->m_btRecoverHP ||
				btRecvMP != this->m_btRecoverMP )
			{
					this->m_pPartyBUFF->Change_ObjectIDX( this );
				}
		}
	}

_RETURN :
	Packet_ReleaseNUnlock( pCPacket );

	return bResult;
}



#ifdef __EXPERIENCE_ITEM
int classUSER::ExecuteExpBuff( int iEXP )
{
	// 존 경험치 및 이벤트(GM치트) 경험치 적용
	int iBuffEXP = 0;
	int iTempEXP = 0;
	DWORD dwCurTIME = classTIME::GetCurrentAbsSecond(); //GetZONE()->GetCurrentTIME();

	// (20070312:남병철) : 기본 경험치에 존과 월드 경험치 버프가 들어가므로 여기서는 제거한다.
	//////////// 존EXP
	//////////iTempEXP = ((GetZONE()->Get_Exp_LV()) / 100);
	//////////if( iTempEXP > 1 ) 
	//////////	iBuffEXP += iTempEXP;
	//////////// 이벤트EXP
	//////////iTempEXP = (::Get_WorldEXP() / 100);
	//////////if( iTempEXP > 1 ) 
	//////////	iBuffEXP += iTempEXP;

	//////////// 기본 경험치 확정
	//////////if( iBuffEXP > 1 )
	//////////	iEXP *= iBuffEXP;

	// 마일리지 경험치 아이템 적용
	iTempEXP = this->m_GrowAbility.GetExpBUFF( dwCurTIME ) / 100;
	if( iTempEXP > 1 )
		iEXP *= iTempEXP;

	return iEXP;
}
#endif



//	클랜전 관련 메시지 함수 처리
#ifdef __CLAN_WAR_SET

//	클랜전에 참여할 거냐? 물어본다.
BOOL classUSER::Send_gsv_CLANWAR_OK(DWORD p_Sub_Type,DWORD p_Clan_dw,WORD p_Zone_w,WORD p_Team_w)
{
	//	전달 값 확인
	if(p_Clan_dw == 0) return false;
	if(p_Zone_w == 0) return false;
	if(p_Team_w == 0) return false;
	if(p_Sub_Type == 0) return false;

	//	전송 패킷 메모리 할당 한다.
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )	return false;	// 할당 실패 했다. 쩝 뻑.

	//	전송 패킷의 메시지 타입을 지정한다.
	pCPacket->m_HEADER.m_wType = GSV_CLANWAR_INTO_OK;
	//	패킷의 크기를 계산한다.
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_cli_CLAN_WAR_OK  );

	//	서브 메시지 번호
	pCPacket->m_gsv_cli_ClanWar_OK.m_Sub_Type = p_Sub_Type;
	//	존번호, 팀번호, 길드번호
	pCPacket->m_gsv_cli_ClanWar_OK.m_Clan_N = p_Clan_dw;
	pCPacket->m_gsv_cli_ClanWar_OK.m_Zone_N = p_Zone_w;
	pCPacket->m_gsv_cli_ClanWar_OK.m_Team_N = p_Team_w;

	//	전송 한다. : 만약 여기서 바로 전송하면 문제가 많아진다. : 버퍼링 하는지 확인 요망.
	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

////	타임 메시지 전송
BOOL classUSER::Send_gsv_CLANWAR_Time(DWORD p_Type,DWORD p_Count)
{
	//	전송 패킷 메모리 할당 한다.
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )	return false;	// 할당 실패 했다. 쩝 뻑.

	//	전송 패킷의 메시지 타입을 지정한다.
	pCPacket->m_HEADER.m_wType = GSV_CLANWAR_TIME;
	//	패킷의 크기를 계산한다.
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_cli_CLAN_WAR_TIME  );

	//	전송 번호 넣기
	switch(p_Type)
	{
	case 2:
	case 3:
	case 4:
		pCPacket->m_gsv_cli_ClanWar_Time.m_Play_Type = 1;
		break;
	case 5:
		pCPacket->m_gsv_cli_ClanWar_Time.m_Play_Type = 2;
		break;
	default:
		pCPacket->m_gsv_cli_ClanWar_Time.m_Play_Type = 0;
		break;
	}
	pCPacket->m_gsv_cli_ClanWar_Time.m_Play_Count = p_Count;

	//	전송 한다. : 만약 여기서 바로 전송하면 문제가 많아진다. : 버퍼링 하는지 확인 요망.
	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

//	클랜전에 참여할 거다. OK 메시지 처리
BOOL classUSER::Recv_cli_CLANWAR_OK(t_PACKET *pPacket)
{
	//	전달 값 확인.
	if(pPacket == NULL) return false;

	//	서브 타입에 따라서 값에 따라 달라짐.
	DWORD	Temp_CD = 0;

	//	pPacket->m_gsv_cli_ClanWar_OK.m_Sub_Type
	switch(pPacket->m_gsv_cli_ClanWar_OK.m_Sub_Type)
	{
	case 1:
		{
			//	클랜장 이동 확인 메시지 처리
#ifdef __CLAN_WAR_EXT
			switch( CClanWar::GetInstance()->CheckClanWarUser( this ) )
#else
			switch(GF_ClanWar_Check_myClan(pPacket->m_gsv_cli_ClanWar_OK.m_Zone_N,this->GetClanID()))
#endif
			{
			case 1:
				{
					//	A 팀 신청 //	현재 위치 백업 받을 필요 있음
					//	클랜전 지역으로 이동시킨다.
#ifdef __CLAN_WAR_EXT
					switch( CClanWar::GetInstance()->WarpUserToClanWar( this, CWTT_A_TEAM ) )
#else
					switch(GF_ClanWar_Move((LPVOID)this,pPacket->m_gsv_cli_ClanWar_OK.m_Zone_N,1))
#endif
					{
					case 0:
						return false;
					case 1:
						break;
					case 2:
						//	중지
						return true;
					default:
						return false;
					}

#ifdef __CLAN_WAR_EXT
					// 클랜전 진행에 필요한 메시지 전달
					// dwProcessType [01:마스터에게 클랜전 참여 메시지 전달]
					//               [02:클랜원들에게 클랜전 참여 메시지 전달]
					//               [07:접속한 유저들에게 클랜전 진행 수치 상태 전달]
					//               [10:각자의 맵으로 보낸다. (기본:주논으로 워프)]
					// dwWarpType [1:A 팀 클랜원들에게 클랜전 참여 메시지 전달]
					//            [2:B 팀 클랜원들에게 클랜전 참여 메시지 전달]
					CClanWar::GetInstance()->SendProcessMessage( this, CWMPT_MEMBER_MSG, CWMWT_A_TEAM_WARP );
#else
					//	해당 클랜 유저들 전송 메시지 전달한다.
					/*Temp_CD = GF_Clan_Fine_List(pPacket->m_gsv_cli_ClanWar_OK.m_Clan_N);
					if(Temp_CD > 0)*/
					
                    GF_ClanWar_Join_OK_Send(pPacket->m_gsv_cli_ClanWar_OK.m_Zone_N - 1,2,1);
#endif					

					break;
				}
			case 2:
				{
					//	B 팀 신청 //	현재 위치 백업 받을 필요 있음
					//	클랜전 지역으로 이동시킨다.
#ifdef __CLAN_WAR_EXT
					switch( CClanWar::GetInstance()->WarpUserToClanWar( this, CWTT_B_TEAM ) )
#else
					switch(GF_ClanWar_Move((LPVOID)this,pPacket->m_gsv_cli_ClanWar_OK.m_Zone_N,2))
#endif
					{
					case 0:
						return false;
					case 1:
						break;
					case 2:
						return true;
					default:
						return false;
					}
#ifdef __CLAN_WAR_EXT
					// 클랜전 진행에 필요한 메시지 전달
					// dwProcessType [01:마스터에게 클랜전 참여 메시지 전달]
					//               [02:클랜원들에게 클랜전 참여 메시지 전달]
					//               [07:접속한 유저들에게 클랜전 진행 수치 상태 전달]
					//               [10:각자의 맵으로 보낸다. (기본:주논으로 워프)]
					// dwWarpType [1:A 팀 클랜원들에게 클랜전 참여 메시지 전달]
					//            [2:B 팀 클랜원들에게 클랜전 참여 메시지 전달]
					CClanWar::GetInstance()->SendProcessMessage( this, CWMPT_MEMBER_MSG, CWMWT_B_TEAM_WARP );
#else
					//	해당 클랜 유저들 전송 메시지 전달한다.
					/*Temp_CD = GF_Clan_Fine_List(pPacket->m_gsv_cli_ClanWar_OK.m_Clan_N);
					if(Temp_CD > 0)*/
						GF_ClanWar_Join_OK_Send(pPacket->m_gsv_cli_ClanWar_OK.m_Zone_N - 1,2,2);
#endif
					break;
				}
			default:
				return true;
			}
			break;
		}
	case 2:
		{
			//	클랜워 이동 처리. 이동 처리 메시지 검사. 해당 클랜이 신청했는지.
#ifdef __CLAN_WAR_EXT
			switch( CClanWar::GetInstance()->CheckClanWarUser( this ) )
#else
			switch(GF_ClanWar_Check_myClan(pPacket->m_gsv_cli_ClanWar_OK.m_Zone_N,this->GetClanID()))
#endif
			{
			case 1:
				{
					//	A 팀 신청 // 현재 위치 백업 받을 필요 있음
					//	클랜전 지역으로 이동시킨다.
#ifdef __CLAN_WAR_EXT
					switch( CClanWar::GetInstance()->WarpUserToClanWar( this, CWTT_A_TEAM ) )
#else
					switch(GF_ClanWar_Move((LPVOID)this,pPacket->m_gsv_cli_ClanWar_OK.m_Zone_N,1))
#endif
					{
					case 0:
						return false;
					case 1:
						break;
					case 2:
						return true;
					default:
						return false;
					}
					break;
				}
			case 2:
				{
					//	B 팀 신청 // 현재 위치 백업 받을 필요 있음
					//	클랜전 지역으로 이동시킨다.
#ifdef __CLAN_WAR_EXT
					switch( CClanWar::GetInstance()->WarpUserToClanWar( this, CWTT_B_TEAM ) )
#else
					switch(GF_ClanWar_Move((LPVOID)this,pPacket->m_gsv_cli_ClanWar_OK.m_Zone_N,2))
#endif
					{
					case 0:
						return false;
					case 1:
						break;
					case 2:
						return true;
					default:
						return false;
					};
					break;
				}
			default:
				return true;
			}
			break;
		}
	case 3:
		{
			//	클랜전 다시 입장.
#ifdef __CLAN_WAR_EXT
			switch( CClanWar::GetInstance()->CheckClanWarUser( this ) )
#else
			switch(GF_ClanWar_Check_myClan(pPacket->m_gsv_cli_ClanWar_OK.m_Zone_N,this->GetClanID()))
#endif
			{
			case 1:
				{
					//	A 팀 신청 // 현재 위치 백업 받을 필요 있음
					//	클랜전 지역으로 이동시킨다.
#ifdef __CLAN_WAR_EXT
					switch( CClanWar::GetInstance()->WarpUserToClanWar( this, CWTT_A_TEAM ) )
#else
					switch(GF_ClanWar_Move((LPVOID)this,pPacket->m_gsv_cli_ClanWar_OK.m_Zone_N,1))
#endif
					{
					case 0:
						return false;
					case 1:
						break;
					case 2:
						return true;
					default:
						return false;
					}
					break;
				}
			case 2:
				{
					//	B 팀 신청 // 현재 위치 백업 받을 필요 있음
					//	클랜전 지역으로 이동시킨다.
#ifdef __CLAN_WAR_EXT
					switch( CClanWar::GetInstance()->WarpUserToClanWar( this, CWTT_B_TEAM ) )
#else
					switch(GF_ClanWar_Move((LPVOID)this,pPacket->m_gsv_cli_ClanWar_OK.m_Zone_N,2))
#endif
					{
					case 0:
						return false;
					case 1:
						break;
					case 2:
						return true;
					default:
						return false;
					}
					break;
				}
			default:
				return true;
			}
			break;
		}
	case 100:
		{
			//	게임 포기 메시지 보낸 유저. 클랜장이면 게임 포기 처리.
			//	클랜 마스터 인지 확인.
			if(this->Is_ClanMASTER())
			{
				//	먼저 들어온 팀이 진다. 다음팀은 내부에서
				//	드럽 처리.
#ifdef __CLAN_WAR_EXT

				DWORD dwCreatedRoom = 0;
				// 클라이언트에서 클랜전 거부시 m_Zone_N == 0
				if( pPacket->m_gsv_cli_ClanWar_OK.m_Zone_N == 0 )
				{
					CClanWar* pClanWar = NULL;

					pClanWar = CClanWar::GetInstance();
					dwCreatedRoom = pClanWar->IsCreatedClanWarRoom( this );

					// 클랜전 진행 중이면 상대편 승리
					if( dwCreatedRoom == CWTT_A_TEAM ) // A팀 포기
					{
						// A팀 포기 --> B팀 승리 //
						// 마스터의 기권을 알려준다.
						pClanWar->ClanWarRejoinGiveUp( this, CWTT_B_TEAM );
					}
					else if( dwCreatedRoom == CWTT_B_TEAM ) // B팀 포기
					{
						// B팀 포기 --> A팀 승리 //
						// 마스터의 기권을 알려준다.
						pClanWar->ClanWarRejoinGiveUp( this, CWTT_A_TEAM );
					}
				}
				else
				{
					CClanWar::GetInstance()->DeclareMasterGiveUp( this );
				}
#else
				if(GF_Clan_War_Drop(pPacket->m_gsv_cli_ClanWar_OK.m_Zone_N,pPacket->m_gsv_cli_ClanWar_OK.m_Team_N))
				{
					//	해당 팀의 퀘스트 처리.
					GF_ClanWar_Quest_DropProc(pPacket->m_gsv_cli_ClanWar_OK.m_Zone_N,pPacket->m_gsv_cli_ClanWar_OK.m_Team_N);
					//	상대팀 클랜장에게 상대편 기권승 메시지 전송.
					GF_ClanWar_GiveUP(pPacket->m_gsv_cli_ClanWar_OK.m_Zone_N,pPacket->m_gsv_cli_ClanWar_OK.m_Team_N);
					//->Send_gsv_CLANWAR_Err(CWAR_QUEST_LIST_FULL);
					// 그냥 돈으로 보상
					// ~~
				}
#endif
			}
			break;
		}
	default:
		{
			//	정의 안된 메시지 진입.
			return false;
		}
	}
	return true;
}

////	클랜전 오류 번호 받음
BOOL classUSER::Recv_cli_CLANWAR_Err(t_PACKET *pPacket)
{
#ifdef __CLAN_WAR_EXT
	//CClanWar::GetInstance()
	if( pPacket == NULL )
		return FALSE;

	switch(pPacket->m_gsv_cli_ClanWar_Err.m_CW_Err_w)
	{
	case 3:
		{
			// 클랜전 포기 처리. : 해당 클랜의 클랜 삭제
			//	유저가 클랜을 삭제할 때 보낸다.
			//	유저가 클랜 삭제할 떄 채크 필요하다.
			//GF_User_Out_Clan((LPVOID)this,true);
			break;
		}
	case 4:
		{
			//	클랜 탈퇴 메시지 받음.
			//	등록된 클랜에서 본인 정보 제거 하기
			//GF_User_Out_Clan((LPVOID)this,false);
			break;
		}
	}
#else
	//	전달 값 확인.
	if(pPacket == NULL) return false;
	switch(pPacket->m_gsv_cli_ClanWar_Err.m_CW_Err_w)
	{
	case 3:
		{
			// 클랜전 포기 처리. : 해당 클랜의 클랜 삭제
			//	유저가 클랜을 삭제할 때 보낸다.
			//	유저가 클랜 삭제할 떄 채크 필요하다.
			GF_User_Out_Clan((LPVOID)this,true);
			break;
		}
	case 4:
		{
			//	클랜 탈퇴 메시지 받음.
			//	등록된 클랜에서 본인 정보 제거 하기
			GF_User_Out_Clan((LPVOID)this,false);
			break;
		}
	}
#endif

	return TRUE;
}

////	클랜전 오류 메시지 전송 함수.
BOOL classUSER::Send_gsv_CLANWAR_Err(WORD p_Type_w)
{
	//	전송 패킷 메모리 할당 한다.
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )	return false;	// 할당 실패 했다. 쩝 뻑.

	//	전송 패킷의 메시지 타입을 지정한다.
	pCPacket->m_HEADER.m_wType = GSV_CLANWAR_ERR;
	//	패킷의 크기를 계산한다.
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_cli_CLAN_WAR_ERR);

	//	에러번호 넣는다.
	pCPacket->m_gsv_cli_ClanWar_Err.m_CW_Err_w = p_Type_w;

	//	전송한다.
	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

////	클랜전 진행 메시지 전송 함수 처리.m_gsv_cli_ClanWar_Progress
BOOL classUSER::Send_gsv_CLANWAR_Progress(WORD p_Type_w)
{
	//	전송 패킷 메모리 할당 한다.
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )	return false;	// 할당 실패 했다. 쩝 뻑.

	//	전송 패킷의 메시지 타입을 지정한다.
	pCPacket->m_HEADER.m_wType = GSV_CLANWAR_PROGRESS;
	//	패킷의 크기를 계산한다.
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_cli_CLAN_WAR_Progress);

	//	진행 변수 넣는다.
	pCPacket->m_gsv_cli_ClanWar_Progress.m_CW_Progress = p_Type_w;

	//	전송한다.
	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	return true;
}


#endif

//	유저 중요 메시지 처리 헤더 추가.
#ifdef __MESSAGE_LV_1

//	유저 중요 메시지 처리
BOOL classUSER::Send_gsv_User_MSG_1LV(WORD p_Type,WORD p_Data)
{
	if(p_Type == 0 ) return false;
	//	전송 패킷 메모리 할당 한다.
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )	return false;	// 할당 실패 했다. 쩝 뻑.

	//	전송 패킷의 메시지 타입을 지정한다.
	pCPacket->m_HEADER.m_wType = GSV_USER_MSG_1LV;
	//	패킷의 크기를 계산한다.
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_cli_USER_MSG_1LV );

	//	진행 변수 넣는다.
	pCPacket->m_gsv_User_MSG_1LV.m_Sub_Type = p_Type;
	pCPacket->m_gsv_User_MSG_1LV.m_Data = p_Data;

	//	전송한다.
	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

#endif

//	유저 퀘스트 보상 체크 함수
BOOL	classUSER::mf_Quest_CheckUP(int p_Idx)
{
	//	동일 값이 오면 취소
	if(m_Quest_Get_IDX == p_Idx) return false;
	//	보관.
	m_Quest_Get_IDX = p_Idx;
	return true;
}

//	유저 퀘스트 보상 초기화
BOOL	classUSER::mf_Quest_CheckInit(int p_Idx)
{
	//	버그 있음.
	if(m_Quest_Get_IDX == p_Idx) return false;
	//	초기화
	m_Quest_Get_IDX = 0;
	return true;
}



//-------------------------------------
// 2007.02.13/김대성/추가 - 클라이언트와 서버시간 동기화
#ifdef __ITEM_TIME_LIMIT
BOOL classUSER::Recv_cli_SERVER_TIME( t_PACKET *pPacket)
{
	return send_svr_SERVER_TIME(pPacket->m_cli_SERVER_TIME.m_ClientTime);
}
BOOL classUSER::send_svr_SERVER_TIME(__int64 clientTime)
{
	//	전송 패킷 메모리 할당 한다.
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )	return FALSE;	// 할당 실패 했다. 쩝 뻑.

	//	전송 패킷의 메시지 타입을 지정한다.
	pCPacket->m_HEADER.m_wType = GSV_SERVER_TIME;
	//	패킷의 크기를 계산한다.
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_SERVER_TIME );

	//	진행 변수 넣는다.
	pCPacket->m_gsv_SERVER_TIME.m_ClientTime = clientTime;

	time_t t;
	time(&t);
	pCPacket->m_gsv_SERVER_TIME.m_ServerTime = (__int64)t;

	struct tm *l;
	l = localtime( &t );
	pCPacket->m_gsv_SERVER_TIME.wYear = l->tm_year;
	pCPacket->m_gsv_SERVER_TIME.wMonth = l->tm_mon;
	pCPacket->m_gsv_SERVER_TIME.wDay = l->tm_mday;
	pCPacket->m_gsv_SERVER_TIME.wHour = l->tm_hour;
	pCPacket->m_gsv_SERVER_TIME.wMin = l->tm_min;
	pCPacket->m_gsv_SERVER_TIME.wSec = l->tm_sec;

	//	전송한다.
	this->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return TRUE;
}

short classUSER::Parse_CheatNoGM( char *szCode )
{
	short nProcMODE=0;

	char *pToken, *pArg1, *pArg2, *pArg3;
	char *pDelimiters = " ";

	CStrVAR *pStrVAR = this->GetZONE()->GetStrVAR ();

	pToken = pStrVAR->GetTokenFirst (szCode, pDelimiters);
	pArg1  = pStrVAR->GetTokenNext  (pDelimiters);
	if ( 0 == pToken )
	{
		return CHEAT_INVALID;
	}
	
	if ( _strcmpi( pToken, "/sit" ) )
	{
		return this->Cheat_sit();
	}
}

short classUSER::Cheat_sit()
{
	return CHEAT_INVALID;
}
#endif
//-------------------------------------