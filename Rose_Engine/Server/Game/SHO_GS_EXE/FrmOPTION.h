//---------------------------------------------------------------------------

#ifndef FrmOPTIONH
#define FrmOPTIONH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TFormOPTION : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label5;
    TEdit *EditWorldNAME;
    TEdit *EditLowAGE;
    TEdit *EditHighAGE;
    TLabel *Label3;
    TLabel *Label8;
    TEdit *EditMaxUSER;
    TEdit *EditChannelNO;
    TLabel *Label4;
    TLabel *Label2;
private:	// User declarations
public:		// User declarations
    __fastcall TFormOPTION(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormOPTION *FormOPTION;
//---------------------------------------------------------------------------
#endif
