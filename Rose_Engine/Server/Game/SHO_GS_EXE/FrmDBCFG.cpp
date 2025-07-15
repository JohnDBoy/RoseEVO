//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "classLOG.h"
#include "BCB_Tools.h"
#include "FrmDBCFG.h"
#include "FrmMAIN.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormDBCFG *FormDBCFG;
//---------------------------------------------------------------------------
__fastcall TFormDBCFG::TFormDBCFG(TComponent* Owner)
    : TForm(Owner)
{
}

//---------------------------------------------------------------------------
void TFormDBCFG::Start( bool bAutoStart )
{
    Timer1->Enabled = bAutoStart;

    ProgressBar1->Position = 0;
    ProgressBar1->Max = EditWaitTIME->Text.ToInt();
    if ( ProgressBar1->Max < 1 )
        ProgressBar1->Max = 1;

    this->Show ();
}

//---------------------------------------------------------------------------
void __fastcall TFormDBCFG::ButtonOKClick(TObject *Sender)
{
    StopTimer( Sender );

    FormMAIN->m_stWorldServerIP = EditWorldServerIP->Text;
    FormMAIN->m_iWorldPort      = EditLoginWorldPORT->Text.ToInt();

    FormMAIN->m_stAccountServerIP = EditAccountServerIP->Text;
    FormMAIN->m_iAccountPort      = EditAccountServerPORT->Text.ToInt();

    FormMAIN->m_stDBServerIP    = EditDBIP->Text;
    FormMAIN->m_stDBName        = EditDBName->Text;
    FormMAIN->m_stDBUser        = EditDBUser->Text;
    FormMAIN->m_stDBPW          = EditDBPassword->Text;

    FormMAIN->m_stLogUser       = EditLogUser->Text;
    FormMAIN->m_stLogPW         = EditLogPassword->Text;

    FormMAIN->m_stMallUser      = EditMallUser->Text;
    FormMAIN->m_stMallPW        = EditMallPW->Text;
    this->Close ();

    if ( !FormMAIN->Go_NextStep() ) {
        this->Show ();
    } else
        BCBUtil::SaveControl( FormMAIN->m_stINIFile.c_str(), this );
}

//---------------------------------------------------------------------------
void __fastcall TFormDBCFG::FormCreate(TObject *Sender)
{
    BCBUtil::LoadControl( FormMAIN->m_stINIFile.c_str(), this );
    Timer1->Enabled = false;
}

//---------------------------------------------------------------------------
void __fastcall TFormDBCFG::Timer1Timer(TObject *Sender)
{
    if ( ProgressBar1->Position >= ProgressBar1->Max ) {
        this->ButtonOKClick( Sender );
        return;
    }
    ProgressBar1->Position ++;
}

//---------------------------------------------------------------------------
void __fastcall TFormDBCFG::StopTimer(TObject *Sender)
{
    if ( Sender == Timer1 ) {
        FormMAIN->m_bAutoStart = true;
    }

    Timer1->Enabled = false;
    ProgressBar1->Position = 0;
}
//---------------------------------------------------------------------------

void __fastcall TFormDBCFG::FormClose(TObject *Sender, TCloseAction &Action)
{
    StopTimer( NULL );
}
//---------------------------------------------------------------------------

