//---------------------------------------------------------------------------

#include "PreHEADER.h"

#include "BCB_Tools.h"
#include "FrmLSCFG.h"
#include "FrmMAIN.h"
#include "classMD5.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormLSCFG *FormLSCFG;
//---------------------------------------------------------------------------
__fastcall TFormLSCFG::TFormLSCFG(TComponent* Owner)
    : TForm(Owner)
{
}


//---------------------------------------------------------------------------
void __fastcall TFormLSCFG::FormCreate(TObject *Sender)
{
    BCBUtil::LoadControl( FormMAIN->m_stINIFile.c_str(), this );
    if ( EditPW->Text.Length() == 32 )
        ::CopyMemory( this->m_btMD5, EditPW->Text.c_str(), 32 );

    Position = poMainFormCenter;
    this->Start( true, true );
}


//---------------------------------------------------------------------------
void TFormLSCFG::Start (bool bAutoStart, bool bFirst )
{
    Timer1->Enabled = bAutoStart;

    ProgressBar1->Position = 0;
    ProgressBar1->Max = EditWaitTIME->Text.ToInt();
    if ( ProgressBar1->Max < 1 )
        ProgressBar1->Max = 1;

    EditDBIP->Enabled = false;
    EditServerPORT->Enabled = false;
    EditLoginRIGHT->Enabled = false;
    StaticTextDB->Enabled = false;
    StaticTextSERVER->Enabled = false;
    StaticTextRIGHT->Enabled = false;

    EditClientPORT->Enabled = false;
    StaticTextCLIENT->Enabled = false;

    CheckBoxGUMS->Enabled = false;
    StaticTextGumsIP->Enabled = false;
    StaticTextGumsPORT->Enabled = false;
    EditGumsIP->Enabled = false;
    EditGumsPORT->Enabled = false;

    if ( bFirst ) {
        this->Caption = "Config step 1";
        EditDBIP->Enabled = true;
        EditServerPORT->Enabled = true;
        EditLoginRIGHT->Enabled = true;
        StaticTextDB->Enabled = true;
        StaticTextSERVER->Enabled = true;
        StaticTextRIGHT->Enabled = true;

        CheckBoxGUMS->Enabled = true;
        if ( CheckBoxGUMS->Checked ) {
            StaticTextGumsIP->Enabled = true;
            StaticTextGumsPORT->Enabled = true;
            EditGumsIP->Enabled = true;
            EditGumsPORT->Enabled = true;
        }
    } else {
        this->Caption = "Config step 2";
        EditClientPORT->Enabled = true;
        StaticTextCLIENT->Enabled = true;
    }
    this->Show ();
}


//---------------------------------------------------------------------------
void __fastcall TFormLSCFG::ButtonOKClick(TObject *Sender)
{
    StopTimer( Sender );
    this->Close();

    FormMAIN->m_stDBIP = EditDBIP->Text;
    FormMAIN->m_stClientPORT  = EditClientPORT->Text;
    FormMAIN->m_stServerPORT  = EditServerPORT->Text;
    FormMAIN->m_stLoginRIGHT  = EditLoginRIGHT->Text;
    FormMAIN->m_iLimitUserCNT = EditLimitUserCNT->Text.ToInt ();

    FormMAIN->m_bUseGUMS    = CheckBoxGUMS->Checked;
    FormMAIN->m_stGumsIP    = EditGumsIP->Text;
    FormMAIN->m_stGumsPORT  = EditGumsPORT->Text;

    FormMAIN->m_bShowOnlyWS = CheckBoxWS->Checked;
    ::CopyMemory( FormMAIN->m_btMD5, this->m_btMD5, 32 );

    if ( FormMAIN->Go_NextStep () ) {
        EditPW->Text = (char*)this->m_btMD5;
        BCBUtil::SaveControl( FormMAIN->m_stINIFile.c_str(), this );
    } else {
        this->Show ();
    }
}


//---------------------------------------------------------------------------
void __fastcall TFormLSCFG::StopTimer(TObject *Sender)
{
    if ( Sender == Timer1 ) {
        FormMAIN->m_bAutoStart = true;
    }

    Timer1->Enabled = false;
    ProgressBar1->Position = 0;
}


//---------------------------------------------------------------------------
void __fastcall TFormLSCFG::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    StopTimer( NULL );
}


//---------------------------------------------------------------------------
void __fastcall TFormLSCFG::Timer1Timer(TObject *Sender)
{
    if ( ProgressBar1->Position >= ProgressBar1->Max ) {
        this->ButtonOKClick( Sender );
        return;
    }
    ProgressBar1->Position ++;
}

//---------------------------------------------------------------------------

void __fastcall TFormLSCFG::CheckBoxGUMSClick(TObject *Sender)
{
    this->StopTimer( NULL );
    if ( CheckBoxGUMS->Checked ) {
        EditGumsIP->Enabled = true;
        EditGumsPORT->Enabled = true;
    } else {
        EditGumsIP->Enabled = false;
        EditGumsPORT->Enabled = false;
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormLSCFG::ButtonPWClick(TObject *Sender)
{
    String stAscPW = EditPW->Text;

    if ( stAscPW.Length() >= 2 ) {
        GetMD5 (m_btMD5, (unsigned char*)stAscPW.c_str(), stAscPW.Length() );
        m_btMD5[32] = 0;
        EditPW->Text = (char*)m_btMD5;
    }
}
//---------------------------------------------------------------------------

