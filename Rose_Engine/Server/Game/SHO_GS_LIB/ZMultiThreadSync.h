
/*---------------------------------------------------------------------------
Make:		2007.03.27
Name:		ZMultiThreadSync
Author:     남병철.레조

Desc:		멀티 쓰레드 싱크 << 상속용 >>
            상속 받는 클래스의 데이타 조작에 동기화가 필요할때 사용
			동기화 처리되는 메소드에 ZThreadSync 객체를 생성하면 m_Sync에
			의해 동기화 처리된다.
            (ZCriticalSection <- ZThreadSync <- ZMultiThreadSync)
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __ZMULTITHREADSYNC_H__
#define __ZMULTITHREADSYNC_H__

//---------------------------------------------------------------------------

template <class T>
class ZMultiThreadSync
{
	friend class ZThreadSync;

private:
	static ZCriticalSection m_Sync;

public:
	class ZThreadSync
	{
	public:
		ZThreadSync(VOID)
		{
			T::m_Sync.Enter();
		}

		~ZThreadSync(VOID)
		{
			T::m_Sync.Leave();
		}
	};
};

template <class T>
ZCriticalSection ZMultiThreadSync<T>::m_Sync;

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

