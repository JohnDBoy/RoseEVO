//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "BCB_Tools.h"
#include "FrmMAIN.h"
#include "LS_ThreadVCL.h"
#pragma package(smart_init)


void CVclLogSTR::Proc ()
{
    if ( FormMAIN->MemoLOG->Lines->Count > 1000 ) {
        FormMAIN->MemoLOG->Lines->Delete ( 0 );
    }

    FormMAIN->MemoLOG->Lines->Add( this->m_String );
//    OutputDebugString ( szString );
}
void CVclSetITEM::Proc ()
{
    m_pListItem->SubItems->Strings[ m_nSubItemIDX ] = m_stValue;
}
void CVclDelListITEM::Proc ()
{
    BCBUtil::DelListITEM( m_pListView, m_pListItem );
}


void CVclAddConnectorITEM::Proc ()
{
    m_pListItem->Caption = "unknown";
    m_pListItem->SubItems->Add( m_stIP );
}
void CVclAddWorldITEM::Proc ()
{
    TLVItem LvITEM;

    LvITEM.mask     = LVIF_PARAM | LVIF_IMAGE;
    LvITEM.iSubItem = 0;
    LvITEM.iImage   = I_IMAGECALLBACK;
    LvITEM.iItem    = FormMAIN->ListViewSERVER->Items->Count;
    LvITEM.lParam   = (LPARAM)m_pListItem;
    ::SendMessage( FormMAIN->ListViewSERVER->Handle, LVM_INSERTITEM, 0, (LPARAM)&LvITEM );


    m_pListItem->Caption = m_stWorld;
    m_pListItem->SubItems->Add( m_stIP );
    m_pListItem->SubItems->Add( IntToStr( m_iPort ) );
    m_pListItem->SubItems->Add( "0" );
    m_pListItem->SubItems->Add( "0" );

    SYSTEMTIME  sysTIME;
    GetLocalTime (&sysTIME);
    String stDate;
    stDate.printf ("%d/%d [%d:%d:%d]", sysTIME.wMonth, sysTIME.wDay, sysTIME.wHour, sysTIME.wMinute, sysTIME.wSecond);
    m_pListItem->SubItems->Add ( stDate );

    m_pListItem->SubItems->Add( IntToStr(m_dwRight) );
}
void CVclAddBlockITEM::Proc ()
{
    m_pListItem->Caption = m_stIP;
    m_pListItem->SubItems->Add( "unknown" );
}

//---------------------------------------------------------------------------

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

__fastcall LS_ThreadVCL::LS_ThreadVCL(bool CreateSuspended)
    : TThread(CreateSuspended)
{
    m_csDATA = new TCriticalSection;//( 4000 );
    Priority = tpLower;
    
#ifdef  __USE_API_EVENT
    m_hEVENT = ::CreateEvent (NULL, FALSE, FALSE, "LSVCL_EVT" );
#else
    m_pEvent = new TEvent(NULL, false, false, NULL );//"LSVCL_EVT");
#endif
}
__fastcall LS_ThreadVCL::~LS_ThreadVCL()
{
#ifdef  __USE_API_EVENT
    ::CloseHandle( m_hEVENT );
#else
    delete m_pEvent;
#endif
    delete m_csDATA;
}
//---------------------------------------------------------------------------
void __fastcall LS_ThreadVCL::Execute()
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
        // 순서 주의 !!!
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

    m_pDataNODE = m_ListWAIT.GetHeadNode();
    while( m_pDataNODE ) {
        delete m_pDataNODE->DATA;
        m_ListWAIT.DeleteNFree( m_pDataNODE );
        m_pDataNODE = m_ListWAIT.GetHeadNode ();
    }
}

//---------------------------------------------------------------------------
void __fastcall LS_ThreadVCL::DrawToVCL()
{
    m_pDataNODE->DATA->Proc ();
}

//---------------------------------------------------------------------------
void __fastcall LS_ThreadVCL::Free (void)
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

