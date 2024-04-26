#pragma once
#include "ClientPacketHandler.h"
#include "BasePacket.h"

#undef	theClass
#define	theClass	CNetClientJhc
#undef theParameters
#define theParameters	CBasePacket &, int

class CNetClientJhc : public CClientPacketHandler
{
public:
	CNetClientJhc();
	virtual ~CNetClientJhc();
	USES_PFNENTRIES;

	virtual	void SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, int nFromId );
	virtual void UserMessageHandler( CBasePacket & Packet, int nFromId );

	void SendCSTest1( DWORD dwTest1, int nFromId );
	void SendCSEchoMsg();
	void OnSCEchoMsg( CBasePacket & Packet, int nFromId );
	void OnSCTest2( CBasePacket & Packet, int nFromId );
	void OnSCAcceptError( CBasePacket & Packet, int nFromId );
private:
};