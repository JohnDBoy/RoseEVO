/*
	$Header: /7HeartsOnline/Server/SHO_GS/SHO_GS_LIB/Common/CSocket.h 1     05-11-23 7:04p Young $
*/
#ifndef	__CSOCKET_H
#define	__CSOCKET_H
#include <winSock.h>
//-------------------------------------------------------------------------------------------------

class CSOCKET {
private :
public  :
	static bool Init ();
	static void Free ();
} ;

int Socket_Error (char *szMsg);

//-------------------------------------------------------------------------------------------------
#endif
