//#pragma once
#include "ServerPacketHandler.h"
#include "BasePacket.h"
#include "TestPacket.h"

#undef	theClass
#define theClass	CNetServerJhc
#undef theParameters
#define theParameters	CBasePacket &, int

class CNetServerJhc : public CServerPacketHandler
{
public:
	CNetServerJhc();
	virtual ~CNetServerJhc();

	//	Operations
	virtual void SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, int nFromId );
	virtual void UserMessageHandler( CBasePacket & Packet, int nFromId );

	//by ogmj: 패킷에서 메시지를 분리
	DWORD GetMsg( void * lpBuf, DWORD dwMsgSize );

	void OnCSTest1( CBasePacket & pPacket, int nFromId );
	void SendSCEcho( DWORD dwTick, int nFromId );
	void OnCSEcho( CBasePacket & pPacket, int nFromId );

	USES_PFNENTRIES;
private:
};