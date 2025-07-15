//---------------------------------------------------------------------------

#include "PreHEADER.h"

#include "BCB_Tools.h"
#include "FrmMAIN.h"
#include "FrmWSCFG.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormWSCFG *FormWSCFG;
//---------------------------------------------------------------------------
__fastcall TFormWSCFG::TFormWSCFG(TComponent* Owner)
    : TForm(Owner)
{
}


//---------------------------------------------------------------------------
void TFormWSCFG::Start( bool bAutoStart, int iStepNO )
{
    Label1->Enabled = false;
    Label2->Enabled = false;

    Label1->Enabled = false;
    Label2->Enabled = false;
    Label3->Enabled = false;
    Label4->Enabled = false;
    Label5->Enabled = false;
    Label6->Enabled = false;
    Label7->Enabled = false;
    Label9->Enabled = false;
    Label10->Enabled = false;

    Label11->Enabled = false;
    Label12->Enabled = false;
    Label13->Enabled = false;

    EditDBServerIP->Enabled = false;
    EditDBTableNAME->Enabled = false;
    EditDBAccount->Enabled = false;
    EditDBPW->Enabled = false;
    EditLogAccount->Enabled = false;
    EditLogPW->Enabled = false;

    EditZoneListenPORT->Enabled = false;

    EditWorldNAME->Enabled = false;
    EditLoginServerIP->Enabled = false;
    EditLoginServerPORT->Enabled = false;
    EditExtIP->Enabled = false;

    EditUserListenPORT->Enabled = false;

    if ( 0 == iStepNO ) {
        EditDBServerIP->Enabled = true;
        EditDBTableNAME->Enabled = true;
        EditDBAccount->Enabled = true;
        EditDBPW->Enabled = true;

        Label1->Enabled = true;
        Label2->Enabled = true;
        Label12->Enabled = true;
        Label13->Enabled = true;
        
        Label9->Enabled = true;
        Label10->Enabled = true;

        EditLogAccount->Enabled = true;
        EditLogPW->Enabled = true;
    } else {
        EditZoneListenPORT->Enabled = true;

        EditWorldNAME->Enabled = true;
        EditLoginServerIP->Enabled = true;
        EditLoginServerPORT->Enabled = true;

        EditExtIP->Enabled = true;
        EditUserListenPORT->Enabled = true;

        Label11->Enabled = true;
        Label3->Enabled = true;
        Label4->Enabled = true;
        Label5->Enabled = true;
        Label6->Enabled = true;
        Label7->Enabled = true;
    }

    Timer1->Enabled = bAutoStart;

    ProgressBar1->Position = 0;
    ProgressBar1->Max = EditWaitTIME->Text.ToInt();
    if ( ProgressBar1->Max < 1 )
        ProgressBar1->Max = 1;

    this->Show ();
}


//---------------------------------------------------------------------------
void __fastcall TFormWSCFG::Timer1Timer(TObject *Sender)
{
    if ( ProgressBar1->Position >= ProgressBar1->Max ) {
        this->ButtonOKClick( Sender );
        return;
    }
    ProgressBar1->Position ++;
}
//---------------------------------------------------------------------------
void __fastcall TFormWSCFG::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    StopTimer( NULL );
}

//---------------------------------------------------------------------------
void __fastcall TFormWSCFG::StopTimer(TObject *Sender)
{
    if ( Sender == Timer1 ) {
        FormMAIN->m_bAutoStart = true;
    }

    Timer1->Enabled = false;
    ProgressBar1->Position = 0;
}

//---------------------------------------------------------------------------
void __fastcall TFormWSCFG::FormCreate(TObject *Sender)
{
    BCBUtil::LoadControl( FormMAIN->m_stINIFile.c_str(), this );
//  this->Start( true, 0 );
}

//---------------------------------------------------------------------------
void __fastcall TFormWSCFG::ButtonOKClick(TObject *Sender)
{
    StopTimer( Sender );
    
    FormMAIN->m_stDBServerIP = EditDBServerIP->Text;
    FormMAIN->m_stDBName = EditDBTableNAME->Text;
    FormMAIN->m_stDBUser = EditDBAccount->Text;
    FormMAIN->m_stDBPw   = EditDBPW->Text;
    FormMAIN->m_stLogDBName = EditLogAccount->Text;
    FormMAIN->m_stLogDBPw   = EditLogPW->Text;

    FormMAIN->m_stLoginServerIP = EditLoginServerIP->Text;
    FormMAIN->m_iLoginServerPORT = EditLoginServerPORT->Text.ToInt();

    // Log server 사용 안함 !!!
    FormMAIN->m_stLogServerIP  = FormMAIN->m_stDBServerIP;
    FormMAIN->m_iLogServerPORT = 13571;//EditLogServerPORT->Text.ToInt();

    FormMAIN->m_iZoneListenPortNO = EditZoneListenPORT->Text.ToInt();

    FormMAIN->m_stWorldName = EditWorldNAME->Text;

    FormMAIN->m_iUserListenPortNO = EditUserListenPORT->Text.ToInt();
    FormMAIN->m_bBlockCreateCHAR  = CheckBoxCreateCHAR->Checked; 

    if ( FormMAIN->Next_STEP() ) {
        BCBUtil::SaveControl( FormMAIN->m_stINIFile.c_str(), this );
    }
}
//---------------------------------------------------------------------------
