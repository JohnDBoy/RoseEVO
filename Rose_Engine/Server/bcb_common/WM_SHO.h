#ifndef __WM_SHO_H
#define __WM_SHO_H
//---------------------------------------------------------------------------

#define WM_SHO                      ( WM_APP + 0x1000 )

#define WM_SHO_REQ_STATUS           ( WM_SHO + 0x01 )
enum eSHOReq {
    SHO_REQ_STATUS          = 0x01,
    SHO_REQ_SHUTDOWN        = 0x02,         // 서버 기능 종료
    SHO_REQ_CLOSE           = 0x03,         // 서버 프로그램 종료
    SHO_REQ_ANNOUNCEMENT    = 0x04,         // 공지 사항~
    SHO_REQ_FORM_HWND       = 0x05,
    SHO_REQ_USER_LIMIT      = 0x06,         // 최대 동접 제한수
} ;


#define WM_SHO_REPLY_RUNNING                ( WM_SHO + 0x12 )
#define WM_SHO_REPLY_DOWNLOADING            ( WM_SHO + 0x13 )
#define WM_SHO_REPLY_EXTRACTING             ( WM_SHO + 0x14 )
#define WM_SHO_REPLY_LAUNCH_READY           ( WM_SHO + 0x15 )
#define WM_SHO_REPLY_INVALID_COMMAND        ( WM_SHO + 0x16 )     // 상황에 맞지 않는 명령이다..

#define WM_SHO_REPLY_SYSTEM_INIT            ( WM_SHO + 0x21 )     // 0x20 일경우... 서버를 바로 종료시킬수 있다.
#define WM_SHO_REPLY_DATABASE_INIT          ( WM_SHO + 0x22 )
#define WM_SHO_REPLY_NETWORK_INIT           ( WM_SHO + 0x23 )
#define WM_SHO_REPLY_ZONE_INIT              ( WM_SHO + 0x24 )

#define WM_SHO_REPLY_SHUTDOWN_START         ( WM_SHO + 0x31 )
#define WM_SHO_REPLY_SHUTDOWN_END           ( WM_SHO + 0x32 )
#define WM_SHO_REPLY_SHUTDOWN_READY         ( WM_SHO + 0x33 )

#define WM_SHO_REPLY_CLOSE_START            ( WM_SHO + 0x41 )
#define WM_SHO_REPLY_CLOSE_END              ( WM_SHO + 0x42 )
#define WM_SHO_REPLY_CLOSE_READY            ( WM_SHO + 0x43 )

#define WM_SHO_REPLY_PATCH_START            ( WM_SHO + 0x4a )
#define WM_SHO_REPLY_PATCH_COMPLETED        ( WM_SHO + 0x4b )

// 서버 실행중 오류 :: 프로세스가 살아 있다...
#define WM_SHO_FAILED_SYSTEM_INIT           ( WM_SHO + 0x51 )     // 0x50 일경우... 서버를 바로 종료시킬수 있다.
#define WM_SHO_FAILED_DATABASE_INIT         ( WM_SHO + 0x52 )
#define WM_SHO_FAILED_NETWORK_INIT          ( WM_SHO + 0x53 )
#define WM_SHO_FAILED_ZONE_INIT             ( WM_SHO + 0x54 )

#define WM_SHO_FAILED_PROCESS_NOT_FOUND     ( WM_SHO + 0x61 )
#define WM_SHO_FAILED_EXE_FILE_NOT_FOUND    ( WM_SHO + 0x62 )     // 실행파일 없다..
#define WM_SHO_FAILED_EXECUTE_EXE_FILE      ( WM_SHO + 0x63 )     // 서버 실행 오류..
#define WM_SHO_FAILED_SEND_MESSAGE          ( WM_SHO + 0x64 )     // 메세지 전송 오류..
#define WM_SHO_FAILED_DOWNLOAD              ( WM_SHO + 0x65 )
#define WM_SHO_FAILED_EXTRACT               ( WM_SHO + 0x66 )
#define WM_SHO_FAILED_PROCESS_NO_RESPONSE   ( WM_SHO + 0x67 )
#define WM_SHO_FAILED_INVALIED_PASSWORD     ( WM_SHO + 0x68 )

/*
#define WM_SHO_REPLY_USERS          ( WM_SHO + 0x10 )
#define WM_SHO_REPLY_STATUS         ( WM_SHO + 0x11 )

// 주의!!!  순서 함부로 바뀌면 안됨...게임 서버와도 맞춰야 되기 때문에...
enum eSHOReply {
    SHO_REPLY_RUNNING               = 0x12,
    SHO_REPLY_DOWNLOADING           = 0x13,
    SHO_REPLY_EXTRACTING            = 0x14,
    SHO_REPLY_LAUNCH_READY          = 0x15,
    SHO_REPLY_INVALID_COMMAND       = 0x16,     // 상황에 맞지 않는 명령이다..

    SHO_REPLY_SYSTEM_INIT           = 0x21,     // 0x20 일경우... 서버를 바로 종료시킬수 있다.
    SHO_REPLY_DATABASE_INIT         = 0x22,
    SHO_REPLY_NETWORK_INIT          = 0x23,
    SHO_REPLY_ZONE_INIT             = 0x24,

    SHO_REPLY_SHUTDOWN_START        = 0x31,
    SHO_REPLY_SHUTDOWN_END          = 0x32,
    SHO_REPLY_SHUTDOWN_READY        = 0x33,

    SHO_REPLY_CLOSE_START           = 0x41,
    SHO_REPLY_CLOSE_END             = 0x42,
    SHO_REPLY_CLOSE_READY           = 0x43,

    SHO_REPLY_PATCH_READY           = 0x4a,

    // 서버 실행중 오류 :: 프로세스가 살아 있다...
    SHO_FAILED_SYSTEM_INIT          = 0x51,     // 0x50 일경우... 서버를 바로 종료시킬수 있다.
    SHO_FAILED_DATABASE_INIT        = 0x52,
    SHO_FAILED_NETWORK_INIT         = 0x53,
    SHO_FAILED_ZONE_INIT            = 0x54,

    SHO_FAILED_PROCESS_NOT_FOUND    = 0x61,
    SHO_FAILED_EXE_FILE_NOT_FOUND   = 0x62,     // 실행파일 없다..
    SHO_FAILED_EXECUTE_EXE_FILE     = 0x63,     // 서버 실행 오류..
    SHO_FAILED_DOWNLOAD             = 0x65,
    SHO_FAILED_EXTRACT              = 0x66,
    SHO_FAILED_SEND_MESSAGE         = 0x64,     // 메세지 전송 오류..
};
*/

//---------------------------------------------------------------------------

#pragma pack (push, 1)

#define MONCLI_LOGIN_REQ        0x0703
#define MONSRV_LOGIN_REPLY      0x0703

#define MONCLI_COMMAND_REQ      0x0704
#define MONSRV_COMMAND_REPLY    0x0704

#define MONCLI_ANNOUNCE_REQ     0x0705


#define MONCMD_STATUS           0x01
#define MONCMD_LAUNCH           0x02
#define MONCMD_SHUTDOWN         0x03
#define MONCMD_CLOSE_APP        0x04
#define MONCMD_PATCH            0x05
#define MONCMD_USER_LIMIT       0x06
#define MONCMD_SYSTEM_REBOOT    0xff


struct moncli_LOGIN_REQ   : public t_PACKETHEADER {
	union {
		BYTE	m_btMD5[ 32 ];
		DWORD	m_dwMD5[ 8 ];
	} ;
} ;
struct monsrv_LOGIN_REPLY : public t_PACKETHEADER {
    BYTE    m_btAllowed;
} ;

struct moncli_COMMAND_REQ : public t_PACKETHEADER {
    BYTE        m_btCMD;
    union {
        DWORD   m_dwTIME;
        DWORD   m_dwUserLIMIT;
    } ;
/*
    char    m_szURL[];
    char    m_szFILE[];
*/
} ;

struct monsrv_COMMAND_REPLY : public t_PACKETHEADER {
    BYTE    m_btCMD;
    WORD    m_wSTATUS;
    DWORD   m_dwTIME;
    BYTE    m_btCPU;
    union {
        DWORD   m_dwUserCNT;
        WORD    m_wUserCNT[2];  // 0 = current user, 1 = user limit...
    } ;
    char    m_szStatus[0];
} ;

struct moncli_ANNOUNCE_REQ : public t_PACKETHEADER {
    WORD    m_wInterval;
    WORD    m_wRepeatCNT;
    char    m_szMSG[0];
} ;

#ifndef __T_PACKET
#define	__T_PACKET
struct t_PACKET {
	union {
		t_PACKETHEADER				m_HEADER;
		BYTE						m_pDATA[ MAX_PACKET_SIZE ];
		t_NETWORK_STATUS			m_NetSTATUS;

        moncli_LOGIN_REQ            m_moncli_LOGIN_REQ;
        monsrv_LOGIN_REPLY          m_monsrv_LOGIN_REPLY;

        moncli_COMMAND_REQ          m_moncli_COMMAND_REQ;
        monsrv_COMMAND_REPLY        m_monsrv_COMMAND_REPLY;

        moncli_ANNOUNCE_REQ         m_moncli_ANNOUNCE_REQ;
    } ;
};
#endif

#pragma pack (pop)

//---------------------------------------------------------------------------
#endif
