#include "stdafx.h"
#include "SessionOfWMType.h"
#include "SessionIndexMng.h"
#include "ToGetIndexBySocketMng.h"

CSessionOfWMType::CSessionOfWMType()
{
	m_nId				= 0;

	m_socket            = INVALID_SOCKET;
	m_remoteIP          = INADDR_NONE;
	m_remotePort        = 0;
	m_localIP           = INADDR_NONE;
	m_localPort         = 0;

	m_nSessionErrorCode = -1;
	m_dwSystemErrorCode = 0;

	m_wsabufSend.buf		= new char[ MAX_WSA_BUF ];
	m_wsabufSend.len		= MAX_WSA_BUF;
	m_wsabufRecv.buf		= new char[ MAX_WSA_BUF ];
	m_wsabufRecv.len		= MAX_WSA_BUF;
}

CSessionOfWMType::CSessionOfWMType( int nIndex )
{
	m_nId			= nIndex;

	m_socket            = INVALID_SOCKET;
	m_remoteIP          = INADDR_NONE;
	m_remotePort        = 0;
	m_localIP           = INADDR_NONE;
	m_localPort         = 0;

	m_nSessionErrorCode = -1;
	m_dwSystemErrorCode = 0;

	m_wsabufSend.buf		= new char[ MAX_WSA_BUF ];
	m_wsabufSend.len		= MAX_WSA_BUF;
	m_wsabufRecv.buf		= new char[ MAX_WSA_BUF ];
	m_wsabufRecv.len		= MAX_WSA_BUF;
}

CSessionOfWMType::~CSessionOfWMType()
{
	delete [] m_wsabufSend.buf;
	delete [] m_wsabufRecv.buf;
}

bool CSessionOfWMType::AcceptError( SOCKET listener, HWND hWnd )
{
	SOCKADDR_IN		remoteAddr, localAddr;
	int addrLen = sizeof(SOCKADDR_IN);

	//m_id      = GenerateSessionId();                                           // assign id
	m_socket  = ::accept( listener, (struct sockaddr*)& remoteAddr, &addrLen);  // assign socket
	if( INVALID_SOCKET == m_socket )
	{
		//LOG( FORMAT( "accept fail %d", ::GetLastError()));
		Close( FD_ACCEPT);
		return false;
	}

	//	CKeepAlive::GetInstance()->KeepAlive( m_socket);

	m_remoteIP    = remoteAddr.sin_addr.S_un.S_addr;
	m_remotePort  = ntohs( remoteAddr.sin_port );

	//by ogmj: 소켓 인덱스 등록
//	CSingleton<CToGetIndexBySocketMng>::GetInstance()->Insert( m_socket, m_nId );

	if( SOCKET_ERROR == getsockname( m_socket, (struct sockaddr*)&localAddr, &addrLen) )
	{
		//LOG( FORMAT( "getsockname fail", ::WSAGetLastError()));
		Close( FD_ACCEPT);
		return false;
	}
	m_localIP   = localAddr.sin_addr.S_un.S_addr;
	m_localPort = ntohs( localAddr.sin_port );

	// use window message for checking socket event.
	if( SOCKET_ERROR == ::WSAAsyncSelect( m_socket, hWnd, WM_SOCKET, FD_WRITE | FD_READ | FD_CLOSE ) )
	{
		//LOG( FORMAT( "WSAAsyncSelect fail %d", ::WSAGetLastError()));
		Close( FD_ACCEPT );
		return false;
	}
	CPacket_SC_AcceptError msg;
	CBasePacket * pPacket = &msg;
	Send( m_nId, pPacket->nSize, reinterpret_cast<char *>( pPacket ) );
/*
	// queue ACCEPT message
	MESSAGE_INFO * pMsg		= new MESSAGE_INFO;
	pMsg->dwMessageEvent	= MESSAGE_INFO::CLOSE;
	pMsg->si.nId			= m_nId;
	pMsg->si.dwRemoteIP		= m_remoteIP;
	pMsg->si.wRemotePort	= m_remotePort;
	pMsg->si.dwLocalIP		= m_localIP;
	pMsg->si.wLocalPort		= m_localPort;

	CSingleton<CMessageQueueMng>::GetInstance()->Push( pMsg );
*/
	return true;
}

bool CSessionOfWMType::Accept( SOCKET listener, HWND hWnd )
{
	SOCKADDR_IN		remoteAddr, localAddr;
	int addrLen = sizeof(SOCKADDR_IN);

	m_socket  = ::accept( listener, (struct sockaddr*)& remoteAddr, &addrLen);  // assign socket
	if( INVALID_SOCKET == m_socket )
	{
		//LOG( FORMAT( "accept fail %d", ::GetLastError()));
		Close( FD_ACCEPT);
		return false;
	}

	m_remoteIP    = remoteAddr.sin_addr.S_un.S_addr;
	m_remotePort  = ntohs( remoteAddr.sin_port );

	//by ogmj: 소켓, 인덱스 등록
	CSingleton<CToGetIndexBySocketMng>::GetInstance()->Insert( m_socket, m_nId );

	if( SOCKET_ERROR == getsockname( m_socket, (struct sockaddr*)&localAddr, &addrLen) )
	{
		//LOG( FORMAT( "getsockname fail", ::WSAGetLastError()));
		Close( FD_ACCEPT);
		return false;
	}
	m_localIP   = localAddr.sin_addr.S_un.S_addr;
	m_localPort = ntohs( localAddr.sin_port );

	// use window message for checking socket event.
	if( SOCKET_ERROR == ::WSAAsyncSelect( m_socket, hWnd, WM_SOCKET, FD_WRITE | FD_READ | FD_CLOSE ) )
	{
		//LOG( FORMAT( "WSAAsyncSelect fail %d", ::WSAGetLastError()));
		Close( FD_ACCEPT);
		return false;
	}

	// queue ACCEPT message
	MESSAGE_INFO * pMsg		= new MESSAGE_INFO;
	pMsg->dwMessageEvent	= MESSAGE_INFO::ACCEPT;
	pMsg->si.nId			= m_nId;
	pMsg->si.dwRemoteIP		= m_remoteIP;
	pMsg->si.wRemotePort	= m_remotePort;
	pMsg->si.dwLocalIP		= m_localIP;
	pMsg->si.wLocalPort		= m_localPort;

	CSingleton<CMessageQueueMng>::GetInstance()->Push( pMsg );

	return true;
}

bool  CSessionOfWMType::Close( DWORD event)
{
	//LOG( FORMAT( "CSession2::Close id:%d, index:%d, event:%d", m_id, m_index, event));

	//if (0 == m_id)
	//	return true;

	if ( FD_CLOSE == event || FD_WRITE == event || FD_READ == event)
	{		
		MESSAGE_INFO * pMsg	= new MESSAGE_INFO;
		pMsg->dwMessageEvent = MESSAGE_INFO::CLOSE;
		pMsg->si.nId = m_nId;
		pMsg->si.dwRemoteIP = m_remoteIP;
		pMsg->si.wRemotePort = m_remotePort;
		pMsg->si.dwLocalIP = m_localIP;
		pMsg->si.wLocalPort = m_localPort;
		pMsg->nSessionErrorCode = m_nSessionErrorCode;
		pMsg->dwSystemErrorCode = m_dwSystemErrorCode;
		CSingleton<CMessageQueueMng>::GetInstance()->Push( pMsg );
	}
	else if ( FD_CONNECT == event)
	{	
		MESSAGE_INFO * pMsg	= new MESSAGE_INFO;
		pMsg->dwMessageEvent = MESSAGE_INFO::CONNECTFAIL;
		pMsg->si.dwRemoteIP = m_remoteIP;
		pMsg->si.wRemotePort = m_remotePort;
		pMsg->nSessionErrorCode = m_nSessionErrorCode;
		pMsg->dwSystemErrorCode = m_dwSystemErrorCode;
		CSingleton<CMessageQueueMng>::GetInstance()->Push( pMsg );
	}
	//by ogmj:인덱스 반환
	CSingleton<CSessionIndexMng>::GetInstance()->Push( m_nId );
	CSingleton<CToGetIndexBySocketMng>::GetInstance()->Erase( m_socket );

	//Reset( m_sendBufCapacity, m_recvBufCapacity);
	return true;
}

bool  CSessionOfWMType::Close( const MESSAGE_INFO& mi )
{
	//if ( si.dwId != m_id) 
	//{
	//	//LOG ( FORMAT( "CSession2::Recv invalid id:%d, session id:%d index:%d", si.dwId, m_id, m_index));
	//	return false;
	//}
	return Close( FD_CLOSE );
}

bool  CSessionOfWMType::Disconnect( const MESSAGE_INFO& mi )
{
	return Disconnect( FD_CLOSE );
}
bool  CSessionOfWMType::Disconnect( DWORD event )
{
	//LOG( FORMAT( "CSession2::Close id:%d, index:%d, event:%d", m_id, m_index, event));

	if ( FD_CLOSE == event || FD_WRITE == event || FD_READ == event)
	{		
		MESSAGE_INFO * pMsg	= new MESSAGE_INFO;
		pMsg->dwMessageEvent = MESSAGE_INFO::CLOSE;
		pMsg->si.nId = m_nId;
		pMsg->si.dwRemoteIP = m_remoteIP;
		pMsg->si.wRemotePort = m_remotePort;
		pMsg->si.dwLocalIP = m_localIP;
		pMsg->si.wLocalPort = m_localPort;
		pMsg->nSessionErrorCode = m_nSessionErrorCode;
		pMsg->dwSystemErrorCode = m_dwSystemErrorCode;
		CSingleton<CMessageQueueMng>::GetInstance()->Push( pMsg );
	}
	else if ( FD_CONNECT == event)
	{	
		MESSAGE_INFO * pMsg	= new MESSAGE_INFO;
		pMsg->dwMessageEvent = MESSAGE_INFO::CONNECTFAIL;
		pMsg->si.dwRemoteIP = m_remoteIP;
		pMsg->si.wRemotePort = m_remotePort;
		pMsg->nSessionErrorCode = m_nSessionErrorCode;
		pMsg->dwSystemErrorCode = m_dwSystemErrorCode;
		CSingleton<CMessageQueueMng>::GetInstance()->Push( pMsg );
	}
	//by ogmj:인덱스 반환
	CSingleton<CSessionIndexMng>::GetInstance()->Push( m_nId );
	CSingleton<CToGetIndexBySocketMng>::GetInstance()->Erase( m_socket );

	::closesocket( m_socket );
	return true;
}


void  CSessionOfWMType::OnIoEvent ( DWORD event, DWORD error )
{
	if( !error)
	{
		if ( FD_WRITE == event) 
		{
			DWORD queueSize = 0; 
			m_sendQueue.ReadLen( queueSize );
			if ( queueSize > 0 )
			{
				if ( false == RequestSend( queueSize ) )
					Close( FD_WRITE );
			}
		}
		else if( FD_READ == event ) 
		{
			if(false == RequestRecv() )
				Close( FD_READ);
		}
		else if( FD_CONNECT == event ) 
		{
			SOCKADDR_IN localAddr;
			int addrLen = sizeof(localAddr);
			if (SOCKET_ERROR == getsockname( m_socket,(struct sockaddr*)&localAddr, &addrLen))
			{
				m_nSessionErrorCode = ASE_SOCKET_ERROR;
				m_dwSystemErrorCode = ::WSAGetLastError();
				//LOG( FORMAT( "getsockname fail", m_dwSystemErrorCode ));
				Close( FD_CONNECT);
				return;
			}
			m_localIP   = localAddr.sin_addr.S_un.S_addr;
			m_localPort = localAddr.sin_port;

			MESSAGE_INFO * pMsg		= new MESSAGE_INFO;
			pMsg->dwMessageEvent    = MESSAGE_INFO::CONNECT;
			pMsg->si.nId		   = m_nId;
			pMsg->si.dwRemoteIP   = m_remoteIP;
			pMsg->si.wRemotePort  = m_remotePort;
			pMsg->si.dwLocalIP    = m_localIP;
			pMsg->si.wLocalPort   = m_localPort;
			CSingleton<CMessageQueueMng>::GetInstance()->Push( pMsg );
		}
		else if ( FD_CLOSE == event )
		{
			m_nSessionErrorCode = ASE_SOCKET_ERROR;
			m_dwSystemErrorCode = ::WSAGetLastError();
			Close( FD_CLOSE);
		}
	} 
	else
	{
		m_nSessionErrorCode = ASE_SOCKET_ERROR;
		m_dwSystemErrorCode = error;
		//LOG( FORMAT( "WM_SOCKET error: %d event: %d id:%d index:%d", error, event, m_id, m_index));
		Close( event);
	}
}

bool  CSessionOfWMType::RequestRecv()
{
	DWORD dwRead =0 , dwFlag = 0;
	if ( SOCKET_ERROR == WSARecv( m_socket, &(m_wsabufRecv), 1, &dwRead, &dwFlag, NULL, NULL ) ) 
	{
		DWORD err = WSAGetLastError();
		if ( WSAEWOULDBLOCK != err ) 
		{
			m_nSessionErrorCode = ASE_SOCKET_ERROR;
			m_dwSystemErrorCode = err;
			//LOG ( FORMAT( "WSARecv fail %d, id:%d, index:%d", err, m_id, m_index));
			return false;		
		}
		else
			return true;
	}

	assert( dwRead > 0 );
	if (false ==  m_recvQueue.Enqueue( dwRead, m_wsabufRecv.buf ) ) 
	{
		m_nSessionErrorCode = ASE_RECV_QUEUE_OVERFOLW;
		//LOG( FORMAT( "recv buffer overflow id:%d index:%d", m_id, m_index));
		Close( FD_READ );
	}
	return true;

}

bool CSessionOfWMType::RequestSend( DWORD dwLen )
{
	m_sendQueue.Dequeue( dwLen, m_wsabufSend.buf );

	m_wsabufSend.len = dwLen;
	DWORD dwSent = 0, dwFlag = 0;
	if (SOCKET_ERROR == WSASend( m_socket, &m_wsabufSend, 1, &dwSent, dwFlag, NULL, NULL ) ) 
	{
		DWORD dwErr = WSAGetLastError();
		if( WSAEWOULDBLOCK != dwErr )
		{
			m_nSessionErrorCode = ASE_SOCKET_ERROR;
			m_dwSystemErrorCode = dwErr;
			//LOG ( FORMAT( "WSASend fail %d, id:%d index:%d", dwErr, m_id, m_index));
			return false;
		}
	}
	return true;
}

bool CSessionOfWMType::Connect( HWND hWnd, WORD remotePort, DWORD remoteIP, DWORD sendBufCapacity, DWORD recvBufCapacity )
{
//	bool  ret = Reset(sendBufCapacity,recvBufCapacity);
//	if (false == ret) 
//	{
//		LOG( FORMAT( "CSession2::Reset fail %d, %d",sendBufCapacity,recvBufCapacity));
//		Close( FD_CONNECT);
//		return false;
//	}

	m_socket  = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // assign socket

	if (INVALID_SOCKET == m_socket) 
	{
		m_nSessionErrorCode = ASE_SOCKET_ERROR;
		m_dwSystemErrorCode = ::WSAGetLastError();
		//LOG( FORMAT( "socket fail %d", m_dwSystemErrorCode));
		Close( FD_CONNECT);
		return false;
	}

	CSingleton<CToGetIndexBySocketMng>::GetInstance()->Insert( m_socket, m_nId );
	// use window message for checking socket event.
	if (SOCKET_ERROR == ::WSAAsyncSelect( m_socket, hWnd, WM_SOCKET, FD_CONNECT | FD_WRITE | FD_READ | FD_CLOSE ) )
	{
		m_nSessionErrorCode = ASE_SOCKET_ERROR;
		m_dwSystemErrorCode = ::WSAGetLastError();
		//LOG( FORMAT( "WSAAsyncSelect fail %d", m_dwSystemErrorCode));
		Close( FD_CONNECT);
		return false;
	}

	SOCKADDR_IN    remoteAddr = { 0 };
	ZeroMemory( &remoteAddr, sizeof(remoteAddr));
	remoteAddr.sin_family			  = AF_INET;
	remoteAddr.sin_addr.s_addr	= remoteIP;
	remoteAddr.sin_port	        = htons(remotePort);
	// connect
	if (SOCKET_ERROR == ::connect( m_socket, (const struct sockaddr*) &remoteAddr, sizeof(sockaddr))) 
	{
		int err = ::WSAGetLastError();
		if (WSAEWOULDBLOCK != err)
		{
			m_nSessionErrorCode = ASE_SOCKET_ERROR;
			m_dwSystemErrorCode = err;
			//LOG( FORMAT( "connect fail %d", err));
			Close( FD_CONNECT);
			return false;
		}
	}

	m_remoteIP    = remoteIP;
	m_remotePort  = remotePort;
	return true;

}

bool CSessionOfWMType::Send( int nId, DWORD dwLen, const char * src )
{
	if ( false == m_sendQueue.Enqueue( dwLen, src ) ) // push to send queue.
	{
		m_nSessionErrorCode = ASE_SEND_QUEUE_OVERFLOW;
		//LOG( FORMAT( "send buffer overflow id:%d index:%d", m_id, m_index));
		Close( FD_WRITE );
		return false;
	}
	DWORD queueSize = 0; 
	m_sendQueue.ReadLen( queueSize );
	if ( queueSize > 0 )
	{
		if(false == RequestSend( queueSize ) ) // call send function 
		{
			Close( FD_WRITE );
			return false;
		}
	}
	return true;
}

bool CSessionOfWMType::Recv( int nId, DWORD dwLen, char* dest )
{
	if ( false == m_recvQueue.Dequeue( dwLen, dest ) )
	{
		//LOG ( FORMAT( "CCircularQueue::Dequeue fail %d %d", len, dest));
		return false;
	}
	return true;

}

bool CSessionOfWMType::Peek( int nId, DWORD & dwLen )
{
	if ( false == m_recvQueue.ReadLen( dwLen ) )
	{
		return false;
	}
	return true;
}

bool CSessionOfWMTypeMng::Find_Const( int nId, CSessionOfWMType * &pSession )
{
	bool bResult;
	CMapSessionOfWMType::const_accessor const_accessor;
	bResult = m_mapSessionOfWMType.find( const_accessor, nId );
	if ( true == bResult )
	{
		pSession = const_accessor->second;
	}
	const_accessor.release();
	return bResult;
}

void CSessionOfWMTypeMng::Insert( int nId, CSessionOfWMType * pSession )
{
	CMapSessionOfWMType::accessor accesor;
	if( true == m_mapSessionOfWMType.insert( accesor, nId ) )
	{
		accesor->second = pSession;
	}
	accesor.release();

}

void CSessionOfWMTypeMng::Erase( int nId )
{
	CMapSessionOfWMType::accessor accessor;
	if ( true == m_mapSessionOfWMType.find( accessor, nId ) )
	{
		m_mapSessionOfWMType.erase( accessor );
	}
}

void CSessionOfWMTypeMng::Clear()
{
	m_mapSessionOfWMType.clear();
}
