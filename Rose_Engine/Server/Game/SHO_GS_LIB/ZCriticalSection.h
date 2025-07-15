
/*---------------------------------------------------------------------------
Make:		2007.03.27
Name:		ZCriticalSection
Author:     남병철.레조

Desc:		크리티컬 섹션 객체 << 상속용 >>
            상속 받은 객체가 생성될때 크리티컬 섹션 초기화
			소멸될때 제거
			 Ex) ZMultiThreadSync 생성시 작동
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

