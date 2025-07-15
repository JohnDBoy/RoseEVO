
/*
	클랜 필드 관리 함수 추가 
	2006년 10월 16일 : 작성자 김영환
*/
#define	MAX_CF_SET_N	5	//	최대 클랜 필드 설정 정보

//	클랜 필드 설정 데이터.
typedef struct _CF_Setting
{
	//	NPC 정보.
	DWORD	m_NPC_Zone;
	DWORD	m_NPC_No;
	LPVOID	m_NPC;

	//	입장 시간 처리
	DWORD	m_Start_Time[2];	// 입장 시작과 입장 종료.
	
	//	종료 시간 처리
	DWORD	m_End_Time[2];		// 종료 시간과 종료 알림 시작 시간 1분 간격 통보.

	//	귀환 위치
	WORD	m_ReturnNO[5];		// 귀환 존 번호
	tPOINTF m_ReturnPos[5];		// 귀환 존 위치
} CF_Setting;

////	클랜 필트 크리티컬 설정
BOOL	GF_CF_CRITICAL_Set(bool pType,HWND	p_HWND);

////	클랜 필드 설정 정보 읽기
BOOL	GF_CF_Set_INI_Load();

////	클랜 필드 관리 NPC 찾기 함수
BOOL	GF_CF_INI_NPC_Find();

////	클랜 필드 이벤트 뷰
BOOL	GF_CF_Event_View();

////	클랜 필드 관리 타입 함수
BOOL	GF_CF_Time_Proc();

////	클랜 필드 관리 NPC 변수 변경.
BOOL	GF_CF_NPC_Var_Set(BOOL p_Type,int p_List);

////	클랜 필드 종료 통보 처리
BOOL	GF_CF_End_Time_Send();

////	클랜 필드 게임 종료 워프 시킴
BOOL	GF_CF_End_Warp();

////	클랜 필드 유저 워프 시킴
BOOL	GF_CF_End_User_Warp(classUSER *p_User,bool Type);