//---------------------------------------------------------------------------

#include "PreHEADER.h"

#include "BCB_Tools.h"
#include "FrmMAIN.h"
#include "FrmABOUT.h"
#include "FrmLSCFG.h"
#include "FrmANNOUNCE.h"
#include "FrmLogMODE.h"
#include "FrmLoginRIGHT.h"
#include "FrmLimitUSER.h"
#include "CVclTHREAD.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMAIN *FormMAIN;

// #define __IGNORE_DLL


extern HINSTANCE g_hCurInstance;


#ifndef __IGNORE_DLL
    #include "../SHO_LS_DLL/SHO_LS_DLL.h"
    CEXE_API     *g_pExeAPI=NULL;

    #include "../SHO_LS_DLL/SHO_LS_DLL.h"
    // #pragma comment (lib, "../../LIB_BC/SHO_LS_DLL.lib" )
#endif



//---------------------------------------------------------------------------
/*
void WriteFILE (char *szString)
{
    SYSTEMTIME  sysTIME;

    GetLocalTime (&sysTIME);

    String stFileName;
    stFileName.printf ("LS%d-%d-%d.log", sysTIME.wYear, sysTIME.wMonth, sysTIME.wDay);

    FILE *fp = fopen (stFileName.c_str(), "a");
    if ( fp != NULL ) {
        fprintf (fp, "[%d:%d:%d] %s\n", sysTIME.wHour, sysTIME.wMinute, sysTIME.wSecond, szString);
        fclose (fp);
    }
}
//---------------------------------------------------------------------------
void WriteLOG (char *szString)
{
    if ( FormMAIN->MemoLOG->Lines->Count > 400 ) {
        FormMAIN->MemoLOG->Lines->Delete ( 0 );
    }

    FormMAIN->MemoLOG->Lines->Add (szString);
}
*/
//---------------------------------------------------------------------------
__fastcall TFormMAIN::TFormMAIN(TComponent* Owner)
    : TForm(Owner)
{
}

//---------------------------------------------------------------------------
#define STEP_START_SERVER_SOCK      0
#define STEP_START_CLIENT_SOCK      1
#define STEP_SHUTDOWN_CLIENT_SOCK   2
#define STEP_SHUTDONW_SERVER        3

void TFormMAIN::UpdateLoginRIGHT( int iRight )
{
    StatusBar1->Panels->Items[1]->Text = "접속권한:" + IntToStr( iRight );
}

void TFormMAIN::UpdateLimitUSER( int iLimitUserCNT )
{
    StatusBar1->Panels->Items[2]->Text = "동접제한:" + IntToStr( iLimitUserCNT );
}

void __fastcall TFormMAIN::AppMessage(tagMSG &Msg, bool &Handled)
{
/*
    switch( Msg.message ) {
        case WM_TEST_MSG :
            Handled = true;
            PrintLOG ("WM_TEST_MSG:: %d, %d ", Msg.wParam, Msg.lParam );
            break;
    }
*/    
    /* for all other messages, Handled remains False so that other message handlers can respond */
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::FormCreate(TObject *Sender)
{
    Application->OnMessage = AppMessage;
    CVclTHREAD::InitInstance();

    m_stINIFile = ChangeFileExt(Application->ExeName, ".ini");
    m_nProcSTEP = STEP_START_SERVER_SOCK;
    m_bAutoStart = false;

#ifndef __IGNORE_DLL
    g_pExeAPI = new CEXE_API;
    LS_Init( g_hCurInstance, g_pExeAPI );
#endif

    this->CheckInterface ();
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::FormClose(TObject *Sender, TCloseAction &Action)
{
#ifndef __IGNORE_DLL
	LS_Shutdown ();
	LS_Free ();

	SAFE_DELETE( g_pExeAPI );
#endif

	CVclTHREAD::Destroy ();
}

//---------------------------------------------------------------------------
void TFormMAIN::CheckInterface ()
{
    MS_Start1->Enabled = false;
    MS_Shutdown1->Enabled = false;

    MS_Acceptclient1->Enabled = false;
    MS_Donotacceptclient1->Enabled = false;

    MO_AnnounceChat->Enabled = false;

    ToolButtonSTART->Down = true;
    ToolButtonSHUTDOWN->Down = true;
    ToolButtonCloseCLIENT->Enabled = false;
    ToolButtonCloseCLIENT->Down = false;

    switch( m_nProcSTEP ) {
        case STEP_START_SERVER_SOCK     :
            MS_Start1->Enabled = true;
            ToolButtonSTART->Down = false;
            break;

        case STEP_START_CLIENT_SOCK     :
            MS_Start1->Enabled = true;
            MS_Acceptclient1->Enabled = true;
            MO_AnnounceChat->Enabled = true;

            ToolButtonSTART->Down = false;
            break;

        case STEP_SHUTDOWN_CLIENT_SOCK  :
            MS_Shutdown1->Enabled = true;
            MS_Donotacceptclient1->Enabled = true;
            MO_AnnounceChat->Enabled = true;

            ToolButtonSTART->Down = true;
            ToolButtonCloseCLIENT->Enabled = true;
            ToolButtonCloseCLIENT->Down = false;
            break;
    } ;
}

//---------------------------------------------------------------------------
bool TFormMAIN::Go_NextStep()
{
    bool bReturn = false;

    switch( m_nProcSTEP ) {
        case STEP_START_SERVER_SOCK :
            #ifndef __IGNORE_DLL
            this->UpdateLoginRIGHT( m_stLoginRIGHT.ToInt() );

            if ( LS_StartServerSOCKET( this->Handle, m_stDBIP.c_str(), m_stServerPORT.ToInt(), m_stLoginRIGHT.ToInt(),
                                        this->m_bUseGUMS ? m_stGumsIP.c_str() : NULL,
                                        m_stGumsPORT.ToInt(),
                                        this->m_bShowOnlyWS ) )
            #endif
            {
                m_nProcSTEP = STEP_START_CLIENT_SOCK;
                FormLSCFG->Start( this->m_bAutoStart, false);
                bReturn = true;

                StatusBar1->Panels->Items[4]->Text = "WP:"+m_stServerPORT;
            }
            break;

        case STEP_START_CLIENT_SOCK :
            #ifndef __IGNORE_DLL
            this->UpdateLimitUSER( m_iLimitUserCNT );

            if ( LS_StartClientSOCKET( m_stClientPORT.ToInt(), m_iLimitUserCNT, this->m_btMD5 ) )
            #endif
            {
                m_nProcSTEP = STEP_SHUTDOWN_CLIENT_SOCK;
                bReturn = true;

                StatusBar1->Panels->Items[5]->Text = "CP:"+m_stClientPORT;
            }
            break;

        case STEP_SHUTDOWN_CLIENT_SOCK :
            #ifndef __IGNORE_DLL
            LS_CloseClientSOCKET ();
            #endif

            m_nProcSTEP = STEP_START_CLIENT_SOCK;
            bReturn = true;
            break;
    }

    this->CheckInterface ();

    return bReturn;
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::MS_Start1Click(TObject *Sender)
{
    if ( m_nProcSTEP < STEP_SHUTDOWN_CLIENT_SOCK ) {
        Go_NextStep();
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::ToolButtonSTARTClick(TObject *Sender)
{
    MS_Start1Click( NULL );
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::MS_Shutdown1Click(TObject *Sender)
{
    if ( STEP_SHUTDOWN_CLIENT_SOCK == m_nProcSTEP ) {
        Go_NextStep();
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::ToolButtonSHUTDOWNClick(TObject *Sender)
{
    MS_Shutdown1Click( NULL );
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::ToolButtonHELPClick(TObject *Sender)
{
    FormABOUT->Show ();
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::MS_Acceptclient1Click(TObject *Sender)
{
    if ( STEP_START_CLIENT_SOCK == m_nProcSTEP ) {
        Go_NextStep();
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::MS_Donotacceptclient1Click(TObject *Sender)
{
    if ( STEP_SHUTDOWN_CLIENT_SOCK == m_nProcSTEP )
        Go_NextStep ();
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::ToolButtonCloseCLIENTClick(TObject *Sender)
{
    MS_Donotacceptclient1Click ( NULL );
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::MO_AnnounceChatClick(TObject *Sender)
{
    FormAnnounce->Show ();
}

//---------------------------------------------------------------------------
void TFormMAIN::SendAnnounceChat (String stAnnounceMSG)
{
#ifndef __IGNORE_DLL
    TListItem *pListItem;
    for (int iS=0; iS<this->ListViewSERVER->Items->Count; iS++) {
        pListItem = this->ListViewSERVER->Items->Item[ iS ];
        if ( !pListItem->Checked )
            continue;

        ::LS_AnnounceChat( pListItem->Data, stAnnounceMSG.c_str() );
    }
#endif
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::SendGsvANNOUNCEClick(TObject *Sender)
{
    // 선택된 서버에 공지 전송.
    FormAnnounce->Show ();
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::SetConnectRIGHTClick(TObject *Sender)
{
    // 선택된 서버의 접속 권한 설정.
    ;
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::Setlogmode1Click(TObject *Sender)
{
    FormLogMODE->Show ();
}
WORD TFormMAIN::GetLogMODE (t_LOG logType)
{
}
void TFormMAIN::SetLogMODE (WORD wLogMODE, t_LOG logType)
{
}

void TFormMAIN::SetLoginRIGHT( int iRight )
{
    this->UpdateLoginRIGHT( iRight );
    ::LS_SetLoginRIGHT( iRight );
}

void TFormMAIN::SetLimitUserCNT( int iLimitUserCNT )
{
    this->UpdateLimitUSER( iLimitUserCNT );
    ::LS_SetLimitUserCNT( iLimitUserCNT );
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::Setlimitusercount1Click(TObject *Sender)
{
    FormLimitUSER->Show ();
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::Setloginright1Click(TObject *Sender)
{
    FormLoginRIGHT->Show ();
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    if ( Application->MessageBox("Terminate Game Service ?", "Confirm", MB_YESNO | MB_ICONQUESTION) == IDNO )
        CanClose = false;
}

//---------------------------------------------------------------------------
void __fastcall TFormMAIN::Clear1Click(TObject *Sender)
{
    MemoLOG->Clear();
}

//---------------------------------------------------------------------------

void __fastcall TFormMAIN::Exit1Click(TObject *Sender)
{
#ifndef __IGNORE_DLL
	LS_Shutdown ();
	LS_Free ();

	SAFE_DELETE( g_pExeAPI );
#endif

	CVclTHREAD::Destroy ();
}
//---------------------------------------------------------------------------

