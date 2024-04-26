#include "stdafx.h"
#include "ClientSessionHandler.h"
#include "NetClientJhc.h"

void CClientSessionHandler::OnConnect( const MESSAGE_INFO& mi )
{
	m_bIsConnecting = false;

	DPMSG_GENERIC msg;
	msg.dwType = DPSYS_CREATEPLAYERORGROUP;

	if ( PN_JHCSRVR == mi.si.wRemotePort)
	{
		CSingleton< CNetClientJhc >::GetInstance()->OnConnect( mi );
		CSingleton< CNetClientJhc >::GetInstance()->SysMessageHandler( (LPDPMSG_GENERIC) &msg, 0, mi.si.nId );
	}
}

void CClientSessionHandler::OnConnectFail( const MESSAGE_INFO& mi )
{
	m_bIsConnecting = false;
}

void CClientSessionHandler::OnClose( const MESSAGE_INFO &mi )
{
	DPMSG_GENERIC msg;
	msg.dwType = DPSYS_DESTROYPLAYERORGROUP;

	if ( PN_JHCSRVR == mi.si.wRemotePort)
	{
		//g_netClientLobby.OnClose( si);
		//g_netClientLobby.SysMessageHandler( &msg, 0, si.dwId);
	}
}

bool CClientSessionHandler::Connect( WORD wRemotePort, DWORD dwRemoteIP, DWORD dwSendBufCapacity , DWORD dwRecvBufCapacity )
{
	if ( false == CSessionHandler::Connect( wRemotePort, dwRemoteIP, dwSendBufCapacity, dwRecvBufCapacity))
		return false;
	m_bIsConnecting = true;
	return true;
}

