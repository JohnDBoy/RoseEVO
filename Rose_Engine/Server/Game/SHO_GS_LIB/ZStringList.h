/*---------------------------------------------------------------------------
Make:		2006.05.15
Name:		ZStringList.h
Author:     ����ö.����

Desc:		ZStringList Ŭ����

** ���� ���� ���׷��̵� ��ȹ **
���� ��� ��Ʈ�� ����Ʈ���� MakeNumberCache( INT/FLOAT/DOUBLE )�� ����Ͽ�
���ڷ� �̷���� ��Ʈ������Ʈ�� ���� �迭�� ĳ���� �����.
�׸��� Add, Delete, Insert���� ����Ÿ ��ȭ �޼ҵ尡 ���Ǳ� ������
IndexOf, Count, Exchange, Find�� ĳ�� �迭�� �̿��Ͽ� ó���Ѵ�.
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __ZSTRINGLIST_H__
#define __ZSTRINGLIST_H__

//---------------------------------------------------------------------------

class ZStringList
{
private:
	ZList* m_pList;

protected:
public:
	ZStringList();
	~ZStringList();

	ZString IndexOf( int iIndex );
	int Count();
	BOOL Add( ZString zAdd );
	BOOL Insert( int iIndex, ZString zInsert );
	BOOL Delete( int iIndex );
	int Clear();	// All Node Clear (�� ��尡 �Ҵ�� �޸𸮶�� Clear�� ���� delete�ʿ�)
	BOOL Exchange( int iIndex1, int iIndex2 );
	int Find( ZString zFind );

	//BOOL MakeNumberCache();
};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
