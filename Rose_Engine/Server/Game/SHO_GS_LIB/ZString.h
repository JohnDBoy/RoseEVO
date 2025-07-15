/*---------------------------------------------------------------------------
Make:		2006.03.23
Name:		ZString.h
Author:     남병철.레조

Desc:		ZString 클래스
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __ZSTRING_H__
#define __ZSTRING_H__

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class ZRefCount
{
private:
	int m_iCount;
	char* m_pszString;

public:
	ZRefCount( const char* const pS );
	~ZRefCount();

	// GET
	char* GetString();
	int GetCount();
	int GetLength();

	// SET
	void IncCount();
	void DecCount();
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class ZString
{
private:
	ZRefCount* m_pCount;

public:
	ZString();
	ZString( const char* const pS );
	ZString( const ZString& S );
	ZString( const short sString );
	ZString( const unsigned short usString );
	ZString( const long lString );
	ZString( const unsigned long ulString );
	ZString( const int iString );
	ZString( const unsigned int uiString );
	ZString( const float fString );
	ZString( const double lfString );
	~ZString();

	ZString& operator = ( const ZString& S );
	ZString& operator + ( const ZString& S );
	ZString& operator += ( const ZString& S );
	BOOL operator == ( const ZString& S );		// Compare
	BOOL operator != ( const ZString& S );		// Compare

	// 볼랜드 헬프에서 설명을 주석으로 사용
	// C string operator
	char* c_str() const;
	int Length();
	BOOL IsEmpty();	// TRUE : Empty, FALSE : Not Empty
	int ToInt();
	double ToDouble();
	float ToFloat();
	BOOL UpperCase();
	BOOL LowerCase();
	BOOL Trim( char* pToken = NULL );
	BOOL TrimLeft( char* pToken = NULL );
	BOOL TrimRight( char* pToken = NULL );

	// Returns the index at which a specified substring begins.
	// 없으면 -1 리턴
	int Pos( const char* subStr );
	int Pos( ZString& subStr );

	// Returns a specified substring of the ZString
	ZString SubString( int Index, int Count );
	ZString Token( ZString zDelimiter );
	ZString LastToken( ZString zDelimiter );
	BOOL Format( const char* fmt, ... );
	ZString& Insert( ZString& str, int Index );
	ZString& Delete( int Index, int Count );

	//Condition  Return Value 
	//S1 > S2 [ > 0 ]
	//S1 < S2 [ < 0 ]
	//S1 = S2 [ = 0 ] 
	int Compare( ZString& S2 );		// Compares the AnsiString to another specified string (case-sensitively)
	int CompareIC( ZString& S2 );	// Compares the AnsiString to another specified string (case insensitively)
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------