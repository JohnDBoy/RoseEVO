//---------------------------------------------------------------------------

#ifndef LS_ThreadVCLH
#define LS_ThreadVCLH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <SyncObjs.hpp>
#include "DLLIST.h"
#include "classSYNCOBJ.h"
//---------------------------------------------------------------------------

class CVclDATA
{
public :
    virtual void Proc() = 0;
} ;

class CVclLogSTR : public CVclDATA
{
public :
    String  m_String;
    void Proc ();
} ;
class CVclSetITEM : public CVclDATA
{
public :
    TListItem *m_pListItem;
    short       m_nSubItemIDX;
    String      m_stValue;
    void Proc ();
} ;
class CVclDelListITEM : public CVclDATA
{
public :
    TListView *m_pListView;
    TListItem *m_pListItem;
    void Proc ();
} ;


class CVclAddConnectorITEM : public CVclDATA
{
public :
    TListItem *m_pListItem;
    String     m_stIP;
    void Proc ();
} ;

class CVclAddWorldITEM : public CVclDATA
{
public :
    TListItem      *m_pListItem;
    String          m_stWorld;
    String          m_stIP;
    int             m_iPort;
    unsigned int    m_dwRight;
    void Proc ();
} ;

class CVclAddBlockITEM : public CVclDATA
{
public :
    TListItem      *m_pListItem;
    String          m_stIP;
    unsigned int    m_dwEndTime;
    void Proc ();
} ;


//---------------------------------------------------------------------------
#define __USE_API_EVENT

class LS_ThreadVCL : public TThread
{
private:
#ifdef  __USE_API_EVENT
    HANDLE       m_hEVENT;
#else
    TEvent      *m_pEvent;
#endif

    TCriticalSection           *m_csDATA;
    classDLLIST< CVclDATA* >    m_ListWAIT;
    classDLLIST< CVclDATA* >    m_ListPROC;

    classDLLNODE< CVclDATA* >  *m_pDataNODE;

protected:
    void __fastcall DrawToVCL();
    void __fastcall Execute();

public:
    __fastcall LS_ThreadVCL(bool CreateSuspended);
    __fastcall ~LS_ThreadVCL();
    void __fastcall Free (void);

    void AddDATA( CVclDATA* pDATA )
    {
        m_csDATA->Enter ();
        m_ListWAIT.AllocNAppend( pDATA );
        m_csDATA->Leave ();
        
#ifdef  __USE_API_EVENT
        ::SetEvent( m_hEVENT );
#else
        m_pEvent->SetEvent ();
#endif
    }
};

extern LS_ThreadVCL *g_pThreadVCL;

//---------------------------------------------------------------------------
#endif
 