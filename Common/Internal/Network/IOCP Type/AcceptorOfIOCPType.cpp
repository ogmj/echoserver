#include  "stdafx.h"
#include  "AcceptorOfIOCPType.h"

CAcceptorOfIOCPType::CAcceptorOfIOCPType()
{
	m_dwLocalIP = INADDR_NONE;
	m_wLocalPort = 0;
	m_hIOCP = nullptr;
	m_hThreadHandle = nullptr;
	m_dwThreadId = 0;
	m_sListener = INVALID_SOCKET;
}

CAcceptorOfIOCPType::~CAcceptorOfIOCPType()
{
	Clean();
}

bool CAcceptorOfIOCPType::Start( DWORD dwIp, WORD wPort )
{
	m_dwLocalIP    = dwIp;
	m_wLocalPort  = wPort;

	m_hThreadHandle = reinterpret_cast< HANDLE >( _beginthreadex( nullptr, 0, Accept, reinterpret_cast< LPVOID >( this ), 0, reinterpret_cast< u_int* >( &m_dwThreadId ) ) );
	if( nullptr == m_hThreadHandle )
	{
//		LOG( FORMAT("_beginthreadex fail %d", ::GetLastError()));
		return false;
	}
	return true;

}

bool CAcceptorOfIOCPType::Clean()
{
	::PostQueuedCompletionStatus( m_hIOCP, 0, 0xFFFFFFFF, nullptr ); // post exit message to worker thread
	::WaitForSingleObject( m_hThreadHandle, INFINITE ); // wait until the thread exit
	if( nullptr != m_hThreadHandle )
	{
		::CloseHandle( m_hThreadHandle );
		m_hThreadHandle = nullptr;
	}
	m_dwThreadId = 0;

	m_dwLocalIP = INADDR_NONE;
	m_wLocalPort = 0;
	m_hIOCP = nullptr;

	return true;
}

u_int  CAcceptorOfIOCPType::Accept( void * param )
{
	return 0;
}
