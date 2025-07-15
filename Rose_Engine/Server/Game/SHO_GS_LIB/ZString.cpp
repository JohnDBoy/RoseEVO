
//---------------------------------------------------------------------------

#include "stdio.h"
#include <mbstring.h>	// _mbscmp, _mbsicmp
#include "ZSTD.h"

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// lzRefCount
//---------------------------------------------------------------------------

ZRefCount::ZRefCount( const char* const pS )
{
	int iLength = static_cast<int>( strlen( pS ) );
	m_pszString = new char[ iLength + 1 ];
	strcpy( m_pszString, pS );
	m_iCount = 1;
}

//---------------------------------------------------------------------------

ZRefCount::~ZRefCount()
{
	SAFE_DELETE_ARRAY( m_pszString );
	//m_pszString = NULL;
}

//---------------------------------------------------------------------------

char* ZRefCount::GetString()
{
	return m_pszString;
}

//---------------------------------------------------------------------------

int ZRefCount::GetCount()
{
	return m_iCount;
}

//---------------------------------------------------------------------------

int ZRefCount::GetLength()
{
	return static_cast<int>( strlen( m_pszString ) );
}

//---------------------------------------------------------------------------

void ZRefCount::IncCount()
{
	++m_iCount;
}

//---------------------------------------------------------------------------

void ZRefCount::DecCount()
{
	--m_iCount;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// ZString
//---------------------------------------------------------------------------

ZString::ZString() 
{
	m_pCount = new ZRefCount( "" );
}

//---------------------------------------------------------------------------

ZString::ZString( const char* const pS ) 
{
	if( pS == NULL )
		m_pCount = new ZRefCount( "" );
	else
		m_pCount = new ZRefCount( pS );
}

//---------------------------------------------------------------------------

ZString::ZString( const ZString& S )
{
	m_pCount = S.m_pCount;
	m_pCount->IncCount();
}

//---------------------------------------------------------------------------

ZString::ZString( const short sString )
{
	m_pCount = new ZRefCount( "" );
	Format( "%d", sString );
}

//---------------------------------------------------------------------------

ZString::ZString( const unsigned short usString )
{
	m_pCount = new ZRefCount( "" );
	Format( "%u", usString );
}

//---------------------------------------------------------------------------

ZString::ZString( const long lString )
{
	m_pCount = new ZRefCount( "" );
	Format( "%ld", lString );
}

//---------------------------------------------------------------------------

ZString::ZString( const unsigned long ulString )
{
	m_pCount = new ZRefCount( "" );
	Format( "%lu", ulString );
}

//---------------------------------------------------------------------------

ZString::ZString( const int iString )
{
	m_pCount = new ZRefCount( "" );
	Format( "%d", iString );
}

//---------------------------------------------------------------------------

ZString::ZString( const unsigned int uiString )
{
	m_pCount = new ZRefCount( "" );
	Format( "%u", uiString );
}

//---------------------------------------------------------------------------

ZString::ZString( const float fString )
{
	m_pCount = new ZRefCount( "" );
	Format( "%7.7f", fString );
	Trim("0");
}

//---------------------------------------------------------------------------

ZString::ZString( const double lfString )
{
	m_pCount = new ZRefCount( "" );
	Format( "%15.15lf", lfString );
	Trim("0");
}

//---------------------------------------------------------------------------

ZString::~ZString() 
{
	if( m_pCount->GetCount() == 1 )
	{
		SAFE_DELETE( m_pCount );
	}
	else
		m_pCount->DecCount();
}

//---------------------------------------------------------------------------
// ZString 대입

ZString& ZString::operator = ( const ZString& S ) 
{
	if( m_pCount->GetCount() == 1 )
	{
		SAFE_DELETE( m_pCount );
	}
	else
		m_pCount->DecCount();

	m_pCount = S.m_pCount;
	m_pCount->IncCount();

	return *this;
}

//---------------------------------------------------------------------------

ZString& ZString::operator + ( const ZString& S )
{
	int iNewLength = 0;
	char* pNewText = NULL;
	ZString* pStr = NULL;

	// 새로운 ZString 생성
	iNewLength = m_pCount->GetLength() + S.m_pCount->GetLength();
	pNewText = new char[ iNewLength + 1 ];
	strcpy( pNewText, m_pCount->GetString() );
	strcat( pNewText, S.c_str() );

	pStr = new ZString( pNewText );	
	pStr->m_pCount->DecCount();
	return *pStr;
}

//---------------------------------------------------------------------------
// ZString 붙이기

ZString& ZString::operator += ( const ZString& S )
{
	int iNewLength = 0;
	char* pNewText = NULL;
	ZString* pStr = NULL;

	// 새로운 ZString 생성
	iNewLength = m_pCount->GetLength() + S.m_pCount->GetLength();
	pNewText = new char[ iNewLength + 1 ];
	strcpy( pNewText, m_pCount->GetString() );
	strcat( pNewText, S.c_str() );

	pStr = new ZString( pNewText );	

	*this = *pStr;

	SAFE_DELETE( pStr );

	return *this;
}

//---------------------------------------------------------------------------

BOOL ZString::operator == ( const ZString& S )
{
	ZString zCompare = S;

	if( Compare( zCompare ) == 0 )
		return TRUE;
	return FALSE;
}

//---------------------------------------------------------------------------

BOOL ZString::operator != ( const ZString& S )
{
	ZString zCompare = S;

	if( Compare( zCompare ) != 0 )
		return TRUE;
	return FALSE;
}

//---------------------------------------------------------------------------

char* ZString::c_str() const
{ 
	return (m_pCount)?m_pCount->GetString():""; 
}

//---------------------------------------------------------------------------

int ZString::Length()
{
	return m_pCount->GetLength();
}

//---------------------------------------------------------------------------

BOOL ZString::IsEmpty()
{
	if( m_pCount->GetLength() <= 0 )
		return TRUE;

	return FALSE;
}

//---------------------------------------------------------------------------

int ZString::ToInt()
{
	// FAIL : Return 0
	return atoi( m_pCount->GetString() );
}

//---------------------------------------------------------------------------

double ZString::ToDouble()
{
	// FAIL : Return 0 , etc.
	return atof( m_pCount->GetString() );
}

//---------------------------------------------------------------------------

float ZString::ToFloat()
{
	// FAIL : Return 0 , etc.
	return static_cast<float>( atof( m_pCount->GetString() ) );
}

//---------------------------------------------------------------------------

BOOL ZString::UpperCase()
{
	int iLength = m_pCount->GetLength();
	char* pText = m_pCount->GetString();

	for( int i = 0; i < iLength; i++ )
	{
		pText[i] = toupper( pText[i] );
	}

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL ZString::LowerCase()
{
	int iLength = m_pCount->GetLength();
	char* pText = m_pCount->GetString();

	for( int i = 0; i < iLength; i++ )
	{
		pText[i] = tolower( pText[i] );
	}

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL ZString::Trim( char* pToken )
{
	TrimLeft( pToken );
	TrimRight( pToken );
	return TRUE;
}

//---------------------------------------------------------------------------

BOOL ZString::TrimLeft( char* pToken )
{
	int iLength = m_pCount->GetLength();
	char* pText = m_pCount->GetString();
	char* pNewText = NULL;
	ZString* pStr = NULL;

	if( pToken == NULL )
	{
		// 공백 제거
		for( int i = 0; i < iLength; i++ )
		{
			if( pText[i] != ' ' )
			{
				pNewText = &pText[i];

				pStr = new ZString( pNewText );
				*this = *pStr;
				SAFE_DELETE( pStr );

				break;
			}
		}
	}
	else
	{
		// 좌측 pToken 제거
		int iPos = 0;
		ZString zToken( pToken ); 

		do
		{
			iPos = Pos( zToken );
			if( iPos == 0 )
				Delete( 0, zToken.Length() );
		} 
		while ( iPos == 0 );
	}

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL ZString::TrimRight( char* pToken )
{
	int iLength = m_pCount->GetLength();
	char* pText = m_pCount->GetString();
	char* pNewText = NULL;
	ZString* pStr = NULL;

	if( pToken == NULL )
	{
		for( int i = (iLength-1); i >= 0; i-- )
		{
			if( pText[i] != ' ' )
			{
				pNewText = new char[i+2];
				memset( pNewText, 0, i+2 );
				strncpy( pNewText, pText, i+1 );

				pStr = new ZString( pNewText );
				SAFE_DELETE_ARRAY( pNewText );
				*this = *pStr;
				SAFE_DELETE( pStr );

				break;
			}
		}
	}
	else
	{
		// 우측 pToken 제거
		ZString zToken = pToken;
		int iPos = 0, iCompare = 0, iTokenLen = zToken.Length();

		do
		{
			ZString zSub = SubString( Length() - iTokenLen, iTokenLen );

			if( ( iCompare = zSub.Compare( zToken ) ) == 0 )
				Delete( Length() - iTokenLen, iTokenLen );
		} 
		while ( iCompare == 0 );
	}

	return TRUE;
}

//---------------------------------------------------------------------------

int ZString::Pos( const char* szStr )
{
	ZString Temp = szStr;

	return Pos( Temp );
}

//---------------------------------------------------------------------------

int ZString::Pos( ZString& subStr )
{
	int iLength = m_pCount->GetLength();
	int iSubLen = subStr.Length();
	char* pText = m_pCount->GetString();
	char* pSubText = subStr.c_str();

	for( int i = 0; i < iLength; i++ )
	{
		// 첫글자 일치
		if( pText[i] == pSubText[0] )
		{
			for( int j = 0; j < iSubLen; j++ )
			{
				// subStr과 전부 비교
				if( pText[i+j] == pSubText[j] )
				{
					// 전부 일치? : 한 바이트도 됨!
					if( j == iSubLen-1 )
						return i;
				}
				else
					break; // 불일치
			}
		}
	}

	return -1;
}

//---------------------------------------------------------------------------

ZString& ZString::Insert( ZString& str, int Index )
{
	ZString NewString;

	// 인덱스 범위가 정상인가?
	if( Index < 0 && Index >= Length() )
		return *this;

	// Insert 스트링이 있는가?
	if( str.Length() < 1 )
		return *this;

	NewString = SubString( 0, Index );
	NewString += str;
	NewString += SubString( Index, Length() - Index );
	*this = NewString;

	return *this;
}

//---------------------------------------------------------------------------

ZString& ZString::Delete( int Index, int Count )
{
	// 인덱스 범위가 정상인가?
	if( Index < 0 && Index >= Length() )
		return *this;

	// 카운트 값이 정상인가?
	if( Count < 1 )
		return *this;

	// 인덱스 + 카운트가 문자열 길이를 초과하는가?
	if( Length() < (Index + Count) )
		return *this;

	ZString NewString;

	NewString = SubString( 0, Index );
	NewString += SubString( Index + Count, (Length() - (Index + Count)) );
	*this = NewString;

	return *this;
}

//---------------------------------------------------------------------------

ZString ZString::SubString( int Index, int Count )
{
	char* pNewText;
	ZString NewString;

	// 스트링 길이를 초과한 인덱스인가?
	if( Length() < (Index+Count) )
		return ZString("");

	// 가져오는 스트링 수가 있는가?
	if( Count < 1 )
		return ZString("");

	pNewText = new char[Count+1];
	memset( pNewText, 0, Count+1 );

	memcpy( pNewText, &c_str()[Index], Count );

	NewString = pNewText;

	SAFE_DELETE_ARRAY( pNewText );

	return NewString;
}

//---------------------------------------------------------------------------

ZString ZString::Token( ZString zDelimiter )
{
	int iDelPos = 0;
	ZString zToken;

	iDelPos = Pos( zDelimiter );
	if( iDelPos == -1 )
	{
		zToken = *this;
		*this = "";
	}
	else
	{
		zToken = SubString( 0, iDelPos );
		Delete( 0, iDelPos + zDelimiter.Length() );
	}

	return zToken;
}

//---------------------------------------------------------------------------

ZString ZString::LastToken( ZString zDelimiter )
{
	ZString zLastToken;
	ZString zToken;

	zToken = *this;
	do
	{
		zLastToken = zToken;
		zToken.Token( zDelimiter );
	} 
	while( zToken.IsEmpty() == FALSE );

	return zLastToken;
}

//---------------------------------------------------------------------------
// Compares the AnsiString to another specified string (case-sensitively)

int ZString::Compare( ZString& S2 )
{
	// 세부 바이트 비교는 추후 시간 있을때... 다시 시도 -_-; 일단...
	int iReturn = 0;

	iReturn = _mbscmp( (const unsigned char*)c_str(), (const unsigned char*)S2.c_str() );

	return iReturn;
}

//---------------------------------------------------------------------------

int ZString::CompareIC( ZString& S2 )
{
	int iReturn = 0;

	iReturn = _mbsicmp( (const unsigned char*)c_str(), (const unsigned char*)S2.c_str() );

	return iReturn;
}

//---------------------------------------------------------------------------

BOOL ZString::Format( const char* fmt, ... )
{
	va_list args;
	int len = 0;
	char* pBuf = NULL;

	va_start( args, fmt );
	len = _vscprintf( fmt, args )	// _vscprintf doesn't count
		+ 1; // terminating '\0'
	pBuf = new char[len];
	memset( pBuf, 0, len );
	vsprintf( pBuf, fmt, args );

	ZString lzTemp( pBuf );

	(*this) = lzTemp;	

	SAFE_DELETE_ARRAY( pBuf );

	return 0;
} 

//---------------------------------------------------------------------------

