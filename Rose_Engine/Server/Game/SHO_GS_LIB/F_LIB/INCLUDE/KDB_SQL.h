#pragma once
#include <sql.h>
#include <sqlext.h>

class KDB_SQL
{
public:
	HENV		m_hEnv;		//	ȯ���ڵ�
	HDBC		m_hDBC;		//	DB �����ڵ�
	bool		m_bIsConn;	//	���� ����
	int			m_QueryType;//	���� Ÿ��

	SQLSMALLINT m_Column;	//	�÷� ����
	char		m_szDataBase[MAX_PATH];	// DB �̸�

	//	DB ���� ����
	char m_IP[256];
	char m_ID[256];
	char m_PW[256];

public:
	KDB_SQL(void);
	~KDB_SQL(void);

	//	���� ����
	void Destroy();
	//	DB ����
	bool Connect(HWND hWnd, char* pcIP, char* pcID, char* pcPW,char* pcDB);
	//	��ɹ� ����
	bool Command(UCHAR *pch);
	bool Command(char *pch);
	int  Command_Return(char *pch);
	//	HSTMT ���
	BOOL	Alloc(HSTMT  *phStmt);
	//	������ ����
	BOOL	Exec(char *pch,HSTMT  phStmt);
	//	������ ���� ����
	BOOL	Free(HSTMT   phStmt);
	//	�÷� ���� ���
	int		Get_Cols_Num(HSTMT   phStmt);
	//	�÷��� ���ڿ��� ���ε� �Ѵ�.
	BOOL	Bind_Cols(HSTMT   phStmt,int nCol,char *Buff,int nBuffLen,char *Buff_Name);
	//	�÷��� ����Ʈ�� ���ε� �Ѵ�.
	BOOL	Bind_Cols_BINARY(HSTMT   phStmt,int nCol,char *Buff,int nBuffLen,char *Buff_Name,int nNameLen);
	//	���̳ʸ� ������ ���̵�
	BOOL	Bind_Parameter(HSTMT   phStmt,int Indx,int p_data_Size,char *p_data);
	//	���̳ʸ� ������ ������Ʈ ó��
	BOOL	Put_Cols_BINARY(HSTMT   phStmt,int indx,char *Buff,int nBuffLen);
	//	��Ÿ �Լ�
	SQLRETURN Fetch(HSTMT   phStmt);		// �� �� ��������
};