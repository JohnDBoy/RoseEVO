
//---------------------------------------------------------------------------

#include "ZSTD.h"
#include <time.h>

//---------------------------------------------------------------------------

ZLog::ZLog( UINT32 nTarget, LPSTR szFileName )
{
	m_nTarget = nTarget;
	m_hWnd = NULL;
	m_hWndListBox = NULL;
	m_pLogFile = NULL;

	if( nTarget & ZLOG_FILE )
	{
		m_pLogFile = new ZFile();
		if( szFileName == NULL )
			m_pLogFile->OpenTextA( "DEFAULT_LOG.txt" );
		else
			m_pLogFile->OpenTextA( szFileName );
	}

	if( nTarget & ZLOG_WINDOW )
		CreateZLogWindow();

	Log( "*** 로그 시작 ***" );
}

//---------------------------------------------------------------------------

ZLog::~ZLog()
{
	DestroyWindow( m_hWnd );
	m_hWnd = NULL;
	m_hWndListBox = NULL;

	if( m_pLogFile != NULL )
		m_pLogFile->Close();
	SAFE_DELETE( m_pLogFile );
}

//---------------------------------------------------------------------------

void ZLog::CreateZLogWindow()
{
	int			x, y, cwidth, cheight;
	WNDCLASS	wc;
	RECT		rc;

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC)ZLog::ZLogProc;
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= GetModuleHandle( NULL );
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH ) ;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= "ZLogWindow";

	RegisterClass(&wc);

	cwidth = ZLOG_WIDTH;
	cheight = ZLOG_HEIGHT;
	x = GetSystemMetrics( SM_CXSCREEN ) - cwidth;
	y = 0;

	m_hWnd = CreateWindow( "ZLogWindow", ZLOG_TITLE, WS_POPUP | WS_CAPTION | WS_MINIMIZEBOX, x, y, cwidth, cheight, NULL, NULL, GetModuleHandle( NULL ), NULL );
	GetClientRect( m_hWnd, &rc );
	m_hWndListBox = CreateWindow( "LISTBOX", "", WS_CHILD | WS_VSCROLL, 0, 0, rc.right, rc.bottom, m_hWnd, NULL, GetModuleHandle( NULL ), NULL );
	ShowWindow( m_hWnd, SW_HIDE );
	ShowWindow( m_hWndListBox, SW_SHOW );
}

//---------------------------------------------------------------------------

HWND ZLog::GetHWND()
{
	return m_hWnd;
}

//---------------------------------------------------------------------------

void ZLog::MoveZLogWIndow( char* pTitle, int iX, int iY, int iWidth, int iHeight )
{
	RECT rcChild;

	if( iWidth == 0 )
		iWidth = GetSystemMetrics( SM_CXSCREEN ) - iX;
	if( iHeight == 0 )
		iHeight = ZLOG_HEIGHT;

	MoveWindow( m_hWnd, iX, iY, iWidth, iHeight, TRUE );
	GetClientRect(m_hWnd, &rcChild);
	MoveWindow( m_hWndListBox, 0, 0, rcChild.right, rcChild.bottom, TRUE );
	if( pTitle != NULL )
		SetWindowText( m_hWnd, pTitle );
}

//---------------------------------------------------------------------------

void ZLog::ShowZLogWindow( BOOL bShow )
{
	if( bShow )
		ShowWindow( m_hWnd, SW_SHOW );
	else
		ShowWindow( m_hWnd, SW_HIDE );
}

//---------------------------------------------------------------------------

int ZLog::Log( LPCSTR fmt, ... )
{

#ifdef __USE_ZLOG__
	va_list args;
	int len = 0;
	char *pBuf = NULL, *pTime = NULL;
	char time[25], temp[128], temp2[5];
	ZString zLogText;
	ZString zTimeText;
    __time64_t ltime;

	// 시간 구하기
	// Sat Jun 03 12:12:42 2006
    _time64( &ltime );
	memset( time, 0, 25 );
	pTime = _ctime64( &ltime );
	memcpy( time, pTime, 24 );
	// 년
	memset( temp, 0, 128 );
	memcpy( temp, &time[20], 4 );
	zTimeText = temp;
	// 월
	memset( temp, 0, 128 );
	memset( temp2, 0, 5 );
	_strdate( temp );
	memcpy( temp2, temp, 2 );
	zTimeText += temp2;
	// 일
	memset( temp, 0, 128 );
	memcpy( temp, &time[8], 2 );
	zTimeText += temp;
	// 시:분:초
	memset( temp, 0, 128 );
	memcpy( temp, &time[10], 9 );
	zTimeText += temp;

	// Log 텍스트
	va_start( args, fmt );
	len = _vscprintf( fmt, args )	// _vscprintf doesn't count
                               + 1; // terminating '\0'
	pBuf = new char[len];
	memset( pBuf, 0, len );
	vsprintf( pBuf, fmt, args );

	

	if( m_nTarget & ZLOG_CONSOLE )
	{
		printf( "[%s] : %s\n", zTimeText.c_str(), pBuf );
	}
	else if( m_nTarget & ZLOG_FILE )
	{
		zLogText.Format( "[%s] : %s\n", zTimeText.c_str(), pBuf );
		m_pLogFile->WriteLine( zLogText );
	}

	if( m_nTarget & ZLOG_WINDOW )
	{
		SendMessage( m_hWndListBox, LB_ADDSTRING, 0, (LPARAM) pBuf );
		UINT32 n = (UINT32)SendMessage( m_hWndListBox, LB_GETCOUNT, 0, 0L ) - 1;
		SendMessage( m_hWndListBox, LB_SETCURSEL, (WPARAM)n, 0L );
	}

	SAFE_DELETE_ARRAY( pBuf );
#endif
	return 1;
}

//---------------------------------------------------------------------------

LRESULT CALLBACK ZLog::ZLogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return DefWindowProc( hwnd, uMsg, wParam, lParam );
}

//---------------------------------------------------------------------------
