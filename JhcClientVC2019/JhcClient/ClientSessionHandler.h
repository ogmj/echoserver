#pragma once
#include "SessionHandler.h"

class CClientSessionHandler : public CSessionHandler
{
public:
	CClientSessionHandler() { m_bIsConnecting = false; }
	~CClientSessionHandler() {}
	virtual void OnConnect( const MESSAGE_INFO& mi );
	virtual void OnConnectFail( const MESSAGE_INFO& mi );
	virtual void OnClose( const MESSAGE_INFO& mi );
	virtual void OnAccept( const MESSAGE_INFO& mi ) {};

	bool Connect( WORD wRemotePort, DWORD dwRemoteIP, DWORD dwSendBufCapacity /* = MAX_PACKET_SIZE */, DWORD dwRecvBufCapacity /* = MAX_PACKET_SIZE */ );
	inline bool IsConnecting() const { return m_bIsConnecting; }

private:
	bool m_bIsConnecting;
};