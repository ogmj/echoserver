#pragma once
#include "BasePacketHandler.h"
using namespace tbb;
typedef concurrent_hash_map< int, MESSAGE_INFO > CMapID2MIMng;
typedef concurrent_hash_map< int, MESSAGE_INFO >::iterator  CMapID2MIMngItor;

class CServerPacketHandler : public CBasePacketHandler
{
public:
	CServerPacketHandler() {};
	virtual ~CServerPacketHandler() {};

	virtual void	ProcessPacket();
	virtual void	OnAccept( const MESSAGE_INFO& mi );
	virtual void	OnClose( const MESSAGE_INFO& mi );
	virtual bool	Send( CBasePacket * pPacket, int nFromId );

	bool			SendToAll( char* pBuffer, DWORD dwLen );
	virtual bool	Disconnect( int nFromId );
	bool			GetPlayerAddr( int nFromId, char* lpAddr );
	bool			SendPacket( char* pBuffer, DWORD dwLen, int nFromId );

private:
	CMapID2MIMng m_Id2MIMng;
};