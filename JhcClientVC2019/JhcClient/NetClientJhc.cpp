#include "stdafx.h"
#include "NetClientJhc.h"
#include "TestPacket.h"

extern HWND g_hWnd;

CNetClientJhc::CNetClientJhc()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_SC_ACCEPT_ERROR, &CNetClientJhc::OnSCAcceptError );
	ON_MSG( PACKETTYPE_SC_ECHO, &CNetClientJhc::OnSCEchoMsg );
	ON_MSG( PACKETTYPE_SC_TEST2, &CNetClientJhc::OnSCTest2 );

}

CNetClientJhc::~CNetClientJhc()
{

}

void CNetClientJhc::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, int nFromId )
{
	switch( lpMsg->dwType )
	{
	case DPSYS_CREATEPLAYERORGROUP:
		{
			SetTimer( g_hWnd, 1, 1000, NULL );
			//SendCSEchoMsg();
			break;
		}
	case DPSYS_DESTROYPLAYERORGROUP:
		{
			break;
		}
	case DPSYS_SESSIONLOST:
		{
			break;
		}
	}
}

void CNetClientJhc::UserMessageHandler( CBasePacket & Packet, int nFromId )
{
	void ( theClass::*pfn )( theParameters ) = GetHandler( Packet.dwPacketType );

	if( pfn ) 
	{
		( this->*( pfn ) )( Packet, nFromId );
	}
}

void CNetClientJhc::SendCSTest1( DWORD dwTest1, int nFromId )
{
	CPacket_CS_Test1 msg;
	msg.dwTest1 = 10000;
	Send( &msg, nFromId );
}

void CNetClientJhc::SendCSEchoMsg()
{
	CPacket_CS_Echo msg;

	//SYSTEMTIME tm;
	//msg.dwTime = ::GetLocalTime( &tm );
	msg.dwTick = GetTickCount();
	msg.string = "에코메시지";

	std::string str;
	str = js::format( "발신패킷::CPacket_CS_Echo::%d", msg.dwTick );
	CSingleton< CTrace >::GetInstance()->SetString( str );

	Send( &msg, TO_ME );
}

void CNetClientJhc::OnSCEchoMsg( CBasePacket & Packet, int nFromId )
{
	CPacket_SC_Echo & Packet_SC_Echo = static_cast< CPacket_SC_Echo & >( Packet );

	std::string str;
	str = js::format( "수신패킷::Packet_SC_Echo::%d", Packet_SC_Echo.dwTick );
	CSingleton< CTrace >::GetInstance()->SetString( str );
}

void CNetClientJhc::OnSCTest2( CBasePacket & Packet, int nFromId )
{
	CPacket_SC_Test2 & Packet_SC_Test2 = static_cast< CPacket_SC_Test2 & >( Packet );
}

void CNetClientJhc::OnSCAcceptError( CBasePacket & Packet, int nFromId )
{
	Disconnect();
}