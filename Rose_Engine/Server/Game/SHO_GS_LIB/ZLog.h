/*---------------------------------------------------------------------------
Make:		2006.05.07
Name:		ZLog.h
Author:     ����ö.����

Desc:		�α� Ŭ����
---------------------------------------------------------------------------*/

#ifndef __ZLOG_H__
#define __ZLOG_H__

//---------------------------------------------------------------------------
// �α� ��ġ

#define ZLOG_CONSOLE			0x00000001
#define ZLOG_FILE				0x00000002
#define ZLOG_WINDOW				0x00000004
#define ZLOG_ALL				(ZLOG_CONSOLE|ZLOG_FILE|ZLOG_WINDOW)

// �⺻ ZLOG ������ ����
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
	void CreateZLogWindow();		// �α� ������ ���� (�⺻:Hide ����)

public:
	ZLog( UINT32 nTarget, LPSTR szFileName = NULL );
	~ZLog();

	HWND GetHWND();
	void MoveZLogWIndow( char* pTitle, int iX, int iY, int iWidth = 0, int iHeight = 0 );
	void ShowZLogWindow( BOOL bShow );
		
	int Log( LPCSTR fmt, ... );		// �α� ���
};

//---------------------------------------------------------------------------

//extern ZLog * g_pLog;

// ���� ���� ������ Log(...) �Լ��� ���δ� �ڵ忡�� �������.
//#define __USE_ZLOG__

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
