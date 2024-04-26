#include "stdafx.h"
#include "SessionOfIOCPType.h"
#include "SessionIndexMng.h"
#include "IOResult.h"

LPFN_ACCEPTEX               g_lpfnAcceptEx = NULL;
LPFN_GETACCEPTEXSOCKADDRS   g_lpfnGetAcceptExSockaddrs = NULL;

CSessionOfIOCPType::CSessionOfIOCPType()
{

}

CSessionOfIOCPType::CSessionOfIOCPType( int nId, HANDLE hIOCP )
{
	m_hIOCP = hIOCP;
	m_nId = nId;

	m_wsabufSend.buf		= new char[ MAX_WSA_BUF ];
	m_wsabufSend.len		= MAX_WSA_BUF;
	m_wsabufRecv.buf		= new char[ MAX_WSA_BUF ];
	m_wsabufRecv.len		= MAX_WSA_BUF;

	ZeroMemory( &m_ioAccept, sizeof( m_ioAccept ) );
	m_ioAccept.nEvent = IORESULT::ACCEPT;
	m_ioAccept.pSession = this;

	ZeroMemory( &m_ioRecv, sizeof( m_ioRecv ) );
	m_ioRecv.nEvent = IORESULT::RECV;
	m_ioRecv.pSession = this;

	ZeroMemory( &m_ioConnect, sizeof( m_ioConnect ) );
	m_ioConnect.nEvent = IORESULT::CONNECT;
	m_ioConnect.pSession = this;

	ZeroMemory( &m_ioSend, sizeof( m_ioSend ) );
	m_ioSend.nEvent = IORESULT::SEND;
	m_ioSend.pSession = this;
	

	m_dwSendBufCapacity = 0;
	m_dwRecvBufCapacity = 0;

	m_socket = INVALID_SOCKET;
	m_sListener = INVALID_SOCKET;
	m_dwRemoteIP = INADDR_NONE;
	m_wRemotePort = 0;
	m_dwLocalIP = INADDR_NONE;
	m_wLocalPort = 0;

	m_hConnectThread = NULL;
}

CSessionOfIOCPType::~CSessionOfIOCPType()
{
	delete [] m_wsabufSend.buf;
	delete [] m_wsabufRecv.buf;
}

bool CSessionOfIOCPType::AcceptEx( SOCKET sListener, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity )
{
	m_socket  = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); // assign socket
	if( INVALID_SOCKET == m_socket ) 
	{
		DWORD err = ::GetLastError();
		//LOG( FORMAT( "socket fail %d",err));
		Close( IORESULT::ACCEPT );
		return false;
	}

	DWORD dwBytes;
	if ( FALSE == g_lpfnAcceptEx( sListener, 
		m_socket, 
		m_bufAccept, 
		0, 
		sizeof(SOCKADDR_STORAGE) + 16, 
		sizeof(SOCKADDR_STORAGE) + 16, 
		&dwBytes, 
		&m_ioAccept ) ) 
	{
		int err = ::WSAGetLastError();
		if (ERROR_IO_PENDING != err) 
		{
			//LOG( FORMAT( "AcceptEx fail %d",err));
			Close(IORESULT::ACCEPT);
			return false;
		}
	}

	m_sListener = sListener;
	return true;
}

bool CSessionOfIOCPType::Connect( WORD wRemotePort, DWORD dwRemoteIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity )
{
	m_socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); // assign socket
	if (INVALID_SOCKET == m_socket)
	{
		m_nSessionErrorCode = ASE_SOCKET_ERROR;
		m_dwSystemErrorCode = ::WSAGetLastError();
		//LOG( FORMAT( "socket fail %d", m_dwSystemErrorCode));
		Close( IORESULT::CONNECT);
		return false;
	}
	m_dwRemoteIP    = dwRemoteIP;
	m_wRemotePort  = wRemotePort;

	DWORD dwThreadID;

	// call connect in thread because connect function is blocking method
	m_hConnectThread = (HANDLE)_beginthreadex( NULL, 0, ConnectThread, (LPVOID)this, 0, (u_int*)&dwThreadID ); 
	if( NULL == m_hConnectThread ) 
	{
		m_nSessionErrorCode = ASE_SOCKET_ERROR;
		m_dwSystemErrorCode = ::WSAGetLastError();
		//LOG( FORMAT( "CreateThread fail", m_dwSystemErrorCode));
		Close( IORESULT::CONNECT);
		return false;
	}
	return true;

}

u_int CSessionOfIOCPType::ConnectThread( void * param )
{
	if( NULL == param ) 
		return 0;
	CSessionOfIOCPType * pSession = reinterpret_cast< CSessionOfIOCPType* >( param );

	sockaddr_in sockaddr = { 0 };
	ZeroMemory( &sockaddr, sizeof(sockaddr));
	sockaddr.sin_family			= AF_INET;
	sockaddr.sin_addr.s_addr	= pSession->m_dwRemoteIP;
	sockaddr.sin_port	        = htons( pSession->m_wRemotePort );

	if( SOCKET_ERROR == ::connect( pSession->m_socket, reinterpret_cast< const struct sockaddr* >( &sockaddr ), sizeof(sockaddr) ) ) 
	{
		pSession->m_nSessionErrorCode = ASE_SOCKET_ERROR;
		pSession->m_dwSystemErrorCode = ::WSAGetLastError();
		//LOG( FORMAT( "connect fail %d", session->m_dwSystemErrorCode));
		pSession->Close( IORESULT::CONNECT );
		return false;
	}

	// link to iocp and call WSARecv in worker thread because calling thread must be alive for the overlapped IO function. 
	// Otherwise the overlapped IO is canceled 
	PostQueuedCompletionStatus( pSession->m_hIOCP, 0, 0, &pSession->m_ioConnect ); 
	return true;
}

bool CSessionOfIOCPType::Send( int nId, DWORD dwLen, const char * src )
{
	if ( false == m_sendQueue.Enqueue( dwLen, src ) ) // push to send queue.
	{
		m_nSessionErrorCode = ASE_SEND_QUEUE_OVERFLOW;
		//LOG( FORMAT( "send buffer overflow id:%d index:%d", m_id, m_index));
		Close( IORESULT::SEND );
		return false;
	}
	DWORD queueSize = 0; 
	m_sendQueue.ReadLen( queueSize );
	if ( queueSize > 0 )
	{
		if(false == RequestSend( queueSize ) ) // call send function 
		{
			Close( IORESULT::SEND );
			return false;
		}
	}
	return true;
}

bool CSessionOfIOCPType::RequestSend( DWORD dwLen )
{
	m_sendQueue.Dequeue( dwLen, m_wsabufSend.buf );

	m_wsabufSend.len = dwLen;
	DWORD dwSent = 0, dwFlag = 0;
	if (SOCKET_ERROR == WSASend( m_socket, &m_wsabufSend, 1, &dwSent, dwFlag, &m_ioSend, NULL ) ) 
	{
		DWORD dwErr = WSAGetLastError();
		if( WSAEWOULDBLOCK != dwErr )
		{
			m_nSessionErrorCode = ASE_SOCKET_ERROR;
			m_dwSystemErrorCode = dwErr;
			//LOG ( FORMAT( "WSASend fail %d, id:%d index:%d", dwErr, m_id, m_index));
			return false;
		}
		if( ERROR_IO_PENDING != dwErr )
		{
			m_nSessionErrorCode = ASE_SOCKET_ERROR;
			m_dwSystemErrorCode = dwErr;
			return false;
		}
	}
	return true;
}

bool CSessionOfIOCPType::Close( const MESSAGE_INFO& mi )
{
	return Close( IORESULT::CLOSE );
}

bool CSessionOfIOCPType::Close( int nEvent )
{
	if ( 0 == m_nId )
		return true;
	if ( IORESULT::RECV == nEvent || IORESULT::SEND == nEvent || IORESULT::CLOSE == nEvent )
	{
		MESSAGE_INFO * pMsg = new MESSAGE_INFO;
		pMsg->dwMessageEvent = MESSAGE_INFO::CLOSE;
		pMsg->si.nId = m_nId;
		pMsg->si.dwRemoteIP  = m_dwRemoteIP;
		pMsg->si.wRemotePort = m_wRemotePort;
		pMsg->si.dwLocalIP   = m_dwLocalIP;
		pMsg->si.wLocalPort  = m_wLocalPort;
		pMsg->nSessionErrorCode = m_nSessionErrorCode;
		pMsg->dwSystemErrorCode = m_dwSystemErrorCode;
		CSingleton<CMessageQueueMng>::GetInstance()->Push( pMsg );
	}
	else if ( IORESULT::CONNECT == nEvent)
	{
		MESSAGE_INFO * pMsg	= new MESSAGE_INFO;
		pMsg->dwMessageEvent = MESSAGE_INFO::CONNECTFAIL;
		pMsg->si.dwRemoteIP = m_dwRemoteIP;
		pMsg->si.wRemotePort = m_wRemotePort;
		pMsg->nSessionErrorCode = m_nSessionErrorCode;
		pMsg->dwSystemErrorCode = m_dwSystemErrorCode;
		CSingleton<CMessageQueueMng>::GetInstance()->Push( pMsg );
	}

	if( INVALID_SOCKET != m_sListener )
	{
		AcceptEx( m_sListener, m_dwSendBufCapacity, m_dwRecvBufCapacity); // make overlapped accept io one more time.
	}
	else
	{
		CSingleton<CSessionIndexMng>::GetInstance()->Push( m_nId );
	}
	return true;
}

bool CSessionOfIOCPType::Disconnect( const MESSAGE_INFO& mi )
{
	return Close( IORESULT::CLOSE );
}

bool CSessionOfIOCPType::Disconnect( int nEvent )
{
	if ( 0 == m_nId )
		return true;
	if ( IORESULT::RECV == nEvent || IORESULT::SEND == nEvent || IORESULT::CLOSE == nEvent )
	{
		MESSAGE_INFO * pMsg = new MESSAGE_INFO;
		pMsg->dwMessageEvent = MESSAGE_INFO::CLOSE;
		pMsg->si.nId = m_nId;
		pMsg->si.dwRemoteIP  = m_dwRemoteIP;
		pMsg->si.wRemotePort = m_wRemotePort;
		pMsg->si.dwLocalIP   = m_dwLocalIP;
		pMsg->si.wLocalPort  = m_wLocalPort;
		pMsg->nSessionErrorCode = m_nSessionErrorCode;
		pMsg->dwSystemErrorCode = m_dwSystemErrorCode;
		CSingleton<CMessageQueueMng>::GetInstance()->Push( pMsg );
	}
	else if ( IORESULT::CONNECT == nEvent)
	{
		MESSAGE_INFO * pMsg	= new MESSAGE_INFO;
		pMsg->dwMessageEvent = MESSAGE_INFO::CONNECTFAIL;
		pMsg->si.dwRemoteIP = m_dwRemoteIP;
		pMsg->si.wRemotePort = m_wRemotePort;
		pMsg->nSessionErrorCode = m_nSessionErrorCode;
		pMsg->dwSystemErrorCode = m_dwSystemErrorCode;
		CSingleton<CMessageQueueMng>::GetInstance()->Push( pMsg );
	}

	if( INVALID_SOCKET != m_sListener )
	{
		AcceptEx( m_sListener, m_dwSendBufCapacity, m_dwRecvBufCapacity); // make overlapped accept io one more time.
	}
	else
	{
		CSingleton<CSessionIndexMng>::GetInstance()->Push( m_nId );
	}
	return true;
}


bool CSessionOfIOCPType::RequestRecv()
{
	DWORD dwRead =0 , dwFlag = 0;

	if ( SOCKET_ERROR == WSARecv( m_socket, &(m_wsabufRecv), 1, &dwRead, &dwFlag, &m_ioRecv, NULL ) ) 
	{
		DWORD err = WSAGetLastError();
		if ( ERROR_IO_PENDING != err ) 
		{
			m_nSessionErrorCode = ASE_SOCKET_ERROR;
			m_dwSystemErrorCode = err;
			//LOG ( FORMAT( "WSARecv fail %d, id:%d, index:%d", err, m_id, m_index));
			return false;		
		}
	}
	return true;
}

bool CSessionOfIOCPType::RequestRecv( DWORD dwLen )
{
	DWORD dwRead =0 , dwFlag = 0;
	
	if ( SOCKET_ERROR == WSARecv( m_socket, &(m_wsabufRecv), 1, &dwRead, &dwFlag, &m_ioRecv, NULL ) ) 
	{
		DWORD err = WSAGetLastError();
		if ( ERROR_IO_PENDING != err ) 
		{
			m_nSessionErrorCode = ASE_SOCKET_ERROR;
			m_dwSystemErrorCode = err;
			//LOG ( FORMAT( "WSARecv fail %d, id:%d, index:%d", err, m_id, m_index));
			return false;		
		}
		else
			return true;
	}

	if( dwRead > 0 )
	{
		if (false ==  m_recvQueue.Enqueue( dwRead, m_wsabufRecv.buf ) ) 
		{
			m_nSessionErrorCode = ASE_RECV_QUEUE_OVERFOLW;
			//LOG( FORMAT( "recv buffer overflow id:%d index:%d", m_id, m_index));
			Close( IORESULT::RECV );
		}
	}
	return true;
}

bool CSessionOfIOCPType::Peek( int nId, DWORD & dwLen )
{
	if ( false == m_recvQueue.ReadLen( dwLen ) )
	{
		return false;
	}
	return true;
}

bool CSessionOfIOCPType::Recv( int nId, DWORD dwLen, char* dest )
{
	if ( false == m_recvQueue.Dequeue( dwLen, dest ) )
	{
		return false;
	}
	return true;
}

void CSessionOfIOCPType::OnIOCompletion( int nEvent, DWORD dwRet, ULONG_PTR dwKey, DWORD dwTs )
{
	if( IORESULT::ACCEPT == nEvent )
	{
		if( dwRet )
		{
			if( 0 != setsockopt( m_socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (const char*)&m_sListener, sizeof(SOCKET) ) )
			{
				Close( MESSAGE_INFO::ACCEPT );
				return;
			}

			if( NULL == ::CreateIoCompletionPort( reinterpret_cast< HANDLE >( m_socket ), m_hIOCP, m_nId, 0) ) // link to iocp
			{	
//				LOG( FORMAT( "CreateIoCompletionPort fail %d", ::GetLastError()));
				Close( IORESULT::ACCEPT );
				return;
			}
			if( false == RequestRecv() ) // make overlapped recv call
			{
				Close( IORESULT::ACCEPT );
				return;
			}
			SOCKADDR_STORAGE* localSockaddr = NULL;
			SOCKADDR_STORAGE* remoteSockaddr = NULL;
			int localSockaddrLen ;
			int remoteSockaddrLen ;
			g_lpfnGetAcceptExSockaddrs( (void*)m_bufAccept,0, sizeof(SOCKADDR_STORAGE)+16, sizeof(SOCKADDR_STORAGE)+16, (SOCKADDR **)&localSockaddr, &localSockaddrLen, (SOCKADDR **)&remoteSockaddr, &remoteSockaddrLen);
			m_dwRemoteIP = ((SOCKADDR_IN*)remoteSockaddr)->sin_addr.S_un.S_addr;
			m_wRemotePort = ntohs(((SOCKADDR_IN*)remoteSockaddr)->sin_port);
			m_dwLocalIP = ((SOCKADDR_IN*)localSockaddr)->sin_addr.S_un.S_addr;
			m_wLocalPort = ntohs(((SOCKADDR_IN*)localSockaddr)->sin_port);
			
			// queue message of ACCEPT
			MESSAGE_INFO * pMsg	= new MESSAGE_INFO;
			pMsg->dwMessageEvent = MESSAGE_INFO::ACCEPT;
			pMsg->si.nId = m_nId;
			pMsg->si.dwRemoteIP = m_dwRemoteIP;
			pMsg->si.wRemotePort = m_wRemotePort;
			pMsg->si.dwLocalIP = m_dwLocalIP;
			pMsg->si.wLocalPort = m_wLocalPort;

			CSingleton<CMessageQueueMng>::GetInstance()->Push( pMsg );
		}
		else
		{
//			LOG( FORMAT( "GQCS fail %d from accept id:%d index:%d", ::WSAGetLastError(), m_id, m_index));
			Close( IORESULT::ACCEPT );//close accept socket 시 무한 재귀 호출
		}
	}
	else if( IORESULT::CONNECT == nEvent )
	{
		if( dwRet )
		{
			SOCKADDR_IN addr;
			int addrLen = sizeof(addr);
			if (SOCKET_ERROR == getsockname( m_socket, reinterpret_cast< struct sockaddr* >( &addr ), &addrLen ) )
			{
				m_nSessionErrorCode = ASE_SOCKET_ERROR;
				m_dwSystemErrorCode = ::WSAGetLastError();
				Close(IORESULT::CONNECT);
				return;
			}
			m_dwLocalIP = addr.sin_addr.S_un.S_addr;
			m_wLocalPort = addr.sin_port;

			// link to iocp 
			if( NULL == ::CreateIoCompletionPort( reinterpret_cast< HANDLE >( m_socket ), m_hIOCP, m_nId, 0 ) )
			{ 
				m_nSessionErrorCode = ASE_SOCKET_ERROR;
				m_dwSystemErrorCode = ::GetLastError();
//				LOG( FORMAT( "CreateIoCompletionPort fail %d", m_dwSystemErrorCode));
				Close( IORESULT::CONNECT );
				return;
			}

			// make overlapped recv call
			if( false == RequestRecv() )
			{
				Close(IORESULT::CONNECT);
				return;
			}
			MESSAGE_INFO * pMsg = new MESSAGE_INFO;
			pMsg->dwMessageEvent = MESSAGE_INFO::CONNECT;
			pMsg->si.nId = m_nId;
			pMsg->si.dwRemoteIP = m_dwRemoteIP;
			pMsg->si.wRemotePort = m_wRemotePort;
			pMsg->si.dwLocalIP = m_dwLocalIP;
			pMsg->si.wLocalPort = m_wLocalPort;
			CSingleton<CMessageQueueMng>::GetInstance()->Push( pMsg );
		}
		else
		{
			m_nSessionErrorCode = ASE_SOCKET_ERROR;
			m_dwSystemErrorCode = ::WSAGetLastError();
//			LOG( FORMAT( "GQCS fail %d from connect id:%d index:%d", m_dwSystemErrorCode, m_id, m_index));
			Close(IORESULT::CONNECT);
		}
	}
	else if( IORESULT::SEND == nEvent )
	{
		if( dwKey == m_nId )
		{
			if( dwRet && dwTs > 0 )
			{
				DWORD queueSize = 0; 
				m_sendQueue.ReadLen( queueSize );
				if ( queueSize > 0 )
				{
					if( false == RequestSend( queueSize ) )
					{
						m_nSessionErrorCode = ASE_SOCKET_ERROR;
						m_dwSystemErrorCode = ::WSAGetLastError();
						Close( IORESULT::SEND );
					}
				}
			}
			else
			{
				m_nSessionErrorCode = ASE_SOCKET_ERROR;
				m_dwSystemErrorCode = ::WSAGetLastError();
//				LOG( FORMAT( "GQCS fail %d from send id:%d index:%d", m_dwSystemErrorCode, m_id, m_index));
				Close( IORESULT::SEND );
			}
		}
	}
	else if( IORESULT::RECV == nEvent )
	{
		if( dwKey == m_nId )
		{
			if( dwRet && dwTs > 0 )
			{
				if( false  == RequestRecv() )
				{
					m_nSessionErrorCode = ASE_SOCKET_ERROR;
					m_dwSystemErrorCode = ::WSAGetLastError();
					Close( IORESULT::RECV );
				}

				if (false ==  m_recvQueue.Enqueue( dwTs, m_wsabufRecv.buf ) ) 
				{
					m_nSessionErrorCode = ASE_RECV_QUEUE_OVERFOLW;
					//LOG( FORMAT( "recv buffer overflow id:%d index:%d", m_id, m_index));
					Close( IORESULT::RECV );
				}
			}
			else
			{
				m_nSessionErrorCode = ASE_SOCKET_ERROR;
				m_dwSystemErrorCode = ::WSAGetLastError();
//				LOG( FORMAT( "GQCS fail %d from recv id:%d inex %d", m_dwSystemErrorCode, m_id, m_index));
				Close( IORESULT::RECV );
			}
		}
	}
}

//CSessionOfIOCPTypeMng////////////////////////////////////////////////////////////////////
bool CSessionOfIOCPTypeMng::Find_Const( int nId, CSessionOfIOCPType * &pSession )
{
	bool bResult;
	CMapSessionOfIOCPType::const_accessor const_accessor;
	bResult = m_mapSessionOfIOCPType.find( const_accessor, nId );
	if ( true == bResult )
	{
		pSession = const_accessor->second;
	}
	const_accessor.release();
	return bResult;
}

void CSessionOfIOCPTypeMng::Insert( int nId, CSessionOfIOCPType * pSession )
{
	CMapSessionOfIOCPType::accessor accesor;
	if( true == m_mapSessionOfIOCPType.insert( accesor, nId ) )
	{
		accesor->second = pSession;
	}
	accesor.release();

}

void CSessionOfIOCPTypeMng::Erase( int nId )
{
	CMapSessionOfIOCPType::accessor accessor;
	if ( true == m_mapSessionOfIOCPType.find( accessor, nId ) )
	{
		m_mapSessionOfIOCPType.erase( accessor );
	}
}

void CSessionOfIOCPTypeMng::Clear()
{
	m_mapSessionOfIOCPType.clear();
}
