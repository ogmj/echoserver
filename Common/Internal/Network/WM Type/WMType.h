#pragma once

#include "Type.h"
#include "SessionOfWMType.h"
#include "AcceptorOfWMType.h"
using namespace tbb;

class CWMType : public CType
{
public:
	CWMType();
	virtual ~CWMType();

	static CWMType * GetInstance();

	bool Startup( int nMaxSession, DWORD dwThreadPerProcessor = 0 );
	void SocketProc( SOCKET s, DWORD event, DWORD error );
	bool Cleanup();
	bool Accept( int nMaxAccept, WORD wLocalPort, DWORD dwLocalIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity );
	bool Connect ( WORD wRemotePort, DWORD wRemoteIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity );
	bool Send( const MESSAGE_INFO& mi, DWORD dwLen, const char * src );
	bool Close( const MESSAGE_INFO& mi );
	bool Disconnect( const MESSAGE_INFO& mi );
	bool Recv( const MESSAGE_INFO& mi, DWORD dwLen, char * dest );
	bool Peek( const MESSAGE_INFO& mi, DWORD & dwLen );

private:

	CSessionOfWMTypeMng m_mapSessionMng;
	CAcceptorOfWMTypeMng m_mapAcceptorMng;

	static  u_int WINAPI Work (LPVOID param);
	HANDLE	m_hThreadHandle;
	DWORD	m_dwThreadId;
	int		m_nMaxSession;
	
};