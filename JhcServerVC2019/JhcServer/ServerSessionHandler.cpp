#include "stdafx.h"
#include "ServerSessionHandler.h"
#include "NetServerJhc.h"

void CServerSessionHandler::OnAccept( const MESSAGE_INFO& mi )
{
	DPMSG_CREATEPLAYERORGROUP msg;
	msg.dwType	= DPSYS_CREATEPLAYERORGROUP;
	msg.nId	= mi.si.nId;

	if( PN_JHCSRVR == mi.si.wLocalPort ) // 클라이언트로부터 접속
	{
		CSingleton< CNetServerJhc >::GetInstance()->OnAccept( mi ); 
		CSingleton< CNetServerJhc >::GetInstance()->SysMessageHandler( (LPDPMSG_GENERIC) &msg, 0, mi.si.nId );
	}
}

void CServerSessionHandler::OnClose( const MESSAGE_INFO& mi )
{
	DPMSG_DESTROYPLAYERORGROUP msg;
	msg.dwType	= DPSYS_DESTROYPLAYERORGROUP;
	msg.nId	= mi.si.nId;

//	if ( PN_LOBBYSRVR == mi.wLocalPort) // 클라이언트로부터 접속
//	{
//		g_NetServerLobby.OnClose( mi );
//		g_NetServerLobby.SysMessageHandler( (LPDPMSG_GENERIC) &msg, 0, mi.si.dwId);
//	}
//	else if ( PN_ACCOUNTSRVR_1 == mi.wRemotePort )
//	{
//		g_NetClientAccount.OnClose( mi);
//		g_NetClientAccount.SysMessageHandler( (LPDPMSG_GENERIC)&msg, 0, mi.si.dwId);
//	}
//	else if ( PN_ROOMSRVR_LOBBY == mi.wRemotePort )
//	{
//		g_NetClientRoom.OnConnect( mi );
//		g_NetClientRoom.SysMessageHandler( (LPDPMSG_GENERIC)&msg, 0, mi.si.dwId );
//	}
}

void CServerSessionHandler::OnConnect( const MESSAGE_INFO& mi )
{
	DPMSG_CREATEPLAYERORGROUP msg;
	msg.dwType	= DPSYS_CREATEPLAYERORGROUP;
	msg.nId	= mi.si.nId;

//	if ( PN_ACCOUNTSRVR_1 == mi.si.wRemotePort)
//	{
//		g_NetClientAccount.OnConnect( mi );
		//g_NetClientAccount.SendAddIdofServer( g_appInfo.dwSys, g_appInfo.dwId );
//		g_NetClientAccount.SysMessageHandler( (LPDPMSG_GENERIC)&msg, 0, mi.si.dwId);
//	} 
//	else if ( PN_ROOMSRVR_LOBBY == mi.si.wRemotePort)
//	{
//		g_NetClientRoom.OnConnect( mi );
//		char roomIP[32];
		//AS::ConvertIpAddress( mi.si.dwRemoteIP, roomIP );
		//g_MyTrace.Add(  CMyTrace::Key( "RoomServers" ), FALSE, "Connected RoomServers - %s", roomIP );
//		g_NetClientRoom.SysMessageHandler( (LPDPMSG_GENERIC)&msg, 0, mi.si.dwId );
//	}

}

void CServerSessionHandler::OnConnectFail( const MESSAGE_INFO &mi )
{

}