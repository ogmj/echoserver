#pragma once

class CType
{
public:
	CType(){};
	virtual ~CType(){};

	virtual bool Startup( int nMaxSession, DWORD dwThreadPerProcessor ) = 0;
	virtual bool Accept( int nMaxAccept, WORD wLocalPort, DWORD dwLocalIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity ) = 0;
	virtual bool Connect( WORD wRemotePort, DWORD wRemoteIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity ) = 0;
	virtual bool Send( const MESSAGE_INFO& mi, DWORD dwLen, const char * src ) = 0;
	virtual bool Close( const MESSAGE_INFO& mi ) = 0;
	virtual bool Disconnect( const MESSAGE_INFO& mi ) = 0;
	virtual bool Recv( const MESSAGE_INFO& mi, DWORD len, char* dest ) = 0;
	virtual bool Peek( const MESSAGE_INFO& mi, DWORD & dwLen ) = 0;

};