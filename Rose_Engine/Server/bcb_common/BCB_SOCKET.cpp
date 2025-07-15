
#include <vcl.h>
#include <mmsystem.h>
#include "BCB_Socket.h"
#include "BCB_Tools.h"


//-------------------------------------------------------------------------------------------------
BCB_Socket::BCB_Socket()
{
    m_pRecvPket = NULL;
    m_dwSocketID = 0;

    m_pRecvPket = (t_PACKETHEADER *)new BYTE[ MAX_PACKET_SIZE ];
}
BCB_Socket::~BCB_Socket()
{
    if ( m_pRecvPket ) {
        delete[] m_pRecvPket;
        m_pRecvPket = NULL;
    }
}

//-------------------------------------------------------------------------------------------------
bool BCB_Socket::_Init (TCustomWinSocket *pSocket, DWORD dwSocketID)
{
    if ( m_pRecvPket == NULL ) {
        ;
        // Error Message ...
        LogString (0xffff, "ERROR[%s:%d] : New Connection Packet Memory Allocation Failed ...\n", __FILE__, __LINE__);
        ;
        return false;
    }

//  m_IPHashKEY = 0;

    m_nSendByte = 0;
    m_nRecvByte = 0;
    m_nPacketSize = 0;

    m_pSocket      = pSocket;
    m_bCloseSocket = false;
    m_dwSocketID   = dwSocketID;

    // m_cStatus   = CLIENT_STATUS_CONNECT;
    m_dwLastActionTime = ::timeGetTime ();

    return true;
}
void BCB_Socket::_Free (void)
{
/*
	// *** 이미 클라이언트 리스트에서 제거되어 SendPacket loop에서 절때루 호출 안된다.
	this->LockSENDq ();		// classCLIENT::Client_Free
		classDLLNODE <classPACKET *> *pPketNode;
		pPketNode = m_SendPketQ.GetHeadNode ();
		while ( pPketNode ) {
			m_SendPketQ.DeleteNode (pPketNode);

			Packet_DecRefCount( pPketNode->DATA );
			delete   pPketNode;

			pPketNode = m_SendPketQ.GetHeadNode ();
		}
	this->UnlockSENDq ();
*/
}

//-------------------------------------------------------------------------------------------------
int BCB_Socket::ReceiveLength ()
{
    if ( this->m_pSocket->SocketHandle != INVALID_SOCKET ) {
        // get receive buff length
        unsigned long ulResult;
        ::ioctlsocket(this->m_pSocket->SocketHandle, FIONREAD, &ulResult);
        return ulResult;
    }

    return 0;
}

int BCB_Socket::ReceiveBuf(void *pBuf, int iCount)
{
    int iResult;

    this->m_pSocket->Lock ();
    __try {
        iResult = 0;
        if ( this->m_pSocket->SocketHandle != INVALID_SOCKET ) {
            iResult = ::recv( this->m_pSocket->SocketHandle, (char*)pBuf, iCount, 0);
        }
    }
    __finally {
        this->m_pSocket->Unlock ();
    }
    return iResult;
/*
function TCustomWinSocket.ReceiveBuf(var Buf; Count: Integer): Integer;
var
  ErrorCode: Integer;
begin
  Lock;
  try
    Result := 0;
    if (Count = -1) and FConnected then
      ioctlsocket(FSocket, FIONREAD, Longint(Result))
    else begin
      if not FConnected then Exit;
      Result := recv(FSocket, Buf, Count, 0);
      if Result = SOCKET_ERROR then
      begin
        ErrorCode := WSAGetLastError;
        if ErrorCode <> WSAEWOULDBLOCK then
        begin
          Error(Self, eeReceive, ErrorCode);
          Disconnect(FSocket);
          if ErrorCode <> 0 then
            raise ESocketError.CreateResFmt(@sWindowsSocketError,
              [SysErrorMessage(ErrorCode), ErrorCode, 'recv']);
        end;
      end;
    end;
  finally
    Unlock;
  end;
end;
*/
}


//-------------------------------------------------------------------------------------------------
int BCB_Socket::SendBuf(void *pBuf, int iCount)
{
    int iResult;

    this->m_pSocket->Lock ();
    __try {
        iResult = SOCKET_ERROR;
        if ( this->m_pSocket->SocketHandle != INVALID_SOCKET )
            iResult = ::send( this->m_pSocket->SocketHandle, (const char*)pBuf, iCount, 0);
    }
    __finally {
        this->m_pSocket->Unlock ();
    }

    return iResult;
/*
function TCustomWinSocket.SendBuf(var Buf; Count: Integer): Integer;
var
  ErrorCode: Integer;
begin
  Lock;
  try
    Result := 0;
    if not FConnected then Exit;
    Result := send(FSocket, Buf, Count, 0);
    if Result = SOCKET_ERROR then
    begin
      ErrorCode := WSAGetLastError;
      if (ErrorCode <> WSAEWOULDBLOCK) then
      begin
        Error(Self, eeSend, ErrorCode);
        Disconnect(FSocket);
        if ErrorCode <> 0 then
          raise ESocketError.CreateResFmt(@sWindowsSocketError,
            [SysErrorMessage(ErrorCode), ErrorCode, 'send']);
      end;
    end;
  finally
    Unlock;
  end;
end;
*/
}

//-------------------------------------------------------------------------------------------------
bool BCB_Socket::AppendSendPacket (classPACKET *pCPacket)
{
    if ( pCPacket->m_HEADER.m_nSize >= MAX_PACKET_SIZE ) {
        LogString (0xffff, "ERROR[%s:%d] : Too big send packet ... Type: 0x%x, Size: %d", __FILE__, __LINE__, pCPacket->m_HEADER.m_wType, pCPacket->m_HEADER.m_nSize);

        m_bCloseSocket = true;
        return true;
    }

    classDLLNODE <classPACKET*> *pNewNode;
    pNewNode = new classDLLNODE<classPACKET*> (NULL);
    if ( pNewNode == NULL ) {
        // ERROR out of Memory ...
        return false;
    }

    if ( 0 == pCPacket->GetLength() ) {
        pCPacket->SetLength( this->E_SendP( &pCPacket->m_HEADER ) );
        _ASSERT( pCPacket->GetLength() >= sizeof(t_PACKETHEADER) );
    }
    
    pCPacket->IncRefCnt();
    pNewNode->DATA = pCPacket;

    this->LockSENDq ();	// classCLIENT::AppendSendPacket
        m_SendPketQ.AppendNode (pNewNode);
        if ( m_SendPketQ.GetNodeCount() == 1 ) {
            // 비어 있던 큐다
        }
    this->UnlockSENDq ();

    this->SetSendEVENT ();

    return true;
}

//-------------------------------------------------------------------------------------------------
bool BCB_Socket::SendPacket ()
{                
    static DWORD dwCurrentTime;

    dwCurrentTime = timeGetTime ();

    if ( !m_bIsWritable ) {
        // 쓰기 상태가 아니다...
        /*
        if ( dwCurrentTime - this->m_dwLastActionTime >= TIMEOUT_DISCONNECT ) {
            // 바로 pClientNode->DATA.m_pSocket->Close () 를 호출하면 데드락 걸린다.
            WM_CloseSOCKET( pClientNode->DATA.m_pSocket, 3 );
        }
        */
        return false;
    }

    if ( this->m_SendPketQ.GetNodeCount () ) {
        classDLLNODE <classPACKET*> *pPketNode;
        int  iSendByte, iPacketSize;
        bool bSendFile;

        this->m_dwLastActionTime = dwCurrentTime;

        this->LockSENDq ();	// classCLIENTMANAGER::Proc
            pPketNode   = this->m_SendPketQ.GetHeadNode ();
        this->UnlockSENDq ();
        while ( pPketNode != NULL ) {
            iPacketSize = pPketNode->DATA->GetLength();

            // iSendByte   = this->m_pSocket->SendBuf ( (BYTE*) pPketNode->DATA->m_pDATA + this->m_nSendByte, iPacketSize - this->m_nSendByte);
            iSendByte   = this->SendBuf ( (BYTE*) pPketNode->DATA->m_pDATA + this->m_nSendByte, iPacketSize - this->m_nSendByte);
            /*
            if ( this->m_pSocket->SocketHandle == INVALID_SOCKET )
                return false;
            iSendByte = ::send( this->m_pSocket->SocketHandle,
                                (BYTE*) pPketNode->DATA->m_pDATA + this->m_nSendByte,
                                iPacketSize - this->m_nSendByte, 0);
            */
            if ( iSendByte == SOCKET_ERROR ) {
                int iErrorCode;
                iErrorCode = WSAGetLastError ();
                if ( WSAEWOULDBLOCK != iErrorCode ) {
                    /* TODO : 접속 끊어야 한다. */
                    this->CloseSOCKET( "SendPacket:: Sock ERR" );

                    return false;
                }

                // LogString (LOG_DEBUG, "%d Socket WSAEWOULDBLOCK ::SendPacket()  \n", this->m_pSocket->SocketHandle );
                
                this->m_bIsWritable = false;
                break;
            } else {
                this->m_nSendByte += iSendByte;
                if ( this->m_nSendByte == iPacketSize ) {
                    classDLLNODE <classPACKET*> *pDelNode;

                    pDelNode  = pPketNode;
                    this->LockSENDq ();	// classCLIENTMANAGER::Proc
                        pPketNode = this->m_SendPketQ.GetNextNode (pDelNode);
                        this->m_SendPketQ.DeleteNode ( pDelNode );
                    this->UnlockSENDq ();

                    Packet_DecRefCount( pDelNode->DATA );
                    delete   pDelNode;

                    this->m_nSendByte = 0;
                }
            }
        }
    } else {
        /*
        if ( dwCurrentTime - this->m_dwLastActionTime > TIMEOUT_INTERVAL ) {
            pCPacket = ::Packet_AllocOnly();
            pCPacket->m_HEADER.wType = TN_SERVER_CHECK_ALIVE;
            pCPacket->m_HEADER.m_nSize = sizeof( t_normal_header );
            this->AppendSendPacket( pCPacket );
        }
        */
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
void BCB_Socket::ON_Read (void)
{
    int   iRecvByte, iQueueByte;

    iQueueByte = this->ReceiveLength ();
    /*
    // 여기 풀면 ON_Read()가 후에 ON_Read()가 호출이 안되어 홀딩되는 경우가 있다...
    if  (iQueueByte <= 0 ) {
        LogString (LOG_DEBUG, "%d Socket::ON_Read() : %d bytes, GetReceiveLEN() \n", this->m_pSocket->SocketHandle, iQueueByte);
        return;
    }
    */
    m_dwLastActionTime = timeGetTime ();
    do {
        if ( m_nRecvByte < sizeof(t_PACKETHEADER) ) {
            // iRecvByte = m_pSocket->ReceiveBuf ( m_pRecvPket->m_pDATA + m_nRecvByte, sizeof(t_PACKETHEADER) - m_nRecvByte);
            iRecvByte = this->ReceiveBuf ( m_pRecvPket->m_pDATA + m_nRecvByte, sizeof(t_PACKETHEADER) - m_nRecvByte);
        } else {
            /*
            nPacketLen = m_pRecvPket->m_nSize;
            if ( nPacketLen >= MAX_PACKET_SIZE ) {
                // Error Too Big Packet or Invalid Packet !!!
                ;
                // LogString (LOG_NORMAL, "ERROR[%s:%d] : Too Big packet or Invalid packet ... Type: 0x%x, Size: %d", __FILE__, __LINE__, m_pRecvPket->m_HEADER.m_wType, m_pRecvPket->m_HEADER.m_nSize);
                ;
                m_nRecvByte    = 0;
                m_bIsWritable  = false;
                // m_bCloseSocket = true;
                LogString (0xffff, "Too big packet size :: IP[ %s ] Close() ...\n", __FILE__, __LINE__, m_pSocket->RemoteAddress.c_str() );

                this->CloseSOCKET( 2 );

                break;
            }
            */

            iRecvByte = this->ReceiveBuf ( m_pRecvPket->m_pDATA + m_nRecvByte, m_nPacketSize - m_nRecvByte);
        }

        if ( iRecvByte == SOCKET_ERROR ) {
            int iErrorCode;
            iErrorCode = WSAGetLastError ();
            if ( WSAEWOULDBLOCK != iErrorCode ) {
                // Error(Self, eeReceive, ErrorCode);
                this->CloseSOCKET( "ON_Read:: Sock ERR" );
                return;
            }

            // LogString (LOG_DEBUG, "%d Socket WSAEWOULDBLOCK ::ON_Read()  \n", this->m_pSocket->SocketHandle );

            return;
        } else
        if ( iRecvByte <= 0 )
            return;

        // FormMain->m_iRecvBytesPerSec += iRecvByte;

        m_nRecvByte += iRecvByte;
        if ( m_nRecvByte >= sizeof( t_PACKETHEADER ) ) {
            if ( m_nRecvByte == sizeof( t_PACKETHEADER ) ) {
                // Decoding Packet Header ...
                this->m_nPacketSize = this->D_RecvH( m_pRecvPket );
                if ( !this->m_nPacketSize ) {
                    // 잘못된 패킷 ...
                    this->CloseSOCKET( "ON_Read:: Invalid Packet" );
                    return;
                }
            }

            _ASSERT( this->m_nPacketSize );
            _ASSERT( m_nRecvByte <= this->m_nPacketSize );
            
            if ( m_nRecvByte == this->m_nPacketSize ) {
                m_pRecvPket->m_pDATA[ m_nRecvByte ] = 0;

                if ( !this->D_RecvB( m_pRecvPket ) ) {
                    // 이상한 패킷이다.
                    this->CloseSOCKET( "ON_Read:: Decode ERR" );
                    return;
                }

                if ( !this->HandlePACKET () ) {
                    // 이상한 패킷이다.
                    this->CloseSOCKET( "ON_Read:: Handle Packet" );
                    return;
                }

                m_nRecvByte = 0;
            }
        }

        iQueueByte -= iRecvByte;
    } while ( iQueueByte > 0 ) ;
}

//-------------------------------------------------------------------------------------------------
// extern void WM_CloseSOCKET (TCustomWinSocket *pSocket, int iFrom);
void BCB_Socket::CloseSOCKET (char *szMSG)
{
    // WM_CloseSOCKET( this->m_pSocket, iFrom );
    _ASSERT( 0 );
}

//-------------------------------------------------------------------------------------------------
void BCB_Socket::SetSendEVENT (void)
{
    // WM_SendPACKET( this->m_pSocket );
    _ASSERT( 0 );
}

//-------------------------------------------------------------------------------------------------
bool BCB_Socket::HandlePACKET (void)
{
    LogString (0xffff, "virtual BCB_Socket::HandlePACKET() type: 0x%x, Size: %d ", m_pRecvPket->m_wType, m_pRecvPket->m_nSize);
    return true;
}



//-------------------------------------------------------------------------------------------------

