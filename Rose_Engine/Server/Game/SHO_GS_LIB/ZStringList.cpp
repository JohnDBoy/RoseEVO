
//---------------------------------------------------------------------------

#include "ZSTD.h"

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// ZStringList
//---------------------------------------------------------------------------

ZStringList::ZStringList()
{
	m_pList = new ZList();
}

//---------------------------------------------------------------------------

ZStringList::~ZStringList()
{
	Clear();
	SAFE_DELETE( m_pList );
}

//---------------------------------------------------------------------------

ZString ZStringList::IndexOf( int iIndex )
{
	int iTotalCount = Count();
	ZString zReturn;
	ZString* pzReturn = NULL;
	void* pString = NULL;

	// �ε��� äũ
	if( !( 0 <= iIndex && iIndex < iTotalCount ) )
		return zReturn;

	pString = m_pList->IndexOf( iIndex );
	if( pString == NULL ) return zReturn;

	pzReturn = static_cast<ZString*>( pString );

	return *pzReturn;
}

//---------------------------------------------------------------------------

int ZStringList::Count()
{
	return m_pList->Count();
}

//---------------------------------------------------------------------------

BOOL ZStringList::Add( ZString zAdd )
{
	ZString* pString = NULL;

	if( (pString = new ZString()) != NULL )
	{
		*pString = zAdd;

		return m_pList->Add( (void*)pString );
	}

	return FALSE;
}

//---------------------------------------------------------------------------

BOOL ZStringList::Insert( int iIndex, ZString zInsert )
{
	int iTotalCount = Count();
	ZString* pString = NULL;

	// �ε��� äũ
	if( !( 0 <= iIndex && iIndex <= iTotalCount ) )
		return FALSE;

	if( (pString = new ZString()) != NULL )
	{
		*pString = zInsert;

		return m_pList->Insert( iIndex, pString );
	}

	return FALSE;
}

//---------------------------------------------------------------------------

BOOL ZStringList::Delete( int iIndex )
{
	int iTotalCount = Count();
	void* pReturn = NULL;

	// �ε��� äũ
	if( !( 0 <= iIndex && iIndex < iTotalCount ) )
		return FALSE;

	pReturn = m_pList->IndexOf( iIndex );
	if( pReturn == NULL ) return NULL;
	/*pString = static_cast<ZString*>( pReturn );*/
	SAFE_DELETE( pReturn );

	return m_pList->Delete( iIndex );
}

//---------------------------------------------------------------------------
// All Node Clear (�� ��尡 �Ҵ�� �޸𸮶�� Clear�� ���� delete�ʿ�)

int ZStringList::Clear()
{
	void* pString = NULL;

	// �� ��忡 �Ҵ�� ����Ÿ(ZString*) ����
	pString = m_pList->IndexOf( 0 );
	
	if( pString == NULL ) return 0;
	do
	{
		SAFE_DELETE( pString );
		pString = m_pList->NextNode();
	} while( pString != NULL );

	return m_pList->Clear();
}

//---------------------------------------------------------------------------

BOOL ZStringList::Exchange( int iIndex1, int iIndex2 )
{
	return m_pList->Exchange( iIndex1, iIndex2 );
}

//---------------------------------------------------------------------------
// ���� : ã�� Index

int ZStringList::Find( ZString zFind )
{
	void* pString = NULL;

	pString = m_pList->IndexOf( 0 );
	// // �߰��� ��尡 ����.
	if( pString == NULL )
		return -1;
	do
	{
		if( *(static_cast<ZString*>(pString)) == zFind )
			return m_pList->GetCurIndex();
		pString = m_pList->NextNode();
	} while( pString != NULL );

	return -1;
}

//---------------------------------------------------------------------------
