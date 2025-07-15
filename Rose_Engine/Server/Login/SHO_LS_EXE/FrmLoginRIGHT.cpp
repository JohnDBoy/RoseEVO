//---------------------------------------------------------------------------

#include "PreHEADER.h"

#include "FrmLoginRIGHT.h"
#include "FrmMAIN.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormLoginRIGHT *FormLoginRIGHT;
//---------------------------------------------------------------------------
__fastcall TFormLoginRIGHT::TFormLoginRIGHT(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormLoginRIGHT::Button1Click(TObject *Sender)
{
    FormMAIN->SetLoginRIGHT( Edit1->Text.ToInt() );
    Close ();
}
//---------------------------------------------------------------------------
