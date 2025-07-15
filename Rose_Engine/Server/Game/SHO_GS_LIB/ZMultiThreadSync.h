
/*---------------------------------------------------------------------------
Make:		2007.03.27
Name:		ZMultiThreadSync
Author:     ����ö.����

Desc:		��Ƽ ������ ��ũ << ��ӿ� >>
            ��� �޴� Ŭ������ ����Ÿ ���ۿ� ����ȭ�� �ʿ��Ҷ� ���
			����ȭ ó���Ǵ� �޼ҵ忡 ZThreadSync ��ü�� �����ϸ� m_Sync��
			���� ����ȭ ó���ȴ�.
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

