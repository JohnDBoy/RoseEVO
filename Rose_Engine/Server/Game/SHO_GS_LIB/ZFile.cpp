
//---------------------------------------------------------------------------

//#include <stdio.h>	// ZFile.h�� ��������
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

	return m_pFStream;	// ERROR �϶� NULL ����
}

//---------------------------------------------------------------------------

FILE* ZFile::OpenBinN( char* FileName )
{
	m_pFStream = fopen( FileName, "w+b" );

	return m_pFStream;	// ERROR �϶� NULL ����
}

//---------------------------------------------------------------------------

FILE* ZFile::OpenBinR( char* FileName )
{
	m_pFStream = fopen( FileName, "rb" );

	return m_pFStream;	// ERROR �϶� NULL ����
}

//---------------------------------------------------------------------------

FILE* ZFile::OpenBinRW( char* FileName )
{
	m_pFStream = fopen( FileName, "r+b" );

	return m_pFStream;	// ERROR �϶� NULL ����
}

//---------------------------------------------------------------------------

FILE* ZFile::OpenBinW( char* FileName )
{
	m_pFStream = fopen( FileName, "wb" );

	return m_pFStream;	// ERROR �϶� NULL ����
}

//---------------------------------------------------------------------------
// Create Text File

FILE* ZFile::OpenTextA( char* FileName )
{
	m_pFStream = fopen( FileName, "a+t" );

	return m_pFStream;	// ERROR �϶� NULL ����
}

//---------------------------------------------------------------------------

FILE* ZFile::OpenTextN( char* FileName )
{
	m_pFStream = fopen( FileName, "w+t" );

	return m_pFStream;	// ERROR �϶� NULL ����
}

//---------------------------------------------------------------------------

FILE* ZFile::OpenTextR( char* FileName )
{
	m_pFStream = fopen( FileName, "rt" );

	return m_pFStream;	// ERROR �϶� NULL ����
}

//---------------------------------------------------------------------------

FILE* ZFile::OpenTextRW( char* FileName )
{
	m_pFStream = fopen( FileName, "r+t" );

	return m_pFStream;	// ERROR �϶� NULL ����
}

//---------------------------------------------------------------------------

FILE* ZFile::OpenTextW( char* FileName )
{
	m_pFStream = fopen( FileName, "wt" );

	return m_pFStream;	// ERROR �϶� NULL ����
}

//---------------------------------------------------------------------------

BOOL ZFile::Flush()
{
	// ��½� : ��Ʈ�� ���ۿ� �ִ� ������ ���Ϸ� ����.
	// �Է½� : �Է¹޴� ��Ʈ�� ���۸� ������ ����.
	// ���� �߻��� EOF ���� (-1)
	if( fflush( m_pFStream ) == -1 )
		return FALSE;
	return TRUE;
}

//---------------------------------------------------------------------------

BOOL ZFile::Close()
{
	// ������ ������� 0�� ����
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
	// ERROR �϶� -1 ����
	// �μ�:����,�д´���(byte),�����ݺ�Ƚ��,���Ͻ�Ʈ��������
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

	// ERROR �϶� -1 ����
	// iByte ���� ���� ��ϵ� ����Ʈ�� ���� ��� ��ũ �뷮�� �����Ѱ��̴�.
	return -1;
}

//---------------------------------------------------------------------------

int ZFile::ReadStruct( void* pStruct, int iByte, int iCount )
{
	// ERROR �϶� -1 ����
	// �μ�:����,�д´���(byte),�����ݺ�Ƚ��,���Ͻ�Ʈ��������
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

	// ERROR �϶� -1 ����
	// iByte ���� ���� ��ϵ� ����Ʈ�� ���� ��� ��ũ �뷮�� �����Ѱ��̴�.
	return -1;	
}

//---------------------------------------------------------------------------

//int ZFile::ReadText( ZString& BufferOut )
//{
//	char* pBuf = NULL;	// ����
//	long FileLen = 0;	// ���� ����
//	int iReadByte = 0;	// ���� Byte
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
	int iMAX_BUF = 65534;	// 65535�� ERROR�� ���(-1)
	ZString zTemp;			// \n�� ã�� �ӽ� ��Ʈ�� ����


	// ����� BufferOut �ʱ�ȭ
	BufferOut = "";

	if( (pBuf = new char[iMAX_BUF]) != NULL )
	{
		memset( pBuf, 0, iMAX_BUF );
		// ���� ����Ÿ�� �ִ°�? (���� �������� NULL ���ԵǾ�����)
		if( fgets( pBuf, iMAX_BUF, m_pFStream ) == NULL ) // ���� ���̰ų� �����϶� NULL
		{
			SAFE_DELETE_ARRAY( pBuf );
			return BufferOut.Length();
		}
		BufferOut = pBuf;
		iReadTotal = iReadByte = BufferOut.Length();

		// ����ũ�� ���� \n�� �ִ°�?
		if( iReadByte < iMAX_BUF-1 )
		{
			SAFE_DELETE_ARRAY( pBuf );
			return iReadTotal;
		}

		// ��� ����
		while( iReadByte < iMAX_BUF )
		{
			memset( pBuf, 0, iMAX_BUF );
			// ���� ����Ÿ�� �ִ°�? (���� �������� NULL ���ԵǾ�����)
			if( fgets( pBuf, iMAX_BUF, m_pFStream ) == NULL ) // ���� ���̰ų� �����϶� NULl
			{
				SAFE_DELETE_ARRAY( pBuf );
				return BufferOut.Length();
			}
			zTemp = pBuf;
			iReadTotal += (iReadByte = zTemp.Length());

			// ����ũ�� ���� \n�� �ִ°�?
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
	//// ERROR �϶� -1 ����
	return static_cast<int>(fwrite( BufferIn.c_str(), 1, BufferIn.Length(), m_pFStream ));
}

//---------------------------------------------------------------------------

int ZFile::WriteLine( ZString& BufferIn )
{
	// ERROR �϶� -1 ����
	return fputs( BufferIn.c_str(), m_pFStream );
}

//---------------------------------------------------------------------------
// File pointer operation

BOOL ZFile::SetFPFirst( int iOffset )
{ // Pos >= 0
	// ERROR �϶� nonezero ����, ���� : 0
	if( fseek( m_pFStream, static_cast<long>(iOffset), SEEK_SET ) != 0 )
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL ZFile::SetFPCur( int iOffset )
{ // Pos > 0, = 0, < 0
	// ERROR �϶� nonezero ����, ���� : 0
	if( fseek( m_pFStream, static_cast<long>(iOffset), SEEK_CUR ) != 0 )
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL ZFile::SetFPLast( int iOffset )
{ // Pos <= 0
	// ERROR �϶� nonezero ����, ���� : 0
	if( fseek( m_pFStream, static_cast<long>(iOffset), SEEK_END ) != 0 )
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------

int ZFile::GetFPPos()
{
	// ERROR �϶� -1 ����
	return ftell( m_pFStream );
}

//---------------------------------------------------------------------------

//int ZFile::GetSize()
//{
//	// ERROR �϶� -1 ����
//	return static_cast<int>( filelength( fileno( m_pFStream ) ) );
//}

//---------------------------------------------------------------------------

FILE* ZFile::GetFStream()
{
	// ���� �ʵǾ����� NULL
	return m_pFStream;
}

//---------------------------------------------------------------------------

BOOL ZFile::ZEOF()
{
	// ���ϳ��̸� 1�̻�, �ƴϸ� 0, ���� -1(�ڵ� ��ȿ)
	int iEOF = 0;

	iEOF = feof( m_pFStream );
	if( iEOF >= 1 )
		return TRUE;	// ���� ��
	else if( iEOF == 0 )
		return FALSE;	// ���� �� �ƴ�
	if( iEOF == -1 )
		return TRUE;	// ����

	return TRUE;
}

//---------------------------------------------------------------------------
