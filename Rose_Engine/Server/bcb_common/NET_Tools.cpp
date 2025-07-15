//---------------------------------------------------------------------------
#include <Registry.hpp>
#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "DLLIST.h"
#include "Net_Tools.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
int BCBUtil::m_iColumnToSort=0;
int BCBUtil::m_iFindIndex=0;


union tagCtrlDATA {
    struct {    // tagEdit
        char szText[ 1 ];
    } ;
    struct {    //tagListView
        bool    bChecked;
        short   nCaptionLen;
        char    szCaption[ 1 ];
    } ;
} ;

//#include "classLOG.h"
bool BCBUtil::LoadControl (char *szIniFileName, TWinControl *pParentCTRL)
{
    if ( !FileExists( szIniFileName ) )
        return false;

    TList *pList;
    pList = new TList;

    TWinControl *pCTRL;
    TClass ClassRef;
    String stClassName;

    tagCtrlDATA *pProperty;
    pProperty = (tagCtrlDATA*) new BYTE[ MAX_PATH ];

    pParentCTRL->GetTabOrderList( pList );

    for (int iL=0; iL<pList->Count; iL++) {
        pCTRL = (TWinControl*)pList->Items[ iL ];
        ClassRef = pCTRL->ClassType ();
        stClassName = String( ClassRef->ClassName() );

        // LogString( 0xffff, "laodControl:: %s: %s", stClassName.c_str(), pCTRL->Name.c_str() );

        if ( stClassName == "TEdit" ) {
            TEdit *pTEdit = (TEdit*)pCTRL;
            if ( ::GetPrivateProfileString (
                            pParentCTRL->Name.c_str(),
                            pCTRL->Name.c_str(),
                            "<<not found>>",
                            pProperty->szText,
                            MAX_PATH,
                            szIniFileName ) ) {
                if ( strcmp( "<<not found>>", pProperty->szText ) ) {
                    pTEdit->Text = pProperty->szText;
                }
            }
        } else
        if ( stClassName == "TListView" ) {
            TListView *pTListView = (TListView*)pCTRL;
            pTListView->Items->Clear ();

            ::GetPrivateProfileString (
                            pParentCTRL->Name.c_str(),
                            pCTRL->Name.c_str(),
                            "0",
                            pProperty->szText,
                            MAX_PATH,
                            szIniFileName );
            int iItemCnt = atoi( pProperty->szText );
            TListItem *pListItem;

            String stKeyName;
            for (int iIdx=0; iIdx<iItemCnt; iIdx++) {

                stKeyName = "size_" + IntToStr(iIdx) + pCTRL->Name;
                ::GetPrivateProfileString (
                        pParentCTRL->Name.c_str(),  // "Size",
                        stKeyName.c_str(),
                        "0",
                        pProperty->szText,
                        MAX_PATH,
                        szIniFileName );

                int iDataLen = atoi( pProperty->szText );

                stKeyName = "item_" + IntToStr(iIdx) + pCTRL->Name;
                ::GetPrivateProfileStruct(
                        pParentCTRL->Name.c_str(),  // "Items",
                        stKeyName.c_str(),
                        pProperty,
                        iDataLen,
                        szIniFileName );

                pListItem = BCBUtil::AddListITEM( pTListView, pProperty->szCaption, NULL );
                pListItem->Checked = pProperty->bChecked;
                pListItem->SubItems->Text = &pProperty->szCaption[ pProperty->nCaptionLen+ 1];
                /*
                LogString( 0xffff, "    %s %d: %d %s",
                        stKeyName.c_str(),
                        sizeof(sProperty), sProperty.bChecked, pProperty->szCaption );
                */
            }
        }
    }

    delete[] pProperty;
    delete pList;

    return true;
}


//---------------------------------------------------------------------------
bool BCBUtil::SaveControl (char *szIniFileName, TWinControl *pParentCTRL)
{
    TList *pList;
    pList = new TList;

    TWinControl *pCTRL;
    TClass ClassRef;
    String stClassName;

    tagCtrlDATA *pProperty;

    pProperty = (tagCtrlDATA*) new BYTE[ MAX_PATH ];

    pParentCTRL->GetTabOrderList( pList );

    for (int iL=0; iL<pList->Count; iL++) {
        pCTRL = (TWinControl*)pList->Items[ iL ];
        ClassRef = pCTRL->ClassType ();
        stClassName = String( ClassRef->ClassName() );

        if ( stClassName == "TEdit" ) {
            TEdit *pTEdit = (TEdit*)pCTRL;
            if ( pTEdit->Text.Length() > 0 ) {
                ::WritePrivateProfileString (
                                pParentCTRL->Name.c_str(),
                                pCTRL->Name.c_str(),
                                pTEdit->Text.c_str(),
                                szIniFileName );
            }
        } else
        if ( stClassName == "TListView" ) {
            TListView *pTListView = (TListView*)pCTRL;
            
            String stCNT = IntToStr( pTListView->Items->Count );
            // write list item count
            ::WritePrivateProfileString (
                            pParentCTRL->Name.c_str(),
                            pCTRL->Name.c_str(),
                            stCNT.c_str(),
                            szIniFileName );

            String stKeyName;
            if ( pTListView->Items->Count ) {
                for (int iIdx=0; iIdx<pTListView->Items->Count; iIdx++) {
                    TListItem *pListItem =  pTListView->Items->Item[ iIdx ];

                    pProperty->bChecked = pListItem->Checked;
                    pProperty->nCaptionLen = pListItem->Caption.Length();

                    strcpy( pProperty->szCaption, pListItem->Caption.c_str() );
                    pProperty->szCaption[ pListItem->Caption.Length() ] = 0;
                    strcpy(&pProperty->szCaption[ pListItem->Caption.Length()+1 ], pListItem->SubItems->Text.c_str() );

                    int iDataLen = sizeof( tagCtrlDATA ) + pListItem->Caption.Length() + pListItem->SubItems->Text.Length();

                    stKeyName = "item_" + IntToStr(iIdx) + pCTRL->Name;
                    ::WritePrivateProfileStruct(
                            pParentCTRL->Name.c_str(),          // "Items",
                            stKeyName.c_str(),
                            pProperty,
                            iDataLen,
                            szIniFileName );

                    stCNT = IntToStr( iDataLen );
                    stKeyName = "size_" + IntToStr(iIdx) + pCTRL->Name;
                    ::WritePrivateProfileString (
                                    pParentCTRL->Name.c_str(),  // "Size",
                                    stKeyName.c_str(),
                                    stCNT.c_str(),
                                    szIniFileName );
                }
            }
        }
//        LogString( 0xffff, "%s: %s", stClassName.c_str(), pCTRL->Name.c_str() );
    }

    delete[] pProperty;
    delete pList;
    
    return true;
}


//---------------------------------------------------------------------------
bool BCBUtil::DeleteDIR (String stParentDIR, String stChildDIR, String stFileMask)
{
    String stFullPath, stCurrentDir, stFindFile;
    TSearchRec sr;

    classDLLIST <String>  DirLIST;
    classDLLNODE<String> *pDirNODE;

    if ( stChildDIR ==  "." ) {
        stCurrentDir = stParentDIR;
    } else {
        stCurrentDir = stParentDIR + "\\" + stChildDIR;
    }

    stFullPath = stCurrentDir + "\\" + stFileMask;
    if ( FindFirst( stFullPath, faAnyFile, sr) == 0 ) {
        do {
            if ( sr.Attr & faDirectory ) {
                if ( sr.Name != "." && sr.Name != ".." ) {
                    pDirNODE = DirLIST.AllocNAppend ();
                    pDirNODE->DATA = sr.Name;
                }
            } else {
                stFindFile = stCurrentDir + "\\" + sr.Name;

                FileSetAttr( stFindFile, 0 );
                DeleteFile( stFindFile );
            }
        } while( FindNext(sr) == 0 );

        FindClose(sr);
    }

    pDirNODE = DirLIST.GetHeadNode ();
    while( pDirNODE ) {
        if ( stChildDIR == "." ) {
            DeleteDIR (stParentDIR,                     pDirNODE->DATA, stFileMask );
        } else {
            DeleteDIR (stParentDIR,  stChildDIR+ "\\" + pDirNODE->DATA, stFileMask );
        }

        DirLIST.DeleteNFree( pDirNODE );
        pDirNODE = DirLIST.GetHeadNode ();
    }

//  SetCurrentDir( stParentDIR );
    ::RemoveDir( stCurrentDir );

    return true;
}

//---------------------------------------------------------------------------
String BCBUtil::ExtractRootDIR (char *szFullPath)
{
    String stResult;

    stResult = szFullPath;
    short nI;
    for (nI=0; szFullPath[nI] && szFullPath[nI] != '\\' ; nI++)
    {
        ;
    }

    stResult.SetLength( nI );
    
    return stResult;
}

//---------------------------------------------------------------------------
String BCBUtil::Get_TObjectName (TObject *Sender, char *szCallFunc)
{
    TClass ClassRef = Sender->ClassType ();
    String stName = String( ClassRef->ClassName() );

    return stName;
}

//---------------------------------------------------------------------------
TListItem* BCBUtil::AddListITEM (TListView *pListView, const char *szCaption, ...)
{
    TListItem *pItem;
    pItem = pListView->Items->Add ();

    if ( !pItem )
        return NULL;

    pItem->Caption = szCaption;

	va_list va;
	va_start(va, szCaption);

    char *szStr;
	while ( (szStr = va_arg(va, char*) ) != NULL) {
        pItem->SubItems->Add ( szStr );
	}

	va_end(va);

    return pItem;
}
bool BCBUtil::DelListITEM (TListView *pListView, TListItem *pItem)
{
    if ( pListView && pItem ) {
        int iItemIndex = pListView->Items->IndexOf( pItem );
        if ( iItemIndex >= 0 ) {
            pListView->Items->Delete( iItemIndex );
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
void BCBUtil::SetListITEM (TListItem *pItem, const char *szText, ...)
{
    if ( !pItem )
        return;

	va_list va;
	va_start(va, szText);

    const char *szStr;
    // pItem->Caption = szCaption;
    szStr = szText;

    int iIndex;
    while( szStr ) {
        iIndex = va_arg(va, int);

        if ( 0 == iIndex ) {    // caption ..
            pItem->Caption = szStr;
        } else {
            pItem->SubItems->Strings[ iIndex-1 ] = szStr;
        }
        szStr = va_arg(va, char *);
    }
/*
	while ( (iIndex = va_arg(va, int)) != -1) {
        szStr = va_arg(va, char *);
        pItem->SubItems->Strings[ iIndex ] = szStr;
	}
*/
	va_end(va);
}
//---------------------------------------------------------------------------
String BCBUtil::GetListItemTEXT (TListItem *pItem, int iIndex)
{
    if ( 0 == iIndex )
        return pItem->Caption;

    return pItem->SubItems->Strings[ iIndex-1 ];
}

//---------------------------------------------------------------------------
TListItem* BCBUtil::FindFirstListITEM (TListView *pListView, char *szString)
{
    m_iFindIndex = 0;

    TListItem *pItem;

    pItem = pListView->FindCaption(m_iFindIndex, szString, false, true, false);
    if ( pItem )
        m_iFindIndex = pItem->Index;

    return pItem;
}
TListItem* BCBUtil::FindNextListITEM (TListView *pListView, char *szString)
{
    TListItem *pItem;

    pItem = pListView->FindCaption(m_iFindIndex, szString, false, false, false);
    if ( pItem )
        m_iFindIndex = pItem->Index;

    return pItem;
}

//---------------------------------------------------------------------------
void BCBUtil::ListViewSORT (TObject *Sender, TListColumn *Column)
{
    m_iColumnToSort = Column->Index;
    ((TCustomListView *)Sender)->AlphaSort();
    ((TCustomListView *)Sender)->Update();
}
void BCBUtil::ListViewCOMPARE (TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
    if ( m_iColumnToSort == 0 )
        Compare = CompareText(Item1->Caption,Item2->Caption);
    else {
        int iX = m_iColumnToSort - 1;
        Compare = CompareText(Item1->SubItems->Strings[iX], Item2->SubItems->Strings[iX]);
    }
}

//---------------------------------------------------------------------------
void BCBUtil::CheckAllListViewITEM (TListView *pListView, bool bCheck)
{
    int iC;
    TListItem *pItem;

    for (iC=0; iC<pListView->Items->Count; iC++) {
        pItem = pListView->Items->Item[ iC ];
        pItem->Checked = bCheck;
    }
}

//---------------------------------------------------------------------------
bool BCBUtil::ListViewLOAD (TListView *pListView, char *szFileName)
{
    FILE *fp;
    int   iIdx, iCnt, iStrLen, iSub, iColCnt;
    TListItem *pItem;
    char *pStr;

    if ( !FileExists(szFileName) )
        return false;

    fp = fopen (szFileName, "rb");
    if ( !fp ) return false;

    fread (&iCnt,       sizeof(int), 1, fp);
    fread (&iColCnt,    sizeof(int), 1, fp);

    pListView->Items->Clear ();
    for (iIdx=0; iIdx<iCnt; iIdx++) {
        pItem = pListView->Items->Add ();

        fread (&iStrLen, sizeof(int),    1, fp);
        if ( iStrLen ) {
            pStr = new char[ iStrLen+1 ];
            ::ZeroMemory (pStr, iStrLen+1);
            fread (pStr,    sizeof(char),   iStrLen, fp);
            pItem->Caption = pStr;
            delete[] pStr;
        }

        for (iSub=0; iSub<iColCnt-1; iSub++) {
            fread (&iStrLen, sizeof(int),    1, fp);
            if ( iStrLen ) {
                pStr = new char[ iStrLen+1 ];
                ::ZeroMemory (pStr, iStrLen+1);
                fread (pStr,    sizeof(char),   iStrLen, fp);
                pItem->SubItems->Add (pStr);
                delete[] pStr;
            } else
                pItem->SubItems->Add ("");
        }
    }
    fclose (fp);

    return true;
}

bool BCBUtil::ListViewSAVE (TListView *pListView, char *szFileName)
{
    FILE *fp;
    int     iIdx, iCnt, iStrLen, iColCnt;
    TListItem *pItem;

    fp = fopen (szFileName, "wb");
    if ( !fp ) return false;

    iIdx = pListView->Items->Count;
    iColCnt = pListView->Columns->Count;

    fwrite (&iIdx,      sizeof(int), 1, fp);
    fwrite (&iColCnt,   sizeof(int), 1, fp);

    for (iIdx=0; iIdx<pListView->Items->Count; iIdx++) {
        pItem = pListView->Items->Item[ iIdx ];

        iStrLen = pItem->Caption.Length ();
        fwrite (&iStrLen, sizeof(int),    1, fp);
        if ( iStrLen )
            fwrite (pItem->Caption.c_str(), sizeof(char),   iStrLen, fp);

        for (iCnt=0; iCnt<iColCnt-1; iCnt++) {
            iStrLen = pItem->SubItems->Strings[ iCnt ].Length ();
            fwrite (&iStrLen, sizeof(int),    1, fp);
            if ( iStrLen )
                fwrite (pItem->SubItems->Strings[ iCnt ].c_str(),  sizeof(char),   iStrLen, fp);
        }
    }
    fclose (fp);

    return true;
}

//---------------------------------------------------------------------------
long BCBUtil::GetFileSize(const AnsiString &FileName)
{
    TSearchRec SearchRec;

    if ( FindFirst(ExpandFileName(FileName), faAnyFile, SearchRec)==0 )
        return SearchRec.Size;
    else
        return -1;
}


//---------------------------------------------------------------------------
String BCBUtil::GetVersionString (const char *szCaption)
{
    unsigned int   uiSize, uiBytes;
    DWORD   dwSize, dwHandle;
    void   *pBlock, *lpBuffer;
    char    SubBlock[ MAX_PATH ];
    String  stResult;

    //Get the size of the version information structure
    dwSize = GetFileVersionInfoSize(TEXT( Application->ExeName.c_str() ), &dwHandle);
    if ( dwSize == 0 )
        return NULL;

    pBlock= new BYTE[ dwSize ];

    GetFileVersionInfo(Application->ExeName.c_str(), dwHandle, dwSize, pBlock);

    // Structure used to store enumerated languages and code pages.
    struct LANGANDCODEPAGE {
        WORD wLanguage;
        WORD wCodePage;
    } *lpTranslate;
    // Read the list of languages and code pages.
    VerQueryValue(pBlock, TEXT("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate, &uiSize);

    // Read the file description for each language and code page.
    for(unsigned int i=0; i < (uiSize/sizeof(struct LANGANDCODEPAGE)); i++ )
    {
        wsprintf( SubBlock, TEXT("\\StringFileInfo\\%04x%04x\\FileVersion"), lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);

        // Retrieve file description for language and code page "i".
        if ( VerQueryValue(pBlock, SubBlock, &lpBuffer, &uiBytes) ) {
            unsigned int nJ, nK;
            String stTmp;
            char *pTmp;
            pTmp = new char[ strlen((char*)lpBuffer)+1 ];

            strcpy (pTmp, (char*)lpBuffer);
            for (nJ=0, nK=0; nJ<uiBytes; nJ++)
                if ( pTmp[ nJ ] == '.' ) {
                    nK ++;
                    if ( nK == 3 ) {
                        stTmp.sprintf (" v%s (Build:%s)", pTmp, &pTmp[nJ+1]);
                        stResult = szCaption + stTmp;
                    } else
                    if ( nK == 2 )
                        pTmp[ nJ ] = 0;
                }
            delete[] pTmp;
        }
    }

    delete[] pBlock;

    return stResult;
}

//---------------------------------------------------------------------------
bool BCBUtil::IsRegKeyExist (char *KeyName)
{
    TRegistry *Reg = new TRegistry;
    bool bResult = false;

    try
    {
        Reg->RootKey = HKEY_LOCAL_MACHINE; // Section of registry to look for
        if ( Reg->KeyExists( KeyName ) )
            bResult = true;
    }

    __finally
    {
        delete Reg;
    }

    return bResult;
}

//---------------------------------------------------------------------------
// enum TRegDataType { rdUnknown, rdString, rdExpandString, rdInteger, rdBinary };
TRegDataType BCBUtil::GetRegistryValueType (char *KeyName, char *ValueName, bool bCreate)
{
    TRegDataType DataType = rdUnknown;
    TRegistry *Registry = new TRegistry;
    try
    {
        Registry->RootKey = HKEY_LOCAL_MACHINE;
        if ( Registry->OpenKey (KeyName, bCreate) ) {
            DataType = Registry->GetDataType( ValueName );
            Registry->CloseKey ();
        }
    }

    __finally
    {
        delete Registry;
    }

    return DataType;
}

//---------------------------------------------------------------------------
void BCBUtil::DelRegistryKey (char *KeyName)
{
    TRegistry *Reg = new TRegistry;
    try
    {
        Reg->RootKey = HKEY_LOCAL_MACHINE; // Section of registry to look for
        if ( Reg->OpenKey( KeyName, false ) )
            Reg->DeleteKey(KeyName);
    }

    __finally
    {
        delete Reg;
    }
}


//---------------------------------------------------------------------------
void BCBUtil::SetRegistrySTR (char *KeyName, char *StrName, char *KeyValue)
{
    TRegistry *Reg = new TRegistry;
    try
    {
        Reg->RootKey = HKEY_LOCAL_MACHINE;
        if ( Reg->OpenKey( KeyName, true ) ) {
            Reg->WriteString (StrName, KeyValue );
            Reg->CloseKey();
        }
    }

    __finally
    {
        delete Reg;
    }
}

//---------------------------------------------------------------------------
void BCBUtil::SetRegistryINT (char *KeyName, char *StrName, int iKeyValue)
{
    TRegistry *Reg = new TRegistry;
    try
    {
        Reg->RootKey = HKEY_LOCAL_MACHINE;
        if ( Reg->OpenKey( KeyName, true ) ) {
            Reg->WriteInteger (StrName, iKeyValue );
            Reg->CloseKey();
        }
    }

    __finally
    {
        delete Reg;
    }
}


//---------------------------------------------------------------------------
String BCBUtil::GetRegistrySTR (char *KeyName, char *StrName, bool bCreate)
{
    String S;
    TRegistry *Registry = new TRegistry;
    try
    {
        Registry->RootKey = HKEY_LOCAL_MACHINE;
        if ( Registry->OpenKey (KeyName, bCreate) ) {
            S = Registry->ReadString( StrName );
            Registry->CloseKey ();
        }
    }

    __finally
    {
        delete Registry;
    }

    return S;
}

//---------------------------------------------------------------------------
int BCBUtil::GetRegistryINT (char *KeyName, char *StrName, bool bCreate)
{
    int iValue;
    TRegistry *Registry = new TRegistry;
    try
    {
        Registry->RootKey = HKEY_LOCAL_MACHINE;
        if ( Registry->OpenKey (KeyName, bCreate) ) {
            iValue = Registry->ReadInteger ( StrName );
            Registry->CloseKey ();
        }
    }

    __finally
    {
        delete Registry;
    }

    return iValue;
}

String BCBUtil::Show_SenderName (TObject *Sender, char *szCallFunc)
{
    TClass ClassRef = Sender->ClassType ();
    String stName = String( ClassRef->ClassName() );

#ifdef  __USE_LOG__
    Log_String (LOG_NORMAL, "Sender is [%s] in calll %s ... ", stName.c_str(), szInCallFunc );
#endif
    return stName;
}

void BCBUtil::Show_LastError (void)
{
    DWORD dwLastError;
    dwLastError = GetLastError ();

    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dwLastError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL
    );

    // Display the string.
    ::MessageBox( NULL, (LPCSTR)lpMsgBuf, "GetLastError", MB_OK|MB_ICONINFORMATION );
#ifdef  __USE_LOG__
    Log_String (LOG_NORMAL, "GetLastError[ %s ] = %d [ 0x%x ]", lpMsgBuf, dwLastError, dwLastError);
#endif

    // Free the buffer.
    LocalFree( lpMsgBuf );
}

//---------------------------------------------------------------------------
/*
#define BITS_PER_BYTE 8
#define ROL(x,n) (((x)<<(n)) | ((x)>>((sizeof(unsigned int)*BITS_PER_BYTE)-(n))))


unsigned int StrToHash (char const * szString)
{
    unsigned int i;
    unsigned int pos;
    unsigned int hash;

    if ( !szString)
    	return 0;

    for (hash=0,pos=0,i=0; i<strlen(szString); i++) {
        hash ^= ROL(((unsigned int)_tolower((int)szString[i])),
	        	    (pos%(sizeof(unsigned int)*BITS_PER_BYTE)));

    	pos += BITS_PER_BYTE-1;
    }

    return hash;
}
*/


//---------------------------------------------------------------------------
//
//  주민등록 번호 검증.
//
bool BCBUtil::CheckPrivateNumber (char *szNumber)
{
//                          0  1  2  3  4  5  6  7  8  9  10 11
    const int nWeight[] = { 2, 3, 4, 5, 6, 7, 8, 9, 2, 3, 4, 5 };
    int nLoop, nSum, nRest;

    if ( strlen(szNumber) != 13 )
        return false;

    nSum = 0;
    for (nLoop=0; nLoop<12; nLoop++)
        nSum += ( szNumber[ nLoop ] -'0' ) * nWeight[ nLoop ];

    nRest = 11 - ( nSum % 11 );

    return ( (nRest%10) == ( szNumber[ 12 ] - '0' ) );
}


//---------------------------------------------------------------------------
//
//  사업자 등록번호 검증.
//
bool BCBUtil::CheckBussinessNumber (char *szNumber)
{
    const int nWeight[] = { 1, 3, 7, 1, 3, 7, 1, 3 };
    int nLoop, nSum;//, nTmp;

    nSum = 0;
    for (nLoop=0; nLoop<8; nLoop++)
        nSum += ( szNumber[nLoop]-'0' ) * nWeight[ nLoop ];

    nLoop = ( szNumber[ 8 ]-'0' ) * 5;
    nSum  = nSum + (nLoop / 10) + (nLoop % 10);
    nSum %= 10;

    if ( !nSum )
        nSum = (10 - nSum);

    if ( nSum != (szNumber[ 9 ]-'0') )
        return false;

    return true;
}


//---------------------------------------------------------------------------
#ifdef  __USE_LOG__
static int   BCBUtil::m_iLogSetting = LOG_NORMAL;
static char  BCBUtil::m_buffer[ 2048 ];

void Out_String (int iLogType, char *fmt, ...)
{
    if ( !( iLogType & m_iLogSetting ) ) return;

    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(s_buffer,fmt,argptr);
    va_end(argptr);

    OutputDebugString (s_buffer);
}

void Log_SetMode (int iLogType)
{
    m_iLogSetting = iLogType;
}


int Log_GetMode (void)
{
    return m_iLogSetting;
}

void Log_String (int iLogType, char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(s_buffer,fmt,argptr);
    va_end(argptr);

    OutputDebugString( s_buffer );
    if ( iLogType & m_iLogSetting ) {
        FormMain->WriteLOG( s_buffer );
    }

/*
    if ( FormMain->MemoLog->Lines->Count > 400 ) {
        FormMain->MemoLog->Lines->Delete ( 0 );
    }

    FormMain->MemoLog->Lines->Add (s_buffer);
*/
}
#endif


