#pragma once
/*
	버퍼 클래스
	작성자 : 김영환
*/
class F_Buffer
{
public:
	F_Buffer(void);
	~F_Buffer(void);

	////	클래스 내장 변수
	char	*m_Buffer;		//	버퍼 메모리 포인트
	char	*m_In_Buffer;	//	입력용 버퍼
	char	*m_Out_Buffer;	//	출력용 버퍼
	DWORD	m_Buffer_Size;	//	1개 버퍼 크기	
	DWORD	m_Buffer_Number;//	전체 버퍼 개수
	DWORD	m_Input_N;		//	입력 위치 포인터
	DWORD	m_Output_N;		//	출력 위치 포인터
	BOOL	m_Loop_TF;		//	루프 설정 변수
	DWORD	m_Buffer_ID;	//	버퍼 아이디.

	////	클래스 내장 함수
	BOOL Create_Buffer(DWORD B_Size, DWORD B_Number);	// 버퍼 생성 함수
	BOOL Create_Buffer(DWORD B_Size, DWORD B_Number,DWORD Sub_B_Size_N);
	BOOL Delete_Buffer(void);	// 버퍼 삭제 함수
	BOOL Input_Buffer(char *i_Buffer);	// 버퍼에 데이터 입력
	BOOL Output_Buffer(char *o_Buffer);	// 버퍼에서 데이터 꺼내기
};
