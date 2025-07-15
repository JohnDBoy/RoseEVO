#pragma once

////	스레드 클래스
class F_Thread
{
public:
	F_Thread(void);
	~F_Thread(void);
public:
	////	스레드 관련 내장 변수
	DWORD ID_Thread;	// 쓰레드 ID
	HANDLE Thread_H;	// 쓰레드 핸드
	HANDLE Event_H;		// 쓰레드 이벤트 핸들
	DWORD	Ex_ID;		// 종료 ID

	////	쓰레드 관련 내장 함수
	//	쓰레드 생성
	BOOL Create(LPTHREAD_START_ROUTINE Thread_Proc,LPVOID Thread_Param);
	//	쓰레드 종료
	BOOL Release(void);
	//	쓰레드 시간이벤트 처리
	void Wait_Event(DWORD Wait_Time);
};
/*

BOOL GetProcessAffinityMask(
  HANDLE hProcess,                  // handle to process
  PDWORD_PTR lpProcessAffinityMask, // process affinity mask
  PDWORD_PTR lpSystemAffinityMask   // system affinity mask
);

DWORD_PTR SetThreadAffinityMask (
  HANDLE hThread,                 // handle to thread
  DWORD_PTR dwThreadAffinityMask  // thread affinity mask
);

DWORD SetThreadIdealProcessor(
  HANDLE hThread,         // handle to the thread
  DWORD dwIdealProcessor  // ideal processor number
);
*/