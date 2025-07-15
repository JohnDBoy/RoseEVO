#ifndef ToolsH
#define ToolsH
//---------------------------------------------------------------------------

#include <vcl.h>
#include <Classes.hpp>
#include <comctrls.hpp>
#include <Filectrl.hpp>
#include <Registry.hpp>

#ifndef _ASSERT
#include <tchar.h>
#include <crtdbg.h>
#endif

#pragma hdrstop


#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif



// #define __USE_LOG__
#ifdef  __USE_LOG__
#define LOG_NONE        0
#define LOG_NORMAL      0x01
#define LOG_VERBOSE     ( LOG_NORMAL  | 0x02 )
#define LOG_DEBUG       ( LOG_VERBOSE | 0x04 )
#endif


class BCBUtil {
private:
#ifdef  __USE_LOG__
    static int      m_iLogSetting = LOG_NORMAL;
    static char     m_buffer[ 1024 ];
#endif
    static int      m_iColumnToSort;
    static int      m_iFindIndex;

public :
    static bool     LoadControl (char *szIniFileName, TWinControl *pParentCTRL);
    static bool     SaveControl (char *szIniFileName, TWinControl *pParentCTRL);

    static String   Get_TObjectName (TObject *Sender, char *szCallFunc);

    static long     GetFileSize(const AnsiString &FileName);
    static String   GetVersionString (const char *szCaption);

/*
TRegDataType Registry_GetValueType   (String KeyName, String ValueName, bool bCreate=false);
void         Registry_DeleteKey      (String KeyName);
void         Registry_WriteString    (String KeyName, String ValueName, String KeyValue);
String       Registry_ReadString     (String KeyName, String ValueName, bool bCreate=false);
void         Registry_WriteInteger   (String KeyName, String ValueName, int iValue);
int          Registry_ReadInteger    (String KeyName, String ValueName, bool bCreate=false);
*/

    static TRegDataType GetRegistryValueType (char *KeyName, char *ValueName, bool bCreate=false);
    static bool     IsRegKeyExist    (char *szKeyName);
    static void     DelRegistryKey   (char *szKeyName);
    static void     SetRegistrySTR   (char *szKeyName, char *szStrName, char *szKeyValue);
    static void     SetRegistryINT   (char *szKeyName, char *szStrName, int  iKeyValue);
    static String   GetRegistrySTR (char *szKeyName, char *szStrName, bool bCreate);
    static int      GetRegistryINT (char *szKeyName, char *szStrName, bool bCreate);

    // unsigned int StrToHash (char const * szString);
    static bool     CheckPrivateNumber  (char *szNumber);
    static bool     CheckBussinessNumber(char *szNumber);

    static void     Show_LastError (void);
    static String   Show_SenderName (TObject *Sender, char *szCallFunc);


    // 마지막에 NULL을 삽입.
    static TListItem*AddListITEM (TListView *pListView, const char *szCaption, ...);
    static bool      DelListITEM (TListView *pListView, TListItem *pItem);

    // SetListITEM( pItem, "caption", 0, "col2", 2, NULL );  문자열, 인덱스로 ... 문자열이 NULL까지..
    static void      SetListITEM (TListItem *pItem, const char *szText, ...);
    static String    GetListItemTEXT (TListItem *pItem, int iIndex);


    static TListItem* FindFirstListITEM (TListView *pListView, char *szString);
    static TListItem* FindNextListITEM  (TListView *pListView, char *szString);

    static bool     ListViewLOAD (TListView *pListView, char *szFileName);
    static bool     ListViewSAVE (TListView *pListView, char *szFileName);

    static void     ListViewSORT (TObject *Sender, TListColumn *Column);
    static void     ListViewCOMPARE (TListItem *Item1, TListItem *Item2, int Data, int &Compare);

    static void     CheckAllListViewITEM (TListView *pListView, bool bCheck);

    static bool     DeleteDIR (String stParentDIR, String stChildDIR, String stFileMask);
    static String   ExtractRootDIR (char *szFullPath);


#ifdef  __USE_LOG__
    static int      Log_GetMode (void);
    static void     Log_SetMode (int iLogType);
    static void     Log_String  (int iLogType, char *fmt, ...);
    static void     Out_String (int iLogType, char *fmt, ...);
#endif
} ;

//---------------------------------------------------------------------------
#endif
