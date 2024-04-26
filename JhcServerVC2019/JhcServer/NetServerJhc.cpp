#include "stdafx.h"
#include "NetServerJhc.h"

CNetServerJhc::CNetServerJhc()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_CS_TEST1, &CNetServerJhc::OnCSTest1 );
	ON_MSG( PACKETTYPE_CS_ECHO, &CNetServerJhc::OnCSEcho );
}

CNetServerJhc::~CNetServerJhc()
{

}

void CNetServerJhc::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD uBufSize, int nFromId )
{
	switch( lpMsg->dwType ) 
	{
	case DPSYS_CREATEPLAYERORGROUP:
		{
			LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
			break;
		}
	case DPSYS_DESTROYPLAYERORGROUP:
		{
			LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
			break;
		}
	}
}


void CNetServerJhc::UserMessageHandler( CBasePacket & Packet, int nFromId )
{
	//by ogmj: lpMsg와 dwMsgSize로 패킷타입을 구해야 한다.
//	DWORD dw = GetMsg( lpMsg, dwMsgSize );
	void ( theClass::*pfn )( theParameters ) = GetHandler( Packet.dwPacketType );

	if( pfn )
	{
		( this->*( pfn ) )( Packet, nFromId );
	}
}

DWORD CNetServerJhc::GetMsg( void * lpBuf, DWORD dwMsgSize ) 
{
	BYTE lpResultMsg[4];
	LPBYTE lpBufStart = reinterpret_cast<LPBYTE>( lpBuf );
	memcpy( lpResultMsg, lpBufStart, dwMsgSize );
	return ( (lpResultMsg[dwMsgSize-1] & 0xFF)|
		((lpResultMsg[dwMsgSize-2] & 0xFF) << 8)|
		((lpResultMsg[dwMsgSize-3] & 0xFF) << 16)|
		((lpResultMsg[dwMsgSize-4] & 0xFF) << 24) );
}

void CNetServerJhc::SendSCEcho( DWORD dwTick, int nFromId )
{
	CPacket_SC_Echo msg;
	msg.dwTick = dwTick;
	Send( &msg, nFromId );
}

void CNetServerJhc::OnCSTest1( CBasePacket & Packet, int nFromId )
{
	CPacket_CS_Test1 & Packet_CS_Test1 = static_cast< CPacket_CS_Test1 & >( Packet );
}

void CNetServerJhc::OnCSEcho( CBasePacket & Packet, int nFromId )
{
	CPacket_CS_Echo & Packet_CS_Echo = static_cast< CPacket_CS_Echo & >( Packet );

	std::string str;
	str = js::format( "수신패킷:발신자ID %d::%s::%d", nFromId, Packet_CS_Echo.string.c_str(), Packet_CS_Echo.dwTick );
	CSingleton< CTrace >::GetInstance()->SetString( str );

	SendSCEcho( Packet_CS_Echo.dwTick, nFromId );
}
