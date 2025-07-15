/*---------------------------------------------------------------------------
Make:		2006.04.25
Name:		ZFile.h
Author:     남병철.레조

Desc:		파일 스트림 액세스
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __ZFILE_H__
#define __ZFILE_H__
//---------------------------------------------------------------------------
#include <stdio.h>
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class ZString;
class ZFile
{
private:
	FILE* m_pFStream;

public:
	ZFile();
	~ZFile();

	// Binary file open
	FILE* OpenBinA( char* FileName );
	FILE* OpenBinN( char* FileName );
	FILE* OpenBinR( char* FileName );
	FILE* OpenBinRW( char* FileName );
	FILE* OpenBinW( char* FileName );

	// Text file open
	FILE* OpenTextA( char* FileName );
	FILE* OpenTextN( char* FileName );
	FILE* OpenTextR( char* FileName );
	FILE* OpenTextRW( char* FileName );
	FILE* OpenTextW( char* FileName );

	// Flush stream data
	BOOL Flush();
	// File close
	BOOL Close();

	// TEXT / BINARY
	int Read( void* pBuffer, int iByte );
	int Write( void* pBuffer, int iByte );

	// BINARY ONLY
	int ReadStruct( void* pStruct, int iByte, int iCount = 1 );
	int WriteStruct( void* pStruct, int iByte, int iCount = 1 );

	// TEXT ONLY
	//int ReadText( ZString& BufferOut );
	int ReadLine( ZString& BufferOut );
	int WriteText( ZString& BufferIn );
	int WriteLine( ZString& BufferIn );

	// File pointer operation
	BOOL SetFPFirst( int iOffset = 0 );  // Offset >= 0
	BOOL SetFPCur( int iOffset = 0 );    // Offset > 0, = 0, < 0
	BOOL SetFPLast( int iOffset = 0 );   // Offset <= 0
	int GetFPPos();		// 파일 포인터 위치
	//int GetSize();		// 파일 크기 (단위:byte)

	// 현재 파일스트림 리턴 ( NULL = 파일이 생성되지 않았거나 오류 )
	FILE* GetFStream();

	// 파일 끝인가?
	BOOL ZEOF();
};

//---------------------------------------------------------------------------

#endif

