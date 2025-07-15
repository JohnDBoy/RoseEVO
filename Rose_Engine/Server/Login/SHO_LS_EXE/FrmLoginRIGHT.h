//---------------------------------------------------------------------------

#ifndef FrmLoginRIGHTH
#define FrmLoginRIGHTH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TFormLoginRIGHT : public TForm
{
__published:	// IDE-managed Components
    TEdit *Edit1;
    TButton *Button1;
    void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TFormLoginRIGHT(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormLoginRIGHT *FormLoginRIGHT;
//---------------------------------------------------------------------------
#endif
