#ifndef BCB_SQLH
#define BCB_SQLH
//---------------------------------------------------------------------------

class BCB_SQL
{
private:
public :
    bool OpenSQL (TQuery *pQuery, String &stQuery);
    bool ExecSQL (TQuery *pQuery, String &stQuery);
} ;

//---------------------------------------------------------------------------
#endif
