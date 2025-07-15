/*---------------------------------------------------------------------------
Make:		2006.04.25
Name:		ZFile.h
Author:     ����ö.����

Desc:		���� ��Ʈ�� �׼���
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
	int GetFPPos();		// ���� ������ ��ġ
	//int GetSize();		// ���� ũ�� (����:byte)

	// ���� ���Ͻ�Ʈ�� ���� ( NULL = ������ �������� �ʾҰų� ���� )
	FILE* GetFStream();

	// ���� ���ΰ�?
	BOOL ZEOF();
};

//---------------------------------------------------------------------------

#endif

