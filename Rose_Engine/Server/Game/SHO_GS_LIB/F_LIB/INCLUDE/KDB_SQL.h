#pragma once
#include <sql.h>
#include <sqlext.h>

class KDB_SQL
{
public:
	HENV		m_hEnv;		//	환경핸들
	HDBC		m_hDBC;		//	DB 연결핸들
	bool		m_bIsConn;	//	연결 상태
	int			m_QueryType;//	퀘리 타입

	SQLSMALLINT m_Column;	//	컬럼 개수
	char		m_szDataBase[MAX_PATH];	// DB 이름

	//	DB 연결 정보
	char m_IP[256];
	char m_ID[256];
	char m_PW[256];

public:
	KDB_SQL(void);
	~KDB_SQL(void);

	//	연결 해지
	void Destroy();
	//	DB 연결
	bool Connect(HWND hWnd, char* pcIP, char* pcID, char* pcPW,char* pcDB);
	//	명령문 실행
	bool Command(UCHAR *pch);
	bool Command(char *pch);
	int  Command_Return(char *pch);
	//	HSTMT 허용
	BOOL	Alloc(HSTMT  *phStmt);
	//	쿼리문 제출
	BOOL	Exec(char *pch,HSTMT  phStmt);
	//	쿼리문 제출 해제
	BOOL	Free(HSTMT   phStmt);
	//	컬럼 개수 얻기
	int		Get_Cols_Num(HSTMT   phStmt);
	//	컬럼을 문자열에 바인드 한다.
	BOOL	Bind_Cols(HSTMT   phStmt,int nCol,char *Buff,int nBuffLen,char *Buff_Name);
	//	컬럼을 바이트에 바인드 한다.
	BOOL	Bind_Cols_BINARY(HSTMT   phStmt,int nCol,char *Buff,int nBuffLen,char *Buff_Name,int nNameLen);
	//	바이너리 데이터 바이드
	BOOL	Bind_Parameter(HSTMT   phStmt,int Indx,int p_data_Size,char *p_data);
	//	바이너리 데이터 없데이트 처리
	BOOL	Put_Cols_BINARY(HSTMT   phStmt,int indx,char *Buff,int nBuffLen);
	//	기타 함수
	SQLRETURN Fetch(HSTMT   phStmt);		// 한 행 가져오기
};