/*
	$Header: /7HeartsOnline/Server/SHO_GS/SHO_GS_LIB/Common/CClientSOCKET.h 1     05-11-23 7:03p Young $
*/
#ifndef	__CClientSOCKET_H
#define	__CClientSOCKET_H
#include "DLLIST.h"
#include "CRawSOCKET.h"
#include "PacketHEADER.h"
//-------------------------------------------------------------------------------------------------

struct t_PACKET;
struct t_SendPACKET;

class CClientSOCKET : public CRawSOCKET
{
private:
	bool				_Init (void);
	void				_Free (void);

	t_PACKET		   *m_pRecvPacket;
	short				m_nRecvBytes;
	short				m_nPacketSize;

	short				m_nSendBytes;
	bool				m_bWritable;

	CRITICAL_SECTION	m_csThread;
	HANDLE				m_hThreadEvent;
	HANDLE				m_hThread;
	// DWORD			m_dwThreadID;
	unsigned int		m_dwThreadID;

	char				m_cStatus;

	classDLLIST <t_PACKET *>		m_RecvPacketQ;
	classDLLIST <t_SendPACKET *>	m_SendPacketQ;
	classDLLIST <t_SendPACKET *>	m_WaitPacketQ;

	classDLLIST <struct tagUDPPACKET *>	m_RecvUDPPacketQ;

protected:
	//friend	DWORD WINAPI ClientSOCKET_SendTHREAD (LPVOID lpParameter);
	friend  unsigned __stdcall ClientSOCKET_SendTHREAD( void* lpParameter );

	void	Packet_Recv (int iToRecvBytes);
	bool	Packet_Send (void);

	virtual WORD	mF_ESP	(t_PACKETHEADER *pPacket);
	virtual WORD	mF_DRH	(t_PACKETHEADER *pPacket);
	virtual short	mF_DRB	(t_PACKETHEADER *pPacket);

public:
    classDLLNODE <CClientSOCKET *> *m_pNode;

	CClientSOCKET ()			{	_Init();	}
	virtual ~CClientSOCKET()	{	_Free();	}

	void	Packet_Register2RecvUDPQ (u_long ulFromIP, WORD wFromPort, int iPacketSize);
	void	Packet_RecvFrom (void);
	void	SetSendEvent ()			{	::SetEvent( m_hThreadEvent );	}

	virtual void	mF_Init			(DWORD dwInit);
	virtual void	OnConnect		(int nErrorCode);
	virtual void	OnClose			(int nErrorCode);
	virtual void	OnReceive		(int nErrorCode);
	virtual void	OnSend			(int nErrorCode);
	virtual void	OnAccepted		(int*pCode);
	virtual	bool    WndPROC			( WPARAM wParam, LPARAM lParam )=0;

public:
	// user interface
	bool	Connect (HWND hWND, char *szServerIP, int iTCPPort, UINT uiWindowMsg);
	void	Close ();

	virtual void Set_NetSTATUS (BYTE btStatus);

	void	Packet_Register2RecvQ (t_PACKET *pRegPacket);
	void	Packet_Register2SendQ (t_PACKET *pRegPacket);
	bool	Peek_Packet (t_PACKET *pPacket, bool bRemoveFromQ=true);
} ;

//-------------------------------------------------------------------------------------------------
#endif
