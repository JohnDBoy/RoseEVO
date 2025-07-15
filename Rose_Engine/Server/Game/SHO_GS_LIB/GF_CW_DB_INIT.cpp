//#ifdef __CLAN_WAR_SET			// 클랜전 추가 정의

////	클랜전 신청 및 결과 DB 저장 처리 함수 초기화
//	작성자 : 김영환


#include "GF_Define.h"
#include "GF_Data.h"


//	작성자 : 김영환

////	DB 정보 읽기
BOOL	DB_CW_Init_Load(char *p_Part,char *file_name,char *p_IP,char *pID,char *pPass,char *p_Table)
{
	// G_Server_ID
	if(p_Part == NULL) return false;	// INI 부분 이름
	if(file_name == NULL) return false;	// 파일 이름
	if(p_IP == NULL) return false;		// 접속 IP
	if(pID == NULL) return false;		// 접속 ID
	if(pPass == NULL) return false;		// 접속 Pass
	if(p_Table == NULL) return false;	// 접속 테이블

	ZeroMemory(p_IP,DB_INI_STRING);
	ZeroMemory(pID,DB_INI_STRING);
	ZeroMemory(pPass,DB_INI_STRING);
	ZeroMemory(p_Table,DB_INI_STRING);
	char	Temp_ID[DB_INI_STRING];
	ZeroMemory(Temp_ID,DB_INI_STRING);

	//	서버 번호 얻기
	GetPrivateProfileString(p_Part, "DBSERVERID", "", Temp_ID, DB_INI_STRING, file_name);
	if (strlen(Temp_ID) == 0) return false;
	G_Server_ID = (DWORD)atoi(Temp_ID);

	//	채널 번호 얻기
	ZeroMemory(Temp_ID,DB_INI_STRING);
	GetPrivateProfileString(p_Part, "CHANNEL", "", Temp_ID, DB_INI_STRING, file_name);
	if (strlen(Temp_ID) == 0) return false;
	G_SV_Channel = (DWORD)atoi(Temp_ID);

	//	IP 주소 읽기
	GetPrivateProfileString(p_Part, "DBIP", "", p_IP, DB_INI_STRING, file_name);
	if (strlen(p_IP) == 0) return false;
	//	ID 읽기
	GetPrivateProfileString(p_Part, "DBID", "", pID, DB_INI_STRING, file_name);
	if (strlen(pID) == 0) return false;
	//	Pass 읽기
	GetPrivateProfileString(p_Part, "DBPS", "", pPass, DB_INI_STRING, file_name);
	if (strlen(pPass) == 0) return false;
	//	테이블 읽기
	GetPrivateProfileString(p_Part, "DBTB", "", p_Table, DB_INI_STRING, file_name);
	if (strlen(p_Table) == 0) return false;
	return true;
}

////	DB 연결.
BOOL	GF_Connect_DB_CW(HWND	p_Main_H)
{
	//	초기화
	GF_DisConnect_DB_CW();
	
	// DB 정보 버퍼
	G_SQL_CWar_Buffer = new F_Buffer;
	G_SQL_CWar_Buffer->Create_Buffer(DB_BUFFER,10000);
	
	//	일정 처리 클래스 만들기
	G_SQL_CWar_Thread = new F_Thread;
	G_SQL_CWar_Thread->Create((LPTHREAD_START_ROUTINE)GF_CWar_Thread_Proc,NULL);
	
	////	DB 연결 클래스
	char	Temp_IP[DB_INI_STRING];
	char	Temp_ID[DB_INI_STRING];
	char	Temp_PASS[DB_INI_STRING];
	char	Temp_TABLE[DB_INI_STRING];
	char	Temp_File[MAX_PATH];
	char	Temp_Part[DB_INI_STRING];
	ZeroMemory(Temp_File,MAX_PATH);
	ZeroMemory(Temp_Part,DB_INI_STRING);
	//	파일에서 읽기
	wsprintf(Temp_File,"%s\\SHO_GS_DB.ini",G_Main_Dir);

	//	DB 연결 정보 얻기
	wsprintf(Temp_Part,"CWARDB");
	DB_CW_Init_Load(Temp_Part,Temp_File,Temp_IP,Temp_ID,Temp_PASS,Temp_TABLE);

	//	DB 연결.
	G_SQL_CWar = new KDB_SQL;
	if(!G_SQL_CWar->Connect(p_Main_H,Temp_IP,Temp_ID,Temp_PASS,Temp_TABLE))
		return false;
	return true;
}

////	DB 해지.
BOOL	GF_DisConnect_DB_CW()
{
	//	스레드 삭제 확인.
	if(G_SQL_CWar_Thread != NULL)
	{
		G_SQL_CWar_Thread->Ex_ID = 0;
		SetEvent(G_SQL_CWar_Thread->Event_H);
		Sleep(1000);
		delete G_SQL_CWar_Thread;
		G_SQL_CWar_Thread = NULL;
	}

	//	버퍼 삭제 확인.
	if(G_SQL_CWar_Buffer != NULL)
	{
		delete G_SQL_CWar_Buffer;
		G_SQL_CWar_Buffer = NULL;
	}

	//	BD 연결 해지
	if(G_SQL_CWar != NULL)
	{
		delete G_SQL_CWar;
		G_SQL_CWar = NULL;
	}
	return true;
}

//#endif
