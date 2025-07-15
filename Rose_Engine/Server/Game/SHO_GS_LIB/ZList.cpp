
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

	// �ε��� ���� �˻�
	// ���� ���
	if( iIndex == 0 )
	{
		m_pCurrent = m_pFirst;
	}
	// �� ���
	else if( iIndex == (iCount-1) )
	{
		m_pCurrent = m_pLast;
	}
	// ��� ���
	else if( 0 < iIndex && iIndex < (iCount-1) )
	{
		sNode* pNode = m_pFirst;

		for( int i = 0; i < iIndex; i++ )
			pNode = pNode->Next;
	
		m_pCurrent = pNode;
	}
	else
		return NULL;

	// Clear() �Ŀ� ���α׷� ����� ��ü�� delete �ɶ� IndexOf(0)�� ��� �Ʒ��� �ش��Ѵ�.
	if( m_pCurrent == NULL )
		return NULL;

	return m_pCurrent->pData;
}

//---------------------------------------------------------------------------
// NextNode�� NULL�̸� ���� ��尡 ������ ����̴�.

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
// PrevNode�� NULL�̸� ���� ��尡 ���� ����̴�.

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
// ���ϰ� : ��� ����Ÿ�� NULL�̶� ������ ��� ����

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

	// ���۳�� äũ
	if( m_pFirst == NULL )
		return 0;
	else
		iCount++;

	// ��� ���� äũ
	while( pNode->Next != NULL )
	{
		iCount++;
		pNode = pNode->Next;
	}

	return iCount;
}

//---------------------------------------------------------------------------
// ���� : ��� �ε���

BOOL ZList::Add( void* pData )
{
	sNode* pTempNode = NULL;

	if( m_pLast == NULL )
	{
		// ���� ��� ����
		if( (pTempNode = new sNode()) != NULL )
		{
			// ����Ÿ ����
			pTempNode->pData = pData;
			// ��ũ ����
			m_pFirst = m_pLast = m_pCurrent = pTempNode;
			m_pFirst->Prev = NULL;	// ���� ����� Prev�� NULL
			m_pLast->Next = NULL;	// �� ����� Next�� NULL
		}
		else
		{
			m_pCurrent = m_pFirst;	// �ʱ� ��ġ
			return FALSE;
		}
	}
	else
	{
		// �߰� ��� ����
		if( (pTempNode = new sNode()) != NULL )
		{
			// ����Ÿ ����
			pTempNode->pData = pData;
			// ��ũ ����
			pTempNode->Prev = m_pLast;
			pTempNode->Next = NULL;		// �� ����� Next�� NULL
			m_pLast->Next = pTempNode;
			m_pLast = pTempNode;
			m_pCurrent = pTempNode;	// �߰��� �������� m_pCurrent�� ����Ŵ
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

	// �ε��� ���� �˻�
	if( 0 <= iIndex && iIndex <= iTotalCount )
	{
		// �߰� ��ġ : �ʱ� ����
		if( iIndex == 0 && m_pFirst == NULL )
		{
			// ���� ��� ����
			if( (pTempNode = new sNode()) != NULL )
			{
				// ����Ÿ ����
				pTempNode->pData = pData;
				// ��ũ ����
				m_pFirst = m_pLast = m_pCurrent = pTempNode;
				m_pFirst->Prev = NULL;	// ���� ����� Prev�� NULL
				m_pLast->Next = NULL;	// �� ����� Next�� NULL
			}
			else
			{
				m_pCurrent = m_pFirst;
				return FALSE;
			}	
		}
		// �߰� ��ġ : ����
		else if( iIndex == 0 && m_pFirst != NULL )
		{
			// ���� ��忡 �߰�
			if( (pTempNode = new sNode()) != NULL )
			{
				// ����Ÿ ����
				pTempNode->pData = pData;
				// ��ũ ����
				m_pFirst->Prev = pTempNode;
				pTempNode->Next = m_pFirst;
				pTempNode->Prev = NULL;
				m_pFirst = pTempNode;
				m_pCurrent = m_pFirst;	// �߰��� �������� m_pCurrent�� ����Ŵ
			}
			else
			{
				m_pCurrent = m_pFirst;
				return FALSE;
			}	
		}
		// �߰� ��ġ : ���
		else if( 0 < iIndex && iIndex < iTotalCount )
		{
			// ��� ��� ����
			if( (pTempNode = new sNode()) != NULL )
			{
				// ����Ÿ ����
				pTempNode->pData = pData;				
				// m_pCurrent�� �߰��� �ε����� �̵�
				IndexOf( iIndex );				
				// ��ũ ����
				m_pCurrent->Prev->Next = pTempNode;
				pTempNode->Prev = m_pCurrent->Prev;
				pTempNode->Next = m_pCurrent;
				m_pCurrent->Prev = pTempNode;
				m_pCurrent = pTempNode;	// �߰��� �������� m_pCurrent�� ����Ŵ
			}
			else
			{
				m_pCurrent = m_pFirst;
				return FALSE;
			}	
		}
		// �߰� ��ġ : ��
		else if( iIndex == iTotalCount )
		{
			// ���� ��� ����
			if( (pTempNode = new sNode()) != NULL )
			{
				// ����Ÿ ����
				pTempNode->pData = pData;
				// ��ũ ����
				pTempNode->Prev = m_pLast;
				pTempNode->Next = NULL;	// �� ����� Next�� NULL
				m_pLast->Next = pTempNode;
				m_pLast = pTempNode;
				m_pCurrent = pTempNode;	// �߰��� �������� m_pCurrent�� ����Ŵ
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


	// �ε��� ���� �˻�
	if( 0 <= iIndex && iIndex < iTotalCount )
	{
		// ���� ��ġ : ����
		if( iIndex == 0 && m_pFirst != NULL )
		{
			if( iTotalCount > 1 )
			{
				// ��ũ ����
				pTemp = m_pFirst;
				m_pFirst->Next->Prev = NULL;
				m_pFirst = m_pFirst->Next;
				m_pCurrent = m_pFirst;	// ������ �������� ��ü ��带 m_pCurrent�� ����Ŵ
			}
			else
			{
				// ������ ��带 ù��° ���ڷ� ����
				m_pCurrent = m_pFirst = m_pLast = NULL;
			}

			// ��� ����
			SAFE_DELETE( pTemp );
		}
		// ���� ��ġ : ��� ~ ��
		else if( 0 < iIndex && iIndex < iTotalCount )
		{
			// m_pCurrent�� ������ �ε����� �̵�
			IndexOf( iIndex );
			pTemp = m_pCurrent;

			// �� ��� ����
			if( m_pCurrent == m_pLast )
			{
				// ��ũ����
				m_pLast = m_pCurrent->Prev;
				m_pLast->Next = NULL;
				m_pCurrent = m_pLast;	// ������ �������� ��ü ��带 m_pCurrent�� ����Ŵ
			}
			// �߰� ��� ����
			else
			{
				// ��ũ ����
				m_pCurrent->Prev->Next = m_pCurrent->Next;
				m_pCurrent->Next->Prev = m_pCurrent->Prev;
				m_pCurrent = m_pCurrent->Next;	// ������ �������� ��ü ��带 m_pCurrent�� ����Ŵ
			}

			// ��� ����
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

	// �ε��� ���� �˻�
	// ���� ���
	if( iIndex == 0 )
	{
		m_pCurrent = m_pFirst;
		SAFE_DELETE( m_pCurrent->pData );
	}
	// �� ���
	else if( iIndex == (iCount-1) )
	{
		m_pCurrent = m_pLast;
		SAFE_DELETE( m_pCurrent->pData );
	}
	// ��� ���
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

	// Clear() �Ŀ� ���α׷� ����� ��ü�� delete �ɶ� IndexOf(0)�� ��� �Ʒ��� �ش��Ѵ�.
	if( m_pCurrent == NULL )
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------
// All Node Clear (�� ��尡 �Ҵ�� �޸𸮶�� Clear�� ���� delete�ʿ�)
// ���ϰ� : ������ ��� ��

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

	// �⺻ ������ �ʱ�ȭ
	m_pFirst = m_pLast = m_pCurrent = NULL;

	return iTotalCount;
}

//---------------------------------------------------------------------------
// ����Ÿ �ٲ�

int ZList::Exchange( int iIndex1, int iIndex2 )
{
	int iTotalCount = Count();
	void* pData1 = NULL;
	void* pData2 = NULL;

	// �ε��� ���� �˻�
	if( (0 <= iIndex1 && iIndex1 < iTotalCount) &&
		(0 <= iIndex2 && iIndex2 < iTotalCount) )
	{
		pData1 = IndexOf( iIndex1 );
		pData2 = IndexOf( iIndex2 );

		// ����Ÿ ��ȯ
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


	// ���� �ε��� ���
	while( pNode != m_pCurrent )
	{
		iIndex++;
		pNode = pNode->Next;
	}

	// ��� ���� äũ
	if( pNode == m_pCurrent )
		return iIndex;
	else
		return -1;
}

//---------------------------------------------------------------------------
// ���� : ã�� ������ �ε��� (������ : -1)

int ZList::Find( void* pData )
{
	int iIndex = 0;
	sNode* pNode = m_pFirst;

	// ���۳�� äũ
	if( m_pFirst == NULL )
		return -1;

	// ����Ÿ ã��
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
