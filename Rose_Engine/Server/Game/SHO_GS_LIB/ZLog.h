/*---------------------------------------------------------------------------
Make:		2006.05.07
Name:		ZLog.h
Author:     남병철.레조

Desc:		로그 클래스
---------------------------------------------------------------------------*/

#ifndef __ZLOG_H__
#define __ZLOG_H__

//---------------------------------------------------------------------------
// 로그 위치

#define ZLOG_CONSOLE			0x00000001
#define ZLOG_FILE				0x00000002
#define ZLOG_WINDOW				0x00000004
#define ZLOG_ALL				(ZLOG_CONSOLE|ZLOG_FILE|ZLOG_WINDOW)

// 기본 ZLOG 윈도우 설정
#define ZLOG_TITLE				"MONITORING"
#define ZLOG_WIDTH				400
#define ZLOG_HEIGHT				800

//---------------------------------------------------------------------------

class ZString;
class ZLog
{
private:
	static LRESULT CALLBACK 
	ZLog::ZLogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	
	
protected:
	unsigned int	m_nTarget;
	ZFile*			m_pLogFile;
	HWND			m_hWnd;
	HWND			m_hWndListBox;
	void CreateZLogWindow();		// 로그 윈도우 생성 (기본:Hide 상태)

public:
	ZLog( UINT32 nTarget, LPSTR szFileName = NULL );
	~ZLog();

	HWND GetHWND();
	void MoveZLogWIndow( char* pTitle, int iX, int iY, int iWidth = 0, int iHeight = 0 );
	void ShowZLogWindow( BOOL bShow );
		
	int Log( LPCSTR fmt, ... );		// 로그 기록
};

//---------------------------------------------------------------------------

//extern ZLog * g_pLog;

// 설정 되지 않으면 Log(...) 함수의 내부는 코드에서 사라진다.
//#define __USE_ZLOG__

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
