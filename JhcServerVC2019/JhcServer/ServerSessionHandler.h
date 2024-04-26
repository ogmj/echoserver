#pragma once
#include "SessionHandler.h"

class CServerSessionHandler : public CSessionHandler
{
public:
	CServerSessionHandler() {}
	~CServerSessionHandler() {}

	virtual void OnConnect( const MESSAGE_INFO& mi );
	virtual void OnConnectFail( const MESSAGE_INFO& mi );
	virtual	void OnAccept( const MESSAGE_INFO& mi );
	virtual void OnClose( const MESSAGE_INFO& mi );
};