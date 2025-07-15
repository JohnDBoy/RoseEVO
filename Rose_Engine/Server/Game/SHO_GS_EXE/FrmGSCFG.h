//---------------------------------------------------------------------------

#ifndef FrmGSCFGH
#define FrmGSCFGH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TFormGSCFG : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TLabel *Label5;
    TEdit *EditWorldNAME;
    TLabel *Label6;
    TEdit *EditClientPORT;
    TListView *ListViewZONE;
    TPanel *Panel1;
    TButton *ButtonOK;
    TButton *ButtonRESET;
    TTimer *Timer1;
    TLabel *Label7;
    TEdit *EditWaitTIME;
    TProgressBar *ProgressBar1;
    TPopupMenu *PopupMenu1;
    TMenuItem *Selectall1;
    TMenuItem *Reset1;
    TMenuItem *N1;
    TMenuItem *Inverselyselect1;
    TMenuItem *Select1;
    TMenuItem *Uncheck1;
    TLabel *Label1;
    TEdit *EditClientIP;
    TEdit *EditLowAGE;
    TLabel *Label2;
    TEdit *EditHighAGE;
    TLabel *Label3;
    TEdit *EditChannelNO;
    TLabel *Label4;
    TLabel *Label8;
    TEdit *EditMaxUSER;
    void __fastcall ButtonOKClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall ButtonRESETClick(TObject *Sender);
    void __fastcall StopTimer(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall Selectall1Click(TObject *Sender);
    void __fastcall Reset1Click(TObject *Sender);
    void __fastcall Inverselyselect1Click(TObject *Sender);
    void __fastcall Select1Click(TObject *Sender);
    void __fastcall Uncheck1Click(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
private:	// User declarations
    bool    m_bFirst;

public:		// User declarations
    __fastcall TFormGSCFG(TComponent* Owner);
    void Start( bool bAutoStart );

};
//---------------------------------------------------------------------------
extern PACKAGE TFormGSCFG *FormGSCFG;
//---------------------------------------------------------------------------
#endif
