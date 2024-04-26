#pragma once
using namespace tbb;

class CAcceptorOfWMType
{
public:
	CAcceptorOfWMType() : m_listener(INVALID_SOCKET), m_sendBufCapacity(0), m_recvBufCapacity(0)
	{
	}
	virtual ~CAcceptorOfWMType() 
	{
		m_listener = INVALID_SOCKET;
		m_sendBufCapacity = 0;
		m_recvBufCapacity = 0;
	};

	SOCKET  m_listener;
	int     m_sendBufCapacity;
	int     m_recvBufCapacity;
};

typedef concurrent_hash_map < SOCKET, CAcceptorOfWMType > CMapAcceptor;
typedef concurrent_hash_map < SOCKET, CAcceptorOfWMType >::iterator CMapAcceptorItor;

class CAcceptorOfWMTypeMng
{
public:
	CAcceptorOfWMTypeMng() {};
	virtual ~CAcceptorOfWMTypeMng() {};

	bool Find_Const( SOCKET s, CAcceptorOfWMType & Acceptor );
	void Insert( SOCKET s, CAcceptorOfWMType Acceptor );
	void Erase( SOCKET s );
	void Clear();

private:
	CMapAcceptor m_mapAcceptor;
};