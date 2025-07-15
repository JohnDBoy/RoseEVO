// Client.cpp : Defines the entry point for the application.


//*-------------------------------------------------------------------------------------------------------------------*
// 2005 / 06 / 20 : nAvy 
// 사용자 정의 전처리기 설명 : 
//  1. _TAIWAN  : TAIWAN, PHILIPHIN 접속시 로그인서버에게 국가 코드( 현재 950 )를 보낼지를 구분하기 위한 정의
//  2. _DE		: 중복실행 가능한 버젼인지에 대한 정의( KR )에만 적용시킨다.
//
// 20060703 raning
// 전처리기 stdafx.h에 설정.
//*-------------------------------------------------------------------------------------------------------------------*

#include "stdafx.h"

#include "CApplication.h"
#include "Game.h"

#include "Network\\CNetwork.h"
#include "Util\\VFSManager.h"
#include "Util\\SystemInfo.h"
#include "Util\\classCRC.h"
#include "CClientStorage.h"
#include "System/CGame.h"
#include "Interface/ExternalUI/CLogin.h"
#include "Debug.h"
#include "TriggerInfo.h"

#include "Country.h"
#include "MiniDumper.h"
#include "Util/Cencryptdecrypt.h"
#include "Hasher.h"


//*--------------------------------------------------------------------------------------*/
// 95,98, me에서 Unicode관련 Api Wrapper dll Load
//HMODULE LoadUnicowsProc(void); 
//HMODULE g_hUnicows = NULL;
//#ifdef _cplusplus
//extern "C" {
//#endif
//extern FARPROC _PfnLoadUnicows = (FARPROC) &LoadUnicowsProc;
//#ifdef _cplusplus
//}
//#endif
//
//
//// Unicode Load
//HMODULE LoadUnicowsProc(void)
//{
//	g_hUnicows = LoadLibraryA("unicows.dll");
//    return g_hUnicows;
//}
//*--------------------------------------------------------------------------------------*/
typedef struct sHashComp
{
	unsigned int h1;
	unsigned int h2;
	unsigned short h3;
} sHashComp;

typedef union uHash
{
	sHashComp hashVal;
	BYTE hashByte[ 20 ];
} uHash;

//-------------------------------------------------------------------------------------------------
bool Init_DEVICE (void)
{
	bool bRet = false;

	//--------------------------[ engine related ]-----------------------//
	::initZnzin();
	::openFileSystem("data.idx");


	::doScript("scripts/init.lua");

	t_OptionResolution Resolution = g_ClientStorage.GetResolution();
	::setDisplayQualityLevel( c_iPeformances[g_ClientStorage.GetVideoPerformance()] );

	g_ClientStorage.ApplyAntiAliasingOption( g_ClientStorage.GetVideoAntiAliasing() );

	if(!g_pCApp->IsFullScreenMode()) 
	{
		RECT ClientRt;
		GetClientRect(g_pCApp->GetHWND(),&ClientRt);
		::setScreen(ClientRt.right, ClientRt.bottom, Resolution.iDepth, g_pCApp->IsFullScreenMode() );		
	}
	else
		::setScreen(g_pCApp->GetWIDTH(), g_pCApp->GetHEIGHT(), Resolution.iDepth, g_pCApp->IsFullScreenMode() );


	bRet = ::attachWindow((const void*)g_pCApp->GetHWND());


	CD3DUtil::Init( );

	g_pSoundLIST = new CSoundLIST( g_pCApp->GetHWND() );
	g_pSoundLIST->Load ( "3DDATA\\STB\\FILE_SOUND.stb" );

	return bRet;
}



//-------------------------------------------------------------------------------------------------
void Free_DEVICE (void)
{	
	delete g_pSoundLIST;

	CD3DUtil::Free ();

	//--------------------------[ engine related ]-----------------------//
	::detachWindow();

	::closeFileSystem();
	::destZnzin();  
}

#ifdef __ARCFOUR

bool	g_bArcfourSucessed = false;

// Load Arcfour Key.
DWORD WINAPI MyThreadProc( LPVOID param )
{	

	g_bArcfourSucessed = false;

#ifdef _DEBUG	

	//키 파일에서 키를 읽는다.

	int			nKeyLen = 0;
	Crypt		crypt;

	//서버에서 쓰는 키 파일을 읽는다.
	crypt = g_EncryptDecrypt.EncryptDataLoad_Server( "key.txt" );
	if( crypt.bKeyJudgment )
	{
		g_pCApp->SetArcfourKey( crypt.sCyrptKey.c_str(), crypt.size );
		g_bArcfourSucessed = true;
		return 0;
	}	

	//릴리즈 실행 파일에서 키를 읽는다.
	crypt = g_EncryptDecrypt.EncryptDataLoad( "TRose.exe" );

	if( crypt.bKeyJudgment )
	{
		g_pCApp->SetArcfourKey( crypt.sCyrptKey.c_str(), crypt.size );
		g_bArcfourSucessed = true;
		return 0;
	}	

#else
	//07. 01. 17 - 김주현 : 당분간 아크포는 사용하지 않는다.(패킷헤더가 달라서 서버에서 
	//잘라버리는 경우가 빈번하게 발생하여 수정할때까지 사용하지 않는다. 패킷헤더는 사용한다. 다만 패킷을 암호화 하지 않을뿐...)
	g_bArcfourSucessed = true;

#ifdef __RELEASE_TEST
	crypt = g_EncryptDecrypt.EncryptDataLoad_Server( "key.txt" );
	if( crypt.bKeyJudgment )
	{
		g_pCApp->SetArcfourKey( crypt.sCyrptKey.c_str(), crypt.size );
		g_bArcfourSucessed = true;
		return 0;
	}	
#endif

#endif	

	// 키 읽기 실패.
	return 0;
}

#endif

//-------------------------------------------------------------------------------------------------
int APIENTRY WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
#ifdef _USE_BG
	//	SetExceptionReport();
#endif


#ifndef _DEBUG
	// *-------------------------------------------------------------------* //
	// 2005. 5. 6. 조호동
	HANDLE AppMutex = NULL;

	AppMutex = ::OpenMutex( MUTEX_ALL_ACCESS, false, __BUILD_REGION_NAME );
	if( AppMutex != NULL )
	{
		::ReleaseMutex( AppMutex);
		::CloseHandle( AppMutex);
		MessageBox( NULL, "Prevention of double execution", "TRose", MB_OK | MB_ICONERROR);
		return 0;
	}
#endif

	// TSVFS.dll의 CRC를 체크하장!!
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __TEST
#else
#ifndef _DEBUG
	classCRC crc;
	std::string	m_csFileName;

	TCHAR tstrFileName[255];

	DWORD dwSize = 0;

	uHash hashOut;
	unsigned int hashlength = 0;


	GetModuleFileName(NULL, tstrFileName, 255);
	m_csFileName = tstrFileName;

///////////////<crc check tsvfs.dll>
	char TSVFSDLL[1024];

	GetModuleFileName( GetModuleHandle("TSVFS.dll"), TSVFSDLL, 1024);

	dwSize = crc.FileCRC32(TSVFSDLL);
	if(dwSize != 0x2F6AA378)
	{
		MessageBox( NULL, "Runtime 2 error!", "TRose", MB_OK | MB_ICONERROR);
		return 0;
	}
///////////////<\crc check tsvfs.dll>
#endif  //_DEBUG

#endif	//__TEST


	//-------------------------------------------------------------------------------
	/// Init System object
	//-------------------------------------------------------------------------------
	g_pCApp		= CApplication::Instance ();




#ifdef __ARCFOUR

#ifdef __ARCFOURLOG
	FILE * pFile = fopen("ArcfourLog.txt", "wt");
	if(pFile)	fprintf( pFile, "Start Log\n" );
	fclose(pFile);
#endif	


	DWORD IDThread;
	HANDLE hThread; 
	//Arcfour thread start.
	hThread = CreateThread( NULL,
		0,
		(LPTHREAD_START_ROUTINE)MyThreadProc,
		NULL,
		0,
		&IDThread );
#endif
	// *-------------------------------------------------------------------* //


	/*_CrtSetDbgFlag (
	_CRTDBG_ALLOC_MEM_DF |
	_CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode ( _CRT_ERROR,
	_CRTDBG_MODE_DEBUG);*/


	//-------------------------------------------------------------------------------
	/// 국가코드 인식
	//-------------------------------------------------------------------------------
	CCountry::GetSingleton().CheckCountry();

	//KeyCrypt 실행 
	//if( !m_npKeyCrypt.InitKeyCrypt() )
	//	return 0;
	///m_npKeyCrypt.IsGetUse() = FALSE;


	g_SystemInfo.CollectingSystemInfo();
	int iWindowVersion = g_SystemInfo.GetWindowsVersion();



	//-------------------------------------------------------------------------------
	/// Init Trigger VFS
	//-------------------------------------------------------------------------------
	VHANDLE hVFS = OpenVFS( "data.idx", (iWindowVersion == WINDOWS_98) ? "r" : "mr" );	
	(CVFSManager::GetSingleton()).SetVFS( hVFS );
	(CVFSManager::GetSingleton()).InitVFS( VFS_TRIGGER_VFS );	


	 DWORD dwCurVersion = VGetCurVersion(hVFS);
	 char temp[32];
	 sprintf(temp, "%d", dwCurVersion);
	 
	 // 07. 1. 18 - 김주현 : 클라이언트 패치 버젼을 저장한다.
	 CGame::GetInstance().SetCurVersion(temp);
	//-------------------------------------------------------------------------------
	/// Get Time
	//-------------------------------------------------------------------------------
	GetLocalTime(	&g_GameDATA.m_SystemTime );	


	//-------------------------------------------------------------------------------
	/// Init System object
	//-------------------------------------------------------------------------------
	//	g_pCApp		= CApplication::Instance ();

#ifdef __ARCFOUR

	//CNetwork 생성하기 전에 Arcfour Key 읽어 온다.
	WaitForSingleObject( hThread, INFINITE );
	CloseHandle( hThread );

	//Arcfour key 읽지를 못하면 클라이언트를 끝낸다.
	if( g_bArcfourSucessed == false )	
	{
		(CVFSManager::GetSingleton()).ReleaseAll();
		g_pCApp->Destroy ();

		MessageBox(NULL, "Fail - 1004", "TRose", MB_OK | MB_ICONERROR);

		return 0;
	}

#endif
	g_pNet		= CNetwork::Instance (hInstance);


	g_pCRange	= CRangeTBL::Instance ();


	//-------------------------------------------------------------------------------
	/// Load Range table
	//-------------------------------------------------------------------------------
	if ( !g_pCRange->Load_TABLE ("3DDATA\\TERRAIN\\O_Range.TBL") ) {
		g_pCApp->ErrorBOX ( "3DDATA\\TERRAIN\\O_Range.TBL file open error", CUtil::GetCurrentDir (), MB_OK);
		return 0;
	}	


	//-------------------------------------------------------------------------------
	/// IP/Port Setting
	//-------------------------------------------------------------------------------
	g_GameDATA.m_wServerPORT = TCP_LSV_PORT;
	g_GameDATA.m_ServerIP.Set( TCP_LSV_IP );

	if ( !g_pCApp->ParseArgument( lpCmdLine ) ) {
		MessageBox( NULL, "Arg error!", "TRose", MB_OK | MB_ICONERROR);
		return 0;
	}

	//-------------------------------------------------------------------------------
	/// 윈도우 생성시 해상도에 관한 Data가 필요하여 이곳에서 로드한다.
	//-------------------------------------------------------------------------------	
	g_TblResolution.Load2( "3DDATA\\STB\\RESOLUTION.STB",	false, false );
	g_TblCamera.Load2( "3DDATA\\STB\\LIST_CAMERA.STB" ,false, false );

	//-------------------------------------------------------------------------------
	///클라이언트에 저장된 Data를 로드한다.
	//-------------------------------------------------------------------------------
	g_ClientStorage.Load();

	//-------------------------------------------------------------------------------
	///이전 옵션에서 조정된 해상도의 인덱스를 가져와서 g_TblResolution을 참조하여
	///해상도를 조정한다.
	//-------------------------------------------------------------------------------	
#pragma message("You can run the stored resolution is determined by testing and initializing the message box does not work TriggerDetect Shall run")
	t_OptionResolution Resolution = g_ClientStorage.GetResolution();
	/// 범위값 체크 
	UINT iFullScreen = g_ClientStorage.GetVideoFullScreen();

	g_pCApp->SetFullscreenMode( iFullScreen );

	if(!CGame::GetInstance().GetCurVersion().empty())
		g_pCApp->CreateWND ("classCLIENT", CStr::Printf("%s [Ver. %s]", __BUILD_REGION_NAME, CGame::GetInstance().GetCurVersion().c_str()) , Resolution.iWidth, Resolution.iHeight,Resolution.iDepth, hInstance);
	else
		g_pCApp->CreateWND ("classCLIENT", __BUILD_REGION_NAME, Resolution.iWidth, Resolution.iHeight,Resolution.iDepth, hInstance);



#ifndef _DEBUG
	// 시스템 정보를 모음
	TI_ReadSysInfoFile ();

#endif


	// *-------------------------------------------------------------------* //
	g_pObjMGR = CObjectMANAGER::Instance ();
	g_pCApp->ResetExitGame();

	bool bDeviceInitialized = Init_DEVICE();

	if ( bDeviceInitialized ) {
		CGame::GetInstance().GameLoop();
	}
	else 
	{
		MessageBox( NULL, "Device error!", "TRose", MB_OK | MB_ICONERROR);
#ifdef _USE_BG
		ReportZnzinLog( "Failed to initialize. Do you want to transfer the cause?", 10 );
#endif
	}



	// *-------------------------------------------------------------------* //

	Free_DEVICE ();


	// *-------------------------------------------------------------------* //
	g_TblCamera.Free();
	g_TblResolution.Free();
	// *-------------------------------------------------------------------* //

	g_pCApp->Destroy ();
	g_pNet->Destroy ();

	//-------------------------------------------------------------------------------
	///2004/3/26/nAvy:Release 구성에서 Debug Mode(F5로실행)로 실행시 로그인 화면에서 취소하면
	///Error난다. 이유 모름 , 하지만 그냥 실행시에는 (bat파일로 혹은 ctrl+f5) Error가 안난다.
	//-------------------------------------------------------------------------------		
	g_pCRange->Destroy ();	

	/*if( g_hUnicows )
	{
	FreeLibrary( g_hUnicows );
	g_hUnicows = NULL;
	}*/

#ifndef _DEBUG
	if (AppMutex)
	{
		::ReleaseMutex(AppMutex);
		::CloseHandle(AppMutex);
	}
#endif

	return 0;
}



extern std::string sCurVersion;








//-------------------------------------------------------------------------------------------------
