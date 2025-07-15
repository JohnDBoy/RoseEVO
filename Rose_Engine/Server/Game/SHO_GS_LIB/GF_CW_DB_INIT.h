//#ifdef __CLAN_WAR_SET			// 클랜전 추가 정의

////	클랜전 신청 및 결과 DB 저장 처리 함수 초기화
//	작성자 : 김영환

////	DB 정보 읽기
BOOL	DB_CW_Init_Load(char *p_Part,char *file_name,char *p_IP,char *pName,char *pPass,char *p_Table);

////	DB 연결.
BOOL	GF_Connect_DB_CW(HWND	p_Main_H);

////	DB 해지.
BOOL	GF_DisConnect_DB_CW();

//#ifdef __CLAN_WAR_SET			// 클랜전 추가 정의