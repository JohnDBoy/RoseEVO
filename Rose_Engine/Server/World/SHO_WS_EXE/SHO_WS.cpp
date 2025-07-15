//---------------------------------------------------------------------------

#include "PreHEADER.h"

USEFORM("FrmMAIN.cpp", FormMAIN);
USEFORM("FrmWSCFG.cpp", FormWSCFG);
USEFORM("FrmDataDIR.cpp", FormDataDIR);
USEFORM("..\..\bcb_common\FrmABOUT.cpp", FormABOUT);
USEFORM("..\..\bcb_common\FrmANNOUNCE.cpp", FormAnnounce);
USEFORM("..\..\bcb_common\FrmLogMODE.cpp", FormLogMODE);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    HANDLE  hMUTEX;
    
    try
    {
        const char szMUTEX[] = "SHO_WS";

        hMUTEX = OpenMutex( MUTEX_ALL_ACCESS, false, szMUTEX );
        if ( hMUTEX )
            return 0;
        hMUTEX = CreateMutex( NULL, true, szMUTEX );

         Application->Initialize();
         Application->CreateForm(__classid(TFormMAIN), &FormMAIN);
		Application->CreateForm(__classid(TFormWSCFG), &FormWSCFG);
		Application->CreateForm(__classid(TFormDataDIR), &FormDataDIR);
		Application->CreateForm(__classid(TFormABOUT), &FormABOUT);
		Application->CreateForm(__classid(TFormAnnounce), &FormAnnounce);
		Application->CreateForm(__classid(TFormLogMODE), &FormLogMODE);
		Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }

    ReleaseMutex( hMUTEX );
    CloseHandle( hMUTEX );

    return 0;
}
//---------------------------------------------------------------------------


