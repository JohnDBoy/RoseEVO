//---------------------------------------------------------------------------
#ifndef FrmLSCFGH
#define FrmLSCFGH
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
class TFormLSCFG : public TForm
{
__published:	// IDE-managed Components
    TStaticText *StaticTextDB;
    TStaticText *StaticTextCLIENT;
    TStaticText *StaticTextSERVER;
    TPanel *Panel1;
    TLabel *Label5;
    TProgressBar *ProgressBar1;
    TEdit *EditWaitTIME;
    TButton *ButtonOK;
    TEdit *EditDBIP;
    TEdit *EditClientPORT;
    TEdit *EditServerPORT;
    TTimer *Timer1;
    TStaticText *StaticTextRIGHT;
    TEdit *EditLoginRIGHT;
    TEdit *EditLimitUserCNT;
    TLabel *Label1;
    TGroupBox *GroupBox1;
    TStaticText *StaticTextGumsIP;
    TEdit *EditGumsIP;
    TEdit *EditGumsPORT;
    TStaticText *StaticTextGumsPORT;
    TCheckBox *CheckBoxGUMS;
    TCheckBox *CheckBoxWS;
    TGroupBox *GroupBox2;
    TEdit *EditPW;
    TButton *ButtonPW;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall ButtonOKClick(TObject *Sender);
    void __fastcall StopTimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall CheckBoxGUMSClick(TObject *Sender);
    void __fastcall ButtonPWClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    union {
        BYTE    m_btMD5[ 33 ];
	    DWORD	m_dwMD5[ 8 ];
    } ;

    __fastcall TFormLSCFG(TComponent* Owner);

    void Start (bool bAutoStart, bool bFirst);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormLSCFG *FormLSCFG;
//---------------------------------------------------------------------------
#endif
