//---------------------------------------------------------------------------

#ifndef FrmMAINH
#define FrmMAINH
//---------------------------------------------------------------------------
#include "classLOG.h"
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <StdCtrls.hpp>
#include <ToolWin.hpp>

#include "PacketHeader.h"
#include "WM_SHO.h"

//---------------------------------------------------------------------------
class TFormMAIN : public TForm
{
__published:	// IDE-managed Components
    TImageList *ImageListToolBAR;
    TMemo *MemoLOG;
    TSplitter *Splitter1;
    TPageControl *PageControl1;
    TTabSheet *TabSheetZONE;
    TListView *ListViewCHANNEL;
    TStatusBar *StatusBar1;
    TMainMenu *MainMenu1;
    TMenuItem *System1;
    TMenuItem *Option1;
    TMenuItem *Sendannouncechat1;
    TMenuItem *Start1;
    TMenuItem *Shutdown1;
    TMenuItem *N1;
    TMenuItem *Exit1;
    TToolBar *ToolBar1;
    TToolButton *ToolButtonSTEP0;
    TToolButton *ToolButtonSTEP1;
    TToolButton *ToolButtonSTEP2;
    TToolButton *ToolButtonSTEP3;
    TToolButton *ToolButton3;
    TToolButton *ToolButtonABOUT;
    TToolButton *ToolButton1;
    TMenuItem *Help1;
    TMenuItem *About1;
    TMenuItem *Changeserver1;
    TMenuItem *N2;
    TMenuItem *Setlogmode1;
    TTabSheet *TabSheet1;
    TListView *ListViewUSER;
    TPopupMenu *PopupMenuCHANNEL;
    TMenuItem *ToggleActive1;
    TMenuItem *N3;
    TMenuItem *DisconnectChannelServer1;
    TMenuItem *N4;
    void __fastcall ToolButtonABOUTClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ToolButtonSTEP0Click(TObject *Sender);
    void __fastcall ToolButtonSTEP1Click(TObject *Sender);
    void __fastcall ToolButtonSTEP2Click(TObject *Sender);
    void __fastcall ToolButtonSTEP3Click(TObject *Sender);
    void __fastcall About1Click(TObject *Sender);
    void __fastcall Setlogmode1Click(TObject *Sender);
    void __fastcall Sendannouncechat1Click(TObject *Sender);
    void __fastcall ListViewUSERColumnClick(TObject *Sender,
          TListColumn *Column);
    void __fastcall Shutdown1Click(TObject *Sender);
    void __fastcall ToggleActive1Click(TObject *Sender);
    void __fastcall DisconnectChannelServer1Click(TObject *Sender);
	void __fastcall Exit1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TFormMAIN(TComponent* Owner);

    bool   m_bAutoStart;
    String m_stINIFile;
    short  m_nProcSTEP;

    String  m_stDataDir;
    int     m_iLangTYPE;

    String  m_stDBServerIP;
    String  m_stDBName;
    String  m_stDBUser;
    String  m_stDBPw;

    String  m_stLogDBName;
    String  m_stLogDBPw;

    String  m_stLoginServerIP;
    int     m_iLoginServerPORT;

    String  m_stLogServerIP;
    int     m_iLogServerPORT;

    String  m_stWorldName;
    int     m_iZoneListenPortNO;
    int     m_iUserListenPortNO;

    bool    m_bBlockCreateCHAR;

    HWND    m_hMonSRV;
    int     m_iUserCNT;
    UINT    m_uiProcMSG;

    void ToggleBUTTON (TToolButton *pToolBTN, bool bOn);

    bool Next_STEP ();

    void SendAnnounceChat(String stAnnounceMSG);
    WORD GetLogMODE (t_LOG logType);
	void SetLogMODE (WORD wLogMODE, t_LOG logType=LOG_SCR);

    void PostMessage2MON (UINT uiMSG, WORD wUserCNT);

    void __fastcall AppMessage(tagMSG &Msg, bool &Handled);

    void __fastcall On_WM_SHO_REQ_STATUS(TMessage &Message);
    void __fastcall On_WM_COPYDATA      (TMessage &Message);

BEGIN_MESSAGE_MAP
    VCL_MESSAGE_HANDLER (WM_SHO_REQ_STATUS, TMessage, On_WM_SHO_REQ_STATUS)
    VCL_MESSAGE_HANDLER (WM_COPYDATA,       TMessage, On_WM_COPYDATA)
END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMAIN *FormMAIN;
//---------------------------------------------------------------------------
#endif
