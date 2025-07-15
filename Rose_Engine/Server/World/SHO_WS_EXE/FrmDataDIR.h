//---------------------------------------------------------------------------

#ifndef FrmDataDIRH
#define FrmDataDIRH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "cdiroutl.h"
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Outline.hpp>
#include <FileCtrl.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TFormDataDIR : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TEdit *EditDataDIR;
    TButton *ButtonOK;
    TDirectoryListBox *DirectoryListBox1;
    TTimer *Timer1;
    TProgressBar *ProgressBar1;
    TEdit *EditWaitTIME;
    TLabel *Label1;
    TPanel *Panel2;
    TComboBox *ComboBoxLANG;
    void __fastcall Panel1Resize(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall ButtonOKClick(TObject *Sender);
    void __fastcall DirectoryListBox1Change(TObject *Sender);
    void __fastcall DirectoryListBox1Click(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall StopTimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ComboBoxLANGClick(TObject *Sender);
    void __fastcall ComboBoxLANGDrawItem(TWinControl *Control, int Index,
          TRect &Rect, TOwnerDrawState State);
private:	// User declarations
public:		// User declarations
    __fastcall TFormDataDIR(TComponent* Owner);

    void Start (bool bAutoStart);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormDataDIR *FormDataDIR;
//---------------------------------------------------------------------------
#endif
