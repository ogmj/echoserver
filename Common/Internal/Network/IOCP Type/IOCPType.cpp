#include "stdafx.h"
#include "SessionIndexMng.h"
#include "IOCPType.h"
#include "IOCPWorker.h"

CIOCPType::CIOCPType()
{
	m_nMaxSession = -1;
	m_dwWorkerCount = 0;
}

CIOCPType::~CIOCPType()
{
	Cleanup();
}

CIOCPType * CIOCPType::GetInstance()
{
	return CSingleton<CIOCPType>::GetInstance();
}

bool CIOCPType::Startup( int nMaxSession, DWORD dwThreadPerProcessor )
{
	WSADATA wd;
	int ret = ::WSAStartup(MAKEWORD(2,2), &wd);
	if ( 0 != ret) 
	{
		//LOG( FORMAT( "WSAStartup fail %d",ret));
		return false;
	}

	//시스템의 프로세스를 얻기 위해 시스템 정보를 얻어온다.
	SYSTEM_INFO sysinfo;
	::GetSystemInfo(&sysinfo);
	m_dwWorkerCount = dwThreadPerProcessor;

	//IOCP 생성
	m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, m_dwWorkerCount );
	if( nullptr == m_hIOCP ) 
	{
		DWORD err = ::GetLastError();
		//LOG( FORMAT( "CreateIoCompletionPort fail %d",err));
		return false;
	}

	//워커 프로세스 생성
	for( DWORD i = 0; i < m_dwWorkerCount; ++i ) 
	{
		CIOCPWorker* pIOCPWorker = new CIOCPWorker;
		if ( nullptr == pIOCPWorker ) 
		{
			//LOG("new fail");
			Cleanup();
			return false;
		}
		if( false == pIOCPWorker->Start( m_hIOCP ) )
		{
			Cleanup();
			return false;
		}
		m_vecIOCPWorkerMng.push_back( pIOCPWorker );
	}

	//각 세션들을 미리 생성
	for( int nId = nMaxSession; nId > 0; --nId ) 
	{
		CSessionOfIOCPType* p  = new CSessionOfIOCPType( nId, m_hIOCP );
		if( nullptr == p ) 
		{
			//LOG("new fail");
			Cleanup();
			return false;
		}
		m_mapSessionMng.Insert( nId, p );
		CSingleton<CSessionIndexMng>::GetInstance()->Push( nId );
	}

	m_nMaxSession = nMaxSession;
	return true;
}

bool CIOCPType::Cleanup()
{
	vector<HANDLE> workerThreads;
	//by ogmj : queue에 있는 모든 데이터를 처리한 후 종료한다.
	for( size_t i = 0; i < m_vecIOCPWorkerMng.size(); ++i )
	{
		workerThreads.push_back( m_vecIOCPWorkerMng[ i ]->GetThreadHandle() );
		::PostQueuedCompletionStatus( m_hIOCP, 0, 0xFFFFFFFF, nullptr ); // post exit message to worker thread

	}
	::WaitForMultipleObjects( (DWORD)m_vecIOCPWorkerMng.size(), &workerThreads[ 0 ], true, INFINITE );

	for( DWORD i = 0; i < m_dwWorkerCount; ++i ) 
	{
		  delete m_vecIOCPWorkerMng[ i ];
	}
	m_vecIOCPWorkerMng.clear();

	for( int nId = m_nMaxSession; nId > 0; --nId )
	{
		CSessionOfIOCPType* p = nullptr;
		if( true == m_mapSessionMng.Find_Const( nId, p ) )
		{
			delete p;
		}
	}
	m_mapSessionMng.Clear();

	::WSACleanup();
	::CloseHandle( m_hIOCP );
	return true;
}

bool CIOCPType::Accept( int nMaxAccept, WORD wLocalPort, DWORD dwLocalIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity )
{
	CAcceptorOfIOCPType * a = new CAcceptorOfIOCPType;
	if (nullptr == a)
	{
//		LOG("new fail");
		return false;
	}

	a->m_hIOCP = m_hIOCP;
	a->m_dwLocalIP = dwLocalIP;
	a->m_wLocalPort = wLocalPort;
	a->m_sListener = ::socket( AF_INET, SOCK_STREAM,IPPROTO_TCP );
	if( INVALID_SOCKET == a->m_sListener ) 
	{
		int err = ::WSAGetLastError();
//		LOG( FORMAT( "socket fail %d", err));
		delete a;
		return false;
	}

	SOCKADDR_IN   addr = { 0 };
	ZeroMemory (&addr, sizeof(addr));
	addr.sin_family       = AF_INET;
	addr.sin_addr.s_addr  = dwLocalIP;
	addr.sin_port         = htons( wLocalPort );

	// link listen socket to iocp
	if( nullptr == ::CreateIoCompletionPort( reinterpret_cast< HANDLE >( a->m_sListener ), m_hIOCP, 0, 0 ) ) 
	{
		DWORD err = ::GetLastError();
//		LOG( FORMAT( "CreateIoCompletionPort fail %d",err));
		delete a;
		return false;
	}

	// bind listen socket
	int ret = ::bind( a->m_sListener, reinterpret_cast< PSOCKADDR >( &addr ), sizeof( addr ) );
	if( SOCKET_ERROR == ret )
	{
		int err = ::WSAGetLastError();
//		LOG( FORMAT( "bind fail %d",err));
		delete a;
		return false;
	}

	// get AcceptEx and GetAcceptExSockaddrs functions
	if (nullptr == g_lpfnAcceptEx) 
	{
		GUID guidAcceptEx = WSAID_ACCEPTEX;
		DWORD dwBytes;
		if (SOCKET_ERROR == ::WSAIoctl( a->m_sListener,
			SIO_GET_EXTENSION_FUNCTION_POINTER, 
			&guidAcceptEx,
			sizeof(guidAcceptEx), 
			&g_lpfnAcceptEx,
			sizeof(g_lpfnAcceptEx), 
			&dwBytes,
			nullptr,
			nullptr)) 
		{
			int err = ::WSAGetLastError();
//			LOG( FORMAT( "WSAIoctl fail %d",err));
			delete a;
			return false;
		}
	}

	if (nullptr == g_lpfnGetAcceptExSockaddrs) 
	{
		GUID  guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
		DWORD dwBytes;
		if (SOCKET_ERROR == ::WSAIoctl( a->m_sListener,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guidGetAcceptExSockaddrs,
			sizeof(guidGetAcceptExSockaddrs),
			&g_lpfnGetAcceptExSockaddrs,
			sizeof(g_lpfnGetAcceptExSockaddrs),
			&dwBytes,
			nullptr,
			nullptr)) 
		{
			int err = ::WSAGetLastError();
//			LOG( FORMAT( "WSAIoctl fail %d", err));
			delete a;
			return false;
		}
	}

	if( SOCKET_ERROR == ::listen( a->m_sListener, SOMAXCONN ) ) 
	{
		int err = ::WSAGetLastError();
//		LOG( FORMAT(  "listen fail %d", err));
		delete a;
		return false;
	}

	// let sessions to prepare to accept
	for( int i = 0; i < nMaxAccept; ++i ) 
	{
		int nId = CSingleton<CSessionIndexMng>::GetInstance()->Pop(); // get free session index from pool
		if( nId < 0 )
		{
//			LOG( FORMAT( "CManager::AcceptEx not enough session %d, %d", maxAccept,i));
			break;
		}
		CSessionOfIOCPType* pSession = nullptr;
		if( true == m_mapSessionMng.Find_Const( nId, pSession ) )
		{
			pSession->AcceptEx( a->m_sListener, dwSendBufCapacity, dwRecvBufCapacity );
		}
	}

	m_vecAcceptorMng.push_back( a );
	return true;
}

bool CIOCPType::Connect( WORD wRemotePort, DWORD wRemoteIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity )
{
	bool bResult = false;
	int nId = CSingleton<CSessionIndexMng>::GetInstance()->Pop(); // get free session index from pool
	if ( nId < 0) 
	{
//		LOG( FORMAT( "Connect fail because session is full %s, %d", ipAddr, remotePort));
		return bResult;
	}

	CSessionOfIOCPType * pSession = nullptr;
	m_mapSessionMng.Find_Const( nId, pSession );
	if( nullptr != pSession )
	{
		bResult = pSession->Connect( wRemotePort, wRemoteIP, dwSendBufCapacity, dwRecvBufCapacity);
	}
	return bResult;
}

bool CIOCPType::Send( const MESSAGE_INFO & mi,  DWORD dwLen, const char* src )
{
	bool bResult = false;
	if ( mi.si.nId > 0 && mi.si.nId <= m_nMaxSession)
	{
		CSessionOfIOCPType * pSession = nullptr;
		m_mapSessionMng.Find_Const( mi.si.nId, pSession );
		if( nullptr != pSession )
		{
			bResult = pSession->Send( mi.si.nId, dwLen, src );
		}	
	}
	return bResult;
}

bool CIOCPType::Peek( const MESSAGE_INFO & mi, DWORD & dwLen )
{
	bool bResult = false;
	if( mi.si.nId > 0 || mi.si.nId <= m_nMaxSession )
	{
		CSessionOfIOCPType * pSession = nullptr;
		m_mapSessionMng.Find_Const( mi.si.nId, pSession );
		if( nullptr != pSession )
		{
			bResult = pSession->Peek( mi.si.nId, dwLen );
		}
	}
	return bResult;
}

bool CIOCPType::Recv( const MESSAGE_INFO & mi, DWORD dwLen, char* dest )
{
	bool bResult = false;
	if( mi.si.nId > 0 || mi.si.nId <= m_nMaxSession )
	{
		CSessionOfIOCPType * pSession = nullptr;
		m_mapSessionMng.Find_Const( mi.si.nId, pSession );
		if( nullptr != pSession )
		{
			bResult = pSession->Recv( mi.si.nId, dwLen, dest );
		}
	}
	return bResult;
}

bool CIOCPType::Close( const MESSAGE_INFO & mi)
{
	bool bResult = false;
	if ( mi.si.nId > 0 || mi.si.nId <= m_nMaxSession )
	{
		CSessionOfIOCPType * pSession = nullptr;
		m_mapSessionMng.Find_Const( mi.si.nId, pSession );
		if( nullptr != pSession )
		{
			bResult = pSession->Close( mi );
		}
	}
	return bResult;
}

bool CIOCPType::Disconnect( const MESSAGE_INFO& mi )
{
	bool bResult = false;
	if ( mi.si.nId > 0 || mi.si.nId <= m_nMaxSession )
	{
		CSessionOfIOCPType * pSession = nullptr;
		m_mapSessionMng.Find_Const( mi.si.nId, pSession );
		if( nullptr != pSession )
		{
			bResult = pSession->Disconnect( mi );
		}
	}
	return bResult;
}