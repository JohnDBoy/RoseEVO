//---------------------------------------------------------------------------

#ifndef FrmMAINH
#define FrmMAINH
//---------------------------------------------------------------------------
#include "cdiroutl.h"
#include "classLOG.h"
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <StdCtrls.hpp>


#include "PacketHeader.h"
#include "WM_SHO.h"

//---------------------------------------------------------------------------
class TFormMAIN : public TForm
{
__published:	// IDE-managed Components
    TMemo *MemoLOG;
    TMainMenu *MainMenu1;
    TMenuItem *System1;
    TMenuItem *Option1;
    TMenuItem *SM_Start1;
    TMenuItem *SM_Shutdown1;
    TMenuItem *N1;
    TMenuItem *SM_Exit1;
    TStatusBar *StatusBar1;
    TPageControl *PageControl1;
    TTabSheet *TabSheet1;
    TMenuItem *Help1;
    TMenuItem *About1;
    TListView *ListViewZONE;
    TSplitter *Splitter1;
    TMenuItem *Announcechat1;
    TMenuItem *N2;
    TMenuItem *Setlogmode1;
    TImageList *ImageList1;
    TMenuItem *Config1;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall SM_Start1Click(TObject *Sender);
    void __fastcall SM_Shutdown1Click(TObject *Sender);
    void __fastcall About1Click(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall Announcechat1Click(TObject *Sender);
    void __fastcall Setlogmode1Click(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall Config1Click(TObject *Sender);
	void __fastcall SM_Exit1Click(TObject *Sender);
private:	// User declarations

public:		// User declarations
    String  m_stINIFile;
    bool    m_bAutoStart;

    BYTE    m_btStep;
    String  m_stDataDir;
    int     m_iLangTYPE;

    String  m_stDBServerIP;
    String  m_stDBName;
    String  m_stDBUser;
    String  m_stDBPW;

    String  m_stLogUser;
    String  m_stLogPW;

    String  m_stMallUser;
    String  m_stMallPW;

    String  m_stWorldServerIP;
    int     m_iWorldPort;

    String  m_stAccountServerIP;
    int     m_iAccountPort;

    String  m_stWorldName;
    String  m_stClientListenIP;
    int     m_iClientListenPort;

    int     m_iChannelNO;
    int     m_iLowAge;
    int     m_iHighAge;

    int     m_iUserCNT;
    int     m_iUserLIMIT;
    
    bool Go_NextStep();
    
    void InitZone ();
    void SendAnnounceChat(String stMsg);
    WORD GetLogMODE (t_LOG logType);
	void SetLogMODE (WORD wLogMODE, t_LOG logType=LOG_SCR);

    HWND    m_hMonSRV;
    UINT    m_uiProcMSG;

    void PostMessage2MON (UINT uiMSG, DWORD dwUserCNT);

    __fastcall TFormMAIN(TComponent* Owner);
    
    void __fastcall AppException(TObject *Sender, Exception *E);
    void __fastcall AppMessage(tagMSG &Msg, bool &Handled);

    void  __fastcall On_WM_SHO_REQ_STATUS(TMessage &Message);
    void  __fastcall On_WM_COPYDATA      (TMessage &Message);

BEGIN_MESSAGE_MAP
    VCL_MESSAGE_HANDLER (WM_SHO_REQ_STATUS, TMessage, On_WM_SHO_REQ_STATUS)
    VCL_MESSAGE_HANDLER (WM_COPYDATA,       TMessage, On_WM_COPYDATA)
END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMAIN *FormMAIN;
//---------------------------------------------------------------------------
#endif
