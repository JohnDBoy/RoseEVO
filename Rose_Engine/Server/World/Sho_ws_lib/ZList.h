/*---------------------------------------------------------------------------
Make:		2006.05.15
Name:		ZList.h
Author:     남병철.레조

Desc:		ZList 클래스 (Circular Queue)
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
	sNode* m_pFirst;	// 노드 시작
	sNode* m_pLast;		// 노드 끝
	sNode* m_pCurrent;	// 현재 가리키는 노드 위치

public:
	ZList();
	~ZList();

	void* First();
	void* Last();
	void* IndexOf( int iIndex );	// m_pCurrent 세팅
	void* NextNode();				// m_pCurrent->Next
	void* PrevNode();				// m_pCurrent->Prev
	//int Pack();						// NULL 데이타 노드 삭제

	int Count();
	BOOL Add( void* pData );
	BOOL Insert( int iIndex, void* pData );
	BOOL Delete( int iIndex );
	BOOL DeleteData( int iIndex );
	int Clear();	// All Node Clear (각 노드가 할당된 메모리라면 Clear전 직접 delete필요)
	BOOL Exchange( int iIndex1, int iIndex2 );
	int GetCurIndex();	// m_pCurrent의 인덱스 리턴
	int Find( void* pData );
};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
