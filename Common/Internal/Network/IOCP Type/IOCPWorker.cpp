#include "stdafx.h"
#include "IOCPWorker.h"
#include "SessionOfIOCPType.h"

CIOCPWorker::CIOCPWorker()
{
	m_hIOCP = NULL;
	m_hThreadHandle = NULL;
	m_dwThreadId = 0;
}

CIOCPWorker::~CIOCPWorker()
{

}

bool CIOCPWorker::Start( HANDLE hIOCP )
{
	m_hIOCP = hIOCP;
	m_hThreadHandle = reinterpret_cast< HANDLE >( ::_beginthreadex( NULL, 0, Work, reinterpret_cast< LPVOID >( m_hIOCP ), 0, reinterpret_cast< u_int* >( &m_dwThreadId ) ) );
	if( NULL == m_hThreadHandle )
	{
		DWORD err = ::GetLastError();
//		LOG( FORMAT("_beginthreadex fail %d", err));
		return false;
	}
	return true;
}

bool CIOCPWorker::Clean()
{
	if( NULL != m_hThreadHandle )
	{
		::CloseHandle( m_hThreadHandle );
		m_hThreadHandle = NULL;
	}
	m_hIOCP = NULL;
	m_dwThreadId = 0;
	return true;
}

unsigned int CIOCPWorker::Work( LPVOID param )
{
	HANDLE hIocp = reinterpret_cast< HANDLE >( param );
	DWORD dwTs;// transfered size
	ULONG_PTR dwKey; // completion key
	OVERLAPPED * pOverLapped;		
	IORESULT * pIOResult;		
	BOOL bRet;
	while( 1 )
	{
		bRet = ::GetQueuedCompletionStatus( hIocp, &dwTs, &dwKey, reinterpret_cast< LPOVERLAPPED * >( &pOverLapped ), INFINITE );
		pIOResult = static_cast< IORESULT * >( pOverLapped );
		if( NULL != pIOResult )
		{
			if( NULL != pIOResult->pSession )
			{
				pIOResult->pSession->OnIOCompletion( pIOResult->nEvent, bRet, dwKey, dwTs );
			}
		}
		else if( 0xFFFFFFFF == dwKey )
		{
			break;
		}
	}
	return 0;
}