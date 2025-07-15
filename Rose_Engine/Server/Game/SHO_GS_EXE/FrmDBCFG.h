//---------------------------------------------------------------------------

#ifndef FrmDBCFGH
#define FrmDBCFGH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormDBCFG : public TForm
{
__published:	// IDE-managed Components
    TProgressBar *ProgressBar1;
    TLabel *Label5;
    TEdit *EditWaitTIME;
    TButton *ButtonOK;
    TTimer *Timer1;
    TPanel *Panel1;
    TGroupBox *GroupBox1;
    TLabel *Label4;
    TEdit *EditDBPassword;
    TEdit *EditDBUser;
    TLabel *Label3;
    TLabel *Label2;
    TEdit *EditDBName;
    TEdit *EditDBIP;
    TLabel *Label1;
    TGroupBox *GroupBox2;
    TLabel *Label6;
    TEdit *EditWorldServerIP;
    TLabel *Label8;
    TEdit *EditLoginWorldPORT;
    TGroupBox *GroupBox3;
    TLabel *Label7;
    TEdit *EditAccountServerIP;
    TLabel *Label9;
    TEdit *EditAccountServerPORT;
    TLabel *Label10;
    TLabel *Label11;
    TEdit *EditLogPassword;
    TEdit *EditLogUser;
    TLabel *Label12;
    TLabel *Label13;
    TEdit *EditMallUser;
    TEdit *EditMallPW;
    void __fastcall ButtonOKClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall StopTimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
    __fastcall TFormDBCFG(TComponent* Owner);

    void Start( bool bAutoStart );
};
//---------------------------------------------------------------------------
extern PACKAGE TFormDBCFG *FormDBCFG;
//---------------------------------------------------------------------------
#endif
