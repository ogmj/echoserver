#include "stdafx.h"
#include "TypeMng.h"
#include "WMType.h"
#include "IOCPType.h"

class CWMType;

CTypeMng::CTypeMng()
{
	m_pType = NULL;
}

CTypeMng::~CTypeMng()
{

}

void CTypeMng::SelectAInstance( int nType )
{
	switch( nType )
	{
	case WM_TYPE:
		m_pType = static_cast<CType*>( CWMType::GetInstance() );
		break;
	case IOCP_TYPE:
		m_pType = static_cast<CType*>( CIOCPType::GetInstance() );
		break;
	}
}

bool CTypeMng::Startup( int nMaxSession, DWORD dwThreadPerProcessor )
{
	return m_pType->Startup( nMaxSession, dwThreadPerProcessor );
}

bool CTypeMng::Accept( int nMaxAccept, WORD wLocalPort, DWORD dwLocalIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity )
{
	return m_pType->Accept( nMaxAccept, wLocalPort, dwLocalIP, dwSendBufCapacity, dwRecvBufCapacity );
}

bool CTypeMng::Connect ( WORD wRemotePort, DWORD wRemoteIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity )
{
	return m_pType->Connect( wRemotePort, wRemoteIP, dwSendBufCapacity, dwRecvBufCapacity );
}

bool CTypeMng::Send( const MESSAGE_INFO& mi, DWORD dwLen, const char * src )
{
	return m_pType->Send( mi, dwLen, src );
}

bool CTypeMng::Close( const MESSAGE_INFO& mi )
{
	return m_pType->Close( mi );
}

bool CTypeMng::Disconnect( const MESSAGE_INFO& mi )
{
	return m_pType->Disconnect( mi );
}

bool CTypeMng::Recv( const MESSAGE_INFO& mi, DWORD dwLen, char* dest )
{
	return m_pType->Recv( mi, dwLen, dest );
}

bool CTypeMng::Peek( const MESSAGE_INFO& mi, DWORD & dwLen )
{
	return m_pType->Peek( mi, dwLen );
}

