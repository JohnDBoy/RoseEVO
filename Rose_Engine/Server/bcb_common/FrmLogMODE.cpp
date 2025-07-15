//---------------------------------------------------------------------------

#include "PreHEADER.h"

#include "FrmLogMODE.h"
#include "BCB_Tools.h"
#include "FrmMAIN.h"


void WriteFILE (char *szString);
void WriteLOG  (char *szString);


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormLogMODE *FormLogMODE;

#define SZ_RHAKMU_REGKEY    "SOFTWARE\\TriggerSOFT\\SERVER\\Project2003_LOGIN"
//---------------------------------------------------------------------------
__fastcall TFormLogMODE::TFormLogMODE(TComponent* Owner)
    : TForm(Owner)
{
}

//---------------------------------------------------------------------------
void __fastcall TFormLogMODE::FormCreate(TObject *Sender)
{
//    String stCurDir = GetCurrentDir ();
    if ( !BCBUtil::IsRegKeyExist( SZ_RHAKMU_REGKEY ) ) {
        BCBUtil::SetRegistryINT( SZ_RHAKMU_REGKEY, "ScreenLOG", RadioGroupSCR->ItemIndex);
        BCBUtil::SetRegistryINT( SZ_RHAKMU_REGKEY, "FileLOG",   RadioGroupFILE->ItemIndex);
    } else {
        RadioGroupSCR->ItemIndex  = BCBUtil::GetRegistryINT( SZ_RHAKMU_REGKEY, "ScreenLOG", false);
        RadioGroupFILE->ItemIndex = BCBUtil::GetRegistryINT( SZ_RHAKMU_REGKEY, "FileLOG", false);
    }

    this->Set_LogMode ();

//    FormMain->MI_Server_ActiveClick( NULL );
}

//---------------------------------------------------------------------------
void __fastcall TFormLogMODE::Set_LogMode ()
{
    WORD wLogType;

    switch( RadioGroupSCR->ItemIndex ) {
        case 0 :    // none
            wLogType  = LOG_NONE;
            break;
        case 1 :    // normal
            wLogType  = LOG_NORMAL;
            break;
        case 2 :    // verbose
            wLogType  = LOG_VERBOSE;
            break;
        case 3 :    // debug
            wLogType  = LOG_DEBUG;
            break;
    }
    FormMAIN->SetLogMODE (wLogType);

    switch( RadioGroupFILE->ItemIndex ) {
        case 0 :    // none
            wLogType  = LOG_NONE;
            break;
        case 1 :    // normal
            wLogType  = LOG_NORMAL;
            break;
        case 2 :    // verbose
            wLogType  = LOG_VERBOSE;
            break;
        case 3 :    // debug
            wLogType  = LOG_DEBUG;
            break;
    }
    FormMAIN->SetLogMODE (wLogType, LOG_FILE);
}

//---------------------------------------------------------------------------
void __fastcall TFormLogMODE::Set_RadioButton ()
{
    WORD wLogType;

    wLogType = FormMAIN->GetLogMODE( LOG_SCR );
    if ( wLogType & LOG_DEBUG )
        RadioGroupSCR->ItemIndex = 3;
    else if ( wLogType & LOG_VERBOSE )
        RadioGroupSCR->ItemIndex = 2;
    else if ( wLogType & LOG_NORMAL )
        RadioGroupSCR->ItemIndex = 1;
    else
        RadioGroupSCR->ItemIndex = 0;

    wLogType = FormMAIN->GetLogMODE( LOG_FILE );
    if ( wLogType & LOG_DEBUG )
        RadioGroupFILE->ItemIndex = 3;
    else if ( wLogType & LOG_VERBOSE )
        RadioGroupFILE->ItemIndex = 2;
    else if ( wLogType & LOG_NORMAL )
        RadioGroupFILE->ItemIndex = 1;
    else
        RadioGroupFILE->ItemIndex = 0;
}

//---------------------------------------------------------------------------
void __fastcall TFormLogMODE::ButtonOKClick(TObject *Sender)
{
    BCBUtil::SetRegistryINT( SZ_RHAKMU_REGKEY, "ScreenLOG", RadioGroupSCR->ItemIndex);
    BCBUtil::SetRegistryINT( SZ_RHAKMU_REGKEY, "FileLOG",   RadioGroupFILE->ItemIndex);

    this->Set_LogMode ();

    Close ();
}
//---------------------------------------------------------------------------
void __fastcall TFormLogMODE::ButtonCANCELClick(TObject *Sender)
{
    this->Set_RadioButton ();

    Close ();
}

//---------------------------------------------------------------------------

