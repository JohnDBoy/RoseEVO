/*---------------------------------------------------------------------------
Make:		2007.02.20
Name:		ZIniFile.h
Author:     ����ö.����

Desc:		ZIniFile Ŭ����
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __ZINIFILE_H__
#define __ZINIFILE_H__

//---------------------------------------------------------------------------

class ZIniFile
{
public:
	ZIniFile(VOID);
	virtual ~ZIniFile(VOID);

private:
	ZString	m_zFileName;

public:
	BOOL	Open(ZString zFileName);
	BOOL	Close(VOID);

	BOOL	SetValue(ZString zKeyName, ZString zValueName, ZString zValue);

	BOOL	GetValue(ZString zKeyName, ZString zValueName, ZString& zValue /*OUT*/, 
					 DWORD dwBufferLength = 1024);
	BOOL	GetValue(ZString zKeyName, ZString zValueName, DWORD& dwValue /*OUT*/);
	BOOL	GetValue(ZString zKeyName, ZString zValueName, FLOAT& fValue /*OUT*/);
};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------