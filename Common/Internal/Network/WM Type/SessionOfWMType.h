#pragma once
#include "NetworkQueueMng.h"
using namespace tbb;

class CSessionOfWMType
{
public:
	CSessionOfWMType();
	CSessionOfWMType( int nIndex );
	virtual ~CSessionOfWMType();

	bool Accept( SOCKET listener, HWND hWnd );
	bool AcceptError( SOCKET listener, HWND hWnd );
	bool Close( DWORD event );
	bool Close( const MESSAGE_INFO& mi );
	bool Disconnect( DWORD event );
	bool Disconnect( const MESSAGE_INFO& mi );

	void OnIoEvent( DWORD event, DWORD error);

	bool Connect( HWND hWnd, WORD remotePort, DWORD remoteIP, DWORD sendBufCapacity, DWORD recvBufCapacity );
	bool Send( int nId, DWORD dwLen, const char * src );
	bool Recv( int nId, DWORD dwLen, char* dest );
	bool Peek( int nId, DWORD & dwLen );

private:
	bool RequestRecv();
	bool RequestSend( DWORD dwLen );

	int		m_nId;

	SOCKET	m_socket;
	DWORD	m_remoteIP;
	WORD	m_remotePort;
	DWORD	m_localIP;
	WORD	m_localPort;
	int		m_nSessionErrorCode;
	DWORD	m_dwSystemErrorCode;

	WSABUF	m_wsabufSend;
	WSABUF	m_wsabufRecv;
	CNetworkQueueMng    m_sendQueue;
	CNetworkQueueMng    m_recvQueue;
};

typedef concurrent_hash_map < int, CSessionOfWMType* > CMapSessionOfWMType;
typedef concurrent_hash_map < int, CSessionOfWMType* >::iterator CMapSessionOfWMTypeItor;

class CSessionOfWMTypeMng
{
public:
	CSessionOfWMTypeMng() {};
	virtual ~CSessionOfWMTypeMng() {};

	bool Find_Const( int nId, CSessionOfWMType * &pSession );
	void Insert( int nId, CSessionOfWMType * pSession );
	void Erase( int nId );
	void Clear();

private:
	CMapSessionOfWMType m_mapSessionOfWMType;
};
