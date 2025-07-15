//---------------------------------------------------------------------------

#include "PreHEADER.h"

#include "FrmANNOUNCE.h"
#include "FrmMAIN.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormAnnounce *FormAnnounce;
//---------------------------------------------------------------------------
__fastcall TFormAnnounce::TFormAnnounce(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormAnnounce::ButtonOKClick(TObject *Sender)
{
    FormMAIN->SendAnnounceChat( EditChat->Text );
}

//---------------------------------------------------------------------------
void __fastcall TFormAnnounce::ButtonCANCELClick(TObject *Sender)
{
    this->Close ();
}

//---------------------------------------------------------------------------

