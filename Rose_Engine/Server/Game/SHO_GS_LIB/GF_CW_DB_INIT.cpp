//#ifdef __CLAN_WAR_SET			// Ŭ���� �߰� ����

////	Ŭ���� ��û �� ��� DB ���� ó�� �Լ� �ʱ�ȭ
//	�ۼ��� : �迵ȯ


#include "GF_Define.h"
#include "GF_Data.h"


//	�ۼ��� : �迵ȯ

////	DB ���� �б�
BOOL	DB_CW_Init_Load(char *p_Part,char *file_name,char *p_IP,char *pID,char *pPass,char *p_Table)
{
	// G_Server_ID
	if(p_Part == NULL) return false;	// INI �κ� �̸�
	if(file_name == NULL) return false;	// ���� �̸�
	if(p_IP == NULL) return false;		// ���� IP
	if(pID == NULL) return false;		// ���� ID
	if(pPass == NULL) return false;		// ���� Pass
	if(p_Table == NULL) return false;	// ���� ���̺�

	ZeroMemory(p_IP,DB_INI_STRING);
	ZeroMemory(pID,DB_INI_STRING);
	ZeroMemory(pPass,DB_INI_STRING);
	ZeroMemory(p_Table,DB_INI_STRING);
	char	Temp_ID[DB_INI_STRING];
	ZeroMemory(Temp_ID,DB_INI_STRING);

	//	���� ��ȣ ���
	GetPrivateProfileString(p_Part, "DBSERVERID", "", Temp_ID, DB_INI_STRING, file_name);
	if (strlen(Temp_ID) == 0) return false;
	G_Server_ID = (DWORD)atoi(Temp_ID);

	//	ä�� ��ȣ ���
	ZeroMemory(Temp_ID,DB_INI_STRING);
	GetPrivateProfileString(p_Part, "CHANNEL", "", Temp_ID, DB_INI_STRING, file_name);
	if (strlen(Temp_ID) == 0) return false;
	G_SV_Channel = (DWORD)atoi(Temp_ID);

	//	IP �ּ� �б�
	GetPrivateProfileString(p_Part, "DBIP", "", p_IP, DB_INI_STRING, file_name);
	if (strlen(p_IP) == 0) return false;
	//	ID �б�
	GetPrivateProfileString(p_Part, "DBID", "", pID, DB_INI_STRING, file_name);
	if (strlen(pID) == 0) return false;
	//	Pass �б�
	GetPrivateProfileString(p_Part, "DBPS", "", pPass, DB_INI_STRING, file_name);
	if (strlen(pPass) == 0) return false;
	//	���̺� �б�
	GetPrivateProfileString(p_Part, "DBTB", "", p_Table, DB_INI_STRING, file_name);
	if (strlen(p_Table) == 0) return false;
	return true;
}

////	DB ����.
BOOL	GF_Connect_DB_CW(HWND	p_Main_H)
{
	//	�ʱ�ȭ
	GF_DisConnect_DB_CW();
	
	// DB ���� ����
	G_SQL_CWar_Buffer = new F_Buffer;
	G_SQL_CWar_Buffer->Create_Buffer(DB_BUFFER,10000);
	
	//	���� ó�� Ŭ���� �����
	G_SQL_CWar_Thread = new F_Thread;
	G_SQL_CWar_Thread->Create((LPTHREAD_START_ROUTINE)GF_CWar_Thread_Proc,NULL);
	
	////	DB ���� Ŭ����
	char	Temp_IP[DB_INI_STRING];
	char	Temp_ID[DB_INI_STRING];
	char	Temp_PASS[DB_INI_STRING];
	char	Temp_TABLE[DB_INI_STRING];
	char	Temp_File[MAX_PATH];
	char	Temp_Part[DB_INI_STRING];
	ZeroMemory(Temp_File,MAX_PATH);
	ZeroMemory(Temp_Part,DB_INI_STRING);
	//	���Ͽ��� �б�
	wsprintf(Temp_File,"%s\\SHO_GS_DB.ini",G_Main_Dir);

	//	DB ���� ���� ���
	wsprintf(Temp_Part,"CWARDB");
	DB_CW_Init_Load(Temp_Part,Temp_File,Temp_IP,Temp_ID,Temp_PASS,Temp_TABLE);

	//	DB ����.
	G_SQL_CWar = new KDB_SQL;
	if(!G_SQL_CWar->Connect(p_Main_H,Temp_IP,Temp_ID,Temp_PASS,Temp_TABLE))
		return false;
	return true;
}

////	DB ����.
BOOL	GF_DisConnect_DB_CW()
{
	//	������ ���� Ȯ��.
	if(G_SQL_CWar_Thread != NULL)
	{
		G_SQL_CWar_Thread->Ex_ID = 0;
		SetEvent(G_SQL_CWar_Thread->Event_H);
		Sleep(1000);
		delete G_SQL_CWar_Thread;
		G_SQL_CWar_Thread = NULL;
	}

	//	���� ���� Ȯ��.
	if(G_SQL_CWar_Buffer != NULL)
	{
		delete G_SQL_CWar_Buffer;
		G_SQL_CWar_Buffer = NULL;
	}

	//	BD ���� ����
	if(G_SQL_CWar != NULL)
	{
		delete G_SQL_CWar;
		G_SQL_CWar = NULL;
	}
	return true;
}

//#endif
