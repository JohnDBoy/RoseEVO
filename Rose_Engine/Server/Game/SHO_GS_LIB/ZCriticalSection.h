
/*---------------------------------------------------------------------------
Make:		2007.03.27
Name:		ZCriticalSection
Author:     ����ö.����

Desc:		ũ��Ƽ�� ���� ��ü << ��ӿ� >>
            ��� ���� ��ü�� �����ɶ� ũ��Ƽ�� ���� �ʱ�ȭ
			�Ҹ�ɶ� ����
			 Ex) ZMultiThreadSync ������ �۵�
            (ZCriticalSection <- ZThreadSync <- ZMultiThreadSync)
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __ZCRITICALSECTION_H__
#define __ZCRITICALSECTION_H__

//---------------------------------------------------------------------------

class ZCriticalSection
{
public:
	ZCriticalSection(VOID)
	{
		InitializeCriticalSection( &m_Sync );
	}

	~ZCriticalSection(VOID)
	{
		DeleteCriticalSection( &m_Sync );
	}

	inline VOID Enter(VOID)
	{
		EnterCriticalSection( &m_Sync );
	}

	inline VOID Leave(VOID)
	{
		LeaveCriticalSection( &m_Sync );
	}

private:
	CRITICAL_SECTION	m_Sync;
};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

