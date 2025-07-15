//---------------------------------------------------------------------------

#include "PreHEADER.h"

#include "BCB_Tools.h"
#include "FrmMAIN.h"
#include "FrmABOUT.h"
#include "FrmWSCFG.h"
#include "FrmLogMODE.h"
#include "FrmANNOUNCE.h"
#include "CVclTHREAD.h"
#include "WM_SHO.h"
#include "CProcess.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMAIN *FormMAIN;



 #define __USE_DLL
#ifdef  __USE_DLL
    #include "../SHO_WS_DLL/SHO_WS_DLL.h"
    CEXE_API    *g_pExeAPI=NULL;
#endif

//---------------------------------------------------------------------------


#define IMG_NO_ON   0
#define IMG_NO_OFF  1

#define STEP_SYSTEM_INIT        0
#define STEP_CONNECT_DB         1
#define STEP_LISTEN_ZONE        2
#define STEP_LISTEN_USER        3
#define STEP_ACTIVE_WORLD       4


void WriteLOG (char *szString)
{
//    ::EnterCriticalSection( &m_csLOG );

    if ( FormMAIN->MemoLOG->Lines->Count > 400 ) {
        FormMAIN->MemoLOG->Lines->Delete ( 0 );
    }

    FormMAIN->MemoLOG->Lines->Add (szString);

//    ::LeaveCriticalSection( &m_csLOG );
}
static char g_TmpStrBuff[ 1024 ];
void PrintLOG (char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(g_TmpStrBuff,fmt,argptr);
    va_end(argptr);

    WriteLOG( g_TmpStrBuff );
}

//---------------------------------------------------------------------------
__fastcall TFormMAIN::TFormMAIN(TComponent* Owner)
    : TForm(Owner)
{
}



void  __fastcall TFormMAIN::On_WM_SHO_REQ_STATUS (TMessage &Message)
{
    m_hMonSRV = (HWND)Message.LParam;
    switch( Message.WParam ) {
        case SHO_REQ_STATUS     :
            this->PostMessage2MON( 0, m_iUserCNT );
            break;

        case SHO_REQ_SHUTDOWN   :
            //PrintLOG ("SHO_REQ_SHUTDOWN:: %d, %d, Down: %d, %d", Message.WParam, Message.LParam, ToolButtonSTEP2->Down , ToolButtonSTEP3->Down );
            if ( ToolButtonSTEP3->Down ) {
                ToolButtonSTEP3->Down = false;
                ToolButtonSTEP3Click( ToolButtonSTEP3 );
                if ( ToolButtonSTEP2->Down ) {
                    ToolButtonSTEP2->Down = false;
                    ToolButtonSTEP2Click( ToolButtonSTEP2 );
                }
            }
            break;

        case SHO_REQ_CLOSE      :
            //PrintLOG ("SHO_REQ_CLOSE:: %d, %d ", Message.WParam, Message.LParam );
            if ( !ToolButtonSTEP3->Down && !ToolButtonSTEP2->Down ) {
                // 계정이 0개 인지 체크...
                this->Close ();
            }
            break;
        case SHO_REQ_USER_LIMIT :
            // 최대 동접자 제한....
            break;
    }
}
void  __fastcall TFormMAIN::On_WM_COPYDATA (TMessage &Message)
{
    // PrintLOG( "WM_COPYDATA" );
    PCOPYDATASTRUCT pMsgDATA = (PCOPYDATASTRUCT)Message.LParam;
    if ( SHO_REQ_ANNOUNCEMENT == pMsgDATA->dwData ) {
        // PrintLOG( (char*)pMsgDATA->lpData );
        // this->SendAnnounceChat( (char*)pMsgDATA->lpData );
    }
}

void __fastcall TFormMAIN::AppMessage(tagMSG &Msg, bool &Handled)
{
    if ( WM_SHO_REQ_STATUS == Msg.message ) {
        TMessage tMSG;
        tMSG.WParam = Msg.wParam;
        tMSG.LParam = Msg.lParam;
        this->On_WM_SHO_REQ_STATUS( tMSG );
        Handled = true;
        return;
    }
    /* for all other messages, Handled remains False so that other message handlers can respond */
}

//---------------------------------------------------------------------------
void TFormMAIN::PostMessage2MON (UINT uiMSG, WORD wUserCNT)
{
    if ( uiMSG )
        m_uiProcMSG = uiMSG;

    if ( m_hMonSRV ) {
        ::PostMessage( m_hMonSRV, m_uiProcMSG/*WM_SHO_REPLY_STATUS*/, wUserCNT, (LPARAM)(this->Handle) );
    }
}


//---------------------------------------------------------------------------
void __fastcall TFormMAIN::FormCreate(TObject *Sender)
{
    m_hMonSRV = CProcess::GetWindowHandleWithFileName( "SHO_MonSRV.exe" );

    m_uiProcMSG = 0;
    m_iUserCNT = 0;

    CVclTHREAD::InitInstance ();
	g_pExeAPI = new CEXE_API;

    m_stINIFile = ChangeFileExt(Application->ExeName, ".ini");
    m_bAutoStart = true;
    m_nProcSTEP = STEP_SYSTEM_INIT;// STEP_CONNECT_DB;

    Application->OnMessage = AppMessage;
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::FormClose(TObject *Sender, TCloseAction &Action)
{
	this->PostMessage2MON( WM_SHO_REPLY_CLOSE_START, 0 );

#ifdef  __USE_DLL
	::WS_Free ();
	SAFE_DELETE( g_pExeAPI );
#endif

	CVclTHREAD::Destroy ();

	this->PostMessage2MON( WM_SHO_REPLY_CLOSE_END, 0 );
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::About1Click(TObject *Sender)
{
    FormABOUT->Show ();
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::ToolButtonABOUTClick(TObject *Sender)
{
    FormABOUT->Show ();
}

//---------------------------------------------------------------------------
void TFormMAIN::ToggleBUTTON (TToolButton *pToolBTN, bool bOn)
{
    if ( bOn ) {
        pToolBTN->Down = true;
        pToolBTN->ImageIndex = IMG_NO_ON;
    } else {
        pToolBTN->Down = false;
        pToolBTN->ImageIndex = IMG_NO_OFF;
    }
}

//---------------------------------------------------------------------------
bool TFormMAIN::Next_STEP ()
{
    switch ( m_nProcSTEP ) {
    case STEP_SYSTEM_INIT :
        this->PostMessage2MON( WM_SHO_REPLY_SYSTEM_INIT, 0 );

        if ( !::WS_Init( ::HInstance, this->m_stDataDir.c_str(), this->m_iLangTYPE, g_pExeAPI ) ) {
            this->PostMessage2MON( WM_SHO_FAILED_SYSTEM_INIT, 0 );

            Application->MessageBox("Server Initialize error...", "SYSTEM ERROR", MB_OK | MB_ICONERROR );
            Application->Terminate();
            break;
        }
        m_nProcSTEP = STEP_CONNECT_DB;
        FormWSCFG->Start( true, 0 );
        return true;

    case STEP_CONNECT_DB :
        g_pExeAPI->WriteLOG ( "STEP_CONNECT_DB" );
        this->PostMessage2MON( WM_SHO_REPLY_DATABASE_INIT, 0 );

        if ( ::WS_ConnectDB( this->m_stDBServerIP.c_str(),
                             this->m_stDBName.c_str(),
                             this->m_stDBUser.c_str(),
                             this->m_stDBPw.c_str(),
                             this->m_stLogDBName.c_str(),
                             this->m_stLogDBPw.c_str() ) )
        {
            this->ToggleBUTTON( ToolButtonSTEP0, true );

            m_nProcSTEP = STEP_LISTEN_ZONE;
            FormWSCFG->Start( this->m_bAutoStart, STEP_LISTEN_ZONE );
            return true;
        }
        this->PostMessage2MON( WM_SHO_FAILED_DATABASE_INIT, 0 );
        break;

    case STEP_LISTEN_ZONE : // 채널 서버 접속 허용
        g_pExeAPI->WriteLOG ( "STEP_LISTEN_ZONE" );
        this->PostMessage2MON( WM_SHO_REPLY_NETWORK_INIT, 0 );
        
        if ( ::WS_Start (this->Handle,
                        this->m_stWorldName.c_str(),
                        this->m_stLoginServerIP.c_str(),    this->m_iLoginServerPORT,
                        this->m_stLogServerIP.c_str(),      this->m_iLogServerPORT,
                        this->m_iZoneListenPortNO,          this->m_iUserListenPortNO, this->m_bBlockCreateCHAR ) )
        {
            StatusBar1->Panels->Items[ 0 ]->Text = m_stWorldName;
            StatusBar1->Panels->Items[ 1 ]->Text = "Channel Port:"+IntToStr( this->m_iZoneListenPortNO );
            StatusBar1->Panels->Items[ 2 ]->Text = "Client Port:" +IntToStr( this->m_iUserListenPortNO );

            this->ToggleBUTTON( ToolButtonSTEP1, true );
            this->ToggleBUTTON( ToolButtonSTEP2, true );

            m_nProcSTEP = STEP_ACTIVE_WORLD;
			FormWSCFG->Close ();

            this->ToggleBUTTON( ToolButtonSTEP3, true );

            this->PostMessage2MON( WM_SHO_REPLY_RUNNING, 0 );
            return true;
        }
        this->PostMessage2MON( WM_SHO_FAILED_NETWORK_INIT, 0 );
        break;
    }
    return false;
}
//---------------------------------------------------------------------------
void TFormMAIN::SendAnnounceChat(String stAnnounceMSG)
{
    /*
    TListItem *pListItem;
    for (int iS=0; iS<this->ListViewZONE->Items->Count; iS++) {
        pListItem = this->ListViewZONE->Items->Item[ iS ];
        if ( !pListItem->Checked )
            continue;
    }
    */
    ::WS_AnnounceChat( stAnnounceMSG.c_str() );
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    if ( ToolButtonSTEP3->Down || ToolButtonSTEP2->Down ) {
        if ( Application->MessageBox("Terminate Game Service ?", "Confirm", MB_YESNO | MB_ICONQUESTION) == IDNO )
            CanClose = false;
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::ToolButtonSTEP0Click(TObject *Sender)
{
    // step 0
    if ( m_nProcSTEP == STEP_CONNECT_DB ) {
        if ( ToolButtonSTEP0->Down ) {
            // off 시켜라..
            ToolButtonSTEP0->ImageIndex = IMG_NO_ON;
        } else {
            // on 시켜라..
        }
    } else  ( (TToolButton*)Sender )->Down = !( (TToolButton*)Sender )->Down;
}


/*
bool TFormMAIN::Step3_ChannelSOCKET ()
{
    // 채널 서버 접속 허용
    if ( STEP_LISTEN_ZONE == m_nProcSTEP ) {
        return true;
    }
    return false;
}
bool TFormMAIN::Step4_ClientSOCKET ()
{
    // 사용자 접속 허용
    if ( STEP_LISTEN_USER == m_nProcSTEP ) {
    }
    return false;
}
void TFormMAIN::Step5_Active ()
{
}
*/
//---------------------------------------------------------------------------
void __fastcall TFormMAIN::ToolButtonSTEP1Click(TObject *Sender)
{
    // step 1
    if ( m_nProcSTEP == STEP_LISTEN_ZONE ) {
        if ( ToolButtonSTEP1->Down ) {
            // off 시켜라..
        } else {
            // on 시켜라..
        }
    } else  ( (TToolButton*)Sender )->Down = !( (TToolButton*)Sender )->Down;
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::ToolButtonSTEP2Click(TObject *Sender)
{
    // 접속된 모든 클라이언트 소켓 ON/OFF
    bool bTrue;
    if ( ToolButtonSTEP2->Down ) {
        // 아래 단계가 동작 중여야 한다.
        bTrue = ToolButtonSTEP1->Down;
    } else {
        // OFF 되라..
        bTrue = !ToolButtonSTEP3->Down;
    }

    if ( bTrue ) {
        this->ToggleBUTTON( (TToolButton*)Sender, ( (TToolButton*)Sender )->Down );

        if ( ToolButtonSTEP2->Down ) {
            // on 시켜라..
            ::WS_StartCLI_SOCKET ();
        } else {
            // off 시켜라..
            this->PostMessage2MON( WM_SHO_REPLY_SHUTDOWN_START, 0 );
            ::WS_ShutdownCLI_SOCKET ();
            this->PostMessage2MON( WM_SHO_REPLY_SHUTDOWN_END, 0 );
        }
    } else  ( (TToolButton*)Sender )->Down = !( (TToolButton*)Sender )->Down;
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::ToolButtonSTEP3Click(TObject *Sender)
{
    // 로그인 서버에 서버 활동/중지 전송.
    bool bTrue;
    if ( ToolButtonSTEP3->Down ) {
        // 동작중이 아니고 정지 시키려 한다.
        // 아래 단계가 동작 중여야 한다.
        bTrue = ToolButtonSTEP2->Down;
    } else
        bTrue = true;

	if ( bTrue ) {
        this->ToggleBUTTON( (TToolButton*)Sender, ( (TToolButton*)Sender )->Down );
        ::WS_Active( ToolButtonSTEP3->Down );
    } else  ( (TToolButton*)Sender )->Down = !( (TToolButton*)Sender )->Down;
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::Setlogmode1Click(TObject *Sender)
{
    FormLogMODE->Show ();
}
WORD TFormMAIN::GetLogMODE (t_LOG logType)
{
    return 0;
}
void TFormMAIN::SetLogMODE (WORD wLogMODE, t_LOG logType)
{
    ;
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::Sendannouncechat1Click(TObject *Sender)
{
    FormAnnounce->Show ();
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::ListViewUSERColumnClick(TObject *Sender, TListColumn *Column)
{
    BCBUtil::ListViewSORT(Sender, Column);
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::Shutdown1Click(TObject *Sender)
{
    ::WS_Shutdown ();
}

//---------------------------------------------------------------------------

void __fastcall TFormMAIN::ToggleActive1Click(TObject *Sender)
{
    if (  ListViewCHANNEL->Selected ) {
        int iChannelNO = ListViewCHANNEL->Selected->Caption.ToInt();
       ::WS_ToggleChannelActive( iChannelNO );
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::DisconnectChannelServer1Click(TObject *Sender)
{
    if (  ListViewCHANNEL->Selected ) {
        int iChannelNO = ListViewCHANNEL->Selected->Caption.ToInt();
       ::WS_DisconnectChannelServer( iChannelNO );
    }
}

//---------------------------------------------------------------------------

void __fastcall TFormMAIN::Exit1Click(TObject *Sender)
{
	this->PostMessage2MON( WM_SHO_REPLY_CLOSE_START, 0 );

#ifdef  __USE_DLL
	::WS_Free ();
	SAFE_DELETE( g_pExeAPI );
#endif

	CVclTHREAD::Destroy ();

    this->PostMessage2MON( WM_SHO_REPLY_CLOSE_END, 0 );
}
//---------------------------------------------------------------------------

