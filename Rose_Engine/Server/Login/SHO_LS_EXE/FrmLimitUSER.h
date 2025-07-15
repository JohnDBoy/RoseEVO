//---------------------------------------------------------------------------

#ifndef FrmLimitUSERH
#define FrmLimitUSERH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TFormLimitUSER : public TForm
{
__published:	// IDE-managed Components
    TEdit *Edit1;
    TButton *Button1;
    void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TFormLimitUSER(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormLimitUSER *FormLimitUSER;
//---------------------------------------------------------------------------
#endif
