//---------------------------------------------------------------------------

#include "PreHEADER.h"

#include "FrmMAIN.h"
#include "FrmGSCFG.h"
#include "FrmDBCFG.h"
#include "FrmDataDIR.h"
#include "FrmABOUT.h"
#include "FrmANNOUNCE.h"
#include "FrmLogMODE.h"
#include "BCB_Tools.h"
#include "CVclTHREAD.h"

#include "CProcess.h"


#include "../SHO_GS_DLL/SHO_GS_DLL.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cdiroutl"
#pragma resource "*.dfm"
TFormMAIN *FormMAIN;


//---------------------------------------------------------------------------
#define STEP_INITDATA       0
#define STEP_CONNECT2DB     1
#define STEP_SELECTZONE     2
#define STEP_SHUTDOWN       3
//#define STEP_SHUTDOWNING    4
//#define STEP_SHUTDOWNED

//---------------------------------------------------------------------------
__fastcall TFormMAIN::TFormMAIN(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

extern HINSTANCE g_hCurInstance;
CEXE_API *g_pExeAPI=NULL;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TFormMAIN::AppException(TObject *Sender, Exception *E)
{
    /*
    FILE *fp = fopen ( "J2_ERROR.log", "a");
    if ( fp != NULL ) {
        SYSTEMTIME  sysTIME;
        GetLocalTime (&sysTIME);
        // occrrnce time ..
        fprintf (fp, "system error : %d-%d-%d[%d:%d:%d] \n",
            sysTIME.wYear, sysTIME.wMonth, sysTIME.wDay,
            sysTIME.wHour, sysTIME.wMinute, sysTIME.wSecond);

        String stObjectName;
        stObjectName = CUtil::Get_TObjectName( E, "AppException" );
        fprintf (fp, "Object: %s \n", stObjectName.c_str() );

        // HelpContext
        fprintf (fp, "HelpContext: %d\n", E->HelpContext);
        // error msg ..
        fprintf (fp, "Message: %s\n", E->Message.c_str() );
        fprintf (fp, "Connection: %d / %d \n\n\n", m_iCurUserCnt, m_iMaxUserCnt);
        fclose (fp);
    }

    // 오류 무시할것들..
    // char *pErrorMSG = E->Message.c_str();
    // if ( !strcmpi( pErrorMSG, "xxxx" ) )
    //    return;
    // 발생한 Execption에따라서... 치명적 오류가 아닌것들은 개별적으로 Execption 처리해야...
    // Application->ShowException( E );

    // 접속된 클라이언트 모두 종료...
    // FormDestory중에 호출되어 지면 이미 제거된것들이 있어 오류가 발생한다.
    g_pClientManager->_Free ();
    */

//    EXCEPTION_POINTERS *xp=GetExceptionInformation();

    Application->Terminate();
}

void  __fastcall TFormMAIN::On_WM_SHO_REQ_STATUS (TMessage &Message)
{
    m_hMonSRV = (HWND)Message.LParam;
    switch( Message.WParam ) {
        case SHO_REQ_STATUS     :
        {
            union {
                DWORD   dwCnt;
                WORD    wCnt[2];
            } ;
            wCnt[0] = m_iUserCNT;
            wCnt[1] = m_iUserLIMIT;
            this->PostMessage2MON( 0, dwCnt );
            break;
        }

        case SHO_REQ_SHUTDOWN   :

			this->SM_Shutdown1Click ( NULL );
			break;

		case SHO_REQ_CLOSE      :

            if ( STEP_SHUTDOWN != this->m_btStep ) {
                // 계정이 0개 인지 체크...
                this->Close ();
            }
            break;
        case SHO_REQ_USER_LIMIT :
        {
            m_iUserLIMIT = (int)( Message.LParam );
            ::GS_SetUserLIMIT( m_iUserLIMIT );
            FormGSCFG->EditMaxUSER->Text = IntToStr( m_iUserLIMIT );
            BCBUtil::SaveControl( m_stINIFile.c_str(), FormGSCFG );
            break;
        }
    }
}
void  __fastcall TFormMAIN::On_WM_COPYDATA (TMessage &Message)
{
    // PrintLOG( "WM_COPYDATA" );
    PCOPYDATASTRUCT pMsgDATA = (PCOPYDATASTRUCT)Message.LParam;
	if ( SHO_REQ_ANNOUNCEMENT == pMsgDATA->dwData ) {
        this->SendAnnounceChat( (char*)pMsgDATA->lpData );
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


void __fastcall TFormMAIN::FormCreate(TObject *Sender)
{
//    Application->OnException = AppException;
    m_hMonSRV = CProcess::GetWindowHandleWithFileName( "SHO_MonSRV.exe" );

    m_uiProcMSG = 0;

    CVclTHREAD::InitInstance ();
    g_pExeAPI = new CEXE_API;

    m_stINIFile = ChangeFileExt(Application->ExeName, ".ini");
    m_bAutoStart = false;

    this->MemoLOG->Clear ();
    for (int i=0;i<=ParamCount();i++)
    {
        MemoLOG->Lines->Add( ParamStr(i) );
    }

    m_btStep = 0;
    Application->OnMessage = AppMessage;
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::FormClose(TObject *Sender, TCloseAction &Action)
{
	this->PostMessage2MON( WM_SHO_REPLY_CLOSE_START, 0 );

	::GS_Free ();

	delete g_pExeAPI;
	CVclTHREAD::Destroy ();

	this->PostMessage2MON( WM_SHO_REPLY_CLOSE_END, 0 );
}

//---------------------------------------------------------------------------
void TFormMAIN::PostMessage2MON (UINT uiMSG, DWORD dwUserCNT)
{
    if ( uiMSG )
        m_uiProcMSG = uiMSG;

    if ( m_hMonSRV ) {
        ::PostMessage( m_hMonSRV, m_uiProcMSG/*WM_SHO_REPLY_STATUS*/, dwUserCNT, (LPARAM)(this->Handle) );
    }
}

//---------------------------------------------------------------------------
bool TFormMAIN::Go_NextStep()
{
    switch ( m_btStep ) {
        case STEP_INITDATA :    // 서버 초기화 / 데이타 로딩
        {
			// FormDataDIR 생성시 처음 호출...

            this->PostMessage2MON( WM_SHO_REPLY_SYSTEM_INIT, 0 );

        //__try
        {
            // this->m_iLangTYPE = 0; ????
            if ( !::GS_Init( g_hCurInstance, this->m_stDataDir.c_str(), this->m_iLangTYPE, g_pExeAPI ) ) {
                this->PostMessage2MON( WM_SHO_FAILED_SYSTEM_INIT, 0 );

                Application->MessageBox("Server Initialize error...", "SYSTEM ERROR", MB_OK | MB_ICONERROR );
                Application->Terminate();
                break;
            }
        }
         //__except (GS_Execption( GetExceptionInformation() ))
         {
           //     Application->Terminate();
            //    return 0;
         }
            m_btStep = STEP_CONNECT2DB;

            this->PostMessage2MON( WM_SHO_REPLY_DATABASE_INIT, 0 );
            FormDBCFG->Start( m_bAutoStart );
            return true;
        }
        case STEP_CONNECT2DB :    // DB 서버 접속...
        {
       
            if ( !::GS_ConnectSERVER( m_stDBServerIP.c_str(),   m_stDBName.c_str(),
                                    m_stDBUser.c_str(),         m_stDBPW.c_str(),
                                    m_stLogUser.c_str(),        m_stLogPW.c_str(),
                                    m_stMallUser.c_str(),       m_stMallPW.c_str(),
                                    m_stWorldServerIP.c_str(),	m_iWorldPort,
                                    m_stAccountServerIP.c_str(),m_iAccountPort ) )
            {
                this->PostMessage2MON( WM_SHO_FAILED_DATABASE_INIT, 0 );

                Application->MessageBox("Database server connect failed ...", "SYSTEM ERROR", MB_OK | MB_ICONERROR );
                FormDBCFG->Start( false );
                break;
            }

            m_btStep = STEP_SELECTZONE;

            this->PostMessage2MON( WM_SHO_REPLY_ZONE_INIT, 0 );
            FormGSCFG->Start( m_bAutoStart );
            return true;
        }
        case STEP_SELECTZONE :          // ip 설정및 존 선택...
        {
            this->ListViewZONE->Items->Clear ();

            ::GS_InitZONE( false );     // 모든 존 로컬 설정 없앰.

            TListItem *pListItem;
            for (int iZ=0; iZ<FormGSCFG->ListViewZONE->Items->Count; iZ++) {
                pListItem = FormGSCFG->ListViewZONE->Items->Item[ iZ ];

                if ( !pListItem->Checked )
                    continue;

                int iZoneNO = pListItem->Caption.ToInt();
                ::GS_CheckZoneACTIVE( iZoneNO, true );
            /*
                String stZoneNO   = BCBUtil::GetListItemTEXT( pListItem, 0 );
                String stZoneName = BCBUtil::GetListItemTEXT( pListItem, 1 );
                BCBUtil::AddListITEM( this->ListViewZONE,
                                        stZoneNO.c_str(),
                                        stZoneName.c_str(),
                                        "0", "0", NULL );
            */
            }

            bool bResult;
            bResult = ::GS_Start( this->Handle, m_stWorldName.c_str(), m_stClientListenIP.c_str(),
                            m_iClientListenPort, m_iChannelNO, m_iLowAge, m_iHighAge );
			if ( !bResult ) {
                this->PostMessage2MON( WM_SHO_FAILED_ZONE_INIT, 0 );
                break;
            }
            ::GS_SetUserLIMIT( m_iUserLIMIT );

            StatusBar1->Panels->Items[ 0 ]->Text = m_stWorldName;
            StatusBar1->Panels->Items[ 1 ]->Text = "Channel No:"+IntToStr( m_iChannelNO );
            StatusBar1->Panels->Items[ 2 ]->Text = "Age:" + IntToStr(m_iLowAge) + " ~ " + IntToStr(m_iHighAge);
            StatusBar1->Panels->Items[ 3 ]->Text = "Client Port:"+IntToStr( m_iClientListenPort );
            StatusBar1->Panels->Items[ 4 ]->Text = "Limits: "+IntToStr( m_iUserLIMIT );

            this->PostMessage2MON( WM_SHO_REPLY_RUNNING, m_iUserLIMIT );
            m_btStep = STEP_SHUTDOWN;
            return true;
        }
        case STEP_SHUTDOWN :
            m_btStep = STEP_SELECTZONE;

            this->PostMessage2MON( WM_SHO_REPLY_SHUTDOWN_START, 0 );
            ::GS_Shutdown ();
            this->PostMessage2MON( WM_SHO_REPLY_SHUTDOWN_END, 0 );
            return true;
    }
    return false;
}


//---------------------------------------------------------------------------
void TFormMAIN::InitZone ()
{
    FormGSCFG->ListViewZONE->Items->Clear ();

    TListItem *pListItem;

    short nZoneCNT = GS_InitZONE( false );
    for (int iZ=1; iZ<nZoneCNT; iZ++) {
        // 존 리스트 초기화...
        String stZoneNO = IntToStr( iZ );
        char *szZoneName = GS_ZoneName( iZ );
        if ( szZoneName ) {
            pListItem = BCBUtil::AddListITEM( FormGSCFG->ListViewZONE, stZoneNO.c_str(), szZoneName, NULL );
            if ( pListItem )
                pListItem->Checked = true;
        }
    }
}


//---------------------------------------------------------------------------
void __fastcall TFormMAIN::SM_Start1Click(TObject *Sender)
{
    switch ( m_btStep ) {
        case STEP_INITDATA :
            FormDataDIR->Start( false );
            break;
        case STEP_CONNECT2DB :
            FormDBCFG->Start( false );
            break;
        case STEP_SELECTZONE :
            FormGSCFG->Start( false );
            break;
        default :
            return;
    }
}


//---------------------------------------------------------------------------
void __fastcall TFormMAIN::SM_Shutdown1Click(TObject *Sender)
{
    if ( m_btStep == STEP_SHUTDOWN )
        Go_NextStep ();
}


//---------------------------------------------------------------------------
void __fastcall TFormMAIN::About1Click(TObject *Sender)
{
    FormABOUT->Show ();
}


//---------------------------------------------------------------------------

void __fastcall TFormMAIN::Announcechat1Click(TObject *Sender)
{
    FormAnnounce->Show ();
}
//---------------------------------------------------------------------------

void __fastcall TFormMAIN::Setlogmode1Click(TObject *Sender)
{
    FormLogMODE->Show ();
}
//---------------------------------------------------------------------------
void TFormMAIN::SendAnnounceChat(String stMsg)
{
    short nZoneNO = 0;      // send to all zone ...
    ::GS_AnnounceChat( nZoneNO, stMsg.c_str() );
}
WORD TFormMAIN::GetLogMODE (t_LOG logType)
{
    ;
    return 0;
}
void TFormMAIN::SetLogMODE (WORD wLogMODE, t_LOG logType)
{
    ;
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    if ( STEP_SHUTDOWN == this->m_btStep ) {
        if ( Application->MessageBox("Terminate Game Service ?", "Confirm", MB_YESNO | MB_ICONQUESTION) == IDNO )
            CanClose = false;
    }
}

//---------------------------------------------------------------------------


void __fastcall TFormMAIN::Config1Click(TObject *Sender)
{
    ;    
}
//---------------------------------------------------------------------------

void __fastcall TFormMAIN::SM_Exit1Click(TObject *Sender)
{
	this->PostMessage2MON( WM_SHO_REPLY_CLOSE_START, 0 );

	::GS_Free ();

	delete g_pExeAPI;
	CVclTHREAD::Destroy ();

    this->PostMessage2MON( WM_SHO_REPLY_CLOSE_END, 0 );
}
//---------------------------------------------------------------------------

