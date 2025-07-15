//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USEFORM("FrmMAIN.cpp", FormMAIN);
USEFORM("..\..\bcb_common\FrmABOUT.cpp", FormABOUT);
USEFORM("..\..\bcb_common\FrmANNOUNCE.cpp", FormAnnounce);
USEFORM("..\..\bcb_common\FrmLogMODE.cpp", FormLogMODE);
USEFORM("FrmLSCFG.cpp", FormLSCFG);
USEFORM("FrmLoginRIGHT.cpp", FormLoginRIGHT);
USEFORM("FrmLimitUSER.cpp", FormLimitUSER);
//---------------------------------------------------------------------------
HINSTANCE   g_hCurInstance;

//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE hCurInstance, HINSTANCE hPrevInstance, LPSTR, int)
{
    HANDLE  hMUTEX;
    
    try
    {
        const char szMUTEX[] = "SHO_LS";

        hMUTEX = OpenMutex( MUTEX_ALL_ACCESS, false, szMUTEX );
        if ( hMUTEX )
            return 0;
        hMUTEX = CreateMutex( NULL, true, szMUTEX );

         g_hCurInstance = hCurInstance;
        
         Application->Initialize();
         Application->CreateForm(__classid(TFormMAIN), &FormMAIN);
		Application->CreateForm(__classid(TFormLSCFG), &FormLSCFG);
		Application->CreateForm(__classid(TFormLoginRIGHT), &FormLoginRIGHT);
		Application->CreateForm(__classid(TFormLimitUSER), &FormLimitUSER);
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



