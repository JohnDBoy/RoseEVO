
/*
	Ŭ�� �ʵ� ���� �Լ� �߰� 
	2006�� 10�� 16�� : �ۼ��� �迵ȯ
*/
#define	MAX_CF_SET_N	5	//	�ִ� Ŭ�� �ʵ� ���� ����

//	Ŭ�� �ʵ� ���� ������.
typedef struct _CF_Setting
{
	//	NPC ����.
	DWORD	m_NPC_Zone;
	DWORD	m_NPC_No;
	LPVOID	m_NPC;

	//	���� �ð� ó��
	DWORD	m_Start_Time[2];	// ���� ���۰� ���� ����.
	
	//	���� �ð� ó��
	DWORD	m_End_Time[2];		// ���� �ð��� ���� �˸� ���� �ð� 1�� ���� �뺸.

	//	��ȯ ��ġ
	WORD	m_ReturnNO[5];		// ��ȯ �� ��ȣ
	tPOINTF m_ReturnPos[5];		// ��ȯ �� ��ġ
} CF_Setting;

////	Ŭ�� ��Ʈ ũ��Ƽ�� ����
BOOL	GF_CF_CRITICAL_Set(bool pType,HWND	p_HWND);

////	Ŭ�� �ʵ� ���� ���� �б�
BOOL	GF_CF_Set_INI_Load();

////	Ŭ�� �ʵ� ���� NPC ã�� �Լ�
BOOL	GF_CF_INI_NPC_Find();

////	Ŭ�� �ʵ� �̺�Ʈ ��
BOOL	GF_CF_Event_View();

////	Ŭ�� �ʵ� ���� Ÿ�� �Լ�
BOOL	GF_CF_Time_Proc();

////	Ŭ�� �ʵ� ���� NPC ���� ����.
BOOL	GF_CF_NPC_Var_Set(BOOL p_Type,int p_List);

////	Ŭ�� �ʵ� ���� �뺸 ó��
BOOL	GF_CF_End_Time_Send();

////	Ŭ�� �ʵ� ���� ���� ���� ��Ŵ
BOOL	GF_CF_End_Warp();

////	Ŭ�� �ʵ� ���� ���� ��Ŵ
BOOL	GF_CF_End_User_Warp(classUSER *p_User,bool Type);