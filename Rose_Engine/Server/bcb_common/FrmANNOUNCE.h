//---------------------------------------------------------------------------

#ifndef FrmANNOUNCEH
#define FrmANNOUNCEH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TFormAnnounce : public TForm
{
__published:	// IDE-managed Components
    TButton *ButtonOK;
    TButton *ButtonCANCEL;
    TEdit *EditChat;
    void __fastcall ButtonOKClick(TObject *Sender);
    void __fastcall ButtonCANCELClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TFormAnnounce(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormAnnounce *FormAnnounce;
//---------------------------------------------------------------------------
#endif
