//---------------------------------------------------------------------------

#ifndef WS_ThreadVCLH
#define WS_ThreadVCLH
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

class CVclAddITEM : public CVclDATA
{
public :
    TListItem *m_pListItem;
    short   m_nZoneNO;
    String  m_stZone;
    String  m_stIP;
    int     m_iPortNO;

    void Proc ();
} ;

class CVclDelITEM : public CVclDATA
{
public :
    TListItem *m_pListItem;
    void Proc ();
} ;
class CVclSetITEM : public CVclDATA
{
public :
    TListItem *m_pListItem;
    short   m_nSubItemIDX;
    int     m_iValue;
    void Proc ();
} ;


#define __USE_API_EVENT
class WS_ThreadVCL : public TThread
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
    __fastcall WS_ThreadVCL(bool CreateSuspended);
    __fastcall ~WS_ThreadVCL();
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

extern WS_ThreadVCL *g_pThreadVCL;

//---------------------------------------------------------------------------
#endif
