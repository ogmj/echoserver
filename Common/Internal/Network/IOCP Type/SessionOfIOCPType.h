#pragma once
#include "NetworkQueueMng.h"
#include "IOResult.h"
using namespace tbb;

extern LPFN_ACCEPTEX               g_lpfnAcceptEx;            
extern LPFN_GETACCEPTEXSOCKADDRS   g_lpfnGetAcceptExSockaddrs;

class CSessionOfIOCPType
{
public:
	CSessionOfIOCPType();
	CSessionOfIOCPType( int nId, HANDLE hIOCP );

	virtual ~CSessionOfIOCPType();

	bool AcceptEx( SOCKET sListener, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity );
	bool Connect( WORD wRemotePort, DWORD dwRemoteIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity );
	bool Send( int nId, DWORD dwLen, const char * src );
	bool Peek( int nId, DWORD & dwLen );
	bool Recv( int nId, DWORD dwLen, char* dest );
	bool Close( const MESSAGE_INFO& mi );
	bool Close( int nEvent );
	bool Disconnect( const MESSAGE_INFO& mi );
	bool Disconnect( int nEvent );

	void OnIOCompletion( int nEvent, DWORD dwRet, ULONG_PTR dwKey, DWORD dwTs );

private:

	static  u_int WINAPI ConnectThread ( void * param );
	bool RequestRecv( DWORD dwLen );
	bool RequestRecv();
	bool RequestSend( DWORD dwLen );

	int m_nId;
	HANDLE m_hIOCP;
	SOCKET m_socket;
	SOCKET m_sListener;

	HANDLE m_hConnectThread;

	WSABUF m_wsabufSend;
	WSABUF m_wsabufRecv;

	DWORD m_dwRemoteIP;
	WORD m_wRemotePort;
	DWORD m_dwLocalIP;
	WORD m_wLocalPort;

	DWORD m_dwSendBufCapacity;
	DWORD m_dwRecvBufCapacity;

	IORESULT m_ioAccept;
	IORESULT m_ioRecv;
	IORESULT m_ioConnect;
	IORESULT m_ioSend;

	char m_bufAccept[ 256 ];

	CNetworkQueueMng m_sendQueue;
	CNetworkQueueMng m_recvQueue;
	int m_nSessionErrorCode;
	DWORD m_dwSystemErrorCode;
};

typedef concurrent_hash_map < int, CSessionOfIOCPType* > CMapSessionOfIOCPType;
typedef concurrent_hash_map < int, CSessionOfIOCPType* >::iterator CMapSessionOfIOCPTypeItor;

class CSessionOfIOCPTypeMng
{
public:
	CSessionOfIOCPTypeMng() {};
	virtual ~CSessionOfIOCPTypeMng() {};

	bool Find_Const( int nId, CSessionOfIOCPType * &pSession );
	void Insert( int nId, CSessionOfIOCPType * pSession );
	void Erase( int nId );
	void Clear();

private:
	CMapSessionOfIOCPType m_mapSessionOfIOCPType;
};

