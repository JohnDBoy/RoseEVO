
#ifndef __BORLANDC__
	#error >>> ERROR :: must compiled in borland c++ builder !!!
#endif

#include "../SHO_GS_LIB/CGS_API.h"
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
void* __stdcall	CEXE_API::AddZoneITEM(void *pOwner, short nZoneNO, char *szZoneName)
{
    CVclAddListITEM *pDATA = new CVclAddListITEM ( FormMAIN->ListViewZONE, nZoneNO, 3, pOwner );
    if ( pDATA ) {
        pDATA->SetSubItem( 0, szZoneName );
        pDATA->SetSubItem( 1, "0" );
        pDATA->SetSubItem( 2, "0" );
        CVclTHREAD::Instance()->AddDATA( pDATA );

        return pDATA->GetListITEM ();
    }
    return NULL;
}

//---------------------------------------------------------------------------
void  __stdcall CEXE_API::DelZoneITEM(void *pListItem)
{
    if ( pListItem ) {
        CVclDelListITEM *pDATA = new CVclDelListITEM( FormMAIN->ListViewZONE, (TListItem*)pListItem );
        if ( pDATA )
            CVclTHREAD::Instance()->AddDATA( pDATA );
    }
}

//---------------------------------------------------------------------------

