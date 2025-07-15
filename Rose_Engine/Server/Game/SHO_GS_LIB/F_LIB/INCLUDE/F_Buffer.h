#pragma once
/*
	���� Ŭ����
	�ۼ��� : �迵ȯ
*/
class F_Buffer
{
public:
	F_Buffer(void);
	~F_Buffer(void);

	////	Ŭ���� ���� ����
	char	*m_Buffer;		//	���� �޸� ����Ʈ
	char	*m_In_Buffer;	//	�Է¿� ����
	char	*m_Out_Buffer;	//	��¿� ����
	DWORD	m_Buffer_Size;	//	1�� ���� ũ��	
	DWORD	m_Buffer_Number;//	��ü ���� ����
	DWORD	m_Input_N;		//	�Է� ��ġ ������
	DWORD	m_Output_N;		//	��� ��ġ ������
	BOOL	m_Loop_TF;		//	���� ���� ����
	DWORD	m_Buffer_ID;	//	���� ���̵�.

	////	Ŭ���� ���� �Լ�
	BOOL Create_Buffer(DWORD B_Size, DWORD B_Number);	// ���� ���� �Լ�
	BOOL Create_Buffer(DWORD B_Size, DWORD B_Number,DWORD Sub_B_Size_N);
	BOOL Delete_Buffer(void);	// ���� ���� �Լ�
	BOOL Input_Buffer(char *i_Buffer);	// ���ۿ� ������ �Է�
	BOOL Output_Buffer(char *o_Buffer);	// ���ۿ��� ������ ������
};
