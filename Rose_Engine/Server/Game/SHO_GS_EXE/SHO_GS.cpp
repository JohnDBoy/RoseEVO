//---------------------------------------------------------------------------

#include "PreHEADER.h"

USEFORM("FrmMAIN.cpp", FormMAIN);
USEFORM("FrmDBCFG.cpp", FormDBCFG);
USEFORM("FrmGSCFG.cpp", FormGSCFG);
USEFORM("FrmDataDIR.cpp", FormDataDIR);
USEFORM("FrmOPTION.cpp", FormOPTION);
USEFORM("..\..\bcb_common\FrmLogMODE.cpp", FormLogMODE);
USEFORM("..\..\bcb_common\FrmABOUT.cpp", FormABOUT);
USEFORM("..\..\bcb_common\FrmANNOUNCE.cpp", FormAnnounce);
//---------------------------------------------------------------------------
HINSTANCE   g_hCurInstance;

#include <excpt.h>
#include "../SHO_GS_DLL/SHO_GS_DLL.h"


LONG	__stdcall GS_Execption2( EXCEPTION_POINTERS* pExPtrs )
{
    ::MessageBox( NULL, "1111", "aaaa", MB_OK );
    return 1;
}

WINAPI WinMain(HINSTANCE hCurInstance, HINSTANCE hPrevInstance, LPSTR, int)
{
/*
m_hMutex = CreateMutex(NULL, TRUE, MUTEX_STRING_ROSEONLINE);

	// Mutex를 생성했을 경우
	if(m_hMutex == NULL) 
	{
		return FALSE;
	}
	else 
	{
		DWORD dwError = ::GetLastError();

		// 이미 다른 프로세스에 의해서 Mutex가 생성됐을 경우
		if(dwError == ERROR_ALREADY_EXISTS || dwError == ERROR_ACCESS_DENIED )
		{
			// Mutex를 소유하지 못했으므로 핸들만 닫아주고 리턴한다.
			::CloseHandle(m_hMutex);

			return FALSE;
		}
	}
*/	
    HANDLE  hMUTEX;

    try
    {
        const char szMUTEX[] = "SHO_GS";

        hMUTEX = OpenMutex( MUTEX_ALL_ACCESS, false, szMUTEX );
        if ( hMUTEX )
            return 0;
        hMUTEX = CreateMutex( NULL, true, szMUTEX );

         g_hCurInstance = hCurInstance;

         Application->Initialize();
         Application->Title = "SHO_GS";
         Application->CreateForm(__classid(TFormMAIN), &FormMAIN);
		Application->CreateForm(__classid(TFormDBCFG), &FormDBCFG);
		Application->CreateForm(__classid(TFormGSCFG), &FormGSCFG);
		Application->CreateForm(__classid(TFormDataDIR), &FormDataDIR);
		Application->CreateForm(__classid(TFormOPTION), &FormOPTION);
		Application->CreateForm(__classid(TFormLogMODE), &FormLogMODE);
		Application->CreateForm(__classid(TFormABOUT), &FormABOUT);
		Application->CreateForm(__classid(TFormAnnounce), &FormAnnounce);
		Application->Run();
/*
         __try {
             Application->Run();
         }
         __except( GS_Execption2( GetExceptionInformation() ) )
         {
         }
*/
    }
/*
         __except( GS_Execption2( GetExceptionInformation() ) )
         {
         }
*/
///*
    catch (  Exception &exception  )
    {
        Application->ShowException(&exception);
    }
//*/

    ReleaseMutex( hMUTEX );
    CloseHandle( hMUTEX );

    return 0;
}
//---------------------------------------------------------------------------

