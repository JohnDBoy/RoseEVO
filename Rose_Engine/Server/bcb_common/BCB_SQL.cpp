//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "classLOG.h"
#include "BCB_SQL.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)


//---------------------------------------------------------------------------
bool BCB_SQL::OpenSQL (TQuery *pQuery, String &stQuery)
{
    pQuery->Close ();
    pQuery->SQL->Clear ();
    pQuery->SQL->Add( stQuery );

    if ( !pQuery->Prepared ) {
        try {
            pQuery->Prepare();
        }
        catch ( EDBEngineError *dbError ) {
            Log_String (LOG_NORMAL, "DB ERROR[ OpenSQL ], Query[%s]\n", stQuery.c_str() );
            for(int i=0; i<dbError->ErrorCount; i++)
                Log_String (LOG_NORMAL, "DB Error :: ErrorCODE: %d, ErrorMSG: %s", dbError->Errors[i]->ErrorCode, dbError->Errors[i]->Message.c_str() );
                
            return false;
        }
        catch( ... ) {
            Log_String (LOG_NORMAL, "  >>> DB ERROR[ OpenSQL ]");
            return false;
        }
    }

    try {
        pQuery->Open ();
    }
    catch ( EDBEngineError *dbError ) {
        Log_String (LOG_NORMAL, "DB ERROR[ OpenSQL ], Query[%s]\n", stQuery.c_str() );
        for(int i=0; i<dbError->ErrorCount; i++)
            Log_String (LOG_NORMAL, "DB Error :: ErrorCODE: %d, ErrorMSG: %s", dbError->Errors[i]->ErrorCode, dbError->Errors[i]->Message.c_str() );
            
        return false;
    }
    catch( ... ) {
        Log_String (LOG_NORMAL, "  >>> DB ERROR[ OpenSQL ]");
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------
bool BCB_SQL::ExecSQL (TQuery *pQuery, String &stQuery)
{
    pQuery->Close ();
    pQuery->SQL->Clear ();
    pQuery->SQL->Add ( stQuery );

    if ( !pQuery->Prepared ) {
        try {
            pQuery->Prepare();
        }
        catch ( EDBEngineError *dbError ) {
            Log_String (LOG_NORMAL, "DB ERROR[ ExecSQL ], Query[%s]\n", stQuery.c_str() );
            for(int i=0; i<dbError->ErrorCount; i++)
                Log_String (LOG_NORMAL, "DB Error :: ErrorCODE: %d, ErrorMSG: %s", dbError->Errors[i]->ErrorCode, dbError->Errors[i]->Message.c_str() );
                
            return false;
        }
        catch( ... ) {
            Log_String (LOG_NORMAL, "  >>> DB ERROR[ ExecSQL ]");
            return false;
        }
    }

    try {
        pQuery->ExecSQL();
    }
    catch ( EDBEngineError *dbError ) {
        Log_String (LOG_NORMAL, "DB ERROR[ ExecSQL ], Query[%s]\n", stQuery.c_str() );
        for(int i=0; i<dbError->ErrorCount; i++)
            Log_String (LOG_NORMAL, "DB Error :: ErrorCODE: %d, ErrorMSG: %s", dbError->Errors[i]->ErrorCode, dbError->Errors[i]->Message.c_str() );

        return false;
    }
    catch( ... ) {
        Log_String (LOG_NORMAL, "  >>> DB ERROR[ ExecSQL ]");
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------
