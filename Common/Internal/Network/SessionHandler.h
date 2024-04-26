#pragma once
#include "ServerPacketHandler.h"

typedef concurrent_hash_map < SOCKET, int > CMapToGetIndexBySocketMng;

class CSessionHandler
{
public:
	CSessionHandler();
	virtual ~CSessionHandler();

	bool	Startup( int nType, DWORD nMaxSession, DWORD dwKeepAliveTime, DWORD dwThreadPerProcessor = 4 );
	bool	Accept( DWORD dwMaxAccept, WORD wLocalPort, DWORD dwLocalIP = INADDR_ANY, DWORD dwSendBufCapacity = MAX_PACKET_SIZE, DWORD dwRecvBufCapacity = MAX_PACKET_SIZE );
	bool	Connect( WORD wRemotePort, DWORD dwRemoteIP, DWORD dwSendBufCapacity = MAX_PACKET_SIZE, DWORD dwRecvBufCapacity = MAX_PACKET_SIZE );
	bool	Close( const MESSAGE_INFO& mi );

	void	InsertPacketHandler( CBasePacketHandler* p );
	void	Process();
	void	ProcessAsMsg();
	void	ProcessPacket();
	int		SizeOfQueue()
	{
		return static_cast<int>( CSingleton<CMessageQueueMng>::GetInstance()->Size() );
	}


	virtual bool IsPrivateIP( DWORD dwIP )
	{
		DWORD dwHost = ::ntohl(dwIP);
		DWORD dwIP1, dwIP2;

		dwIP1 = ::ntohl( inet_addr("10.0.0.0") );
		dwIP2 = ::ntohl( inet_addr("10.255.255.255") );
		if(dwHost >= dwIP1 && dwHost <= dwIP2) 
			return true;

		dwIP1 = ::ntohl( inet_addr("172.16.0.0") );
		dwIP2 = ::ntohl( inet_addr("172.31.255.255") );
		if(dwHost >= dwIP1 && dwHost <= dwIP2) 
			return true;

		dwIP1 = ::ntohl( inet_addr("192.168.0.0") );
		dwIP2 = ::ntohl( inet_addr("192.168.255.255") );
		if( dwHost >= dwIP1 && dwHost <= dwIP2 ) 
			return true;
		return false;
	}


	virtual bool GetPublicIP( DWORD & dwIP )
	{
		char szName[255];
		if(SOCKET_ERROR == ::gethostname(szName, sizeof(szName))) 
		{
			int err = ::WSAGetLastError();
			return false;
		}

		PHOSTENT pHostinfo = ::gethostbyname(szName);
		if(NULL == pHostinfo) 
		{
			int err = ::WSAGetLastError();
			return false;
		}

		DWORD ip = INADDR_NONE;
		for( int i = 0; pHostinfo->h_addr_list[i]; ++i ) 
		{
			ip = ( (struct in_addr far *)(pHostinfo->h_addr_list[i])) ->S_un.S_addr;
			if ( false == IsPrivateIP(ip) )
			{
				dwIP = ip;
				return true;
			}
		}

		return false;
	}

	virtual bool GetPrivateIP( DWORD & dwIP )
	{
		char szName[255];
		if(SOCKET_ERROR == ::gethostname(szName, sizeof(szName))) 
		{
			int err = ::WSAGetLastError();
			return false;
		}

		PHOSTENT pHostinfo = ::gethostbyname(szName);
		if(NULL == pHostinfo) 
		{
			int err = ::WSAGetLastError();
			return false;
		}

		DWORD ip = INADDR_NONE;
		for(int i = 0; pHostinfo->h_addr_list[i]; ++i) 
		{
			ip = ( (struct in_addr far *)(pHostinfo->h_addr_list[i]) )->S_un.S_addr;
			if ( true == IsPrivateIP( ip ) )
			{
				dwIP = ip;
				return true;
			}
		}
		return false;
	}

	virtual bool ConvertIpAddress( const char* pszSrc, DWORD& dwDest )
	{
		PHOSTENT pHostinfo = ::gethostbyname(pszSrc);
		if(NULL == pHostinfo) 
		{
			int err = ::WSAGetLastError();
			return false;
		}

		//	DWORD ip = INADDR_NONE;
		for(int i = 0; pHostinfo->h_addr_list[i]; ++i) 
		{
			dwDest = ((struct in_addr far *)(pHostinfo->h_addr_list[i]))->S_un.S_addr;
			return true;
		}
		return false;
	}


	virtual bool ConvertIpAddress( DWORD dwRemoteIP, char * szAddr )
	{
		if( NULL == szAddr )
			return false;
		IN_ADDR addr;
		addr.S_un.S_addr = dwRemoteIP;
		const char* p = inet_ntoa( addr );
		if( NULL == p )
			return false;
//		::strcpy( szAddr, p );
		strcpy_s( szAddr, 16, p );
		return true;
	}

protected:
	virtual	void OnAccept( const MESSAGE_INFO& mi ) = 0;
	virtual void OnConnect( const MESSAGE_INFO& mi ) = 0;
	virtual void OnConnectFail( const MESSAGE_INFO& mi ) = 0;
	virtual void OnClose( const MESSAGE_INFO& mi ) = 0;

	void	TestType( int nType );
private:
	vector< CBasePacketHandler* > m_packetHandlers;
};