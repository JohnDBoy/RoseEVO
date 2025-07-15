//---------------------------------------------------------------------------

#include "PreHEADER.h"

#include "CVclTHREAD.h"
#pragma package(smart_init)


CVclTHREAD* CVclTHREAD::m_pInstance=NULL;

//---------------------------------------------------------------------------
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

__fastcall CVclTHREAD::CVclTHREAD(bool CreateSuspended)
    : TThread(CreateSuspended)
{
    m_csDATA = new TCriticalSection;//( 4000 );
//  Priority = tpLower;
    Priority = tpNormal;
    
#ifdef  __USE_API_EVENT
    m_pEvent = new classEVENT( NULL, FALSE, FALSE, NULL );
#else
    m_pEvent = new TEvent(NULL, false, false, NULL );//"LSVCL_EVT");
#endif
}

__fastcall CVclTHREAD::~CVclTHREAD()
{
    delete m_pEvent;
    delete m_csDATA;
}

//---------------------------------------------------------------------------
void __fastcall CVclTHREAD::Execute()
{
    while( TRUE ) {
#ifdef  __USE_API_EVENT
        if (m_pEvent->WaitFor( INFINITE ) != WAIT_OBJECT_0 )
            break;
#else
        if ( m_pEvent->WaitFor( INFINITE ) != wrSignaled )
            break;
#endif
        m_pEvent->ResetEvent();
        // 순서 주의 !!!

        if ( this->Terminated )
			break;

		this->Synchronize( CVclTHREAD::DrawToVCL );
	}

	m_pDataNODE = m_ListWAIT.GetHeadNode();
	while( m_pDataNODE ) {
		delete m_pDataNODE->DATA;
		m_ListWAIT.DeleteNFree( m_pDataNODE );
		m_pDataNODE = m_ListWAIT.GetHeadNode ();
	}
}

//---------------------------------------------------------------------------
void __fastcall CVclTHREAD::DrawToVCL()
{
    m_csDATA->Enter ();
        m_ListPROC.AppendNodeList( &m_ListWAIT );
        m_ListWAIT.Init ();
    m_csDATA->Leave ();

    m_pDataNODE = m_ListPROC.GetHeadNode();
    while( m_pDataNODE ) {
        m_pDataNODE->DATA->Proc ();
        delete m_pDataNODE->DATA;
        m_ListPROC.DeleteNFree( m_pDataNODE );

        m_pDataNODE = m_ListPROC.GetHeadNode ();
    }
}

//---------------------------------------------------------------------------
void __fastcall CVclTHREAD::Free (void)
{
    this->Terminate ();
    this->m_pEvent->SetEvent ();
    this->WaitFor ();
}

//---------------------------------------------------------------------------


