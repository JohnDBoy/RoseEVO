
//---------------------------------------------------------------------------

#include "ZSTD.h"

//---------------------------------------------------------------------------

int debug_printf( char *fmt, ... )
{
	va_list args;
	int len = 0;
	char* pBuf = NULL;

	va_start( args, fmt );
	len = _vscprintf( fmt, args )	// _vscprintf doesn't count
                               + 1; // terminating '\0'
	pBuf = new char[len];
	memset( pBuf, 0, len );
	vsprintf( pBuf, fmt, args );

    // 디버그 스트링을 출력함 
    OutputDebugString( pBuf );

    return true; 
}

//---------------------------------------------------------------------------
