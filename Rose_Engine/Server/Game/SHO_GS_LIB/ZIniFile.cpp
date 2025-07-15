
//---------------------------------------------------------------------------

#include "ZSTD.h"

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// ZIniFile
//---------------------------------------------------------------------------

ZIniFile::ZIniFile(VOID)
{

}

//---------------------------------------------------------------------------

ZIniFile::~ZIniFile(VOID)
{
}

//---------------------------------------------------------------------------

BOOL ZIniFile::Open(ZString zFileName)
{
	if (zFileName.IsEmpty())
		return FALSE;

	m_zFileName = zFileName;

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL ZIniFile::Close(VOID)
{
	return TRUE;
}

BOOL ZIniFile::GetValue(ZString zKeyName, ZString zValueName, DWORD& dwValue /*OUT*/)
{
	if( zKeyName.IsEmpty() || zValueName.IsEmpty() || !(&dwValue) )
		return FALSE;

	dwValue = GetPrivateProfileInt(zKeyName.c_str(), zValueName.c_str(), 0, m_zFileName.c_str());

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL ZIniFile::GetValue(ZString zKeyName, ZString zValueName, FLOAT& fValue /*OUT*/)
{
	char Value[16] = {0,};

	if( zKeyName.IsEmpty() || zValueName.IsEmpty() || !(&fValue) )
		return FALSE;

	GetPrivateProfileString(zKeyName.c_str(), zValueName.c_str(), "", Value, 16, m_zFileName.c_str());

	fValue = (FLOAT) atof( Value );
	//*value = (FLOAT) _tstof(Value);

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL ZIniFile::GetValue(ZString zKeyName, ZString zValueName, ZString& zValue, DWORD dwBufferLength)
{
	char* pBuffer = NULL;

	if( zKeyName.IsEmpty() || zValueName.IsEmpty() || !(&dwBufferLength) )
        return FALSE;

	if( (pBuffer = new char[dwBufferLength]) == NULL )
		return FALSE;
	memset( pBuffer, 0, size_t(dwBufferLength) );

	dwBufferLength = GetPrivateProfileString(zKeyName.c_str(), zValueName.c_str(), "", pBuffer, dwBufferLength, m_zFileName.c_str());

	zValue.Format( "%s", pBuffer );
	SAFE_DELETE_ARRAY( pBuffer );

	return TRUE;
}

//---------------------------------------------------------------------------

BOOL ZIniFile::SetValue(ZString zKeyName, ZString zValueName, ZString zValue)
{
	if( zKeyName.IsEmpty() || zValueName.IsEmpty() || zValue.IsEmpty() )
		return FALSE;

	WritePrivateProfileString(zKeyName.c_str(), zValueName.c_str(), zValue.c_str(), m_zFileName.c_str());

	return TRUE;
}

//---------------------------------------------------------------------------
