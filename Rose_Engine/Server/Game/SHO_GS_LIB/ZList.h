/*---------------------------------------------------------------------------
Make:		2006.05.15
Name:		ZList.h
Author:     ����ö.����

Desc:		ZList Ŭ���� (Circular Queue)
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __ZLIST_H__
#define __ZLIST_H__

//---------------------------------------------------------------------------

class ZList
{
private:
    struct sNode
	{
        void* pData;
		sNode* Prev;
        sNode* Next;

		sNode()
		{
			pData = NULL;
			Prev = Next = NULL;
		}
		~sNode()
		{
			pData = NULL;
			Prev = Next = NULL;
		}
    };
	sNode* m_pFirst;	// ��� ����
	sNode* m_pLast;		// ��� ��
	sNode* m_pCurrent;	// ���� ����Ű�� ��� ��ġ

public:
	ZList();
	~ZList();

	void* First();
	void* Last();
	void* IndexOf( int iIndex );	// m_pCurrent ����
	void* NextNode();				// m_pCurrent->Next
	void* PrevNode();				// m_pCurrent->Prev
	//int Pack();						// NULL ����Ÿ ��� ����

	int Count();
	BOOL Add( void* pData );
	BOOL Insert( int iIndex, void* pData );
	BOOL Delete( int iIndex );
	BOOL DeleteData( int iIndex );
	int Clear();	// All Node Clear (�� ��尡 �Ҵ�� �޸𸮶�� Clear�� ���� delete�ʿ�)
	BOOL Exchange( int iIndex1, int iIndex2 );
	int GetCurIndex();	// m_pCurrent�� �ε��� ����
	int Find( void* pData );
};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
