//---------------------------------------------------------------------------

#ifndef FrmABOUTH
#define FrmABOUTH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TFormABOUT : public TForm
{
__published:	// IDE-managed Components
    TButton *Button1;
    TMemo *Memo1;
    void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TFormABOUT(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormABOUT *FormABOUT;
//---------------------------------------------------------------------------
#endif
