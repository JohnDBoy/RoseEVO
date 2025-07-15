// 2006.06.09/김대성/추가 - CLS_SqlTHREAD_Log 클래스 추가, 로그인서버에서 로그인 성공 로그남기기

#ifndef	__CLS_SQLTHREAD_LOG_H
#define	__CLS_SQLTHREAD_LOG_H
//-------------------------------------------------------------------------------------------------
#include "CSqlTHREAD.h"
//-------------------------------------------------------------------------------------------------

class CLS_SqlTHREAD_Log : public CSqlTHREAD
{
private:

	// inherit virtual function from CSqlTHREAD...
	bool Run_SqlPACKET( tagQueryDATA *pSqlPACKET );

	void Execute ();

	CLS_SqlTHREAD_Log ();
	~CLS_SqlTHREAD_Log ();
    
public :
	void Set_EVENT ()		{	m_pEVENT->SetEvent ();		}

	DECLARE_INSTANCE( CLS_SqlTHREAD_Log )
} ;
extern CLS_SqlTHREAD_Log *g_pThreadSQL_Log;

//-------------------------------------------------------------------------------------------------
#endif