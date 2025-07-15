
#ifndef __BORLANDC__
	#error >>> ERROR :: must compiled in borland c++ builder !!!
#endif

#include "../SHO_WS_LIB/CWS_API.h"
#include "BCB_Tools.h"
#include "FrmMAIN.h"
#include "CVclTHREAD.h"

//---------------------------------------------------------------------------
void  __stdcall CEXE_API::SetUserCNT( int iUserCNT )
{
    FormMAIN->m_iUserCNT = iUserCNT;
}

void  __stdcall CEXE_API::WriteLOG(char *szString)
{
    CVclLogSTR *pDATA = new CVclLogSTR( FormMAIN->MemoLOG, szString, 500 );
    if ( pDATA )
        CVclTHREAD::Instance()->AddDATA( pDATA );
}

//---------------------------------------------------------------------------
void __stdcall CEXE_API::SetListItemINT(void *pListItem, int iSubStrIDX, int iValue)
{
    if ( pListItem ) {
        CVclSetListITEM *pDATA = new CVclSetListITEM( (TListItem*)pListItem, iSubStrIDX, iValue);
        CVclTHREAD::Instance()->AddDATA( pDATA );
    }
}
//---------------------------------------------------------------------------
void __stdcall CEXE_API::SetListItemSTR(void *pListItem, int iSubStrIDX, char *szStr)
{
    if ( pListItem ) {
        CVclSetListITEM *pDATA = new CVclSetListITEM( (TListItem*)pListItem, iSubStrIDX, szStr );
        if ( pDATA )
            CVclTHREAD::Instance()->AddDATA( pDATA );
    }
}


//---------------------------------------------------------------------------
void* __stdcall CEXE_API::AddChannelITEM(void *pOwner, short nChannelNO, char *szChannelName, char *szServerIP, int iPortNO )
{
    CVclAddListITEM *pDATA = new CVclAddListITEM( FormMAIN->ListViewCHANNEL, nChannelNO, 7, pOwner );
    if ( pDATA ) {
        pDATA->SetSubItem( 0, szChannelName );
        pDATA->SetSubItem( 1, szServerIP );
        pDATA->SetSubItem( 2, iPortNO );
        pDATA->SetSubItem( 3, "0" );
        pDATA->SetSubItem( 4, "0" );

        pDATA->SetSubItem( 5, CVclTHREAD::Instance()->GetTimeSTR() );
        pDATA->SetSubItem( 6, "1" );        // Active                                  

        CVclTHREAD::Instance()->AddDATA( pDATA );

        return pDATA->GetListITEM();
    }
    return NULL;
}
//---------------------------------------------------------------------------
void __stdcall CEXE_API::DelChannelITEM(void *pListItem)
{
    if ( pListItem ) {
        CVclDelListITEM *pDATA = new CVclDelListITEM( FormMAIN->ListViewCHANNEL, (TListItem*)pListItem );
        if ( pDATA ) {
            CVclTHREAD::Instance()->AddDATA( pDATA );
        }
    }
}

//---------------------------------------------------------------------------
void* __stdcall CEXE_API::AddUserITEM(void *pUser, char *szAccount, char *szCharName, char *szIP)
{
    CVclAddListITEM *pDATA = new CVclAddListITEM( FormMAIN->ListViewUSER, szAccount, 3, pUser );
    if ( pDATA ) {
        pDATA->SetSubItem( 0, szCharName );
        pDATA->SetSubItem( 1, szIP );
        pDATA->SetSubItem( 2, CVclTHREAD::Instance()->GetTimeSTR() );

        CVclTHREAD::Instance()->AddDATA( pDATA );

        return pDATA->GetListITEM();
    }
    return NULL;
}

//---------------------------------------------------------------------------
void __stdcall CEXE_API::DelUserITEM(void *pListItem)
{
    if ( pListItem ) {
        CVclDelListITEM *pDATA = new CVclDelListITEM( FormMAIN->ListViewUSER, (TListItem*)pListItem );
        if ( pDATA ) {
            CVclTHREAD::Instance()->AddDATA( pDATA );
        }
    }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

