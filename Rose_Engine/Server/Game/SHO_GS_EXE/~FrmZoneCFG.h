//---------------------------------------------------------------------------

#ifndef FrmZoneCFGH
#define FrmZoneCFGH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <CheckLst.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormZoneCFG : public TForm
{
__published:	// IDE-managed Components
    TCheckListBox *CheckListBoxZONE;
    TPanel *Panel1;
    TButton *ButtonCheck;
    TButton *ButtonOk;
    TButton *ButtonCancel;
    TButton *ButtonUncheck;
    void __fastcall ButtonCheckClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TFormZoneCFG(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormZoneCFG *FormZoneCFG;
//---------------------------------------------------------------------------
#endif
