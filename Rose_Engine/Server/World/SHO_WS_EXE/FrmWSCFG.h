//---------------------------------------------------------------------------

#ifndef FrmWSCFGH
#define FrmWSCFGH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormWSCFG : public TForm
{
__published:	// IDE-managed Components
    TEdit *EditZoneListenPORT;
    TLabel *Label6;
    TPanel *Panel1;
    TLabel *Label8;
    TProgressBar *ProgressBar1;
    TEdit *EditWaitTIME;
    TButton *ButtonOK;
    TTimer *Timer1;
    TEdit *EditLoginServerIP;
    TEdit *EditLoginServerPORT;
    TLabel *Label4;
    TLabel *Label5;
    TGroupBox *GroupBoxEX;
        TLabel *Label3;
        TLabel *Label11;
        TLabel *Label7;
        TEdit *EditUserListenPORT;
    TEdit *EditExtIP;
        TEdit *EditWorldNAME;
    TGroupBox *GroupBoxDB;
        TLabel *Label1;
        TEdit *EditDBServerIP;
        TEdit *EditDBTableNAME;
        TLabel *Label2;
        TLabel *Label12;
        TEdit *EditDBAccount;
        TLabel *Label13;
        TEdit *EditDBPW;
    TLabel *Label10;
    TLabel *Label9;
    TEdit *EditLogAccount;
    TEdit *EditLogPW;
    TCheckBox *CheckBoxCreateCHAR;
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall StopTimer(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall ButtonOKClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TFormWSCFG(TComponent* Owner);

    void TFormWSCFG::Start( bool bAutoStart, int iStepNO );
};
//---------------------------------------------------------------------------
extern PACKAGE TFormWSCFG *FormWSCFG;
//---------------------------------------------------------------------------
#endif
