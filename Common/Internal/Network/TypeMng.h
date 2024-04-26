#pragma once
#include "Type.h"

class CTypeMng
{
public:
	CTypeMng();
	~CTypeMng();

	void SelectAInstance( int nType );
	bool Startup( int nMaxSession, DWORD dwThreadPerProcessor );
	bool Accept ( int nMaxAccept, WORD wLocalPort, DWORD dwLocalIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity );
	bool Connect ( WORD wRemotePort, DWORD wRemoteIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity );
	bool Send ( const MESSAGE_INFO& mi, DWORD dwLen, const char * src );
	bool Close( const MESSAGE_INFO& mi );
	bool Disconnect( const MESSAGE_INFO& mi );
	bool Recv( const MESSAGE_INFO& mi, DWORD len, char* dest );
	bool Peek( const MESSAGE_INFO& mi, DWORD & dwLen );

private:

	CType * m_pType;
};