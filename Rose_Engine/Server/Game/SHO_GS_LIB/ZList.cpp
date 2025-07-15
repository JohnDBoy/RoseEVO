
//---------------------------------------------------------------------------

#include "ZSTD.h"

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// ZList
//---------------------------------------------------------------------------

ZList::ZList()
{
	m_pFirst = NULL;
	m_pLast = NULL;
	m_pCurrent = NULL;
}

//---------------------------------------------------------------------------

ZList::~ZList()
{
	Clear();
}

//---------------------------------------------------------------------------
// Public
//---------------------------------------------------------------------------

void* ZList::First()
{
	return m_pFirst->pData;
}

//---------------------------------------------------------------------------

void* ZList::Last()
{
	return m_pLast->pData;
}

//---------------------------------------------------------------------------

void* ZList::IndexOf( int iIndex )
{
	int iCount = Count();
	//int iCurIndex = GetCurIndex();

	// 인덱스 범위 검사
	// 시작 노드
	if( iIndex == 0 )
	{
		m_pCurrent = m_pFirst;
	}
	// 끝 노드
	else if( iIndex == (iCount-1) )
	{
		m_pCurrent = m_pLast;
	}
	// 가운데 노드
	else if( 0 < iIndex && iIndex < (iCount-1) )
	{
		sNode* pNode = m_pFirst;

		for( int i = 0; i < iIndex; i++ )
			pNode = pNode->Next;
	
		m_pCurrent = pNode;
	}
	else
		return NULL;

	// Clear() 후에 프로그램 종료시 객체가 delete 될때 IndexOf(0)일 경우 아래에 해당한다.
	if( m_pCurrent == NULL )
		return NULL;

	return m_pCurrent->pData;
}

//---------------------------------------------------------------------------
// NextNode가 NULL이면 현재 노드가 마지막 노드이다.

void* ZList::NextNode()
{
	sNode* pReturn = m_pCurrent->Next;

	if( pReturn != NULL )
	{
		m_pCurrent = m_pCurrent->Next;
		return pReturn->pData;
	}

	return NULL;
}

//---------------------------------------------------------------------------
// PrevNode가 NULL이면 현재 노드가 시작 노드이다.

void* ZList::PrevNode()
{
	sNode* pReturn = m_pCurrent->Prev;

	if( pReturn != NULL )
	{
        m_pCurrent = m_pCurrent->Prev;
		return m_pCurrent->pData;
	}

	return NULL;
}

//---------------------------------------------------------------------------
// 리턴값 : 노드 데이타가 NULL이라서 삭제된 노드 개수

//int ZList::Pack()
//{
//	int iTotalCount = 0;
//	sNode* pNode = m_pFirst;
//	sNode* pTempNode = NULL;
//
//	while( pNode != NULL )
//	{
//		pTempNode = pNode;
//		pNode = pNode->Next;
//
//		if( pTempNode->pData == NULL )
//		{
//			SAFE_DELETE( pTempNode );
//			iTotalCount++;
//		}
//	}
//
//	return iTotalCount;
//}

//---------------------------------------------------------------------------

int ZList::Count()
{
	int iCount = 0;
	sNode* pNode = m_pFirst;

	// 시작노드 채크
	if( m_pFirst == NULL )
		return 0;
	else
		iCount++;

	// 노드 개수 채크
	while( pNode->Next != NULL )
	{
		iCount++;
		pNode = pNode->Next;
	}

	return iCount;
}

//---------------------------------------------------------------------------
// 리턴 : 노드 인덱스

BOOL ZList::Add( void* pData )
{
	sNode* pTempNode = NULL;

	if( m_pLast == NULL )
	{
		// 시작 노드 생성
		if( (pTempNode = new sNode()) != NULL )
		{
			// 데이타 설정
			pTempNode->pData = pData;
			// 링크 설정
			m_pFirst = m_pLast = m_pCurrent = pTempNode;
			m_pFirst->Prev = NULL;	// 시작 노드의 Prev는 NULL
			m_pLast->Next = NULL;	// 끝 노드의 Next는 NULL
		}
		else
		{
			m_pCurrent = m_pFirst;	// 초기 위치
			return FALSE;
		}
	}
	else
	{
		// 추가 노드 생성
		if( (pTempNode = new sNode()) != NULL )
		{
			// 데이타 설정
			pTempNode->pData = pData;
			// 링크 설정
			pTempNode->Prev = m_pLast;
			pTempNode->Next = NULL;		// 끝 노드의 Next는 NULL
			m_pLast->Next = pTempNode;
			m_pLast = pTempNode;
			m_pCurrent = pTempNode;	// 추가된 아이템을 m_pCurrent가 가리킴
		}
		else
		{
			m_pCurrent = m_pFirst;
			return FALSE;
		}		
	}

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL ZList::Insert( int iIndex, void* pData )
{
	int iTotalCount = Count();
	sNode* pTempNode = NULL;

	// 인덱스 범위 검사
	if( 0 <= iIndex && iIndex <= iTotalCount )
	{
		// 추가 위치 : 초기 생성
		if( iIndex == 0 && m_pFirst == NULL )
		{
			// 시작 노드 생성
			if( (pTempNode = new sNode()) != NULL )
			{
				// 데이타 설정
				pTempNode->pData = pData;
				// 링크 설정
				m_pFirst = m_pLast = m_pCurrent = pTempNode;
				m_pFirst->Prev = NULL;	// 시작 노드의 Prev는 NULL
				m_pLast->Next = NULL;	// 끝 노드의 Next는 NULL
			}
			else
			{
				m_pCurrent = m_pFirst;
				return FALSE;
			}	
		}
		// 추가 위치 : 시작
		else if( iIndex == 0 && m_pFirst != NULL )
		{
			// 시작 노드에 추가
			if( (pTempNode = new sNode()) != NULL )
			{
				// 데이타 설정
				pTempNode->pData = pData;
				// 링크 설정
				m_pFirst->Prev = pTempNode;
				pTempNode->Next = m_pFirst;
				pTempNode->Prev = NULL;
				m_pFirst = pTempNode;
				m_pCurrent = m_pFirst;	// 추가된 아이템을 m_pCurrent가 가리킴
			}
			else
			{
				m_pCurrent = m_pFirst;
				return FALSE;
			}	
		}
		// 추가 위치 : 가운데
		else if( 0 < iIndex && iIndex < iTotalCount )
		{
			// 가운데 노드 생성
			if( (pTempNode = new sNode()) != NULL )
			{
				// 데이타 설정
				pTempNode->pData = pData;				
				// m_pCurrent를 추가할 인덱스로 이동
				IndexOf( iIndex );				
				// 링크 설정
				m_pCurrent->Prev->Next = pTempNode;
				pTempNode->Prev = m_pCurrent->Prev;
				pTempNode->Next = m_pCurrent;
				m_pCurrent->Prev = pTempNode;
				m_pCurrent = pTempNode;	// 추가된 아이템을 m_pCurrent가 가리킴
			}
			else
			{
				m_pCurrent = m_pFirst;
				return FALSE;
			}	
		}
		// 추가 위치 : 끝
		else if( iIndex == iTotalCount )
		{
			// 시작 노드 생성
			if( (pTempNode = new sNode()) != NULL )
			{
				// 데이타 설정
				pTempNode->pData = pData;
				// 링크 설정
				pTempNode->Prev = m_pLast;
				pTempNode->Next = NULL;	// 끝 노드의 Next는 NULL
				m_pLast->Next = pTempNode;
				m_pLast = pTempNode;
				m_pCurrent = pTempNode;	// 추가된 아이템을 m_pCurrent가 가리킴
			}
			else
			{
				m_pCurrent = m_pFirst;
				return FALSE;
			}	
		}
		else
		{
			m_pCurrent = m_pFirst;
			return FALSE;
		}
	}
	else
	{
		m_pCurrent = m_pFirst;
		return FALSE;
	}

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL ZList::Delete( int iIndex )
{
	sNode* pTemp = NULL;
	int iTotalCount = Count();


	// 인덱스 범위 검사
	if( 0 <= iIndex && iIndex < iTotalCount )
	{
		// 삭제 위치 : 시작
		if( iIndex == 0 && m_pFirst != NULL )
		{
			if( iTotalCount > 1 )
			{
				// 링크 설정
				pTemp = m_pFirst;
				m_pFirst->Next->Prev = NULL;
				m_pFirst = m_pFirst->Next;
				m_pCurrent = m_pFirst;	// 삭제된 아이템의 대체 노드를 m_pCurrent가 가리킴
			}
			else
			{
				// 마지막 노드를 첫번째 인자로 삭제
				m_pCurrent = m_pFirst = m_pLast = NULL;
			}

			// 노드 삭제
			SAFE_DELETE( pTemp );
		}
		// 삭제 위치 : 가운데 ~ 끝
		else if( 0 < iIndex && iIndex < iTotalCount )
		{
			// m_pCurrent를 삭제할 인덱스로 이동
			IndexOf( iIndex );
			pTemp = m_pCurrent;

			// 끝 노드 삭제
			if( m_pCurrent == m_pLast )
			{
				// 링크설정
				m_pLast = m_pCurrent->Prev;
				m_pLast->Next = NULL;
				m_pCurrent = m_pLast;	// 삭제된 아이템의 대체 노드를 m_pCurrent가 가리킴
			}
			// 중간 노드 삭제
			else
			{
				// 링크 설정
				m_pCurrent->Prev->Next = m_pCurrent->Next;
				m_pCurrent->Next->Prev = m_pCurrent->Prev;
				m_pCurrent = m_pCurrent->Next;	// 삭제된 아이템의 대체 노드를 m_pCurrent가 가리킴
			}

			// 노드 삭제
			SAFE_DELETE( pTemp );	
		}
		else
		{
			m_pCurrent = m_pFirst;
			return FALSE;
		}
	}
	else
		m_pCurrent = m_pFirst;

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL ZList::DeleteData( int iIndex )
{
	int iCount = Count();
	//int iCurIndex = GetCurIndex();

	// 인덱스 범위 검사
	// 시작 노드
	if( iIndex == 0 )
	{
		m_pCurrent = m_pFirst;
		SAFE_DELETE( m_pCurrent->pData );
	}
	// 끝 노드
	else if( iIndex == (iCount-1) )
	{
		m_pCurrent = m_pLast;
		SAFE_DELETE( m_pCurrent->pData );
	}
	// 가운데 노드
	else if( 0 < iIndex && iIndex < (iCount-1) )
	{
		sNode* pNode = m_pFirst;

		for( int i = 0; i < iIndex; i++ )
			pNode = pNode->Next;
	
		m_pCurrent = pNode;
		SAFE_DELETE( m_pCurrent->pData );
	}
	else
		return FALSE;

	// Clear() 후에 프로그램 종료시 객체가 delete 될때 IndexOf(0)일 경우 아래에 해당한다.
	if( m_pCurrent == NULL )
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------
// All Node Clear (각 노드가 할당된 메모리라면 Clear전 직접 delete필요)
// 리턴값 : 삭제된 노드 수

int ZList::Clear()
{
	int iTotalCount = 0;
	sNode* pNode = m_pFirst;
	sNode* pTempNode = NULL;


	while( pNode != NULL )
	{
		pTempNode = pNode;
		pNode = pNode->Next;
		SAFE_DELETE( pTempNode );
		iTotalCount++;
	}

	// 기본 포인터 초기화
	m_pFirst = m_pLast = m_pCurrent = NULL;

	return iTotalCount;
}

//---------------------------------------------------------------------------
// 데이타 바꿈

int ZList::Exchange( int iIndex1, int iIndex2 )
{
	int iTotalCount = Count();
	void* pData1 = NULL;
	void* pData2 = NULL;

	// 인덱스 범위 검사
	if( (0 <= iIndex1 && iIndex1 < iTotalCount) &&
		(0 <= iIndex2 && iIndex2 < iTotalCount) )
	{
		pData1 = IndexOf( iIndex1 );
		pData2 = IndexOf( iIndex2 );

		// 데이타 교환
		IndexOf( iIndex1 );
		m_pCurrent->pData = pData2;
		IndexOf( iIndex2 );
		m_pCurrent->pData = pData1;
	}
	else
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------

int ZList::GetCurIndex()
{
	int iIndex = 0;
	sNode* pNode = m_pFirst;


	// 현재 인덱스 계산
	while( pNode != m_pCurrent )
	{
		iIndex++;
		pNode = pNode->Next;
	}

	// 결과 에러 채크
	if( pNode == m_pCurrent )
		return iIndex;
	else
		return -1;
}

//---------------------------------------------------------------------------
// 리턴 : 찾은 포인터 인덱스 (없으면 : -1)

int ZList::Find( void* pData )
{
	int iIndex = 0;
	sNode* pNode = m_pFirst;

	// 시작노드 채크
	if( m_pFirst == NULL )
		return -1;

	// 데이타 찾기
	if( pNode->pData == pData )
		return iIndex;
	else
		iIndex++;
	while( pNode->Next != NULL )
	{
		pNode = pNode->Next;
		if( pNode->pData == pData )
			return iIndex;
		else
			iIndex++;
	}

	return -1;
}

//---------------------------------------------------------------------------
