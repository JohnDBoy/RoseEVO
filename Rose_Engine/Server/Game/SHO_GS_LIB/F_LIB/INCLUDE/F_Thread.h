#pragma once

////	������ Ŭ����
class F_Thread
{
public:
	F_Thread(void);
	~F_Thread(void);
public:
	////	������ ���� ���� ����
	DWORD ID_Thread;	// ������ ID
	HANDLE Thread_H;	// ������ �ڵ�
	HANDLE Event_H;		// ������ �̺�Ʈ �ڵ�
	DWORD	Ex_ID;		// ���� ID

	////	������ ���� ���� �Լ�
	//	������ ����
	BOOL Create(LPTHREAD_START_ROUTINE Thread_Proc,LPVOID Thread_Param);
	//	������ ����
	BOOL Release(void);
	//	������ �ð��̺�Ʈ ó��
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