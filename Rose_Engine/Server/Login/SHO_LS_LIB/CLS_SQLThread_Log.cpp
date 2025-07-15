
#include "stdAFX.h"
#include "blockLIST.h"

#include "CLS_Account.h"
#include "CLS_Server.h"
#include "CLS_Client.h"
#include "CLS_SqlTHREAD_Log.h"
#include "CAS_GUMS.h"

IMPLEMENT_INSTANCE( CLS_SqlTHREAD_Log )

//-------------------------------------------------------------------------------------------------
CLS_SqlTHREAD_Log::CLS_SqlTHREAD_Log () : CSqlTHREAD( true )
{
}
__fastcall CLS_SqlTHREAD_Log::~CLS_SqlTHREAD_Log ()
{
    this->Free ();
}

//-------------------------------------------------------------------------------------------------
void CLS_SqlTHREAD_Log::Execute ()
{
	// 높였더니...로그인 몰릴때..다른 처리를 못해서...문제...
	// this->SetPriority( THREAD_PRIORITY_HIGHEST );	// Priority 2 point above the priority class
	// this->SetPriority( THREAD_PRIORITY_ABOVE_NORMAL );	// Priority 2 point above the priority class

	g_LOG.CS_ODS( 0xffff, ">  > >> CLS_SqlTHREAD_Log::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );

	while( TRUE ) {
		if ( !this->Terminated ) {
			m_pEVENT->WaitFor( INFINITE );
		} else {
			int iCnt1;
			this->m_CS.Lock ();
			iCnt1 = m_AddPACKET.GetNodeCount();
			this->m_CS.Unlock ();
			if ( iCnt1 <= 0 ) {
				break;
			}
		}

		// 로그 저장..
		this->Proc_QuerySTRING ();
	}

	g_LOG.CS_ODS( 0xffff, "<  < << CLS_SqlTHREAD_Log::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );
}

//-------------------------------------------------------------------------------------------------
bool CLS_SqlTHREAD_Log::Run_SqlPACKET( tagQueryDATA *pSqlPACKET )
{
	return true;
}

//-------------------------------------------------------------------------------------------------
