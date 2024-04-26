#pragma once

class CAcceptorOfIOCPType
{
public:
	CAcceptorOfIOCPType();
	virtual ~CAcceptorOfIOCPType();

	bool  Start( DWORD dwIp, WORD wPort );
	bool  Clean();
	static  u_int WINAPI Accept( void * param );

	SOCKET m_sListener;
	DWORD m_dwLocalIP;
	WORD m_wLocalPort;
	HANDLE m_hIOCP;

private:
	HANDLE m_hThreadHandle;
	DWORD m_dwThreadId;
};