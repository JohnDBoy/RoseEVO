//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "BCB_Tools.h"
#include "FrmMAIN.h"
#include "WS_ThreadVCL.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

void CVclLogSTR::Proc ()
{
    if ( FormMAIN->MemoLOG->Lines->Count > 1000 ) {
        FormMAIN->MemoLOG->Lines->Delete ( 0 );
    }

    FormMAIN->MemoLOG->Lines->Add( this->m_String );
//    OutputDebugString ( szString );
}
void CVclAddITEM::Proc ()
{
    m_pListItem->Caption = IntToStr( m_nZoneNO );

    m_pListItem->SubItems->Add ( m_stZone );
    m_pListItem->SubItems->Add ( m_stIP );
    m_pListItem->SubItems->Add ( IntToStr( m_iPortNO ) );
    m_pListItem->SubItems->Add ( "0" );
    m_pListItem->SubItems->Add ( "0" );

    SYSTEMTIME  sysTIME;
    GetLocalTime (&sysTIME);

    String stDate;
    stDate.printf ("%d/%d [%d:%d:%d]", sysTIME.wMonth, sysTIME.wDay, sysTIME.wHour, sysTIME.wMinute, sysTIME.wSecond);
    m_pListItem->SubItems->Add ( stDate );
}
void CVclDelITEM::Proc ()
{
    BCBUtil::DelListITEM( FormMAIN->ListViewZONE, m_pListItem );
}
void CVclSetITEM::Proc ()
{
    m_pListItem->SubItems->Strings[ m_nSubItemIDX ] = IntToStr( m_iValue );
}


//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall Unit1::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------
__fastcall WS_ThreadVCL::WS_ThreadVCL(bool CreateSuspended)
    : TThread(CreateSuspended)  // , m_csDATA( 4000 )
{
/*
tpIdle	The thread executes only when the system is idle. Windows won't interrupt other threads to execute a thread with tpIdle priority.
tpLowest	The thread's priority is two points below normal.
tpLower	The thread's priority is one point below normal.
tpNormal	The thread has normal priority.
tpHigher	The thread's priority is one point above normal.
tpHighest	The thread's priority is two points above normal.
tpTimeCritical	The thread gets highest priority.
Warning:		Boosting the thread priority of a CPU intensive operation may “starve” other threads in the application. Only apply priority boosts to threads that spend most of their time waiting for external events.
*/
    m_csDATA = new TCriticalSection;//( 4000 );
    Priority = tpLower;
#ifdef  __USE_API_EVENT
    m_hEVENT = ::CreateEvent (NULL, FALSE, FALSE, NULL );
#else
    m_pEvent = new TEvent(NULL, false, false, "WSVCL_EVT");
#endif
}
__fastcall WS_ThreadVCL::~WS_ThreadVCL()
{
#ifdef  __USE_API_EVENT
    ::CloseHandle( m_hEVENT );
#else
    delete m_pEvent;
#endif
    delete m_csDATA;
}

//---------------------------------------------------------------------------
void __fastcall WS_ThreadVCL::Execute()
{
    while( TRUE ) {
#ifdef  __USE_API_EVENT
        if ( WAIT_OBJECT_0 != ::WaitForSingleObject( m_hEVENT, INFINITE ) )
            break;
#else
        if ( m_pEvent->WaitFor( INFINITE ) != wrSignaled )
            break;
        m_pEvent->ResetEvent();
#endif
        // ¼ø¼­ ÁÖÀÇ !!!
        if ( this->Terminated )
            break;

        m_csDATA->Enter ();
		m_ListPROC.AppendNodeList( &m_ListWAIT );
		m_ListWAIT.Init ();
        m_csDATA->Leave ();

        m_pDataNODE = m_ListPROC.GetHeadNode();
        while( m_pDataNODE ) {

            Synchronize( DrawToVCL );

			delete m_pDataNODE->DATA;
			m_ListPROC.DeleteNFree( m_pDataNODE );

            m_pDataNODE = m_ListPROC.GetHeadNode ();
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall WS_ThreadVCL::DrawToVCL()
{
    m_pDataNODE->DATA->Proc ();
}

//---------------------------------------------------------------------------
void __fastcall WS_ThreadVCL::Free (void)
{
    this->Terminate ();

#ifdef  __USE_API_EVENT
    ::SetEvent( m_hEVENT );
#else
    this->m_pEvent->SetEvent ();
#endif

    this->WaitFor ();
}

//---------------------------------------------------------------------------

