//---------------------------------------------------------------------------

#ifndef FrmLogMODEH
#define FrmLogMODEH
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
class TFormLogMODE : public TForm
{
__published:	// IDE-managed Components
    TRadioGroup *RadioGroupSCR;
    TButton *ButtonOK;
    TButton *ButtonCANCEL;
    TRadioGroup *RadioGroupFILE;
    void __fastcall ButtonOKClick(TObject *Sender);
    void __fastcall ButtonCANCELClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    
private:	// User declarations
    void __fastcall Set_LogMode ();
    void __fastcall Set_RadioButton ();

public:		// User declarations
    __fastcall TFormLogMODE(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormLogMODE *FormLogMODE;

//---------------------------------------------------------------------------
#endif
