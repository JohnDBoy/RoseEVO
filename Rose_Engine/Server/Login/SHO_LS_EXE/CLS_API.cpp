
#ifndef __BORLANDC__
	#error >>> ERROR :: must compiled in borland c++ builder !!!
#endif

#include "../SHO_LS_LIB/CLS_API.h"
#include "BCB_Tools.h"
#include "FrmMAIN.h"
#include "CVclTHREAD.h"

//---------------------------------------------------------------------------
void  __stdcall CEXE_API::WriteLOG(char *szString)
{
    CVclLogSTR *pDATA = new CVclLogSTR( FormMAIN->MemoLOG, szString );
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
void* __stdcall CEXE_API::AddConnectorITEM (void *pOwner, char *szIP)
{
    CVclAddListITEM *pDATA = new CVclAddListITEM( FormMAIN->ListViewCLIENT, "unknown", 2, pOwner);
    if ( pDATA ) {
        pDATA->SetSubItem( 0, szIP );
        pDATA->SetSubItem( 1, CVclTHREAD::Instance()->GetTimeSTR()  );

        CVclTHREAD::Instance()->AddDATA( pDATA );

        return pDATA->GetListITEM();
    }
    return NULL;
}
void  __stdcall CEXE_API::DelConnectorITEM (void *pListItem)
{
    if ( pListItem ) {
        CVclDelListITEM *pDATA = new CVclDelListITEM( FormMAIN->ListViewCLIENT, (TListItem*)pListItem );
        if ( pDATA )
            CVclTHREAD::Instance()->AddDATA( pDATA );
    }
}
/*
constructor TListItem.Create(AOwner: TListItems);
begin
  FOwner := AOwner;
  FSubItems := TSubItems.Create(Self);
  FOverlayIndex := -1;
  FStateIndex := -1;
end;

function TListItems.CreateItem(Index: Integer;
  ListItem: TListItem): TLVItem;
begin
  with Result do
  begin
    mask := LVIF_PARAM or LVIF_IMAGE;
    iItem := Index;
    iSubItem := 0;
    iImage := I_IMAGECALLBACK;
    lParam := Longint(ListItem);
  end;
end;


// OK !!! new TListItem( TListItems )
function TCustomListView.CreateListItem: TListItem;
begin
  Result := TListItem.Create(Items);
end;


function TListItems.Add: TListItem;
begin
  Result := Owner.CreateListItem;
  ListView_InsertItem(Handle, CreateItem(Count, Result));
end;
*/

//---------------------------------------------------------------------------
void* __stdcall CEXE_API::AddWorldITEM (void *pOwner, char *szWorld, char *szIP, int iPort, unsigned int dwRight)
{
    CVclAddListITEM *pDATA = new CVclAddListITEM( FormMAIN->ListViewSERVER, szWorld, 7, pOwner );

    if ( pDATA ) {
        pDATA->SetSubItem( 0, szIP );
        pDATA->SetSubItem( 1, iPort );
        pDATA->SetSubItem( 2, "0" );    // channels
        pDATA->SetSubItem( 3, "0" );    // users
        pDATA->SetSubItem( 4, "0" );    // max users
        pDATA->SetSubItem( 5, CVclTHREAD::Instance()->GetTimeSTR()  );
        pDATA->SetSubItem( 6, "0" );    // right

        CVclTHREAD::Instance()->AddDATA( pDATA );

        return pDATA->GetListITEM ();
    }
    return NULL;
}
void  __stdcall CEXE_API::DelWorldITEM (void *pListItem)
{
    if ( pListItem ) {
        CVclDelListITEM *pDATA = new CVclDelListITEM( FormMAIN->ListViewSERVER, (TListItem*)pListItem );
        if ( pDATA )
            CVclTHREAD::Instance()->AddDATA( pDATA );
    }
}

//---------------------------------------------------------------------------
void* __stdcall CEXE_API::AddBlockITEM (void *pOwner, char *szIP, unsigned int dwEndTime)
{
    CVclAddListITEM *pDATA = new CVclAddListITEM( FormMAIN->ListViewIP, szIP, 2, pOwner );
    if ( pDATA ) {
        pDATA->SetSubItem( 0, "??" );
        pDATA->SetSubItem( 1, "??" );

        CVclTHREAD::Instance()->AddDATA( pDATA );

        return pDATA->GetListITEM();
    }
    return NULL;
}
void  __stdcall CEXE_API::DelBlockITEM (void *pListItem)
{
    if ( pListItem ) {
        CVclDelListITEM *pDATA = new CVclDelListITEM( FormMAIN->ListViewIP, (TListItem*)pListItem );
        if ( pDATA )
            CVclTHREAD::Instance()->AddDATA( pDATA );
    }
}

void __stdcall CEXE_API::SetStatusBarTEXT (unsigned int iItemIDX, char *szText)
{
    FormMAIN->StatusBar1->Panels->Items[ iItemIDX ]->Text = szText;
}

//---------------------------------------------------------------------------
