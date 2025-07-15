/*---------------------------------------------------------------------------
Make:		2006.05.15
Name:		ZStringList.h
Author:     남병철.레조

Desc:		ZStringList 클래스

** 수에 대한 업그레이드 계획 **
수의 경우 스트링 리스트에서 MakeNumberCache( INT/FLOAT/DOUBLE )를 사용하여
숫자로 이루어진 스트링리스트를 숫자 배열로 캐쉬를 만든다.
그리고 Add, Delete, Insert등의 데이타 변화 메소드가 사용되기 전까지
IndexOf, Count, Exchange, Find를 캐쉬 배열을 이용하여 처리한다.
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
	int Clear();	// All Node Clear (각 노드가 할당된 메모리라면 Clear전 직접 delete필요)
	BOOL Exchange( int iIndex1, int iIndex2 );
	int Find( ZString zFind );

	//BOOL MakeNumberCache();
};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
