#pragma once
#include "BasePacket.h"

class CBasePacketHandler
{
public:
	CBasePacketHandler() {};
	virtual ~CBasePacketHandler() {};

	virtual void SysMessageHandler( LPDPMSG_GENERIC /*lpMsg*/, DWORD /*dwMsgSize*/, int /*dpId*/ ) {}
	virtual void UserMessageHandler( CBasePacket & Packet, int nFromId ) {}
	virtual void ProcessPacket() {}
	virtual bool Send( char* pBuffer, DWORD dwLen, int nFromId ) { return true; }

};