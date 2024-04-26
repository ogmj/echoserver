#pragma once
#include "BasePacketHandler.h"

class CClientPacketHandler : public CBasePacketHandler
{
public:
	CClientPacketHandler();
	virtual ~CClientPacketHandler();

	virtual void OnConnect( const MESSAGE_INFO& mi );
	virtual void OnClose();

	virtual void ProcessPacket();
	virtual bool Send( CBasePacket * pPacket, int nFromId = -1 );
	virtual bool Disconnect();
	
	int GetSessionId();
private:
	MESSAGE_INFO m_mi;
};