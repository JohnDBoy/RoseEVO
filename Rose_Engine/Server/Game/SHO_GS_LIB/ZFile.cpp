
//---------------------------------------------------------------------------

//#include <stdio.h>	// ZFile.h에 선언했음
//#include <io.h>			// filelength ...
#include "ZSTD.h"

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// ZFile
//---------------------------------------------------------------------------

ZFile::ZFile()
{
	m_pFStream = NULL;
}

//---------------------------------------------------------------------------

ZFile::~ZFile()
{
	if( m_pFStream != NULL )
		fclose( m_pFStream );
}

//---------------------------------------------------------------------------
// Create binary file

FILE* ZFile::OpenBinA( char* FileName )
{
	m_pFStream = fopen( FileName, "a+b" );

	return m_pFStream;	// ERROR 일땐 NULL 리턴
}

//---------------------------------------------------------------------------

FILE* ZFile::OpenBinN( char* FileName )
{
	m_pFStream = fopen( FileName, "w+b" );

	return m_pFStream;	// ERROR 일땐 NULL 리턴
}

//---------------------------------------------------------------------------

FILE* ZFile::OpenBinR( char* FileName )
{
	m_pFStream = fopen( FileName, "rb" );

	return m_pFStream;	// ERROR 일땐 NULL 리턴
}

//---------------------------------------------------------------------------

FILE* ZFile::OpenBinRW( char* FileName )
{
	m_pFStream = fopen( FileName, "r+b" );

	return m_pFStream;	// ERROR 일땐 NULL 리턴
}

//---------------------------------------------------------------------------

FILE* ZFile::OpenBinW( char* FileName )
{
	m_pFStream = fopen( FileName, "wb" );

	return m_pFStream;	// ERROR 일땐 NULL 리턴
}

//---------------------------------------------------------------------------
// Create Text File

FILE* ZFile::OpenTextA( char* FileName )
{
	m_pFStream = fopen( FileName, "a+t" );

	return m_pFStream;	// ERROR 일땐 NULL 리턴
}

//---------------------------------------------------------------------------

FILE* ZFile::OpenTextN( char* FileName )
{
	m_pFStream = fopen( FileName, "w+t" );

	return m_pFStream;	// ERROR 일땐 NULL 리턴
}

//---------------------------------------------------------------------------

FILE* ZFile::OpenTextR( char* FileName )
{
	m_pFStream = fopen( FileName, "rt" );

	return m_pFStream;	// ERROR 일땐 NULL 리턴
}

//---------------------------------------------------------------------------

FILE* ZFile::OpenTextRW( char* FileName )
{
	m_pFStream = fopen( FileName, "r+t" );

	return m_pFStream;	// ERROR 일땐 NULL 리턴
}

//---------------------------------------------------------------------------

FILE* ZFile::OpenTextW( char* FileName )
{
	m_pFStream = fopen( FileName, "wt" );

	return m_pFStream;	// ERROR 일땐 NULL 리턴
}

//---------------------------------------------------------------------------

BOOL ZFile::Flush()
{
	// 출력시 : 스트림 버퍼에 있는 내용을 파일로 쓴다.
	// 입력시 : 입력받는 스트림 버퍼를 깨끗이 비운다.
	// 에러 발생시 EOF 리턴 (-1)
	if( fflush( m_pFStream ) == -1 )
		return FALSE;
	return TRUE;
}

//---------------------------------------------------------------------------

BOOL ZFile::Close()
{
	// 오류가 없을경우 0을 리턴
	if( m_pFStream == NULL )
		return FALSE;
	if( fclose( m_pFStream ) != 0 )
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------
// Read and Write

int ZFile::Read( void* pBuffer, int iByte )
{
	// ERROR 일땐 -1 리턴
	// 인수:버퍼,읽는단위(byte),단위반복횟수,파일스트림포인터
	return static_cast<int>(fread( pBuffer, 1, iByte, m_pFStream ));
}

//---------------------------------------------------------------------------

int ZFile::Write( void* pBuffer, int iByte )
{
	int iWriteByte = 0;


	if( (iWriteByte = static_cast<int>(fwrite( pBuffer, 1, iByte, m_pFStream ))) == iByte )
	{
		return iWriteByte;
	}

	// ERROR 일땐 -1 리턴
	// iByte 보다 실제 기록된 바이트가 작을 경우 디스크 용량이 부족한것이다.
	return -1;
}

//---------------------------------------------------------------------------

int ZFile::ReadStruct( void* pStruct, int iByte, int iCount )
{
	// ERROR 일땐 -1 리턴
	// 인수:버퍼,읽는단위(byte),단위반복횟수,파일스트림포인터
	return static_cast<int>(fread( pStruct, iByte, iCount, m_pFStream ));
}

//---------------------------------------------------------------------------

int ZFile::WriteStruct( void* pStruct, int iByte, int iCount )
{
	int iWriteByte = 0;


	if( (iWriteByte = static_cast<int>(fwrite( pStruct, iByte, iCount, m_pFStream ))) 
		== (iByte*iCount) )
	{
		return iWriteByte;
	}

	// ERROR 일땐 -1 리턴
	// iByte 보다 실제 기록된 바이트가 작을 경우 디스크 용량이 부족한것이다.
	return -1;	
}

//---------------------------------------------------------------------------

//int ZFile::ReadText( ZString& BufferOut )
//{
//	char* pBuf = NULL;	// 버퍼
//	long FileLen = 0;	// 파일 길이
//	int iReadByte = 0;	// 읽은 Byte
//
//
//	FileLen = filelength( fileno( m_pFStream ) );
//	if( (pBuf = new char[FileLen+1]) != NULL )
//	{
//		memset( pBuf, 0, FileLen+1 );
//		iReadByte = static_cast<int>((fread( pBuf, 1, FileLen, m_pFStream )));
//		BufferOut = pBuf;
//		SAFE_DELETE_ARRAY( pBuf );
//		return iReadByte;
//	}
//	
//	return -1;
//}

//---------------------------------------------------------------------------

int ZFile::ReadLine( ZString& BufferOut )
{
	char* pBuf = NULL;
	int iReadByte = 0, iReadTotal = 0;
	int iMAX_BUF = 65534;	// 65535는 ERROR로 사용(-1)
	ZString zTemp;			// \n을 찾는 임시 스트링 보관


	// 출력할 BufferOut 초기화
	BufferOut = "";

	if( (pBuf = new char[iMAX_BUF]) != NULL )
	{
		memset( pBuf, 0, iMAX_BUF );
		// 읽은 데이타가 있는가? (읽은 마지막에 NULL 삽입되어있음)
		if( fgets( pBuf, iMAX_BUF, m_pFStream ) == NULL ) // 파일 끝이거나 오류일때 NULL
		{
			SAFE_DELETE_ARRAY( pBuf );
			return BufferOut.Length();
		}
		BufferOut = pBuf;
		iReadTotal = iReadByte = BufferOut.Length();

		// 버퍼크기 내에 \n이 있는가?
		if( iReadByte < iMAX_BUF-1 )
		{
			SAFE_DELETE_ARRAY( pBuf );
			return iReadTotal;
		}

		// 계속 읽음
		while( iReadByte < iMAX_BUF )
		{
			memset( pBuf, 0, iMAX_BUF );
			// 읽은 데이타가 있는가? (읽은 마지막에 NULL 삽입되어있음)
			if( fgets( pBuf, iMAX_BUF, m_pFStream ) == NULL ) // 파일 끝이거나 오류일때 NULl
			{
				SAFE_DELETE_ARRAY( pBuf );
				return BufferOut.Length();
			}
			zTemp = pBuf;
			iReadTotal += (iReadByte = zTemp.Length());

			// 버퍼크기 내에 \n이 있는가?
			if( iReadByte < iMAX_BUF-1 )
			{
				BufferOut += zTemp;
				SAFE_DELETE_ARRAY( pBuf );
				return iReadTotal;
			}
			BufferOut += zTemp;
		}

		SAFE_DELETE_ARRAY( pBuf );
	}
	else
	{
		iReadTotal = -1;
	}

	return iReadTotal;
}

//---------------------------------------------------------------------------

int ZFile::WriteText( ZString& BufferIn )
{
	//// ERROR 일땐 -1 리턴
	return static_cast<int>(fwrite( BufferIn.c_str(), 1, BufferIn.Length(), m_pFStream ));
}

//---------------------------------------------------------------------------

int ZFile::WriteLine( ZString& BufferIn )
{
	// ERROR 일땐 -1 리턴
	return fputs( BufferIn.c_str(), m_pFStream );
}

//---------------------------------------------------------------------------
// File pointer operation

BOOL ZFile::SetFPFirst( int iOffset )
{ // Pos >= 0
	// ERROR 일땐 nonezero 리턴, 성공 : 0
	if( fseek( m_pFStream, static_cast<long>(iOffset), SEEK_SET ) != 0 )
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL ZFile::SetFPCur( int iOffset )
{ // Pos > 0, = 0, < 0
	// ERROR 일땐 nonezero 리턴, 성공 : 0
	if( fseek( m_pFStream, static_cast<long>(iOffset), SEEK_CUR ) != 0 )
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL ZFile::SetFPLast( int iOffset )
{ // Pos <= 0
	// ERROR 일땐 nonezero 리턴, 성공 : 0
	if( fseek( m_pFStream, static_cast<long>(iOffset), SEEK_END ) != 0 )
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------

int ZFile::GetFPPos()
{
	// ERROR 일땐 -1 리턴
	return ftell( m_pFStream );
}

//---------------------------------------------------------------------------

//int ZFile::GetSize()
//{
//	// ERROR 일땐 -1 리턴
//	return static_cast<int>( filelength( fileno( m_pFStream ) ) );
//}

//---------------------------------------------------------------------------

FILE* ZFile::GetFStream()
{
	// 생성 않되었으면 NULL
	return m_pFStream;
}

//---------------------------------------------------------------------------

BOOL ZFile::ZEOF()
{
	// 파일끝이면 1이상, 아니면 0, 오류 -1(핸들 무효)
	int iEOF = 0;

	iEOF = feof( m_pFStream );
	if( iEOF >= 1 )
		return TRUE;	// 파일 끝
	else if( iEOF == 0 )
		return FALSE;	// 파일 끝 아님
	if( iEOF == -1 )
		return TRUE;	// 오류

	return TRUE;
}

//---------------------------------------------------------------------------
