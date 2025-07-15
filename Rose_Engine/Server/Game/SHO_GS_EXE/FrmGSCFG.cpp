//---------------------------------------------------------------------------

#include "PreHEADER.h"

#include "BCB_Tools.h"
#include "FrmGSCFG.h"
#include "FrmDataDIR.h"
#include "FrmMAIN.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormGSCFG *FormGSCFG;

#define	MAX_CHANNEL_SERVER	32

//---------------------------------------------------------------------------
__fastcall TFormGSCFG::TFormGSCFG(TComponent* Owner)
    : TForm(Owner)
{
}

//---------------------------------------------------------------------------
void __fastcall TFormGSCFG::FormCreate(TObject *Sender)
{
    m_bFirst = true;
    BCBUtil::LoadControl( FormMAIN->m_stINIFile.c_str(), this );
}

//---------------------------------------------------------------------------

#define MAX_ZONE_USER_BUFF  8192
#define MIN_ZONE_USER_BUFF  256

void __fastcall TFormGSCFG::ButtonOKClick(TObject *Sender)
{
    StopTimer( Sender );

    int iChannelNO = EditChannelNO->Text.ToInt();
    if ( iChannelNO < 0 || iChannelNO >=  MAX_CHANNEL_SERVER )
         return;

    BCBUtil::SaveControl( FormMAIN->m_stINIFile.c_str(), this );

    FormMAIN->m_stWorldName         = EditWorldNAME->Text;
    FormMAIN->m_iClientListenPort   = EditClientPORT->Text.ToInt();
    FormMAIN->m_stClientListenIP    = EditClientIP->Text;
    FormMAIN->m_iChannelNO          = iChannelNO;
    FormMAIN->m_iLowAge             = EditLowAGE->Text.ToInt ();
    FormMAIN->m_iHighAge            = EditHighAGE->Text.ToInt ();
    FormMAIN->m_iUserLIMIT          = EditMaxUSER->Text.ToInt ();

    if ( FormMAIN->m_iUserLIMIT >= MAX_ZONE_USER_BUFF ) {
        FormMAIN->m_iUserLIMIT = MAX_ZONE_USER_BUFF;
    } else
    if ( FormMAIN->m_iUserLIMIT < MIN_ZONE_USER_BUFF ) {
        FormMAIN->m_iUserLIMIT = MIN_ZONE_USER_BUFF;
    }
    EditMaxUSER->Text = IntToStr( FormMAIN->m_iUserLIMIT );

    this->Close ();

    if ( !FormMAIN->Go_NextStep() ) {
        this->Show ();
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormGSCFG::ButtonRESETClick(TObject *Sender)
{
    FormMAIN->InitZone ();
}

//---------------------------------------------------------------------------
void TFormGSCFG::Start( bool bAutoStart )
{
    Timer1->Enabled = bAutoStart;

    ProgressBar1->Position = 0;
    ProgressBar1->Max = EditWaitTIME->Text.ToInt();
    if ( ProgressBar1->Max < 1 )
        ProgressBar1->Max = 1;

    this->Show ();
}

//---------------------------------------------------------------------------
void __fastcall TFormGSCFG::Timer1Timer(TObject *Sender)
{
    if ( ProgressBar1->Position >= ProgressBar1->Max ) {
        this->ButtonOKClick( Sender );
        return;
    }
    ProgressBar1->Position ++;
}

//---------------------------------------------------------------------------
void __fastcall TFormGSCFG::StopTimer(TObject *Sender)
{
    if ( Sender == Timer1 ) {
        FormMAIN->m_bAutoStart = true;
    }

    Timer1->Enabled = false;
    ProgressBar1->Position = 0;
}

//---------------------------------------------------------------------------
void __fastcall TFormGSCFG::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    StopTimer( NULL );
}

//---------------------------------------------------------------------------
void __fastcall TFormGSCFG::Selectall1Click(TObject *Sender)
{
    TListItem *pListItem;

    for (int iZ=0; iZ<this->ListViewZONE->Items->Count; iZ++) {
        pListItem = this->ListViewZONE->Items->Item[ iZ ];
        pListItem->Selected = true;
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormGSCFG::Reset1Click(TObject *Sender)
{
    this->ButtonRESETClick( Sender );
}

//---------------------------------------------------------------------------
void __fastcall TFormGSCFG::Inverselyselect1Click(TObject *Sender)
{
    TListItem *pListItem;

    for (int iZ=0; iZ<this->ListViewZONE->Items->Count; iZ++) {
        pListItem = this->ListViewZONE->Items->Item[ iZ ];
        pListItem->Checked = !pListItem->Checked;
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormGSCFG::Select1Click(TObject *Sender)
{
    TListItem *pListItem;

    // 선택된것 체크...
    for (int iZ=0; iZ<this->ListViewZONE->Items->Count; iZ++) {
        pListItem = this->ListViewZONE->Items->Item[ iZ ];
        if ( pListItem->Selected )
            pListItem->Checked = true;
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormGSCFG::Uncheck1Click(TObject *Sender)
{
    TListItem *pListItem;

    // 선택된것 체크...
    for (int iZ=0; iZ<this->ListViewZONE->Items->Count; iZ++) {
        pListItem = this->ListViewZONE->Items->Item[ iZ ];
        if ( pListItem->Selected )
            pListItem->Checked = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormGSCFG::FormShow(TObject *Sender)
{
    if ( m_bFirst ) {
        m_bFirst = false;
        FormMAIN->InitZone ();
    }
}
//---------------------------------------------------------------------------

