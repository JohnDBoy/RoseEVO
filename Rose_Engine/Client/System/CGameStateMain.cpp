#include "stdafx.h"
#include "CGameStateMain.h"
#include "CGame.h"
#include "../GameCommon/item.h"
#include "../GameProc/CDayNNightProc.h"
#include "../Interface/ExternalUI/ExternalUILobby.h"
#include "../Interface/it_mgr.h"
#include "../Interface/CHelpMgr.h"
#include "../Interface/CKeyMove.h"
#include "../Interface/CUIMediator.h"
#include "../Interface/CTFontImpl.h"
#include "../Interface/Dlgs/ChattingDlg.h"
#include "../Interface/cursor/CCursor.h"
#include "../Interface/controls/EffectString.h"
#include "../Interface/ClanMarkManager.h"
#include "../Interface/CDragNDropMgr.h"

#include "../util/clipboardutil.h"
#include "../System/CGame.h"

#include "../Country.h"
#include "../Game.h"
#include "../Object.h"
#include "../Bullet.h"
#include "../CCamera.h"
#include "../Network/CNetwork.h"
#include "../IO_Terrain.h"
#include "../CSkyDOME.H"
#include "../JCommandState.h"
#include "../CViewMsg.h"
#include <crtdbg.h>
#include "../GameProc/UseItemDelay.h"
#include "../CClientStorage.h"
#include "SystemProcScript.h"
#include "../GameProc/TargetManager.h"
#include "../GameData/CParty.h"
#include "../GameData/CClan.h"
#include "TIme2.h"
#include "../GameProc/LiveCheck.h"
#include "../GameProc/DelayedExp.h"

#include "../GameProc/PreventDuplicatedCommand.h"
#include "../GameProc/SkillCommandDelay.h"
#include "../GameCommon/Skill.h"

#include "SFX/SFXManager.h"
#include "SFX/ISFX.h"
#include "SFX/SFXFont.h"
#include "TControlMgr.h"
#include "TEditBox.h"

#include "../Tutorial/TutorialEventManager.h"

#include "../GameProc/CaptureChatList.h"
#include "../Util/SystemUtil.h"


#define CAMERA_MOVE_SPEED 10


static CEffect *s_pEF=NULL;
static short s_nEffectIDX=29;
//static tPOINT16 s_PosClick;


#define		SCREEN_LEFT		0x0001
#define		SCREEN_RIGHT	0x0002
#define		SCREEN_UP		0x0004
#define		SCREEN_DOWN		0x0008
#define		MOUSE_STOP_CHECK_FRAME		5

const	int SKILLINDEX_PICKUP	= 12;
const	int	SKILLINDEX_SIT		= 11;

#define KEY_DOWN_PROC(vk_code)  ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

CGameStateMain::CGameStateMain(int iID)
{
	m_iStateID = iID;

	m_bPickedPOS=false;
	m_iPickedOBJ=0; 


	CGame::GetInstance().ResetCheckFrame(); 
//    CGame::GetInstance().m_bUseUserMouse = true;


}

CGameStateMain::~CGameStateMain(void)
{
}

int	CGameStateMain::Update( bool bLostFocus )
{

#ifdef _DEBUG
	if ( g_pCApp->IsActive() && !g_GameDATA.m_bObserverCameraMode) {
		g_KeyMove.Proc();
		CD3DSOUND::UpdateListener(g_pCamera);
	}
    
	if(g_GameDATA.m_bObserverCameraMode)
	{
		ProcOberserCamerMove();
	}
#else
	if ( g_pCApp->IsActive()) {
		g_KeyMove.Proc();
		CD3DSOUND::UpdateListener(g_pCamera);
	}
#endif



#ifdef	__VIRTUAL_SERVER
	g_pTerrain->Proc_RegenAREA ();
#endif
	g_pEffectLIST->Proc ();
	g_pBltMGR->ProcBULLET ();
	

	g_pCamera->Update ();

	g_UIMed.Update();
	g_DayNNightProc.Proc();
	g_UseItemDelay.Proc();
	g_UseSkillDelay.Proc();
	g_SoloSkillDelayTick.Proc();
	g_SoloUseItemDelayTick.Proc();


	CParty::GetInstance().Update();
	CSkillCommandDelay::GetSingleton().Proc();
	// processing  ...

	/// SFX
	CSFXManager::GetSingleton().Update();

	/// Tutorial event check..
	CTutorialEventManager::GetSingleton().Proc();

	
	if ( !bLostFocus ) 
	{
		UpdateCameraPositionByMouse();		
		UpdateCheckFrame();		
		CTargetManager::GetSingleton().Proc();
	}
	
	CLiveCheck::GetSingleton().Check();
	CDelayedExp::GetSingleton().Proc();
	CPreventDuplicatedCommand::GetSingleton().Proc();


	CClanMarkManager::GetSingleton().UpdatePool();

	::updateSceneTransform ();	// 이동 애니메이션 처리...	
	::updateSceneEx ();	// 에니메이션 처리...
	  
#ifdef __NPC_COLLISION
	g_pObjMGR->ResponseCollisionNPC(1);
#endif

	g_pObjMGR->ProcOBJECT ();

	D3DVECTOR vPos = g_pAVATAR->GetWorldPos();
	g_pTerrain->SetCenterPosition( vPos.x, vPos.y );

	::updateSceneExAfter();
	
	//박 지호 : 여신소환 업데이트 
	goddessMgr.Update();

	//조성현 2006 10/23 카메라 옵져버 추가
#ifdef _DEBUG
	if( g_GameDATA.m_bObserverCameraMode )
	{
		float xCameraPos, yCameraPos;
		GetObserverCameraPosXY(&xCameraPos, &yCameraPos);
		SetObserverCameraPosZ(g_pTerrain->GetHeight(xCameraPos, yCameraPos));
	}
#endif
	

	
	if ( !bLostFocus ) 
	{
/*		UpdateCameraPositionByMouse();		
		UpdateCheckFrame();		
		CTargetManager::GetSingleton().Proc();
		CSkillCommandDelay::GetSingleton().Proc();   */


		if ( ::beginScene() ) // 성공한 경우에만 렌더링
		{
			::clearScreen();
			::renderScene();

			if ( !g_GameDATA.m_bNoUI ) 
			{ // 인터페이스를 그려야 한다면... - zho
				DrawScreenSpaceTexts();
				Render_GameMENU ();			
			}

			::endScene ();
			::swapBuffers();
		}
	}else
	{
		Sleep( 30 );
	}

	g_pObjMGR->ClearViewObjectList();

	
	return 0;
}

int CGameStateMain::Enter( int iPrevStateID )
{
	::SetOceanSFXOnOff( true );


	CGame::GetInstance().ClearWndMsgQ();
	g_pNet->Send_cli_JOIN_ZONE( g_pAVATAR->GetWeightRate() );	

	///무게 패킷 보내주다.
	g_HelpMgr.Update();	


	g_itMGR.ChangeState( IT_MGR::STATE_NORMAL );
	if( CGame::REPAIR_NONE != CGame::GetInstance().GetRepairMode() )
		CGame::GetInstance().ResetRepairMode();


	if( CGame::MAKESOCKET_NONE != CGame::GetInstance().GetMakeSocketMode() )
		CGame::GetInstance().ResetMakeSocketMode();


	if( ISFX* p = CSFXManager::GetSingleton().FindSFXWithType( SFX_FONT ))
	{
		CSFXFont* sfx_font =(CSFXFont*)p;

		POINT draw_position;
		int   draw_width;

		///Zone Name
		SIZE  size = getFontTextExtent( g_GameDATA.m_hFONT[ FONT_OUTLINE_18_BOLD ], ZONE_NAME( g_pTerrain->GetZoneNO() ) );
		draw_width		= size.cx;
		draw_position.x = g_pCApp->GetWIDTH() / 2 - size.cx / 2;
		draw_position.y = 150;///일단 고정


		CEffectString* child = new CEffectString;
		child->SetType( CSFXFont::TYPE_ZONE_NAME );
		child->SetString( FONT_OUTLINE_18_BOLD, (char*)ZONE_NAME( g_pTerrain->GetZoneNO() ), draw_position, draw_width, 6 * 1000 );
		sfx_font->AddEffectString( child );


		///Zone Description
		draw_width      = 300;
		draw_position.x = g_pCApp->GetWIDTH() / 2 - 300 / 2;
		draw_position.y = 250;
		child = new CEffectString;
		child->SetType( CSFXFont::TYPE_ZONE_DESC );
		child->SetString( FONT_OUTLINE_14_BOLD, (char*)ZONE_DESC( g_pTerrain->GetZoneNO() ), draw_position, draw_width, 6 * 1000 );
		sfx_font->AddEffectString( child );

	}

	if( g_pTerrain->IsAgitZone() )
		g_itMGR.CloseDialog( DLG_TYPE_MINIMAP );
	else
		g_itMGR.OpenDialog( DLG_TYPE_MINIMAP , false );

	int zoneNum = g_pTerrain->GetZoneNO();
	int prevZoneNum = g_pTerrain->GetPrevZoneNO();

	if ( zoneNum >= 101 && zoneNum <= 150 && zoneNum != prevZoneNum )
	{
		SetClanWarStartMsg();
	}

	//홍근 : 워프중에 키보드 메세지 삭제. 워프중에 퀵바 사용 못하게.	
	CGame::GetInstance().ClearWindowMsgQ();

	return 0;
}

int CGameStateMain::Leave( int iNextStateID )
{

	g_pCamera->Detach ();


	::SetOceanSFXOnOff( false );

	if( ISFX* p = CSFXManager::GetSingleton().FindSFXWithType( SFX_FONT ))
	{
		CSFXFont* sfx_font =(CSFXFont*)p;
		sfx_font->RemoveEffectStringsByType( CSFXFont::TYPE_ZONE_NAME );
		sfx_font->RemoveEffectStringsByType( CSFXFont::TYPE_ZONE_DESC );
	}

	return 0;
}

void CGameStateMain::ServerDisconnected()
{
	g_itMGR.ServerDisconnected();
}

#include "../GameProc/UseItemDelay.h"


void CGameStateMain::Render_GameMENU()	
{	

	::beginSprite( D3DXSPRITE_ALPHABLEND );

	g_pViewMSG->Draw ();
	/// Screen message display
	g_UIMed.Draw();

	::endSprite();


	::drawSpriteSFX();


	::beginSprite( D3DXSPRITE_ALPHABLEND );

	g_itMGR.Update();

	/// UI display	
	//	g_pViewMSG->Draw ();

	CTargetManager::GetSingleton().Draw();

	CTIme::GetInstance().Draw();

	/// SFX
	CSFXManager::GetSingleton().Draw();
	//CTutorialEventManager::GetSingleton().Draw();

	::endSprite();	

#ifdef _DEBUG
	if( g_GameDATA.m_bDisplayDebugInfo  && !g_GameDATA.m_bObserverCameraMode)
#else
	if( g_GameDATA.m_bDisplayDebugInfo)
#endif
	{
//		::drawFontf( g_GameDATA.m_hFONT[ FONT_LARGE_BOLD ], 
//			false, 5, 120, g_dwYELLOW, "bCastingStart[ %d ], DoingSkill[ %d ], Command[ %d ]",					
//			g_pAVATAR->m_bCastingSTART, g_pAVATAR->m_nDoingSkillIDX, g_pAVATAR->Get_COMMAND() );

		//----------------------------------------------------------------------------------------------------
		/// 임시 소환몹 개수
		//----------------------------------------------------------------------------------------------------
		if( g_pAVATAR->GetCur_SummonCNT() > 0 )
		{
			::drawFontf( g_GameDATA.m_hFONT[ FONT_LARGE_BOLD ], 
				false, 5, 150, g_dwYELLOW, "[ 현재 소환된 몹 수[ %d ] , 현재 사용한 소환량[%d] ]/ 현재 소환가능한 소환량[%d]",					
				g_pAVATAR->GetCur_SummonCNT(), 
				g_pAVATAR->GetCur_SummonUsedCapacity(),
				g_pAVATAR->GetCur_SummonMaxCapacity() );
		}


		//----------------------------------------------------------------------------------------------------
		/// 스태미너
		//----------------------------------------------------------------------------------------------------
//		::drawFontf( g_GameDATA.m_hFONT[ FONT_LARGE_BOLD ], 
//			false, 5, 180, g_dwYELLOW, "현재 스태미너[ %d ] \n", g_pAVATAR->m_GrowAbility.m_nSTAMINA );

		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], false, 5, 220, g_dwYELLOW, "Skill command delay %d",
			CSkillCommandDelay::GetSingleton().GetSkillCommandDelayProgressRatio() );


		/// UseItem delay
		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], false, 5, 250, g_dwRED, 
			"HP:%d, MP:%d, Others",
			g_UseItemDelay.GetUseItemDelay( USE_ITEM_HP ), 
			g_UseItemDelay.GetUseItemDelay( USE_ITEM_MP ),
			g_UseItemDelay.GetUseItemDelay( USE_ITEM_OTHERS ) );

		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], false, 220, 25, g_dwBLUE, "%d FPS, Patch[ %d, %d ], Pos[ %f, %f, %f ]",
			g_pCApp->DisplayFrameRate (),
			g_GameDATA.m_PosPATCH.x, g_GameDATA.m_PosPATCH.y,
			g_pAVATAR->Get_CurPOS().x,	 g_pAVATAR->Get_CurPOS().y, g_pAVATAR->Get_CurPOS().z );

		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], false, 220, 40, g_dwRED, "TotOBJ: %d, CnstOBJ: %d, Item : %d, TreeOBJ: %d, MobCNT: %d, EffectCNT: %d, PatchCNT: %d ", 
			g_pObjMGR->Get_ObjectCount(),
			g_pObjMGR->Get_ObjectCount( OBJ_CNST ), 
			g_pObjMGR->Get_ObjectCount( OBJ_ITEM ), 
			g_pObjMGR->Get_ObjectCount( OBJ_GROUND ),
			g_pObjMGR->Get_ObjectCount( OBJ_MOB ),
			g_pEffectLIST->GetCount(),
			CTERRAIN::m_RegistedPatchCnt );			

			::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ],
			false,
			220, 80,
			g_dwYELLOW,
			"( 공속: %d, 이속(Get_MoveSPEED): %f, 기본속도(Get_DefaultSPEED): %f, 서버세팅속도(GetOri_RunSPEED): %d )",
			( g_pAVATAR->GetPetMode() < 0 )? g_pAVATAR->Get_nAttackSPEED() : g_pAVATAR->m_pObjCART->Get_nAttackSPEED(), 
			( g_pAVATAR->GetPetMode() < 0 )? g_pAVATAR->Get_MoveSPEED() : g_pAVATAR->m_pObjCART->Get_MoveSPEED(), 
			( g_pAVATAR->GetPetMode() < 0 )? g_pAVATAR->Get_DefaultSPEED() : g_pAVATAR->m_pObjCART->Get_DefaultSPEED(),
			( g_pAVATAR->GetPetMode() < 0 )? g_pAVATAR->GetOri_RunSPEED() : g_pAVATAR->m_pObjCART->GetOri_RunSPEED()
			);

		int y = 100;
		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], false, 10, y, g_dwYELLOW, "( 월드타입: %d, 존타임:%d, 블랜딩비율:%f )",
			g_DayNNightProc.GetWorldTime(), g_DayNNightProc.GetZoneTime(), g_DayNNightProc.GetBlendFactor() );

		y += 20;
		time_t long_time = g_GameDATA.m_dwGameStartTime;
		struct tm l;
		_getsystime( &l );

		DWORD dwTime = mktime( &l );		
		
		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], false, 10, y, g_dwYELLOW, "( 월드타임: %d, %d, %d, %d, %d, time:%d)",
			l.tm_year+1900,
			l.tm_mon+1,
			l.tm_mday,
			l.tm_hour,
			l.tm_min,
			dwTime
			);

		y += 20;
		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], false, 10, y, g_dwYELLOW, "( WorldRATE: %d, WorldPROD:%d, Country Code:%d )",
			Get_WorldRATE(), Get_WorldPROD (), CCountry::GetSingleton().GetCountryCode() );

		y += 20;
		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ],
			false,
			10, y,
			g_dwYELLOW,
			"Cal_RunSPEED: %.02f",
			g_pAVATAR->Cal_RunSPEED()
			);

	}	

}

int CGameStateMain::ProcMouseInput(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	CGame& refGame = CGame::GetInstance();

	POINT ptMouse = { LOWORD( lParam ), HIWORD( lParam ) };;

	if( !g_GameDATA.m_bNoUI && g_itMGR.MsgProc( uiMsg , wParam, lParam )) 
		return 1;

	m_ScreenPOS.m_nX = (short)ptMouse.x;
	m_ScreenPOS.m_nY = (short)ptMouse.y; 

	switch( uiMsg ) 
	{
	case WM_LBUTTONDOWN :
		{
			// set screen mouse position & world position
			Pick_POSITION ();
			On_WM_LBUTTONDOWN ( wParam, lParam);
			break;
		}

	/*case WM_RBUTTONDOWN:
		{
			On_WM_RBUTTONDOWN( wParam, lParam );
		}
		break;*/

	case WM_MOUSEWHEEL:
		On_WM_MOUSEWHEEL( wParam, lParam);
		break;

	case WM_LBUTTONDBLCLK:
		On_WM_LBUTTONDBCLICK( wParam, lParam );
		break;

	default:
		break;
	}


	return true;
}

#include "System/System_FUNC.h"
int CGameStateMain::ProcKeyboardInput(UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	int iRet = 0;
	
	D3DXVECTOR3 vPos(0.0f, 0.0f, 0.0f);

	switch( uiMsg )
	{		
		case WM_KEYUP:
		{
			unsigned int oemScan = int( lParam & (0xff << 16) ) >> 16; 
			UINT vk = MapVirtualKey( oemScan, 1 );
			switch( vk )
			{
			case VK_CONTROL:
				g_GameDATA.m_bShowDropItemInfo = false;
				break;
			case 0x5A://z : 앉기/서기 토글
				if( IsApplyNewVersion() 
					&& CTControlMgr::GetInstance()->GetKeyboardInputType() == CTControlMgr::INPUTTYPE_NORMAL 
					&& NULL == CTEditBox::s_pFocusEdit )
				{
					assert( g_pAVATAR );
					CSkillSlot* pSkillSlot = g_pAVATAR->GetSkillSlot();
					assert( pSkillSlot );

					CSkill* pSkill = pSkillSlot->GetSkillBySkillIDX( SKILLINDEX_SIT );
					assert( pSkill );
					if( pSkill )
						pSkill->Execute();
				}
				break;
#pragma message("일본어의 경우 변환키와 중복되지 않는지 테스트해보아야 한다")
			case 192://` : 줍기
				if( IsApplyNewVersion() && 
					CTControlMgr::GetInstance()->GetKeyboardInputType() == CTControlMgr::INPUTTYPE_NORMAL &&
					NULL == CTEditBox::s_pFocusEdit )
				{
					assert( g_pAVATAR );
					CSkillSlot* pSkillSlot = g_pAVATAR->GetSkillSlot();
					assert( pSkillSlot );

					CSkill* pSkill = pSkillSlot->GetSkillBySkillIDX( SKILLINDEX_PICKUP );
					assert( pSkill );
					if( pSkill )
						pSkill->Execute();

				}
				break;
				case VK_SHIFT://
				if( IsApplyNewVersion() && 
					CTControlMgr::GetInstance()->GetKeyboardInputType() == CTControlMgr::INPUTTYPE_NORMAL && 
					NULL == CTEditBox::s_pFocusEdit )
				{
					g_UserInputSystem.SetTargetSelf();
				}
			default:
				break;
			}
		}
		break;

	case WM_SYSKEYDOWN :
		{
			unsigned int oemScan = int( lParam & (0xff << 16) ) >> 16; 
			UINT vk = MapVirtualKey( oemScan, 1 );
			if( GetAsyncKeyState( VK_SHIFT ) < 0 )
			{
				switch( vk )
				{
				//case 0x4B://K
				//	{
				//		if( CTControlMgr::GetInstance()->GetKeyboardInputType() == CTControlMgr::INPUTTYPE_AUTOENTER )
				//		{
				//			it_SetKeyboardInputType( CTControlMgr::INPUTTYPE_NORMAL );
				//		}
				//		else
				//		{
				//			it_SetKeyboardInputType( CTControlMgr::INPUTTYPE_AUTOENTER );
				//			if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAT ) )
				//				pDlg->Show();
				//		}
				//	}
				//	break;
				case 0x52:///R
					if( CGame::GetInstance().GetRight() >= CHEAT_GM )
						CGame::GetInstance().ToggleAutoRun();

					break;
				default:
					break;
				}
			}
		}
		break;

	default:
		break;
	}
#ifdef _DEBUG
	if(!g_GameDATA.m_bObserverCameraMode)
	{
		if(g_itMGR.MsgProc( uiMsg , wParam, lParam)) 
			return 1;
	}
#else
	if(g_itMGR.MsgProc( uiMsg , wParam, lParam))
		return 1;
#endif

	switch( uiMsg ) 
	{

	case WM_CHAR :
		return On_WM_CHAR ( wParam, lParam);

	case WM_KEYDOWN :

#ifdef _DEBUG
		if(!g_GameDATA.m_bObserverCameraMode)
		{
			return On_WM_KEYDOWN ( wParam, lParam);	
		}
#else
		return On_WM_KEYDOWN ( wParam, lParam);	
#endif

	case WM_SYSKEYDOWN :
		{
			unsigned int oemScan = int( lParam & (0xff << 16) ) >> 16; 
			UINT vk = MapVirtualKey( oemScan, 1 );

			switch( vk )
			{					
				///'2'
				case 0x32:
				//{
					//CGame::GetInstance().ToggleUserMouseFlag();
					//CGame::GetInstance().SetUserCursor( CGame::CURSOR_DEFAULT );
				//	CCursor::GetInstance().ChangeNextState();
				//}
					///ScreenFadeInStart( 25, 0, 0, 0, 0, 0 ); 
				break;

				/// 촬영모드 전환..
				case 0x35:	// 5
					{	
						if( CGame::GetInstance().GetRight() >= CHEAT_GM )
						{
							g_GameDATA.m_bFilmingMode = !g_GameDATA.m_bFilmingMode;
							if( g_GameDATA.m_bFilmingMode )
								::setVisibilityRecursive( g_pAVATAR->GetZMODEL(), 0.0f );
							else
								::setVisibilityRecursive( g_pAVATAR->GetZMODEL(), 1.0f );
						}
					}
					break;

				/// 마우스 커서 숨기기
				case 0x36:	// 6
					{		
						if( CGame::GetInstance().GetRight() >= CHEAT_GM )
						{
							g_GameDATA.m_bShowCursor = !g_GameDATA.m_bShowCursor;
						}
					}
					break;				

				/// 'K'
				case 0x4B :
					{
						if( CGame::GetInstance().GetRight() >= CHEAT_GM )
						{
							g_DayNNightProc.SetWorldTime( g_pTerrain->GetZoneNO(), g_DayNNightProc.GetWorldTime() + 10 );
						}
					}							
					break;

				/// '9'
				case 0x39:
					{
						if( CGame::GetInstance().GetRight() >= CHEAT_GM )
						{
							int iClientObjectIndex = g_UserInputSystem.GetCurrentTarget();
							CObjCHAR * pChar = g_pObjMGR->Get_ClientCharOBJ( g_pObjMGR->Get_ServerObjectIndex( iClientObjectIndex ), false );
							if( pChar )
							{
								switch( pChar->Get_TYPE() )
								{
								case OBJ_MOB:
									g_pNet->Send_cli_CHAT( "/get tg" );
									break;
								case OBJ_AVATAR:
									{
										std::string cheat = "/get hp ";
										cheat.append( pChar->Get_NAME() );
										g_pNet->Send_cli_CHAT( (char*)cheat.c_str() );
									}
									break;
								case OBJ_USER:
									g_pNet->Send_cli_CHAT( "/get hp" );
									break;
								default:
									break;
								}
							}
							else
							{
								g_pNet->Send_cli_CHAT( "/get hp" );
							}
						}
					}
					break;

								
				/// 'j'
				case 0x4A:
					if( CGame::GetInstance().GetRight() >= CHEAT_GM )
					{
#ifdef _DEBUG
						g_GameDATA.m_bShowCurPos = !g_GameDATA.m_bShowCurPos;
#endif
					}
					break;

				/// 'w'
				case 0x57: 
				{
					if( CGame::GetInstance().GetRight() >= CHEAT_DEV )
					{
						g_GameDATA.m_bWireMode = !g_GameDATA.m_bWireMode;
						::useWireMode( g_GameDATA.m_bWireMode );
					}
					return 1;
				}	
				
				/// 'H'
				//case 0x48:
				//	if( CGame::GetInstance().GetRight() )
				//	{
				//		g_GameDATA.m_bNoUI = !g_GameDATA.m_bNoUI;
				//	}
				//	break;
				
				/// 'D'
				case 0x44:
					if( CGame::GetInstance().GetRight() )
					{
						g_GameDATA.m_bDisplayDebugInfo = !g_GameDATA.m_bDisplayDebugInfo;
					}
					break;						
				case 0x31:
					{
#ifdef _DEBUG
						if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAT ) )
						{
							CChatDLG* pChatDlg = (CChatDLG*)pDlg;
							pChatDlg->SendChatMsgRepeat();
						}
#else

						if( CGame::GetInstance().GetRight() >= CHEAT_DEV )
						{
							if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAT ) )
							{
								CChatDLG* pChatDlg = (CChatDLG*)pDlg;
								pChatDlg->SendChatMsgRepeat();
							}
						}
						
#endif
					break;
					}


#if defined( _DEBUG ) || defined( _D_RELEASE )				
					/// '7'
				case 0x37:
					///g_ClientStorage.SetUseRoughMap( !g_ClientStorage.GetUseRoughMap() );
					{
						int iFace = g_pAVATAR->GetCharExpression();
						iFace++;
						if( iFace > 6 )
							iFace = 0;

						g_pAVATAR->SetCharExpression( iFace );
						g_pAVATAR->Update();
					}
					break;

				///'8'
				case 0x38:
					{						
						if ( !s_pEF ) 
						{
							s_pEF = g_pEffectLIST->Add_EffectWithIDX ( s_nEffectIDX );
							if ( s_pEF ) {
								s_pEF->LinkNODE( g_pAVATAR->GetZMODEL() );
								s_pEF->InsertToScene ();
							}

							s_nEffectIDX = ( s_nEffectIDX+1 ) % g_pEffectLIST->GetFileCNT();
						} else 
						{
							///g_pEffectLIST->Del_EFFECT( s_pEF );
							SAFE_DELETE( s_pEF );
							s_pEF = NULL;
						}
					}
					break;		

				///'0'
				case 0x30:
					
					g_GameDATA.m_bObserverCameraMode = !g_GameDATA.m_bObserverCameraMode;
					::getCameraEye(g_pCamera->GetZHANDLE(), vPos);
					SetObserverCameraOnOff2(vPos.x, vPos.y, g_pTerrain->GetHeight(vPos.x, vPos.y));
					break;

#endif
				default:
					return false;
			}
			return true;
		}
	default:
		break;
	}
	return false;;
}


///*-------------------------------------------------------------------------------------*/
#ifdef _DEBUG
#include "../ObjFixedEvent.h"
#endif //_DEBUG
bool CGameStateMain::On_WM_KEYDOWN (WPARAM wParam, LPARAM lParam)
{
	/// 명령에 관련된 메세지 처리...
	g_UserInputSystem.OnKeyDown( wParam, lParam );

	static float s_fScale = 1.0f;   

	if ( lParam & 0x40000000 ) 
	{
		// 이전에 눌려 있던 키다....
		return false;
	}

	switch ( wParam ) 
	{
#ifdef _DEBUG
		case '8':
			{
				CObjFixedEvent* pObj = g_pObjMGR->GetEventObject( 1 );	
				if( pObj == NULL )
					return 0;
				pObj->ExecEventScript( 0 );
			}			
			break;
		case '9':
			{
				CObjFixedEvent* pObj = g_pObjMGR->GetEventObject( 1 );	
				if( pObj == NULL )
					return 0;
				pObj->ExecEventScript( 1 );
			}
#endif //_DEBUG
			break;
		/*case VK_UP:
			g_pAVATAR->m_fHeightOfGround += 10.0f;
			break;

		case VK_DOWN:
			g_pAVATAR->m_fHeightOfGround -= 10.0f;
			break;*/
	
		case VK_CONTROL:
			{
				g_GameDATA.m_bShowDropItemInfo = true;
			}
			break;

		case VK_TAB :
			{
				if( GetAsyncKeyState( VK_SHIFT ) < 0 )
				{		
					return false;
				}
				else
				{
					WORD avtst = g_pAVATAR->Get_STATE();

					if (avtst == CS_STOP || 
						avtst == CS_MOVE) {
						g_pNet->Send_cli_TOGGLE( TOGGLE_TYPE_RUN );
					}
				}
			}
			return true;

		case 'C':
		case 'c':
			{
				
				if( GetAsyncKeyState( VK_CONTROL ) < 0 )
				{
					CTEditBox * pEditBox;
                    pEditBox = g_itMGR.GetFocusEditBox();
					if(pEditBox != NULL)
					{
						char* ptext = pEditBox->get_text();
						if(ptext != NULL)
						{
							CClipboardUtil::CopyStringtoClibboard(std::string(ptext));
						}
					}
				}
			}
			return true;
		case 'V':
		case 'v':
			{
				if( GetAsyncKeyState( VK_CONTROL ) < 0 )
				{
					CTEditBox * pEditBox;
					pEditBox = g_itMGR.GetFocusEditBox();
					if(pEditBox != NULL)
					{
						std::string ptext = CClipboardUtil::GetStringFromClibboard();
						if(ptext.c_str() != NULL)
						{
							pEditBox->Insert( ptext.c_str() );
						}
					}
				}
			}
			return true;
		/*
		case 'S':
		case 's':
			{
				if( GetAsyncKeyState( VK_CONTROL ) )
				{
					CCaptureChatList captureChatList;
					captureChatList.DoCaptureChatList();
				}
			}
			break;*/

		default:
			{
				
			}
			break;		
	}
	return false;
}


bool CGameStateMain::On_WM_CHAR (WPARAM wParam, LPARAM lParam)
{
	switch ( wParam ) 
	{
		case VK_ESCAPE:
			{
				g_UserInputSystem.ClearMouseState();
			}
			break;
		case VK_RETURN :
			LogString (LOG_NORMAL, "VK_RETURN \n");

#ifdef	__USE_IME
			m_IME.ClearString();
			m_IME.SetActive ( true );
			m_IME.ToggleInputMode ( true );	// 강제로 한글로 전환 시킨다.
#endif
			return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

bool CGameStateMain::On_WM_LBUTTONDOWN (WPARAM wParam, LPARAM lParam)
{
	if ( NULL == g_pAVATAR )
		return true;

	if ( g_pAVATAR->Get_HP() <= 0 )
		return true;

	if ( !this->m_bPickedPOS )
		return true;

	
	/// 입력은 서버의 결과와는 상관없다.
	if( g_pAVATAR->bCanUserInput() )
	{
		g_UserInputSystem.ClickObject( this->m_iPickedOBJ, this->m_PosPICK, wParam  );			
	}else
	{
		g_itMGR.AppendChatMsg( STR_DOING_SKILL_ACTION, IT_MGR::CHAT_TYPE_SYSTEM );
	}
	
	CGame::GetInstance().ResetAutoRun();
	return true;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief WM_RBUTTONDOWN 
//    9/14 현재 기능제거.. (카메라 워크와 충돌 )
//----------------------------------------------------------------------------------------------------

bool CGameStateMain::On_WM_RBUTTONDOWN (WPARAM wParam, LPARAM lParam)
{
	if ( NULL == g_pAVATAR )
		return true;

	if ( g_pAVATAR->Get_HP() <= 0 )
		return true;

	if ( !this->m_bPickedPOS )
		return true;

	
	/// 입력은 서버의 결과와는 상관없다.
	if( g_pAVATAR->bCanUserInput() )
	{
		g_UserInputSystem.RButtonDown( this->m_iPickedOBJ, this->m_PosPICK, wParam  );			
	}/*else
	{
		g_itMGR.AppendChatMsg( STR_DOING_SKILL_ACTION, IT_MGR::CHAT_TYPE_SYSTEM );
	}*/
	
	CGame::GetInstance().ResetAutoRun();
	return true;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
//----------------------------------------------------------------------------------------------------

bool CGameStateMain::On_WM_LBUTTONDBCLICK (WPARAM wParam, LPARAM lParam)
{
	if ( NULL == g_pAVATAR )
		return true;

	if ( g_pAVATAR->Get_HP() <= 0 )
		return true;

	if ( !this->m_bPickedPOS )
		return true;

	
	/// 입력은 서버의 결과와는 상관없다.
	if( g_pAVATAR->bCanUserInput() )
	{
		g_UserInputSystem.DBClickObject( this->m_iPickedOBJ, this->m_PosPICK, wParam  );			
	}else
	{
		g_itMGR.AppendChatMsg( STR_DOING_SKILL_ACTION, g_dwRED );
	}	
	CGame::GetInstance().ResetAutoRun();
	return true;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

bool CGameStateMain::On_WM_MOUSEWHEEL (WPARAM wParam, LPARAM lParam)
{
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

#ifdef _DEBUG
	if( g_GameDATA.m_bObserverCameraMode )
	{
		ObserverCameraZoomInOut( (float)-(zDelta) );
	}else
#endif
	{
		g_pCamera->Add_Distance( (float)-(zDelta) );
	}
	return true;
}
//-------------------------------------------------------------------------------------------


void CGameStateMain::Pick_POSITION (/*LPARAM	lParam*/)
{
	D3DXVECTOR3 RayOrig;
	D3DXVECTOR3 RayDir;

	::getRay (m_ScreenPOS.m_nX, m_ScreenPOS.m_nY, &RayOrig.x, &RayOrig.y,	&RayOrig.z, &RayDir.x, &RayDir.y,	&RayDir.z );
//	::getRay (CGame::GetInstance().Get_XPos(),CGame::GetInstance().Get_YPos(), &RayOrig.x, &RayOrig.y,	&RayOrig.z, &RayDir.x, &RayDir.y,	&RayDir.z );

	CGame::GetInstance().SetRayDir( RayDir );
	CGame::GetInstance().SetRayOrig( RayOrig );

	D3DXVECTOR3 PosPICKTerrain;

#pragma message( " TODO : 이동시의 지형과 오브젝트 충돌 이상하다" )

	float fDistanceTerrain = g_fMaxDistance;
	float fDistanceObject = g_fMaxDistance;

	//----------------------------------------------------------------------------------------------------	
	/// @brief 먼저 나와 충돌한 오브젝트들( 밟고 있는 오브젝트 )부터 찝어낸다.
	//----------------------------------------------------------------------------------------------------
	///m_iPickedOBJ = g_pTerrain->Pick_NearObject( m_PosPICK, fDistanceObject );
	
	//----------------------------------------------------------------------------------------------------	
	/// 지형과의 충돌 체크
	//----------------------------------------------------------------------------------------------------	
	fDistanceTerrain = g_pTerrain->Pick_POSITION( PosPICKTerrain );

	//----------------------------------------------------------------------------------------------------	
	/// 나와 충돌된 놈이 찝혔다면 지형과 충돌을 검사후에 가까운놈 쪽으로 피킹판정
	//----------------------------------------------------------------------------------------------------	
    /*if( m_iPickedOBJ )
	{
		m_bPickedPOS = true;
		if( ( fDistanceTerrain > 0 ) && fDistanceObject > fDistanceTerrain )
		{
			m_PosPICK = PosPICKTerrain;
			m_iPickedOBJ = 0;
		}
	}else*/
	{
		//----------------------------------------------------------------------------------------------------	
		/// 먼저 오브젝트 충돌 검사
		//----------------------------------------------------------------------------------------------------	
		m_iPickedOBJ = g_pTerrain->Pick_OBJECT( m_PosPICK, fDistanceObject );
		//----------------------------------------------------------------------------------------------------	
		/// 오브젝트가 찝혔다면 지형과 충돌을 검사후에 가까운놈 쪽으로 피킹판정
		//----------------------------------------------------------------------------------------------------	
		if( m_iPickedOBJ )
		{
			m_bPickedPOS = true;
			/// 지형이 충돌했고, 지형과의 충돌거리가 오브젝트와의 충돌거리보다 더 가깝다면..
			if( ( fDistanceTerrain > 0 ) && ( fDistanceObject > fDistanceTerrain ) )
			{
				m_PosPICK = PosPICKTerrain;
				m_iPickedOBJ = 0;
			}
		} 
		else 
		{
			/// 오브젝트와는 충돌이 없고 지형과만 충돌이 있음.
			if( fDistanceTerrain > 0 )
			{
				m_PosPICK = PosPICKTerrain;
				m_bPickedPOS = true;
			}else
			{
				/// SKY 와의 충돌 체크
				HNODE hSky = CSkyDOME::GetHNODE();
				float fDistanceSky = g_fMaxDistance;
				const float fDefaultSkyDistance = 3000.0f;

				m_iPickedOBJ = 0;

				if ( g_pAVATAR && hSky && CGameOBJ::IsIntersect( hSky, fDistanceSky ) ) {
					m_bPickedPOS = true;
					CGameOBJ::AdjustPickPOSITION( fDefaultSkyDistance );
					m_PosPICK = CGameOBJ::Get_PickPOSITION();
				}
				else {
					m_bPickedPOS = false;
				}
			}
		}
	}
}


void CGameStateMain::UpdateCheckFrame()
{
	CGame& refGame = CGame::GetInstance();
	CCursor& refCursor = CCursor::GetInstance();

	//if( !refGame.GetUseUserMouseFlag() )		
	//	return;

	if( ( refCursor.GetCurrCursorType() == CCursor::CURSOR_DEFAULT && refGame.GetCheckFrame() >= MOUSE_STOP_CHECK_FRAME )
		|| ( refCursor.GetCurrCursorType() != CCursor::CURSOR_DEFAULT && refGame.GetCheckFrame() >= MOUSE_STOP_CHECK_FRAME/2 ) )
	{
		Pick_POSITION ( /*this->m_sCurMsg.lParam */);

		CTargetManager::GetSingleton().SetMouseTargetObject( 0 );
		
		/// 현재 마우스 커서를 갱신한다.

		// 피킹이 제대로 수행됬는지도 함께 체크.
		// this->m_iPickedOBJ 이 0이라면 유효하지 않은 건가?
		// - 2004.01.17.zho
		// - 2004.02.25.nAvy : 현재 마우스가 인터페이스에서 사용중일때는 Default
		// - 2004.07.13 nAvy : 수리모드일경우 항상 수리커서로
		if( refGame.GetRepairMode() )
		{
			refCursor.SetCursorType( CCursor::CURSOR_REPAIR );
		}
		else if( g_itMGR.GetState() == IT_MGR::STATE_APPRAISAL )
		{
			refCursor.SetCursorType( CCursor::CURSOR_APPRAISAL );
		}

		else if( g_itMGR.GetState() == IT_MGR::STATE_MAKESOCKET ) 
		{
		#if defined( _NEWINTERFACEKJH )
			refCursor.SetCursorType( CCursor::CURSOR_MAKESOCKET ); 
		#else
			refCursor.SetCursorType( CCursor::CURSOR_REPAIR );
		#endif
		}
		else if( GetAsyncKeyState( VK_RBUTTON ) < 0 )
		{
			if( !g_itMGR.IsMouseOnInterface() )
				refCursor.SetCursorType( CCursor::CURSOR_WHEEL );
			else
				refCursor.SetCursorType( CCursor::CURSOR_DEFAULT );
		}
		else
		{

			POINT ptMouse;
			refGame.Get_MousePos( ptMouse );

			int iCheckFlag = CheckMouseRegion( ptMouse.x, ptMouse.y );

			//06. 03. 09 - 김주현 : 화면 밖으로 마우스가 나가면 드래그 하던 아이템 해제
			//마우스를 순간 이동 시키면;; MOVe가 안넘어와서;; 마우스 위치 체크가 제대로 안된다;;
			//그래서 CItemDlg에서도 체크함.. 혹시나해서 이 소스도 넣어둡니당..

			RECT rect = {0, 0, g_pCApp->GetWIDTH(), g_pCApp->GetHEIGHT()};

			CGame::GetInstance().Get_MousePos( ptMouse );

			if( iCheckFlag & SCREEN_LEFT && !PtInRect(&rect,ptMouse))
			{
				CDragNDropMgr::GetInstance().DragCancel();
				refCursor.SetCursorType( CCursor::CURSOR_LEFT );
			}
			else if( iCheckFlag & SCREEN_RIGHT )
			{
				CDragNDropMgr::GetInstance().DragCancel();
				refCursor.SetCursorType( CCursor::CURSOR_RIGHT );
			}
			else if( iCheckFlag & SCREEN_DOWN)
			{
				CDragNDropMgr::GetInstance().DragCancel();
			}
			else if( iCheckFlag & SCREEN_UP )
			{
				CDragNDropMgr::GetInstance().DragCancel();
			}
			else if( g_itMGR.IsMouseOnInterface() )
				refCursor.SetCursorType( CCursor::CURSOR_DEFAULT );
			else if( m_bPickedPOS && (m_iPickedOBJ > 0) )
			{	
				CGameOBJ* pObj = g_pObjMGR->m_pOBJECTS[ this->m_iPickedOBJ ];
				if( pObj )
				{				
					switch ( pObj->Get_TYPE() ) 
					{
						case OBJ_MOB :
							if( ((CObjCHAR*)pObj)->CanClickable() )
							{
								if( (g_pAVATAR->Is_ALLIED( (CObjCHAR*)pObj ) ))
								{
									refCursor.SetCursorType( CCursor::CURSOR_USER );
								}
								else
								{
									refCursor.SetCursorType( CCursor::CURSOR_ATTACK );
									{
										CTargetManager::GetSingleton().SetMouseTargetObject( this->m_iPickedOBJ );
									}
								}
							}
							break;
						case OBJ_NPC:
							if( ((CObjCHAR*)pObj)->CanClickable() )
							{
									refCursor.SetCursorType( CCursor::CURSOR_NPC );
							}
							break;

						case OBJ_AVATAR:
							if( ((CObjCHAR*)pObj)->CanClickable() )
							{
								if( !(g_pAVATAR->Is_ALLIED( (CObjCHAR*)pObj ) ))
									refCursor.SetCursorType( CCursor::CURSOR_ATTACK);
								else
									refCursor.SetCursorType( CCursor::CURSOR_USER );
							}
							break;

						case OBJ_ITEM:
							{
								CInfo	MouseInfo;
								MouseInfo.Clear();
								CObjITEM* pItem = (CObjITEM*)g_pObjMGR->m_pOBJECTS[ this->m_iPickedOBJ ];
								MouseInfo.AddString( pItem->Get_NAME() , CItem::GetItemNameColor( pItem->m_ITEM.GetTYPE(), pItem->m_ITEM.GetItemNO() ) );
								refCursor.SetCursorType( CCursor::CURSOR_ITEM_PICK , &MouseInfo);
								break;
							}
						case OBJ_GROUND :
						case OBJ_CNST   :				
							refCursor.SetCursorType( CCursor::CURSOR_DEFAULT );
							break;
						default:
							{
								refCursor.SetCursorType( CCursor::CURSOR_DEFAULT );						
							}
							break;
					}
				}				
			}
			else
			{
				refCursor.SetCursorType( CCursor::CURSOR_DEFAULT );
			}
		}
		refGame.ResetCheckFrame();
		return;
	}

	refGame.IncreseCheckFrame();
}
#define SCREEN_CHECK_WIDTH 1
int CGameStateMain::CheckMouseRegion( int x, int y )
{
	int iCheckFlag = 0;

	/// 왼쪽 
	if( x < SCREEN_CHECK_WIDTH )	
		iCheckFlag |= SCREEN_LEFT;

	/// 오른쪽
	if( x > ( g_pCApp->GetWIDTH() - 1 - SCREEN_CHECK_WIDTH ) )	
		iCheckFlag |= SCREEN_RIGHT;

	
	/// 윗쪽
	if( y < SCREEN_CHECK_WIDTH )	
		iCheckFlag |= SCREEN_UP;

	/// 아랬쪽
	if( y > (g_pCApp->GetHEIGHT() - SCREEN_CHECK_WIDTH ) )	
		iCheckFlag |= SCREEN_DOWN;
	

	return iCheckFlag;
}

void CGameStateMain::UpdateCameraPositionByMouse()
{
	POINT ptCurrMouse;
	CGame::GetInstance().Get_MousePos( ptCurrMouse );
	int iCheckFlag = CheckMouseRegion( ptCurrMouse.x, ptCurrMouse.y );

	if( iCheckFlag & SCREEN_LEFT )
	{
		g_pCamera->Add_YAW( -CAMERA_MOVE_SPEED );
		return;

	}

	if( iCheckFlag & SCREEN_RIGHT )
	{
		g_pCamera->Add_YAW( CAMERA_MOVE_SPEED );
		return;
	}
/*
	if( iCheckFlag & SCREEN_UP )
		g_pCamera->Add_PITCH( -CAMERA_MOVE_SPEED );

	if( iCheckFlag & SCREEN_DOWN )
		g_pCamera->Add_PITCH( CAMERA_MOVE_SPEED );*/
}

int CGameStateMain::ProcWndMsgInstant( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	POINT ptMouse = { LOWORD( lParam ), HIWORD( lParam ) };
	switch( uiMsg )
	{
	case WM_MOUSEMOVE:
		{
#ifdef _DEBUG
			if( g_GameDATA.m_bObserverCameraMode )
			{
				if ( ( wParam & MK_RBUTTON ) ) 
				{
//					ObserverCameraTransform( (short)(ptMouse.x - m_PosRButtonClick.m_nX), (short)(ptMouse.y - m_PosRButtonClick.m_nY) );
					/*if ( ptMouse.x - m_PosRButtonClick.m_nX ) 
						g_pCamera->Add_YAW( (short)(ptMouse.x - m_PosRButtonClick.m_nX) );
					if ( ptMouse.y - m_PosRButtonClick.m_nY ) 
						g_pCamera->Add_PITCH( (short)(ptMouse.y - m_PosRButtonClick.m_nY) );*/

					m_PosRButtonClick.m_nX = (short)ptMouse.x;
					m_PosRButtonClick.m_nY = (short)ptMouse.y; 					
				}
			}else
#endif
			{
				if ( ( wParam & MK_RBUTTON ) ) 
				{
					if ( ptMouse.x - m_PosRButtonClick.m_nX ) 
						g_pCamera->Add_YAW( (short)(ptMouse.x - m_PosRButtonClick.m_nX) );
					if ( ptMouse.y - m_PosRButtonClick.m_nY ) 
						g_pCamera->Add_PITCH( (short)(ptMouse.y - m_PosRButtonClick.m_nY) );

					m_PosRButtonClick.m_nX = (short)ptMouse.x;
					m_PosRButtonClick.m_nY = (short)ptMouse.y; 
				}
			}
			break;
		}
	case WM_RBUTTONDOWN:
		m_PosRButtonClick.m_nX = (short)ptMouse.x;
		m_PosRButtonClick.m_nY = (short)ptMouse.y; 
		break;
	default:
		break;
	}
	return 0;
}


void CGameStateMain::ProcOberserCamerMove()
{
	if(KEY_DOWN_PROC('W') == TRUE)
	{
		SetObserverCameraSpeed(0.15f);
	}
	
	if(KEY_DOWN_PROC('S') == TRUE)
	{
		SetObserverCameraSpeed(-0.15f);
	}

	if(KEY_DOWN_PROC('A') == TRUE)
	{
		SetObserverCameraSideSpeed(-0.15f);
	}
	
	if(KEY_DOWN_PROC('D') == TRUE)
	{
		SetObserverCameraSideSpeed(0.15f);
	}
	
	if(KEY_DOWN_PROC('N') == TRUE)
	{
		SetObserverCameraHeightSpeed(0.15f);
	}
	
	if(KEY_DOWN_PROC('M') == TRUE)
	{
		SetObserverCameraHeightSpeed(-0.15f);
	}

	if(KEY_DOWN_PROC(37) == TRUE)
	{
		SetObserverCameraBaseRotation(0.2f);
	}
	
	if(KEY_DOWN_PROC(39) == TRUE)
	{
		SetObserverCameraBaseRotation(-0.2f);
	}

 	if(KEY_DOWN_PROC(38) == TRUE)
	{
		SetObserverCameraRotationPhi(0.035f);
	}
	
	if(KEY_DOWN_PROC(40) == TRUE)
	{
		SetObserverCameraRotationPhi(-0.035f);
	}

#ifdef __NPC_COLLISION	
	if(KEY_DOWN_PROC('1') == TRUE)
	{
		g_pObjMGR->ClosedCollisionNPCMotion();
	}
	if(KEY_DOWN_PROC('2') == TRUE)
	{
		g_pObjMGR->OpeningCollisionNPCMotion();
	}
    if(KEY_DOWN_PROC('3') == TRUE)
	{
		g_pObjMGR->OpenedCollisionNPCMotion();
	}

#endif
}
