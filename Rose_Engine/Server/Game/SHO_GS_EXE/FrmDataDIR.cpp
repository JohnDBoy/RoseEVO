//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "classLOG.h"
#include "BCB_Tools.h"
#include "FrmMAIN.h"
#include "FrmDataDIR.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cdiroutl"
#pragma resource "*.dfm"
TFormDataDIR *FormDataDIR;
//---------------------------------------------------------------------------
__fastcall TFormDataDIR::TFormDataDIR(TComponent* Owner)
    : TForm(Owner)
{
}

//---------------------------------------------------------------------------
void __fastcall TFormDataDIR::Panel1Resize(TObject *Sender)
{
    EditDataDIR->Top  = 0;
    EditDataDIR->Left = 0;
    EditDataDIR->Width = Panel1->Width;
    ComboBoxLANG->Width = Panel2->Width;
}

#define ZONELIST_STB_FILE   "\\3DDATA\\STB\\LIST_ZONE.STB"

//---------------------------------------------------------------------------
void __fastcall TFormDataDIR::FormCreate(TObject *Sender)
{
    ComboBoxLANG->ItemIndex = 0;

    BCBUtil::LoadControl( FormMAIN->m_stINIFile.c_str(), this );

    // 입력되어 있던 경로로 폴더를 바꿔서
    if ( EditDataDIR->Text.Length() ) {
        //SetCurrentDir( EditDataDIR->Text );
        DirectoryListBox1->Directory = EditDataDIR->Text;
    } else
        EditDataDIR->Text = DirectoryListBox1->Directory;

    this->Start( true );
}

//---------------------------------------------------------------------------
void TFormDataDIR::Start (bool bAutoStart)
{
    String stFullPath = DirectoryListBox1->Directory + ZONELIST_STB_FILE;
    if ( FileExists( stFullPath ) ) {
        Timer1->Enabled = bAutoStart;
    } else
        Timer1->Enabled = false;

    ProgressBar1->Position = 0;
    ProgressBar1->Max = EditWaitTIME->Text.ToInt();
    if ( ProgressBar1->Max < 1 )
        ProgressBar1->Max = 1;
        
    this->Show ();
}

//---------------------------------------------------------------------------
void __fastcall TFormDataDIR::ButtonOKClick(TObject *Sender)
{
    StopTimer( Sender );
    this->Close();

    String stFullPath = DirectoryListBox1->Directory + ZONELIST_STB_FILE;
    if ( FileExists( stFullPath ) ) {
        FormMAIN->m_iLangTYPE = ComboBoxLANG->ItemIndex;
        FormMAIN->m_stDataDir = EditDataDIR->Text;
        if ( FormMAIN->Go_NextStep () ) {
            BCBUtil::SaveControl( FormMAIN->m_stINIFile.c_str(), this );
        } else
            this->Show ();
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormDataDIR::DirectoryListBox1Change(TObject *Sender)
{
    EditDataDIR->Text = DirectoryListBox1->Directory;
}

//---------------------------------------------------------------------------
void __fastcall TFormDataDIR::DirectoryListBox1Click(TObject *Sender)
{
    StopTimer( Sender );
    DirectoryListBox1->OpenCurrent();
}

//---------------------------------------------------------------------------
void __fastcall TFormDataDIR::Timer1Timer(TObject *Sender)
{
    if ( ProgressBar1->Position >= ProgressBar1->Max ) {
        this->ButtonOKClick( Sender );
        return;
    }
    ProgressBar1->Position ++;
}

//---------------------------------------------------------------------------
void __fastcall TFormDataDIR::StopTimer(TObject *Sender)
{
    if ( Sender == Timer1 ) {
        FormMAIN->m_bAutoStart = true;
    }

    Timer1->Enabled = false;
    ProgressBar1->Position = 0;
}
//---------------------------------------------------------------------------

void __fastcall TFormDataDIR::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    StopTimer( NULL );
}

//---------------------------------------------------------------------------
void __fastcall TFormDataDIR::ComboBoxLANGClick(TObject *Sender)
{
    StopTimer( Sender );
}

//---------------------------------------------------------------------------
void __fastcall TFormDataDIR::ComboBoxLANGDropDown(TObject *Sender)
{
    StopTimer( Sender );

}

//---------------------------------------------------------------------------

