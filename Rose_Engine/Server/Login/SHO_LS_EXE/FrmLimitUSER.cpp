//---------------------------------------------------------------------------

#include "PreHEADER.h"

#include "FrmLimitUSER.h"
#include "FrmMAIN.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormLimitUSER *FormLimitUSER;
//---------------------------------------------------------------------------
__fastcall TFormLimitUSER::TFormLimitUSER(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormLimitUSER::Button1Click(TObject *Sender)
{
    FormMAIN->SetLimitUserCNT( Edit1->Text.ToInt() );
    Close ();    
}
//---------------------------------------------------------------------------
