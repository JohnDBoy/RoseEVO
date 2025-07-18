/*
$Header: /Client/Network/RecvPACKET.cpp 822   07-05-14 1:48p Raning $
*/
#include "stdAFX.h"
#include "CNetwork.h"
#include "Game.h"
#include "CCamera.h"
#include "IO_Terrain.h"
#include "Game_FUNC.h"
#include "JCommandState.h"	

#include "../CClientStorage.h"
#include "../CJustModelAVT.h"
#include "../SqliteDB.h"
#include "../Country.h"

#include "../System/CGame.h"
#include "../GameData/ServerList.h"
#include "../GameData/CClan.h"
#include "../GameData/CBank.h"
#include "../GameData/CParty.h"
#include "../GameData/CSeparate.h"
#include "../GameData/CGameDataCreateAvatar.h"
#include "../GameData/CUpgrade.h"
#include "../GameCommon/Skill.h"
#include "../Common/IO_STB.h"
#include "../Common/IO_Quest.h"
#include "../common/Calculation.h"
#include "../Util/CCheat.h"
#include "../GameProc/CDayNNightProc.h"

#include "../Interface/it_MGR.h"
#include "../Interface/TypeResource.h"	
#include "../Interface/CHelpMgr.h"
#include "../Interface/CUIMediator.h"
#include "../Interface/ExternalUI/CExternalUI.h"
#include "../Interface/ExternalUI/CLogin.h"
#include "../Interface/ExternalUI/ExternalUILobby.h"
#include "../Interface/ExternalUI/CSelectChannel.h"
#include "../Interface/ClanMarkTransfer.h"
#include "../Interface/ClanMarkManager.h"

#include "../Interface/Dlgs/DeliveryStoreDlg.h"
#include "../Interface/Dlgs/ChattingDlg.h"
#include "../interface/dlgs/CQuestDlg.h"
#include "../interface/dlgs/CPartyDlg.h"
#include "../interface/dlgs/DealDlg.h"
#include "../interface/dlgs/MakeDlg.h"
#include "../interface/dlgs/CSystemMsgDlg.h"
#include "../interface/dlgs/ExchangeDlg.h"
#include "../interface/dlgs/CCommDlg.h"
#include "../interface/dlgs/CSeparateDlg.h"
#include "../interface/DLGs/ClanWar_BossDlg.h"

#include "../interface/icon/ciconitem.h"

#include "../interface/command/CTCmdHotExec.h"
#include "../interface/command/CTCmdNumberInput.h"
#include "../interface/command/UICommand.h"

#include "../Util/LogWnd.h"

#include "../Event/QuestRewardQueue.h"
#include "../ObjFixedEvent.h"

#include "../Event/Quest_Func.h"
#include "GameProc/LiveCheck.h"
#include "GameProc/DelayedExp.h"

#include "../Tutorial/TutorialEventManager.h"
#include "../GameProc/UseItemDelay.h"
#include "../Util/SystemUtil.h"

#include "../interface/dlgs/CMsgBox.h"
WORD myid = 0;
WORD pickupitem = 0;
// 서버에서 받은 전역 번수들 설정...
void SetServerVAR( tagVAR_GLOBAL *pVAR )
{
	// short	m_nWorld_PRODUCT;					// 제조 관련
	// DWORD	m_dwUpdateTIME;						// 갱신된 시간.
	// short	m_nWorld_RATE;						// 경제 관련 :: 세계물가 80~140
	// BYTE		m_btTOWN_RATE;						// 마을 물가					80~140
	// BYTE		m_btItemRATE[ MAX_PRICE_TYPE ];		// 아이템별 물가				1~127

	::Set_WorldPROD( pVAR->m_nWorld_PRODUCT );
	::Set_WorldRATE( pVAR->m_nWorld_RATE );

	if ( g_pTerrain ) {
		g_pTerrain->m_Economy.m_dwUpdateTIME = pVAR->m_dwUpdateTIME;
		g_pTerrain->m_Economy.m_btTOWN_RATE = pVAR->m_btTOWN_RATE;
		::CopyMemory( g_pTerrain->m_Economy.m_btItemRATE, pVAR->m_btItemRATE, sizeof(BYTE)*MAX_PRICE_TYPE);
	}

	g_GameDATA.m_iPvPState  = pVAR->m_dwGlobalFLAGS & ZONE_FLAG_PK_ALLOWED;
}



//-------------------------------------------------------------------------------------------------
CRecvPACKET::CRecvPACKET ()
{
	m_pRecvPacket = (t_PACKET*) new char[ MAX_PACKET_SIZE ];
}
CRecvPACKET::~CRecvPACKET()
{
	SAFE_DELETE_ARRAY( m_pRecvPacket );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_srv_ERROR ()
{
	switch( m_pRecvPacket->m_lsv_ERROR.m_wErrorCODE ) 
	{
	case 0:
		break;
	default :
		;
	} 
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_GM_COMMAND ()
{
	switch( m_pRecvPacket->m_gsv_GM_COMMAND.m_btCMD ) 
	{
	case GM_CMD_SHUT   :
		{
			short nOffset=sizeof( gsv_GM_COMMAND );

			DWORD dwBlockTime = m_pRecvPacket->m_gsv_GM_COMMAND.m_wBlockTIME;
			DWORD dwBlockTimeMilSecond = dwBlockTime * 60 * 1000;

			char *szName = Packet_GetStringPtr ( m_pRecvPacket, nOffset );
			char szTemp[128];
			///채팅 블록 해제
			if( dwBlockTime <= 0 )
			{
				if( szName )
					sprintf( szTemp,FORMAT_STR_CANCEL_BLOCK_CHAT, szName);
				else
					sprintf( szTemp,STR_CANCEL_BLOCK_CHAT);

				g_itMGR.OpenMsgBox(szTemp);
				CChatDLG* pDlg = g_itMGR.GetChatDLG();
				if( pDlg )
					pDlg->SetChatUnBlock();

			}
			else///채팅 블록
			{
				if( szName )
					sprintf( szTemp,F_STR2_CHAT_BLOCKED_BY_GM, szName ,dwBlockTime );
				else
					sprintf( szTemp,FORMAT_STR_BLOCK_CHAT,dwBlockTime );

				g_itMGR.OpenMsgBox(szTemp);
				CChatDLG* pDlg = g_itMGR.GetChatDLG();
				if( pDlg )
					pDlg->SetChatBlock( dwBlockTimeMilSecond );
			}					
			break;
		}
	case GM_CMD_LOGOUT :
		{
			CTCmdExit Command;
			Command.Exec( NULL );
			//CTCommand* pCmd = new CTCmdExit;
			//short nOffset=sizeof( gsv_GM_COMMAND );
			//char *szName = Packet_GetStringPtr ( m_pRecvPacket, nOffset );
			//char szTemp[128];
			//if( szName )
			//	sprintf( szTemp,FORMAT_STR_GM_BAN_USER, szName );
			//else
			//	sprintf( szTemp,STR_GM_BAN_USER);
			//
			//g_itMGR.OpenMsgBox(szTemp,CMsgBox::BT_OK, true, 0, pCmd ,NULL);
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SET_GLOBAL_VAR ()
{
	SetServerVAR( (tagVAR_GLOBAL*)&m_pRecvPacket->m_pDATA[ sizeof(t_PACKETHEADER) ] );

	g_itMGR.AppendChatMsg(STR_CHANGE_PRICES, IT_MGR::CHAT_TYPE_SYSTEM );

	if( CDealData::GetInstance().GetTradeItemCnt() )
	{
		CTCmdCloseStore* pCmd = new CTCmdCloseStore;
		char szTemp[256];
		sprintf( szTemp,"%s %s",STR_CHANGE_PRICES,STR_QUERY_STOP_TRADE );
		g_itMGR.OpenMsgBox(szTemp , CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd , NULL );
	}

}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SET_GLOVAL_FLAG ()
{
	g_GameDATA.m_iPvPState = m_pRecvPacket->m_gsv_SET_GLOBAL_FLAG.m_dwGlobalFLAGS & ZONE_FLAG_PK_ALLOWED;
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_srv_ANNOUNCE_TEXT ()
{
#pragma message ( "TODO:: Notice received treatment..." __FILE__ )
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_ANNOUNCE_CHAT ()
{
	short nOffset=sizeof( t_PACKETHEADER );
	char *szMSG = NULL;
	szMSG = CStr::Printf("%s", Packet_GetStringPtr( m_pRecvPacket, nOffset ) );
	char *szName = Packet_GetStringPtr( m_pRecvPacket, nOffset );			

	if( g_Cheat.DoSpecialCheat( szMSG ) )
		return;

	if( szMSG[ 0 ] == '/' )
		return;

	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_SYSTEMMSG );
	if( pDlg )
	{
		std::string Title = STR_NOTIFY_03;
		if( szName )
		{
			Title.append(":");				
			Title.append( szName );
		}
		CSystemMsgDlg* pSysMsgDlg = (CSystemMsgDlg*) pDlg;
		pSysMsgDlg->SetMessage( Title.c_str(),szMSG, CSystemMsgDlg::MSG_TYPE_NOTICE, g_dwWHITE, 15000 , g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ]);

		std::string ChatMsg = STR_NOTIFY_03;
		if( szName )///추가
		{
			ChatMsg.append(":");
			ChatMsg.append( szName );
			ChatMsg.append(" ");
		}			

		ChatMsg.append( szMSG );
		g_itMGR.AppendChatMsg( ChatMsg.c_str(), IT_MGR::CHAT_TYPE_NOTICE );
	}
}

//-------------------------------------------------------------------------------------------------
bool CRecvPACKET::Recv_lsv_LOGIN_REPLY ()
{
	CLogin* pLogin = (CLogin*)g_EUILobby.GetEUI( EUI_LOGIN );

	if( pLogin == NULL )
		return false;

	g_EUILobby.HideMsgBox();


	CServerList& ServerList = CServerList::GetInstance();
	
	///채널 보이기, 숨기기 구분
//	if( m_pRecvPacket->m_srv_LOGIN_REPLY.m_btResult & 0x80 )
//		ServerList.HideChannel();
//	else
//		ServerList.ShowChannel();

	//홍근 : 채널 항상 보이기로 세팅 한다.
	ServerList.ShowChannel();

	///채널 보이기, 숨기기 정보를 뺀상태에서 
	BYTE btResult = m_pRecvPacket->m_srv_LOGIN_REPLY.m_btResult & ~0x80;

	if( RESULT_LOGIN_REPLY_OK != btResult 
		&& RESULT_LOGIN_REPLY_TAIWAN_OK != btResult
		&& RESULT_LOGIN_REPLY_JAPAN_OK != btResult )
	{

		CTCommand* pCmd = NULL;

#ifdef __AUTOLOGIN	

		/*
		CTDialog* pDlg = g_EUILobby.GetEUI( EUI_LOGIN ) ;
		assert( pDlg );
		if( pDlg && pDlg->IsVision()==false )
		{
			pDlg->Show();
		}*/

		//자동 로그인 실패시에는 클라이언트 종료.
		if( g_GameDATA.m_dwSeq && g_GameDATA.m_szSessionKey )
		{
			pCmd = new CTCmdExit;
		}
#endif
		switch( btResult ) 
		{
		case RESULT_LOGIN_REPLY_NO_RIGHT_TO_CONNECT:
			g_EUILobby.ShowMsgBox(STR_LOGIN_REPLY_NO_RIGHT_TO_CONNECT, CTMsgBox::BT_OK ,true , pLogin->GetDialogType(), pCmd );
			return false;
		case RESULT_LOGIN_REPLY_FAILED               :// 오류
			g_EUILobby.ShowMsgBox(STR_COMMON_ERROR,CTMsgBox::BT_OK ,true , pLogin->GetDialogType(), pCmd );
			return false;
		case RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT    :// 계정 없다.				
			g_EUILobby.ShowMsgBox(STR_NOT_FOUND_ACCOUNT, CTMsgBox::BT_OK, true, pLogin->GetDialogType(), pCmd );
			return false;
		case RESULT_LOGIN_REPLY_INVALID_PASSWORD     :// 비번 오류
			g_EUILobby.ShowMsgBox(STR_INVALID_PASSWORD,CTMsgBox::BT_OK ,true ,pLogin->GetDialogType(), pCmd );
			return false;
		case RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN     :// 이미 로그인 중이다
			g_EUILobby.ShowMsgBox(STR_ALREADY_LOGGEDIN,CTMsgBox::BT_OK ,true , pLogin->GetDialogType(), pCmd );
			return false;
		case RESULT_LOGIN_REPLY_REFUSED_ACCOUNT      :// 서버에서 거부된 계정이다.혹은 블럭된 계정입니다.
			g_EUILobby.ShowMsgBox(STR_REFUSED_ACCOUNT,CTMsgBox::BT_OK ,true , pLogin->GetDialogType(), pCmd );
			return false;
		case RESULT_LOGIN_REPLY_NEED_CHARGE          :// 충전이 필요한다
			g_EUILobby.ShowMsgBox( STR_BILL_AT_ROSEONLINE_HOMEPAGE	,CTMsgBox::BT_OK ,true ,pLogin->GetDialogType(), pCmd ); //로즈온라인 홈페이지에서 결제 신청을 해주세요~
			return false;		   			
		case RESULT_LOGIN_REPLY_TOO_MANY_USER:
			g_EUILobby.ShowMsgBox(STR_LOGIN_REPLY_TOO_MANY_USER,CTMsgBox::BT_OK ,true ,pLogin->GetDialogType(), pCmd );
			return false;
		case RESULT_LOGIN_REPLY_NO_REAL_NAME:
			g_EUILobby.ShowMsgBox(STR_RESULT_LOGIN_REPLY_NO_REAL_NAME, CTMsgBox::BT_OK ,true ,pLogin->GetDialogType(), pCmd );
			return false;
		case RESULT_LOGIN_REPLY_OUT_OF_IP:
			g_EUILobby.ShowMsgBox( STR_INSUFFICIENCY_IP, CTMsgBox::BT_OK ,true ,pLogin->GetDialogType(), pCmd); //"접속 가능한 ip 수가 초과 되었습니다"
			return false;
		case RESULT_LOGIN_REPLY_TAIWAN_FAILED:
			switch( m_pRecvPacket->m_srv_LOGIN_REPLY.m_wPayType )
			{
			case 7:
				g_EUILobby.ShowMsgBox(STR_NOT_FOUND_ACCOUNT,CTMsgBox::BT_OK ,true , pLogin->GetDialogType(), pCmd );
				break;
			default:
				g_EUILobby.ShowMsgBox(STR_COMMON_ERROR,CTMsgBox::BT_OK ,true , pLogin->GetDialogType(), pCmd );
				break;
			}

			return false;
		default:
			g_EUILobby.ShowMsgBox("Login Failed", CTMsgBox::BT_OK ,true ,pLogin->GetDialogType(), pCmd );
			return false;
		}
	}
	short nOffset=sizeof(srv_LOGIN_REPLY);
	char *szServerName;
	DWORD *pServerID;

	szServerName = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	pServerID = (DWORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(DWORD) );

	if ( !szServerName || !pServerID ) 
	{
		CTCommand* pCmd = new CTCmdExit;
		g_EUILobby.ShowMsgBox(STR_INSPECT_ALL_SEVER,CTMsgBox::BT_OK ,true , pLogin->GetDialogType() ,pCmd);
		return false;
	}


	///
	/// Get server list
	///

	CExternalUIManager& refEUIManager = g_EUILobby.GetExternalUIManager();

	//	bool	bSetFirstServerID = false;
	//	int		iFirstServerID  = 0;

	DWORD dwServerID = 0;
	short nServerID=0;
	DWORD dwRight = m_pRecvPacket->m_srv_LOGIN_REPLY.m_wRight ;
	CGame::GetInstance().SetRight( dwRight );
	CGame::GetInstance().SetPayType( m_pRecvPacket->m_srv_LOGIN_REPLY.m_wPayType );

	std::map< BYTE, pair< DWORD, std::string> > TempServerList;




	///이전에 사용하던 인터페이스창이 있다면 Observer에서 삭제한다.
	if( CTDialog* pUI = refEUIManager.GetEUI( EUI_SELECT_SERVER ) )
	{
		CSelectServer* pSelectServer = (CSelectServer*)pUI;
		ServerList.DeleteObserver( (IObserver*)pSelectServer );
	}


	//EUI_SELECT_SERVER,
	CSelectServer* pSelectServer = new CSelectServer;
	refEUIManager.AddEUI( EUI_SELECT_SERVER, pSelectServer );
	pSelectServer->SetEUIObserver( &refEUIManager );

	if( CServerList::GetInstance().IsShowChannel() )
		pSelectServer->Create("DlgSelSvr");
	else
		pSelectServer->Create("DlgSelOnlySvr");///채널을 감춘다.

	POINT	ptNew;
	ptNew.x = g_pCApp->GetWIDTH() / 2 - pSelectServer->GetWidth()/2;
	ptNew.y = g_pCApp->GetHEIGHT() / 3;
	pSelectServer->MoveWindow( ptNew );

	ServerList.AddObserver( pSelectServer );
	ServerList.ClearWorldServerList();

	//CTDialog* pUI = refEUIManager.GetEUI( EUI_SELECT_SERVER );
	//CSelectServer* pSelectServer = (CSelectServer*)pUI;
	//
	//pSelectServer->ClearServerList();
	//pSelectServer->ClearChannelList();

	while( szServerName && pServerID ) {
		if ( !dwServerID )
			dwServerID = *pServerID;

		nServerID ++;

		LogString (LOG_DEBUG, "\n\n[[ Server: %s, ID: %d ]]\n\n\n", szServerName, *pServerID );

		///서버네임에@에 붙은경우 개발자 권한에서만 리스트에 보여준다.
		if( g_GameDATA.m_bForOpenTestServer )
		{
			if( szServerName[0] == '@' && strlen(szServerName) >= 2 )
				TempServerList.insert( std::map< BYTE, pair< DWORD, std::string> >::value_type( (BYTE)( 128 + (*pServerID) ), make_pair( *pServerID, &szServerName[1] ) ) );
		}
		else
		{
			if( szServerName[0] == '@')
			{  
				if( dwRight >= CHEAT_MM) 
				{
					if( strlen(szServerName) >= 2 )
						TempServerList.insert( std::map< BYTE, pair< DWORD, std::string> >::value_type( (BYTE)( 128 + (*pServerID) ) , make_pair( *pServerID, &szServerName[0] ) ) );
				}
			}
			else
			{
				if( strlen(szServerName) >= 2 )
					TempServerList.insert( std::map< BYTE, pair< DWORD, std::string> >::value_type( szServerName[0], make_pair( *pServerID, &szServerName[1] ) ) );

			}
		}

		szServerName = Packet_GetStringPtr( m_pRecvPacket, nOffset);
		pServerID = (DWORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(DWORD) );
	}

	std::map< BYTE, pair< DWORD, std::string> >::iterator iter;
	int iCount = 0;
	for( iter = TempServerList.begin(); iter != TempServerList.end(); ++iter, ++iCount )
		ServerList.AddWorldServerList( iCount, iter->second.first, (char*)(iter->second.second.c_str()) );


	g_EUILobby.HideMsgBox();

	///한프레임을 그려주어야 Loading이 나온다.
	g_EUILobby.ShowMsgBox( "Loading...",CTMsgBox::BT_NONE, true, pSelectServer->GetDialogType() );
	//CGame::GetInstance().UpdateCurrentState();
	//->

	CGame::GetInstance().ChangeState( CGame::GS_SELECTSVR );
	g_EUILobby.HideMsgBox();

	g_EUILobby.CreateServerListDlg();
	g_EUILobby.SendReqFirstServerChannelList();

	///접속에 성공했다

	if( CTDialog* pDlg = g_EUILobby.GetEUI( EUI_LOGIN ) )
	{
		CLogin* p = (CLogin*)pDlg;
		if( g_ClientStorage.IsSaveLastConnectID() )
			g_ClientStorage.SaveLastConnectID( p->GetID() );
		else
			g_ClientStorage.SaveLastConnectID( NULL );

		g_ClientStorage.SaveOptionLastConnectID();
	}


	return true;
}

//-------------------------------------------------------------------------------------------------
int CRecvPACKET::Recv_lsv_SELECT_SERVER ()
{
	LogString (LOG_DEBUG, "Recv_lsv_SELECT_SERVER:: Result: %d ", m_pRecvPacket->m_lsv_SELECT_SERVER.m_btResult);

	///
	/// Recv proc
	/// 내부에서 정상 동작 여부 확인...
	///
	CSelectServer* pSelectServer = (CSelectServer*)g_EUILobby.GetEUI( EUI_SELECT_SERVER );

	if( pSelectServer == NULL )
		return 0;

	pSelectServer->RecvSelectServer( m_pRecvPacket );

	if ( m_pRecvPacket->m_lsv_SELECT_SERVER.m_btResult != RESULT_SELECT_SERVER_OK	) {
		return 0;
	}

	short nOffset = sizeof( lsv_SELECT_SERVER );
	char *szServerIP;
	WORD *pServerPort;

	szServerIP = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	pServerPort= (WORD*)Packet_GetDataPtr  ( m_pRecvPacket, nOffset, sizeof(WORD) );

	// 소켓 동작 중에는 주소나 포트를 바꿀수 없다.
	this->m_WSV_IP.Set( szServerIP );
	this->m_wWSV_PORT  = *pServerPort;
	this->m_dwWSV_ID   = m_pRecvPacket->m_lsv_SELECT_SERVER.m_dwIDs[0];

	LogString (LOG_DEBUG, "Recv_lsv_SELECT_SERVER:: Result: %d, IP: %s, Port: %d ", m_pRecvPacket->m_lsv_SELECT_SERVER.m_btResult, szServerIP, *pServerPort);

	this->DisconnectFromServer( NS_DIS_FORM_LSV );	

	// 임시
	//DestroyWaitDlg();

	return m_pRecvPacket->m_lsv_SELECT_SERVER.m_dwIDs[1];
}

//-------------------------------------------------------------------------------------------------
int CRecvPACKET::Recv_srv_JOIN_SERVER_REPLY ()
{
	char *szResult[] = {
		"RESULT_JOIN_SERVER_OK",
			"RESULT_JOIN_SERVER_FAILED",
			"RESULT_JOIN_SERVER_TIME_OUT",
			"RESULT_JOIN_SERVER_INVALID_PASSWORD",
			"RESULT_JOIN_SERVER_ALREADY_LOGGEDIN"
	} ;

	LogString (LOG_DEBUG, "Recv_srv_JOIN_SERVER_REPLY:: Result: %d [ %s ]", m_pRecvPacket->m_srv_JOIN_SERVER_REPLY.m_btResult, szResult[ m_pRecvPacket->m_srv_JOIN_SERVER_REPLY.m_btResult ]);

	if ( RESULT_JOIN_SERVER_OK != m_pRecvPacket->m_srv_JOIN_SERVER_REPLY.m_btResult ) {
		return 0;
	}

	return m_pRecvPacket->m_srv_JOIN_SERVER_REPLY.m_dwID;
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_INIT_DATA ()
{
	m_pRecvPacket->m_gsv_INIT_DATA.m_iRandomSEED;
	m_pRecvPacket->m_gsv_INIT_DATA.m_wRandomINDEX;
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_wsv_CHAR_LIST ()
{
	CSelectAvata* pSelectAvata = (CSelectAvata*)g_EUILobby.GetEUI( EUI_SELECT_AVATA );

	pSelectAvata->RecvAvataList( m_pRecvPacket );

	g_EUILobby.CloseWaitAvataListDlg();	

	if( CGame::GetInstance().GetCurrStateID() == CGame::GS_SELECTSVR )
		CGame::GetInstance().ChangeState( CGame::GS_PREPARESELECTAVATAR );
	else
		CGame::GetInstance().ChangeState( CGame::GS_SELECTAVATAR );
}

bool CRecvPACKET::Recv_wsv_CREATE_CHAR ()
{

	CCreateAvata* pCreateAvata = (CCreateAvata*)g_EUILobby.GetEUI( EUI_CREATE_AVATA );
	if( pCreateAvata == NULL )
		return false;

	if( pCreateAvata->RecvCreateAvata( m_pRecvPacket ) )
	{
		g_EUILobby.CloseWaitJoinServerDlg();		
		g_pNet->Send_cli_CHAR_LIST();
		return true;
	}

	return false;
}

void CRecvPACKET::Recv_wsv_MOVE_SERVER ()
{
	short nOffset = sizeof(wsv_MOVE_SERVER);
	char *szServerIP  =Packet_GetStringPtr( m_pRecvPacket, nOffset);

	this->m_GSV_IP.Set( szServerIP );
	this->m_wGSV_PORT    = m_pRecvPacket->m_wsv_MOVE_SERVER.m_wPortNO;
	this->m_dwGSV_IDs[0] = m_pRecvPacket->m_wsv_MOVE_SERVER.m_dwIDs[0];
	this->m_dwGSV_IDs[1] = m_pRecvPacket->m_wsv_MOVE_SERVER.m_dwIDs[1];
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_JOIN_ZONE ()
{
	if( !g_pAVATAR )
		return;
myid=m_pRecvPacket->m_gsv_JOIN_ZONE.m_wServerObjectIndex;
	// 내 아바타의 인덱스...
	g_pObjMGR->Set_ServerObjectIndex (g_pAVATAR->Get_INDEX(), m_pRecvPacket->m_gsv_JOIN_ZONE.m_wServerObjectIndex );

	/// 죽었을경우.. 현재 HP와 패널티 경험치 세팅..
	/// 현재 경험치 sETTING
	g_pAVATAR->SetCur_EXP( m_pRecvPacket->m_gsv_JOIN_ZONE.m_lCurEXP );
	g_pAVATAR->Set_HP( m_pRecvPacket->m_gsv_JOIN_ZONE.m_nCurHP );
	g_pAVATAR->Set_MP( m_pRecvPacket->m_gsv_JOIN_ZONE.m_nCurMP );
	g_pAVATAR->m_GrowAbility.m_lPenalEXP = m_pRecvPacket->m_gsv_JOIN_ZONE.m_lPenalEXP;

	/// 회복을 위한 시간 리셋..
	g_pAVATAR->ClearTimer();

	// set server var 함수 만들어어함...
	SetServerVAR( &m_pRecvPacket->m_gsv_JOIN_ZONE.m_VAR );

	g_pObjMGR->ResetTime();
	//	g_pCMouse->SetEnable();
	//	g_pCKeybd->SetEnable();

	g_DayNNightProc.SetWorldTime( g_pTerrain->GetZoneNO(), m_pRecvPacket->m_gsv_JOIN_ZONE.m_dwAccWorldTIME );	

	/// 소환몹 리스트 클리어..
	g_pAVATAR->ClearSummonedMob();



	//----------------------------------------------------------------------------------------------------	
	/// @brief 존데이터에 설정된 트리거실행
	//----------------------------------------------------------------------------------------------------
	g_pAVATAR->SetTeamInfo( m_pRecvPacket->m_gsv_JOIN_ZONE.m_iTeamNO );
	/// 퀘스트 보상 실행.	
	/// 클라이언트에서 실행하는것이 아니라.. Quest_Reply가 오게 수정 7/28
	/*char* pTriggerName = ZONE_JOIN_TRIGGER( g_pTerrain->GetZoneNO() );
	t_HASHKEY HashKEY = ::StrToHashKey( pTriggerName );
	eQST_RESULT bResult = g_QuestList.CheckQUEST( g_pAVATAR, HashKEY, true );*/

	//----------------------------------------------------------------------------------------------------	
	/// @brief 존에 접속하며 나올효과
	//----------------------------------------------------------------------------------------------------
	SE_CharJoinZone( g_pAVATAR->Get_INDEX() );

	//----------------------------------------------------------------------------------------
	/// 날씨 이펙트
	//----------------------------------------------------------------------------------------
	int iZoneWeather = ZONE_WEATHER_TYPE( g_pTerrain->GetZoneNO() );
	SE_WeatherEffect( g_pAVATAR->Get_INDEX(), iZoneWeather );


	//----------------------------------------------------------------------------------------------------	
	/// @brief 파티중일경우에는 SvrIndex를 바꾸어준다.
	//----------------------------------------------------------------------------------------------------
	if( CParty::GetInstance().HasParty() )
	{
		tag_PARTY_MEMBER MemberInfo;
		MemberInfo.m_dwUserTAG  = g_pAVATAR->GetUniqueTag();
		MemberInfo.m_wObjectIDX = m_pRecvPacket->m_gsv_JOIN_ZONE.m_wServerObjectIndex	;
		/* 나일경우는 무시한다.
		MemberInfo.m_nHP		= g_pAVATAR->Get_HP();
		MemberInfo.m_nMaxHP		= g_pAVATAR->Get_MaxHP();
		MemberInfo.m_btRecoverHP = g_pAVATAR->Get_RecoverHP();
		MemberInfo.m_btRecoverMP = g_pAVATAR->Get_RecoverMP();
		MemberInfo.m_btCON		 = g_pAVATAR->Get_CON();
		MemberInfo.m_dwStatusFALG = g_pAVATAR->m_EndurancePack.GetStateFlag()
		*/
		CParty::GetInstance().ChangeMemberInfoByUserTag( MemberInfo );
	}


	//	CLiveCheck::GetSingleton().ResetTime();		// 2004.11.11, icarus: SRV_JOIN_SERVER_REPLY받았을 때로 이동..

	g_GameDATA.m_iReceivedAvatarEXP = g_pAVATAR->Get_EXP();

	::setModelBlinkCloseMode( g_pAVATAR->GetZMODEL(), false );

	//------------------------------------------------------------------------------------
	///박 지호: 여신소환 상태인 유저는 소환중인 이펙트를 설정한다. 
	//------------------------------------------------------------------------------------
	goddessMgr.SetProcess(goddessMgr.IsAvataState(),m_pRecvPacket->m_gsv_JOIN_ZONE.m_wServerObjectIndex,TRUE);

}


//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_REVIVE_REPLY ()
{
	if ( m_pRecvPacket->m_gsv_REVIVE_REPLY.m_nZoneNO > 0 )
	{
		if( g_pAVATAR )
			g_pAVATAR->m_nReviveZoneNO = m_pRecvPacket->m_gsv_REVIVE_REPLY.m_nZoneNO;
	}
}
void CRecvPACKET::Recv_gsv_SET_VAR_REPLY ()
{
	if( !g_pAVATAR )
		return;

	if ( m_pRecvPacket->m_gsv_SET_VAR_REPLY.m_btVarTYPE & REPLY_GSV_SET_VAR_FAIL_BIT ) {
		// TODO:: Parameter setting error !!!
		return;
	}

	int iValue = m_pRecvPacket->m_gsv_SET_VAR_REPLY.m_iValue;
	switch( m_pRecvPacket->m_gsv_SET_VAR_REPLY.m_btVarTYPE ) 
	{
	case SV_SEX		:	return g_pAVATAR->Set_SEX ( iValue );		
	case SV_CLASS	:	return g_pAVATAR->Set_JOB ( iValue );
	case SV_UNION	:	return g_pAVATAR->Set_UNION ( iValue );
	case SV_RANK	:	return g_pAVATAR->Set_RANK ( iValue );
	case SV_FAME	:	return g_pAVATAR->Set_FAME ( iValue );

	case SV_STR		:	return g_pAVATAR->Set_STR ( iValue );
	case SV_DEX		:	return g_pAVATAR->Set_DEX ( iValue );
	case SV_INT		:	return g_pAVATAR->Set_INT ( iValue );
	case SV_CON		:	return g_pAVATAR->Set_CON ( iValue );
	case SV_CHA		:	return g_pAVATAR->Set_CHARM ( iValue );
	case SV_SEN		:	return g_pAVATAR->Set_SENSE ( iValue );

	case SV_LEVEL	:	return g_pAVATAR->Set_LEVEL ( iValue );
	case SV_EXP		:	return g_pAVATAR->Set_EXP ( iValue );

	}

	//캐릭터 속성을 바꾸면 배울수 있는 스킬을 업데이트 한다.
	CTCmdUpdateLearnAbleSkill Cmd;
	Cmd.Exec(NULL);
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SELECT_CHAR ()
{
	short nOffset=sizeof( gsv_SELECT_CHAR );
	char *szName;
	szName = Packet_GetStringPtr( m_pRecvPacket, nOffset);

	LogString (LOG_NORMAL, "\n\n\n>>> AVATER( %s ) : Zone: %d, Pos: %f, %f <<<\n\n\n\n",
		szName, 
		m_pRecvPacket->m_gsv_SELECT_CHAR.m_nZoneNO,
		m_pRecvPacket->m_gsv_SELECT_CHAR.m_PosSTART.x, m_pRecvPacket->m_gsv_SELECT_CHAR.m_PosSTART.y);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 선택된 아바타 데이터 세팅..
	CGame& refGame = CGame::GetInstance();
	refGame.m_strAvataName = std::string( ( szName == NULL )? "NULL":szName );

	::CopyMemory( refGame.m_SelectedAvataInfo.m_PartITEM,			m_pRecvPacket->m_gsv_SELECT_CHAR.m_PartITEM,		sizeof( refGame.m_SelectedAvataInfo.m_PartITEM ) );
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_BasicINFO,		&m_pRecvPacket->m_gsv_SELECT_CHAR.m_BasicINFO,		sizeof(tagBasicINFO)	);
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_BasicAbility,		&m_pRecvPacket->m_gsv_SELECT_CHAR.m_BasicAbility,	sizeof(tagBasicAbility)	);
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_BasicAbility,		&m_pRecvPacket->m_gsv_SELECT_CHAR.m_BasicAbility,	sizeof(tagBasicAbility)	);
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_GrowAbility,		&m_pRecvPacket->m_gsv_SELECT_CHAR.m_GrowAbility,	sizeof(tagGrowAbility)	);
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_Skill,			&m_pRecvPacket->m_gsv_SELECT_CHAR.m_Skill,			sizeof(tagSkillAbility)	);
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_HotICONS,			&m_pRecvPacket->m_gsv_SELECT_CHAR.m_HotICONS,		sizeof(CHotICONS)		);
#ifdef __KRRE_NEW_AVATAR_DB
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_dwCoolTime,		&m_pRecvPacket->m_gsv_SELECT_CHAR.m_dwCoolTime,		sizeof(refGame.m_SelectedAvataInfo.m_dwCoolTime)		);
#endif	
	
	refGame.m_SelectedAvataInfo.m_btCharRACE     = m_pRecvPacket->m_gsv_SELECT_CHAR.m_btCharRACE;
	refGame.m_SelectedAvataInfo.m_nZoneNO		= m_pRecvPacket->m_gsv_SELECT_CHAR.m_nZoneNO;
	refGame.m_SelectedAvataInfo.m_PosSTART.x		= m_pRecvPacket->m_gsv_SELECT_CHAR.m_PosSTART.x;
	refGame.m_SelectedAvataInfo.m_PosSTART.y		= m_pRecvPacket->m_gsv_SELECT_CHAR.m_PosSTART.y;
	refGame.m_SelectedAvataInfo.m_nReviveZoneNO	= m_pRecvPacket->m_gsv_SELECT_CHAR.m_nReviveZoneNO;
	refGame.m_SelectedAvataInfo.m_dwUniqueTAG	= m_pRecvPacket->m_gsv_SELECT_CHAR.m_dwUniqueTAG;

}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_INVENTORY_DATA ()
{
	CGame& refGame = CGame::GetInstance();
	refGame.m_SelectedAvataINV.m_INV.m_i64Money = m_pRecvPacket->m_gsv_INVENTORY_DATA.m_INV.m_i64Money;

	memcpy( &refGame.m_SelectedAvataINV.m_INV.m_ItemLIST, 
			&m_pRecvPacket->m_gsv_INVENTORY_DATA.m_INV.m_ItemLIST, 
			sizeof( m_pRecvPacket->m_gsv_INVENTORY_DATA.m_INV.m_ItemLIST ) );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_QUEST_DATA()
{ 
	CGame& refGame = CGame::GetInstance();

	memcpy( &refGame.m_QuestData,
		&m_pRecvPacket->m_gsv_QUEST_DATA,
		sizeof( m_pRecvPacket->m_gsv_QUEST_DATA ) );

	g_EUILobby.CloseAvataListDlg();	

	CGame::GetInstance().CreateSelectedAvata();


	gsv_TELEPORT_REPLY data;
	data.m_nZoneNO = refGame.m_SelectedAvataInfo.m_nZoneNO;
	data.m_PosWARP.x = refGame.m_SelectedAvataInfo.m_PosSTART.x;
	data.m_PosWARP.y = refGame.m_SelectedAvataInfo.m_PosSTART.y;


	refGame.SetLoadingData(  data );
	refGame.ChangeState( CGame::GS_MOVEMAIN );

}

//20060811 홍근
void CRecvPACKET::Recv_gsv_QUEST_ONLY()
{ 
	if( !g_pAVATAR )
		return;

	g_pAVATAR->m_bQuestUpdate = true;

	::CopyMemory( &g_pAVATAR->m_Quests, &m_pRecvPacket->m_gsv_QUEST_ONLY.m_Quests, sizeof( tagQuestData ) );

	//지금은 클랜 변수만 사용하므로 주석 처리
/*
	///퀘스트 리스트 갱신
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_QUEST );
	if( pDlg && pDlg->IsVision() )
	{
		CQuestDlg* pQuestDlg = ( CQuestDlg* ) pDlg;
		pQuestDlg->UpdateQuestList();
	}
*/
}

//20060811 홍근
void CRecvPACKET::Recv_gsv_WISH_LIST()
{ 
	//지금은 미사용.
	/*
	if( !g_pAVATAR )	return;

	g_pAVATAR->m_bQuestUpdate = true;

	::CopyMemory ( &g_pAVATAR->m_WishLIST,		&m_pRecvPacket->m_gsv_WISH_LIST,				sizeof(tagWishLIST)		);

	///퀘스트 리스트 갱신
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_QUEST );
	if( pDlg && pDlg->IsVision() )
	{
		CQuestDlg* pQuestDlg = ( CQuestDlg* ) pDlg;
		pQuestDlg->UpdateQuestList();
	}
	*/
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CHEAT_CODE ()
{
	char *szCheatCode = m_pRecvPacket->m_gsv_CHEAT_CODE.m_szCheatCODE;
	g_Cheat.DoCheat( szCheatCode );
}


//-------------------------------------------------------------------------------------------------
/// 일단 정지상태에서만 동작한다..
void CRecvPACKET::Recv_gsv_SET_MOTION ()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_SET_MOTION.m_wObjectIDX, false );
	if ( pCHAR ) 
	{
		if( m_pRecvPacket->m_gsv_SET_MOTION.m_wIsSTOP )
		{
			pCHAR->SetCMD_STOP();
		}

		// 현재 이동속도유지(?) 혹시 이동 중일지 몰라서... 애니 속도는 디폴트... 
		pCHAR->Set_MOTION( m_pRecvPacket->m_gsv_SET_MOTION.m_nMotionNO, pCHAR->m_fCurMoveSpeed );
		::setRepeatCount( pCHAR->GetZMODEL(), 1 );

		pCHAR->SetUpdateMotionFlag( true );
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_TOGGLE ()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_TOGGLE.m_wObjectIDX, false );
	if ( pCHAR ) 
	{
		pCHAR->SetCMD_TOGGLE( m_pRecvPacket->m_gsv_TOGGLE.m_btTYPE );				

		/// 속도가 변했다면 속도 세팅
		if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_TOGGLE ) + sizeof(short) ) )
		{
			if( pCHAR->IsUSER() )
			{
				((CObjAVT*)pCHAR)->SetOri_RunSPEED( m_pRecvPacket->m_gsv_TOGGLE.m_nRunSPEED[0] );

				//20060626 홍근 카트 탑승시에 이동속도 재계산.
				((CObjAVT*)pCHAR)->Update_SPEED();
			}
		}	
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CHAT ()
{
	short nOffset = sizeof( gsv_CHAT );
	char *szMsg;

	CObjAVT *pCHAR = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_CHAT.m_wObjectIDX, false );
	if ( pCHAR ) 
	{
		szMsg = Packet_GetStringPtr( m_pRecvPacket, nOffset);

		if( szMsg[ 0 ] == '/' )
			return;

		g_itMGR.AppendChatMsg( CStr::Printf("%s>%s", pCHAR->Get_NAME(), szMsg), IT_MGR::CHAT_TYPE_ALL );

		g_UIMed.AddChatMsg( m_pRecvPacket->m_gsv_CHAT.m_wObjectIDX, szMsg );

#ifdef __VIRTUAL_SERVER
		g_Cheat.DoCheat( szMsg );
#endif

	}
}

void CRecvPACKET::Recv_gsv_PARTY_CHAT()
{
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_CHAT_1(%d)\n" ,g_GameDATA.GetGameTime() );
	short nOffset = sizeof( gsv_CHAT );
	char *szMsg;

	PartyMember member;
	if( CParty::GetInstance().GetMemberInfoByObjSvrIdx( m_pRecvPacket->m_gsv_CHAT.m_wObjectIDX, member ))
	{
		szMsg = Packet_GetStringPtr( m_pRecvPacket, nOffset);
		g_itMGR.AppendChatMsg( CStr::Printf("%s>%s", member.m_strName.c_str(), szMsg), IT_MGR::CHAT_TYPE_PARTY );
		g_UIMed.AddChatMsg( m_pRecvPacket->m_gsv_CHAT.m_wObjectIDX, szMsg );
	}
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_CHAT_2(%d)\n" ,g_GameDATA.GetGameTime() );
}

void CRecvPACKET::Recv_gsv_WHISPER ()
{
	short nOffset = sizeof( gsv_WHISPER );
	char *szAccount, *szMsg;
	szAccount = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	szMsg     = Packet_GetStringPtr( m_pRecvPacket, nOffset);

	if( szMsg[ 0 ] == '/' )
		return;
	char buffer[1024];

	if ( szMsg && szMsg[ 0 ] ) 
	{
		///서버가 보내는 귓속말은 항상 보여준다.
		///GM이나 시스템 귓속말의경우도 그렇게 해야 할텐데...
		if( _strcmpi( szAccount, "<SERVER>::" ) == 0 )
		{
			sprintf( buffer, "[%s]%s>%s",STR_WHISPER,szAccount, szMsg );
			g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );
		}
		else
		{
			if( g_ClientStorage.IsApproveWhisper() )
			{
				sprintf( buffer,"[%s]%s>%s", STR_WHISPER,szAccount, szMsg );
				g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );
			}
			else
			{
				g_pNet->Send_cli_WHISPER(  szAccount, STR_OTHER_WHISPER_REJECT_STATE );
			}
		}
	}
	else
		g_itMGR.AppendChatMsg ( CStr::Printf( FORMAT_STR_FAIL_WHISPER, szAccount ), IT_MGR::CHAT_TYPE_WHISPER );
}

void CRecvPACKET::Recv_gsv_SHOUT ()
{
	short nOffset = sizeof( gsv_SHOUT );
	char *szAccount, *szMsg;
	szAccount = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	szMsg     = Packet_GetStringPtr( m_pRecvPacket, nOffset);

	g_itMGR.AppendChatMsg( CStr::Printf("[%s]%s>%s",STR_SHOUT, szAccount, szMsg), IT_MGR::CHAT_TYPE_SHOUT);
}

//-------------------------------------------------------------------------------------------------
bool CRecvPACKET::Recv_tag_ADD_CHAR (short nCliObjIDX, tag_ADD_CHAR* tagAddChar )
{
	//struct tag_ADD_CHAR : public t_PACKETHEADER {
	//    WORD		m_wObjectIDX;
	//	tPOINTF		m_PosCUR;
	//	tPOINTF		m_PosTO;
	//	WORD		m_wCommand;
	//	WORD		m_wTargetOBJ;
	//	BYTE		m_btMoveMODE;					// 0:걷기, 1:뛰기, 2:내 승용아이템에 타고있다, 3: m_wTargetOBJ의 승용아이템에 타고있다.
	//	int			m_iHP;
	//#ifdef	__APPLY_04_10_15_TEAMNO
	//	int			m_iTeamNO;						//	char m_cTeamNO => int로 변경 04.10.15 수정...
	//#else
	//	char		m_cTeamNO;
	//#endif
	//	DWORD		m_dwStatusFALG;			
	//} ;


	CObjCHAR *pSourCHAR = g_pObjMGR->Get_CharOBJ( nCliObjIDX, false );
	if ( pSourCHAR ) 
	{
		switch( tagAddChar->m_btMoveMODE )
		{
		case 0: // 걷기
			pSourCHAR->m_bRunMODE = false;
			break;
		case 1: // 뛰기
			pSourCHAR->m_bRunMODE = true;
			break;
		case 2: // 카트에 탄 상태
			{
				//pSourCHAR->m_bRunMODE = false;
				if( pSourCHAR->Is_AVATAR() )
				{
					CObjAVT* pAVT = ( CObjAVT* )pSourCHAR;
					pAVT->CreateCartFromMyData();
				}
			}
			break;
		}		

		pSourCHAR->m_btMoveMODE = tagAddChar->m_btMoveMODE;

		pSourCHAR->Set_HP( tagAddChar->m_iHP );

		/// Team info
		pSourCHAR->SetTeamInfo( tagAddChar->m_iTeamNO );

		if ( pSourCHAR->Get_HP() > 0 ) 
		{
			// pSourCHAR->m_PosGOTO.x = m_pRecvPacket->m_tag_ADD_CHAR.m_PosTO.x;
			// pSourCHAR->m_PosGOTO.y = m_pRecvPacket->m_tag_ADD_CHAR.m_PosTO.y;
			// ;
			switch( tagAddChar->m_wCommand ) {
		case CMD_DIE		:
			pSourCHAR->SetCMD_DIE();
			return false;
		case CMD_SIT		:
			pSourCHAR->SetCMD_SIT ();
			break;

		case CMD_STOP		:
			break;

		case CMD_ATTACK		:
			{
				WORD wServeDist = CD3DUtil::distance ((int)tagAddChar->m_PosCUR.x, (int)tagAddChar->m_PosCUR.y, 
					(int)tagAddChar->m_PosTO.x, (int)tagAddChar->m_PosTO.y);

				D3DVECTOR PosTO;
				PosTO.x = tagAddChar->m_PosTO.x;
				PosTO.y = tagAddChar->m_PosTO.y;
				PosTO.z = 0; // 새로 들어오시는 분들은 높이를 알 수 없으므로 현재 자기 아바타의 높이로 일단 설정

				pSourCHAR->SetCMD_ATTACK( 
					tagAddChar->m_wTargetOBJ, 
					wServeDist, 
					PosTO );						
			}
			break;

		case CMD_RUNAWAY    :
		case CMD_MOVE		:
		case CMD_PICK_ITEM	:
			{
				WORD wServeDist = CD3DUtil::distance ((int)tagAddChar->m_PosCUR.x, (int)tagAddChar->m_PosCUR.y, 
					(int)tagAddChar->m_PosTO.x, (int)tagAddChar->m_PosTO.y);
				D3DVECTOR PosTO;
				PosTO.x = tagAddChar->m_PosTO.x;
				PosTO.y = tagAddChar->m_PosTO.y;
				PosTO.z = 0; // 새로 들어오시는 분들은 높이를 알 수 없으므로 현재 자기 아바타의 높이로 일단 설정

				pSourCHAR->SetCMD_MOVE ( 
					wServeDist, 
					PosTO, 
					tagAddChar->m_wTargetOBJ );						
			}
			break;

		case CMD_SKILL2SELF :
			// TODO:: // pSourCHAR->SetCMD_Skill2SELF
			break;
		case CMD_SKILL2OBJ :
			// TODO:: // pSourCHAR->SetCMD_Skill2OBJ
			// 타겟을 못찾으면 명령과 타겟을 유지하고
			// 최종 좌표로 이동..
			break;
		case CMD_SKILL2POS :
			// TODO:: // pSourCHAR->SetCMD_Skill2POS
			break;
			}

			return true;
		}else
		{
			pSourCHAR->Set_HP( DEAD_HP + 1 );
			pSourCHAR->Dead();
		}
	}
	return false;
}

void CRecvPACKET::Recv_tag_ADJ_STATUS ( short& nPacketOffset, tag_ADD_CHAR* ptagAddChar )
{
	int	  iStateSTBIDX = 0;
	DWORD dwTemp = 0;
	DWORD dwResult = ptagAddChar->m_dwStatusFALG;

	CObjCHAR* pChar = g_pObjMGR->Get_ClientCharOBJ( ptagAddChar->m_wObjectIDX, true );
	if( pChar == NULL )
		return;

	/// 모든 상태 비트들 체크.
	for( int i = 0; i < sizeof( DWORD )*8; i++ )
	{
		dwTemp = 0x00000001 & ( dwResult >> i );

		if( dwTemp == 0 )
			continue;

		int iIngType = 0;
		if( CEndurancePack::m_StateFlagTable.find( ( dwTemp << i ) ) != CEndurancePack::m_StateFlagTable.end() )
		{
			iIngType = CEndurancePack::m_StateFlagTable[ ( dwTemp << i ) ];
		}

		if( CEndurancePack::m_StateSTBIDXTable.find( iIngType ) != CEndurancePack::m_StateSTBIDXTable.end() )
		{
			iStateSTBIDX = CEndurancePack::m_StateSTBIDXTable[ iIngType ];

			/// 아이템 사용에 의한 지속이 아니라면...
			if( ING_INC_HP != iStateSTBIDX && ING_INC_MP != iStateSTBIDX )
			{
				if( iStateSTBIDX == ING_DEC_LIFE_TIME )
					pChar->AddEnduranceEntity( 0, 43, 100, ENDURANCE_TYPE_SKILL );
				else
					pChar->AddEnduranceEntity( 0, iStateSTBIDX, 100, ENDURANCE_TYPE_SKILL );
			}
		}/*else
		 {
		 AddMsgToChatWND( STR_UNKNOWN_STATE_FLAG, g_dwRED ,CChatDLG::CHAT_TYPE_SYSTEM);
		 }*/
	}


	if ( ptagAddChar->m_dwStatusFALG & ( FLAG_ING_MAX_HP | FLAG_ING_INC_MOV_SPEED | FLAG_ING_DEC_MOV_SPEED | FLAG_ING_INC_ATK_SPEED | FLAG_ING_DEC_ATK_SPEED ) ) 
	{
		///pChar->m_EndurancePack.ClearEntityPack();
		BYTE* pDATA = reinterpret_cast< BYTE* >( ptagAddChar );
		short *nSTATUS = ( short* )( &(pDATA[ nPacketOffset ]) );

		/// 설정된 플래그만큼 데이터는 넣는다.
		int iIndex = 0;
		if( ptagAddChar->m_dwStatusFALG & FLAG_ING_MAX_HP )		
			pChar->m_EndurancePack.SetStateValue( ING_INC_MAX_HP, nSTATUS[ iIndex++ ] );

		if( ptagAddChar->m_dwStatusFALG & FLAG_ING_INC_MOV_SPEED )		
			pChar->m_EndurancePack.SetStateValue( ING_INC_MOV_SPD, nSTATUS[ iIndex++ ] );

		if( ptagAddChar->m_dwStatusFALG & FLAG_ING_DEC_MOV_SPEED )		
			pChar->m_EndurancePack.SetStateValue( ING_DEC_MOV_SPD, nSTATUS[ iIndex++ ] );

		if( ptagAddChar->m_dwStatusFALG & FLAG_ING_INC_ATK_SPEED )		
			pChar->m_EndurancePack.SetStateValue( ING_INC_ATK_SPD, nSTATUS[ iIndex++ ] );

		if( ptagAddChar->m_dwStatusFALG & FLAG_ING_DEC_ATK_SPEED )		
			pChar->m_EndurancePack.SetStateValue( ING_DEC_ATK_SPD, nSTATUS[ iIndex++ ] );		

		nPacketOffset += iIndex * sizeof( short );

	}
	//else if( ptagAddChar->m_dwStatusFALG & FLAG_ING_STORE_MODE )
	//{
	//	if( pChar->IsUSER() )///아바타일경우
	//	{
	//		CObjAVT* pAvt=( CObjAVT* )pChar;		


	//		char *pszTitle = Packet_GetStringPtr ( m_pRecvPacket, nPacketOffset );

	//		pAvt->SetPersonalStoreTitle( pszTitle );
	//		g_UIMed.AddPersonalStoreIndex( pAvt->Get_INDEX() );
	//	}
	//}
}

// MOB & NPC 추가.
void CRecvPACKET::Recv_gsv_SET_NPC_SHOW ()
{
	CObjCHAR *pCHAR = (CObjCHAR*)g_pObjMGR->Get_ClientOBJECT( m_pRecvPacket->m_gsv_SET_NPC_SHOW.m_wObjectIDX );

	if ( pCHAR && pCHAR->IsA(OBJ_NPC ) ) {
		if ( m_pRecvPacket->m_gsv_SET_NPC_SHOW.m_bShow )
			pCHAR->SHOW ();
		else
			pCHAR->HIDE ();
	}
}

void CRecvPACKET::Recv_gsv_NPC_CHAR ()
{
	D3DVECTOR PosCUR;

	PosCUR.x = m_pRecvPacket->m_gsv_NPC_CHAR.m_PosCUR.x;
	PosCUR.y = m_pRecvPacket->m_gsv_NPC_CHAR.m_PosCUR.y;
	PosCUR.z = 0.0f;

	short nCObj, nCharIdx = abs( m_pRecvPacket->m_gsv_NPC_CHAR.m_nCharIdx );

	if ( NPC_TYPE( nCharIdx ) == 999 ) 
	{

		nCObj = g_pObjMGR->Add_NpcCHAR( m_pRecvPacket->m_gsv_NPC_CHAR.m_wObjectIDX, nCharIdx, PosCUR, m_pRecvPacket->m_gsv_NPC_CHAR.m_nQuestIDX, m_pRecvPacket->m_gsv_NPC_CHAR.m_fModelDIR);


		if ( this->Recv_tag_ADD_CHAR( nCObj, &(m_pRecvPacket->m_gsv_NPC_CHAR) ) ) 
		{
			short nOffset = sizeof( gsv_NPC_CHAR );
			this->Recv_tag_ADJ_STATUS( nOffset, &(m_pRecvPacket->m_gsv_NPC_CHAR) );			
		}

		CObjCHAR *pSourCHAR = g_pObjMGR->Get_CharOBJ( nCObj, false );
		if( pSourCHAR )///2004/2/27:추가 nAvy
		{
			if ( m_pRecvPacket->m_gsv_NPC_CHAR.m_nCharIdx < 0 ) {
				( (CObjNPC*)pSourCHAR )->HIDE ();
			}

			/// 04/5/18 특정 NPC에 이벤트 변수 추가 - jeddli
			((CObjNPC*)pSourCHAR)->SetEventValue( m_pRecvPacket->m_gsv_NPC_CHAR.m_nEventSTATUS );
			/*char Buf[255];
			sprintf( Buf, " 서버로 부터이벤트 변수 받음 %d\n", m_pRecvPacket->m_gsv_NPC_CHAR.m_nEventSTATUS );
			MessageBox( NULL, Buf, "...", MB_OK );*/

			LogString (LOG_NORMAL, "Add NPC : [%s] CObj: %d, SObj: %d \n", 
				pSourCHAR->Get_NAME(),
				nCObj, g_pObjMGR->Get_ServerObjectIndex( nCObj ) );
		}
		else
		{
			LogString (LOG_NORMAL, "Fail Add NPC : CharIdx[%d], QuestIdx[%d]\n", 
				m_pRecvPacket->m_gsv_NPC_CHAR.m_nCharIdx,
				m_pRecvPacket->m_gsv_NPC_CHAR.m_nQuestIDX
				);


		}
	} else {
		_ASSERT( 0 );
	}
}

void CRecvPACKET::Recv_gsv_MOB_CHAR ()
{
	CObjCHAR* pChar = NULL;
	int iSkillOwner = 0;
	int iDoingSkillIDX = 0;

	/// 소환수인가?
	if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_DEC_LIFE_TIME )
	{
		short nOffset = sizeof( gsv_MOB_CHAR );

		short *nSTATUS = ( short* )( &( m_pRecvPacket->m_pDATA[ nOffset ] ) );

		/// 설정된 플래그만큼 데이터는 넣는다.
		int iIndex = 0;
		if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_MAX_HP )		
			iIndex++;

		if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_INC_MOV_SPEED )		
			iIndex++;

		if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_DEC_MOV_SPEED )		
			iIndex++;

		if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_INC_ATK_SPEED )		
			iIndex++;

		if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_DEC_ATK_SPEED )		
			iIndex++;

		iSkillOwner = nSTATUS[ iIndex++ ];
		pChar = g_pObjMGR->Get_ClientCharOBJ( iSkillOwner, true );

		/// 현재 캐스팅 중이 아니면 바로 소환		
		if( pChar && pChar->m_nActiveSkillIDX && !iDoingSkillIDX )
			iDoingSkillIDX = pChar->m_nActiveSkillIDX;

		if( pChar && pChar->m_nToDoSkillIDX && !iDoingSkillIDX )
			iDoingSkillIDX = pChar->m_nToDoSkillIDX;

		if( pChar && pChar->m_nDoingSkillIDX && iDoingSkillIDX )
			iDoingSkillIDX = pChar->m_nDoingSkillIDX;



		/// 일단 바로소환
		//if( ( pChar != NULL ) && 		
		//	/// 현재 캐스팅 중이지 않은것은.. 이미 액션이 진행되버렸다.. 너무 늦게 도착한 패킷..
		//	iDoingSkillIDX )		
		//{
		//	pChar->SetSummonMobInfo( m_pRecvPacket->m_gsv_MOB_CHAR );
		//	return;
		//}

		/// 시전자가 없다면 그냥 소환..
	}

	/// 일반 몹 등장..
	{

		D3DVECTOR PosCUR;

		PosCUR.x = m_pRecvPacket->m_gsv_MOB_CHAR.m_PosCUR.x;
		PosCUR.y = m_pRecvPacket->m_gsv_MOB_CHAR.m_PosCUR.y;
		PosCUR.z = 0.0f;


		short nCObj;

		if ( NPC_TYPE( m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx ) != 999 ) 
		{
			nCObj = g_pObjMGR->Add_MobCHAR( m_pRecvPacket->m_gsv_MOB_CHAR.m_wObjectIDX, 
				m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx, 
				PosCUR, 
				m_pRecvPacket->m_gsv_MOB_CHAR.m_nQuestIDX, 
				(m_pRecvPacket->m_gsv_MOB_CHAR.m_btMoveMODE) ? true:false );
			if ( this->Recv_tag_ADD_CHAR( nCObj, &(m_pRecvPacket->m_gsv_MOB_CHAR) ) ) 
			{
				short nOffset = sizeof( gsv_MOB_CHAR );
				this->Recv_tag_ADJ_STATUS( nOffset, &(m_pRecvPacket->m_gsv_MOB_CHAR) );
			}

			/*LogString( LOG_NORMAL, " =====ADD MOB[ %s, %f, %f, %f ]=====\n", 
			g_pObjMGR->Get_CharOBJ( nCObj, false )->Get_NAME(),
			g_pObjMGR->Get_CharOBJ( nCObj, false )->Get_CurPOS().x,
			g_pObjMGR->Get_CharOBJ( nCObj, false )->Get_CurPOS().y,
			g_pObjMGR->Get_CharOBJ( nCObj, false )->Get_CurPOS().z );*/

			//-------------------------------------------------------------------------------------
			/// 소환몹일경우.. 소환몹임을 표시함.			
			//-------------------------------------------------------------------------------------
			if( pChar )
			{
				;
			}

			//-------------------------------------------------------------------------------------
			/// 내 소환몹일경우에 소환몹 리스트에 추가
			//-------------------------------------------------------------------------------------
			if( (pChar != NULL) && ( pChar->IsA( OBJ_USER ) ) )
			{
				SummonMobInfo mobInfo;
				bool bIsThere = ((CObjUSER*)pChar)->GetSummonedMobInfo( m_pRecvPacket->m_gsv_MOB_CHAR.m_wObjectIDX, mobInfo );
				if( bIsThere )
					iDoingSkillIDX = mobInfo.iSkillIDX;

				((CObjUSER*)pChar)->AddSummonedMob( m_pRecvPacket->m_gsv_MOB_CHAR.m_wObjectIDX, iDoingSkillIDX );

				if( iDoingSkillIDX )
				{					

					int iSkillLevel = SKILL_LEVEL( iDoingSkillIDX );
					int iMaxHP = 0;

					///2005.06.29 대만 적용식 분리
					if( IsApplyNewVersion() )
					{
						/// 소환수 타입에 따라 HP계산식 분리 2005/8/26 최종진 
						switch( NPC_SUMMONMOB_TYPE( m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx ) )
						{
						case 0:
							iMaxHP = (int)( NPC_HP( m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx	) + g_pAVATAR->Get_MaxHP() * 8 / 10 );
							break;
						case 1:
							iMaxHP = (int)( NPC_HP( m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx	));
							break;
						default:
							_RPTF0( _CRT_ASSERT,"알수 없는 타입(소환수 HP계산식구분)-LIST_NPC.STB의 29번컬럼" );
							break;
						}
					}
					else
					{
						iMaxHP = (int)( NPC_HP( m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx	) * ( iSkillLevel + 16 ) 
							* ( g_pAVATAR->Get_LEVEL() + 85  ) / 2600.f );
					}

					CObjCHAR* pMobChar = g_pObjMGR->Get_CharOBJ( nCObj, true );
					if( pMobChar )
					{
						((CObjMOB*)pMobChar)->Set_MaxHP( iMaxHP );
					}
				}else
				{
					assert( 0 && "This is summoned mob, but don't have owner" );
				}
			}
		} else 
		{
			Recv_gsv_NPC_CHAR ();
			return;
			//		_ASSERT( 0 );
		}
	}
}

//-------------------------------------------------------------------------------------------------
/// 사용자 추가 
/// 파티중일때 이 캐릭터가 파티원이라면 미리 받아 저장중이던 Data를 가져와서 Setting해준다.
/// 상태는 어케 하나?현재 Recv_gsv_AVT_CHAR에 상태가 넘어오므로 별도로 처리가 필요없고 Object List에 넣기만하면 된다.
void CRecvPACKET::Recv_gsv_AVT_CHAR ()
{
	/*
	WORD		m_wObjectIDX;
	tPOINTF		m_PosCUR;
	tPOINTF		m_PosTO;
	WORD		m_wCommand;
	WORD		m_wTargetOBJ;
	short		m_nHP;

	bool		m_bIsFemale;
	short		m_nPartItemIDX[ MAX_BODY_PART ];
	// char		szUserID[]
	*/
	short nOffset=sizeof( gsv_AVT_CHAR );
	char *szName = Packet_GetStringPtr ( m_pRecvPacket, nOffset );

	CObjAVT *pNewAVT = g_pObjMGR->New_AvtCHAR( m_pRecvPacket->m_gsv_AVT_CHAR.m_wObjectIDX, szName ); 
	if ( !pNewAVT ) 
	{
		LogString (LOG_NORMAL, "ADD_USER[ %s ] !!!!!!!!FAILED!!!!!!!!!!! \n", szName );
		return;
	}

	/// Bullet 정보 저장
	for( int i = 0;  i < MAX_SHOT_TYPE; ++i )
		pNewAVT->SetShotData( i, m_pRecvPacket->m_gsv_AVT_CHAR.m_sShotItem[i].m_nItemNo );
	//------------------------------------------------------------------------------------------


	// 아래 3줄은 순서 주의
	//	pNewAVT->SetAllPARTS    ( m_pRecvPacket->m_gsv_AVT_CHAR.m_nPartItemIDX );
	pNewAVT->SetAllPARTS    ( m_pRecvPacket->m_gsv_AVT_CHAR.m_PartITEM );
	//	pNewAVT->SetAllPetPARTS ( m_pRecvPacket->m_gsv_AVT_CHAR.m_nRidingITEM );
	pNewAVT->SetAllPetPARTS ( m_pRecvPacket->m_gsv_AVT_CHAR.m_RidingITEM );
	pNewAVT->SetOri_RunSPEED( m_pRecvPacket->m_gsv_AVT_CHAR.m_nRunSpeed );
	pNewAVT->SetPsv_AtkSPEED( m_pRecvPacket->m_gsv_AVT_CHAR.m_nPsvAtkSpeed );

	pNewAVT->Set_HP( m_pRecvPacket->m_gsv_AVT_CHAR.m_iHP );
	//	assert( m_pRecvPacket->m_gsv_AVT_CHAR.m_nHP > 0 );

	pNewAVT->SetTeamInfo( m_pRecvPacket->m_gsv_AVT_CHAR.m_iTeamNO );

	pNewAVT->SetAvtLevel( m_pRecvPacket->m_gsv_AVT_CHAR.m_btLEVEL );
	/// Job .. 추가..

	//------------------------------------------------------------------------------------------
	///파티원일경우
	///CParty에서 정보를 얻어온다.
	PartyMember member;
	if( CParty::GetInstance().GetMemberInfoByObjSvrIdx( m_pRecvPacket->m_gsv_AVT_CHAR.m_wObjectIDX , member ) )
	{
		pNewAVT->Set_MaxHP( member.m_Info.m_nMaxHP );
		pNewAVT->Set_CON( member.m_Info.m_nCON );
		pNewAVT->Set_AddRecoverHP( member.m_Info.m_btRecoverHP );
		pNewAVT->Set_AddRecoverMP( member.m_Info.m_btRecoverMP );
		pNewAVT->SetStamina( member.m_Info.m_nSTAMINA );
	}

	//------------------------------------------------------------------------------------------
	/// TODO:: 무게 비율에 따라 뛰기/걷기 제한을 걸어야 함...
	m_pRecvPacket->m_gsv_AVT_CHAR.m_btWeightRate;


	pNewAVT->Update_SPEED ();

	LogString (LOG_NORMAL, "ADD_USER[ %s ], MoveSpeed: %f \n", szName, pNewAVT->GetOri_RunSPEED() );

	D3DVECTOR PosCUR;
	PosCUR.x = m_pRecvPacket->m_gsv_AVT_CHAR.m_PosCUR.x;
	PosCUR.y = m_pRecvPacket->m_gsv_AVT_CHAR.m_PosCUR.y;
	PosCUR.z = 0.0f;

	if ( g_pObjMGR->Add_AvtCHAR( pNewAVT, PosCUR, m_pRecvPacket->m_gsv_AVT_CHAR.m_btCharRACE) ) 
	{
		if ( this->Recv_tag_ADD_CHAR( pNewAVT->Get_INDEX(), &(m_pRecvPacket->m_gsv_AVT_CHAR) ) ) 
		{
			this->Recv_tag_ADJ_STATUS( nOffset, &(m_pRecvPacket->m_gsv_AVT_CHAR) );
		}
	} 
	else 
	{
		g_pObjMGR->Del_Object( pNewAVT );
		return;
	}

	// 개인 상점 체크를 옮김... 2004. 11. 25
	// Process For Ment string... 
	// 06. 11. 23 - 김주현 : 플래그 체크를 위해서 switch 문에서 if문으로 변경;;
	if( m_pRecvPacket->m_gsv_AVT_CHAR.m_dwSubFLAG & FLAG_SUB_STORE_MODE)
	{
		short* nType = (short*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( short ) );
		char* pszTitle = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );			

		pNewAVT->SetPersonalStoreTitle( pszTitle, *nType );
		g_UIMed.AddPersonalStoreIndex( pNewAVT->Get_INDEX() );
	}

	if( m_pRecvPacket->m_gsv_AVT_CHAR.m_dwSubFLAG & FLAG_SUB_INTRO_CHAT)
	{
		char *pszMent = Packet_GetStringPtr ( m_pRecvPacket, nOffset );
	}



	/// clan information
	tag_CLAN_ID* pClanInfo = (tag_CLAN_ID*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_CLAN_ID) ) ;
	if( pClanInfo && pNewAVT )
	{
		pNewAVT->SetClan( 
			pClanInfo->m_dwClanID, 
			pClanInfo->m_wClanMARK[0],
			pClanInfo->m_wClanMARK[1],
			(char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ),
			pClanInfo->m_btClanLEVEL,
			pClanInfo->m_btClanPOS
			);
	}

	//------------------------------------------------------------------------------------
	/// 특수한 상태 플래그
	//------------------------------------------------------------------------------------
	pNewAVT->ChangeSpecialState( m_pRecvPacket->m_gsv_AVT_CHAR.m_dwSubFLAG );

	//------------------------------------------------------------------------------------
	///박 지호: 여신소환 상태인 유저는 소환 이펙트를 설정한다. 
	//------------------------------------------------------------------------------------
	goddessMgr.SetProcess((m_pRecvPacket->m_gsv_AVT_CHAR.m_dwSubFLAG & FLAG_SUB_ARUA_FAIRY),
		m_pRecvPacket->m_gsv_AVT_CHAR.m_wObjectIDX,TRUE);
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SUB_OBJECT ()
{
	short nObjCNT= ( m_pRecvPacket->m_HEADER.m_nSize - sizeof( t_PACKETHEADER ) ) / 2;

	CGameOBJ *pObj;
	for (short nI=0; nI<nObjCNT; nI++) 
	{
		pObj = g_pObjMGR->Get_ClientOBJECT( m_pRecvPacket->m_gsv_SUB_OBJECT.m_wObjectIDX[ nI ] );
		if ( pObj ) 
		{			 
			switch( pObj->Get_TYPE() )
			{
			case OBJ_EVENTOBJECT:					
				break; 

			case OBJ_ITEM:					
				/*	LogString( LOG_NORMAL, "SubItem [ %d ] \n", iDropItem );
				iDropItem--;
				*/	 

			default:  
				{
					//---------------------------------------------------------------------------
					//박지호::2인승 해제 
					CObjAVT* pAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_SUB_OBJECT.m_wObjectIDX[ nI ], false );
					if(pAVT)
					{	pAVT->Process_JOIN_RIDEUSER(); }
					//---------------------------------------------------------------------------

					g_UIMed.SubPersonalStoreIndex( pObj->Get_INDEX() );

					LogString ( LOG_NORMAL, "Del_Object: CObj: %s, SObj: %d \n", pObj->Get_NAME(), m_pRecvPacket->m_gsv_SUB_OBJECT.m_wObjectIDX[ nI ] );
					g_pObjMGR->Del_Object( pObj );		
				}
				break;
			} 
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SET_WEIGHT_RATE ()
{
	// 무게 비율에 따라 뛰기, 걷기 제한등이 적용된다.
	CObjAVT* pAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_SET_WEIGHT_RATE.m_wObjectIDX, false );
	if( pAVT )
	{
		pAVT->SetWeightRate( m_pRecvPacket->m_gsv_SET_WEIGHT_RATE.m_btWeightRate );

		if( m_pRecvPacket->m_gsv_SET_WEIGHT_RATE.m_btWeightRate >= 110 )
		{
			if( pAVT->GetPetMode() < 0 )
				pAVT->SetCMD_TOGGLE( MOVE_MODE_WALK + TOGGLE_TYPE_DRIVE );

			if( pAVT->IsA(OBJ_USER) )
				g_itMGR.AppendChatMsg( CStr::Printf("%s >= 110%%, %s",STR_WEIGHT_RATE,STR_CANT_ATTACK), IT_MGR::CHAT_TYPE_SYSTEM );
		}
		else if( m_pRecvPacket->m_gsv_SET_WEIGHT_RATE.m_btWeightRate >= 100  )
		{
			if( pAVT->GetPetMode() < 0 )
				pAVT->SetCMD_TOGGLE( MOVE_MODE_WALK + TOGGLE_TYPE_DRIVE );

			if( pAVT->IsA( OBJ_USER ) )
				g_itMGR.AppendChatMsg(CStr::Printf("100%% <= %s < 110%%, %s", STR_WEIGHT_RATE, STR_CANT_RUN ), IT_MGR::CHAT_TYPE_SYSTEM );
		}
		else
		{					
			if( pAVT->IsA( OBJ_USER ) )
				g_itMGR.AppendChatMsg( CStr::Printf("%s < 100%%, %s", STR_WEIGHT_RATE ,STR_NO_PENALTY), IT_MGR::CHAT_TYPE_SYSTEM );
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_ADJUST_POS ()
{
	m_pRecvPacket->m_gsv_ADJUST_POS.m_wObjectIDX;
	CObjCHAR *pSourCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_ADJUST_POS.m_wObjectIDX, true );
	if ( pSourCHAR ) 
	{
		// 사용자의 좌표를 보정할곳 !!!
		// 출발 위치는 같고 최종 위치가 바뀌는경우로...
		// 출발 위치 보다 멀리 간 경우는 빽시키고
		// 덜 간 경우는 최종 위치를 바꾸어 주면 될듯...

		// TODO:: pSourCHAR의 좌표를 m_pRecvPacket->m_gsv_ADJUST_POS.m_PosCUR로 최종 위치로 빠르게 이동 보정한다.
		// 일단 강제로..
		D3DVECTOR PosCUR;
		PosCUR.x = m_pRecvPacket->m_gsv_ADJUST_POS.m_PosCUR.x;
		PosCUR.y = m_pRecvPacket->m_gsv_ADJUST_POS.m_PosCUR.y;
		PosCUR.z = m_pRecvPacket->m_gsv_ADJUST_POS.m_nPosZ;

		pSourCHAR->SetCMD_MOVE(	PosCUR, pSourCHAR->m_bRunMODE );

		// 정지 명령으로..
		//pSourCHAR->SetCMD_STOP ();
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_STOP ()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_STOP.m_wObjectIDX, true );
	if ( pCHAR ) {
		pCHAR->SetCMD_STOP ();
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_ATTACK ()
{


#ifdef _DEBUG	

	//조성현 10 - 27 chatting
	//--------------------------------------------------------------
	char buffer[1024];
	sprintf( buffer, "*******************");
	g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );

	sprintf( buffer, "Receive Attack");
	g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );
	//-------------------------------------------------------------
#endif	

	CObjCHAR *pSourCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_ATTACK.m_wAtkObjIDX, true );

	if ( pSourCHAR ) 
	{
		if( pSourCHAR->IsA( OBJ_MOB ) )
			pSourCHAR->m_bRunMODE = true;

		D3DVECTOR PosTO;
		PosTO.x = m_pRecvPacket->m_gsv_ATTACK.m_PosTO.x;
		PosTO.y = m_pRecvPacket->m_gsv_ATTACK.m_PosTO.y;
		PosTO.z = 0;

		pSourCHAR->SetCMD_ATTACK( m_pRecvPacket->m_gsv_ATTACK.m_wDefObjIDX, 
			m_pRecvPacket->m_gsv_ATTACK.m_wSrvDIST, 
			PosTO );

#ifdef	_DEBUG
		if ( g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->m_nIndex ) == m_pRecvPacket->m_gsv_ATTACK.m_wDefObjIDX ) {
			LogString (LOG_NORMAL, "공격명령:: %d:%s[ %d ]  ==> %s[ %f, %f, %f ] \n", 
				m_pRecvPacket->m_gsv_ATTACK.m_wAtkObjIDX,
				pSourCHAR->Get_NAME(),

				m_pRecvPacket->m_gsv_ATTACK.m_wSrvDIST, 					

				g_pAVATAR->Get_NAME(),

				m_pRecvPacket->m_gsv_ATTACK.m_PosTO.x, 
				m_pRecvPacket->m_gsv_ATTACK.m_PosTO.y,
				PosTO.z );
		}
#endif
	}
}

void CRecvPACKET::Recv_gsv_ATTACK_START ()
{
	_ASSERT( 0 );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief z값이 추가됨..
//----------------------------------------------------------------------------------------------------

void CRecvPACKET::Recv_gsv_MOUSECMD ()
{
	/*
	struct gsv_MOUSECMD : public t_PACKETHEADER {
	WORD	m_wSourObjIDX;
	WORD	m_wDestObjIDX;
	tPOINTF	m_PosCUR;
	tPOINTF m_PosTO;
	} ;
	*/

#ifdef _DEBUG	
	//조성현 10 - 27 chatting 
	//-----------------------------------------------------------------
	char buffer[1024];
	sprintf( buffer, "*******************");	
	CStr::DebugString(buffer);
	sprintf( buffer, "Receive Mouse Command");	
	CStr::DebugString(buffer);
	//------------------------------------------------------------------
#endif	

	CObjCHAR *pSourCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_MOUSECMD.m_wSourObjIDX, true );

	if ( !pSourCHAR )
		return;

	D3DVECTOR PosTO;

	PosTO.x = m_pRecvPacket->m_gsv_MOUSECMD.m_PosTO.x;
	PosTO.y = m_pRecvPacket->m_gsv_MOUSECMD.m_PosTO.y;
	PosTO.z = 0;

	if (0 == pSourCHAR->Get_ModelSPEED()) 
	{
		// 현재 제자리에 멈추어 서 있던 경우에만 높이 보정.
		// Update_Height 에서 갱신할 힌트 높이값 세팅.
		pSourCHAR->SetResetPosZ( true, m_pRecvPacket->m_gsv_MOUSECMD.m_nPosZ );
	}

	pSourCHAR->SetCMD_MOVE ( m_pRecvPacket->m_gsv_MOUSECMD.m_wSrvDIST,
		PosTO, 
		m_pRecvPacket->m_gsv_MOUSECMD.m_wDestObjIDX );

}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
/// @bug 이패킷은 몬스터 이동에만 들어온다. 그래서m_pRecvPacket->m_gsv_MOVE.m_nPosZ 이 쓰레기값 
///		 높이 보정을 하지마라.
//----------------------------------------------------------------------------------------------------

void CRecvPACKET::Recv_gsv_MOVE ()
{
	/// Recv_gsv_MOUSECMD에 이동속도 추가된 패킷...
	CObjCHAR *pSourCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_MOVE.m_wSourObjIDX, true );
	if ( !pSourCHAR )
		return;	

	pSourCHAR->m_bRunMODE = m_pRecvPacket->m_gsv_MOVE.m_btMoveMODE;
	pSourCHAR->m_btMoveMODE = m_pRecvPacket->m_gsv_MOVE.m_btMoveMODE;

	D3DVECTOR PosTO;
	PosTO.x = m_pRecvPacket->m_gsv_MOVE.m_PosTO.x;
	PosTO.y = m_pRecvPacket->m_gsv_MOVE.m_PosTO.y;	
	PosTO.z = 0;

	/*if( pSourCHAR->Is_AVATAR() )
	{
	D3DXVECTOR3 PosCur = pSourCHAR->Get_CurPOS();
	if( abs( PosCur.z - m_pRecvPacket->m_gsv_MOVE.m_nPosZ )  )
	{
	PosCur.z = m_pRecvPacket->m_gsv_MOVE.m_nPosZ;
	pSourCHAR->ResetCUR_POS( PosCur );
	}
	}*/

	pSourCHAR->SetCMD_MOVE ( 
		m_pRecvPacket->m_gsv_MOVE.m_wSrvDIST, 
		PosTO, 
		m_pRecvPacket->m_gsv_MOVE.m_wDestObjIDX );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_DAMAGE ()
{
	//------------------------------------------------------------------------------------
	/// 데미지로는 소환몹인지 모른다. 리스트에서 찾아서 있으면 지워라. 소환몹 리스트에서 빼라..
	//------------------------------------------------------------------------------------	
#ifdef _DEBUG
	DWORD dwDamage = (DWORD)m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage;
	WORD wAction = (WORD)m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wACTION;
	WORD wValue = (WORD)m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wVALUE;
//CObjCHAR * temp;
	char buffer[1024];
	if(myid==m_pRecvPacket->m_gsv_DAMAGE.m_wDefObjIDX){
		sprintf( buffer, "Receive damage:%i",dwDamage);
		g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );
	}
	
#endif

	

	if ( m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage & DMG_BIT_DEAD )
	{
		if( g_pAVATAR )
			g_pAVATAR->SubSummonedMob( m_pRecvPacket->m_gsv_DAMAGE.m_wDefObjIDX );
	}

	CObjCHAR *pAtkOBJ, *pDefOBJ;

	pDefOBJ = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_DAMAGE.m_wDefObjIDX, true );
	pAtkOBJ = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_DAMAGE.m_wAtkObjIDX, false );

	

	if ( NULL == pDefOBJ ) 
		return;

	if ( pAtkOBJ ) 
	{
		// 2004. 01. 06 아이템 드롭 씽그 맞추기..
		if ( m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage & DMG_BIT_DEAD ) 
		{
			if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_DAMAGE ) + sizeof( tag_DROPITEM ) ) ) 
			{
				// 죽을때 드롭된 아이템이 있다...
				short nOffset = sizeof( gsv_DAMAGE );
				tag_DROPITEM* pFieldItem = ( tag_DROPITEM* )Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_DROPITEM ) );


				/// 몹이 죽으면서 드랍한것이다.
				if( pDefOBJ )
				{
					assert( ( pFieldItem->m_ITEM.GetTYPE() < 15 ) || ( pFieldItem->m_ITEM.GetTYPE() == ITEM_TYPE_MONEY ) );
					pDefOBJ->PushFieldItemToList( *pFieldItem );
					if(myid==pFieldItem->m_wOwnerObjIDX/*m_pRecvPacket->m_gsv_DAMAGE.m_wAtkObjIDX*/ && !g_ClientStorage.IsAutoPickup())
					{
						pickupitem = m_pRecvPacket->m_gsv_DAMAGE.m_DropITEM->m_wServerItemIDX;
#ifdef _DEBUG
						sprintf( buffer, "Pickup:%i",pickupitem);
						g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );
#endif
						CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)&pickup, 0, 0, NULL );
					}
					pDefOBJ->m_bDead = true;
				}			
			}
		}


		
		


		if ( (m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage & DMG_BIT_DEAD ) )			
		{
			// 이 데미지 맞고 죽어야 한다...
			// 미리 죽는것을 막기위해서 죽은 넘을 일정 시간 대기시켜 놓는다.
			pDefOBJ->m_DeadDAMAGE.m_nTargetObjIDX = pAtkOBJ->Get_INDEX();	// 때린넘
			pDefOBJ->m_DeadDAMAGE.m_wDamage = m_pRecvPacket->m_gsv_DAMAGE.m_Damage;	// 홍근 : 맥스 데미지 수정. (Recv에 데미지 수정 한곳이 많음.)
			pDefOBJ->m_lDeadTIME = g_GameDATA.GetGameTime();

			pDefOBJ->m_bDead = true;			

			return;
		}

		// 즉시 데미지 적용..
		if( m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage & DMG_BIT_IMMEDIATE )
		{			
			pDefOBJ->Apply_DAMAGE( pAtkOBJ, m_pRecvPacket->m_gsv_DAMAGE.m_Damage );
			if( pDefOBJ != pAtkOBJ )
				pDefOBJ->CreateImmediateDigitEffect( (DWORD)m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage );


			g_itMGR.AppendChatMsg( CStr::Printf(  "%s[ %d ]", STR_SHIELD_COUNTERATTACK, m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage ) ,
				IT_MGR::CHAT_TYPE_SYSTEM );
			return;
		}





		//----------------------------------------------------------------------------------------------------
		/// @param
		/// @brief Damage 등록..
		///			Pet Mode 일 경우에는 Pet 의 인덱스를 넣는다.( 서버는 탑승자만 존재하므로 탑승자로 Attacker 가 날라온다. )
		//----------------------------------------------------------------------------------------------------		
		/// Pet 탑승중일때는
		if( pAtkOBJ->GetPetMode() >= 0 )
		{
			if( pAtkOBJ->IsUSER() )
			{
				if( ((CObjAVT*)pAtkOBJ)->m_pObjCART != NULL )
				{
					int iIndex = ((CObjAVT*)pAtkOBJ)->m_pObjCART->Get_INDEX();
					pDefOBJ->PushDamageToList( iIndex, m_pRecvPacket->m_gsv_DAMAGE.m_Damage );		
				}else
				{
					assert( 0 && "PETMODE is valid, but ChildCART is NULL" );
				}
			}
		}else
		{
			pDefOBJ->PushDamageToList( g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_DAMAGE.m_wAtkObjIDX ), m_pRecvPacket->m_gsv_DAMAGE.m_Damage );		
		}
		return;






	}

	if( !pAtkOBJ )
		Log_String( LOG_NORMAL, "공격자가 없다~~~~!! \n" );


	/// 공격자가 없는데 죽는 패킷이 왔을경우...
	if ( (m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage & DMG_BIT_DEAD ) ) 
	{
		// 이 데미지 맞고 죽어야 한다...
		// 미리 죽는것을 막기위해서 죽은 넘을 일정 시간 대기시켜 놓는다.
		pDefOBJ->m_DeadDAMAGE.m_nTargetObjIDX = 0;	// 때린넘
		pDefOBJ->m_DeadDAMAGE.m_wDamage = m_pRecvPacket->m_gsv_DAMAGE.m_Damage;
		pDefOBJ->m_lDeadTIME = g_GameDATA.GetGameTime();
		pDefOBJ->m_bDead = true;

		//------------------------------------------------------------------------------------
		/// 데미지로는 소환몹인지 모른다. 리스트에서 찾아서 있으면 지워라. 소환몹 리스트에서 빼라..
		//------------------------------------------------------------------------------------
		if( g_pAVATAR )
			g_pAVATAR->SubSummonedMob( m_pRecvPacket->m_gsv_DAMAGE.m_wDefObjIDX );

		return;
	}

	// 공격자가 없거나, 공격자가 현재 다른 타겟을 공격중이지 않을경우 바로 데미지 적용...
	pDefOBJ->Apply_DAMAGE( pAtkOBJ, m_pRecvPacket->m_gsv_DAMAGE.m_Damage );
	if( pDefOBJ != pAtkOBJ )
		pDefOBJ->CreateImmediateDigitEffect( (DWORD)m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CHANGE_NPC ()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_CHANGE_NPC.m_wObjectIDX, true );
	if ( pCHAR && pCHAR->IsNPC() ) {
		pCHAR->Change_CHAR( m_pRecvPacket->m_gsv_CHANGE_NPC.m_nNPCNo );
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SETEXP ()
{
	if( !g_pAVATAR )
		return;

	/// 해당 몹이 죽는 타이밍에 경험치 획득 메세지를 출력하기 위해서
	if( m_pRecvPacket->m_gsv_SETEXP.m_wFromObjIDX == 0 )
	{
		/// 갱신된 내 경험치다...
		long lDiff = m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP - g_GameDATA.m_iReceivedAvatarEXP;

		/// 이건 뭔가 서버에서 순서가 잘못되서 날라온것이다.
		if( lDiff < 0 )
		{
			char buf[ 255 ];
			sprintf( buf, "Received exp is invalid[ %d/%d ]", m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP, g_GameDATA.m_iReceivedAvatarEXP );
			assert( 0 && buf );

			lDiff = 1;

			/// 경험치 깍일 경우에는 출력안함...
			/// 리졀렉션 등의 스킬로 부활할경우 경험치 패널티 때문에.. 이전경험치가 더 클수 있다
			/// 결국 lDiff 가 음수가 될수 있다...( 버그로 인한 - 는 없다는 가정.. )
		}else
		{
			/// lDiff의 경험치를 획득했다... 메세지출력..
			char szMsg[256];
			sprintf( szMsg, STR_GET_EXP, lDiff );
			g_itMGR.AppendChatMsg( szMsg, IT_MGR::CHAT_TYPE_SYSTEM );	

			g_pAVATAR->SetCur_EXP( m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP );				
			/// 스태미너도 타이밍때문에.. 따로 처리하기로 했다.. 2005/1/19
			g_pAVATAR->SetCur_STAMINA( m_pRecvPacket->m_gsv_SETEXP.m_nCurSTAMINA );
		}
	}else
	{
		/// 현재 경험치를 넣었더니.. 처리 순서가 틀려지만 - 경험치를 얻을 경우가 있다..
		long lDiff = m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP - g_GameDATA.m_iReceivedAvatarEXP;
		/// 이건 뭔가 서버에서 순서가 잘못되서 날라온것이다.
		if( lDiff < 0 )
		{
			char buf[ 255 ];
			sprintf( buf, "Received exp is invalid[ %d/%d ]", m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP, g_GameDATA.m_iReceivedAvatarEXP );
			assert( 0 && buf );

			lDiff = 1;

			/// 경험치 깍일 경우에는 출력안함...
			/// 리졀렉션 등의 스킬로 부활할경우 경험치 패널티 때문에.. 이전경험치가 더 클수 있다
			/// 결국 lDiff 가 음수가 될수 있다...( 버그로 인한 - 는 없다는 가정.. )
		}else
		{
			int iClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_SETEXP.m_wFromObjIDX );
			CDelayedExp::GetSingleton().PushEXPData( iClientIndex, lDiff, m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP, 
				m_pRecvPacket->m_gsv_SETEXP.m_nCurSTAMINA );
		}
	}

	g_GameDATA.m_iReceivedAvatarEXP = m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP;

	/// 스태미너는 그냥...
	/// 스태미너도 타이밍때문에.. 따로 처리하기로 했다.. 2005/1/19
	///g_pAVATAR->SetCur_STAMINA( m_pRecvPacket->m_gsv_SETEXP.m_nCurSTAMINA );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_LEVELUP ()
{
	CObjAVT *pCHAR = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_LEVELUP.m_wObjectIDX, false );
	if ( pCHAR ) 
	{
		if ( pCHAR->IsA( OBJ_USER ) ) 
		{		
			if ( m_pRecvPacket->m_gsv_LEVELUP.m_nSize != sizeof( gsv_LEVELUP ) ) 
				return;


			long lDiffExp = g_pAVATAR->Get_NeedEXP( g_pAVATAR->Get_LEVEL() )+ m_pRecvPacket->m_gsv_LEVELUP.m_lCurEXP - g_GameDATA.m_iReceivedAvatarEXP ;

			char szMsg[256];
			sprintf( szMsg, STR_GET_EXP, lDiffExp );
			g_itMGR.AppendChatMsg( szMsg, IT_MGR::CHAT_TYPE_SYSTEM );


			// 내가 레벨업 했다.
			g_pAVATAR->m_GrowAbility.m_lPenalEXP = 0;
			g_pAVATAR->Set_LEVEL( m_pRecvPacket->m_gsv_LEVELUP.m_nCurLevel );
			g_pAVATAR->Set_EXP  ( m_pRecvPacket->m_gsv_LEVELUP.m_lCurEXP );			
			g_pAVATAR->Set_BonusPOINT( m_pRecvPacket->m_gsv_LEVELUP.m_nBonusPoint );

			int old_skillpoint = g_pAVATAR->Get_SkillPOINT();
			int new_skillpoint = m_pRecvPacket->m_gsv_LEVELUP.m_nSkillPoint;
			if( old_skillpoint != new_skillpoint )
				g_itMGR.AppendChatMsg( CStr::Printf(STR_EARN_SP, new_skillpoint - old_skillpoint ), IT_MGR::CHAT_TYPE_SYSTEM ,D3DCOLOR_ARGB( 255,255,0,0 ));

			g_pAVATAR->Set_SkillPOINT( m_pRecvPacket->m_gsv_LEVELUP.m_nSkillPoint );


			g_pAVATAR->LevelUP ();

			///새로운 레벨에 맞는 Tip의 Index List를 재구성한다.
			g_HelpMgr.Update();
			if( g_pAVATAR->GetClanID() )
				g_pNet->Send_cli_CLAN_MEMBER_JOBnLEV( m_pRecvPacket->m_gsv_LEVELUP.m_nCurLevel, g_pAVATAR->Get_JOB() );


			/// Notify levelup event
			CTutorialEventManager::GetSingleton().CheckLevelUpEvent( g_pAVATAR->Get_LEVEL() );

			g_GameDATA.m_iReceivedAvatarEXP = m_pRecvPacket->m_gsv_LEVELUP.m_lCurEXP;
		}
		else
		{
			pCHAR->SetAvtLevel( pCHAR->Get_LEVEL() + 1 );
		}

		//----------------------------------------------------------------------------------------------------		
		/// @brief 레벨업 효과 출력
		//----------------------------------------------------------------------------------------------------
		SE_CharLevelUp( pCHAR->Get_INDEX() );
	}
}

void CRecvPACKET::Recv_gsv_HP_REPLY()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_HP_REPLY.m_wObjectIDX, false );
	if ( pCHAR && g_pAVATAR ) 
	{
		short client_obj_index = g_pObjMGR->Get_ClientObjectIndex ( m_pRecvPacket->m_gsv_HP_REPLY.m_wObjectIDX );

		if( client_obj_index == g_pAVATAR->Get_INDEX() )
			pCHAR->SetReviseHP( m_pRecvPacket->m_gsv_HP_REPLY.m_iHP - pCHAR->Get_HP() );
		else
			pCHAR->Set_HP( m_pRecvPacket->m_gsv_HP_REPLY.m_iHP );
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CHANGE_SKIN ()
{
	CObjAVT *pCHAR = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_CHANGE_SKIN.m_wObjectIDX, false );
	if ( pCHAR ) 
	{
		// 2003. 11. 27 추가..
		// 아바타 얼굴, 머리털 변경할 부분...


		/// Object 번호가 나라면 패스~
		if( pCHAR->Get_INDEX() == g_pAVATAR->Get_INDEX() )
			return;

		pCHAR->SetPartITEM( m_pRecvPacket->m_gsv_CHANGE_SKIN.m_wAbilityTYPE,
			m_pRecvPacket->m_gsv_CHANGE_SKIN.m_iValue );

		if( !pCHAR->IsPersonalStoreMode() )
			pCHAR->Update ();	
	}
}

//-------------------------------------------------------------------------------------------------
/// m_pRecvPacket->m_gsv_EQUIP_ITEM.m_nEquipIndex 가 인벤토리 슬롯 넘버가 넘어온다..
/// @bug 이동속도 체크하라..
void CRecvPACKET::Recv_gsv_EQUIP_ITEM ()
{
	CObjAVT *pCHAR = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_EQUIP_ITEM.m_wObjectIDX, false );
	if ( pCHAR ) 
	{	
		short nBodyPart = CInventory::GetBodyPartByEquipSlot( m_pRecvPacket->m_gsv_EQUIP_ITEM.m_nEquipIndex );

		if( nBodyPart == MAX_EQUIP_IDX )
		{
			_ASSERT( 0 && "nBodyPart == MAX_EQUIP_IDX" );
			return;
		}

		short nEquipItemNO = m_pRecvPacket->m_gsv_EQUIP_ITEM.m_PartITEM.m_nItemNo;


		if( nBodyPart == BODY_PART_WEAPON_R || nBodyPart == BODY_PART_WEAPON_L )
		{			
			if( nBodyPart == BODY_PART_WEAPON_R ) 
			{
				//	short nWeaponTYPE = WEAPON_TYPE( m_pRecvPacket->m_gsv_EQUIP_ITEM.m_nItemNO );
				short nWeaponTYPE = WEAPON_TYPE( nEquipItemNO );

				if ( nWeaponTYPE >= 221 && nWeaponTYPE <= 255 ) {
					// 양손검 : 221 ~
					// 원거리 : 231 ~
					// 마법무기 : 241 ~
					// 카트르계열 : 251 ~
					if ( nWeaponTYPE != 242 ) 
					{
						// 양손 무기면 왼손의 무기를 삭제...
						if( pCHAR->Get_L_WEAPON() != 0 )
						{
							pCHAR->SetPartITEM( BODY_PART_WEAPON_L, 0 );
							pCHAR->ClearLWeaponSkillEffect();
						}
					} // else ( 242 한손 마법 도구 )					
				}

				pCHAR->SetChangeWeaponR( nEquipItemNO );
				pCHAR->ClearRWeaponSkillEffect();
			} else
				if( nBodyPart == BODY_PART_WEAPON_L )
				{
					pCHAR->SetChangeWeaponL( nEquipItemNO );	
					pCHAR->ClearLWeaponSkillEffect();
				}

				pCHAR->SetUpdateMotionFlag( true );
		}

		if( nBodyPart < MAX_BODY_PART )///2004 / 2 / 2 :nAvy추가 - 반지,목걸이, 귀걸이의 경우는 없다.
		{
			pCHAR->SetPartITEM( nBodyPart, nEquipItemNO );			
		}

		// 등급, 보석번호 포함...
		pCHAR->SetPartITEM( nBodyPart, m_pRecvPacket->m_gsv_EQUIP_ITEM.m_PartITEM );

		if( !pCHAR->IsPersonalStoreMode() )
			pCHAR->Update ();		


		/// 속도가 변했다면 속도 세팅
		if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_EQUIP_ITEM ) + sizeof(short) ) )
		{
			if( pCHAR->IsUSER() )
			{
				((CObjAVT*)pCHAR)->SetOri_RunSPEED( m_pRecvPacket->m_gsv_EQUIP_ITEM.m_nRunSPEED[0] );
			}
		}


		/// if pCHAR is my avatar, update ability
		if( pCHAR->IsA( OBJ_USER ) ) 
		{
			((CObjUSER*)pCHAR)->UpdateAbility();
		}

#ifndef	__VIRTUAL_SERVER
		// 서버에서 받은 이동 속도 적용..
		pCHAR->SetOri_RunSPEED( m_pRecvPacket->m_gsv_EQUIP_ITEM.m_nRunSPEED[0] );
		pCHAR->Update_SPEED();
#endif


	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_STORE_TRADE_REPLY ()
{
	// TODO:: 상점거래 요청시 실패할경우 응답 메세지 띄우기 ...

	switch( m_pRecvPacket->m_gsv_STORE_TRADE_REPLY.m_btRESULT ) 
	{
	case STORE_TRADE_RESULT_PRICE_DIFF:	//경제 데이타가 갱신되어 가격이 틀려 졌다, 가격 다시 계산해서 팔아야함
		g_itMGR.OpenMsgBox(STR_CHANGE_PRICES);
		break;
	case STORE_TRADE_RESULT_NPC_NOT_FOUND:// 상점 주인이 없다.
		break;
	case STORE_TRADE_RESULT_TOO_FAR:// 상점 주인이 멀다.
		break;
	case STORE_TRADE_RESULT_OUT_OF_MONEY:
		g_itMGR.OpenMsgBox(STR_NOT_ENOUGH_MONEY);
		break;
	case STORE_TRADE_RESULT_NOT_UNION_USER:// 같은 조합원이 아니다
		g_itMGR.OpenMsgBox( STR_CANT_OPEN_UNIONSTORE );
		break;
	case STORE_TRADE_RESULT_OUT_OF_POINT://		0x06	// 조합 포인트가 모자르다
		g_itMGR.OpenMsgBox( STR_WANT_UNIONPOINT );
		break;
	case STORE_TRADE_RESULT_OUT_OF_REWARD_POINT://		0x07	// 보상 포인트가 모자르다
		g_itMGR.OpenMsgBox( LIST_STRING( 963 ) ); //보상포인트가 부족합니다.
		break;
	default:
		break;
	}
}

void CRecvPACKET::Recv_gsv_P_STORE_MONEYnINV()
{
	Recv_gsv_SET_MONEYnINV();
}
//-------------------------------------------------------------------------------------------------
/// 2004 / 2 / 19 : 수정 nAvy ( Add_ITEM => Set_ITEM );
void CRecvPACKET::Recv_gsv_SET_MONEYnINV ()
{
	_ASSERT( m_pRecvPacket->m_HEADER.m_nSize == sizeof( gsv_SET_MONEYnINV ) + sizeof( tag_SET_INVITEM ) * m_pRecvPacket->m_gsv_SET_MONEYnINV.m_btItemCNT );

	if( g_pAVATAR )
	{
		g_pAVATAR->SetCur_MONEY( m_pRecvPacket->m_gsv_SET_MONEYnINV.m_i64Money );

		g_pAVATAR->SetWaitUpdateInventory( true );
		for (short nI=0; nI<m_pRecvPacket->m_gsv_SET_MONEYnINV.m_btItemCNT; nI++) {
			g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ nI ].m_btInvIDX, 
				m_pRecvPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ nI ].m_ITEM );
		}
		g_pAVATAR->SetWaitUpdateInventory( false );
		g_pAVATAR->UpdateInventory();

	}

	//g_pAVATAR->RefreshLookUpTable();
}

//-------------------------------------------------------------------------------------------------
/// 2004 / 2 / 19 : 수정 nAvy ( Add_ITEM => Set_ITEM );
void CRecvPACKET::Recv_gsv_SET_INV_ONLY ()
{
	_ASSERT( m_pRecvPacket->m_HEADER.m_nSize == sizeof( gsv_SET_INV_ONLY ) + sizeof( tag_SET_INVITEM ) * m_pRecvPacket->m_gsv_SET_INV_ONLY.m_btItemCNT );

	if( g_pAVATAR )
	{
		g_pAVATAR->SetWaitUpdateInventory( true );
		for (short nI=0; nI<m_pRecvPacket->m_gsv_SET_INV_ONLY.m_btItemCNT; nI++) {
			g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ nI ].m_btInvIDX,
				m_pRecvPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ nI ].m_ITEM );
		}
		g_pAVATAR->SetWaitUpdateInventory( false );

		g_pAVATAR->UpdateAbility();///장착된 아이템에 따른 능력치 재계산

		if( g_pAVATAR->Get_COMMAND() & CMD_ATTACK )///공격중일경우
		{
			///현재 장착된 무기와 필요소모탄의 개수를 비교해서 0일경우 STOP을 날리고 메세지를 한번 뿌려준다.
			if( int iItemNo = g_pAVATAR->Get_R_WEAPON() )
			{

			}
		}
	}
}


//-------------------------------------------------------------------------------------------------
/// 아이템을 사용했다.
///아이템이 사용되었을때 서버로부터 받는 패킷: 2004/4/29:nAvy
/// 1. 내가 쓴경우 ( 한번 사용에 1~2번 온다 )
///		1) 뒤에 인벤토리 인덱스가 붙은경우는 소모성 아이템의 갯수를 빼기 위하여 받은값이다 . 이때 다른 유저에게 알려주기 위한 BroadCast에 내가 포함되어 한번더 올수 있다.
///		2) 만약 아이템이 다 사용되었을경우에는 인벤토리 인덱스가 붙은 패킷을 오지 않고 gsv_SET_INV_ONLY가 오게 된다.
/// 2. 다른 캐릭터가 쓴경우( 오직 1번만 온다 )
///     1) 효과와
void CRecvPACKET::Recv_gsv_USE_ITEM ()
{

	CObjAVT *pAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_USE_ITEM.m_wObjectIDX, true );

	/// 아이템을 사용한놈이 있다면..
	if ( pAVT ) 
	{
		if( pAVT->IsA( OBJ_USER ))
		{
			///Size가 같은 경우에는 BroadCast 시에 중복되는 패킷이다
			if( m_pRecvPacket->m_HEADER.m_nSize != sizeof( gsv_USE_ITEM ) )
			{
				tagITEM UsedItem = g_pAVATAR->m_Inventory.m_ItemLIST[ m_pRecvPacket->m_gsv_USE_ITEM.m_btInvIDX[0] ];
				UsedItem.m_uiQuantity = 1;
				g_pAVATAR->Sub_ITEM( m_pRecvPacket->m_gsv_USE_ITEM.m_btInvIDX[0], UsedItem );

				//--------------------------------------------------------------------------------------------------------------
				//박지호::USEITEM CoolTime 적용, 서버의 결과를 받고 쿨타임을 적용한다. 
				g_pAVATAR->SetCoolTimeUseItem(pAVT->Get_NAME(),m_pRecvPacket->m_gsv_USE_ITEM.m_nUseItemNO);
				//--------------------------------------------------------------------------------------------------------------


#ifdef __EXPERIENCE_ITEM
				//06. 11. 20 - 김주현 : 경험치의 량을 일정 배수로 증대 시켜주는 아이템 추가. 
				if( ITEM_TYPE( ITEM_TYPE_USE, UsedItem.GetItemNO()) == USE_ITEM_MAINTAIN_ITEM && ITEM_NEED_DATA_TYPE( ITEM_TYPE_USE, UsedItem.GetItemNO(), 0) == AT_PSV_EXPERIENCE_ITEM)
				{
					char Temp[32];

					int iQuality = ITEM_QUALITY(ITEM_TYPE_USE,UsedItem.GetItemNO() ) % 100;
					if( iQuality > 0)
					{
						float fTemp = float(ITEM_QUALITY(ITEM_TYPE_USE,UsedItem.GetItemNO() )) / 100.0f;
						sprintf(Temp, "%.1f", fTemp);
					}
					else
					{
						int iTemp = ITEM_QUALITY(ITEM_TYPE_USE,UsedItem.GetItemNO() ) / 100;
						sprintf(Temp, "%d", iTemp);
					}
					// 
					g_itMGR.AppendChatMsg(CStr::Printf(LIST_STRING(957),Temp), IT_MGR::CHAT_TYPE_SYSTEM);
					g_pNet->Send_cli_MILEAGE_ITEM_TIME(4); //4 - 경험치 아이템.

				}
#endif
			}
			else///마지막 물약일경우에는 BroadCast시에만 오게 된다.
			{
				///아이템으로 상태가 변화하는 경우
				WORD wUseItemNO = m_pRecvPacket->m_gsv_USE_ITEM.m_nUseItemNO;

				/// 이벤트 아이템은 효과번호와 con 파일 번호를 같이 사용한다.
				if( ITEM_TYPE( ITEM_TYPE_USE, wUseItemNO  ) != USE_ITEM_EVENT_ITEM )
				{
					// 효과 출력..
					int iUseItemEffect = USEITEM_USE_EFFECT( wUseItemNO );
					int iUseItemSound = USEITEM_USE_SOUND( wUseItemNO );

					pAVT->ShowEffectOnCharByIndex( iUseItemEffect, iUseItemSound );
				}
				g_pAVATAR->Use_ITEM( wUseItemNO );


#ifdef __CLEAR_SKILL
				if( ITEM_TYPE( ITEM_TYPE_USE, wUseItemNO  ) == USE_ITEM_CLEAR_SKILL )
				{					
					int iUseItemEffect	= USEITEM_USE_EFFECT( wUseItemNO );
					int iUseItemSound	= USEITEM_USE_SOUND( wUseItemNO );
					pAVT->ShowEffectOnCharByIndex( iUseItemEffect, iUseItemSound );

					switch( USEITEM_ADD_DATA_VALUE(wUseItemNO) )
					{
					case SKILL_CLEAR_JOB:
						g_pAVATAR->Reward_InitSKILLType(SKILL_TYPE_JOB_ALL);
						break;
					case SKILL_CLEAR_UNIQUE:
						g_pAVATAR->Reward_InitSKILLType(SKILL_TYPE_UNIQUE);
						break;
					case SKILL_CLEAR_MILEAGE:
						g_pAVATAR->Reward_InitSKILLType(SKILL_TYPE_MILEAGE);
						break;
					case SKILL_CLEAR_ALL:
						g_pAVATAR->Reward_InitSKILLType(SKILL_TYPE_ALL);
						break;
					}
				}
#endif 



				if ( USEITME_STATUS_STB( wUseItemNO ) && ( USE_ITEM_SKILL_DOING != ITEM_TYPE( ITEM_TYPE_USE, wUseItemNO ) ) ) /// 지속형 아이템이라면..
				{		
					for( int i = 0; i < STATE_APPLY_ABILITY_CNT; i++ )
					{
						int	 iStateNO1 = STATE_APPLY_ING_STB( USEITME_STATUS_STB( wUseItemNO ), i );

						if( iStateNO1 )
						{
							int iStepValue = STATE_APPLY_ABILITY_VALUE( USEITME_STATUS_STB( wUseItemNO ), i );
							if( iStepValue )
							{
								int iApplyValue = USEITEM_ADD_DATA_VALUE( wUseItemNO );

								pAVT->AddEnduranceEntity( wUseItemNO,
									iStateNO1, 
									( iApplyValue * 1000 ) / iStepValue,
									ENDURANCE_TYPE_USEITEM );
							}					
						}	
					}
				}
			}
		}else
		{
			///아이템으로 상태가 변화하는 경우
			WORD wUseItemNO = m_pRecvPacket->m_gsv_USE_ITEM.m_nUseItemNO;

			/// 이벤트 아이템은 효과번호와 con 파일 번호를 같이 사용한다.
			if( ITEM_TYPE( ITEM_TYPE_USE, wUseItemNO  ) != USE_ITEM_EVENT_ITEM )
			{
				// 효과 출력..
				int iUseItemEffect = USEITEM_USE_EFFECT( wUseItemNO );
				int iUseItemSound = USEITEM_USE_SOUND( wUseItemNO );
				pAVT->ShowEffectOnCharByIndex( iUseItemEffect, iUseItemSound );
			}

			if ( USEITME_STATUS_STB( wUseItemNO ) && ( USE_ITEM_SKILL_DOING != ITEM_TYPE( ITEM_TYPE_USE, wUseItemNO ) ) ) /// 지속형 아이템이라면..
			{		
				for( int i = 0; i < STATE_APPLY_ABILITY_CNT; i++ )
				{
					int	 iStateNO1 = STATE_APPLY_ING_STB( USEITME_STATUS_STB( wUseItemNO ), i );

					if( iStateNO1 )
					{
						int iStepValue = STATE_APPLY_ABILITY_VALUE( USEITME_STATUS_STB( wUseItemNO ), i );
						if( iStepValue )
						{
							int iApplyValue = USEITEM_ADD_DATA_VALUE( wUseItemNO );

							pAVT->AddEnduranceEntity( wUseItemNO,
								iStateNO1, 
								( iApplyValue * 1000 ) / iStepValue,
								ENDURANCE_TYPE_USEITEM );
						}					
					}	
				}
			}
			/// 상태가 없을경우 바로 적용..
			/// HP, MP만..
			else
			{
				switch( USEITEM_ADD_DATA_TYPE( wUseItemNO ) )
				{
				case AT_HP:
						pAVT->Add_HP( USEITEM_ADD_DATA_VALUE( wUseItemNO ) );

					if( pAVT->Get_HP() > pAVT->Get_MaxHP() )
						pAVT->Set_HP( pAVT->Get_MaxHP() );

					break;
				case AT_MP:
						pAVT->Add_MP( USEITEM_ADD_DATA_VALUE( wUseItemNO ) );

					if( pAVT->Get_MP() > pAVT->Get_MaxMP() )
						pAVT->Set_MP( pAVT->Get_MaxMP() );

					break;
				case AT_STAMINA:
					break;
				default:
					assert( 0 && "Invalid property is modified" );
					break;
				}				 
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
/// 1.7 몹이 드랍하는 아이템은 데미지에 붙어서 온다. 이 패킷은 내가 떨어뜨리거나, 다른 아바타가 떨어뜨렸을때 옴.
void CRecvPACKET::Recv_gsv_ADD_FIELDITEM ()
{	
	/// 내가 떨어뜨린것이다.
	{
		int iItemOBJ = g_pObjMGR->Add_GndITEM( 
			m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wServerItemIDX, 
			m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_ITEM, 
			m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_PosCUR );
		if ( iItemOBJ ) 
		{
			CObjITEM *pITEM = (CObjITEM *)g_pObjMGR->Get_OBJECT( iItemOBJ );
			pITEM->m_wOwnerServerObjIDX = m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wOwnerObjIDX;
			pITEM->m_wRemainTIME		= m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wRemainTIME;

			pITEM->InsertToScene ();

			// m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wOwnerObjIDX;
			// m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wRemainTIME;

			if( g_pAVATAR )
			{
				WORD wClientIdx = g_pAVATAR->Get_INDEX();
				WORD wSvrIdx = g_pObjMGR->Get_ServerObjectIndex( wClientIdx );

				if( pITEM->m_wOwnerServerObjIDX == g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX()))
				{
					tagITEM sITEM = m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_ITEM;

					/*if ( sITEM.m_cType != ITEM_TYPE_MONEY )
					g_itMGR.AppendChatMsg(CStr::Printf ("%s %s Type: %d, NO: %d ", 
					STR_DROP_ITEM,
					ITEM_NAME( sITEM.m_cType, sITEM.m_nItemNo ),
					sITEM.m_cType, sITEM.m_nItemNo ), IT_MGR::CHAT_TYPE_SYSTEM);
					else 
					g_itMGR.AppendChatMsg(CStr::Printf ("%s %d",  STR_DROP_MONEY ,sITEM.m_uiMoney), IT_MGR::CHAT_TYPE_SYSTEM);*/
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_GET_FIELDITEM_REPLY ()
{
	// 내가 아이템 먹었다.
	CObjITEM *pITEM = g_pObjMGR->Get_ClientItemOBJ( m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_wServerItemIDX );

	switch( m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_btResult ) 
	{
	case REPLY_GET_FIELDITEM_REPLY_OK	 :
		// 인벤토리에 추가...
		g_itMGR.AppendChatMsg( m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_ITEM.GettingMESSAGE(m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_nInventoryListNO), IT_MGR::CHAT_TYPE_SYSTEM );

		if( g_pAVATAR )
			g_pAVATAR->Add_CatchITEM( m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_nInventoryListNO, m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_ITEM );
	case REPLY_GET_FIELDITEM_REPLY_NONE :
		if ( pITEM ) 
		{
			pITEM->RemoveFromScene ();
			g_pObjMGR->Del_Object( pITEM );
		}
		break;
	case REPLY_GET_FIELDITEM_REPLY_TOO_MANY :
		g_itMGR.AppendChatMsg( STR_NOTIFY_01, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case REPLY_GET_FIELDITEM_REPLY_NO_RIGHT :
		g_itMGR.AppendChatMsg( STR_NOTIFY_02, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	}

}

#include "../System/GameStateMovePlanet.h"
//-------------------------------------------------------------------------------------------------
/// @todo 같은 존일경우 존 풀고 로딩하는거 고쳐라...
void CRecvPACKET::Recv_gsv_TELEPORT_REPLY ()
{	
	//홍근 : 텔레포트 에러시 존 값이 0 오후 12:12 2006-12-11
	if(m_pRecvPacket->m_gsv_TELEPORT_REPLY.m_nZoneNO==0) 
	{
		return;
	}

	CGame::GetInstance().SetLoadingData( m_pRecvPacket->m_gsv_TELEPORT_REPLY );

	//---------------------------------------------------------------------------
	//박지호::2인승 해제 
	CObjAVT* pAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_TELEPORT_REPLY.m_wObjectIDX,false);
	if(pAVT)
	{	pAVT->Process_JOIN_RIDEUSER(); }
	//---------------------------------------------------------------------------

	/// 행성번호가 같다면..
	if( ZONE_PLANET_NO( m_pRecvPacket->m_gsv_TELEPORT_REPLY.m_nZoneNO ) == 
		ZONE_PLANET_NO( g_pTerrain->GetZoneNO() ) )		
	{
		CGame::GetInstance().ChangeState( CGame::GS_WARP );
	}
	else
	{
		/// 행성번호가 틀리다면..
		if( ZONE_PLANET_NO( m_pRecvPacket->m_gsv_TELEPORT_REPLY.m_nZoneNO ) != 8 &&
			ZONE_PLANET_NO( g_pTerrain->GetZoneNO() ) != 8 )
		{
			CGameStateMovePlanet::m_iTargetPlanet = m_pRecvPacket->m_gsv_TELEPORT_REPLY.m_nZoneNO;
			CGame::GetInstance().ChangeState( CGame::GS_MOVEPLANET );
		}else			
		{
			/// 아지트로 혹은 아지트로부터의 이동에는 컷신 없음..
			CGame::GetInstance().ChangeState( CGame::GS_WARP );
		}
	}

#ifdef __CLAN_WAR_BOSSDLG
	//if( g_pAVATAR->Get_TeamNO() == 1000 || g_pAVATAR->Get_TeamNO() == 2000 )
	{
		CClanWar_BossDlg* pDLG = (CClanWar_BossDlg*)g_itMGR.FindDlg( DLG_TYPE_CLANWAR_BOSS );
		if( pDLG && pDLG->IsVision() )
		{			
			pDLG->Hide();
		}
	}	
#endif

}


//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SET_HOTICON ()
{
	if ( g_pAVATAR ) 
	{
		///g_pAVATAR->m_HotICONS.m_IconLIST[ m_pRecvPacket->m_gsv_SET_HOTICON.m_btListIDX ] = m_pRecvPacket->m_gsv_SET_HOTICON.m_HotICON;
		CHotIconSlot* pHotIcon = g_pAVATAR->GetHotIconSlot();
		pHotIcon->AddQuickItem( m_pRecvPacket->m_gsv_SET_HOTICON.m_btListIDX, 
			m_pRecvPacket->m_gsv_SET_HOTICON.m_HotICON.m_cType,
			m_pRecvPacket->m_gsv_SET_HOTICON.m_HotICON.m_nSlotNo );
	}
}


//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_USE_BPOINT_REPLY ()
{
	if ( g_pAVATAR ) 
	{
		// 기본 능력치가 업데이트 됐다..
		short nNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( m_pRecvPacket->m_gsv_USE_BPOINT_REPLY.m_btAbilityNO );
		g_pAVATAR->Set_BonusPOINT( g_pAVATAR->Get_BonusPOINT() - nNeedPoint );

		g_pAVATAR->m_BasicAbility.m_nBasicA[ m_pRecvPacket->m_gsv_USE_BPOINT_REPLY.m_btAbilityNO ] = m_pRecvPacket->m_gsv_USE_BPOINT_REPLY.m_nAbilityValue;
		g_pAVATAR->UpdateAbility();
	}
}


//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SKILL_LEARN_REPLY ()
{
	if ( !g_pAVATAR ) 
		return;

	switch( m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_btResult ) 
	{
	case RESULT_SKILL_LEARN_SUCCESS	 :		// 배우기 성공.		


		g_pAVATAR->AddNewSkill( m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_nSkillIDX, m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_wSkillSLOT );

		g_itMGR.OpenMsgBox( CStr::Printf( F_STR_SKILL_LEARNED,SKILL_NAME( m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_nSkillIDX )), 
			CMsgBox::BT_OK, false );

		g_pAVATAR->UpdateAbility();

		g_pAVATAR->GetSkillSlot()->SetActiveSkillEnableByRideState( g_pAVATAR->GetPetMode()>=0 );

		{
			//캐릭터 속성을 바꾸면 배울수 있는 스킬을 업데이트 한다.
			CTCmdUpdateLearnAbleSkill Cmd;
			Cmd.Exec(NULL);
		}

		break;
	case RESULT_SKILL_LEARN_FAILED :		// 배우기 실패.	
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_NEED_JOB : 		// 직업이 일치하지 않는다.		
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_JOB_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_NEED_SKILL :	// 보유할 스킬이 필요한다.		
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_NEED_PRESKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_NEED_ABILITY :	// 능력치가 부족하다		
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_NEED_ABILITY_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_OUTOFSLOT :		// 더이상 스킬을 배울수 없다.		
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_SLOT_FULL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_INVALID_SKILL :		// 잘못된 스킬번호 입니다.		
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_INVALID_SKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_OUTOFPOINT ://	0x07		// 스킬 포인트 부족
		g_itMGR.AppendChatMsg( STR_NOT_ENOUGH_SKILLPOINT, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_DELETE:///	0x08		// m_wSkillSLOT의 m_nSkillIDX번 스킬을 삭제해라..
		///스킬삭제
		break;
	default:
		break;

	}

	g_pAVATAR->Set_SkillPOINT( m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_nSkillPOINT );

}
void CRecvPACKET::Recv_gsv_SKILL_LEVELUP_REPLY ()
{
	if ( !g_pAVATAR ) 
		return;

	switch( m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_btResult ) 
	{
		case RESULT_SKILL_LEVELUP_SUCCESS		:
			{	
				g_pAVATAR->Skill_LEARN( m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_wSkillSLOT, m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_nSkillINDEX , false );
				g_pAVATAR->Skill_LevelUp( m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_wSkillSLOT, m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_nSkillINDEX );
				SE_SkillLevelUp( g_pAVATAR->Get_INDEX() );

				if( IsApplyNewVersion() )
				{
					int skill_index = m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_nSkillIDX;			

					int consume_july = SKILL_LEVELUP_NEED_ZULY( skill_index ) * 100;
					int consume_sp   = SKILL_NEED_LEVELUPPOINT( skill_index );

					std::string message;
					if( consume_july )
					{
						message.append( CStr::Printf( STR_CONSUME_JULY, consume_july ) );
						if( consume_sp )
							message.append( "{BR}");

						g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - consume_july );

					}

					if( consume_sp )
						message.append( CStr::Printf( STR_CONSUME_SP , consume_sp ) );

					if( consume_july || consume_sp )
						g_itMGR.OpenMsgBox( message.c_str() , CMsgBox::BT_OK, false );
				}
				g_pAVATAR->UpdateAbility();

				//캐릭터 속성을 바꾸면 배울수 있는 스킬을 업데이트 한다.
				CTCmdUpdateLearnAbleSkill Cmd;
				Cmd.Exec(NULL);
			}
			break;
		case RESULT_SKILL_LEVELUP_FAILED		:
			g_itMGR.AppendChatMsg( STR_FAIL_SKILL_LEVELUP, IT_MGR::CHAT_TYPE_SYSTEM );
			break;
		case RESULT_SKILL_LEVELUP_OUTOFPOINT	:
			g_itMGR.AppendChatMsg( STR_NOT_ENOUGH_SKILLPOINT, IT_MGR::CHAT_TYPE_SYSTEM );
			break;
		case RESULT_SKILL_LEVELUP_NEED_ABILITY	:
			g_itMGR.AppendChatMsg( STR_LEARN_SKILL_NEED_ABILITY_FAILED, IT_MGR::CHAT_TYPE_SYSTEM);
			break;
		case RESULT_SKILL_LEVELUP_NEED_JOB :
			g_itMGR.AppendChatMsg( STR_LEARN_SKILL_JOB_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
			break;
		case RESULT_SKILL_LEVELUP_NEED_SKILL :
			g_itMGR.AppendChatMsg( STR_LEARN_SKILL_NEED_PRESKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
			break;
	} 
	g_pAVATAR->Set_SkillPOINT( m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_nSkillPOINT );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SELF_SKILL ()
{

	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_SELF_SKILL.m_wSourObjIDX, true );
	if ( pCHAR ) 
	{
		/// 마지막으로 캐스팅한 스킬( cancel시에 마나를 깍기 위해서 )
		pCHAR->SetLastCastingSkill( m_pRecvPacket->m_gsv_SELF_SKILL.m_nSkillIDX );

		/// Mob 이라면 스킬모션을 세팅해준다.
		if( pCHAR->IsA( OBJ_MOB ) == true )
		{			
			if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_SELF_SKILL ) + sizeof(char) ) )
			{
				((CObjMOB*)pCHAR)->SetMobAniSkill( m_pRecvPacket->m_gsv_SELF_SKILL.cNpcSkillMOTION[0] );						
				Log_String( LOG_NORMAL, "SELF SKILL : cNpcSkillMOTION[0][ %d ]\n", m_pRecvPacket->m_gsv_SELF_SKILL.cNpcSkillMOTION[0] );			
			}
		}	

		pCHAR->SetCMD_Skill2SELF ( m_pRecvPacket->m_gsv_SELF_SKILL.m_nSkillIDX );

		//--------------------------------------------------------------------------------------------------------------
		//박지호::Skill CoolTime 적용, 서버의 결과를 받고 쿨타임을 적용한다. 
		g_pAVATAR->SetCoolTimeSkill(pCHAR->Get_NAME(),m_pRecvPacket->m_gsv_SELF_SKILL.m_nSkillIDX);
		//--------------------------------------------------------------------------------------------------------------
	}
}

void CRecvPACKET::Recv_gsv_TARGET_SKILL ()
{
	Log_String( LOG_NORMAL, "Recv Target Skill[%d]\n", m_pRecvPacket->m_gsv_TARGET_SKILL.m_nSkillIDX );
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_TARGET_SKILL.m_wSourObjIDX, true );
	if ( pCHAR ) 
	{

		/// 마지막으로 캐스팅한 스킬( cancel시에 마나를 깍기 위해서 )
		pCHAR->SetLastCastingSkill( m_pRecvPacket->m_gsv_TARGET_SKILL.m_nSkillIDX );

		/// Mob 이라면 스킬모션을 세팅해준다.
		if( pCHAR->IsA( OBJ_MOB ) == true )
		{			
			if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_TARGET_SKILL ) + sizeof(char) ) )
			{
				((CObjMOB*)pCHAR)->SetMobAniSkill( m_pRecvPacket->m_gsv_TARGET_SKILL.cNpcSkillMOTION[0] );
				Log_String( LOG_NORMAL, "TARGET SKILL : cNpcSkillMOTION[0][ %d ]\n", m_pRecvPacket->m_gsv_TARGET_SKILL.cNpcSkillMOTION[0] );
			}
		}

		D3DVECTOR PosTO;
		PosTO.x = m_pRecvPacket->m_gsv_TARGET_SKILL.m_PosTO.x;
		PosTO.y = m_pRecvPacket->m_gsv_TARGET_SKILL.m_PosTO.y;
		PosTO.z = pCHAR->Get_CurZPOS();

		pCHAR->SetCMD_Skill2OBJ( m_pRecvPacket->m_gsv_TARGET_SKILL.m_wSrvDIST,
			PosTO,
			m_pRecvPacket->m_gsv_TARGET_SKILL.m_wDestObjIDX, 
			m_pRecvPacket->m_gsv_TARGET_SKILL.m_nSkillIDX );

		//--------------------------------------------------------------------------------------------------------------
		//박지호::Skill CoolTime 적용, 서버의 결과를 받고 쿨타임을 적용한다. 
		g_pAVATAR->SetCoolTimeSkill(pCHAR->Get_NAME(),m_pRecvPacket->m_gsv_TARGET_SKILL.m_nSkillIDX);
		//--------------------------------------------------------------------------------------------------------------

	}
}

void CRecvPACKET::Recv_gsv_POSITION_SKILL ()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_POSITION_SKILL.m_wObjectIDX, true );
	if ( pCHAR ) 
	{
		/// 마지막으로 캐스팅한 스킬( cancel시에 마나를 깍기 위해서 )
		pCHAR->SetLastCastingSkill( m_pRecvPacket->m_gsv_POSITION_SKILL.m_nSkillIDX );

		/// Mob 이라면 스킬모션을 세팅해준다.
		if( pCHAR->IsA( OBJ_MOB ) == true )
		{			
			if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_POSITION_SKILL ) + sizeof(char) ) )
			{
				((CObjMOB*)pCHAR)->SetMobAniSkill( m_pRecvPacket->m_gsv_POSITION_SKILL.cNpcSkillMOTION[0] );
				Log_String( LOG_NORMAL, "TARGET SKILL : cNpcSkillMOTION[0][ %d ]\n", m_pRecvPacket->m_gsv_POSITION_SKILL.cNpcSkillMOTION[0] );
			}
		}

		D3DVECTOR PosTarget;
		PosTarget.x = m_pRecvPacket->m_gsv_POSITION_SKILL.m_PosTARGET.x;
		PosTarget.y = m_pRecvPacket->m_gsv_POSITION_SKILL.m_PosTARGET.y;
		PosTarget.z = pCHAR->Get_CurZPOS();

		pCHAR->SetCMD_Skill2POS ( PosTarget,
			m_pRecvPacket->m_gsv_POSITION_SKILL.m_nSkillIDX );

		//--------------------------------------------------------------------------------------------------------------
		//박지호::Skill CoolTime 적용, 서버의 결과를 받고 쿨타임을 적용한다. 
		g_pAVATAR->SetCoolTimeSkill(pCHAR->Get_NAME(),m_pRecvPacket->m_gsv_POSITION_SKILL.m_nSkillIDX);
		//--------------------------------------------------------------------------------------------------------------

	} 
}


//------------------------------------------------------------------------------------
/// CRecvPACKET::
/// @param 
/// @brief : 실제 서버에서 캐스팅을 시작했다.
//------------------------------------------------------------------------------------

void CRecvPACKET::Recv_gsv_SKILL_START()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_SKILL_START.m_wObjectIDX, true );
	if ( pCHAR ) 
	{
#ifdef _DEBUG		
		Log_String( LOG_NORMAL, "스킬 시작[%d]\n", m_pRecvPacket->m_gsv_SKILL_START.m_wObjectIDX );
#endif		

		/// 현재 시전할려는 스킬은 스타트를 받았다.
		if( pCHAR->bCanStartSkill() )
		{
			///assert( 0 && "Recv_gsv_SKILL_START" );
			/// 그렇다면 명령큐를 뒤져라..
			pCHAR->m_CommandQueue.SetValidFlag();

		}else
			pCHAR->SetStartSkill( true );
	}else
	{
		assert( 0 && "SKILL_START[ Not exist owner ]" );
	}
}

/// @bug /// 일단 유져일경우만 속성객체 추가..  이라인이 왜 있지? 2004/3/23
void CRecvPACKET::Recv_gsv_EFFECT_OF_SKILL ()
{
	CObjCHAR*			pObjCHAR		= NULL;
	int					iObjIDX			= 0;

	/// 스킬 시전자에게.. 등록..
	short iSkillOwner = m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_wSpellObjIDX;
	CObjCHAR *pChar = g_pObjMGR->Get_ClientCharOBJ( iSkillOwner, true );

	/// 현재 사용중인 스킬이 발사형이라면.. 바로 적용한다.
	int iDoingSkillIDX = 0;

	if( pChar && pChar->m_nActiveSkillIDX )
		iDoingSkillIDX = pChar->m_nActiveSkillIDX;

	if( pChar && pChar->m_nToDoSkillIDX )
		iDoingSkillIDX = pChar->m_nToDoSkillIDX;    	

	if( ( pChar != NULL ) && 		
		/// 현재 캐스팅 중이지 않은것은.. 이미 액션이 진행되버렸다.. 너무 늦게 도착한 패킷..
		iDoingSkillIDX )//&&
		/// 총알 발사형 스킬을 시전중 온 스킬 적용결과는 ( 다 지속이겠지? ) 바로 적용..
		//( !( SKILL_TYPE( iDoingSkillIDX ) == SKILL_ACTION_FIRE_BULLET ) || 
		//  (	SKILL_TYPE( iDoingSkillIDX ) == SKILL_ACTION_ENFORCE_BULLET ) ) )	
	{
		gsv_DAMAGE_OF_SKILL damageOfSkill;
		memcpy( &damageOfSkill, &(m_pRecvPacket->m_gsv_EFFECT_OF_SKILL ), sizeof( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL ) );
		pChar->PushEffectedSkillToList( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nSkillIDX, 										
			damageOfSkill,
			m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nINT );		
	}
	else
	{	
		/// 소유주가 없을때는 바로 적용...
		CObjCHAR *pEffectedChar = CSkillManager::GetSkillTarget( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_wObjectIDX, m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nSkillIDX );
		if( pEffectedChar == NULL )
		{
			/// 그놈이 죽었거나.. 나의 관리에서 빠졌다..
			return;
		}
		int iSkillIDX = m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nSkillIDX;

		if( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_btSuccessBITS == 0 )/// 적용 효과후 바로 삭제..즉 스킬 적용 실패다
		{
			///AddMsgToChatWND( STR_SKILL_APPLY_FAILED, g_dwRED, true );
		}
		else
		{
			///스킬이 지속형일 경우 최대 n개의 상태까지 바뀔수 있으므로
			for( int i = 0 ; i < SKILL_INCREASE_ABILITY_CNT ; i++ ) 
			{			
				if ( ( 0x01 << i ) & m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_btSuccessBITS ) 
				{
					int iStateIndex = 0;

					/// 지속형이 아닌 단순 능력치 상승형..
					/*if( SKILL_TYPE( iSkillIDX ) != SKILL_ACTION_SELF_BOUND &&
					SKILL_TYPE( iSkillIDX ) != SKILL_ACTION_TARGET_BOUND )*/
					{
						iStateIndex = SKILL_STATE_STB( iSkillIDX, i );

						/// 유리상태 해지, 불리상태 해지 등의 상태 해제 스킬들
						if( iStateIndex && STATE_TYPE( iStateIndex ) > ING_CHECK_END )
						{
							pEffectedChar->ProcFlushStateSkill( iStateIndex );
							continue;
						}
					}

					if( iStateIndex != 0 )
						/// 지속형 스킬이라면..
						/*if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_BOUND_DURATION ||
						SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_BOUND_DURATION ||
						SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_STATE_DURATION ||
						SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_STATE_DURATION )*/
					{
						pEffectedChar->AddEnduranceEntity( iSkillIDX,	iStateIndex, SKILL_DURATION( iSkillIDX ), ENDURANCE_TYPE_SKILL ) ;

						/// 상태 타입..
						int iStateType = STATE_TYPE( iStateIndex );
						/// 상태 번호가 1,2 번인경우에는 LIST_STATUS.STB 의 값을 참고하고
						if( iStateType == ING_INC_HP || iStateType == ING_INC_MP || iStateType == ING_POISONED )
							pEffectedChar->m_EndurancePack.SetStateValue( iStateType, STATE_APPLY_ABILITY_VALUE( iStateIndex, i ) );
						else
						{
							int iIncValue = 0;		
#ifdef __CAL_BUF2
							iIncValue = CCal::Get_SkillAdjustVALUE( ( CObjUSER* )pEffectedChar, iSkillIDX, i, m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nINT, iStateType );
#else

							/// 04/4/24
							if( pEffectedChar->IsA( OBJ_USER ) )
								iIncValue = CCal::Get_SkillAdjustVALUE( (CObjUSER*)pEffectedChar, iSkillIDX, i, m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nINT ,iStateType);
							else
							{
								iIncValue = 1;
								/// 유져가 아닐경우 알수가 없다. 몬스터일경우에는 공속, 이속만 계산해본다.
								if( pEffectedChar->IsA( OBJ_MOB ) )
								{
									int iAbilityValue = 0;
									switch( SKILL_INCREASE_ABILITY( iSkillIDX, i ) )
									{
									case AT_SPEED:
										iAbilityValue = pEffectedChar->GetOri_RunSPEED();
										break;
									case AT_ATK_SPD:
										iAbilityValue = pEffectedChar->GetOri_ATKSPEED();
										break;
									}
									iIncValue = (short)( iAbilityValue * SKILL_CHANGE_ABILITY_RATE( iSkillIDX, i ) / 100.f + SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i ) );
							}

								//--------------------------------------------------------------------------------------------
								/// 다른 아바타일경우.. MAX_HP는 고려를 한다..
								if( pEffectedChar->IsA( OBJ_AVATAR ) )
								{
									int iAbilityValue = 0;
									switch( SKILL_INCREASE_ABILITY( iSkillIDX, i ) )
									{
									case AT_MAX_HP:
										iAbilityValue = pEffectedChar->Get_MaxHP();
										break;								
									}
									iIncValue = (short)( iAbilityValue * SKILL_CHANGE_ABILITY_RATE( iSkillIDX, i) / 100.f + SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i) * ( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nINT + 300 ) / 315.f );								
								}
								//--------------------------------------------------------------------------------------------

							}
#endif // __CAL_BUF2

							pEffectedChar->m_EndurancePack.SetStateValue( STATE_TYPE( iStateIndex ), iIncValue );

							/// 상태스킬이 걸릴경우 AVATAR 의 경우는 능력치 업데이트..
							if( pEffectedChar->IsA( OBJ_USER ) )
							{
								(( CObjUSER* )pEffectedChar)->UpdateAbility();
							}
						}

					}else if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_BOUND ||
						SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_BOUND )
					{
						/// hp 는 계산식 적용..
						int iIncValue = CCal::Get_SkillAdjustVALUE( (CObjUSER*)pEffectedChar, iSkillIDX, i, m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nINT );

						switch( SKILL_INCREASE_ABILITY( iSkillIDX, i ) )
						{
						case AT_HP:
							pEffectedChar->Add_HP( iIncValue );
							break;
						case AT_MP:						
							pEffectedChar->Add_MP( iIncValue );
							break;
						case AT_STAMINA:
							{
								/// 상태스킬이 걸릴경우 AVATAR 의 경우는 능력치 업데이트..
								if( pEffectedChar->IsA( OBJ_USER ) )
								{
									(( CObjUSER* )pEffectedChar)->AddCur_STAMINA( SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i ) );
								}								
							}
							break;
						default:
							g_itMGR.AppendChatMsg( "Unknown property..", IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						}
					}			
				} 				
			}
		}		
	}	
}

///
/// 7,17번 타입의 경우. Damage 가 적용된다.
///
void CRecvPACKET::Recv_gsv_DAMAGE_OF_SKILL ()			
{
	Log_String( LOG_NORMAL, "스킬 데미지 받음[%d]\n", m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wDamage );


	if( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_btSuccessBITS )
	{
		Recv_gsv_EFFECT_OF_SKILL();
	}


	CObjCHAR*			pObjCHAR		= NULL;
	int					iObjIDX			= 0;

	/// 스킬 시전자에게.. 등록..
	short iSkillOwner = m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wSpellObjIDX;
	CObjCHAR *pChar = g_pObjMGR->Get_ClientCharOBJ( iSkillOwner, true );


	/// 죽는 데미지일경우.. 아이템 드랍리스트 등록..	
	if ( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wDamage & DMG_BIT_DEAD ) 
	{

		//------------------------------------------------------------------------------------
		/// 데미지로는 소환몹인지 모른다. 리스트에서 찾아서 있으면 지워라. 소환몹 리스트에서 빼라..
		//------------------------------------------------------------------------------------			
		if( g_pAVATAR )
			g_pAVATAR->SubSummonedMob( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wObjectIDX );



		if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_DAMAGE_OF_SKILL ) + sizeof( tag_DROPITEM ) ) ) 
		{
			// 죽을때 드롭된 아이템이 있다...

			short nOffset = sizeof( gsv_DAMAGE_OF_SKILL );
			tag_DROPITEM* pFieldItem = ( tag_DROPITEM* )Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_DROPITEM ) );

			CObjCHAR* pObjDead = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wObjectIDX, true );
			/// 몹이 죽으면서 드랍한것이다.			
			if( pObjDead )
			{
				assert( ( pFieldItem->m_ITEM.GetTYPE() >= ITEM_TYPE_FACE_ITEM &&
					pFieldItem->m_ITEM.GetTYPE() <= ITEM_TYPE_RIDE_PART ) || 
					pFieldItem->m_ITEM.GetTYPE() == ITEM_TYPE_MONEY );

				pObjDead->PushFieldItemToList( *pFieldItem );
				if(myid==pFieldItem->m_wOwnerObjIDX/*m_pRecvPacket->m_gsv_DAMAGE.m_wAtkObjIDX*/ && !g_ClientStorage.IsAutoPickup())
				{
					pickupitem = m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_DropITEM->m_wServerItemIDX;
					CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)&pickup, 0, 0, NULL );
				}
				pObjDead->m_bDead = true;
			}			
		}
	}


	/// 장거리 공격 스킬의경우에는 데미지로 전환해서 등록해준다.
	int iSkillType = SKILL_TYPE( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_nSkillIDX );
	if( iSkillType == SKILL_ACTION_ENFORCE_BULLET || 
		iSkillType == SKILL_ACTION_FIRE_BULLET )
	{
		iObjIDX = m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wObjectIDX;
		CObjCHAR *pEffectedChar = g_pObjMGR->Get_ClientCharOBJ( iObjIDX, true );

		if(  pEffectedChar )
			pEffectedChar->ConvertDamageOfSkillToDamage( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL );
		else
		{
			/// 바로 적용..			

			if( pEffectedChar == NULL )
				return;

			pEffectedChar->Apply_DAMAGE( NULL, m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_Damage );
			pEffectedChar->CreateImmediateDigitEffect( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_Damage.m_wVALUE );
		}

		return;
	}



	/// 현재 사용중인 스킬이 발사형이라면.. 바로 적용한다.
	int iDoingSkillIDX = 0;

	if( pChar && pChar->m_nActiveSkillIDX )
		iDoingSkillIDX = pChar->m_nActiveSkillIDX;

	if( pChar && pChar->m_nToDoSkillIDX )
		iDoingSkillIDX = pChar->m_nToDoSkillIDX;    

	/*if( pChar && pChar->m_nToDoSkillIDX )
	iDoingSkillIDX = pChar->m_nDoingSkillIDX;   */ 


	if( ( pChar != NULL ) && 		
		/// 현재 캐스팅 중이지 않은것은.. 이미 액션이 진행되버렸다.. 너무 늦게 도착한 패킷..
		iDoingSkillIDX )//&&
		/// 총알 발사형 스킬을 시전중 온 스킬 적용결과는 ( 다 지속이겠지? ) 바로 적용..
		//( !( SKILL_TYPE( iDoingSkillIDX ) == SKILL_ACTION_FIRE_BULLET ) || 
		//  (	SKILL_TYPE( iDoingSkillIDX ) == SKILL_ACTION_ENFORCE_BULLET ) ) )	
	{
		pChar->PushEffectedSkillToList( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_nSkillIDX,
			m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL, 
			m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_nINT,
			true );		
	}else
	{
		/// 바로 적용..	

		iObjIDX = m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wObjectIDX;
		///CObjCHAR *pEffectedChar = g_pObjMGR->Get_ClientCharOBJ( iObjIDX, true );
		CObjCHAR *pEffectedChar = CSkillManager::GetSkillTarget( iObjIDX, m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nSkillIDX );

		if( pEffectedChar == NULL )
			return;

		pEffectedChar->Apply_DAMAGE( NULL, m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_Damage );
		pEffectedChar->CreateImmediateDigitEffect( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_Damage.m_wVALUE );		
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 서버에서 MP를 깍은후 날라오는 스킬
//----------------------------------------------------------------------------------------------------

void CRecvPACKET::Recv_gsv_SKILL_CANCEL()
{
	short iSkillOwner = m_pRecvPacket->m_gsv_SKILL_CANCEL.m_wObjectIDX;
	CObjCHAR *pObjCHAR = g_pObjMGR->Get_ClientCharOBJ( iSkillOwner, true );

	if( pObjCHAR )
	{
		switch( m_pRecvPacket->m_gsv_SKILL_CANCEL.m_btWHY )
		{
		case SKILL_CANCEL_NEED_ABILITY:
			{					
				pObjCHAR->SetCMD_STOP();
			}
			break;
		case SKILL_CANCEL_NEED_TARGET:
			{
				pObjCHAR->SetCMD_STOP();
			}
			break;
			// 케스팅후 실제동작전 타겟이 없어졌다... MP소모 있음
		case SKILL_CANCEL_TARGET_NOT_FOUND:
			{
				pObjCHAR->SetEffectedSkillFlag( true );		

				CSkillManager::UpdateUseProperty( pObjCHAR, pObjCHAR->GetLastCastingSkill() );

				if( pObjCHAR->IsA( OBJ_USER ) )
				{
					if( !CGame::GetInstance().GetRight() )
						g_itMGR.AppendChatMsg( "타겟 사망 스킬취소", IT_MGR::CHAT_TYPE_SYSTEM );
				}

				/// 결과가 온것에 대해서만 마나를 소모한다..
				/*if( pObjCHAR->m_nActiveSkillIDX )
				CSkillManager::UpdateUseProperty( pObjCHAR, pObjCHAR->m_nActiveSkillIDX );
				else
				k						assert( 0 && "Skill cancel[ Invalid skill index ]" );*/
			}
			break;
		}
	}
}


void CRecvPACKET::Recv_gsv_RESULT_OF_SKILL ()	
{
	CObjCHAR *pObjCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_RESULT_OF_SKILL.m_wObjectIDX, false );

	if( pObjCHAR != NULL )
	{
		/*if( pObjCHAR->IsA( OBJ_USER ) )
		g_itMGR.AppendChatMsg( "스킬결과 받았다", g_dwRED );*/


		/// 큐에 저장된 스킬 명령을 뒤진다.( start 를 받은 스킬이 있다면 그 스킬에 결과를 세팅.. )
		pObjCHAR->m_CommandQueue.SetValidResultOfSkill();
		pObjCHAR->SetEffectedSkillFlag( true );			


		/// 결과가 온것에 대해서만 마나를 소모한다..
		CSkillManager::UpdateUseProperty( pObjCHAR, m_pRecvPacket->m_gsv_RESULT_OF_SKILL.m_nSkillIDX );		

		Log_String( LOG_NORMAL, "스킬 결과받음[%d][%d]\n",pObjCHAR->bCanActionActiveSkill(), m_pRecvPacket->m_gsv_SKILL_START.m_wObjectIDX );
	}
	else
	{		
		assert( 0 && "recv result of skill" );
		return;
	}

	/// 현재 사용중인 스킬이 발사형이라면.. 바로 적용한다.
	int iDoingSkillIDX = 0;

	if( pObjCHAR && pObjCHAR->m_nActiveSkillIDX )
		iDoingSkillIDX = pObjCHAR->m_nActiveSkillIDX;

	if( pObjCHAR && pObjCHAR->m_nToDoSkillIDX )
		iDoingSkillIDX = pObjCHAR->m_nToDoSkillIDX;    	

	/// 이미 액션동작이 시작후에 온것이라면( 그럴리는없지? ) 바로 수행..
	///if( pObjCHAR && pObjCHAR->m_nToDoSkillIDX )
	///	iDoingSkillIDX = pObjCHAR->m_nDoingSkillIDX;    	

	/// 스킬 액션이 끝났다.. ( 캐스팅 동작이 없을경우 발생가능 )
	if( iDoingSkillIDX == 0 )
	{
		pObjCHAR->ProcEffectedSkill();
	}
}


/// 효과가 종료되면서 난라 오는거니 Client 에서는 빼주면 된다.
void CRecvPACKET::Recv_gsv_CLEAR_STATUS ()
{
	//TODO:: /// 상태가 바뀌었다...
	CObjCHAR *pObjCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_CLEAR_STATUS.m_wObjectIDX, false );
	
	if ( pObjCHAR ) 
	{
		DWORD dwResult = pObjCHAR->m_EndurancePack.GetStateFlag() & (~m_pRecvPacket->m_gsv_CLEAR_STATUS.m_dwStatusFLAG);

		int iIndexA = 0;

		if( dwResult & FLAG_ING_HP )
		{	
			pObjCHAR->Set_HP( m_pRecvPacket->m_gsv_CLEAR_STATUS.m_nAdjVALUE[ iIndexA ] );

			if( pObjCHAR->Get_HP() > pObjCHAR->Get_MaxHP() )
				pObjCHAR->Set_HP( pObjCHAR->Get_MaxHP() );

			iIndexA++;
		}
		if( dwResult & FLAG_ING_MP )
		{
			pObjCHAR->Set_MP( m_pRecvPacket->m_gsv_CLEAR_STATUS.m_nAdjVALUE[ iIndexA ] );			

			if( pObjCHAR->Get_MP() > pObjCHAR->Get_MaxMP() )
				pObjCHAR->Set_MP( pObjCHAR->Get_MaxMP() );

			iIndexA++;
		}
//		#endif

		///새로운 지속상태 플래그로 기존 상태 종료..
		pObjCHAR->m_EndurancePack.DeleteEnduranceState( m_pRecvPacket->m_gsv_CLEAR_STATUS.m_dwStatusFLAG );


	}
}

/// 속도가 변경됐다.
/// 자신일경우 능력치를 재계산해준다.			2005/07/12 - nAvy
void CRecvPACKET::Recv_gsv_SPEED_CHANGED ()
{
	CObjAVT *pAVTChar = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_SPEED_CHANGED.m_wObjectIDX, false);

	if ( pAVTChar ) {
		pAVTChar->SetOri_RunSPEED( m_pRecvPacket->m_gsv_SPEED_CHANGED.m_nRunSPEED );			// 패시브 상태를 포함, 지속 상태 제외
		pAVTChar->SetPsv_AtkSPEED( m_pRecvPacket->m_gsv_SPEED_CHANGED.m_nPsvAtkSPEED );			// 패시브 값만...

		if( g_pAVATAR && pAVTChar->IsA( OBJ_USER ))
			g_pAVATAR->UpdateAbility();

		///TODO::
		m_pRecvPacket->m_gsv_SPEED_CHANGED.m_btWeightRate;		// 현재소지량/최대소지량*100
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_P_STORE_OPENED	()
{
	CObjAVT *pAVTChar = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_P_STORE_OPENED.m_wObjectIDX, true );
	if( pAVTChar )
	{
		if ( m_pRecvPacket->m_HEADER.m_nSize > ( sizeof( gsv_P_STORE_OPENED ) ) ) 
		{
			short nOffset = sizeof( gsv_P_STORE_OPENED );
			short* pPersonalStoreType = (short*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(short) );
			char* pszTitle = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );

			//마일리지 개인상점 세팅.
			pAVTChar->SetPersonalStoreTitle( pszTitle, 
				*pPersonalStoreType );
			g_UIMed.AddPersonalStoreIndex( pAVTChar->Get_INDEX() );
		}else
			assert( 0 && " Recv_gsv_P_STORE_OPENED.. but strTitle is null" );
	}	
}

//-------------------------------------------------------------------------------------------------
#include "../interface/dlgs/CAvatarStoreDlg.h"
void CRecvPACKET::Recv_gsv_P_STORE_CLOSED	()
{
	CObjAVT *pAVTChar = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_P_STORE_CLOSED.m_wObjectIDX, true );
	if( pAVTChar )
	{
		pAVTChar->SetPersonalStoreTitle( NULL );
		g_UIMed.SubPersonalStoreIndex( pAVTChar->Get_INDEX() );

		if( g_itMGR.IsDlgOpened( DLG_TYPE_AVATARSTORE ) )
		{
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
			if( pDlg )
			{
				CAvatarStoreDlg* pAvtStoreDlg = ( CAvatarStoreDlg*)pDlg;
				if( pAvtStoreDlg->GetMasterSvrObjIdx() == m_pRecvPacket->m_gsv_P_STORE_CLOSED.m_wObjectIDX )
				{
					g_itMGR.CloseDialog( DLG_TYPE_AVATARSTORE );
					g_itMGR.AppendChatMsg(STR_STORE_IS_CLOSED, IT_MGR::CHAT_TYPE_SYSTEM );
				}
			}
		}

	}
}

//-------------------------------------------------------------------------------------------------
#include "../interface/dlgs/CAvatarStoreDlg.h"
void CRecvPACKET::Recv_gsv_P_STORE_LIST_REPLY ()
{
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
	if( pDlg )
	{
		CAvatarStoreDlg* pStoreDlg = (CAvatarStoreDlg*)pDlg;
		///판매희망 목록
		for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_btSellItemCNT; ++i )
			pStoreDlg->AddItem2SellList( m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_SlotITEMs[i] );

		///구입희망 목록
		for( int i = m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_btSellItemCNT; 
			i < m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_btSellItemCNT + m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_btWishItemCNT;
			++i)
		{
			pStoreDlg->AddItem2BuyList( m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_SlotITEMs[i] );
		}

		g_itMGR.OpenDialog( DLG_TYPE_AVATARSTORE );
	}	


	///CAvatarStoreDlg에 아이템을 추가하고
	///CAvatarStoreDlg를 연다.
}
//-------------------------------------------------------------------------------------------------
#include "../GameData/CPrivateStore.h"
void CRecvPACKET::Recv_gsv_P_STORE_RESULT()
{
	switch(m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btResult)	
	{

	case RESULT_P_STORE_BOUGHT_PART:// 구입 완료...단, 일부 품목은 품절되어 구입 실패...
	case RESULT_P_STORE_BOUGHT_ALL:	//			0x05	// 요청한 품목 전체 구입 완료 
		{
			if( g_pAVATAR )
			{
				///내가 개설한 상점에서 거래가 완료되었을때
				if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_wStoreObjectIDX 
					== g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX() ) )
				{
					if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
					{
						for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
							CPrivateStore::GetInstance().UpdateSellList(  m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );

					}
				}
				else///개인상점이용이 성공했을때
				{
					if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
					{
						CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
						if( pDlg )
						{
							CAvatarStoreDlg* pStoreDlg = (CAvatarStoreDlg*)pDlg;
							for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
								pStoreDlg->UpdateSellList( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );
						}
					}
				}
			}
			break;
		}
	case RESULT_P_STORE_SOLD_PART:	// 판매 완료...단, 일부 품목은 구입완료되어 판매 실패...
	case RESULT_P_STORE_SOLD_ALL:	//				0x07	// 요청한 품목 전체 판매 완료
		{
			if( g_pAVATAR )
			{
				///내가 개설한 상점에서 거래가 완료되었을때
				if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_wStoreObjectIDX 
					== g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX() ) )
				{
					if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
					{
						for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
							CPrivateStore::GetInstance().UpdateBuyList(  m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );

					}
				}
				else///개인상점이용이 성공했을때
				{
					if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
					{
						CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
						if( pDlg )
						{
							CAvatarStoreDlg* pStoreDlg = (CAvatarStoreDlg*)pDlg;
							for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
								pStoreDlg->UpdateBuyList( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );
						}
					}
				}
			}
			break;
		}
		break;
	case RESULT_P_STORE_CANCLED:
		g_itMGR.AppendChatMsg( STR_TRADE_IS_CANCEL, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_P_STORE_TRADE_SOLD_OUT:
		{
			if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
			{
				CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
				if( pDlg )
				{
					CAvatarStoreDlg* pStoreDlg = (CAvatarStoreDlg*)pDlg;
					for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
						pStoreDlg->UpdateSellList( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );

					g_itMGR.AppendChatMsg( STR_ITEM_IS_SOLD, IT_MGR::CHAT_TYPE_SYSTEM );
				}
			}
			break;
		}
	case RESULT_P_STORE_TRADE_NO_MORE_NEED:
		{
			if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
			{
				CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
				if( pDlg )
				{
					CAvatarStoreDlg* pStoreDlg = (CAvatarStoreDlg*)pDlg;
					for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
						pStoreDlg->UpdateBuyList( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );

					g_itMGR.AppendChatMsg( STR_ITEM_IS_BUY_ALL, IT_MGR::CHAT_TYPE_SYSTEM );
				}
			}
			break;
		}
	default:
		break;
	}


}
//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_QUEST_REPLY ()
{
	if( !g_pAVATAR )
		return;

	g_pAVATAR->m_bQuestUpdate = true;

	switch( m_pRecvPacket->m_gsv_QUEST_REPLY.m_btResult ) 
	{
	case RESULT_QUEST_REPLY_ADD_SUCCESS	:
		/// 퀘스트 등록
		if ( !g_pAVATAR->Quest_Append(	m_pRecvPacket->m_gsv_QUEST_REPLY.m_btQuestSLOT,
			m_pRecvPacket->m_gsv_QUEST_REPLY.m_iQuestID ) ) {
				/// 뭐지?? 서버에선 등록됐는데 ???
				break;
			}

#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGOUT( "!!!퀘스트 등록!!!" );
			//--------------------------------------------------------------------------------
#endif

			break;
	case RESULT_QUEST_REPLY_ADD_FAILED	:
		break;
	case RESULT_QUEST_REPLY_DEL_SUCCESS	:
		/// 퀘스트 삭제
		if ( !g_pAVATAR->Quest_Delete(	m_pRecvPacket->m_gsv_QUEST_REPLY.m_btQuestSLOT,
			m_pRecvPacket->m_gsv_QUEST_REPLY.m_iQuestID ) ) 
		{
			/// 이건또 뭐야?? 서버에선 삭제됐는데...
			break;
		}

#ifndef	__SERVER
		//--------------------------------------------------------------------------------
		LOGOUT( "!!!퀘스트 삭제!!!" );
		//--------------------------------------------------------------------------------
#endif

		break;
	case RESULT_QUEST_REPLY_DEL_FAILED	:
		break;
	case RESULT_QUEST_REPLY_TRIGGER_SUCCESS :

#ifndef	__SERVER
		//--------------------------------------------------------------------------------
		LOGOUT( "!!!퀘스트 보상 실행!!!" );
		//--------------------------------------------------------------------------------
#endif

		/// 퀘스트 보상 실행.
		g_QuestList.CheckQUEST( g_pAVATAR, m_pRecvPacket->m_gsv_QUEST_REPLY.m_TriggerHash, true );
		break;
	case RESULT_QUEST_REPLY_TRIGGER_FAILED	:
#ifndef	__SERVER
		//--------------------------------------------------------------------------------
		LOGOUT( "!!!트리거 실패!!!" );
		//--------------------------------------------------------------------------------
#endif		
		break;
	}

	///퀘스트 리스트 갱신
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_QUEST );
	if( pDlg && pDlg->IsVision() )
	{
		CQuestDlg* pQuestDlg = ( CQuestDlg* ) pDlg;
		pQuestDlg->UpdateQuestList();
	}
}

///트레이드 관련 
void CRecvPACKET::Recv_gsv_TRADE_P2P()
{
	switch( m_pRecvPacket->m_gsv_TRADE_P2P.m_btRESULT )
	{ 

		///1.내가 요구를 받은경우
	case RESULT_TRADE_REQUEST:
		{
			///죽은넘은 거래하지마!!!!
			CObjCHAR* pObjChar = g_pObjMGR->Get_ClientCharOBJ ( m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX , true );
			if( pObjChar )
			{
				//홍근 : 일본 빌링 수정
#ifdef __JAPAN_BILL
				if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_TRADE) )
				{
					g_pNet->Send_cli_TRADE_P2P(  m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX  , RESULT_TRADE_NO_CHARGE_TARGET  );
					return;
				}        
#endif
	
				CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_EXCHANGE );
				if( pDlg && !pDlg->IsVision() && !pDlg->IsInValidShow() && g_itMGR.FindMsgBox(CMsgBox::MSGTYPE_RECV_TRADE_REQ) == NULL && g_ClientStorage.IsApproveExchange() )
				{
					if(g_pAVATAR->GetExChangeWnd() == false)
					{
						g_pAVATAR->SetExChangeWnd(true);
						char szTemp[128];
						sprintf( szTemp,FORMAT_STR_RECEIVE_TRADE_REQ, pObjChar->Get_NAME() );

						CTCommand* pCmdOk     = new CTCmdAcceptTradeReq( m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX );
						CTCommand* pCmdCancel = new CTCmdRejectTradeReq( m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX );

						g_itMGR.OpenMsgBox( szTemp, CMsgBox::BT_OK | CMsgBox::BT_CANCEL, false , 0, pCmdOk, pCmdCancel ,CMsgBox::MSGTYPE_RECV_TRADE_REQ);
					}
				}
				else
				{
					g_pAVATAR->SetExChangeWnd(false);
					g_pNet->Send_cli_TRADE_P2P(  m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX  , RESULT_TRADE_BUSY  );
				}
			}
			break;
		}
		///상대방이 거래를 도중에 중단되었다
	case RESULT_TRADE_CANCEL:
		{
			g_itMGR.CloseDialog( DLG_TYPE_EXCHANGE );				
			g_itMGR.OpenMsgBox( STR_OTHER_CANCEL_TRADE, CMsgBox::BT_OK , true );
			break;
		}
		///상대방이 물건을 다 올리고 준비상태이다.
	case RESULT_TRADE_CHECK_READY:
		{   
			CExchange::GetInstance().SetReadyOther( true );
			break;
		}
	case RESULT_TRADE_UNCHECK_READY:
		{	
			CExchange::GetInstance().SetReadyOther( false );
			break;
		}
		///상대방의 인벤토리가 부족하다, 내가 올린 아이템을 다시 빼자
	case RESULT_TRADE_OUT_OF_INV:
		{
			CExchange::GetInstance().RemoveMyItemBySlotIndex( m_pRecvPacket->m_cli_TRADE_P2P.m_cTradeSLOT );
			g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_OTHER_INVENTORY_SPACE );
			break;
		}
		///거래 완료
	case RESULT_TRADE_DONE:
		{
			g_itMGR.CloseDialog( DLG_TYPE_EXCHANGE );
			CExchange::GetInstance().EndExchange();
			g_itMGR.OpenMsgBox( STR_SUCCESS_EXCHANGE, CMsgBox::BT_OK, false );
			break;
		}
		///상대방이 교환버튼을 누르고 대기중이다.
		///*-------------------------------------------------------------------------------------*/
		///2.내요구의 응답
	case RESULT_TRADE_BUSY:
		{
			g_itMGR.OpenMsgBox( STR_OTHER_CANT_RECEIVE_TRADE_REQ, CMsgBox::BT_OK , true );
			break;
		}
	case RESULT_TRADE_NO_CHARGE_TARGET:
		{
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_TRADE_DONT_PAY_TARGET, CMsgBox::BT_OK , true );
			break;
		}
	case RESULT_TRADE_ACCEPT:
		{
			CObjCHAR* pObjChar = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX, true );
			if( pObjChar == NULL )///상대방이 없을경우 무시
				break;

			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_EXCHANGE );
			if( pDlg )
			{
				CExchange::GetInstance().StartExchange( m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX , pObjChar->Get_NAME() );

				int iPosX = g_pCApp->GetWIDTH() / 2 - pDlg->GetWidth();
				int iPosY = g_pCApp->GetHEIGHT() / 2 - pDlg->GetHeight() / 2;

				g_itMGR.OpenDialog( DLG_TYPE_EXCHANGE ,  true, iPosX , iPosY );	
				iPosX = g_pCApp->GetWIDTH() / 2;
				g_itMGR.OpenDialog( DLG_TYPE_ITEM , false , iPosX, iPosY );
			}
			break;
		}
	case RESULT_TRADE_REJECT:
		{
			g_itMGR.OpenMsgBox( STR_OTHER_REJECT_TRADE_REQ, CMsgBox::BT_OK , true );
			break;
		}
	case RESULT_TRADE_TOO_FAR:
		{
			g_itMGR.OpenMsgBox( STR_OTHER_IS_TOO_FAR_TO_TRADE, CMsgBox::BT_OK , true );
			break;
		}
	case RESULT_TRADE_NOT_TARGET:
		{
			g_itMGR.OpenMsgBox( STR_NOT_FOUND_TRADE_OTHER, CMsgBox::BT_OK , true );
			break;
		}
	default:
		break;
	}
}

void CRecvPACKET::Recv_gsv_TRADE_P2P_ITEM()
{
	if( CExchange::GetInstance().IsReadyMe() )
		g_itMGR.OpenMsgBox( STR_CHANGE_TRADEITEM_IN_MYREADY );

	CExchange::GetInstance().UpdateOtherItem( m_pRecvPacket->m_gsv_TRADE_P2P_ITEM.m_cTradeSLOT, m_pRecvPacket->m_gsv_TRADE_P2P_ITEM.m_ITEM );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_PARTY_REQ()
{
	_RPT2( _CRT_WARN,"Recv_gsv_PARTY_REQ_1 %d(%d)\n",m_pRecvPacket->m_gsv_PARTY_REQ.m_btREQUEST, g_GameDATA.GetGameTime() );

	WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REQ.m_dwFromIDXorTAG;
	CObjAVT* pObjChar		= g_pObjMGR->Get_CharAVT( g_pObjMGR->Get_ClientObjectIndex( wObjSvrIdx ), true );
	CParty& Party = CParty::GetInstance();


	switch( m_pRecvPacket->m_gsv_PARTY_REQ.m_btREQUEST )
	{
	case PARTY_REQ_JOIN:///파티장이 파티에 들어올것을 요청했다.
		{
			//홍근 : 일본 빌링 수정
#ifdef __JAPAN_BILL
			if( !(CGame::GetInstance().GetPayType() & PAY_FLAG_JP_BATTLE) )
			{
				g_pNet->Send_cli_PARTY_REPLY( PAATY_REPLY_NO_CHARGE_TARGET, wObjSvrIdx );
				return;
			}        
#endif

			if( !Party.IsValidJoinParty()  || g_itMGR.IsOpenedMsgBox( CMsgBox::MSGTYPE_RECV_PARTY_REQ ) || !g_ClientStorage.IsApproveParty() )
				g_pNet->Send_cli_PARTY_REPLY( PARTY_REPLY_BUSY, wObjSvrIdx );
			else
			{
				if( pObjChar )
				{
					if(g_pAVATAR->GetPartyInvitationWnd() == false)
					{
						g_pAVATAR->SetPartyInvitationWnd(true);
						sprintf( g_MsgBuf,FORMAT_STR_PARTY_INVITE,pObjChar->Get_NAME() );
						CTCmdAcceptPartyJoin* pCmdOk		= new CTCmdAcceptPartyJoin( wObjSvrIdx );
						CTCmdRejectPartyJoin* pCmdCancel	= new CTCmdRejectPartyJoin( wObjSvrIdx );

						g_itMGR.OpenMsgBox( g_MsgBuf, CMsgBox::BT_OK | CMsgBox::BT_CANCEL, false, 0, pCmdOk, pCmdCancel , CMsgBox::MSGTYPE_RECV_PARTY_REQ );
					}
				}
			}
			break;
		}
	case PARTY_REQ_MAKE:///파티중이 아닌 다른 아바타가 파티 결성을 요청했다.
		{
			//홍근 : 일본 빌링 수정
#ifdef __JAPAN_BILL
			if( !(CGame::GetInstance().GetPayType() & PAY_FLAG_JP_BATTLE) )
			{
				g_pNet->Send_cli_PARTY_REPLY( PAATY_REPLY_NO_CHARGE_TARGET, wObjSvrIdx );
				return;
			}        
#endif

			if( !Party.IsValidJoinParty() || g_itMGR.IsOpenedMsgBox( CMsgBox::MSGTYPE_RECV_PARTY_REQ ) || !g_ClientStorage.IsApproveParty())
				g_pNet->Send_cli_PARTY_REPLY( PARTY_REPLY_BUSY, wObjSvrIdx );
			else
			{
				if( pObjChar )
				{
					if(g_pAVATAR->GetPartyInvitationWnd() == false)
					{
						g_pAVATAR->SetPartyInvitationWnd(true);
						sprintf( g_MsgBuf,FORMAT_STR_PARTY_MAKE_REQ,pObjChar->Get_NAME() );

						CTCmdAcceptPartyJoin* pCmdOk		= new CTCmdAcceptPartyJoin( wObjSvrIdx );
						CTCmdRejectPartyJoin* pCmdCancel	= new CTCmdRejectPartyJoin( wObjSvrIdx );

						g_itMGR.OpenMsgBox( g_MsgBuf, CMsgBox::BT_OK | CMsgBox::BT_CANCEL, false, 0, pCmdOk, pCmdCancel , CMsgBox::MSGTYPE_RECV_PARTY_REQ );
					}
				}
			}
			break;
		}

	default:
		break;
	}
	_RPT2( _CRT_WARN,"Recv_gsv_PARTY_REQ_2 %d(%d)\n",m_pRecvPacket->m_gsv_PARTY_REQ.m_btREQUEST, g_GameDATA.GetGameTime() );

}
void CRecvPACKET::Recv_gsv_PARTY_REPLY()
{
	_RPT2( _CRT_WARN,"Recv_gsv_PARTY_REPLY_1 %d(%d)\n",m_pRecvPacket->m_gsv_PARTY_REPLY.m_btREPLY,g_GameDATA.GetGameTime() );
	CParty& Party = CParty::GetInstance();

	switch( m_pRecvPacket->m_gsv_PARTY_REPLY.m_btREPLY )
	{
	case PARTY_REPLY_INVALID_LEVEL:
		{
			WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			CObjAVT* pObjAvt = g_pObjMGR->Get_CharAVT( g_pObjMGR->Get_ClientObjectIndex( wObjSvrIdx ), true);
			if( pObjAvt )
			{

				sprintf( g_MsgBuf,"%s%s", pObjAvt->Get_NAME(),STR_CANT_JOIN_PARTY_REASON_LEVEL);
				g_itMGR.AppendChatMsg( g_MsgBuf, IT_MGR::CHAT_TYPE_SYSTEM);
			}
			break;
		}
	case PARTY_REPLY_CHANGE_OWNERnDISCONN:
		{
			DWORD dwLeaderTag = Party.GetLeaderObjectTAG();
			CParty::GetInstance().MemberDisconnected( dwLeaderTag );

			WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			Party.ChangeLeaderByObjSvrIdx( wObjSvrIdx );
			break;
		}
	case PARTY_REPLY_CHANGE_OWNER:
		{
			WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			Party.ChangeLeaderByObjSvrIdx( wObjSvrIdx );
			break;
		}
	case PARTY_REPLY_BAN:
		{
			DWORD dwObjectTag = m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			Party.BanMember( dwObjectTag );
			break;
		}
	case PARTY_REPLY_NOT_FOUND:
		g_itMGR.AppendChatMsg(STR_PARTY_NOTFOUND_DEST, IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case PARTY_REPLY_BUSY:
		{
			WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			CObjCHAR* pObjChar = g_pObjMGR->Get_CharOBJ( g_pObjMGR->Get_ClientObjectIndex( wObjSvrIdx ), false );
			if( pObjChar )
				g_itMGR.AppendChatMsg( CStr::Printf( FORMAT_STR_PARTY_BUSY,pObjChar->Get_NAME() ), IT_MGR::CHAT_TYPE_SYSTEM);

			break;
		}
	case PAATY_REPLY_NO_CHARGE_TARGET:
		{
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_INVITED_PARTY );
		}
		break;

	case PARTY_REPLY_ACCEPT_MAKE:///자신이 파티결성을 요청해서 상대방이 응답했을경우
		{
			if( g_pAVATAR )
			{
				g_itMGR.AppendChatMsg(STR_PARTY_MADE, IT_MGR::CHAT_TYPE_SYSTEM);
				CParty::GetInstance().SetLeaderObjSvrIdx( g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX()));
				CParty::GetInstance().SetLeaderObjectTag( g_pAVATAR->GetUniqueTag() );
			}
			break;
		}
	case PARTY_REPLY_ACCEPT_JOIN:
		{
			break;
		}
	case PARTY_REPLY_REJECT_JOIN:
		{
			WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			CObjCHAR* pObjChar = g_pObjMGR->Get_CharOBJ( g_pObjMGR->Get_ClientObjectIndex( wObjSvrIdx ), false );
			if( pObjChar )
			{
				sprintf( g_MsgBuf ,FORMAT_STR_PARTY_REJECT_INVITE,pObjChar->Get_NAME() );
				g_itMGR.OpenMsgBox( g_MsgBuf,CMsgBox::BT_OK, false  );
			}
			break;
		}
	case PARTY_REPLY_DESTROY:
		{
			CParty::GetInstance().Destory();
			break;
		}
	case PARTY_REPLY_REJOIN:
		{
			//CParty::GetInstance().Join();
			break;
		}
	case PARTY_REPLY_DISCONNECT:
		{
			CParty::GetInstance().MemberDisconnected( m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG );
			///비정상 종료되었다.그러면 Object Index를 0으로 만들까? 그리고 위임은 안되게 하자.
			break;
		}
	default:
		break;
	}
	_RPT2( _CRT_WARN,"Recv_gsv_PARTY_REPLY_2 %d(%d)\n",m_pRecvPacket->m_gsv_PARTY_REPLY.m_btREPLY,g_GameDATA.GetGameTime() );
}
void CRecvPACKET::Recv_gsv_PARTY_MEMBER()
{
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_MEMBER_1 %d\n",g_GameDATA.GetGameTime() );
	short  nOffset = sizeof( gsv_PARTY_MEMBER );

	tag_PARTY_MEMBER* pMember = NULL;
	char*	pszName	= NULL;

	CParty& Party = CParty::GetInstance();

	if( Party.HasParty() )
	{
		if( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_cUserCNT == PARTY_MEMBER_SUB )///Leave
		{
			Party.LeaveMember( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_dwObjectTAG[0] );

			if( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_dwObjectTAG[0] == Party.GetLeaderObjectTAG() )
				Party.GetInstance().ChangeLeaderByTag( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_dwObjectTAG[1] );

		}
		else///Join
		{
			pMember = (tag_PARTY_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_PARTY_MEMBER ) );
			pszName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
			assert( pMember && pszName );
			if( pMember && pszName )
				Party.JoinMember( *pMember, pszName );
		}
	}
	else///새로운 파티가 결성되었을때
	{
		///새롭게 파티를 만들고 이곳에 가입한 사람이다.
		if( Party.IsPartyLeader() )///나는 내려오지 않는다.
		{
			CParty::GetInstance().Make();
			pMember = (tag_PARTY_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_PARTY_MEMBER ) );
			pszName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
			assert( pMember && pszName );
			if( pMember && pszName )
				Party.JoinMember( *pMember, pszName );

			Party.RecvPartyRule( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_btPartyRULE );
		}
		else///처음으로 내려온 사람이 파티장이다.///나도 내려오나?
		{
			CParty::GetInstance().Join();
			for( char i = 0; i < m_pRecvPacket->m_gsv_PARTY_MEMBER.m_cUserCNT; ++i )
			{
				pMember = (tag_PARTY_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_PARTY_MEMBER ) );
				pszName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
				assert( pMember && pszName );
				if( pMember && pszName )
				{
					Party.JoinMember( *pMember, pszName );
					if( i == 0 )///파티장이면
					{
						Party.ChangeLeaderByObjSvrIdx( pMember->m_wObjectIDX );
					}
				}
			}
			Party.RecvPartyRule( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_btPartyRULE );
		}
	}
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_MEMBER_2 %d\n",g_GameDATA.GetGameTime() );
} 

void CRecvPACKET::Recv_gsv_PARTY_LEVnEXP()
{
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_LEVnEXP_1(%d)\n" ,g_GameDATA.GetGameTime() );

	CParty::GetInstance().SetExp( m_pRecvPacket->m_gsv_PARTY_LEVnEXP.m_iEXP );
	CParty::GetInstance().SetLevel( m_pRecvPacket->m_gsv_PARTY_LEVnEXP.m_btLEVEL );
	///레벨업시에
	if( m_pRecvPacket->m_gsv_PARTY_LEVnEXP.m_bitLevelUP )
	{
		CParty::GetInstance().LevelUp();

		//----------------------------------------------------------------------------------------------------			
		/// @brief 파티원들에게 이펙트를 붙인다.
		//----------------------------------------------------------------------------------------------------
		if( g_pAVATAR )
			SE_PartyLevelUp( g_pAVATAR->Get_INDEX() );	

	}
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_LEVnEXP_2(%d)\n" ,g_GameDATA.GetGameTime() );
}

///일단 저장하고 Object List에 있는 AVATAR라면 그곳에도 Data를 넣어준다.
///서버 구조가 바뀜에 따라 아마도 서버에서는 이제 안오는 패킷일것 같은데
///2004/7/15 - 파티중 내가 죽었을경우 ObjIndex가 변해도 오지않는다.JOIN_ZONE에서 처리한다.
void CRecvPACKET::Recv_gsv_CHANGE_OBJIDX()
{
	_RPT1( _CRT_WARN,"Recv_gsv_CHANGE_OBJIDX-1(%d)\n", g_GameDATA.GetGameTime() );
	if( CParty::GetInstance().ChangeMemberInfoByUserTag( m_pRecvPacket->m_gsv_CHANGE_OBJIDX.m_Member ) )
	{
		CObjAVT *pObjAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_CHANGE_OBJIDX.m_Member.m_wObjectIDX , false ); 
		if ( pObjAVT ) 
		{
			tag_PARTY_MEMBER* pMemberInfo = &(m_pRecvPacket->m_gsv_CHANGE_OBJIDX.m_Member);
			if( pMemberInfo )///파티원일경우
			{
				pObjAVT->Set_HP( pMemberInfo->m_nHP );
				pObjAVT->Set_MaxHP( pMemberInfo->m_nMaxHP );
				pObjAVT->Set_CON( pMemberInfo->m_nCON );
				pObjAVT->Set_AddRecoverHP( pMemberInfo->m_btRecoverHP );
				pObjAVT->Set_AddRecoverMP( pMemberInfo->m_btRecoverMP );
				pObjAVT->SetStamina( pMemberInfo->m_nSTAMINA );
			}
		}
	}
	_RPT1( _CRT_WARN,"Recv_gsv_CHANGE_OBJIDX-2(%d)\n", g_GameDATA.GetGameTime() );

}
//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CREATE_ITEM_REPLY()
{
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_MAKE );
	if( pDlg && pDlg->IsVision() )
	{
		CMakeDLG* pMakeDlg = ( CMakeDLG* ) pDlg;
		pMakeDlg->RecvResult( m_pRecvPacket );
	}
}
//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_BANK_LIST_REPLY()
{
	if( !g_pAVATAR )
		return;

	switch( m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_btREPLY )
	{
	case BANK_REPLY_INIT_DATA:
		{
			g_pAVATAR->InitBank();///
			for( BYTE bt = 0; bt < m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_btItemCNT;++bt )
				g_pAVATAR->SetBankItem(  m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_sInvITEM[bt].m_btInvIDX, 
				m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_sInvITEM[bt].m_ITEM);

			///은행에 보관된 돈정보가 붙어 있을때
			short Not_IncludeMoneyPacketSize = sizeof( gsv_BANK_LIST_REPLY ) + sizeof(tag_SET_INVITEM) * m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_btItemCNT;

			if( m_pRecvPacket->m_HEADER.m_nSize  == Not_IncludeMoneyPacketSize + 8 )
			{
				__int64* money = (__int64*)Packet_GetDataPtr( m_pRecvPacket, Not_IncludeMoneyPacketSize, sizeof(__int64) );
				CBank::GetInstance().SetMoney( *money );
			}
		}
	case BANK_REPLY_SHOW_DATA:
		g_itMGR.OpenDialog( DLG_TYPE_ITEM, false );
		g_itMGR.OpenDialog( DLG_TYPE_BANK ,false );
		break;
	case BANK_REPLY_PLATINUM:///INIT_DATA보다 나중에 와야된다.
		{
			for( BYTE bt = 0; bt < m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_btItemCNT;++bt )
				g_pAVATAR->SetBankItem(  m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_sInvITEM[bt].m_btInvIDX, 
				m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_sInvITEM[bt].m_ITEM);
		}
		break;
	case BANK_REPLY_NEED_PASSWORD:// 창고 비번이 필요..
		g_itMGR.OpenMsgBox(STR_BANK_REPLY_NEED_PASSWORD);
		break;
	case BANK_REPLY_CHANGED_PASSWORD:// 창고 비번이 변경됐음
		g_itMGR.OpenMsgBox(STR_BANK_REPLY_CHANGED_PASSWORD);
		break;
	case BANK_REPLY_INVALID_PASSWORD:// 창고 비번이 틀림
		g_itMGR.OpenMsgBox(STR_BANK_REPLY_INVALID_PASSWORD);
		break;
	case BANK_REPLY_CLEARED_PASSWORD:// 창고 비번 삭제됨
		g_itMGR.OpenMsgBox( STR_BANK_REPLY_CLEARED_PASSWORD);
		break;
	default:
		break;
	}
}

void CRecvPACKET::Recv_gsv_MOVE_ITEM()
{
	if( !g_pAVATAR )
		return;

	if( m_pRecvPacket->m_gsv_MOVE_ITEM.m_nInvIDX >= 0 )
		g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_MOVE_ITEM.m_nInvIDX, m_pRecvPacket->m_gsv_MOVE_ITEM.m_InvItem );

	if( m_pRecvPacket->m_gsv_MOVE_ITEM.m_nBankIDX >= 0 )
		g_pAVATAR->SetBankItem( m_pRecvPacket->m_gsv_MOVE_ITEM.m_nBankIDX, m_pRecvPacket->m_gsv_MOVE_ITEM.m_BankITEM );

	// 패킷 사이즈 == gsv_MOVE_ITEM 이면				창고=>인벤토리 이동
	// 패킷 사이즈 == gsv_MOVE_ITEM+sizeof(__int64)면	인벤=>창고, m_iCurMoney[0]에 돈들어 있음
	if( m_pRecvPacket->m_HEADER.m_nSize > sizeof( gsv_MOVE_ITEM) )
		g_pAVATAR->Set_MONEY( m_pRecvPacket->m_gsv_MOVE_ITEM.m_iCurMoney[0] );

	if( m_pRecvPacket->m_gsv_MOVE_ITEM.m_nInvIDX < 0 && m_pRecvPacket->m_gsv_MOVE_ITEM.m_nBankIDX < 0 )
		g_itMGR.AppendChatMsg(STR_BANK_MOVEITEM_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );

}
//-------------------------------------------------------------------------------------------------
///2004/ 2 / 18 : Create : nAvy : 소모탄 관련
///2004/ 2 / 19 : Modify : nAvy : 현재 소모탄이 다 사용된경우 Inventory에서 삭제해준다.
void CRecvPACKET::Recv_gsv_SET_BULLET()
{
	CObjAVT* pObjAVT = g_pObjMGR->Get_CharAVT( g_pObjMGR->Get_ClientObjectIndex(m_pRecvPacket->m_gsv_SET_BULLET.m_wObjectIDX ), false);
	if( pObjAVT )
	{
		tagShotDATA ShotData = m_pRecvPacket->m_gsv_SET_BULLET.m_sShot;
		pObjAVT->SetShotData( ShotData.m_cType, ShotData.m_nItemNo );
	}
}
//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SERVER_DATA()
{
	switch( m_pRecvPacket->m_gsv_SERVER_DATA.m_btDataTYPE )
	{
	case SERVER_DATA_ECONOMY:
		{
			struct EconomyData{
				DWORD	m_nTown_COUNTER;					// 카운터 1분에 1씩 감소.		50~100

				short	m_nTown_POP_BASE;					// 기준 인구수.					100~8000
				short	m_nTown_DEV_BASE;					// 기준 발전도					10~100
				short	m_nTown_CONSUM[ MAX_PRICE_TYPE ];	// 아이템별 소비량

				short	m_nTownDEV;							// 마을 발전도
				int		m_iTownPOP;							// 마을 인구.
				int		m_iTownITEM[ MAX_PRICE_TYPE ];		// 아이템별 보유량				1000~100000
			};

			EconomyData Data;
			memcpy( &Data, m_pRecvPacket->m_gsv_SERVER_DATA.m_pDATA, sizeof( EconomyData ));


			sprintf( g_MsgBuf,"기준인구수:%d", Data.m_nTown_POP_BASE );
			g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );

			sprintf( g_MsgBuf,"기준 발전도:%d", Data.m_nTown_DEV_BASE );
			g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );

			for( int i = 1; i < MAX_PRICE_TYPE; ++i )
			{
				sprintf( g_MsgBuf,"아이템별 소비량(%d):%d", i,Data.m_nTown_CONSUM[i] );
				g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );
			}

			sprintf( g_MsgBuf,"마을 발전도:%d", Data.m_nTownDEV );
			g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );

			sprintf( g_MsgBuf,"마을 인구:%d", Data.m_iTownPOP );
			g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );


			for( int i = 1; i < MAX_PRICE_TYPE; ++i )
			{
				sprintf( g_MsgBuf,"아이템별 보유량(%d):%d", i,Data.m_iTownITEM[i] );
				g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );
			}


			break;
		}
	case SERVER_DATA_NPC:
		{
#define	MAX_OBJ_VAR_CNT		20
			struct tagObjVAR 
			{
				union 
				{
					struct 
					{
						int			m_iNextCheckTIME;
						t_HASHKEY	m_HashNextTrigger;
						int			m_iPassTIME;
						union {
							short	m_nEventSTATUS;
							short	m_nAI_VAR[ MAX_OBJ_VAR_CNT ];
						} ;
					} ;
					BYTE m_pVAR[ 1 ];
				};		
			};

			tagObjVAR* pObjVAR = (tagObjVAR*)m_pRecvPacket->m_gsv_SERVER_DATA.m_pDATA;

			g_itMGR.AppendChatMsg( CStr::Printf("NextCheckTIME: %d", pObjVAR->m_iNextCheckTIME ),g_dwYELLOW );
			g_itMGR.AppendChatMsg( CStr::Printf("HashNextTrigger: %d", pObjVAR->m_HashNextTrigger ), g_dwYELLOW );
			g_itMGR.AppendChatMsg( CStr::Printf("PassTime: %d", pObjVAR->m_iPassTIME ), g_dwYELLOW );
			g_itMGR.AppendChatMsg( CStr::Printf("EventStatus: %d", pObjVAR->m_nEventSTATUS), g_dwYELLOW );
			for( int i = 0 ; i < MAX_OBJ_VAR_CNT; ++i )
				g_itMGR.AppendChatMsg( CStr::Printf("AI_VAR(%d):%d", i+1, pObjVAR->m_nAI_VAR[ i ] ), g_dwYELLOW );

			break;
		}
	default:

		break;
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_ASSEMBLE_RIDE_ITEM()
{
	/*struct gsv_ASSEMBLE_RIDE_ITEM : public t_PACKETHEADER 
	{
	WORD	m_wObjectIDX;
	short	m_nRidingPartIDX;
	short	m_nItemNO;
	} ;*/

	CObjAVT *pAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_wObjectIDX, false );
	if ( pAVT ) 
	{
		pAVT->SetPetParts( m_pRecvPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_nRidingPartIDX,
			m_pRecvPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_RideITEM.m_nItemNo );
		//m_pRecvPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_nItemNO );

		pAVT->UpdatePet();

		/// 속도가 변했다면 속도 세팅
		if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_ASSEMBLE_RIDE_ITEM ) + sizeof(short) ) )
		{
			pAVT->SetOri_RunSPEED( m_pRecvPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_nRunSPEED[0] );
		}

	}else
	{
		;
		// ERROR
	}
}


void CRecvPACKET::Recv_GSV_SET_EVENT_STATUS()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_CharOBJ( g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_SET_EVENT_STATUS.m_wObjectIDX ), false );
	if ( pCHAR ) 
	{
		switch( pCHAR->Get_TYPE() )
		{
		case OBJ_NPC:
			{
				((CObjNPC*)pCHAR)->SetEventValue( m_pRecvPacket->m_gsv_SET_EVENT_STATUS.m_nEventSTATUS );
			}
			break;

		case OBJ_EVENTOBJECT:
			{
				CObjFixedEvent* pObj = (CObjFixedEvent*)pCHAR;					
				pObj->ExecEventScript( m_pRecvPacket->m_gsv_SET_EVENT_STATUS.m_nEventSTATUS );
			}
			break;			
		}
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_GSV_SET_ITEM_LIFE()
{	
	if( !g_pAVATAR )
		return;

	CItemSlot* pItemSlot =  g_pAVATAR->GetItemSlot();
	CItem* pItem = pItemSlot->GetItem( m_pRecvPacket->m_gsv_SET_ITEM_LIFE.m_nInventoryIDX );
	if( pItem )
	{
		pItem->SetLife( m_pRecvPacket->m_gsv_SET_ITEM_LIFE.m_nLife );

		if( pItem->GetItem().GetLife() <= 0 &&
			(ITEM_TYPE( pItem->GetItem().GetTYPE(), pItem->GetItem().GetItemNO() ) == TUNING_PART_ENGINE_CART ||
			ITEM_TYPE( pItem->GetItem().GetTYPE(), pItem->GetItem().GetItemNO() ) == TUNING_PART_ENGINE_CASTLEGEAR) )
		{						
			g_itMGR.AppendChatMsg( STR_CANT_REFAIR_PAT_ENGINE, IT_MGR::CHAT_TYPE_SYSTEM );
		}

		if( pItem->GetItem().GetLife() <= 0  )
		{
			g_pAVATAR->UpdateAbility();
		}	
	}
}


void CRecvPACKET::Recv_lsv_CHANNEL_LIST_REPLY ()
{

	DWORD worldserver_id = m_pRecvPacket->m_lsv_CHANNEL_LIST_REPLY.m_dwServerID;
	CServerList::GetInstance().ClearChannelServerList( worldserver_id );

	short nOffset = sizeof( lsv_CHANNEL_LIST_REPLY );

	if( m_pRecvPacket->m_lsv_CHANNEL_LIST_REPLY.m_btChannelCNT > 0 )
	{
		tagCHANNEL_SRV* pChannel = NULL;
		char* pszChannelName = NULL;
		int iFirstChannelNo = 0;
		for( int i = 0; i < m_pRecvPacket->m_lsv_CHANNEL_LIST_REPLY.m_btChannelCNT; ++i )
		{
			pChannel = (tagCHANNEL_SRV*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tagCHANNEL_SRV) );
			pszChannelName = Packet_GetStringPtr( m_pRecvPacket, nOffset);
			if( !pszChannelName )
				pszChannelName = CStr::Printf("Channel-%d", pChannel->m_btChannelNO );

			CServerList::GetInstance().AddChannelServerList( worldserver_id, pChannel->m_btChannelNO, pszChannelName, pChannel->m_nUserPERCENT );

			if( i == 0 )
				iFirstChannelNo = pChannel->m_btChannelNO;

		}
		g_EUILobby.HideMsgBox();
	}
}


//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "../GameData/CChatRoom.h"
void CRecvPACKET::Recv_wsv_CHATROOM ()
{
	switch( m_pRecvPacket->m_tag_CHAT_HEADER.m_btCMD ) {
		case CHAT_REPLY_ROOM_MAKED		:
			{
				short nOffset = sizeof( tag_CHAT_HEADER );
				WORD* pwMyID = (WORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(WORD) );
				//char* szRoomName = Packet_GetStringPtr( m_pRecvPacket, nOffset );

				CChatRoom::GetInstance().Made( *pwMyID );
				CChatRoom::GetInstance().SetMaster( *pwMyID );

				break;
			}
		case CHAT_REPLY_ROOM_JOINED		:
			{
				short nOffset = sizeof( tag_CHAT_HEADER );
				WORD* pwMyID = (WORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(WORD) );
				
				// UserTag				
#ifdef __PRIVATECHAT2
				DWORD * pdwMyTag = (DWORD*)Packet_GetDataPtr(m_pRecvPacket, nOffset, sizeof(DWORD));
#endif
				char* szRoomName = Packet_GetStringPtr( m_pRecvPacket, nOffset );

				CChatRoom::GetInstance().SetTitle( szRoomName );
				CChatRoom::GetInstance().Join();			

				WORD* pwUserID;
				DWORD * pdwUserTag;
				char* szUserName;
				bool bSetMaster = false;

				for( ; nOffset < m_pRecvPacket->m_HEADER.m_nSize ; )
				{
					pwUserID = (WORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( WORD ));
					
					// UserTag				
#ifdef __PRIVATECHAT2
					pdwUserTag = (DWORD*)Packet_GetDataPtr(m_pRecvPacket, nOffset, sizeof(DWORD));
					szUserName = Packet_GetStringPtr(m_pRecvPacket, nOffset);
					CChatRoom::GetInstance().JoinMember( *pwUserID, szUserName, *pdwUserTag );
#else
					szUserName = Packet_GetStringPtr(m_pRecvPacket, nOffset);
					CChatRoom::GetInstance().JoinMember( *pwUserID, szUserName );
#endif
					if( !bSetMaster )
					{
						CChatRoom::GetInstance().SetMaster( *pwUserID );
						bSetMaster = true;
					}
				}

#ifdef __PRIVATECHAT2
				if( g_pAVATAR )
				{
					CChatRoom::GetInstance().JoinMember( 
						*pwMyID,
						g_pAVATAR->Get_NAME(),
						*pdwMyTag );
				}
#else
				if( g_pAVATAR )
				{
					CChatRoom::GetInstance().JoinMember( 
						*pwMyID,
						g_pAVATAR->Get_NAME() );
				}
#endif
				

				break;
			}

		case CHAT_REPLY_MAKE_FAILED		:	// 방만들기 실패...
			g_itMGR.OpenMsgBox(STR_CHAT_REPLY_MAKE_FAILED);
			CChatRoom::GetInstance().SetState( CChatRoom::STATE_DEACTIVATED );

			CChatRoom::GetInstance().SetPassword(NULL);
			CChatRoom::GetInstance().SetTitle(NULL);
			break;

		case CHAT_REPLY_MAKE_TITLE_DUP		:	//...방제 중복으로 방만들기 실패
			g_itMGR.OpenMsgBox(LIST_STRING(952));
			CChatRoom::GetInstance().SetState( CChatRoom::STATE_DEACTIVATED );

			CChatRoom::GetInstance().SetPassword(NULL);
			CChatRoom::GetInstance().SetTitle(NULL);
			break;

		case CHAT_REPLY_INVALIED_PASS	:	// 참가 요청한 방의 비번이 틀림.
			g_itMGR.OpenMsgBox( STR_CHAT_REPLY_INVALIED_PASS );
			CChatRoom::GetInstance().SetState( CChatRoom::STATE_DEACTIVATED );
			break;
		case CHAT_REPLY_ROOM_NOT_FOUND	:	// 요청한 방이 뽀개졌음.
			g_itMGR.OpenMsgBox(STR_CHAT_REPLY_ROOM_NOT_FOUND);
			CChatRoom::GetInstance().SetState( CChatRoom::STATE_DEACTIVATED );
			///채팅리스트에서 삭제할것
			break;
		case CHAT_REPLY_FULL_USERS		:	// 요청한 방이 만땅임..
			g_itMGR.OpenMsgBox(STR_CHAT_REPLY_FULL_USERS);
			CChatRoom::GetInstance().SetState( CChatRoom::STATE_DEACTIVATED );
			break;

		case CHAT_REPLY_KICKED			:	
			{
				if( g_pAVATAR )
				{
					// CHAT_REPLY_USER_LEFT와 같은 구조... 쫏겨난 사람한테만 온다.. 다른 사람한데는 CHAT_REPLY_USER_LEFT로 전송됨 
					if( g_pObjMGR->Get_ServerObjectIndex(g_pAVATAR->Get_INDEX()) 
						== m_pRecvPacket->m_wsv_CHAT_ROOM_USER.m_wUserID )
					{
						CChatRoom::GetInstance().Leave();
						g_itMGR.AppendChatMsg(STR_CHAT_REPLY_KICKED,IT_MGR::CHAT_TYPE_SYSTEM );
					}
				}
				break;
			}
		case CHAT_REPLY_USER_LEFT		:
			{
				CChatRoom::GetInstance().LeaveMember( m_pRecvPacket->m_wsv_CHAT_ROOM_USER.m_wUserID );
				break;
			}
		case CHAT_REPLY_USER_LEFT_NAME:
			{
				short nOffset = sizeof( tag_CHAT_HEADER );
				char* pszName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
				CChatRoom::GetInstance().LeaveMember( pszName );
				break;
			}
		case CHAT_REPLY_USER_ADD		:	
			{
				char* pszUserName;
				short nOffset = sizeof( wsv_CHAT_ROOM_USER );	

				pszUserName = Packet_GetStringPtr(m_pRecvPacket, nOffset);

				// UserTag				
#ifdef __PRIVATECHAT2
				CChatRoom::GetInstance().JoinMember( 
					m_pRecvPacket->m_wsv_CHAT_ROOM_USER.m_wUserID,
					pszUserName,
					m_pRecvPacket->m_wsv_CHAT_ROOM_USER.m_dwDBID );
#else
				CChatRoom::GetInstance().JoinMember( 
					m_pRecvPacket->m_wsv_CHAT_ROOM_USER.m_wUserID,
					pszUserName );
#endif

				
				break;
			}

		case CHAT_REPLY_ROOM_LIST_END:
		case CHAT_REPLY_ROOM_LIST:
			{
				tag_CHAT_ROOM *pRoom;
				short nOffset = sizeof( wsv_CHAT_ROOM_LIST );
				char *szRoomName;

				CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
				assert( pDlg );
				if( pDlg == NULL ) break;

				CCommDlg* pCommDlg = (CCommDlg*)pDlg;
				for (short nI=0; nI<m_pRecvPacket->m_wsv_CHAT_ROOM_LIST.m_cRoomCNT; nI++) 
				{
					pRoom = (tag_CHAT_ROOM*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_CHAT_ROOM) );
					szRoomName = Packet_GetStringPtr( m_pRecvPacket, nOffset );

					pCommDlg->AddChatRoom( szRoomName, pRoom->m_btRoomTYPE, pRoom->m_nRoomIDX, pRoom->m_cUserCNT );
				}
				break;
			}
	}
}
#include "../interface/dlgs/subclass/CFriendListItem.h"
void CRecvPACKET::Recv_tag_MCMD_HEADER ()
{
	switch( m_pRecvPacket->m_tag_MCMD_HEADER.m_btCMD ) 
	{
	case MSGR_CMD_APPEND_REQ	:
		{
				if( g_ClientStorage.IsApproveAddFriend() )
				{
					if(g_pAVATAR->GetAddFriendWnd() == false)
					{
						g_pAVATAR->SetAddFriendWnd(true);
					CTCmdAcceptAddFriend* pAcceptCmd = new CTCmdAcceptAddFriend( m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_wUserIDX,
						0,
						m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_szName);

					CTCmdRejectAddFriend* pRejectCmd = new CTCmdRejectAddFriend( m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_wUserIDX ,
#ifdef __PRIVATECHAT2
						g_pAVATAR->Get_NAME() );
#else
						m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_szName );
#endif					

					g_itMGR.OpenMsgBox( CStr::Printf( F_STR_QUERY_APPEND_FRIEND_REQ, m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_szName ),
						CMsgBox::BT_OK | CMsgBox::BT_CANCEL, 
						false,
						0,
						pAcceptCmd,
						pRejectCmd );
					}
                }
				else
				{
					CTCmdRejectAddFriend Command(	m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_wUserIDX ,
#ifdef __PRIVATECHAT2
													g_pAVATAR->Get_NAME() );
#else
													m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_szName );
#endif					
													
					Command.Exec( NULL );
				}
			break;
		}
	case MSGR_CMD_APPEND_ACCEPT	:
		{
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
			assert( pDlg );
			if( pDlg )
			{
				CCommDlg* pCommDlg = (CCommDlg*)pDlg;
				//기존에 
				if( CFriendListItem* pItem = pCommDlg->FindFriend( m_pRecvPacket->m_wsv_MCMD_APPEND_ACCEPT.m_dwUserTAG ) )
				{
					pItem->SetStatus( m_pRecvPacket->m_wsv_MCMD_APPEND_ACCEPT.m_btStatus );
				}
				else
				{
					pCommDlg->AddFriend( m_pRecvPacket->m_wsv_MCMD_APPEND_ACCEPT.m_dwUserTAG,
						m_pRecvPacket->m_wsv_MCMD_APPEND_ACCEPT.m_btStatus,
						m_pRecvPacket->m_wsv_MCMD_APPEND_ACCEPT.m_szName );
				}

			}
			break;
		}
	case MSGR_CMD_APPEND_REJECT	:
		{
			// 친구 추가 요청한 상대(szName)가 거절했다.
			short nOffset = sizeof( tag_MCMD_HEADER );
			char *szName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
			g_itMGR.OpenMsgBox(CStr::Printf(F_STR_REJECT_APPEND_FRIEND_REQ, szName) );
			break;
		}
	case MSGR_CMD_NOT_FOUND		:
		{
			// 친구 추가 요청한 상대(szName)가 없다.
			short nOffset = sizeof( tag_MCMD_HEADER );
			char *szName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
			g_itMGR.OpenMsgBox(CStr::Printf(F_STR_NOT_FOUNDORCONNECT_FRIEND, szName) );
			break;
		}
	case MSGR_CMD_CHANGE_STATUS	:
		{
			// 사용자 m_pRecvPacket->m_wsv_MCMD_STATUS_REPLY.m_dwUserTAG의 상태가
			// m_pRecvPacket->m_wsv_MCMD_STATUS_REPLY.m_btStatus로 바꿔었다.
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
			assert( pDlg );
			if( pDlg )
			{
				CCommDlg* pCommDlg = (CCommDlg*)pDlg;
				pCommDlg->ChangeFriendStatus( m_pRecvPacket->m_wsv_MCMD_STATUS_REPLY.m_dwUserTAG,
					m_pRecvPacket->m_wsv_MCMD_STATUS_REPLY.m_btStatus );
			}		
			break;
		}
	case MSGR_CMD_LIST:
		{
			// 친구 리스트...
#pragma pack (push, 1)
			struct tagFriend {
				DWORD	m_dwUserTAG;
				BYTE	m_btStatus;
			} *pFR;
#pragma pack (pop)
			char *szName;
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
			assert( pDlg );
			if( pDlg )
			{
				CCommDlg* pCommDlg = (CCommDlg*)pDlg;
				pCommDlg->ClearFriendList();

				short nOffset = sizeof( wsv_MCMD_LIST );
				for (short nI=0; nI<m_pRecvPacket->m_wsv_MCMD_LIST.m_btFriendCNT; nI++) 
				{
					pFR = (tagFriend*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tagFriend ) );
					szName = Packet_GetStringPtr( m_pRecvPacket, nOffset );

					pCommDlg->AddFriend( pFR->m_dwUserTAG, pFR->m_btStatus, szName );
				}
			}
			break;
		}
	default:
		break;
		/*
		case MSGR_CMD_DELETE		:	
		{
		break;
		}
		case MSGR_CMD_REFUSE		:	
		{
		break;
		}
		*/
	}
}

//----------------------------------------------------------------------------------------------------
#include "../interface/dlgs/subclass/CFriendListItem.h"
#include "../interface/dlgs/CPrivateChatDlg.h"
void CRecvPACKET::Recv_wsv_MESSENGER_CHAT ()
{
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
	assert( pDlg );
	if( pDlg == NULL ) return;


#ifdef __PRIVATECHAT2	
		//홍근 : 일본 빌링 수정
	#ifdef __JAPAN_BILL
		if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_COMMUNITY) )
		{					
			g_pNet->Send_cli_WHISPER(  m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_FromName, STR_JP_BILL_CANT_SEND_MSG );
			return;
		}        
	#endif

		if( g_ClientStorage.IsApproveMessanger() )
		{
			CPrivateChatDlg* pPrivateChatDlg = g_itMGR.GetPrivateChatDlg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG );
			if( pPrivateChatDlg  == NULL )
			{
				g_itMGR.OpenPrivateChatDlg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG,
											FRIEND_STATUS_NORMAL,
											m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_FromName );
			}

			if( pPrivateChatDlg = g_itMGR.GetPrivateChatDlg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG ) )
			{
				pPrivateChatDlg->RecvChatMsg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG ,
					m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_szMSG );
			}
			else
			{
				assert( pPrivateChatDlg );
			}
		}
		else
		{			
			g_pNet->Send_cli_WHISPER(	m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_FromName,
										STR_OTHERSTATE_IS_REJCTMESSANGER );
		}

#else
	CCommDlg* pCommDlg = (CCommDlg*)pDlg;
	CFriendListItem* pItem = pCommDlg->FindFriend( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG );

	if( pItem )
	{

		//홍근 : 일본 빌링 수정
#ifdef __JAPAN_BILL
		if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_COMMUNITY) )
		{					
			g_pNet->Send_cli_WHISPER(  (char*)pItem->GetName(), STR_JP_BILL_CANT_SEND_MSG );
			return;
		}        
#endif

		if( g_ClientStorage.IsApproveMessanger() )
		{
			CPrivateChatDlg* pPrivateChatDlg = g_itMGR.GetPrivateChatDlg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG );
			if( pPrivateChatDlg  == NULL )
				g_itMGR.OpenPrivateChatDlg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG, pItem->GetStatus(), pItem->GetName() );

			if( pPrivateChatDlg = g_itMGR.GetPrivateChatDlg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG ) )
			{
				pPrivateChatDlg->RecvChatMsg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG ,
					m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_szMSG );
			}
			else
			{
				assert( pPrivateChatDlg );
			}
		}
		else
		{			
			g_pNet->Send_cli_WHISPER(  (char*)pItem->GetName(), STR_OTHERSTATE_IS_REJCTMESSANGER );
		}
	}
#endif
	
}
//----------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_wsv_MEMO()
{
	switch( m_pRecvPacket->m_wsv_MEMO.m_btTYPE )
	{
	case MEMO_REPLY_RECEIVED_CNT:
		{
			if( g_pAVATAR )
			{
				g_pAVATAR->SetServerSavedMemoCount( m_pRecvPacket->m_wsv_MEMO.m_nRecvCNT[0] );
				if( g_pAVATAR->GetServerSavedMemoCount() > 0 )
					g_pNet->Send_cli_MEMO_REQ();
			}
			break;
		}
	case MEMO_REPLY_CONTENTS:
		{
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
			assert( pDlg );
			if( pDlg )
			{
				CCommDlg* pCommDlg = (CCommDlg*)pDlg;

				DWORD*	pdwRecvTime;
				char*	pszFromName;
				char*	pszMemo;

				for( short	nOffset = sizeof( wsv_MEMO ) ; nOffset < m_pRecvPacket->m_HEADER.m_nSize ; )
				{
					pdwRecvTime = (DWORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( DWORD ) );
					pszFromName	= Packet_GetStringPtr( m_pRecvPacket, nOffset );
					pszMemo		= Packet_GetStringPtr( m_pRecvPacket, nOffset );

					///파일에 저장
					//pCommDlg->AddMemo( pszFromName, pszMemo, *pdwRecvTime );
					CSqliteDB::GetInstance().AddMemo( g_pAVATAR->Get_NAME(), pszFromName, pszMemo, *pdwRecvTime );

					if( g_pAVATAR )
						g_pAVATAR->DecreaseServerSavedMemoCount();
				}

				if( g_pAVATAR )
				{
					if( g_pAVATAR->GetServerSavedMemoCount() > 0 )
						g_pNet->Send_cli_MEMO_REQ();
				}
			}
			break;
		}
	case MEMO_REPLY_SEND_OK:
		break;
	case MEMO_REPLY_SEND_INVALID_TARGET:
	case MEMO_REPLY_SEND_NOT_EXIST:
		g_itMGR.OpenMsgBox(STR_MEMO_REPLY_SEND_NOT_EXISTORINVALID);
		break;
	case MEMO_REPLY_SEND_REFUSED:
		g_itMGR.OpenMsgBox(STR_MEMO_REPLY_SEND_REFUSED);
		break;
	case MEMO_REPLY_SEND_FULL_MEMO:
		g_itMGR.OpenMsgBox(STR_MEMO_REPLY_SEND_FULL_MEMO);
		break;
	case MEMO_REPLY_SEND_INVALID_CONTENT:
		g_itMGR.OpenMsgBox(STR_MEMO_REPLY_SEND_INVALID_CONTENT);
		break;
	default:
		break;
	}
}
#include "../interface/dlgs/CChatRoomDlg.h"
void CRecvPACKET::Recv_wsv_CHATROOM_MSG ()
{
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHATROOM );
	assert( pDlg );
	if( pDlg )
	{
		CChatRoomDlg* pChatRoomDlg = ( CChatRoomDlg*) pDlg;
		pChatRoomDlg->RecvChatMsg( m_pRecvPacket->m_wsv_CHATROOM_MSG.m_wObjectID,
			m_pRecvPacket->m_wsv_CHATROOM_MSG.m_szMSG
			);
	}
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CRAFT_ITEM_REPLY()
{
	//struct gsv_CRAFT_ITEM_REPLY : public t_PACKETHEADER {
	//BYTE			m_btRESULT;
	//BYTE			m_btOutCNT;			// 변경된 아이템 갯수
	//tag_SET_INVITEM	m_sInvITEM[ 0 ];	// 변경된 갯수 만큼 들어 있다... 
	//									// 예외) CRAFT_UPGRADE_SUCCESS, CRAFT_UPGRADE_FAILED 일경우
	//									// m_sInvITEM[ m_btOutCNT-1 ].m_uiQuantity에 성공도 계산된값이 들어있음
	//} ;

	switch( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btRESULT )
	{
	case	CRAFT_GEMMING_SUCCESS:		//	0x01
		{


			g_pAVATAR->SetWaitUpdateInventory( true );
			for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
			{
				int iPartIdx = CInventory::GetBodyPartByEquipSlot( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX );

				if( g_pAVATAR )
				{
					g_pAVATAR->SetPartITEM( iPartIdx,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
					g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
					/// Equip 패킷이 날라오기때문에 속성 업데이트는 불필요해 보인다.
					/// 아이템 슬롯만 갱신시키면 될듯..
					g_pAVATAR->Update();
				}
			}
			g_pAVATAR->SetWaitUpdateInventory( false );
			g_pAVATAR->UpdateInventory();


			g_itMGR.AppendChatMsg( STR_GEMMING_SUCCESS, IT_MGR::CHAT_TYPE_SYSTEM );					
		}
		break;
	case	CRAFT_GEMMING_NEED_SOCKET:	//	0x02	// 소켓없다.
		{
			g_itMGR.AppendChatMsg( STR_GEMMING_NEED_SOCKET, IT_MGR::CHAT_TYPE_SYSTEM );					
		}
		break;
	case	CRAFT_GEMMING_USED_SOCKET:	//	0x03	// 재밍된 경우등....
		break;

	case	CRAFT_BREAKUP_SUCCESS_GEM:	//	0x04	// 보석 분리 성공
		{
			if( g_pAVATAR )
			{
				g_pAVATAR->SetWaitUpdateInventory( true );
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
				{
					g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
				}
				g_pAVATAR->SetWaitUpdateInventory( false );
				g_pAVATAR->UpdateInventory();

				g_itMGR.OpenMsgBox(STR_CRAFT_BREAKUP_SUCCESS_GEM);

				CSeparate& Separate = CSeparate::GetInstance();
				switch( Separate.GetType() )
				{
				case CSeparate::TYPE_SKILL:
					g_pAVATAR->Sub_MP( Separate.GetRequireMp() );
					break;
				case CSeparate::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Separate.GetRequireMoney() );
					break;
				default:
					break;
				}			

				SE_SuccessSeparate( g_pAVATAR->Get_INDEX() );
			}

			break;
		}
	case	CRAFT_BREAKUP_DEGRADE_GEM:	//	0x05	// 보석 분리 성공, 보석등급 강등
		{
			if( g_pAVATAR )
			{
				g_pAVATAR->SetWaitUpdateInventory( true );
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
				{
					g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
				}
				g_pAVATAR->SetWaitUpdateInventory( false );
				g_pAVATAR->UpdateInventory();

				CSeparate& Separate = CSeparate::GetInstance();
				switch( Separate.GetType() )
				{
				case CSeparate::TYPE_SKILL:
					g_pAVATAR->Sub_MP( Separate.GetRequireMp() );
					break;
				case CSeparate::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Separate.GetRequireMoney() );
					break;
				default:
					break;
				}

				SE_SuccessSeparate( g_pAVATAR->Get_INDEX() );
				g_itMGR.OpenMsgBox(STR_CRAFT_BREAKUP_DEGRADE_GEM);
			}
			break;
		}
	case	CRAFT_BREAKUP_CLEARED_GEM:	//	0x06	// 보석 분리 성공, 보석 삭제됨
		{
			if( g_pAVATAR )
			{
				g_pAVATAR->SetWaitUpdateInventory( true );
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
				{
					g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
				}
				g_pAVATAR->SetWaitUpdateInventory( false );
				g_pAVATAR->UpdateInventory();


				CSeparate& Separate = CSeparate::GetInstance();
				switch( Separate.GetType() )
				{
				case CSeparate::TYPE_SKILL:
					g_pAVATAR->Sub_MP( Separate.GetRequireMp() );
					break;
				case CSeparate::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Separate.GetRequireMoney() );
					break;
				default:
					break;
				}

				g_itMGR.OpenMsgBox(STR_CRAFT_BREAKUP_CLEARED_GEM);
				SE_SuccessSeparate( g_pAVATAR->Get_INDEX() );
			}
			break;
		}

	case	CRAFE_BREAKUP_SUCCESS:		//	0x07	// 아이템 분해 성공
		{
#ifdef _NEWBREAK
			if( g_pAVATAR )
			{

				CSeparate& Separate = CSeparate::GetInstance();

				switch( Separate.GetType() )
				{
				case CSeparate::TYPE_SKILL:
					//06. 01. 1 :김주현 - 뉴버젼에서는 분해스킬이 기본스킬 이므로 MP소모를 안한다.
					break;

				case CSeparate::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Separate.GetRequireMoney() );
					break;

				default:
					break;
				}

				Separate.ClearResultItemSet();

				// 05.11.30 - 김주현
				// 주의 m_sInvITEM 마지막의 m_btInvIDX 에 분해할려고 올렸던 아이템의 실제 인덱스가 들어있다.
				//		m_sInvITEM 마지막의	m_ITEM에는 0 으로 채워져있다.. 주의.
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
					Separate.AddResultItemSet( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ] );

				Separate.SetResult(m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btRESULT );

			}
			break;
#else
			if( g_pAVATAR )
			{
				CSeparate& Separate = CSeparate::GetInstance();
				switch( Separate.GetType() )
				{
				case CSeparate::TYPE_SKILL:
					g_pAVATAR->Sub_MP( Separate.GetRequireMp() );
					break;
				case CSeparate::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Separate.GetRequireMoney() );
					break;
				default:
					break;
				}

				tagITEM		ItemData;
				tagITEM*	pItem = NULL;
				//CIcon*		pIcon = NULL;
				CreateMsgBoxData MsgBoxData;
				MsgBoxData.strMsg = STR_CRAFE_BREAKUP_SUCCESS;
				CIconItem* pItemIcon = NULL;

				g_pAVATAR->SetWaitUpdateInventory( true );
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
				{
					pItem = &m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM;
					if( !pItem->IsEmpty() )
					{
						if( !pItem->IsEnableDupCNT() )
						{
							pItemIcon = new CIconItem(pItem);		
							MsgBoxData.m_Icons.push_back( pItemIcon );
						}
						else
						{
							ItemData = *pItem;
							if( ItemData.m_uiQuantity > g_pAVATAR->m_Inventory.m_ItemLIST[ m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX ].m_uiQuantity )
							{
								ItemData.m_uiQuantity -= g_pAVATAR->m_Inventory.m_ItemLIST[ m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX ].m_uiQuantity;
								pItemIcon = new CIconItem(&ItemData);
								MsgBoxData.m_Icons.push_back( pItemIcon );
							}
						}
					}

					g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
				}
				g_pAVATAR->SetWaitUpdateInventory( false );
				g_pAVATAR->UpdateInventory();

				g_itMGR.OpenMsgBox2(MsgBoxData);
				SE_SuccessSeparate( g_pAVATAR->Get_INDEX() );
			}
			break;

#endif
		}
	case	CRAFE_INVALID_ITEM:  // 뭔가 잘못됬다!! 
		{
#ifdef _NEWINTERFACEKJH
			g_itMGR.OpenMsgBox( "CRAFT_INVALID_ITEM_ERROR" );
#endif
		}
		break;
	case	CRAFT_UPGRADE_SUCCESS:		//	0x10	// 재련 성공
		{
			if( g_pAVATAR )
			{
				CUpgrade& Upgrade = CUpgrade::GetInstance();
				Upgrade.ClearResultItemSet();

				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT - 1; i++ )
					Upgrade.AddResultItemSet( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ] );

				Upgrade.SetResultSuccessProb( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT - 1].m_ITEM.m_uiQuantity );
				Upgrade.SetResult( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btRESULT  );

				switch( Upgrade.GetType() )
				{
				case CUpgrade::TYPE_SKILL:
					g_pAVATAR->Sub_MP( Upgrade.GetRequireMp() );
					break;
				case CUpgrade::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Upgrade.GetRequireMoney() );
					break;
				default:
					break;
				}
			}
			break;
		}
	case	CRAFT_UPGRADE_FAILED:		//	0x11	// 재련 실패
		{
			if( g_pAVATAR )
			{
				CUpgrade& Upgrade = CUpgrade::GetInstance();
				Upgrade.ClearResultItemSet();
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT - 1; i++ )
					Upgrade.AddResultItemSet( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ] );

				Upgrade.SetResultSuccessProb( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT - 1].m_ITEM.m_uiQuantity );

				// 07. 03. 08 - 김주현 : 재련한 아이템의 등급이 하락할 경우에 m_sInvITEM[1] 자리에 아이템의 정보가 온다. 0x99 로 세팅후 CUpgradeDlgStateResult에서 등급 하락 스트링을 출력한다.
				if(m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[1].m_ITEM.GetItemNO() != 0)
					Upgrade.SetResult( 0x99 );
				else
					Upgrade.SetResult( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btRESULT  );

				switch( Upgrade.GetType() )
				{
				case CUpgrade::TYPE_SKILL:
					g_pAVATAR->Sub_MP( Upgrade.GetRequireMp() );
					break;
				case CUpgrade::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Upgrade.GetRequireMoney() );
					break;
				default:
					break;
				}
			}
		}
		break;
	case	CRAFT_UPGRADE_INVALID_MAT:	//	0x12	// 재료 아이템이 잘못됐다.
		{
			CUpgrade::GetInstance().SetResult( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btRESULT  );
			break;
		}
	
	case	CRAFT_GIFTBOX_SUCCESS:		// 0x13		// 선물 상자 분해
		{
			if( g_pAVATAR )
			{
				tagITEM		ItemData;
				tagITEM*	pItem = NULL;
				//CIcon*		pIcon = NULL;
				CreateMsgBoxData MsgBoxData;
				MsgBoxData.strMsg = STR_GETTING_GIFTBOX;
				CIconItem* pItemIcon = NULL;

				g_pAVATAR->SetWaitUpdateInventory( true );
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
				{
					pItem = &m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM;
					if( !pItem->IsEmpty() )
					{
						if( !pItem->IsEnableDupCNT() )
						{
							pItemIcon = new CIconItem(pItem);		
							MsgBoxData.m_Icons.push_back( pItemIcon );
						}
						else
						{
							ItemData = *pItem;
							if( ItemData.m_uiQuantity > g_pAVATAR->m_Inventory.m_ItemLIST[ m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX ].m_uiQuantity )
							{
								ItemData.m_uiQuantity -= g_pAVATAR->m_Inventory.m_ItemLIST[ m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX ].m_uiQuantity;
								pItemIcon = new CIconItem(&ItemData);
								MsgBoxData.m_Icons.push_back( pItemIcon );
							}
						}
					}

					g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
				}
				g_pAVATAR->SetWaitUpdateInventory( false );
				g_pAVATAR->UpdateInventory();

				g_itMGR.OpenMsgBox2(MsgBoxData);
				SE_SuccessGiftBox( g_pAVATAR->Get_INDEX() );
			}
			break;
		}
	case  CRAFT_DRILL_SOCKET_SUCCESS	:    //소캣 생성 성공
		{
			g_pAVATAR->SetWaitUpdateInventory( true );
			for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
			{
				g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
					m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
			}
			g_pAVATAR->SetWaitUpdateInventory( false );
			g_pAVATAR->UpdateInventory();
			CGame::GetInstance().EndMakeSocket(); 
			g_itMGR.OpenMsgBox( STR_MAKESOCKET_SUCCESS );
		}
		SE_SuccessUpgrade( g_pAVATAR->Get_INDEX() );
		break;
	case  CRAFT_DRILL_CLEAR_USEITME      :    // 드릴 아이템만 사라졌다..
		{
			g_pAVATAR->SetWaitUpdateInventory( true );
			for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
			{
				g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
					m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
			}
			g_pAVATAR->SetWaitUpdateInventory( false );
			g_pAVATAR->UpdateInventory();
			CGame::GetInstance().EndMakeSocket();
			g_itMGR.OpenMsgBox( STR_MAKESOCKET_FAILED );
		}
		SE_FailUpgrade( g_pAVATAR->Get_INDEX() );
		break;
	case CRAFE_DRILL_INVALID_USEITEM   :    //드릴 아이템이 아니다.
		{
			int k=0;
		}
		break;
	case	CRAFT_DRILL_EXIST_SOCKET	        :	  // 소켓이 이미 있다.
		{
			int k=0;

		}
		break;
	case	CRAFT_DRILL_INVALID_TITEM	    :	  //소켓을뚫을 수 없는 아이템이다...
		{
			int k=0;

		}
		break;
	case CRAFE_NOT_ENOUGH_MONEY:
		{
			g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_MONEY );
			break;
		}
	case CRAFE_NOT_ENOUGH_MP:
		{
			g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_MANA);
			break;
		}
	default:
		break;
	}
}

void CRecvPACKET::Recv_gsv_USED_ITEM_TO_REPAIR()
{
	Recv_gsv_SET_INV_ONLY();

	if( g_pAVATAR )
		g_pAVATAR->UpdateAbility();
}
void CRecvPACKET::Recv_gsv_REPAIRED_FROM_NPC()
{
	Recv_gsv_SET_MONEYnINV();

	if( g_pAVATAR )
		g_pAVATAR->UpdateAbility();
}

void CRecvPACKET::Recv_gsv_SET_MONEY_ONLY()
{
	__int64 i64Diff = m_pRecvPacket->m_gsv_SET_MONEY_ONLY.m_i64Money - g_pAVATAR->Get_MONEY();

	if( i64Diff > 0 )
		g_itMGR.AppendChatMsg( CStr::Printf(F_STR_SET_MONEY_INC, i64Diff) ,IT_MGR::CHAT_TYPE_SYSTEM );
	else
		g_itMGR.AppendChatMsg( CStr::Printf(F_STR_SET_MONEY_DEC, -i64Diff),IT_MGR::CHAT_TYPE_SYSTEM );

	if( g_pAVATAR )
		g_pAVATAR->Set_MONEY( m_pRecvPacket->m_gsv_SET_MONEY_ONLY.m_i64Money );
}

void CRecvPACKET::Recv_gsv_REWARD_MONEY()
{
	__int64 i64Diff = m_pRecvPacket->m_gsv_SET_MONEY_ONLY.m_i64Money - g_pAVATAR->Get_MONEY();

	if( i64Diff > 0 )
		g_itMGR.AppendChatMsg( CStr::Printf(F_STR_REWARD_MONEY_INC, i64Diff) , IT_MGR::CHAT_TYPE_QUESTREWARD );
	else
		g_itMGR.AppendChatMsg( CStr::Printf(F_STR_REWARD_MONEY_DEC, -i64Diff), IT_MGR::CHAT_TYPE_QUESTREWARD );

	if( g_pAVATAR )
		g_pAVATAR->Set_MONEY( m_pRecvPacket->m_gsv_SET_MONEY_ONLY.m_i64Money );
}


//----------------------------------------------------------------------------------------------------	
///
/// @brief 퀘스트 보상관련
///
//----------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_REWARD_ITEM()
{
	if( !g_pAVATAR )
		return;

	for ( int i = 0; i < m_pRecvPacket->m_gsv_SET_INV_ONLY.m_btItemCNT ; i++ ) 
	{
		char *szMsg = NULL;

		tagITEM *pInvITEM = NULL;
		tagITEM *pSrvITEM = NULL;

		pSrvITEM = &m_pRecvPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[i].m_ITEM;
		if( !pSrvITEM->IsEmpty() )
		{
			if ( pSrvITEM->IsEnableDupCNT() ) 
			{
				pInvITEM = &g_pAVATAR->m_Inventory.m_ItemLIST[ m_pRecvPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[i].m_btInvIDX ];
				szMsg = CStr::Printf(F_STR_REWARD_ITEMS, ITEM_NAME( pSrvITEM->GetTYPE(), pSrvITEM->GetItemNO() ), pSrvITEM->GetQuantity() - pInvITEM->GetQuantity() );
			} 
			else 
			{
				szMsg = CStr::Printf(F_STR_REWARD_ITEM, ITEM_NAME( pSrvITEM->GetTYPE(), pSrvITEM->GetItemNO() ) );
			}

			g_itMGR.AppendChatMsg( szMsg, IT_MGR::CHAT_TYPE_QUESTREWARD );
		}
	}
}

void CRecvPACKET::Recv_gsv_REWARD_ADD_ABILITY()
{
	g_QuestRewardQueue.PushAddAbility( m_pRecvPacket->m_gsv_SET_ABILITY );
}

void CRecvPACKET::Recv_gsv_REWARD_SET_ABILITY()
{
	g_QuestRewardQueue.PushSetAbility( m_pRecvPacket->m_gsv_SET_ABILITY );
}

void CRecvPACKET::Recv_gsv_PARTY_ITEM()
{
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_ITEM_1%d\n",g_GameDATA.GetGameTime() );
	CParty& refParty = CParty::GetInstance();
	assert( refParty.HasParty() );
	if( refParty.HasParty() )
	{
		PartyMember Member;
		if( refParty.GetMemberInfoByObjSvrIdx( m_pRecvPacket->m_gsv_PARTY_ITEM.m_wObjectIDX , Member ) )
		{						
			g_itMGR.AppendChatMsg( 				
				CStr::Printf( m_pRecvPacket->m_gsv_PARTY_ITEM.m_ITEM.GettingMESSAGE_Party( Member.m_strName.c_str() ) ),
				IT_MGR::CHAT_TYPE_SYSTEM );			
		}		
	}
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_ITEM_2%d\n",g_GameDATA.GetGameTime() );
}

void CRecvPACKET::Recv_gsv_PARTY_RULE()
{
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_RULE_1(%d)\n",g_GameDATA.GetGameTime() );
	CParty::GetInstance().RecvPartyRule( m_pRecvPacket->m_gsv_PARTY_RULE.m_btPartyRULE );
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_RULE_2(%d)\n",g_GameDATA.GetGameTime() );
}

void CRecvPACKET::Recv_wsv_CHAR_CHANGE()
{
	CGame::GetInstance().ChangeState( CGame::GS_EXITMAIN );
}

void CRecvPACKET::Recv_gsv_LOGOUT_REPLY()
{
	g_itMGR.SetWaitDisconnectTime( m_pRecvPacket->m_gsv_LOGOUT_REPLY.m_wWaitSec );
	g_itMGR.ChangeState( IT_MGR::STATE_WAITDISCONNECT );
}

void CRecvPACKET::Recv_wsv_DELETE_CHAR ()
{
	short nOffset = (short)sizeof( wsv_DELETE_CHAR);
	char *szName = Packet_GetStringPtr( m_pRecvPacket, nOffset );

	CJustModelAVT* pAVT = CGameDataCreateAvatar::GetInstance().FindAvatar( szName );
	assert( pAVT );
	if( pAVT )
	{
		if( m_pRecvPacket->m_wsv_DELETE_CHAR.m_dwDelRemainTIME == 0xFFFFFFFF )
		{
			g_EUILobby.ShowMsgBox( STR_CLAN_CANT_DELETE_CLANMASTER, CTMsgBox::BT_OK ,true , EUI_SELECT_AVATA );
		}
		else
		{
			pAVT->SetDeleteRemainSEC( m_pRecvPacket->m_wsv_DELETE_CHAR.m_dwDelRemainTIME );
			if( m_pRecvPacket->m_wsv_DELETE_CHAR.m_dwDelRemainTIME )
			{
				pAVT->SwitchState( CJustModelAVT::STATE_SITDOWN );

				pAVT->m_iCurrentMotionState = 5;
				pAVT->SetMotion( pAVT->m_iMotionIDX[pAVT->m_iCurrentMotionState], 0);
			}
			else
			{
				pAVT->SwitchState( CJustModelAVT::STATE_STANDUP );
				pAVT->m_iCurrentMotionState = 3;
				pAVT->SetMotion( pAVT->m_iMotionIDX[pAVT->m_iCurrentMotionState], 0);
			}



		}
	}
}

//----------------------------------------------------------------------------------------------------	
///
/// @brief 이벤트 오브젝트 처리 관련
///
//----------------------------------------------------------------------------------------------------	
void CRecvPACKET::Recv_gsv_ADD_EVENTOBJ()
{
	/*struct gsv_ADD_EVENTOBJ : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	char	m_cMapX;
	char	m_cMapY;
	WORD	m_wMapEventID;
	short	m_nEventSTATUS;
	} ;*/

	CObjFixedEvent* pObj = g_pObjMGR->GetEventObject( m_pRecvPacket->m_gsv_ADD_EVENTOBJ.m_wMapEventID );
	if( pObj )
	{		
		g_pObjMGR->Set_ServerObjectIndex( pObj->Get_INDEX(), m_pRecvPacket->m_gsv_ADD_EVENTOBJ.m_wObjectIDX );

		pObj->ExecEventScript( m_pRecvPacket->m_gsv_ADD_EVENTOBJ.m_nEventSTATUS, true, false, true );
	}else
	{
		assert( 0 && "Recv invalid event object" );
	}


}

void CRecvPACKET::Recv_gsv_APPRAISAL_REPLY()
{
	if( m_pRecvPacket->m_gsv_APPRAISAL_REPLY.m_btResult == RESULT_APPRAISAL_REPLY_OK )
	{
		if( g_pAVATAR )
		{
			g_pAVATAR->m_Inventory.m_ItemLIST[ m_pRecvPacket->m_gsv_APPRAISAL_REPLY.m_wInventoryIndex ].m_bIsAppraisal = 1;
			g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - CGame::GetInstance().GetAppraisalCost() );


		}
	}
	else
	{
		g_itMGR.OpenMsgBox(STR_FAIL_IDENTIFY );
	}
	CGame::GetInstance().EndAppraisal();
}


//----------------------------------------------------------------------------------------------------	
///
/// @brief HP, MP 물약 종료 패킷.
///			이패킷의 내용으로 현재 상태를 갱신하고, 또한 이전에 누적되어있던 데미지도 다 소진해야된다.
//----------------------------------------------------------------------------------------------------	
void CRecvPACKET::Recv_gsv_SET_HPnMP()
{
	short iTarget = m_pRecvPacket->m_gsv_SET_HPnMP.m_wObjectIDX;
	CObjCHAR *pChar = g_pObjMGR->Get_ClientCharOBJ( iTarget, true );

	if( pChar && g_pAVATAR )
	{
		short client_obj_index = g_pObjMGR->Get_ClientObjectIndex ( m_pRecvPacket->m_gsv_SET_HPnMP.m_wObjectIDX );

		if( m_pRecvPacket->m_gsv_SET_HPnMP.m_nHP >= 0 )
		{
			/// 모든 데미지 클리어..
			pChar->ClearAllDamage();
				///자기 자신일경우에만 Revise HP(서서히 보정)를 적용한다.
				if( client_obj_index == g_pAVATAR->Get_INDEX() )
					pChar->SetReviseHP( m_pRecvPacket->m_gsv_SET_HPnMP.m_nHP - pChar->Get_HP() );
				else
					pChar->Set_HP( m_pRecvPacket->m_gsv_SET_HPnMP.m_nHP );
		}

		if( m_pRecvPacket->m_gsv_SET_HPnMP.m_nMP >= 0 )
		{
			//06. 02. 09 - 김주현 : 물약 종료 됬을 경우 MP 보정을 사용안한다.
			
				///자기 자신일경우에만 Revise MP(서서히 보정)를 적용한다.
				if( client_obj_index == g_pAVATAR->Get_INDEX() )
					pChar->SetReviseMP( m_pRecvPacket->m_gsv_SET_HPnMP.m_nMP - pChar->Get_MP() );
				else
					pChar->Set_MP( m_pRecvPacket->m_gsv_SET_HPnMP.m_nMP );
			
		}
	}	
}
//----------------------------------------------------------------------------------------------------	
///
/// @brief 서버에서 1초마다 MP랑 HP를 보내준다.
///			
//----------------------------------------------------------------------------------------------------	
void CRecvPACKET::Recv_gsv_CHAR_HPMP_INFO()
{ 
#ifdef _NoRecover
	if( g_pAVATAR )
	{
		if(m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_CurHP > 0)
		{
			if(m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_CurHP >= g_pAVATAR->Get_MaxHP())
				g_pAVATAR->Set_HP(g_pAVATAR->Get_MaxHP());
			else 
			{
				g_pAVATAR->SetReviseHP( m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_CurHP - g_pAVATAR->Get_HP());
			}
		}
		if(m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_CurMP > 0)
		{
			if(m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_CurMP >= g_pAVATAR->Get_MaxMP())
				g_pAVATAR->Set_MP(g_pAVATAR->Get_MaxMP());
			else 
			{
				g_pAVATAR->SetReviseMP( m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_CurMP - g_pAVATAR->Get_MP());
/*
#ifdef _DEBUG
				char Temp[64];
				sprintf(Temp, "회복되야하는 MP 수치 : %d", m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_CurMP - g_pAVATAR->Get_MP());
				g_itMGR.AppendChatMsg(Temp, IT_MGR::CHAT_TYPE_SYSTEM );
#endif
*/
			}
		}
	}
#endif
}
//----------------------------------------------------------------------------------------------------	
///
/// @brief Do_DeadEvent 에서 호출하던것을 서버에서 요청시 처리하게 변경..
///
//----------------------------------------------------------------------------------------------------	
void CRecvPACKET::Recv_gsv_CHECK_NPC_EVENT()
{	
	LOGOUT( " 서버로부터 트리서 실행 명령 받음[ %s ] ", NPC_DESC( m_pRecvPacket->m_gsv_CHECK_NPC_EVENT.m_nNpcIDX ) );
	QF_doQuestTrigger( NPC_DESC( m_pRecvPacket->m_gsv_CHECK_NPC_EVENT.m_nNpcIDX ) );
}

void CRecvPACKET::Recv_wsv_CLAN_COMMAND()
{
	switch( m_pRecvPacket->m_wsv_CLAN_COMMAND.m_btRESULT )
	{
	case RESULT_CLAN_MEMBER_JOBnLEV:
		{
			short nOffset = sizeof( wsv_CLAN_MEMBER_JOBnLEV );
			if( char* pszName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{
				CClan::GetInstance().SetMemberJob( pszName, m_pRecvPacket->m_wsv_CLAN_MEMBER_JOBnLEV.m_nJOB );
				CClan::GetInstance().SetMemberLevel( pszName, m_pRecvPacket->m_wsv_CLAN_MEMBER_JOBnLEV.m_nLEVEL );
			}
		}
		break;
	case RESULT_CLAN_CREATE_NO_CONDITION:
		g_itMGR.OpenMsgBox( STR_CLAN_RESULT_CLAN_CREATE_NO_CONDITION );
		break;
	case RESULT_CLAN_CREATE_OK:/// 창설한 유저에게 온다.
		{			
			g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - 1000000 );
		
			g_itMGR.CloseDialog( DLG_TYPE_CLAN_ORGANIZE );
			g_itMGR.OpenMsgBox(STR_CLAN_ORGANIZED );
			short nOffset = sizeof( wsv_CLAN_COMMAND );

			if( tag_MY_CLAN* pInfo = (tag_MY_CLAN*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_MY_CLAN ) ) )
			{
				if( char* pszName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				{
					CClan& Clan = CClan::GetInstance();

					Clan.SetName( pszName );
					Clan.SetClanNo( pInfo->m_dwClanID );
					Clan.SetMarkBack( pInfo->m_wClanMARK[0] );
					Clan.SetMarkCenter( pInfo->m_wClanMARK[1] );
					Clan.SetPoint( pInfo->m_iClanSCORE );
					Clan.SetClass( pInfo->m_btClanPOS );
					Clan.SetMoney( pInfo->m_biClanMONEY );
					Clan.SetStoreRate( pInfo->m_iClanRATE );
					Clan.SetLevel( pInfo->m_btClanLEVEL );
					Clan.SetSkill( pInfo->m_ClanBIN.m_SKILL, MAX_CLAN_SKILL_SLOT );

#ifdef __EDIT_CLANDLG_CLANWAR
					Clan.SetRewardPoint(pInfo->m_iRewardPoint);
#else 
					Clan.SetRewardPoint(0);
#endif
#ifdef __CLAN_INFO_ADD
					Clan.SetRankPoint(pInfo->m_iRankPoint);
#endif
					if( char* pszSlogan = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						Clan.SetSlogan( pszSlogan );

					Clan.AddMember( g_pAVATAR->Get_NAME(), CClan::CLAN_MASTER , pInfo->m_iClanCONT, CGame::GetInstance().GetJoinChannelNo() , g_pAVATAR->Get_LEVEL(), g_pAVATAR->Get_JOB() );

					g_pAVATAR->SetClan( pInfo->m_dwClanID, pInfo->m_wClanMARK[0], pInfo->m_wClanMARK[1], pszName , 1, CClan::CLAN_MASTER );

					Clan.SetMemberJob( g_pAVATAR->Get_NAME(), g_pAVATAR->Get_JOB() );
					Clan.SetMemberLevel( g_pAVATAR->Get_NAME(), g_pAVATAR->Get_LEVEL() );

					g_pNet->Send_cli_CLAN_MEMBER_JOBnLEV(  g_pAVATAR->Get_LEVEL() , g_pAVATAR->Get_JOB());
				}
			}
		}
		break;
	case RESULT_CLAN_JOIN_OK:///클랜에 새로 가입된 유저에게만 온다.	
		{
			g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_JOIN_OK);
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			if( tag_MY_CLAN* pInfo = (tag_MY_CLAN*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_MY_CLAN ) ) )
			{
				if( char* pszName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				{
					CClan& Clan = CClan::GetInstance();

					Clan.SetName( pszName );
					Clan.SetClanNo( pInfo->m_dwClanID );
					Clan.SetMarkBack( pInfo->m_wClanMARK[0] );
					Clan.SetMarkCenter( pInfo->m_wClanMARK[1] );
					Clan.SetPoint( pInfo->m_iClanSCORE );
					Clan.SetClass( pInfo->m_btClanPOS );
					Clan.SetMoney( pInfo->m_biClanMONEY );
					Clan.SetStoreRate( pInfo->m_iClanRATE );
					Clan.SetLevel( pInfo->m_btClanLEVEL );
					Clan.SetSkill( pInfo->m_ClanBIN.m_SKILL, MAX_CLAN_SKILL_SLOT );
#ifdef __EDIT_CLANDLG_CLANWAR
					Clan.SetRewardPoint(pInfo->m_iRewardPoint);
#else 
					Clan.SetRewardPoint(0);
#endif

#ifdef __CLAN_INFO_ADD
					Clan.SetRankPoint(pInfo->m_iRankPoint);
#endif
					if( char* pszSlogan = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						Clan.SetSlogan( pszSlogan );

					if( char* pszNotice = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						Clan.SetNotice( pszNotice );

					g_pAVATAR->SetClan( pInfo->m_dwClanID, pInfo->m_wClanMARK[0], pInfo->m_wClanMARK[1], pszName , pInfo->m_btClanLEVEL, CClan::CLAN_JUNIOR );

					Clan.RequestMemberList();
					//Clan.RequestClanInfo();

					g_pNet->Send_cli_CLAN_MEMBER_JOBnLEV(  g_pAVATAR->Get_LEVEL() , g_pAVATAR->Get_JOB());
				}
			}
		}
		break;
	case RESULT_CLAN_DESTROY_OK://안쓰이는 패킷
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_DESTROY_OK);
		break;
	case RESULT_CLAN_CREATE_FAILED://		
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_CREATE_FAILED );
		break;
	case RESULT_CLAN_CREATE_DUP_NAME://		
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_CREATE_DUP_NAME);
		break;
	case RESULT_CLAN_CREATE_NO_RIGHT://			
		g_itMGR.CloseDialog( DLG_TYPE_CLAN_ORGANIZE );
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_CREATE_NO_RIGHT);
		break;
	case RESULT_CLAN_DESTROYED://			0x051		// 클랜 뽀개 졌다..
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_DESTROYED);
		CClan::GetInstance().Clear();
		if( g_pAVATAR )
			g_pAVATAR->ResetClan();
		break;
	case RESULT_CLAN_DESTROY_FAILED://		
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_DESTROY_FAILED);
		break;
	case RESULT_CLAN_DESTROY_NO_RIGHT://			// 뽀갤 권한 없다.
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_DESTROY_NO_RIGHT);
		break;
	case RESULT_CLAN_JOIN_MEMBER:/// 새로 가입된 클랜원이 있을때 온다.
		{
			/*strujct {
			char *m_szMember;	신규 가입자
			char *m_szMaster;	가입시킨자
			*/
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszMember = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			char* pszMaster = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			assert( pszMember && pszMaster );
			if( pszMember && pszMaster )
			{
				if( _strcmpi( g_pAVATAR->Get_NAME(), pszMember ) )///내가 아닐 경우에만 보여주자.
				{
					g_itMGR.OpenMsgBox( CStr::Printf(STR_CLAN_RESULT_CLAN_JOIN, pszMember, pszMaster));

					CObjCHAR* pChar = g_pObjMGR->Get_ClientCharOBJByName( pszMember );
					if( pChar && pChar->Is_AVATAR() )
					{
						CObjAVT* pAvt = (CObjAVT*)pChar;				
						pAvt->SetClan( g_pAVATAR->GetClanID(),
							g_pAVATAR->GetClanMarkBack(),
							g_pAVATAR->GetClanMarkCenter(),
							g_pAVATAR->GetClanName() ,
							g_pAVATAR->GetClanLevel() , CClan::CLAN_JUNIOR );
					}
					CClan::GetInstance().AddMember( pszMember, CClan::CLAN_JUNIOR , 0, 0 ,0, 0);
				}
			}
		}
		break;
	case RESULT_CLAN_JOIN_FAILED:			
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_JOIN_FAILED);
		break;
	case RESULT_CLAN_JOIN_NO_RIGHT://				// 가입 시킬 권한이 없다.
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_JOIN_NO_RIGHT);
		break;
	case RESULT_CLAN_JOIN_HAS_CLAN://				// 다른 길드에 가입해 있다.
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_JOIN_HAS_CLAN);
		break;
	case RESULT_CLAN_MY_DATA:///처음 로그인시 클랜정보
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			if( tag_MY_CLAN* pInfo = (tag_MY_CLAN*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_MY_CLAN ) ) )
			{
				if( char* pszName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				{
					CClan& Clan = CClan::GetInstance();
					Clan.SetName( pszName );
					Clan.SetClanNo( pInfo->m_dwClanID );
					Clan.SetMarkBack( pInfo->m_wClanMARK[0] );
					Clan.SetMarkCenter( pInfo->m_wClanMARK[1] );
					Clan.SetPoint( pInfo->m_iClanSCORE );
					Clan.SetClass( pInfo->m_btClanPOS );
					Clan.SetLevel( pInfo->m_btClanLEVEL );
					Clan.SetMoney( pInfo->m_biClanMONEY );
					Clan.SetStoreRate( pInfo->m_iClanRATE );
					Clan.SetSkill( pInfo->m_ClanBIN.m_SKILL, MAX_CLAN_SKILL_SLOT );
#ifdef __EDIT_CLANDLG_CLANWAR
					Clan.SetRewardPoint(pInfo->m_iRewardPoint);
#else 
					Clan.SetRewardPoint(0);
#endif

#ifdef __CLAN_INFO_ADD
					Clan.SetRankPoint(pInfo->m_iRankPoint);
#endif
					if( char* pszSlogan = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						Clan.SetSlogan( pszSlogan );

					if( char* pszNotice = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						Clan.SetNotice( pszNotice );

					if( g_pAVATAR )///혹시 아바타가 만들어진 후에 올수 있다.
					{
						g_pAVATAR->SetClan( pInfo->m_dwClanID, pInfo->m_wClanMARK[0], pInfo->m_wClanMARK[1], pszName , pInfo->m_btClanLEVEL , pInfo->m_btClanPOS );
					}

				}
			}
		}
		break;
	case RESULT_CLAN_INFO://				0x031		// 길드 정보 /// 2004 / 11/ 23일 삭제
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			tag_MY_CLAN* pInfo = (tag_MY_CLAN*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_MY_CLAN) );

			assert( pInfo );
			if( pInfo )
			{
				CClan& Clan = CClan::GetInstance();

				if( Clan.GetLevel() < pInfo->m_btClanLEVEL )
					g_itMGR.OpenMsgBox( STR_CLAN_LEVELUP,CMsgBox::BT_OK, false );

				Clan.SetClanNo( pInfo->m_dwClanID );
				Clan.SetMarkBack( pInfo->m_wClanMARK[0] );
				Clan.SetMarkCenter( pInfo->m_wClanMARK[1] );
				Clan.SetPoint( pInfo->m_iClanSCORE );
				//Clan.SetClass( pInfo->m_btClanPOS );
				Clan.SetLevel( pInfo->m_btClanLEVEL );
				Clan.SetMoney( pInfo->m_biClanMONEY );
				Clan.SetStoreRate( pInfo->m_iClanRATE );
				Clan.SetSkill( pInfo->m_ClanBIN.m_SKILL, MAX_CLAN_SKILL_SLOT );

#ifdef __EDIT_CLANDLG_CLANWAR
				Clan.SetRewardPoint(pInfo->m_iRewardPoint);
#else 
				Clan.SetRewardPoint(0);
#endif

#ifdef __CLAN_INFO_ADD
				Clan.SetRankPoint(pInfo->m_iRankPoint);
#endif
			}
		}
		break;
	case RESULT_CLAN_ROSTER://				0x032		// 길드 리스트
		{
			CClan& Clan = CClan::GetInstance();
			Clan.ClearMemberlist();

			short nOffset = sizeof( wsv_CLAN_COMMAND );
			tag_CLAN_MEMBER* pMember = NULL;
			char*  pszCharName	= NULL;


			//CStringManager& StringMgr = CStringManager::GetSingleton();
			while( pMember = (tag_CLAN_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_CLAN_MEMBER ) ) )
			{
				if( pszCharName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
					Clan.AddMember( pszCharName, pMember->m_btClanPOS, pMember->m_iClanCONTRIBUTE, pMember->m_btChannelNO , pMember->m_nLEVEL ,pMember->m_nJOB);

			}


		}
		break;
	case RESULT_CLAN_KICK://				0x081		// 클랜에서 추방됨
		{
			//char *m_szMember;	탈퇴자
			//char *m_szMaster;	탈퇴시킨자

			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszMember = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			char* pszMaster = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );

			assert( pszMember && pszMaster );
			if( pszMember && pszMaster )
			{
				if( _strcmpi( pszMember, g_pAVATAR->Get_NAME() ) )
				{
					g_itMGR.OpenMsgBox( CStr::Printf(STR_CLAN_RESULT_CLAN_KICK, pszMember, pszMaster ) );
					CClan::GetInstance().RemoveMember( pszMember );
				}
				else
				{
					g_itMGR.OpenMsgBox( CStr::Printf(STR_CLAN_RESULT_CLAN_KICK_ME, pszMaster ));
					CClan::GetInstance().Clear();
					if( g_pAVATAR )
						g_pAVATAR->ResetClan();
				}
			}
		}
		break;
	case RESULT_CLAN_QUIT://				0x082		// 소속 클랜에서 삭제됨( 클랜변수 초기화 )
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszMember = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			assert( pszMember );
			if( pszMember )
			{
				if( _strcmpi( pszMember, g_pAVATAR->Get_NAME() )== 0)			
				{
					CClan::GetInstance().Clear();
					if( g_pAVATAR )
						g_pAVATAR->ResetClan();
				}
				else
				{
					CClan::GetInstance().RemoveMember( pszMember );
					g_itMGR.OpenMsgBox( CStr::Printf(STR_CLAN_RESULT_CLAN_QUIT,  pszMember ) );
				}
			}
		}
		break;
	case GCMD_INVITE_REQ://나에게 클랜 가입을 요청했다. <요청자이름>
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszMaster = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			assert( pszMaster );
			if( pszMaster )
			{

//홍근 : 일본 빌링 수정
#ifdef __JAPAN_BILL
				if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_BATTLE) )
				{
					g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_JOIN_CLAN );

					CTCmdRejectReqJoinClan pCmdNo( pszMaster );
					pCmdNo.Exec(NULL);

					return;
				}        
#endif

				//MSGTYPE_RECV_CLANJOIN_REQ
				if( g_itMGR.FindMsgBox( CMsgBox::MSGTYPE_RECV_CLANJOIN_REQ ) == NULL )
				{
					CTCommand* pCmdYes  = new CTCmdAcceptReqJoinClan( pszMaster );
					CTCommand* pCmdNo	= new CTCmdRejectReqJoinClan( pszMaster );
					g_itMGR.OpenMsgBox(
						CStr::Printf( STR_CLAN_GCMD_INVITE_REQ, pszMaster ),
						CMsgBox::BT_OK | CMsgBox::BT_CANCEL, false, 0,
						pCmdYes, pCmdNo ,CMsgBox::MSGTYPE_RECV_CLANJOIN_REQ);
				}
			}
		}
		break;
		//#define	GCMD_INVITE_REPLY_YES	0x00c		//클랜가입 승낙 <요청자이름>
	case GCMD_INVITE_REPLY_NO://	0x00d		//클랜가입 거절 <요청자이름>
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszTargetName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );

			g_itMGR.OpenMsgBox( CStr::Printf(STR_CLAN_GCMD_INVITE_REPLY_NO, pszTargetName ) );
		}
		break;
	case RESULT_CLAN_MOTD:
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			CClan::GetInstance().SetNotice( (const char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) );
		}
		break;
	case RESULT_CLAN_SLOGAN:
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			CClan::GetInstance().SetSlogan( (const char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) );
		}
		break;
	case RESULT_CLAN_POSITION://			0x083		// 클랜에서의 나의 등급이 조정되었을때 
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszMaster = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			BYTE* pbtClass  = (BYTE*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( BYTE ) );
			CClan::GetInstance().SetClass( *pbtClass );
			if( g_pAVATAR && g_pAVATAR->GetClanMarkBack() == 0 && *pbtClass > CClan::CLAN_MASTER )
				g_pNet->Send_cli_CLANMARK_REG_TIME();


			if( pszMaster )
			{
				std::string strMsg = CStr::Printf(STR_CLAN_RESULT_CLAN_POSITION,CStringManager::GetSingleton().GetClanClass( *pbtClass ) );
				strMsg.append(":");
				strMsg.append( pszMaster );

				g_itMGR.OpenMsgBox( strMsg.c_str() ); 
			}
		}
		break;
	case RESULT_CLAN_SET:
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			WORD* pwObjIDX			= (WORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( WORD ));
			tag_CLAN_ID* pClanID	= (tag_CLAN_ID*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_CLAN_ID) );
			assert( pwObjIDX && pClanID );
			if( pwObjIDX && pClanID )
			{
				CObjAVT* pAvt = g_pObjMGR->Get_ClientCharAVT ( *pwObjIDX, false );
				if( pAvt )
				{
					if( pClanID->m_dwClanID == 0 )
					{
						pAvt->ResetClan();		
					}
					else
					{
						if( char* pszClanName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						{
							if( pClanID->m_wClanMARK[0] == 0 && pAvt->GetClanMarkCenter() != pClanID->m_wClanMARK[1] )
								g_pNet->Send_cli_CLANMARK_REQ( pClanID->m_dwClanID );

							pAvt->SetClan( pClanID->m_dwClanID, pClanID->m_wClanMARK[0], pClanID->m_wClanMARK[1], pszClanName , pClanID->m_btClanLEVEL , pClanID->m_btClanPOS );
						}
					}
				}
			}
		}
		break;
	case RESULT_CLAN_MEMBER_LOGIN:// 전체에게 보내지는 멤버 로그인
	case RESULT_CLAN_MEMBER_LOGOUT:// 전체에게 보내지는 멤버 로그아웃
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			if( tag_CLAN_MEMBER* pMember = (tag_CLAN_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_CLAN_MEMBER ) ) )
			{
				if( char* pszCharName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
					CClan::GetInstance().ChangeConnectStatus( pszCharName, pMember->m_btChannelNO );
			}
		}
		break;
	case RESULT_CLAN_MEMBER_POSITION://		0x075		// 전체에게 보내지는 멤버 직위조절
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			if( tag_CLAN_MEMBER* pMember = (tag_CLAN_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_CLAN_MEMBER ) ) )
			{
				if( char* pszCharName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
					CClan::GetInstance().ChangeClass(pszCharName, pMember->m_btClanPOS );
			}
		}
		break;
	default:
		break;
	}
}

void CRecvPACKET::Recv_wsv_CLAN_CHAT()
{
	short nOffset = sizeof( gsv_SHOUT );
	char *szAccount, *szMsg;
	szAccount = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	szMsg     = Packet_GetStringPtr( m_pRecvPacket, nOffset);

	g_itMGR.AppendChatMsg( CStr::Printf("[%s]%s>%s","CLAN", szAccount, szMsg), IT_MGR::CHAT_TYPE_CLAN);
}

/// 서버로 부터 전송받은 클랜 마크 저장..
void CRecvPACKET::Recv_wsv_CLANMARK_REPLY()
{
	int iClanID = (int)( m_pRecvPacket->m_wsv_CLANMARK_REPLY.m_dwClanID );
	if( iClanID )//성공및 변경되었음
	{
		WORD wCRC16 = m_pRecvPacket->m_wsv_CLANMARK_REPLY.m_wMarkCRC16;	

		int iSize = m_pRecvPacket->m_wsv_CLANMARK_REPLY.m_nSize - sizeof( wsv_CLANMARK_REPLY );	
		short nOffset = sizeof( wsv_CLANMARK_REPLY );
		BYTE* pDATA = (BYTE*)Packet_GetStringPtr( m_pRecvPacket, nOffset );

		///화일로 저장
		CClanMarkTransfer::GetSingleton().ReceiveMarkFromServer( iClanID, wCRC16, pDATA, iSize );

		///현재 등록되어서 사용중인 클랜마크가 변경되었을경우 Texture를 변경한다.
		///사용중이지 않다면 아무 동작도 하지 않는다.

		std::string file_name;
		CClanMarkUserDefined::GetClanMarkFileName( CGame::GetInstance().GetSelectedServerID(), iClanID, file_name );

		CClanMarkManager::GetSingleton().ReloadTexture( file_name.c_str(), wCRC16 );

		if( g_pAVATAR && g_pAVATAR->GetClanID() == iClanID && g_pAVATAR->GetClanPos() >= CClan::CLAN_MASTER )
		{
			///현재 2번 내려온다.
			if( g_pAVATAR->GetClanMarkCenter() != wCRC16 )
			{
				g_pNet->Send_cli_CLANMARK_REG_TIME();
				g_itMGR.OpenMsgBox( STR_CLANMARK_REGISTER_SUCCESS );
			}
		}

		/// 현재 해당 클랜원들(나포함)의 모든 클랜마크정보를 갱신해주어야 한다.
		/// 클랜마크가 새로 등록된 경우에는 CNameBox에서 새로 CClanMarkUserDefined를 찾을것이고
		/// 등록되어 있던 마크가 변경시에는 위에서 Texture가 바뀌므로 별도로 해줄 작업이 없다.
		g_pObjMGR->ResetClanMarkInfo( iClanID, wCRC16 );
		//g_pAVATAR->SetClanMark( 0, wCRC16 );
	}
	else//실패
	{
		switch( m_pRecvPacket->m_wsv_CLANMARK_REPLY.m_wFailedReason )
		{
		case RESULT_CLANMARK_TOO_MANY_UPDATE:// 마크 갱신은 일정시간 후에 해야 한다...
			g_itMGR.OpenMsgBox( STR_CLANMARK_UPDATE_ERROR );
			break;
		case RESULT_CLANMARK_DB_ERROR:// 디비 갱신 오류
			g_itMGR.OpenMsgBox("Register ClanMark Error:DB");
			break;
		case RESULT_CLANMAKR_SP_ERROR://			0x0003	// 디비 SP 오류
			g_itMGR.OpenMsgBox("Register ClanMark Error:SP");
			break;
		default:
			g_itMGR.OpenMsgBox("Register ClanMark Error");
			break;
		}
	}
}

void CRecvPACKET::Recv_gsv_ALLIED_CHAT()
{

	short nOffset = sizeof( gsv_ALLIED_CHAT );

	CObjAVT *pCHAR = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_ALLIED_CHAT.m_wObjectIDX, false );
	if ( pCHAR && g_pAVATAR->Get_TeamNO() == m_pRecvPacket->m_gsv_ALLIED_CHAT.m_iTeamNO ) 
	{
		char* pszMsg = Packet_GetStringPtr( m_pRecvPacket, nOffset);
		g_itMGR.AppendChatMsg( CStr::Printf("[%s]%s>%s", STR_ALLIED, pCHAR->Get_NAME(), pszMsg), IT_MGR::CHAT_TYPE_ALLIED );
	}
}

void CRecvPACKET::Recv_gsv_ALLIED_SHOUT()
{
	short nOffset = sizeof( gsv_ALLIED_SHOUT );
	char* pszAccount = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	char* szMsg		 = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	if ( g_pAVATAR && pszAccount && szMsg && g_pAVATAR->Get_TeamNO() == m_pRecvPacket->m_gsv_ALLIED_SHOUT.m_iTeamNO ) 
		g_itMGR.AppendChatMsg ( CStr::Printf("[%s]%s>%s", STR_ALLIED, pszAccount, szMsg), IT_MGR::CHAT_TYPE_ALLIED );
}

void CRecvPACKET::Recv_wsv_CLANMARK_REG_TIME()
{
	SYSTEMTIME clanmark_regtime;

	clanmark_regtime.wYear  = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_wYear;
	clanmark_regtime.wMonth = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_btMon;
	clanmark_regtime.wDay   = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_btDay;
	clanmark_regtime.wHour  = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_btHour;
	clanmark_regtime.wMinute = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_btMin;
	clanmark_regtime.wSecond = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_btSec;

	CClan::GetInstance().SetClanMarkRegTime( clanmark_regtime );
}


//=======================================================================================
//
//  Begin - 클랜전 관련
//
//=======================================================================================
#ifdef __CLAN_WAR_SET

//---------------------------------------------------------------------------------------
//  Name  : Recv_gsv_CLANWAR_JOIN_REQ()
//  Desc  : 클랜전 입장 요청
//  Author: Antonio - 2006-07-24  오후 12:29:53
//---------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CLANWAR_JOIN_REQ()
{
	char psz[_MAX_PATH];
	sprintf( psz, STR_CLANWAR_JOIN_REQ );

	gsv_cli_CLAN_WAR_OK *p = &m_pRecvPacket->m_gsv_cli_ClanWar_OK;
	CTCmdClanWarJoinAck *pCmdOk		= new CTCmdClanWarJoinAck( p, true  );
	CTCmdClanWarJoinAck *pCmdCancel	= new CTCmdClanWarJoinAck( p, false );
	short btType = CMsgBox::BT_OK | CMsgBox::BT_CANCEL;

	//200700503 홍근 : 모달로 바꿈.
	g_itMGR.OpenMsgBox( psz,
						btType,
						true,
						0,
						pCmdOk, pCmdCancel,
						CMsgBox::MSGTYPE_RECV_CLANWAR_JOIN_REQ,
						"MsgBox",
						true,
						CTCmdClanWarJoinAck::time,
						CMsgBox::CHECKCOMMAND_NONE );
}

//---------------------------------------------------------------------------------------
//  Name  : Recv_gsv_CLANWAR_TIME()
//  Desc  : 클랜전 관련 남은 시간 처리
//  Author: Antonio - 2006-07-27  오후 6:19:06
//---------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CLANWAR_TIME()
{
	gsv_cli_CLAN_WAR_TIME *p = &m_pRecvPacket->m_gsv_cli_ClanWar_Time;
	SetSpriteTime( p->m_Play_Count );
}

//---------------------------------------------------------------------------------------
//  Name  : Recv_gsv_CLANWAR_ERR()
//  Desc  : 클랜전 관련 오류 처리
//  Author: Antonio - 2006-07-27  오후 6:19:06
//---------------------------------------------------------------------------------------
//클랜전 메세지 수정.
void CRecvPACKET::Recv_gsv_CLANWAR_ERR()
{
	char psz[_MAX_PATH] = { '\0' };

	gsv_cli_CLAN_WAR_ERR *p = &m_pRecvPacket->m_gsv_cli_ClanWar_Err;

	switch ( p->m_CW_Err_w )
	{
	case CWAR_OPPONENT_MASTER_ALREADY_GIVEUP:	// 상대 클랜장이 이미 포기했을 때
		{
			if ( CTCmdClanWarJoinAck::isPresent )
			{
				CTCmdClanWarJoinAck::err = CWAR_OPPONENT_MASTER_ALREADY_GIVEUP;
			}
			else
			{
				//[클랜전] 상대방 클랜 마스터가 기권하였습니다.
				sprintf( psz, STR_CLANWAR_ABSTENTION );
				g_itMGR.OpenMsgBox( psz );
			}
			break;
		}
	case CWAR_QUEST_LIST_FULL:	// 클랜전 신청시 퀘스트 목록이 가득 찼을 때
		{
			if ( CTCmdClanWarJoinAck::isPresent )
			{
				CTCmdClanWarJoinAck::err = CWAR_QUEST_LIST_FULL;
			}
			else
			{
				//[클랜전]보유 퀘스트가 10개 이상이므로 클랜전에 참가하실 수 없습니다.
				sprintf( psz, STR_CLANWAR_QUEST_LIST_FULL );
				g_itMGR.OpenMsgBox( psz );
			}
			break;
		}

	case CWAR_CLAN_MASTER_LEVEL_RESTRICTED:	// 클랜전 신성시 클랜장의 Level 제한으로 신청 불가할 때
		{
			if ( CTCmdClanWarJoinAck::isPresent )
			{
				CTCmdClanWarJoinAck::err = CWAR_CLAN_MASTER_LEVEL_RESTRICTED;
			}
			else
			{
				//[클랜전] 선택하신 금액은 참가하실 수 없는 레벨입니다.
				sprintf( psz, STR_CLANWAR_OFFER_MISLEVEL );
				g_itMGR.OpenMsgBox( psz );
			}
			break;
		}
#if 0
	case 4:	// 클랜전 Level 제한으로 입장 불가할 때
		{
			//[클랜전] 클랜 마스터가 신청한 클랜전에 참가하실 수 없는 레벨입니다.
			sprintf( psz, STR_CLANWAR_ENTER_MISLEVEL );
			g_itMGR.OpenMsgBox( psz );
			break;
		}
#endif
	case CWAR_TOO_MANY_CWAR_PROGRESS:       
		{            
			//  클랜전 참여인원 제한으로 입장불가 메시지.
			//[클랜전]인원을 초과하여 더 이상 클랜전에 참가하실 수 없습니다.
			sprintf( psz, LIST_STRING(953) );
			g_itMGR.OpenMsgBox( psz );
			break;
		}
	case CWAR_TIME_OUT:
		{
			//클랜전 수락 시간 초과. 클랜전 끝.			
			CMsgBox * pMsgBox = NULL;
			if( pMsgBox = g_itMGR.FindMsgBox(CMsgBox::MSGTYPE_RECV_CLANWAR_JOIN_REQ) )
			{
				pMsgBox->Hide();
			}
			if( pMsgBox = g_itMGR.FindMsgBox(CMsgBox::MSGTYPE_RECV_CLANWAR_JOIN_REQ_RECONFIRM) )
			{
				pMsgBox->Hide();
			}
		}
		break;
	case CWAR_OPPONENT_MASTER_GIVEUP:
		{
			// 974 : [클랜전] 상대방 클랜 마스터가 기권하여 클랜전에 승리하였습니다.
			sprintf( psz, LIST_STRING(974) );
			g_itMGR.OpenMsgBox( psz );
		}
		break;
	case CWAR_MASTER_GIVEUP:
		{
			// 975 : [클랜전] 클랜 마스터가 기권하여 클랜전에 패배하였습니다.
			sprintf( psz, LIST_STRING(975) );
			g_itMGR.OpenMsgBox( psz );
		}
		break;
	}
}

//---------------------------------------------------------------------------------------
//  Name  : Recv_gsv_CLANWAR_PROGRESS()
//  Desc  : 클랜전 진행 메시지 처리
//  Author: Antonio - 2006-08-21  오후 5:35:48
//---------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CLANWAR_PROGRESS()
{
	gsv_cli_CLAN_WAR_PROGRESS *p = &m_pRecvPacket->m_gsv_cli_ClanWar_Progress;
	SetClanWarProgressMsg( p->m_CW_Progress );
}


void CRecvPACKET::Recv_gsv_CLANWAR_BOSS_HP()
{
#ifdef __CLAN_WAR_BOSSDLG
	gsv_cli_CLAN_WAR_BOSS_HP * pRecvPacket = &m_pRecvPacket->m_gsv_cli_ClanWar_BOSS_HP;	

	CClanWar_BossDlg* pDLG = (CClanWar_BossDlg*)g_itMGR.FindDlg( DLG_TYPE_CLANWAR_BOSS );
	if( !pDLG )//없으면 만들어 넣기.
	{	
		pDLG = new CClanWar_BossDlg;
		pDLG->Show();
	}	
	pDLG->Set_Boss( pRecvPacket );
#endif
}

#endif
//=======================================================================================
//
//  End - 클랜전 관련
//
//=======================================================================================


void CRecvPACKET::Recv_gsv_ITEM_RESULT_REPORT()
{
	WORD wObjectIdx = m_pRecvPacket->m_gsv_ITEM_RESULT_REPORT.m_wObjectIDX;
	if( CObjAVT* pAvt = g_pObjMGR->Get_ClientCharAVT( wObjectIdx, false ) )
	{
		switch( m_pRecvPacket->m_gsv_ITEM_RESULT_REPORT.m_btREPORT )
		{
		case REPORT_ITEM_CREATE_START://		0x00	// 아이템 제조 시작
			SE_StartMake ( pAvt->Get_INDEX() );
			break;
		case REPORT_ITEM_CREATE_SUCCESS://		0x01	// 아이템 제조 성공
			SE_SuccessMake( pAvt->Get_INDEX() );
			break;
		case REPORT_ITEM_CREATE_FAILED://		0x02	// 아이템 제조 실패
			SE_FailMake( pAvt->Get_INDEX() );
			break;
		case REPORT_ITEM_UPGRADE_START://		0x03	// 아이템 제련 시작
			SE_StartUpgrade( pAvt->Get_INDEX() );
			break;
		case REPORT_ITEM_UPGRADE_SUCCESS://		0x04	// 아이템 제련 성공
			SE_SuccessUpgrade( pAvt->Get_INDEX() );
			break;
		case REPORT_ITEM_UPGRADE_FAILED://		0x05	// 아이템 제련 실패
			SE_FailUpgrade( pAvt->Get_INDEX() );
			break;
		default:
			break;
		}
	}
}

void CRecvPACKET::Recv_gsv_MALL_ITEM_REPLY()
{
	switch( m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_btReplyTYPE )
	{
	case REPLY_MALL_ITEM_CHECK_CHAR_FOUND:
		///선물할 대상을 찾았다.
		/// 저장해두었던 캐릭터명과 아이템이름을 가져와서 메세지 박스에 확인 버튼에 할당될 Command 를 생성해서 Open한다.
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DELIVERYSTORE ) )
		{

			CDeliveryStoreDlg* p = (CDeliveryStoreDlg*)pDlg;
			assert( p->get_selecteditem_slotindex() >= 1 );
			if( p->get_selecteditem_slotindex() >= 1 )
			{
				CTCommand* pCmdOk = new CTCmdGiftMallItem( (BYTE)(p->get_selecteditem_slotindex() - 1 ), p->get_receiver_name() );
				g_itMGR.OpenMsgBox( CStr::Printf("%s:%s{BR}%s:%s", STR_RECEIVER, p->get_receiver_name(), STR_SENDING_ITEM ,p->GetSelectedItemName() ), CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmdOk );
			}
		}	

		break;
	case REPLY_MALL_ITEM_CHECK_CHAR_NONE:
		g_itMGR.OpenMsgBox(STR_REPLY_MALL_ITEM_CHECK_CHAR_NONE);
		break;
	case REPLY_MALL_ITEM_CHECK_CHAR_INVALID:
		g_itMGR.OpenMsgBox(STR_REPLY_MALL_ITEM_CHECK_CHAR_INVALID);
		break;
	case REPLY_MALL_ITEM_BRING_SUCCESS:
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DELIVERYSTORE ) )
		{
			g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[0].m_btInvIDX,
				m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[0].m_ITEM );

			CDeliveryStoreDlg* p = (CDeliveryStoreDlg*)pDlg;
			p->SetItem( m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[1].m_btInvIDX,
				m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[1].m_ITEM	);///DeliveryStore에서 지운다.
		}
		break;
	case REPLY_MALL_ITEM_BRING_FAILED:
		g_itMGR.OpenMsgBox(STR_REPLY_MALL_ITEM_BRING_FAILED);
		break;
	case REPLY_MALL_ITME_GIVE_SUCCESS:
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DELIVERYSTORE ) )
		{
			CDeliveryStoreDlg* p = (CDeliveryStoreDlg*)pDlg;
			p->SetItem( m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[0].m_btInvIDX,
				m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[0].m_ITEM );

			g_itMGR.OpenMsgBox(STR_REPLY_MALL_ITME_GIVE_SUCCESS);
		}
		break;
	case REPLY_MALL_ITEM_GIVE_FAILED:
		g_itMGR.OpenMsgBox(STR_REPLY_MALL_ITEM_GIVE_FAILED);
		break;
	case REPLY_MALL_ITEM_LIST_START:
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DELIVERYSTORE ) )
		{
			CDeliveryStoreDlg* p = (CDeliveryStoreDlg*)pDlg;
			p->ClearItem();
		}
		break;
	case REPLY_MALL_ITEM_LIST_DATA:
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DELIVERYSTORE ) )
		{
			CDeliveryStoreDlg* p = (CDeliveryStoreDlg*)pDlg;

			short nOffset = sizeof( gsv_MALL_ITEM_REPLY );

			tagITEM* pItem = NULL;
			char* pszFrom	= NULL;
			char* pszDesc	= NULL;
			char* pszTo		= NULL;

			for( int count = 0; count < m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_btCntOrIdx; ++count )
			{
				pItem = (tagITEM*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tagITEM) );
				pszFrom = Packet_GetStringPtr( m_pRecvPacket, nOffset );                

				if( pszFrom && pszFrom[0] != '\0' )
				{
					pszDesc = Packet_GetStringPtr( m_pRecvPacket, nOffset );
					pszTo   = Packet_GetStringPtr( m_pRecvPacket, nOffset );
				}
								
				p->AddItem( pItem , pszFrom , pszDesc, pszTo );							

				pszFrom = NULL;
				pszDesc = NULL;
				pszTo   = NULL;
			}
		}
		break;
	case REPLY_MALL_ITEM_LIST_END:
		///?????
		break;
	default:
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//	일본쪽 빌링 처리
///////////////////////////////////////////////////////////////////////////////////////////////////////
void CRecvPACKET::Set_BILLING_MESSAGE_EXT_JPN()
{
	CGame::GetInstance().SetPayType( 0 );

	g_itMGR.AppendChatMsg( STR_JP_PAY_TYPE, IT_MGR::CHAT_TYPE_SYSTEM );				

	if(m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0])
	{
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_BATTLE )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_ENTRY, IT_MGR::CHAT_TYPE_SYSTEM );				
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_COMMUNITY )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_COMMUNITY, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_TRADE )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_TRADE, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_STOCK_SPACE )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_STOCKSPACE, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_EXTRA_STOCK )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_EXTRASTOCK, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_STARSHIP_PASS )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_STARSHIPPASS, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_DUNGEON_ADV )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_DUNGEONADVENTURE, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_EXTRA_CHAR)
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_EXTRACHARACTER, IT_MGR::CHAT_TYPE_SYSTEM );			
		}		
	}	
	else
	{
		g_itMGR.AppendChatMsg( STR_JP_PAY_FREEPLAY, IT_MGR::CHAT_TYPE_SYSTEM );
	}

	CGame::GetInstance().SetPayType( (WORD)m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] );
}


//우선 TW 넣어둔다... 아직 쓰지는 않는다.
void CRecvPACKET::Set_BILLING_MESSAGE_EXT_TW()
{
	switch(m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType)
	{
	case 0:	//금지
		break;
	case 1: //시간제, 매 1 시간 6포인트 삭감, 1시간이 채우지 못했을 경우 1시간으로 계산.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_1HOUR, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 2:	//월정액제,  30일 기준 350 포인트 삭감.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_MONTH, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 3:	//시간단위 정액제,  12시간을 기준으로 20포인트 삭감.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_12HOUR, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 4:	//3개월 정액제, 90일을 기준으로 888포인트 삭감.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_3MONTH, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 5:	//홍콩 월단위 정액제.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_HONGKONG_MONTH, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 6:	//홍콩 3개월 정액제.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_HONGKONG_3MONTH, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 7:	//홍콩 시간제
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_HONGKONG_HOUR, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 8:	//홍콩 시간단위 정액제.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_HONGKONG_12HOUR, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 9:	//분 정액제, 매 분 단위로 0.25포인트 삭감.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_MINUTE, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 10:	//2시간 단위 정액제, 매 2시간 기준으로 12포인트 삭감.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_2HOUR, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 11:	//10일 정액제, 매 10일 기준으로 150포인트 삭감.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_10DAY, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 12:	//무료 체험제
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_FREE, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 101:	//블록 3일
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_BLOCK_3DAY, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 102:	//블록7일
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_BLOCK_7DAY, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 103:	//블록10일
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_BLOCK_10DAY, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//	한국, 필리핀쪽 빌링 처리
//
//	개인정액(플레티넘)이 아닌 경우는 모두 프리미엄 입니다.
///////////////////////////////////////////////////////////////////////////////////////////////////////
void CRecvPACKET::Set_BILLING_MESSAGE_EXT_KOR_And_PH()
{
#ifdef __KOREA
	switch(m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType)
	{
	case BILLING_MSG_PAY_FU:
		g_itMGR.AppendChatMsg( STR_FREE_ACCOUNT_USER, IT_MGR::CHAT_TYPE_SYSTEM );		
		break;
	case BILLING_MSG_PAY_FA:
		if( CGame::GetInstance().GetDetailPayType() != BILLING_MSG_FREE_USER )
		{
			g_itMGR.AppendChatMsg( STR_PAID_DAYBASED_PLAN, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		else
		{
			g_itMGR.AppendChatMsg( STR_BILLING_MSG_FREE_USER, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		break;
	case BILLING_MSG_PAY_FAP:
		g_itMGR.AppendChatMsg( STR_PAID_DAYBASED_PLAN_PLATINUM, IT_MGR::CHAT_TYPE_SYSTEM );		
		break;

	case BILLING_MSG_PAY_FQ:
		g_itMGR.AppendChatMsg( STR_PAID_TIMEBASED_PLAN, IT_MGR::CHAT_TYPE_SYSTEM );		
		break;

	case BILLING_MSG_PAY_GU:
		g_itMGR.AppendChatMsg( STR_PAID_INTERNET_CAFE, IT_MGR::CHAT_TYPE_SYSTEM );		
		break;

	case BILLING_MSG_PAY_GQ:
		g_itMGR.AppendChatMsg( STR_PAID_USER_PLAN_IC_TIME_END, IT_MGR::CHAT_TYPE_SYSTEM );
		break;

	case BILLING_MSG_PAY_IQ:
		g_itMGR.AppendChatMsg( STR_PAID_USER_PLAN_IC_IP_FULL, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	}
	
#else
	switch(m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType)
	{
	case PAY_FLAG_KR_FU:
		g_itMGR.AppendChatMsg( STR_FREE_ACCOUNT_USER, IT_MGR::CHAT_TYPE_SYSTEM );			
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT | PLAY_FLAG_EXTRA_CHAR | PLAY_FLAG_EXTRA_STOCK );
		break;

	case PAY_FLAG_KR_FA:
		{
			g_itMGR.AppendChatMsg( STR_PAID_DAYBASED_PLAN, IT_MGR::CHAT_TYPE_SYSTEM );				
			CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT );
			break;
		}			

	case PAY_FLAG_KR_FAP:
		g_itMGR.AppendChatMsg( STR_PAID_DAYBASED_PLAN_PLATINUM, IT_MGR::CHAT_TYPE_SYSTEM );			
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT | PLAY_FLAG_EXTRA_CHAR | PLAY_FLAG_EXTRA_STOCK );
		break;
	case PAY_FLAG_KR_FQ:
		g_itMGR.AppendChatMsg( STR_PAID_TIMEBASED_PLAN, IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT );
		break;
	case PAY_FLAG_KR_GU:
		g_itMGR.AppendChatMsg( STR_PAID_INTERNET_CAFE, IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT );
		break;
	case PAY_FLAG_KR_GQ:
		g_itMGR.AppendChatMsg( STR_PAID_USER_PLAN_IC_TIME_END, IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT );
		break;
	case PAY_FLAG_KR_IQ:
		g_itMGR.AppendChatMsg( STR_PAID_USER_PLAN_IC_IP_FULL, IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT );
		break;		
	}	
#endif	
}	

///////////////////////////////////////////////////////////////////////////////////////////////////////
// BILLING_MESSAGE 에서 처리 해주던 부분.
///////////////////////////////////////////////////////////////////////////////////////////////////////
void CRecvPACKET::Set_BILLING_MESSAGE_EXT_Other()
{
	switch( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_wMsgTYPE )
	{
	case BILLING_MSG_KICKOUT_CHEAT_DETECT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_CHEAT_DETECT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_ALREADY_LOGIN:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_ALREADY_LOGIN ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_TIME_EXPIRED:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_TIME_EXPIRED ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_DUP_LOGIN:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_DUP_LOGIN ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_NO_RIGHT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_NO_RIGHT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_OUT_OF_IP:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_OUT_OF_IP ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_EXIST_ACCOUNT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_EXIST_ACCOUNT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_TYPE_SET_DATE:///만료 일자
		{
			if( CGame::GetInstance().GetDetailPayType() != PAY_FLAG_KR_FU )
			{
				short nOffset = sizeof( gsv_BILLING_MESSAGE_EXT );

				if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				{
					std::string strTime = pszRemainTime;
					CGame::GetInstance().SetExpireTime( pszRemainTime );
					CGame::GetInstance().SetExpireTimeType( 1 );

					if( strTime.size() >= 12 )
					{
						int year  = atoi( strTime.substr( 0, 4 ).c_str() );
						int month = atoi( strTime.substr( 4, 2 ).c_str() );
						int day   = atoi( strTime.substr( 6, 2 ).c_str() );
						int hour  = atoi( strTime.substr( 8, 2 ).c_str() );
						int min   = atoi( strTime.substr( 10, 2 ).c_str() );

						g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_EXPIRED_TIME, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
					}
				}
			}
		}
		break;
	case BILLING_MSG_TYPE_SET_TIME:///남은 분
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE_EXT );
			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{
				CGame::GetInstance().SetExpireTime( pszRemainTime );
				CGame::GetInstance().SetExpireTimeType( 2 );
				DWORD time = atoi( pszRemainTime );
				g_itMGR.AppendChatMsg( CStr::Printf(STR_GAME_TIME_LEFT_MINS, time ), IT_MGR::CHAT_TYPE_SYSTEM );
			}
		}
		break;
	case BILLING_MSG_TYPE_TIME_ALERT:///남은 분
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE_EXT );
			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{
				CGame::GetInstance().SetExpireTime( pszRemainTime );
				CGame::GetInstance().SetExpireTimeType( 2 );

				DWORD time = atoi( pszRemainTime );
				g_itMGR.AppendChatMsg( CStr::Printf( STR_GAME_TIME_LEFT_MINS, time ), IT_MGR::CHAT_TYPE_ERROR );
			}
		}
		break;
	case BILLING_MSG_TYPE_TAIWAN:
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE_EXT );
			if( char* pszMsg = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				g_itMGR.AppendChatMsg( pszMsg, IT_MGR::CHAT_TYPE_SYSTEM );
		}
		break;
	case BILLING_MSG_KICKOUT_TAIWAN:
		{
			//g_itMGR.AppendChatMsg( "Disconnect From Billing Server", IT_MGR::CHAT_TYPE_SYSTEM );
		}
		break;

	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 확장된 빌링처리.
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CRecvPACKET::Recv_gsv_BILLING_MESSAGE_EXT()
{
	///적당한 메세지 출력
	//CGame::GetInstance().SetExpireTimeType( 0 );
	//CGame::GetInstance().SetPayType( 0 );

	std::string strBuf;

	switch( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_wMsgTYPE )
	{
	case EXT_BILLING_MSG_PAY_KOREA:	//korea, Philippines.
		Set_BILLING_MESSAGE_EXT_KOR_And_PH();
		break;

	case EXT_BILLING_MSG_PAY_TAIWAN:
		Set_BILLING_MESSAGE_EXT_TW();
		break;

	case EXT_BILLING_MSG_PAY_JAPAN:		
		Set_BILLING_MESSAGE_EXT_JPN();					
		break;

	case EXT_BILLING_MSG_TYPE_SET_DATE: //만료일자

		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType==0 
			&& m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] == 0)
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE_EXT );

			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{

				//ExpireTime
				g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_JP_PAY_EXPIRE_DATE), IT_MGR::CHAT_TYPE_SYSTEM );

				char* p = strtok( pszRemainTime, "\t" );
				while( p != NULL )
				{
					//type
					std::string strType = p;

					//time
					p = strtok( NULL,"\t" );					

					std::string strTime;
					if(p)
					{
						strTime = p;
					}

					//함수 작성.

					int iType = atoi( strType.c_str() );

					CGame::GetInstance().SetExpireTimeExt( iType-1, strTime.c_str() );
					CGame::GetInstance().SetExpireTimeTypeExt( iType-1, 1 );

					if( strTime.size() >= 12 )
					{
						int year  = atoi( strTime.substr( 0, 4 ).c_str() );
						int month = atoi( strTime.substr( 4, 2 ).c_str() );
						int day   = atoi( strTime.substr( 6, 2 ).c_str() );
						int hour  = atoi( strTime.substr( 8, 2 ).c_str() );
						int min   = atoi( strTime.substr( 10, 2 ).c_str() );

						switch(iType-1)
						{
						case BILL_FLAG_JP_BATTLE:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_ENTRY, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_COMMUNITY:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_COMMUNITY, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_TRADE:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_TRADE, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_STOCK_SPACE:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_STOCKSPACE, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_EXTRA_STOCK:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_EXTRASTOCK, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_STARSHIP_PA:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_STARSHIPPASS, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_DUNGEON_ADV:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_DUNGEONADVENTURE, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_EXTRA_CHAR:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_EXTRACHARACTER, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						}											
					}
					p = strtok( NULL,"\t" );
				}
			}
		}

		break;

	case EXT_BILLING_MSG_TYPE_SET_TIME:	//남은분

		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType==0 
			&& m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] == 0)
		{
			short nOffset = sizeof( GSV_BILLING_MESSAGE_EXT );
			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{

				//남은 시간
				g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_JP_PAY_REMAIN_TIME), IT_MGR::CHAT_TYPE_SYSTEM );


				char* p = strtok( pszRemainTime, "\t" );
				while( p != NULL )
				{
					//type
					std::string strType = p;

					//time
					p = strtok( NULL,"\t" );
					std::string strTime = p;

					int iType = atoi( strType.c_str() );

					CGame::GetInstance().SetExpireTimeExt( iType-1, strTime.c_str() );					
					CGame::GetInstance().SetExpireTimeTypeExt( iType-1, 2 );

					DWORD time = atoi( strTime.c_str() );					

					switch(iType-1)
					{
					case BILL_FLAG_JP_BATTLE:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_ENTRY , strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_COMMUNITY:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_COMMUNITY, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_TRADE:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_TRADE, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_STOCK_SPACE:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_STOCKSPACE, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_EXTRA_STOCK:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_EXTRASTOCK, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_STARSHIP_PA:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_STARSHIPPASS, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_DUNGEON_ADV:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_DUNGEONADVENTURE, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_EXTRA_CHAR:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_EXTRACHARACTER, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					}	


					p = strtok( NULL,"\t" );
				}			
			}
		}

		break;

	case EXT_BILLING_MSG_TYPE_TIME_ALERT:
	case EXT_BILLING_MSG_TYPE_TIME_EXPIRED:

		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_wMsgTYPE == EXT_BILLING_MSG_TYPE_TIME_ALERT )
		{
			//Expired Alert
			strBuf = STR_JP_PAY_EXPIRE_ALERT;
		}
		else
		{
			//Expired.
			strBuf = STR_JP_PAY_EXPIRE;			
		}

		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType==0 )
		{
			//일본
			if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] )
			{                
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_BATTLE )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_ENTRY), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_COMMUNITY )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_COMMUNITY), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_TRADE )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_TRADE), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_STOCK_SPACE )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_STOCKSPACE), IT_MGR::CHAT_TYPE_SYSTEM );					
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_EXTRA_STOCK )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_EXTRASTOCK), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_STARSHIP_PASS )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_STARSHIPPASS), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_DUNGEON_ADV )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_DUNGEONADVENTURE), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_EXTRA_CHAR)
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_EXTRACHARACTER), IT_MGR::CHAT_TYPE_SYSTEM );
				}

				if(  m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_wMsgTYPE == EXT_BILLING_MSG_TYPE_TIME_EXPIRED )
				{
					DWORD dwCurPayType = CGame::GetInstance().GetPayType();

					dwCurPayType &= ~m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0];

					CGame::GetInstance().SetPayType( (WORD)dwCurPayType );
				}				
			}			
		}
		break;

	default:
		//BILLING_MSG 들도 올거 같아서 해 놨는데 안쓸것 같다.
		Set_BILLING_MESSAGE_EXT_Other();

	}	   
}

void CRecvPACKET::Recv_gsv_BILLING_MESSAGE()
{
	///적당한 메세지 출력

	CGame::GetInstance().SetExpireTimeType( 0 );

	switch( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE )
	{
		//case BILLING_MSG_FREE_USER:
		//	break;

	case BILLING_MSG_JPN_NEED_CHARGE:

		switch( m_pRecvPacket->m_gsv_BILLING_MESSAGE2.m_cFunctionTYPE )
		{
		case 'H':	//clan skill				
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_USE_CLANSKILL );
			break;

		case 'I':	//basic field		
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_MOVE_PLANET );
			break;

		case 'J':	//Friends List
			g_itMGR.OpenMsgBox( LIST_STRING(917) );	//송신처의 유저가 이용 요금을 지불하지 않아 메시지를 송신할 수 없습니다. 
			break;

		case 'P':	// space taravel
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_MOVE_PLANET2 );
			break;

		case 'Q':	// Dungeon Adventure
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_ENTER_DUNGEON );
			break;
		}

		break;

	case BILLING_MSG_PAY_FU:
		g_itMGR.AppendChatMsg( "평생무료 사용자입니다", IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_PAY_FA:
		if( CGame::GetInstance().GetDetailPayType() != BILLING_MSG_FREE_USER )
		{
			g_itMGR.AppendChatMsg( "개인정액 사용자입니다", IT_MGR::CHAT_TYPE_SYSTEM );
			CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		}
		else
		{
			g_itMGR.AppendChatMsg( "무료 사용자입니다", IT_MGR::CHAT_TYPE_SYSTEM );
			CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		}
		break;
	case BILLING_MSG_PAY_FAP:
		g_itMGR.AppendChatMsg( "개인정액(플레티넘) 사용자입니다", IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_PAY_FQ:
		g_itMGR.AppendChatMsg( "개인정량 사용자입니다",IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_PAY_GU:
		g_itMGR.AppendChatMsg( "게임방 사용자로 접속되었습니다", IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_PAY_GQ:
		g_itMGR.AppendChatMsg( "게임방의 과금기간(시간)이 만료되어 개인정량으로 접속되었습니다", IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_PAY_IQ:
		g_itMGR.AppendChatMsg( "게임방의 접속 가능 IP숫자를 넘어 개인정량으로 접속되었습니다",IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_KICKOUT_CHEAT_DETECT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_CHEAT_DETECT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_ALREADY_LOGIN:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_ALREADY_LOGIN ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_TIME_EXPIRED:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_TIME_EXPIRED ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_DUP_LOGIN:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_DUP_LOGIN ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_NO_RIGHT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_NO_RIGHT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_OUT_OF_IP:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_OUT_OF_IP ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_EXIST_ACCOUNT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_EXIST_ACCOUNT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_TYPE_SET_DATE:///만료 일자
		{
			if( CGame::GetInstance().GetDetailPayType() != BILLING_MSG_PAY_FU )
			{
				short nOffset = sizeof( gsv_BILLING_MESSAGE );

				if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				{
					std::string strTime = pszRemainTime;
					

					if( strTime.size() >= 12 )
					{
						int year  = atoi( strTime.substr( 0, 4 ).c_str() );
						int month = atoi( strTime.substr( 4, 2 ).c_str() );
						int day   = atoi( strTime.substr( 6, 2 ).c_str() );
						int hour  = atoi( strTime.substr( 8, 2 ).c_str() );
						int min   = atoi( strTime.substr( 10, 2 ).c_str() );

						//홍근 평생무료사용자.
						if(year <= 1970)
						{
							break;
						}

						g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_EXPIRED_TIME, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
					}

					CGame::GetInstance().SetExpireTime( pszRemainTime );
					CGame::GetInstance().SetExpireTimeType( 1 );
				}
			}
		}
		break;
	case BILLING_MSG_TYPE_SET_TIME:///남은 분
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE );
			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{
				CGame::GetInstance().SetExpireTime( pszRemainTime );
				CGame::GetInstance().SetExpireTimeType( 2 );
				DWORD time = atoi( pszRemainTime );
				g_itMGR.AppendChatMsg( CStr::Printf(STR_GAME_TIME_LEFT_MINS, time ), IT_MGR::CHAT_TYPE_SYSTEM );
			}
		}
		break;
	case BILLING_MSG_TYPE_TIME_ALERT:///남은 분
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE );
			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{
				CGame::GetInstance().SetExpireTime( pszRemainTime );
				CGame::GetInstance().SetExpireTimeType( 2 );

				DWORD time = atoi( pszRemainTime );
				g_itMGR.AppendChatMsg( CStr::Printf( STR_GAME_TIME_LEFT_MINS, time ), IT_MGR::CHAT_TYPE_ERROR );
			}
		}
		break;
	case BILLING_MSG_TYPE_TAIWAN:
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE );
			if( char* pszMsg = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				g_itMGR.AppendChatMsg( pszMsg, IT_MGR::CHAT_TYPE_SYSTEM );
		}
		break;
	case BILLING_MSG_KICKOUT_TAIWAN:
		{
			//g_itMGR.AppendChatMsg( "Disconnect From Billing Server", IT_MGR::CHAT_TYPE_SYSTEM );
		}
		break;
	default:
		break;
	}
}

void CRecvPACKET::Recv_gsv_MOVE_ZULY()
{
	if( g_pAVATAR )
	{
		CBank::GetInstance().SetMoney( m_pRecvPacket->m_gsv_MOVE_ZULY.m_i64BankZuly );
		g_pAVATAR->Set_MONEY( m_pRecvPacket->m_gsv_MOVE_ZULY.m_i64InvZuly );
	}	
}


//----------------------------------------------------------------------------------------------------	
///박 지호 :: 여신 소환 패킷처리 
///최 종진 :: - 능력치 변경 재계산후 저장 : 2005/7/12 : nAvy
///           - 뉴버젼에서만 이패킷이 온다고 가정 
void CRecvPACKET::Recv_gsv_GODDNESS_MODE()
{

	LOGOUT( "여신소환 명령 받음 On/Off [ %d ]",m_pRecvPacket->m_gsv_GODDNESS_MODE.m_btOnOff);
	goddessMgr.SetProcess(m_pRecvPacket->m_gsv_GODDNESS_MODE.m_btOnOff,
		m_pRecvPacket->m_gsv_GODDNESS_MODE.m_wObjectIDX);


	//short	client_object_index = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_GODDNESS_MODE.m_wObjectIDX);
	//if( client_object_index == g_pAVATAR->Get_INDEX() )
	//	g_pAVATAR->Calc_AruaAddAbility();


}

void CRecvPACKET::Recv_gsv_PATSTATE_CHANGE()
{
	_RPT1( _CRT_WARN,"m_gsv_PATSTATE_CHANGE.m_btOnOff is %d\n", m_pRecvPacket->m_gsv_PATSTATE_CHANGE.m_btOnOff );

	if( g_pAVATAR && g_pObjMGR && g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX() ) == m_pRecvPacket->m_gsv_PATSTATE_CHANGE.m_wObjectIDX )
	{
		g_pAVATAR->SetCur_PatHP( m_pRecvPacket->m_gsv_PATSTATE_CHANGE.m_nMaxPatHP );
		g_pAVATAR->SetCur_PatCoolTIME( m_pRecvPacket->m_gsv_PATSTATE_CHANGE.m_dwCoolTIME );

		///박지호::탑승스킬에 관한 타임을 설정한다. 	
		if( 0 == m_pRecvPacket->m_gsv_PATSTATE_CHANGE.m_btOnOff )
		{
			if( g_SkillList.Get_Cart_SkillIndex()>0 )
			{
				g_SoloSkillDelayTick.SetUseItemDelay(	g_SkillList.Get_Cart_SkillIndex(),
														g_SkillList.GetDelayTickCount( g_SkillList.Get_Cart_SkillIndex() ) );				
			}			
		}

	}
}


void CRecvPACKET::Recv_gsv_CHARSTATE_CHANGE()
{
	CObjAVT *pNewAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_CHARSTATE_CHANGE.m_wObjectIDX, true ); 
	if ( pNewAVT ) 
	{
		//------------------------------------------------------------------------------------
		/// 특수한 상태 플래그
		//------------------------------------------------------------------------------------
		pNewAVT->ChangeSpecialState( m_pRecvPacket->m_gsv_CHARSTATE_CHANGE.m_dwFLAG );		
	}
}

// 2인승 카트.
void CRecvPACKET::Recv_gsv_CART_RIDE()
{
//06. 04. 18 - 김주현 : 카트 초대 메시지 오버플로우 관련 버그 수정.

if( g_pAVATAR->m_bPersonalStoreMode == false)
{
	switch( m_pRecvPacket->m_gsv_CART_RIDE.m_btType )
	{
	case CART_RIDE_REQ:				// 탈래 ?	m_wGuestObjIDX에게만 전송됨
		{ 
			if( g_pAVATAR->GetCartInvitationWnd() == false )
			{
				g_pAVATAR->SetCartInvitationWnd(true);

				//Find Index Owner.
				
				int iOwnerObjClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX );
				CObjAVT* pAVT = g_pObjMGR->Get_CharAVT( iOwnerObjClientIndex, true );

				if( pAVT == NULL)
				{
					return;
				}	

				// %s님이 승차를 요구 하였습니다. 탑승 하시겠습니까?
				sprintf( g_MsgBuf, STR_WANT_BOARDS, pAVT->Get_NAME() );

				CTCmdAcceptCartRide * pCmdOk	= new CTCmdAcceptCartRide( m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX, m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX );
				CTCmdRejectCartRide * pCmdCancel	= new CTCmdRejectCartRide( m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX, m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX );

				g_itMGR.OpenMsgBox_CartRide( g_MsgBuf, CMsgBox::BT_OK | CMsgBox::BT_CANCEL, false, 0, pCmdOk, pCmdCancel , CMsgBox::MSGTYPE_RECV_CART_RIDE_REQ,
					m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX,
					m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX );
			}
		}
		break;	

	case CART_RIDE_ACCEPT:			// 탈께		주변 모든 케릭에 전송됨
		{
			g_pAVATAR->SetCartInvitationWnd(false);
			// 탑승 중이라 사용 할 수 없습니다.
			// STR_BOARDING_CANT_USE

			// 탑승할 수 없습니다.
			// STR_CANT_BOARD
			//-------------------------------------------------------------------------------------------------------------------
			//박지호::2인승 탑승 
			CObjAVT *oSrc = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX, true ); 
			if(oSrc == NULL)
				return;

			oSrc->SetRideUser(m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX);
			//-------------------------------------------------------------------------------------------------------------------

			//20050901 홍근 2인승 카트 보조석에 탄 사람만.
			int iGuestObjClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX );
			CObjAVT* pAVT = g_pObjMGR->Get_CharAVT( iGuestObjClientIndex, true );
			if( pAVT && pAVT->Get_INDEX() == g_pAVATAR->Get_INDEX() )
			{				
				g_pAVATAR->Set_Block_CartRide( true );
			}
		}

		break;

	case CART_RIDE_REFUSE:			// 안탈래	m_wOwnerObjIDX에게만 전송됨
		{
			g_pAVATAR->SetCartInvitationWnd(false);
			//Find Index Owner.
			int iGuestObjClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX );
			int iOwnerObjClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX );

			CObjAVT* pAVT = g_pObjMGR->Get_CharAVT( iGuestObjClientIndex, true );

			if( pAVT == NULL )
			{
				return;
			}	

			// Owner에게 만 메세지를 보낸다.
			if( iOwnerObjClientIndex == g_pAVATAR->Get_INDEX() )
			{
				// %s님이 승차를 거부했습니다.			
				g_itMGR.AppendChatMsg(	CStr::Printf( STR_REFUSE_BOARD, pAVT->Get_NAME() ),
					IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
			}

			//-------------------------------------------------------------------------------------------------------------------
			//박지호::탑승거부 애니메이션 
			pAVT->Ride_Cansel_Motion();	
			//-------------------------------------------------------------------------------------------------------------------
		}
		break;

	case CART_RIDE_OWNER_NOT_FOUND:// 태워 준다던 케릭이 사라졌다
		// 탑승할 수 없습니다.
		g_itMGR.AppendChatMsg(STR_CANT_BOARD, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
		break;

	case CART_RIDE_GUEST_NOT_FOUND:// 태우려던 케릭이 사라졌다
		// 탑승할 수 없습니다.
		g_itMGR.AppendChatMsg(STR_CANT_BOARD, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
		break;	
	}
}
}

void  CRecvPACKET::Recv_gsv_SCREEN_SHOT_TIME()
{
	SYSTEMTIME SysTime;
	SysTime.wYear = m_pRecvPacket->m_gsv_SCREEN_SHOT_TIME.wYear;
	SysTime.wMonth = m_pRecvPacket->m_gsv_SCREEN_SHOT_TIME.btMonth;
	SysTime.wDay = m_pRecvPacket->m_gsv_SCREEN_SHOT_TIME.btDay;
	SysTime.wHour = m_pRecvPacket->m_gsv_SCREEN_SHOT_TIME.btHour;
	SysTime.wMinute = m_pRecvPacket->m_gsv_SCREEN_SHOT_TIME.btMin;

	CGame::GetInstance().ScreenCAPTURE(SysTime);

}

void  CRecvPACKET::Recv_gsv_WARNING_MSGCODE()
{
	switch(m_pRecvPacket->m_gsv_WARNING_MSGCODE.m_dwWarningCode)
	{
	case EGSV_PARTY_EXP_LIMIT:
		{
			// 플레이어와 몬스터의 레벨 차이로 인해 경험치 획득에 실패하였습니다.
			g_itMGR.AppendChatMsg( LIST_STRING(455), IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
		}
		break;
	case EGSV_DONT_DUPLICATE_EXPERIENCE_MILEAGE_ITEM_USE:
		{
			// "중복 적용할 수 없습니다." 
			g_itMGR.AppendChatMsg( LIST_STRING(249), IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
#ifdef __EXPERIENCE_ITEM
			g_pNet->Send_cli_MILEAGE_ITEM_TIME(4); //4 - 경험치 아이템.
#endif
		}
		break;
	case EGSV_EXPERIENCE_MILEAGE_ITEM_EXPIRED:
		{
			// "경험치 아이템이 만료 되었습니다." 
			g_itMGR.AppendChatMsg( LIST_STRING(956), IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
		}
		break;
	}	
}

void CRecvPACKET::Recv_gsv_Cli_USER_MSG_1LV()
{
	switch(m_pRecvPacket->m_gsv_User_MSG_1LV.m_Sub_Type)
	{
	case 1:
		{
			g_pAVATAR->m_bDead = true;
			uniDAMAGE sDamage;
			sDamage.m_wDamage = g_pAVATAR->Get_MaxHP();
			sDamage.m_wACTION = DMG_ACT_DEAD;			
			g_pAVATAR->Apply_DAMAGE (NULL,  sDamage);	
			g_pAVATAR->m_lDeadTIME = g_GameDATA.GetGameTime();
		}
		break;
	
	case 100:
		{
#ifdef __NPC_COLLISION

			if(m_pRecvPacket->m_gsv_User_MSG_1LV.m_Data == 0)
			{
				//닫혀있는 상태
				g_pObjMGR->ClosedCollisionNPCMotion();				

#ifdef _DEBUG
				g_itMGR.AppendChatMsg( "문이 닫혀 있는 상태", IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
#endif				
			
			}
			else if(m_pRecvPacket->m_gsv_User_MSG_1LV.m_Data == 1)
			{
				//열리는중
				g_pObjMGR->OpeningCollisionNPCMotion();				

#ifdef _DEBUG
				g_itMGR.AppendChatMsg( "문이 열리는 중", IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
#endif				
			}
			else if(m_pRecvPacket->m_gsv_User_MSG_1LV.m_Data == 2)
			{
				//열려 있는 상태.
				g_pObjMGR->OpenedCollisionNPCMotion();		

#ifdef _DEBUG
				g_itMGR.AppendChatMsg( "문이 닫혀 있는 중", IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
#endif			
			}
#endif	
		}
	}
	
}
#ifdef __EXPERIENCE_ITEM
void CRecvPACKET::Recv_gsv_MALL_ITEM_TIME()
{
	DWORD dwIndex = m_pRecvPacket->m_gsv_MIELAGE_ITEM_TIME.dwIndex;

	SYSTEMTIME SysTime;
	SysTime.wYear = m_pRecvPacket->m_gsv_MIELAGE_ITEM_TIME.wYear;
	SysTime.wMonth = m_pRecvPacket->m_gsv_MIELAGE_ITEM_TIME.btMonth;
	SysTime.wDay = m_pRecvPacket->m_gsv_MIELAGE_ITEM_TIME.btDay;
	SysTime.wHour = m_pRecvPacket->m_gsv_MIELAGE_ITEM_TIME.btHour;
	SysTime.wMinute = m_pRecvPacket->m_gsv_MIELAGE_ITEM_TIME.btMin;

	if(dwIndex < 0)
	{
#ifdef _DEBUG
		g_itMGR.AppendChatMsg( CStr::Printf("Debug : Mall Item Index가 0보다 작다! dwIndex =  %d",dwIndex), IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
#endif
		return;
	}

	// 06. 12. 05 - 김주현 : 인덱스는 4부터는 + 35을 해서 계산한다.  t_AbilityINDEX 94번부터 참고.
	if(dwIndex > 3)
	{
		dwIndex += 35;
	}

	char* pszName =  CStringManager::GetSingleton().GetAbility( dwIndex +AT_MAINTAIN_ABILITY );

	if(SysTime.wYear == 0)
	{
#ifdef _DEBUG
		g_itMGR.AppendChatMsg( CStr::Printf("Debug : %s 아이템은 이미 만료된 아이템이다. 남은 시간이 0!!",pszName), IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
#endif
		return;
	}
	
	g_itMGR.AppendChatMsg( CStr::Printf(STR_ITEM_TIME_OUT, pszName, SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute ), IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136));
}

#endif



void CRecvPACKET::Recv_gsv_Server_Time()
{	
	__int64 i64ServerTime = m_pRecvPacket->m_gsv_Server_Time.m_ServerTime;
	__int64 i64ClientTime;
	_time64( &i64ClientTime );

	int iServerGap = i64ServerTime - i64ClientTime;

	struct tm tmServerLocalTime;
	tmServerLocalTime.tm_year	= m_pRecvPacket->m_gsv_Server_Time.wYear;
	tmServerLocalTime.tm_mon	= m_pRecvPacket->m_gsv_Server_Time.wMonth;
	tmServerLocalTime.tm_mday	= m_pRecvPacket->m_gsv_Server_Time.wDay;
	tmServerLocalTime.tm_hour	= m_pRecvPacket->m_gsv_Server_Time.wHour;
	tmServerLocalTime.tm_min	= m_pRecvPacket->m_gsv_Server_Time.wMin;
	tmServerLocalTime.tm_sec	= m_pRecvPacket->m_gsv_Server_Time.wSec;
		
	time_t ServerLocalTime_t = mktime( &tmServerLocalTime );

	g_GameDATA.m_dwServerTimeGap = (ServerLocalTime_t - i64ClientTime);

}

void CRecvPACKET::Recv_srv_LOGIN_REPLY2()
{
	//char sztxtAccount[36];
	//char szMD5PW[33];
	g_GameDATA.m_Account.Set(m_pRecvPacket->m_srv_LOGIN_REPLY2.sztxtAccount);
	ZeroMemory( g_GameDATA.m_PasswordMD5, sizeof(g_GameDATA.m_PasswordMD5) );
	CopyMemory( g_GameDATA.m_PasswordMD5, m_pRecvPacket->m_srv_LOGIN_REPLY2.szMD5PW, sizeof(g_GameDATA.m_PasswordMD5) );


}

void pickup(){
	WORD wFieldItem = pickupitem;
	Sleep(4000);
	g_pNet->Send_cli_GET_FIELDITEM_REQ( wFieldItem );
}

