#pragma once

class CSessionOfIOCPType;

typedef struct _IORESULT : public WSAOVERLAPPED
{
	int nEvent;
	CSessionOfIOCPType * pSession;

	enum _Event
	{ 
		ACCEPT, 
		CONNECT, 
		SEND, 
		RECV, 
		CLOSE,
	};
} IORESULT, *LPIORESULT;
