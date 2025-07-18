// TriggerDetect.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "TriggerDetect.h"
#include "TriggerDetectDlg.h"
#include "TriggerInfo.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTriggerDetectApp

BEGIN_MESSAGE_MAP(CTriggerDetectApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTriggerDetectApp 생성

CTriggerDetectApp::CTriggerDetectApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CTriggerDetectApp 개체입니다.

CTriggerDetectApp theApp;
LPCSTR g_CmdLine = NULL;

// CTriggerDetectApp 초기화

BOOL CTriggerDetectApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControls()가 필요합니다. 
	// InitCommonControls()를 사용하지 않으면 창을 만들 수 없습니다.

	///[TRIGGER] DETECTED가 1이면 그냥 CLIENT실행
	
	bool bSysDataFile = _access (SYSINFO_FILE, 0) == 0 && _access (ADPTINFO_FILE, 0) == 0;

	g_CmdLine = theApp.m_lpCmdLine;
	
	if(!TI_DXInfo())
	{
		MessageBox (NULL,"DirectX 9.0b is required! ","Error",MB_OK);
		return FALSE;
	}

	if(bSysDataFile)
	{
		if( theApp.m_lpCmdLine && strlen( theApp.m_lpCmdLine ) > 1 
			&& GetPrivateProfileInt( "TRIGGER","DETECTED", 0, "./SevenHearts.ini" ) == 1)
		{	

			ShellExecute( NULL, NULL, "TRose.exe", theApp.m_lpCmdLine, NULL, SW_SHOWNORMAL );	
			return false;
		}
	}
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성한 응용 프로그램"));

	CTriggerDetectDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 대화 상자가 확인을 눌러 없어지는 경우 처리할
		// 코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 대화 상자가 취소를 눌러 없어지는 경우 처리할
		// 코드를 배치합니다.
	}

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고
	// 응용 프로그램을 끝낼 수 있도록 FALSE를 반환합니다.
	
	return FALSE;
}
