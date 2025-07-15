#include "stdAFX.h"
//#if defined(__SHO_WS) || defined(__INC_WORLD)

#ifdef	__SHO_WS
#include "CWS_Client.h"
#include "CWS_Server.h"
#include "CThreadLOG.h"
#else
#include "LIB_gsMAIN.h"
#include "GS_ListUSER.h"
#include "GS_SocketLSV.h"
#include "GS_ThreadLOG.h"
#endif

#include "CThreadGUILD.h"
#include "classLOG.h"
#include "IO_Skill.h"

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
#include "AlphabetCvt/AlphabetCvt.h"
//-------------------------------------

const int CClan::s_iUserLimit[MAX_CLAN_LEVEL+1] = { 15, 15, 20, 25, 30, 36, 43, 50 };
// const int CClan::s_iPosLimit [MAX_CLAN_LEVEL+1] = { 1, 7, 15, 20, 25, 32, 40, 50 };

//-------------------------------------------------------------------------------------------------
CClan::CClan () : CCriticalSection( 4000 )
{
	m_dwClanID = 0;
}
CClan::~CClan ()
{
	this->Free();
}

DWORD CClan::GetCurAbsSEC ()
{
	return classTIME::GetCurrentAbsSecond();
}

//-------------------------------------------------------------------------------------------------
// 클랜 해체...
void CClan::Disband ()
{
	CWS_Client *pUSER;
	CDLList< CClanUSER >::tagNODE *pNode;
	this->Lock ();
	{
		pNode = m_ListUSER.GetHeadNode();
		while( pNode ) {
			if ( pNode->m_VALUE.m_iConnSockIDX ) {
				pUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pNode->m_VALUE.m_iConnSockIDX );
				if ( pUSER && pUSER->GetClanID() == this->m_dwClanID ) {
					pUSER->ClanINIT( 0 );		// 클렌 삭제..
					pUSER->Send_wsv_CLAN_COMMAND( RESULT_CLAN_DESTROYED, NULL );
#ifdef	__SHO_WS
					// GS에 통보
					g_pListSERVER->Send_zws_DEL_USER_CLAN( pUSER );
#endif
				}
			}

			pNode = pNode->GetNext ();
		}
	}
	this->Unlock ();
}

//-------------------------------------------------------------------------------------------------
bool CClan::Send_MemberSTATUS( CClanUSER *pClanUser, BYTE btResult )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	tag_CLAN_MEMBER sMember;

	sMember.m_btChannelNO		= pClanUser->m_btChannelNo;
	sMember.m_btClanPOS			= pClanUser->m_iPosition;
	sMember.m_iClanCONTRIBUTE	= pClanUser->m_iContribute;
	sMember.m_nLEVEL			= pClanUser->m_nLevel;
	sMember.m_nJOB				= pClanUser->m_nJob;

	pCPacket->m_HEADER.m_wType = WSV_CLAN_COMMAND;
	pCPacket->m_HEADER.m_nSize = sizeof( wsv_CLAN_COMMAND );
	pCPacket->m_wsv_CLAN_COMMAND.m_btRESULT = btResult;// RESULT_CLAN_MEMBER_STATUS;

	pCPacket->AppendData( &sMember, sizeof(tag_CLAN_MEMBER) );
	pCPacket->AppendString( pClanUser->m_Name.Get() );

	this->SendPacketToMEMBER( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}
//-------------------------------------------------------------------------------------------------
// 클랜 유저가 로그인 했다.
bool CClan::LogIn_ClanUSER( char *szCharName, int iSenderSockIDX, int iContribute )
{
	t_HASHKEY HashName = ::StrToHashKey ( szCharName );

	CDLList< CClanUSER >::tagNODE *pNode;
	this->Lock ();
	{
		pNode = m_ListUSER.GetHeadNode();
		while( pNode ) {
			if ( HashName == pNode->m_VALUE.m_HashName && !_strcmpi(szCharName, pNode->m_VALUE.m_Name.Get() ) ) {
				// Find..
				CWS_Client *pUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( iSenderSockIDX );
				if ( pUSER ) 
				{
					pNode->m_VALUE.m_iContribute = iContribute;	// 전체 통보 보다 앞서게...
#ifdef	__SHO_WS
					pNode->m_VALUE.m_btChannelNo = pUSER->Get_ChannelNO();
#else
					pNode->m_VALUE.m_btChannelNo = (BYTE)::Get_ServerChannelNO();
#endif
					// 로그인된 전체 클랜원에게 로그인 했다 통보...
					this->Send_MemberSTATUS( &pNode->m_VALUE, RESULT_CLAN_MEMBER_LOGIN );

					pNode->m_VALUE.m_iConnSockIDX = pUSER->m_iSocketIDX;

					pUSER->SetClanCONTRIBUTE( iContribute );
					pUSER->SetClanPOS( pNode->m_VALUE.m_iPosition );

					pUSER->SetClanID( this->m_dwClanID );
					pUSER->SetClanMARK( this->m_dwClanMARK );

					pUSER->SetClanLEVEL( this->m_nClanLEVEL );
					pUSER->SetClanSCORE( this->m_iClanSCORE );

					pUSER->SetClanUserCNT( this->m_ListUSER.GetNodeCount() );
					pUSER->SetClanMONEY( this->m_biClanMONEY );
					pUSER->SetClanRATE( this->m_iClanRATE );

					::CopyMemory( pUSER->m_CLAN.m_ClanBIN.m_pDATA, this->m_ClanBIN.m_pDATA, sizeof( tagClanBIN ) );

					pUSER->SetClanNAME( this->m_Name.Get() );


#ifdef	__CLAN_INFO_ADD
					//	랭크 포인트 적용.
					pUSER->m_CLAN.m_iRankPoint = this->m_iRankPoint;
#endif


#ifdef __CLAN_REWARD_POINT
					// (20070319:남병철) : 보상 포인트는 클랜 로딩할때 CClanUSER에 각 유저별로 로딩했음
					pUSER->m_CLAN.m_iRewardPoint = pNode->m_VALUE.m_iRewardPoint;
#endif

					this->Send_SetCLAN ( RESULT_CLAN_MY_DATA, pUSER );

					this->Unlock ();
					return true;
				}
				break;
			}
			pNode = pNode->GetNext ();
		}
	}
	this->Unlock ();
	return false;
}
// 길드 유저가 로그아웃 했다.
bool CClan::LogOut_ClanUSER( char *szCharName )
{
	t_HASHKEY HashName = ::StrToHashKey ( szCharName );

	CDLList< CClanUSER >::tagNODE *pNode;
	this->Lock ();
	{
		pNode = m_ListUSER.GetHeadNode();
		while( pNode ) {
			if ( HashName == pNode->m_VALUE.m_HashName && !_strcmpi(szCharName, pNode->m_VALUE.m_Name.Get() ) ) {
				// Find..
				pNode->m_VALUE.m_iConnSockIDX = 0;
				pNode->m_VALUE.m_btChannelNo  = 0xff;

				// 로그인된 전체 클랜원에게 로그아웃 했다 통보...
				this->Send_MemberSTATUS( &pNode->m_VALUE, RESULT_CLAN_MEMBER_LOGOUT );

				this->Unlock ();
				return true;
			}
			pNode = pNode->GetNext ();
		}
	}
	this->Unlock ();
	return false;
}

//-------------------------------------------------------------------------------------------------
BYTE CClan::FindClanSKILL (short nSkillNo1, short nSkillNo2)
{
#ifdef	MAX_CLAN_SKILL_SLOT
	for (short nI=0; nI<MAX_CLAN_SKILL_SLOT; nI++) {
		if ( this->m_ClanBIN.m_SKILL[ nI ].m_nSkillIDX >= nSkillNo1 &&
			this->m_ClanBIN.m_SKILL[ nI ].m_nSkillIDX <= nSkillNo2 ) {
				if ( SKILL_NEED_LEVELUPPOINT( this->m_ClanBIN.m_SKILL[ nI ].m_nSkillIDX ) ) {
					// 만료 날짜 체크...
					if ( this->GetCurAbsSEC() >= this->m_ClanBIN.m_SKILL[ nI ].m_dwExpiredAbsSEC ) {
						this->m_ClanBIN.m_SKILL[ nI ].m_nSkillIDX = 0;
						this->m_ClanBIN.m_SKILL[ nI ].m_dwExpiredAbsSEC = 0;
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
bool CClan::AddClanSKILL (short nSkillNo)
{
#ifdef	MAX_CLAN_SKILL_SLOT
	if ( MAX_CLAN_SKILL_SLOT == this->FindClanSKILL(nSkillNo,nSkillNo) ) {
		// 이미등록된 스킬이 아니다...
		tagClanBIN ClanBIN;
		::CopyMemory( &ClanBIN, &this->m_ClanBIN, sizeof( tagClanBIN ) );

		for (short nI=0; nI<MAX_CLAN_SKILL_SLOT; nI++) {
			if ( 0 == ClanBIN.m_SKILL[ nI ].m_nSkillIDX ) {
				ClanBIN.m_SKILL[ nI ].m_nSkillIDX = nSkillNo;
				// 현재시간 + 입력된 수치 * 10분까지 사용 가능..
				if ( SKILL_NEED_LEVELUPPOINT(nSkillNo) ) {
					ClanBIN.m_SKILL[ nI ].m_dwExpiredAbsSEC = this->GetCurAbsSEC() + SKILL_NEED_LEVELUPPOINT(nSkillNo) * 10 * 60;
				} else {
					ClanBIN.m_SKILL[ nI ].m_dwExpiredAbsSEC = 0;
				}

				// 월드섭이나 개인섭에서만 이함수가 호출된다...
				if ( !g_pThreadGUILD->Query_UpdateClanBINARY( this->m_dwClanID, (BYTE*)&ClanBIN, sizeof( tagClanBIN ) ) ) {
					return false;
				}
				//skill log...
				g_pThreadLOG->When_ws_CLAN( "Clan master", "?", "?", this, NEWLOG_CLAN_ADD_SKILL_DONE, nSkillNo );

				this->m_ClanBIN.m_SKILL[ nI ] = ClanBIN.m_SKILL[ nI ];
				this->Send_UpdateInfo ();

				//#ifdef	__SHO_WS
				//				this->m_ClanBIN.m_SKILL[ nI ] = ClanBIN.m_SKILL[ nI ];
				//				this->Send_UpdateInfo ();
				//#else
				//				if ( g_pThreadGUILD->Query_UpdateClanBINARY( this->m_dwClanID, (BYTE*)&ClanBIN, sizeof( tagClanBIN ) ) ) {
				//					this->m_ClanBIN.m_SKILL[ nI ] = ClanBIN.m_SKILL[ nI ];
				//			#ifdef	__INC_WORLD		// 개인서버면...
				//					this->Send_UpdateInfo ();
				//			#endif
				//				} else 
				//					return false;
				//#endif
				return true;
			}
		}
	}
#endif

	return false;
}
bool CClan::DelClanSKILL (short nSkillNo)
{
#ifdef	MAX_CLAN_SKILL_SLOT
	tagClanBIN ClanBIN;
	::CopyMemory( &ClanBIN, &this->m_ClanBIN, sizeof( tagClanBIN ) );

	for (short nI=0; nI<MAX_CLAN_SKILL_SLOT; nI++) {
		if ( ClanBIN.m_SKILL[ nI ].m_nSkillIDX == nSkillNo ) {
			ClanBIN.m_SKILL[ nI ].m_nSkillIDX = 0;
			ClanBIN.m_SKILL[ nI ].m_dwExpiredAbsSEC = 0;

			// 월드섭이나 개인섭에서만 이함수가 호출된다...
			if ( !g_pThreadGUILD->Query_UpdateClanBINARY( this->m_dwClanID, (BYTE*)&ClanBIN, sizeof( tagClanBIN ) ) ) {
				return false;
			}
			//skill log...
			g_pThreadLOG->When_ws_CLAN( "Clan master", "?", "?", this, NEWLOG_CLAN_DEL_SKILL, nSkillNo );

			this->m_ClanBIN.m_SKILL[ nI ] = ClanBIN.m_SKILL[ nI ];
			this->Send_UpdateInfo ();

			//#ifdef	__SHO_WS
			//			// send to all clan members
			//			this->m_ClanBIN.m_SKILL[ nI ] = ClanBIN.m_SKILL[ nI ];
			//			this->Send_UpdateInfo ();
			//#else
			//			if ( g_pThreadGUILD->Query_UpdateClanBINARY( this->m_dwClanID, (BYTE*)&ClanBIN, sizeof( tagClanBIN ) ) ) {
			//				this->m_ClanBIN.m_SKILL[ nI ] = ClanBIN.m_SKILL[ nI ];
			//		#ifdef	__INC_WORLD		// 개인서버면...
			//				this->Send_UpdateInfo ();
			//		#endif
			//			} else
			//				return false;
			//#endif
			return true;
		}
	}
#endif
	return false;
}

void CClan::SetJOBnLEV( t_HASHKEY HashCHAR, char *szCharName, short nJob, short nLev )
{
	t_HASHKEY HashName = ::StrToHashKey ( szCharName );

	CDLList< CClanUSER >::tagNODE *pNode;
	this->Lock ();
	{
		pNode = m_ListUSER.GetHeadNode();
		while( pNode ) {
			if ( HashName == pNode->m_VALUE.m_HashName && !_strcmpi(szCharName, pNode->m_VALUE.m_Name.Get() ) ) {
				// Find..
				pNode->m_VALUE.m_nLevel = nLev;
				pNode->m_VALUE.m_nJob = nJob;

				classPACKET *pCPacket = Packet_AllocNLock ();
				if ( pCPacket ) {
					pCPacket->m_HEADER.m_wType = WSV_CLAN_COMMAND;
					pCPacket->m_HEADER.m_nSize = sizeof( wsv_CLAN_MEMBER_JOBnLEV );
					pCPacket->m_wsv_CLAN_MEMBER_JOBnLEV.m_btRESULT = RESULT_CLAN_MEMBER_JOBnLEV;

					pCPacket->m_wsv_CLAN_MEMBER_JOBnLEV.m_nLEVEL = nLev;
					pCPacket->m_wsv_CLAN_MEMBER_JOBnLEV.m_nJOB   = nJob;
					pCPacket->AppendString( szCharName );

					this->SendPacketToMEMBER( pCPacket );

					Packet_ReleaseNUnlock( pCPacket );
				}

				this->Unlock ();
				return;
			}
			pNode = pNode->GetNext ();
		}
	}
	this->Unlock ();
}

//-------------------------------------------------------------------------------------------------
//bool CClan::Send_AdjCLAN ()
//{
////RESULT_CLAN_INFO xxx
//}

bool CClan::Send_SetCLAN (BYTE btCMD, CWS_Client *pMember )
{
#ifdef	__SHO_WS
	// GS에 사용자 클랜 설정 통보...
	g_pListSERVER->Send_zws_SET_USER_CLAN( pMember );
#endif

	if ( this->m_Motd.Get() ) {
		if ( this->m_Desc.Get() ) {
			pMember->Send_wsv_CLAN_COMMAND( btCMD, 
				sizeof( tag_MY_CLAN ), &pMember->m_CLAN, 
				this->m_Name.BuffLength()+1, this->m_Name.Get(),
				this->m_Desc.BuffLength()+1, this->m_Desc.Get(),
				this->m_Motd.BuffLength()+1, this->m_Motd.Get(), 
				NULL ); 
		} else {
			pMember->Send_wsv_CLAN_COMMAND( btCMD, 
				sizeof( tag_MY_CLAN ), &pMember->m_CLAN, 
				this->m_Name.BuffLength()+1, this->m_Name.Get(), 
				1, " ",
				this->m_Motd.BuffLength()+1, this->m_Motd.Get(), 
				NULL ); 
		}
	} else {
		if ( this->m_Desc.Get() ) {
			pMember->Send_wsv_CLAN_COMMAND( btCMD, 
				sizeof( tag_MY_CLAN ), &pMember->m_CLAN, 
				this->m_Name.BuffLength()+1, this->m_Name.Get(), 
				this->m_Desc.BuffLength()+1, this->m_Desc.Get(),
				NULL ); 
		} else {
			pMember->Send_wsv_CLAN_COMMAND( btCMD, 
				sizeof( tag_MY_CLAN ), &pMember->m_CLAN, 
				this->m_Name.BuffLength()+1, this->m_Name.Get(), 
				1, " ",
				NULL ); 
		}
	} 

	return true;
}

//-------------------------------------------------------------------------------------------------
// 길드 유저 추가...
bool CClan::Insert_MEMBER( BYTE btResult, CWS_Client *pMember, int iClanPos, char *szMaster )
{
	CDLList< CClanUSER >::tagNODE *pNode;

	this->Lock ();
	{
		if ( m_ListUSER.GetNodeCount() < this->GetUserLimitCNT() ) {
			if ( g_pThreadGUILD->Query_InsertClanMember( pMember->Get_NAME(), this->m_dwClanID, iClanPos ) ) {

				// 클랜 가입 로그...
				g_pThreadLOG->When_ws_CLAN( pMember->Get_NAME(), pMember->Get_IP(), szMaster, this, NEWLOG_CLAN_JOIN_MEMBER ); 

				pNode = m_ListUSER.AllocNAppend ();

				pNode->m_VALUE.m_Name.Set( pMember->Get_NAME() );
				pNode->m_VALUE.m_HashName = ::StrToHashKey ( pNode->m_VALUE.m_Name.Get() );
				pNode->m_VALUE.m_iPosition = iClanPos;
				pNode->m_VALUE.m_iConnSockIDX = pMember->m_iSocketIDX;
#ifdef	__SHO_WS
				pNode->m_VALUE.m_btChannelNo = pMember->Get_ChannelNO();
#else
				pNode->m_VALUE.m_btChannelNo = (BYTE)::Get_ServerChannelNO();
#endif

				this->m_nPosCNT[ iClanPos ] ++;			// 신규 멤버 추가

				pMember->ClanINIT( this->m_dwClanID, iClanPos );
				pMember->SetClanMARK( this->m_dwClanMARK );
				pMember->SetClanNAME( this->m_Name.Get() );

				pMember->SetClanLEVEL( this->m_nClanLEVEL );
				pMember->SetClanSCORE( this->m_iClanSCORE );

				pMember->SetClanUserCNT( this->m_ListUSER.GetNodeCount() );
				pMember->SetClanMONEY( this->m_biClanMONEY );
				pMember->SetClanRATE( this->m_iClanRATE );

				::CopyMemory( pMember->m_CLAN.m_ClanBIN.m_pDATA, this->m_ClanBIN.m_pDATA, sizeof( tagClanBIN ) );


#ifdef	__CLAN_INFO_ADD
				//	랭크 포인트 적용.
				pMember->m_CLAN.m_iRankPoint = this->m_iRankPoint;
				// (20070413:남병철) : 클랜원 추가할때 보상 포인트는 초기화!
				pMember->m_CLAN.m_iRewardPoint = 0;
#endif

				this->Send_SetCLAN( btResult, pMember );


				if ( szMaster ) {
					this->Send_wsv_CLAN_COMMAND( RESULT_CLAN_JOIN_MEMBER, 
						pMember->m_Name.BuffLength()+1, pMember->Get_NAME(),
						strlen( szMaster ) + 1,		szMaster, 
						NULL );
				} // else 창설됨과 동시에 추가 되는 창설자

				this->Unlock ();
				return true;
			}
		}
	}
	this->Unlock ();
	return false;
}

// 길드 유저 삭제...
bool CClan::Delete_MEMBER( t_HASHKEY HashCommander, char *szCharName, BYTE btCMD, char *szKicker )
{
	// 1. 접속해 있으면 길드 삭제 -> GS통보
	// 2. 길드원 수 감소.
	// 3. 길드 멤버 DB 삭제.
	t_HASHKEY HashName = ::StrToHashKey ( szCharName );
	CDLList< CClanUSER >::tagNODE *pNode;
	this->Lock ();
	{
		pNode = m_ListUSER.GetHeadNode();
		while( pNode ) {
			if ( HashName == pNode->m_VALUE.m_HashName && !_strcmpi(szCharName, pNode->m_VALUE.m_Name.Get() ) ) {
				if ( HashName == HashCommander ) {
					// 자기 자신은 안돼 !!!!
					this->Unlock ();
					return false;
				}

				// 동료 길드원에게 통보~~~
				if ( szKicker ) {
					this->Send_wsv_CLAN_COMMAND( btCMD, strlen(szCharName)+1, szCharName, strlen(szKicker)+1, szKicker, NULL );
				} else {
					this->Send_wsv_CLAN_COMMAND( btCMD, strlen(szCharName)+1, szCharName, NULL );
				}

				// Find..
				// szCharName이 접속된 상태라면 GS에 통보 및 클랜 삭제통보
				CWS_Client *pUSER = g_pUserLIST->Find_CHAR( szCharName );
				if ( pUSER )
				{
					// 보내고 초기화...
					pUSER->ClanINIT ();
#ifdef	__SHO_WS
					// GS에 통보
					g_pListSERVER->Send_zws_DEL_USER_CLAN( pUSER );
#endif
				}

				if ( g_pThreadGUILD->Query_DeleteClanMember( szCharName ) > -2 ) 
				{
					this->m_nPosCNT[ pNode->m_VALUE.m_iPosition ] --;		// 멤버 삭제

					m_ListUSER.DeleteNFree( pNode );
				}
				this->Unlock ();
				return true;
			}
			pNode = pNode->GetNext ();
		}
	}
	this->Unlock ();
	return false;
}

//-------------------------------------------------------------------------------------------------
bool CClan::Adjust_MEMBER( t_HASHKEY HashCommander, char *szCharName, int iAdjContr, int iAdjPos, char *szMaster, int iMaxPos, int iMinPos, bool bCheckPosLimit )
{
	t_HASHKEY HashName = ::StrToHashKey ( szCharName );
	bool bResult = false;

	CDLList< CClanUSER >::tagNODE *pNode;
	this->Lock ();
	{
		pNode = m_ListUSER.GetHeadNode();
		while( pNode ) {
			if ( HashName == pNode->m_VALUE.m_HashName && !_strcmpi(szCharName, pNode->m_VALUE.m_Name.Get() ) ) {
				if ( HashName == HashCommander ) {
					// 자기 자신은 안돼 !!!!
					goto _JUMP_RETURN;
				}
				if ( iMinPos && pNode->m_VALUE.m_iPosition < iMinPos ) {
					// 최하 자격은 iMinPos되야함~~~
					goto _JUMP_RETURN;
				}

				// Find..
				if ( pNode->m_VALUE.m_iPosition + iAdjPos > GPOS_MASTER )
					iAdjPos = GPOS_MASTER - pNode->m_VALUE.m_iPosition;
				if ( iAdjPos > iMaxPos ) {
					goto _JUMP_RETURN;
				}

				if ( pNode->m_VALUE.m_iPosition + iAdjPos >= 0 ) {
					if ( bCheckPosLimit ) {
						// 등급별 최대 인원 체크...
						int iNewPos = pNode->m_VALUE.m_iPosition+iAdjPos;
						if ( this->m_nPosCNT[ iNewPos ] >= this->GetPositionLimitCNT(iNewPos) ) {
							goto _JUMP_RETURN;
						}
					}

					if ( g_pThreadGUILD->Query_AdjustClanMember( szCharName, iAdjContr, iAdjPos ) ) {
						// 직위 변경 로그...
						g_pThreadLOG->When_ws_CLAN( szCharName, "?", szMaster, this, NEWLOG_CLAN_CHANGE_POSITION, pNode->m_VALUE.m_iPosition, pNode->m_VALUE.m_iPosition + iAdjPos);

						this->m_nPosCNT[ pNode->m_VALUE.m_iPosition ] --;		// 직위 이동 : 해당 직위의 인원 감소
						// 대상 유저의 직위 증가.
						pNode->m_VALUE.m_iContribute += iAdjContr;
						pNode->m_VALUE.m_iPosition += iAdjPos;
						bResult = true;

						this->m_nPosCNT[ pNode->m_VALUE.m_iPosition ] ++;		// 직위 이동 : 변경된 직위의 인원 증가.

						// 캡틴 이상으로 승격됐으면.. 전길드원에 통보
						this->Send_MemberSTATUS( &pNode->m_VALUE, RESULT_CLAN_MEMBER_POSITION );

						// szCharName이 접속된 상태라면
						// TODO:: GS에 통보 및 클랜 삭제통보
						CWS_Client *pUSER = g_pUserLIST->Find_CHAR( szCharName );
						if ( pUSER ) {
							pUSER->SetClanPOS( pNode->m_VALUE.m_iPosition );
							BYTE btPos = (BYTE)pNode->m_VALUE.m_iPosition;
							//	해당 유저에게 전송
							pUSER->Send_wsv_CLAN_COMMAND( RESULT_CLAN_POSITION, strlen(szMaster)+1, (BYTE*)szMaster, sizeof(BYTE), &btPos, NULL );
							//	GS통보
							this->Send_SetCLAN ( RESULT_CLAN_MY_DATA, pUSER );
						}
					}
				}
//////////////////////////////////////////////////////////////////////////
// 2006.06.12:이성활:goto _JUMP_RETURN으로 바로 갈 경우 Lock해제가 되지 않는다. 따라서 순서가 변경되어야 함
//				this->Unlock ();
//_JUMP_RETURN :
_JUMP_RETURN :
				this->Unlock ();
//////////////////////////////////////////////////////////////////////////

				return bResult;
			}
			pNode = pNode->GetNext ();
		}
	}
	this->Unlock ();

	return false;
}

//-------------------------------------------------------------------------------------------------
#ifdef __CLAN_REWARD_POINT
int CClan::Adjust_MEMBER_SetRewardPoint( int iRewardPoint )
{
	int iResult = 0;
	bool bResult = false;
	CDLList< CClanUSER >::tagNODE *pNode;


	this->Lock ();
	{
		pNode = m_ListUSER.GetHeadNode();

		while( pNode )
		{
			// DB 업데이트
			if( pNode->m_VALUE.m_Name.Get() != NULL )
			{
				if ( g_pThreadGUILD->Query_AdjustClanMember_RewardPoint( /*SET*/0, pNode->m_VALUE.m_Name.Get(), iRewardPoint, iResult ) )
				{
					pNode->m_VALUE.m_iRewardPoint = iResult;

					// szCharName이 접속된 상태라면
					// TODO:: GS에 통보 및 클랜 삭제통보
					CWS_Client *pUSER = g_pUserLIST->Find_CHAR( pNode->m_VALUE.m_Name.Get() );
					if ( pUSER )
					{
						//	해당 유저에게 전송
						//pUSER->Send_wsv_CLAN_COMMAND( RESULT_CLAN_POSITION, strlen(szMaster)+1, (BYTE*)szMaster, sizeof(BYTE), &btPos, NULL );
						pUSER->m_CLAN.m_iRewardPoint = iResult;
						//	GS통보
						this->Send_SetCLAN ( RESULT_CLAN_MY_DATA, pUSER );
					}
				}
			}

			pNode = pNode->GetNext ();
		}
	}
	this->Unlock ();

	return 0;
}

int CClan::Adjust_MEMBER_AddRewardPoint( int iRewardPoint, char* pClanWarJoinNames )
{
	bool bResult = false;
	CDLList< CClanUSER >::tagNODE *pNode;
	int iResult = 0;
	ZString zClanWarNames, zTemp;
	ZStringList zslCWNameList;


	// 클랜전 참여자 리스트
	if( pClanWarJoinNames )
	{
		// 이름 리스트가 전달 되었으면 리스트로 만든다.
		zClanWarNames = pClanWarJoinNames;
		zClanWarNames.Trim();
		while( zClanWarNames.IsEmpty() == FALSE )
		{
			zTemp = zClanWarNames.Token( " " );
			if( zTemp.IsEmpty() == FALSE )
			{
				zslCWNameList.Add( zTemp );
				zTemp = "";
			}
		}
	}


	this->Lock ();
	{
		pNode = m_ListUSER.GetHeadNode();

		while( pNode )
		{
			// (20070320:남병철) : 클랜전 참여자에게만 적용할 것인가?
			if( pClanWarJoinNames )
			{
				// DB 업데이트
				if( zslCWNameList.Find( pNode->m_VALUE.m_Name.Get() ) != -1 )
				{
					if ( g_pThreadGUILD->Query_AdjustClanMember_RewardPoint( /*ADD*/1, pNode->m_VALUE.m_Name.Get(), iRewardPoint, iResult ) )
					{
						pNode->m_VALUE.m_iRewardPoint = iResult;

						// szCharName이 접속된 상태라면
						// TODO:: GS에 통보 및 클랜 삭제통보
						CWS_Client *pUSER = g_pUserLIST->Find_CHAR( pNode->m_VALUE.m_Name.Get() );
						if ( pUSER )
						{
							//	해당 유저에게 전송
							//pUSER->Send_wsv_CLAN_COMMAND( RESULT_CLAN_POSITION, strlen(szMaster)+1, (BYTE*)szMaster, sizeof(BYTE), &btPos, NULL );
							pUSER->m_CLAN.m_iRewardPoint = iResult;
							//	GS통보
							this->Send_SetCLAN ( RESULT_CLAN_MY_DATA, pUSER );
						}
					}
				}
			}
			else
			{
				// DB 업데이트
				if( pNode->m_VALUE.m_Name.Get() != NULL )
				{
					if ( g_pThreadGUILD->Query_AdjustClanMember_RewardPoint( /*ADD*/1, pNode->m_VALUE.m_Name.Get(), iRewardPoint, iResult ) )
					{
						pNode->m_VALUE.m_iRewardPoint = iResult;

						// szCharName이 접속된 상태라면
						// TODO:: GS에 통보 및 클랜 삭제통보
						CWS_Client *pUSER = g_pUserLIST->Find_CHAR( pNode->m_VALUE.m_Name.Get() );
						if ( pUSER )
						{
							//	해당 유저에게 전송
							//pUSER->Send_wsv_CLAN_COMMAND( RESULT_CLAN_POSITION, strlen(szMaster)+1, (BYTE*)szMaster, sizeof(BYTE), &btPos, NULL );
							pUSER->m_CLAN.m_iRewardPoint = iResult;
							//	GS통보
							this->Send_SetCLAN ( RESULT_CLAN_MY_DATA, pUSER );
						}
					}
				}
			}

			pNode = pNode->GetNext ();
		}

	}
	this->Unlock ();

	return 0;
}
#endif
//-------------------------------------------------------------------------------------------------
// 길드 유저 갱신...
bool CClan::Update_MEMBER ()
{
	return true;
}

//-------------------------------------------------------------------------------------------------
void CClan::Send_UpdateInfo ()
{
	tag_MY_CLAN sInfo;

	sInfo.m_biClanMONEY	= this->m_biClanMONEY;
	sInfo.m_btClanLEVEL = (BYTE)this->m_nClanLEVEL;
	sInfo.m_dwClanID	= this->m_dwClanID;
	sInfo.m_dwClanMARK	= this->m_dwClanMARK;
	sInfo.m_iClanRATE	= this->m_iClanRATE;
	sInfo.m_iClanSCORE	= this->m_iClanSCORE;

#ifdef	__CLAN_INFO_ADD
	//	랭크 포인트 적용.
	sInfo.m_iRankPoint	= this->m_iRankPoint;
#endif

	::CopyMemory( sInfo.m_ClanBIN.m_pDATA, this->m_ClanBIN.m_pDATA, sizeof( tagClanBIN ) );

	//	sInfo.m_nMemberCNT	= 0;
	//	sInfo.m_btClanPOS	= xxx;
	//	sInfo.m_iClanCONT	= xxx;

	// 전체 클랜 멤버에 전송...
	this->Send_wsv_CLAN_COMMAND ( RESULT_CLAN_INFO, sizeof(tag_MY_CLAN), &sInfo, NULL );

	// 각 채널서버에 갱신된 내용을 전달해야해~~~~
	CDLList< CClanUSER >::tagNODE *pNODE;
	CWS_Client *pUSER;
	this->Lock ();
	{
		/*
		struct tag_CLAN_ID {
		DWORD	m_dwClanID;
		union {
		DWORD	m_dwClanMARK;
		WORD	m_wClanMARK[2];
		} ;
		BYTE	m_btClanLEVEL;		// 클랜 레벨
		BYTE	m_btClanPOS;		// 클랜에서의 직위
		} ;
		struct tag_MY_CLAN : public tag_CLAN_ID {
		int		m_iClanSCORE;		// 클랜 포인트
		int		m_iClanRATE;		// 창고효율
		__int64	m_biClanMONEY;
		short	m_nMemberCNT;		// 멤버 수
		int		m_iClanCONT;		// 클랜 기여도
		// char m_szClanName[];
		// char m_szClanDESC[];
		// char m_szClanMOTD[];
		} ;
		*/
		pNODE = m_ListUSER.GetHeadNode();
		while( pNODE ) {
			if ( pNODE->m_VALUE.m_iConnSockIDX ) {
				pUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pNODE->m_VALUE.m_iConnSockIDX );
				if ( pUSER && this->m_dwClanID == pUSER->GetClanID() ) {
					pUSER->SetClanMONEY( sInfo.m_biClanMONEY );	
					pUSER->SetClanLEVEL( sInfo.m_btClanLEVEL );
					pUSER->SetClanID   ( sInfo.m_dwClanID	 );
					pUSER->SetClanMARK ( sInfo.m_dwClanMARK	 );
					pUSER->SetClanRATE ( sInfo.m_iClanRATE	 );
					pUSER->SetClanSCORE( sInfo.m_iClanSCORE	 );
					::CopyMemory( pUSER->m_CLAN.m_ClanBIN.m_pDATA, this->m_ClanBIN.m_pDATA, sizeof( tagClanBIN ) );


#ifdef	__CLAN_INFO_ADD
					//	랭크 포인트 적용.
					pUSER->m_CLAN.m_iRankPoint = sInfo.m_iRankPoint;
#endif

					this->Send_SetCLAN ( RESULT_CLAN_MY_DATA, pUSER );
				}
			}
			pNODE = pNODE->GetNext ();
		}
	}
	this->Unlock ();

	/*
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
	return;

	pCPacket->m_HEADER.m_wType = WSV_CLAN_COMMAND;
	pCPacket->m_HEADER.m_nSize = sizeof( wsv_CLAN_COMMAND );
	pCPacket->m_wsv_CLAN_COMMAND.m_btRESULT = RESULT_CLAN_INFO;

	tag_CLAN_INFO sInfo;

	sInfo.m_nClanLEVEL	= this->m_nClanLEVEL;
	sInfo.m_iClanSCORE	= this->m_iClanSCORE;
	sInfo.m_iClanRATE	= this->m_iClanRATE;
	sInfo.m_biClanMONEY = this->m_biClanMONEY;

	pCPacket->AppendData( &sInfo, sizeof( tag_CLAN_INFO ) );
	pCPacket->AppendString( this->m_Name.Get() );
	pCPacket->AppendString( this->m_Desc.Get() );

	//CClan *pAllied;
	//for (int iC=0; iC<MAX_ALLIED_CLANS; iC++) {
	//	if ( this->m_dwAlliedClanID[ iC ] ) {
	//		pAllied = g_pThreadGUILD->Find_CLAN( this->m_dwAlliedClanID[ iC ] );
	//	}
	//}

	g_pUserLIST->SendPacketToSocketIDX( iSenderSockIDX, pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	*/
}

void CClan::SendRoster( int iSenderSockIDX )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;

	pCPacket->m_HEADER.m_wType = WSV_CLAN_COMMAND;
	pCPacket->m_HEADER.m_nSize = sizeof( wsv_CLAN_COMMAND );
	pCPacket->m_wsv_CLAN_COMMAND.m_btRESULT = RESULT_CLAN_ROSTER;

	CDLList< CClanUSER >::tagNODE *pNODE;
	tag_CLAN_MEMBER	sMember;
	this->Lock ();
	{
		pNODE = m_ListUSER.GetHeadNode();
		while( pNODE ) {
			sMember.m_btClanPOS			= pNODE->m_VALUE.m_iPosition;
			sMember.m_iClanCONTRIBUTE	= pNODE->m_VALUE.m_iContribute;
			sMember.m_btChannelNO		= pNODE->m_VALUE.m_btChannelNo;
			sMember.m_nLEVEL			= pNODE->m_VALUE.m_nLevel;
			sMember.m_nJOB				= pNODE->m_VALUE.m_nJob;

			pCPacket->AppendData( &sMember, sizeof(tag_CLAN_MEMBER) );
			pCPacket->AppendString( pNODE->m_VALUE.m_Name.Get() );

			if ( pCPacket->m_HEADER.m_nSize >= MAX_PACKET_SIZE - 34 ) {
				g_pUserLIST->SendPacketToSocketIDX( iSenderSockIDX, pCPacket );
				Packet_ReleaseNUnlock( pCPacket );

				pCPacket = Packet_AllocNLock ();
				if ( !pCPacket )
					break;

				pCPacket->m_HEADER.m_wType = WSV_CLAN_COMMAND;
				pCPacket->m_HEADER.m_nSize = sizeof( wsv_CLAN_COMMAND );
				pCPacket->m_wsv_CLAN_COMMAND.m_btRESULT = RESULT_CLAN_ROSTER;
			}
			pNODE = pNODE->GetNext ();
		}
	}
	this->Unlock ();

	if ( pCPacket && pCPacket->m_HEADER.m_nSize > sizeof( wsv_CLAN_COMMAND ) ) {
		g_pUserLIST->SendPacketToSocketIDX( iSenderSockIDX, pCPacket );
	}
	Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
void CClan::SendPacketToMEMBER (classPACKET *pCPacket)
{
	CDLList< CClanUSER >::tagNODE *pNODE;
	CWS_Client *pUSER;
	this->Lock ();
	{
		pNODE = m_ListUSER.GetHeadNode();
		while( pNODE ) {
			if ( pNODE->m_VALUE.m_iConnSockIDX ) {
				pUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pNODE->m_VALUE.m_iConnSockIDX );
				if ( pUSER && this->m_dwClanID == pUSER->GetClanID() )
					pUSER->SendPacket( pCPacket );
			}
			pNODE = pNODE->GetNext ();
		}
	}
	this->Unlock ();
}

// param :: CMD, Len, Ptr
bool CClan::Send_wsv_CLAN_COMMAND ( BYTE btCMD, ... )
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

	this->SendPacketToMEMBER( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

bool CClan::SetClanMARK( BYTE *pMarkData, short nDataLen, WORD wMarkCRC16, CWS_Client *pReqUSER )
{
	// WORD wCRC = classCRC::DataCRC16( pMarkData, nDataLen );
	//	클랜 마크를 디비에 기록....
	WORD wResult = g_pThreadGUILD->Query_UpdateClanMARK( this, wMarkCRC16, pMarkData, nDataLen );
	if ( wResult ) {
		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( !pCPacket )
			return false;

		pCPacket->m_HEADER.m_wType = WSV_CLANMARK_REPLY;
		pCPacket->m_HEADER.m_nSize = sizeof( wsv_CLANMARK_REPLY );

		pCPacket->m_wsv_CLANMARK_REPLY.m_dwClanID   = 0;
		pCPacket->m_wsv_CLANMARK_REPLY.m_wMarkCRC16 = wResult;

		pReqUSER->SendPacket( pCPacket );

		Packet_ReleaseNUnlock( pCPacket );
		return false;
	}

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	this->m_wClanMARK[0] = 0;
	this->m_wClanMARK[1] = wMarkCRC16;

	this->m_nClanMarkLEN = nDataLen;
	::CopyMemory( this->m_pClanMARK, pMarkData, nDataLen );

	pCPacket->m_HEADER.m_wType = WSV_CLANMARK_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( wsv_CLANMARK_REPLY );

	pCPacket->m_wsv_CLANMARK_REPLY.m_dwClanID = this->m_dwClanID;
	pCPacket->m_wsv_CLANMARK_REPLY.m_wMarkCRC16 = wMarkCRC16;
	pCPacket->AppendData( pMarkData, nDataLen );

	// 클랜의 모든 멤버에게...
	CDLList< CClanUSER >::tagNODE *pNODE;
	CWS_Client *pUSER;
	this->Lock ();
	{
		pNODE = m_ListUSER.GetHeadNode();
		while( pNODE ) {
			if ( pNODE->m_VALUE.m_iConnSockIDX ) {
				pUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pNODE->m_VALUE.m_iConnSockIDX );
				if ( pUSER && this->m_dwClanID == pUSER->GetClanID() ) {
					pUSER->SetClanMARK( m_dwClanMARK );
					pUSER->SendPacket( pCPacket );
#ifdef	__SHO_WS
					// 마크변경된 클랜주변의 사용자들을 위해서... GS에 마크ID통보=>CLI에 전송=>WS에 요청
					g_pListSERVER->Send_zws_SET_USER_CLAN( pUSER );
#endif
				}
			}
			pNODE = pNODE->GetNext ();
		}
	}
	this->Unlock ();

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}


//-------------------------------------------------------------------------------------------------
bool CClan::Send_wsv_CLANMARK_REG_TIME( CWS_Client *pMember )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = WSV_CLANMARK_REG_TIME;
	pCPacket->m_HEADER.m_nSize = sizeof( wsv_CLANMARK_REG_TIME );

	pCPacket->m_wsv_CLANMARK_REG_TIME.m_wYear = this->m_RegTIME.m_wYear ;
	pCPacket->m_wsv_CLANMARK_REG_TIME.m_btMon = this->m_RegTIME.m_btMon ;
	pCPacket->m_wsv_CLANMARK_REG_TIME.m_btDay = this->m_RegTIME.m_btDay ;
	pCPacket->m_wsv_CLANMARK_REG_TIME.m_btHour= this->m_RegTIME.m_btHour;
	pCPacket->m_wsv_CLANMARK_REG_TIME.m_btMin = this->m_RegTIME.m_btMin ;
	pCPacket->m_wsv_CLANMARK_REG_TIME.m_btSec = this->m_RegTIME.m_btSec ;

	pMember->SendPacket( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	return true;
}


//-------------------------------------------------------------------------------------------------
CThreadGUILD::CThreadGUILD( UINT uiInitDataCNT, UINT uiIncDataCNT ) 
: CSqlTHREAD( true ),
m_Pools("CGuildPOOL", uiInitDataCNT, uiIncDataCNT )
{
	m_pHashCLAN	= new classHASH< CClan* >( 1024 * 2 );
}
CThreadGUILD::~CThreadGUILD ()
{
	SAFE_DELETE( m_pHashCLAN );
}

//-------------------------------------------------------------------------------------------------
void CThreadGUILD::Test_add (char *pGuildName, char *pGuildDesc)
{
	long   iResultSP=-99;
	SDWORD cbSize1=SQL_NTS;

	((classODBC*)this->m_pSQL)->SetParam_long( 1, iResultSP, cbSize1 );

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(pGuildName, strlen(pGuildName));
	AlphabetCvt(pGuildDesc, strlen(pGuildDesc));
//-------------------------------------

	if ( this->m_pSQL->QuerySQL( "{?=call ws_ClanCREATE(\'%s\',\'%s\',%d,%d)}", pGuildName, pGuildDesc, 1,2 ) ) {
		DWORD dwClanID=0;
		while( this->m_pSQL->GetNextRECORD() ) {
			// 기존 클랜 ID...
			dwClanID = (DWORD)this->m_pSQL->GetInteger(0);
		}
		while ( this->m_pSQL->GetMoreRESULT() ) {
			if ( this->m_pSQL->BindRESULT () ) {
				if ( this->m_pSQL->GetNextRECORD() ) {
					// 생성된 클랜 ID...
					dwClanID = (DWORD)this->m_pSQL->GetInteger(0);
				}
			}
		}
		switch( iResultSP ) {
				case 0 :	// 성공
					g_LOG.CS_ODS( 0xffff, "Create clan success : dwCladID::%d\n", dwClanID );
					break;
				case -1 :	// 중복된 이름
					LogString( 0xffff, "duplicated clan name : result: %d\n", iResultSP );
					break;
				case -2 :	// insert 오류( 디비 오류 )
					g_LOG.CS_ODS( 0xffff, "insert sclan failed : result: %d\n", iResultSP );
					break;
				default :
					assert( "invalid ws_CreateCLAN SP retrun value" && 0 );
		}
	} else {
		// 디비 오류...

		int iii =999;
	}
}
void CThreadGUILD::Test_del (char *pGuildName)
{
	long   iResultSP=-99;
	SDWORD cbSize1=SQL_NTS;

	this->m_pSQL->SetParam_long( 1, iResultSP, cbSize1 );

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(pGuildName, strlen(pGuildName));
//-------------------------------------

	if ( this->m_pSQL->QuerySQL( "{?=call ws_ClanDELETE(\'%s\')}", pGuildName) ) {
		DWORD dwClanID=0;
		while( this->m_pSQL->GetNextRECORD() ) {
			// 성공...
			dwClanID = (DWORD)this->m_pSQL->GetInteger(0);
		}
		while ( this->m_pSQL->GetMoreRESULT() ) {
			if ( this->m_pSQL->BindRESULT() ) {
				if ( this->m_pSQL->GetNextRECORD() ) {
					// 성공... 생성된 ClanID = this->m_pSQL->GetInteger(0)
					dwClanID = (DWORD)this->m_pSQL->GetInteger(0);
				}
			}
		}
		switch( iResultSP ) {
			case 0 :	// 성공
				g_LOG.CS_ODS( 0xffff, "Delete clan success : dwCladID::%d\n", dwClanID );
				break;
			default :	// 실패
				g_LOG.CS_ODS( 0xffff, "Delete clan failed : %d\n", iResultSP );
		}
	} else {
		// 디비 SP 오류...
	}
}

void CThreadGUILD::Execute ()
{
	//this->Test_add( "aa", "bb" );
	//this->Test_add( "bb", "bb" );
	//this->Test_add( "cc", "bb" );
	//this->Test_add( "dd", "bb" );

	//this->Test_del ("bb");
	//this->Test_del ("cc");
	//this->Test_del ("ff");

	g_LOG.CS_ODS( 0xffff, ">  > >> CThreadGUILD::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );

	CDLList< tagCLAN_CMD >::tagNODE *pCmdNODE;

	while( TRUE ) {
		if ( !this->Terminated ) {
			m_pEVENT->WaitFor( INFINITE );
		} else {
			int iReaminCNT;
			this->m_CS.Lock ();
			iReaminCNT = m_WaitCMD.GetNodeCount();
			this->m_CS.Unlock ();

			if ( iReaminCNT <= 0 )
				break;
		}

		this->m_CS.Lock ();
		m_ProcCMD.AppendNodeList( &m_WaitCMD );
		m_WaitCMD.Init ();
		m_pEVENT->ResetEvent ();
		this->m_CS.Unlock ();

		for( pCmdNODE = m_ProcCMD.GetHeadNode(); pCmdNODE; ) {
			this->Run_GuildPACKET( &pCmdNODE->m_VALUE );

			SAFE_DELETE_ARRAY( pCmdNODE->m_VALUE.m_pPacket );
			m_ProcCMD.DeleteNFree( pCmdNODE );
			pCmdNODE = m_ProcCMD.GetHeadNode();
		}
	}

	int iCnt = m_AddPACKET.GetNodeCount();
	_ASSERT( iCnt == 0 );

	g_LOG.CS_ODS( 0xffff, "<<<< CThreadGUILD::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );
}



//-------------------------------------------------------------------------------------------------
bool CThreadGUILD::Add_ClanCMD (BYTE btCMD, int iSocketIDX, t_PACKET *pPacket, char *szCharName)
{
	CDLList< tagCLAN_CMD >::tagNODE *pNewNODE;

	pNewNODE = new CDLList< tagCLAN_CMD >::tagNODE;

	pNewNODE->m_VALUE.m_btCMD = btCMD;
	pNewNODE->m_VALUE.m_iSocketIDX	= iSocketIDX;
	if ( pPacket ) {
		// 2005.05.10 패킷 구조가 틀린것이 있는데... pPacket->m_cli_CLAN_COMMAND.m_btCMD ??????
		switch( btCMD ) {
			case GCMD_CLANMARK_SET :
			case GCMD_CLANMARK_GET :
			case GCMD_CLANMARK_REGTIME :
				break;
			default :
				if ( pPacket->m_cli_CLAN_COMMAND.m_btCMD >= GCMD_LOGOUT ) {
					if ( NULL == szCharName  ) {
						delete pNewNODE;
						return false;
					}
					pNewNODE->m_VALUE.m_CharNAME.Set( szCharName );
				} else
					if ( szCharName ) {
						pNewNODE->m_VALUE.m_CharNAME.Set( szCharName );
					}
					break;
		}

		pNewNODE->m_VALUE.m_pPacket	= (t_PACKET*) new BYTE[ pPacket->m_HEADER.m_nSize ];
		::CopyMemory( pNewNODE->m_VALUE.m_pPacket, pPacket, pPacket->m_HEADER.m_nSize );
	} else {
		pNewNODE->m_VALUE.m_CharNAME.Set( szCharName );
		pNewNODE->m_VALUE.m_pPacket	= NULL;
	}

	this->m_CS.Lock ();
	m_WaitCMD.AppendNode( pNewNODE );
	this->m_CS.Unlock ();

	this->Set_EVENT ();

	return true;
}


//-------------------------------------------------------------------------------------------------
bool CThreadGUILD::Run_GuildPACKET ( tagCLAN_CMD *pGuildCMD )
{
	short nOffset = sizeof( cli_CLAN_COMMAND );
	switch( pGuildCMD->m_btCMD )
	{
	case GCMD_CLANMARK_SET :
		{
			CWS_Client *pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pGuildCMD->m_iSocketIDX );
			if ( pFindUSER->GetClanID() && pFindUSER->GetClanPOS() >= GPOS_MASTER ) 
			{
				CClan *pClan = this->Find_CLAN( pFindUSER->GetClanID() );
				if ( pClan ) {
					short nMarkLen = pGuildCMD->m_pPacket->m_HEADER.m_nSize - sizeof( cli_CLANMARK_SET );
					BYTE *pMark = &pGuildCMD->m_pPacket->m_pDATA[ sizeof( cli_CLANMARK_SET ) ];
					pClan->SetClanMARK( pMark, nMarkLen, pGuildCMD->m_pPacket->m_cli_CLANMARK_SET.m_wMarkCRC16, pFindUSER );
				}
			}
			break;
		}
	case GCMD_CLANMARK_GET :
		{
			CClan *pClan = this->Find_CLAN( pGuildCMD->m_pPacket->m_cli_CLANMARK_REQ.m_dwClanID );
			if ( pClan ) {
				CWS_Client *pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pGuildCMD->m_iSocketIDX );
				if ( pFindUSER ) {
					pFindUSER->Send_wsv_CLANMARK_REPLY( pClan->m_dwClanID, pClan->m_wClanMARK[ 1 ], pClan->m_pClanMARK, pClan->m_nClanMarkLEN );
					return true;
				}
			}
			break;
		}
	case GCMD_CLANMARK_REGTIME :
		{
			CWS_Client *pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pGuildCMD->m_iSocketIDX );
			if ( pFindUSER->GetClanID() ) {
				CClan *pClan = this->Find_CLAN( pFindUSER->GetClanID() );
				if ( pClan ) {
					return pClan->Send_wsv_CLANMARK_REG_TIME( pFindUSER );
				}
			}
			break;
		}
	case GCMD_ADJ_VAR	:
		{
			//	클랜 관련 변수 변경 처리 한다.
#ifdef	__SHO_WS
			CClan *pClan = this->Find_CLAN( pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_dwClanID );
			if ( pClan ) 
			{
				switch( pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_btVarType ) 
				{
					case CLVAR_INC_LEV	:
						pClan->m_nClanLEVEL = pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue;
						pClan->Send_UpdateInfo ();
						break;

					case CLVAR_ADD_SCORE:
						pClan->m_iClanSCORE = pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue;
						pClan->Send_UpdateInfo ();
						break;

					case CLVAR_ADD_ZULY	:
						pClan->m_biClanMONEY = pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_biResult;
						pClan->Send_UpdateInfo ();
						break;

#ifdef	__CLAN_INFO_ADD
					// 클랜 랭크 포인트 업데이트 처리 : 김영환
					case CLVAR_SET_RANKPOINT:
						//	클랜 랭크 포인트 업데이트
						pClan->m_iRankPoint = pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue;
						pClan->Send_UpdateInfo();
						break;
#endif



#ifdef __CLAN_REWARD_POINT
					case CLVAR_SET_REWARDPOINT:
						// 클랜전 보상 포인트 추가
						pClan->Adjust_MEMBER_SetRewardPoint( pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue );
						break;

					case CLVAR_ADD_REWARDPOINT:
						// 클랜전 보상 포인트 추가
						pClan->Adjust_MEMBER_AddRewardPoint( pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue,
															 pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_pNameList );
							
						break;
#endif

					case CLVAR_ADD_SKILL:
						{
							CClan *pClan = this->Find_CLAN( pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_dwClanID );
							if ( pClan ) {
								pClan->AddClanSKILL( pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue );
							}
							break;
						}
					case CLVAR_DEL_SKILL :
						{
							CClan *pClan = this->Find_CLAN( pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_dwClanID );
							if ( pClan ) {
								pClan->DelClanSKILL( pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue );
							}
							break;
						}

				}
			}

#else

			switch( pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_btVarType ) 
			{
				case CLVAR_INC_LEV	:
					if ( !this->Query_UpdateClanDATA( "intLEVEL", pGuildCMD->m_pPacket ) ) // CLVAR_INC_LEV, 1 ) )
						return true;
					break;

				case CLVAR_ADD_SCORE:
					if ( !this->Query_UpdateClanDATA( "intPOINT", pGuildCMD->m_pPacket ) ) // CLVAR_ADD_SCORE,  pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue ) )
						return true;
					break;

				case CLVAR_ADD_ZULY	:
					if ( !this->Query_UpdateClanDATA( "intMONEY", pGuildCMD->m_pPacket ) ) // CLVAR_ADD_ZULY,  pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue ) )
						return true;
					break;

#ifdef	__CLAN_INFO_ADD
				//	클랜 랭크 포인트 업데이트 처리 : 김영환
				case CLVAR_SET_RANKPOINT:
					//	클랜 랭크 포인트 DB 업데이트
					if ( !this->Query_UpdateClanDATA( "intRankPoint", pGuildCMD->m_pPacket ) )
						return true;
					break;
#endif



#ifdef	__INC_WORLD

				case CLVAR_ADD_SKILL :	// 클랜 오너에 의해서만 명령이 하달됨...
					{
						// 개인섭만 적용... GS에서는 Find_CLAN은 무조건 NULL !!!
						CClan *pClan = this->Find_CLAN( pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_dwClanID );
						if ( pClan ) {
							pClan->AddClanSKILL( pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue );
						}
						break;
					}

				case CLVAR_DEL_SKILL :	// 클랜 멤버에 의해서도 명령이 하달될수 있음..
					// 개인섭만 적용...GS에서는 Find_CLAN은 무조건 NULL !!!
					CClan *pClan = this->Find_CLAN( pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_dwClanID );
					if ( pClan ) {
						pClan->DelClanSKILL( pGuildCMD->m_pPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue );
					}
					break;

#endif
			}
#if !defined(__INC_WORLD)
			// GS에서 호출되는 함수로 WS에 전송...
			g_pSockLSV->Send_gsv_ADJ_CLAN_VAR( pGuildCMD->m_pPacket );
#endif

#endif	// __SHO_WS

			return true;
		}
	case GCMD_CHAT		:		// 클랜 챗
		{
			char *szMsg;
			short nOffset=sizeof( cli_CLAN_CHAT );

			szMsg = Packet_GetStringPtr( pGuildCMD->m_pPacket, nOffset );
			if ( szMsg ) {
				CClan *pClan = this->Find_CLAN( pGuildCMD->m_iSocketIDX );
				if ( pClan ) {
					classPACKET *pCPacket = Packet_AllocNLock ();
					if ( !pCPacket )
						return false;

					pCPacket->m_HEADER.m_wType = WSV_CLAN_CHAT;
					pCPacket->m_HEADER.m_nSize = sizeof( wsv_CLAN_CHAT );
					pCPacket->AppendString( pGuildCMD->m_CharNAME.Get() );
					pCPacket->AppendString( szMsg );

					pClan->SendPacketToMEMBER( pCPacket );

					Packet_ReleaseNUnlock( pCPacket );
				}
			}
			return true;
		}
	case GCMD_LOGIN		:		// 사용자 로그인
		{
			this->Query_LoginClanMember( pGuildCMD->m_CharNAME.Get(), pGuildCMD->m_iSocketIDX );
			return true;
		}
	case GCMD_LOGOUT	:		// 사용자 로그아웃
		{
			CClan *pClan = this->Find_CLAN( (DWORD)pGuildCMD->m_iSocketIDX );
			if ( pClan )
				pClan->LogOut_ClanUSER( pGuildCMD->m_CharNAME.Get() );
			return true;
		}
	case GCMD_CREATE	:		// 길드 생성 :: GS에서 처리
		{
#ifndef	__SHO_WS
			CWS_Client *pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pGuildCMD->m_iSocketIDX );
			if ( pFindUSER && 0 == pFindUSER->GetClanID() ) {
				CClan *pClan = this->Query_CreateCLAN( pGuildCMD->m_iSocketIDX, pGuildCMD->m_pPacket );
				if ( pClan ) {
					if ( pClan->Insert_MEMBER( RESULT_CLAN_CREATE_OK, pFindUSER, GPOS_MASTER ) ) {
						if ( !pFindUSER->CheckClanCreateCondition( 1 ) ) {	// 클랜 생성 시도시 생성된 조건 검증...
							// 접속 종료된 넘...
							this->Query_DeleteCLAN ( pClan->m_Name.Get() );
							pFindUSER->UnlockSOCKET ();
							return true;
						}

#ifndef	__INC_WORLD
						// 월드섭으루 클랜 생성 통보....
						g_pSockLSV->Send_zws_CREATE_CLAN( pFindUSER->m_dwWSID, pFindUSER->m_HashCHAR );
#endif
						return true;
					} else {
						this->Query_DeleteCLAN ( pClan->m_Name.Get() );
						pFindUSER->Send_wsv_CLAN_COMMAND( RESULT_CLAN_CREATE_NO_RIGHT, NULL );
					}
				} else {
					// 실패...
					pFindUSER->Send_wsv_CLAN_COMMAND( RESULT_CLAN_CREATE_DUP_NAME, NULL );
				}

				pFindUSER->CheckClanCreateCondition( 2 );	// 클랜 생성 실패 :: 감소된 조건 복원...
			}
#endif
			return true;
		}
	case GCMD_DISBAND	:		// 길드 해체
		{
			CWS_Client *pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pGuildCMD->m_iSocketIDX );
			if ( pFindUSER->GetClanID() && pFindUSER->GetClanPOS() >= GPOS_MASTER ) {
				CClan *pClan = this->Find_CLAN( pFindUSER->GetClanID() );
				if ( pClan && pClan->m_ListUSER.GetNodeCount() < 2 ) {
					if ( this->Query_DeleteCLAN( pClan->m_Name.Get() ) ) {
						g_pThreadLOG->When_ws_CLAN( pFindUSER->Get_NAME(), pFindUSER->Get_IP(), "Disband", pClan, NEWLOG_CLAN_DESTROYED );
					} else {
						pFindUSER->Send_wsv_CLAN_COMMAND( RESULT_CLAN_DESTROY_FAILED, NULL );
					}
				}
			}
			return true;
		}
	case GCMD_INVITE_REPLY_YES	:		// 길드 초대 승락
	case GCMD_INVITE_REPLY_NO	:		// 길드 초대 거부
		{
			short nOffset = sizeof( cli_CLAN_COMMAND );
			char *pOwnerName = Packet_GetStringPtr( pGuildCMD->m_pPacket, nOffset );
			CWS_Client *pOwner = g_pUserLIST->Find_CHAR( pOwnerName );
			if ( pOwner && pOwner->GetClanPOS() >= GPOS_SUB_MASTER ) {
				CWS_Client *pReplyer = (CWS_Client*)g_pUserLIST->GetSOCKET( pGuildCMD->m_iSocketIDX );
				if ( pReplyer && 0 == pReplyer->GetClanID() ) {
					if ( pGuildCMD->m_btCMD == GCMD_INVITE_REPLY_YES ) {
						CClan *pClan = this->Find_CLAN( pOwner->GetClanID() );
						if ( pClan ) {
							if ( !pClan->Insert_MEMBER( RESULT_CLAN_JOIN_OK, pReplyer, GPOS_JUNIOR, pOwner->Get_NAME() ) ) {
								pOwner->Send_wsv_CLAN_COMMAND( RESULT_CLAN_JOIN_MEMBER_FULL, NULL );
							}
						}
					} else
						pOwner->Send_wsv_CLAN_COMMAND( GCMD_INVITE_REPLY_NO, pReplyer->m_Name.BuffLength()+1, pReplyer->Get_NAME(), NULL );
				}
			}
			return true;
		}
	case GCMD_REMOVE	:		// 길드 추방
	case GCMD_PROMOTE	:		// 길드 승급
	case GCMD_DEMOTE	:		// 길드 강등
	case GCMD_LEADER	:		// 길드 위임
	case GCMD_MOTD		:		// 길드 공지
	case GCMD_SLOGAN	:		// 길드 슬로건
	case GCMD_INVITE    :
	case GCMD_MEMBER_JOBnLEV :
		{
			short nOffset = sizeof( cli_CLAN_COMMAND );
			char *pAddStr = Packet_GetStringPtr( pGuildCMD->m_pPacket, nOffset );
			if ( pAddStr && pAddStr[0] ) 
			{
				CWS_Client *pClanOwner = (CWS_Client*)g_pUserLIST->GetSOCKET( pGuildCMD->m_iSocketIDX );
				if ( pClanOwner && pClanOwner->GetClanID() ) 
				{
					CClan *pClan = this->Find_CLAN( pClanOwner->GetClanID() );
					if ( pClan ) 
					{
						switch( pGuildCMD->m_btCMD )
						{
							case GCMD_INVITE	:		// 길드 초대
								if ( pClanOwner->GetClanPOS() >= GPOS_SUB_MASTER ) {
									CWS_Client *pDestUSER = g_pUserLIST->Find_CHAR( pAddStr );
									if ( pDestUSER ) {
										if ( 0 == pDestUSER->GetClanID() ) {
											pDestUSER->Send_wsv_CLAN_COMMAND( GCMD_INVITE_REQ, pClanOwner->m_Name.BuffLength()+1, pClanOwner->Get_NAME(), NULL );
										} else {
											pClanOwner->Send_wsv_CLAN_COMMAND( RESULT_CLAN_JOIN_HAS_CLAN, strlen( pAddStr )+1, pAddStr, NULL );
										}
									} else 
										pClanOwner->Send_wsv_CLAN_COMMAND( RESULT_CLAN_JOIN_FAILED, strlen( pAddStr )+1, pAddStr, NULL );
								}
								break;
							case GCMD_REMOVE	:		// 길드 추방
								if ( pClanOwner->GetClanPOS() >= GPOS_SUB_MASTER ) {
									// 자신은 안됨
									if ( pClan->Delete_MEMBER( pClanOwner->m_HashCHAR, pAddStr, RESULT_CLAN_KICK, pClanOwner->Get_NAME() ) ) {
										// 추방 로그...
										g_pThreadLOG->When_ws_CLAN( pAddStr, pClanOwner->Get_IP(), pClanOwner->Get_NAME(), pClan, NEWLOG_CLAN_KICK_MEMBER );
									}
								}
								break;
							case GCMD_PROMOTE	:		// 길드 승급
								if ( pClanOwner->GetClanPOS() >= GPOS_MASTER ) {
									// 자신은 안됨.
									pClan->Adjust_MEMBER( pClanOwner->m_HashCHAR, pAddStr, 0, 1, pClanOwner->Get_NAME(), GPOS_SUB_MASTER, 0, true );
								}
								break;
							case GCMD_DEMOTE	:		// 길드 강등
								if ( pClanOwner->GetClanPOS() >= GPOS_MASTER ) {
									// 자신은 안됨
									pClan->Adjust_MEMBER( pClanOwner->m_HashCHAR, pAddStr, 0, -1, pClanOwner->Get_NAME(), GPOS_SUB_MASTER );
								}
								break;
							case GCMD_LEADER	:		// 길드 위임
								if ( pClanOwner->GetClanPOS() >= GPOS_MASTER ) 
								{
									// pAddStr 유저를 GPOS_MASTER로 지정
									if ( pClan->Adjust_MEMBER( pClanOwner->m_HashCHAR, pAddStr, 0, GPOS_MASTER, pClanOwner->Get_NAME(), GPOS_MASTER, GPOS_COMMANDER ) ) 
									{
										// 위임 로그...
										pClan->Adjust_MEMBER( 0, pClanOwner->Get_NAME(), 0, GPOS_COMMANDER-GPOS_MASTER, pClanOwner->Get_NAME(), GPOS_COMMANDER );
									}
								}
								break;
							case GCMD_MOTD		:		// 길드 공지
								if ( pClanOwner->GetClanPOS() >= GPOS_MASTER ) {
									if ( this->Query_UpdateClanMOTD( pClan->m_dwClanID, pAddStr ) ) {
										pClan->m_Motd.Set( pAddStr );
										if ( pAddStr && pAddStr[0] ) {
											if ( pClan->m_Motd.BuffLength() < 368 ) {
												pClan->Send_wsv_CLAN_COMMAND( RESULT_CLAN_MOTD, pClan->m_Motd.BuffLength()+1, pClan->m_Motd.Get(), NULL );
											}
										}
									}
								}
								break;
							case GCMD_SLOGAN	:		// 길드 슬로건
								if ( pClanOwner->GetClanPOS() >= GPOS_MASTER ) {
									if ( this->Query_UpdateClanSLOGAN( pClan->m_dwClanID, pAddStr ) ) {
										pClan->m_Desc.Set( pAddStr );
										if ( pAddStr && pAddStr[0] ) {
											if ( pClan->m_Desc.BuffLength() < 128 ) {
												pClan->Send_wsv_CLAN_COMMAND( RESULT_CLAN_SLOGAN, pClan->m_Desc.BuffLength()+1, pClan->m_Desc.Get(), NULL );
											}
										}
									}
								}
								break;
							case GCMD_MEMBER_JOBnLEV :
								pClan->SetJOBnLEV( pClanOwner->m_HashCHAR, pClanOwner->Get_NAME(),
									pGuildCMD->m_pPacket->m_cli_CLAN_MEMBER_JOBnLEV.m_nJOB,
									pGuildCMD->m_pPacket->m_cli_CLAN_MEMBER_JOBnLEV.m_nLEVEL );
								break;
						}
					}
				}
			}
			return true;
		}

	case GCMD_QUIT		:		// 길드 탈퇴
		{
			// 1. 길드원 인가 ?
			CWS_Client *pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pGuildCMD->m_iSocketIDX );
			if ( pFindUSER && pFindUSER->GetClanID() && pFindUSER->GetClanPOS() < GPOS_MASTER ) {
				CClan *pClan = this->Find_CLAN( pFindUSER->GetClanID() );
				if ( pClan ) {
					if ( pClan->Delete_MEMBER( 0, pFindUSER->Get_NAME(), RESULT_CLAN_QUIT ) ) {
						// 탈퇴 로그...
						g_pThreadLOG->When_ws_CLAN( pFindUSER->Get_NAME(), pFindUSER->Get_IP(), "?", pClan, NEWLOG_CLAN_QUIT_MEMBER );
					}
				}
			}
			return true;
		}
	case GCMD_ROSTER	:		// 길드 목록
		{
			CWS_Client *pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pGuildCMD->m_iSocketIDX );
			if ( pFindUSER && pFindUSER->GetClanID() ) {
				CClan *pClan = this->Find_CLAN( pFindUSER->GetClanID() );
				if ( pClan ) {
					pClan->SendRoster( pGuildCMD->m_iSocketIDX );
				}
			}
			return true;
		}
	}

	return false;
}


//-------------------------------------------------------------------------------------------------
CClan *CThreadGUILD::Find_CLAN( DWORD dwClanID )
{
	tagHASH< CClan* > *pHashNode;

	pHashNode = m_pHashCLAN->Search( dwClanID );
	CClan *pClan = pHashNode ? pHashNode->m_DATA : NULL;
	while( pClan ) {
		if ( dwClanID == pClan->m_dwClanID ) {
			// Find !!!
			break;
		}

		pHashNode = m_pHashCLAN->SearchContinue( pHashNode, dwClanID );
		pClan = pHashNode ? pHashNode->m_DATA : NULL;
	}

	return pClan;
}

//-------------------------------------------------------------------------------------------------
CClan *CThreadGUILD::Load_CLAN( DWORD dwClanID )
{
	if ( this->m_pSQL->QuerySQL( "{call ws_ClanSELECT(%d)}", dwClanID ) ) {
		if ( this->m_pSQL->GetNextRECORD() ) {
			// this->m_pSQL->GetInteger(0); cladID
			char *pClanName  = this->m_pSQL->GetStrPTR(1,false);
			char *pClanDesc  = this->m_pSQL->GetStrPTR(2,false);
			WORD  wMarkIdx1  = this->m_pSQL->GetInteger(3);
			WORD  wMarkIdx2  = this->m_pSQL->GetInteger(4);
			short nClanLEV   = this->m_pSQL->GetInteger(5);
			int	  iClanSCORE = this->m_pSQL->GetInteger(6);
			DWORD dwAlliedGRP= (DWORD)this->m_pSQL->GetInteger(7);
			short nRate		 = this->m_pSQL->GetInteger(8);
			__int64 biZuly   = this->m_pSQL->GetInt64(9);
			BYTE *pClanBIN	 = this->m_pSQL->GetDataPTR(10);
			char *pClanMOTD  = this->m_pSQL->GetStrPTR(11,false);
			WORD  wMarkCRC   = this->m_pSQL->GetInteger(12);
			short nMarkLen   = this->m_pSQL->GetInteger(13);
			BYTE *pClanMARK  = this->m_pSQL->GetDataPTR(14);
			sqlTIMESTAMP sTimeStamp;
			this->m_pSQL->GetTimestamp(15, &sTimeStamp);


#ifdef	__CLAN_INFO_ADD
			//	클랜의 랭킹 점수 추가. 김영환 추가.
			//	DB 컬럼 읽기
			int	iRank_P		= this->m_pSQL->GetInteger(16);
#endif


			CClan *pClan = m_Pools.Pool_Alloc ();
			pClan->Init( pClanName, pClanDesc, dwClanID, wMarkIdx1, wMarkIdx2, pClanBIN, wMarkCRC, nMarkLen, pClanMARK );

			pClan->m_nClanLEVEL		= nClanLEV;
			pClan->m_iClanSCORE		= iClanSCORE;
			pClan->m_dwAlliedGroupID= dwAlliedGRP;
			pClan->m_iClanRATE		= nRate;
			pClan->m_biClanMONEY	= biZuly;


#ifdef	__CLAN_INFO_ADD
			//	클랜 DB에 저장된 랭크 포인트를 넣는다.
			pClan->m_iRankPoint = 	iRank_P;
#endif


			pClan->m_RegTIME.m_wYear = sTimeStamp.m_wYear ;
			pClan->m_RegTIME.m_btMon = sTimeStamp.m_btMon ;
			pClan->m_RegTIME.m_btDay = sTimeStamp.m_btDay ;
			pClan->m_RegTIME.m_btHour= sTimeStamp.m_btHour;
			pClan->m_RegTIME.m_btMin = sTimeStamp.m_btMin ;
			pClan->m_RegTIME.m_btSec = sTimeStamp.m_btSec ;

			// 모든 클랜멤버 얻기...
			if ( this->m_pSQL->QuerySQL( "{call ws_ClanCharALL(%d)}", dwClanID) ) {
				int iClanPos;
				int iClanRewardPoint = 0;
				CDLList< CClanUSER >::tagNODE *pNode;
				char *pCharName;

				// 내 클랜의 모든 클랜원 보관
				while( this->m_pSQL->GetNextRECORD() ) 
				{
					pCharName  = this->m_pSQL->GetStrPTR (0);
					iClanSCORE = this->m_pSQL->GetInteger(1);
					iClanPos   = this->m_pSQL->GetInteger(2);
					iClanRewardPoint = this->m_pSQL->GetInteger(3);

					pNode = pClan->m_ListUSER.AllocNAppend ();
					if ( !pNode ) {
						// out of mem...
						break;
					}

					if ( iClanPos > GPOS_MASTER ) iClanPos = GPOS_MASTER;

					pClan->m_nPosCNT[ iClanPos ] ++;		// 클랜 로딩...

					pNode->m_VALUE.m_Name.Set( pCharName );
					pNode->m_VALUE.m_HashName	= ::StrToHashKey ( pCharName );
					pNode->m_VALUE.m_iPosition	= iClanPos;
					pNode->m_VALUE.m_iContribute= iClanSCORE;
					pNode->m_VALUE.m_btChannelNo= 0xff;
					pNode->m_VALUE.m_nJob		= 0;
					pNode->m_VALUE.m_nLevel		= 0;
					// (20070319:남병철) : 보상 포인트 읽기 (월드 서버에는 CClanUSER에 보상포인트를 보관한다.)
					pNode->m_VALUE.m_iRewardPoint = iClanRewardPoint;
				}

				// 클랜원이 있으면 등록
				if ( pClan->m_ListUSER.GetNodeCount() ) 
				{
					m_pHashCLAN->Insert( dwClanID, pClan );
					return pClan;
				}
				g_LOG.CS_ODS( 0xffff, "Clan[%d] member counter == 0 !!!!\n", dwClanID );
			} else {
				g_LOG.CS_ODS( 0xffff, "Clan[%d] db error !!!!\n", dwClanID );
			}

			pClan->Free ();
			m_Pools.Pool_Free( pClan );
			return NULL;
		}
	}
	return NULL;
}

//-------------------------------------------------------------------------------------------------
CClan *CThreadGUILD::Query_CreateCLAN( int iSocketIDX, t_PACKET *pPacket )
{
	short nOffset = sizeof( cli_CLAN_CREATE );
	char *pGuildName = Packet_GetStringPtr( pPacket, nOffset );
	if ( !pGuildName )
		return NULL;
	char *pGuildDesc = Packet_GetStringPtr( pPacket, nOffset );
	if ( !pGuildDesc )
		return NULL;

	long   iResultSP=-99;
	SDWORD cbSize1=SQL_NTS;

	this->m_pSQL->SetParam_long( 1, iResultSP, cbSize1 );

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(pGuildName, strlen(pGuildName));
	AlphabetCvt(pGuildDesc, strlen(pGuildDesc));
//-------------------------------------

	if ( this->m_pSQL->QuerySQL( "{?=call ws_ClanINSERT(\'%s\',\'%s\',%d,%d)}", pGuildName, pGuildDesc, 
		pPacket->m_cli_CLAN_CREATE.m_wMarkIDX[0],
		pPacket->m_cli_CLAN_CREATE.m_wMarkIDX[1] ) ) {
			DWORD dwClanID=0;
			while ( this->m_pSQL->GetMoreRESULT() ) {
				if ( this->m_pSQL->BindRESULT() ) {
					if ( this->m_pSQL->GetNextRECORD() ) {
						// 성공... 생성된 ClanID = this->m_pSQL->GetInteger(0)
						dwClanID = (DWORD)this->m_pSQL->GetInteger(0);
					}
				}
			}
			switch( iResultSP ) {
			case 0 :	// 성공
				{
					g_LOG.CS_ODS( 0xffff, "Create clan success : dwCladID::%d\n", dwClanID );

					CClan *pClan = m_Pools.Pool_Alloc ();
					pClan->Init( pGuildName, pGuildDesc, dwClanID, pPacket->m_cli_CLAN_CREATE.m_wMarkIDX[0], pPacket->m_cli_CLAN_CREATE.m_wMarkIDX[1], NULL, 0, 0, NULL );
					m_pHashCLAN->Insert( dwClanID, pClan );
					return pClan;
				}
			case -1 :	// 중복된 이름
				LogString( 0xffff, "duplicated clan name : result: %d\n", iResultSP );
				break;
			case -2 :	// insert 오류( 디비 오류 )
				g_LOG.CS_ODS( 0xffff, "insert sclan failed : result: %d\n", iResultSP );
				break;
			default :
				assert( "invalid ws_CreateCLAN SP retrun value" && 0 );
			}
		} else {
			// 디비 SP 오류...
		}

		return NULL;
}
bool CThreadGUILD::Query_DeleteCLAN( char *szClanName )
{
	long   iResultSP=-99;
	SDWORD cbSize1=SQL_NTS;

	this->m_pSQL->SetParam_long( 1, iResultSP, cbSize1 );

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(szClanName, strlen(szClanName));
//-------------------------------------

	if ( this->m_pSQL->QuerySQL( "{?=call ws_ClanDELETE(\'%s\')}", szClanName) ) {
		DWORD dwClanID=0;
		//while( this->m_pSQL->GetNextRECORD() ) {
		//	// 성공...
		//	dwClanID = (DWORD)this->m_pSQL->GetInteger(0);
		//}
		while ( this->m_pSQL->GetMoreRESULT() ) {
			if ( this->m_pSQL->BindRESULT() ) {
				if ( this->m_pSQL->GetNextRECORD() ) {
					dwClanID = (DWORD)this->m_pSQL->GetInteger(0);
				}
			}
		}
		switch( iResultSP ) {
			case 0 :	// 성공
				{
					CClan *pClan = this->Find_CLAN( dwClanID );
					if ( pClan ) {
						pClan->Disband (); 

						m_pHashCLAN->Delete( dwClanID, pClan );
						pClan->Free ();
						m_Pools.Pool_Free( pClan );
					}
					g_LOG.CS_ODS( 0xffff, "Delete clan success : dwCladID::%d\n", dwClanID );
					return true;
				}
			default :	// 실패
				g_LOG.CS_ODS( 0xffff, "Delete clan failed : %d\n", iResultSP );
		}
	} else {
		// 디비 SP 오류...
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
bool CThreadGUILD::Query_InsertClanMember( char *szCharName, DWORD dwClanID, int iClanPos )
{
	long   iResultSP=-99;
	SDWORD cbSize1=SQL_NTS;

	this->m_pSQL->SetParam_long( 1, iResultSP, cbSize1 );

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(szCharName, strlen(szCharName));
//-------------------------------------

	if ( this->m_pSQL->QuerySQL( "{?=call ws_ClanCharADD(\'%s\',%d,%d)}", szCharName, dwClanID, iClanPos ) ) {
		while ( this->m_pSQL->GetMoreRESULT() ) {
			;
		}
		switch( iResultSP ) {
			case 0 :	// 성공
				g_LOG.CS_ODS( 0xffff, "Insert clan user : ClanID::%d / %s\n", dwClanID, szCharName );
				return true;
			case -1 :	// 중복된 이름
				LogString( 0xffff, "duplicated clan user name : ClanID:%d / %s \n", dwClanID, szCharName);
				break;
			case -2 :	// insert 오류( 디비 오류 )
				g_LOG.CS_ODS( 0xffff, "insert clan user failed : result: %d / %s\n", dwClanID, szCharName );
				break;
			default :
				assert( "invalid ws_CreateCLAN SP retrun value" && 0 );
		}
	} else {
		// 디비 SP 오류...
	}
	return false;
}
int CThreadGUILD::Query_DeleteClanMember( char *szCharName )
{
	long   iResultSP=-99;
	SDWORD cbSize1=SQL_NTS;

	this->m_pSQL->SetParam_long( 1, iResultSP, cbSize1 );

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(szCharName, strlen(szCharName));
//-------------------------------------

	if ( this->m_pSQL->QuerySQL( "{?=call ws_ClanCharDEL(\'%s\')}", szCharName ) ) 
	{
		DWORD dwClanID=0;
		while( this->m_pSQL->GetNextRECORD() ) 
		{
			dwClanID = (DWORD)this->m_pSQL->GetInteger(0);
		}
		while ( this->m_pSQL->GetMoreRESULT() )
		{
			if ( this->m_pSQL->BindRESULT() ) 
			{
				if ( this->m_pSQL->GetNextRECORD() ) 
				{
					assert( 0 );
				}
			}
		}
		switch( iResultSP ) 
		{
		case 0 :	// 성공
			LogString( 0xffff, "Delete clan user success : dwCladID:%d / %s\n", dwClanID, szCharName );
			return true;
		default :	// 실패
			g_LOG.CS_ODS( 0xffff, "Delete clan user failed : Result: %d dwCladID: %d / %s\n",iResultSP, dwClanID, szCharName );
		}
	} 
	else
	{
		// 디비 SP 오류...
		g_LOG.CS_ODS( 0xffff, "SP  ws_ClanCharDEL Error   %s\n", szCharName );

	}
	return iResultSP;
}

bool CThreadGUILD::Query_AdjustClanMember( char *szCharName, int iAdjContr, int iAdjPos )
{
	long   iResultSP=-99;
	SDWORD cbSize1=SQL_NTS;

	this->m_pSQL->SetParam_long( 1, iResultSP, cbSize1 );

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(szCharName, strlen(szCharName));
//-------------------------------------
	
	// ws_ClanCharADJ( Type(0:세팅, 1:더하기), 캐릭터명, 기여도, 클랜전 보상 포인트 )
	if ( this->m_pSQL->QuerySQL("{?=call ws_ClanCharADJ(%d,\'%s\',%d,%d,%d)}", 1, szCharName, iAdjContr, iAdjPos,  0 ) ) {
		while ( this->m_pSQL->GetMoreRESULT() ) {
			;
		}
		switch( iResultSP ) {
			case 0 :	// 성공
				g_LOG.CS_ODS( 0xffff, "update clan user :  %s\n", szCharName );
				return true;
			case -1 :	// 멤버 없다.
				LogString( 0xffff, "not clan user name :  %s \n", szCharName);
				break;
			case -2 :	// 디비 오류
				LogString( 0xffff, "update clan user failed : result: %s\n", szCharName );
				break;
			default :
				assert( "invalid ws_ClanAdjCHAR SP retrun value" && 0 );
		}


	} else {
		// 디비 SP 오류...
	}

	return false;
}

// (20070108:남병철) : 클랜 캐릭터 정보 테이블의 값 수정
bool CThreadGUILD::Query_AdjustClanMember_RewardPoint( int iType, char* szCharName, int iRewardPoint, int& iResult )
{
	long   iResultSP=-99;
	int	   iRe = 0;
	SDWORD cbSize1=SQL_NTS;


	this->m_pSQL->SetParam_long( 1, iResultSP, cbSize1 );

	//-------------------------------------
	// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(szCharName, strlen(szCharName));
	//-------------------------------------

	// ws_ClanCharADJ( Type(0:세팅, 1:더하기), 캐릭터명, 기여도(0:값보전), 캐릭등급(0:값보전), 클랜전 보상 포인트 )
	if( iType == 1 )
		int iiiii = 555;
	if ( this->m_pSQL->QuerySQL("{?=call ws_ClanCharADJ(%d,\'%s\',%d,%d,%d)}", iType, szCharName, 0, 0, iRewardPoint ) )
	{
		while ( this->m_pSQL->GetMoreRESULT() ) 
		{
			if ( this->m_pSQL->BindRESULT() ) {
				if ( this->m_pSQL->GetNextRECORD() ) 
				{
					// 성공...
					// dwClanID = (DWORD)this->m_pSQL->GetInteger(0);
					iRe = this->m_pSQL->GetInteger(2);	// 보상 포인트
					iResult = iRe;
					//biResult = this->m_pSQL->GetInt64(0);
				}
			}
		}

		switch( iResultSP )
		{
			case 0 :	// 성공
				g_LOG.CS_ODS( 0xffff, "update clan user :  %s\n", szCharName );
				return true;

			case -1 :	// 멤버 없다.
				LogString( 0xffff, "not clan user name :  %s \n", szCharName);
				break;

			case -2 :	// 디비 오류
				LogString( 0xffff, "update clan user failed : result: %s\n", szCharName );
				break;

			default :
				assert( "invalid ws_ClanAdjCHAR SP retrun value" && 0 );
		}
	}
	else 
	{
		// 디비 SP 오류...
	}

	iResult = -1;

	return false;
}

//-------------------------------------------------------------------------------------------------
bool CThreadGUILD::Query_UpdateClanMOTD( DWORD dwClanID, char *szMessage )
{
	long   iResultSP=-99;
	SDWORD cbSize1=SQL_NTS;

	this->m_pSQL->SetParam_long( 1, iResultSP, cbSize1 );

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(szMessage, strlen(szMessage));
//-------------------------------------

	if ( this->m_pSQL->QuerySQL( "{?=call ws_ClanMOTD(%d,\'%s\')}       ", dwClanID, szMessage ) ) {
		while ( this->m_pSQL->GetMoreRESULT() ) {
			;
		}
		switch( iResultSP ) {
			case 0 :	// 성공
				g_LOG.CS_ODS( 0xffff, "update clan motd : %d / %s\n", dwClanID, szMessage );
				return true;
				//case -1 :	// ????
				//	g_LOG.CS_ODS( 0xffff, "not clan user name :  %s \n", szCharName);
				//	break;
			case -2 :	// 디비 오류
				g_LOG.CS_ODS( 0xffff, "update clan motd failed : %d / %s\n", dwClanID, szMessage );
				break;
			default :
				assert( "invalid ws_ClanMOTD SP retrun value" && 0 );
		}
	} else {
		// 디비 SP 오류...
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
bool CThreadGUILD::Query_UpdateClanSLOGAN( DWORD dwClanID, char *szMessage )
{
	long   iResultSP=-99;
	SDWORD cbSize1=SQL_NTS;

	this->m_pSQL->SetParam_long( 1, iResultSP, cbSize1 );

//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(szMessage, strlen(szMessage));
//-------------------------------------

	if ( this->m_pSQL->QuerySQL( "{?=call ws_ClanSLOGAN(%d,\'%s\')}", dwClanID, szMessage ) ) {
		while ( this->m_pSQL->GetMoreRESULT() ) {
			;
		}
		switch( iResultSP ) {
			case 0 :	// 성공
				g_LOG.CS_ODS( 0xffff, "update clan desc : %d / %s\n", dwClanID, szMessage );
				return true;
				//case -1 :	// ????
				//	g_LOG.CS_ODS( 0xffff, "not clan user name :  %s \n", szCharName);
				//	break;
			case -2 :	// 디비 오류
				g_LOG.CS_ODS( 0xffff, "update clan user failed : %d / %s\n", dwClanID, szMessage );
				break;
			default :
				assert( "invalid ws_ClanSLOGAN SP retrun value" && 0 );
		}
	} else {
		// 디비 SP 오류...
	}
	return false;
}

bool CThreadGUILD::Query_LoginClanMember ( char *szCharName, int iSenderSockIDX )
{
//-------------------------------------
// 2006.06.30/김대성/추가 - SQL 쿼리문조작을 이용한 해킹방지
	AlphabetCvt(szCharName, strlen(szCharName));
//-------------------------------------

	if ( this->m_pSQL->QuerySQL( "{call ws_ClanCharGET(\'%s\')}", szCharName ) ) {
		if ( this->m_pSQL->GetNextRECORD() ) {
			// 클랜 있다.
			DWORD dwClanID   = (DWORD)this->m_pSQL->GetInteger(0);
			int   iContribute = this->m_pSQL->GetInteger(1);
			CClan *pClan = this->Find_CLAN( dwClanID );
			if ( NULL == pClan ) {
				// 클렌 로딩...
				pClan = this->Load_CLAN( dwClanID );
				if ( NULL == pClan )
					return false;
			} 

			return pClan->LogIn_ClanUSER( szCharName, iSenderSockIDX, iContribute );
		}
	}

	return false;
}

bool CThreadGUILD::Query_UpdateClanDATA ( char *szField, t_PACKET *pPacket )//BYTE btAdjType, int iAdjValue)
{
	long   iResultSP=-99;
	SDWORD cbSize1=SQL_NTS;

	int iAdjValue = pPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue;

	this->m_pSQL->SetParam_long( 1, iResultSP, cbSize1 );
	if ( this->m_pSQL->QuerySQL( "{?=call ws_ClanUPDATE(%d,\'%s\',%d)}", pPacket->m_gsv_ADJ_CLAN_VAR.m_dwClanID, szField, pPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue ) ) {
		int iResult;
		__int64 biResult;
		while ( this->m_pSQL->GetMoreRESULT() ) {
			if ( this->m_pSQL->BindRESULT() ) {
				if ( this->m_pSQL->GetNextRECORD() ) {
					// 성공...
					// dwClanID = (DWORD)this->m_pSQL->GetInteger(0);
					iResult = this->m_pSQL->GetInteger(0);
					biResult = this->m_pSQL->GetInt64(0);
				}
			}
		}

		if ( iResultSP ) {
			// success
			switch( pPacket->m_gsv_ADJ_CLAN_VAR.m_btVarType ) {
				case CLVAR_INC_LEV	:
					//pClan->m_nClanLEVEL = iResult;//this->m_pSQL->GetInteger(0);
					if ( iResult > MAX_CLAN_LEVEL )
						iResult = MAX_CLAN_LEVEL;
					pPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue = iResult;
					break;
				case CLVAR_ADD_SCORE:
					//pClan->m_iClanSCORE = iResult;//this->m_pSQL->GetInteger(0);
					pPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue = iResult;
					break;
				case CLVAR_ADD_ZULY	:
					// bigint
					//pClan->m_biClanMONEY = biResult;//this->m_pSQL->GetInt64(0);
					pPacket->m_gsv_ADJ_CLAN_VAR.m_biResult = biResult;
					break;

#ifdef	__CLAN_INFO_ADD
					//	저장된 결과를 월드 서버에 전송 : 김영환 : 띠블..DB 저장을 게임서버에서 처리하냐.. 엉망이야...
				case CLVAR_SET_RANKPOINT:
					pPacket->m_gsv_ADJ_CLAN_VAR.m_iAdjValue = iResult;
					break;
#endif

			}
			g_LOG.CS_ODS( 0xffff, "update clan data : %d / %s, %d\n", pPacket->m_gsv_ADJ_CLAN_VAR.m_dwClanID, szField, iAdjValue );
			return true;
		} else {
			// failed
			g_LOG.CS_ODS( 0xffff, "failed2 update clan data : %d / %s, %d\n", pPacket->m_gsv_ADJ_CLAN_VAR.m_dwClanID, szField, iAdjValue );
		}
	} else {
		// failed
		g_LOG.CS_ODS( 0xffff, "SP error update clan data : %d / %s, %d\n", pPacket->m_gsv_ADJ_CLAN_VAR.m_dwClanID, szField, iAdjValue );
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
bool CThreadGUILD::Query_UpdateClanBINARY( DWORD dwClanID, BYTE *pDATA, unsigned int uiSize )
{
	long   iResultSP=-99;
	SDWORD cbSize1=SQL_NTS;

	this->m_pSQL->SetParam_long( 1, iResultSP, cbSize1 );
	this->m_pSQL->BindPARAM( 2, (BYTE*)pDATA, uiSize );

	if ( this->m_pSQL->QuerySQL( "{?=call ws_ClanBinUPDATE(%d,?)}", dwClanID ) ) {
		while ( this->m_pSQL->GetMoreRESULT() ) {
			;
		}
		switch( iResultSP ) {
			case 1 :	// 성공
				g_LOG.CS_ODS( 0xffff, "update clan binary : %d \n", dwClanID );
				return true;
				//case -1 :	// ????
				//	g_LOG.CS_ODS( 0xffff, "not clan user name :  %s \n", szCharName);
				//	break;
			case 0 :	// 디비 오류
				g_LOG.CS_ODS( 0xffff, "update clan binary failed : %d \n", dwClanID );
				break;
			default :
				assert( "invalid ws_ClanBinUPDATE SP retrun value" && 0 );
		}
	} else {
		// 디비 SP 오류...
		g_LOG.CS_ODS( 0xffff, "Query ERROR :: ws_ClanBinUPDATE( %d ) \n", dwClanID );
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
WORD CThreadGUILD::Query_UpdateClanMARK( CClan *pClan, WORD wMarkCRC, BYTE *pDATA, unsigned int uiSize )
{
	long   iResultSP=-99;
	SDWORD cbSize1=SQL_NTS;

	this->m_pSQL->SetParam_long( 1, iResultSP, cbSize1 );
	this->m_pSQL->BindPARAM( 2, (BYTE*)pDATA, uiSize );

	// 주의~~wMarkCRC로 될경우 디비에 smallint로 잡혀있어 32767 보다 클경우 디비 오류~~~
	if ( this->m_pSQL->QuerySQL( "{?=call ws_ClanMarkUPDATE(%d,%d,%d,?)}", pClan->m_dwClanID, (short)(wMarkCRC), uiSize ) ) {
		while ( this->m_pSQL->GetMoreRESULT() ) {
			;
		}
		switch( iResultSP ) {
			case 1 :	// 성공
				g_LOG.CS_ODS( 0xffff, "update clan mark : %d \n", pClan->m_dwClanID );
				if ( this->m_pSQL->QuerySQL( "select dateMarkREG from tblWS_CLAN where intID=%d", pClan->m_dwClanID ) ) {
					if ( this->m_pSQL->GetNextRECORD() ) {
						sqlTIMESTAMP sTimeStamp;
						this->m_pSQL->GetTimestamp(0, &sTimeStamp);

						pClan->m_RegTIME.m_wYear = sTimeStamp.m_wYear ;
						pClan->m_RegTIME.m_btMon = sTimeStamp.m_btMon ;
						pClan->m_RegTIME.m_btDay = sTimeStamp.m_btDay ;
						pClan->m_RegTIME.m_btHour= sTimeStamp.m_btHour;
						pClan->m_RegTIME.m_btMin = sTimeStamp.m_btMin ;
						pClan->m_RegTIME.m_btSec = sTimeStamp.m_btSec ;
					}
				}
				return 0;
			case 0 :	// 날짜비교에서 갱신 실패
				return RESULT_CLANMARK_TOO_MANY_UPDATE;
			case -1 :	// 디비 오류
				g_LOG.CS_ODS( 0xffff, "update clan mark failed : %d \n", pClan->m_dwClanID );
				return RESULT_CLANMARK_DB_ERROR;
			default :
				assert( "invalid ws_ClanBinMARK SP retrun value" && 0 );
		}
	} else {
		// 디비 SP 오류...
		g_LOG.CS_ODS( 0xffff, "Query ERROR :: ws_ClanMarkUPDATE( %d ) \n", pClan->m_dwClanID );
	}
	return RESULT_CLANMAKR_SP_ERROR;
}

//-------------------------------------------------------------------------------------------------
// #endif