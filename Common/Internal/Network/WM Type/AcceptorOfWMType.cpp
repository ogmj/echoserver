#include "stdafx.h"
#include "AcceptorOfWMType.h"

bool CAcceptorOfWMTypeMng::Find_Const( SOCKET s, CAcceptorOfWMType & Acceptor )
{
	bool bResult;
	CMapAcceptor::const_accessor const_accessor;
	bResult = m_mapAcceptor.find( const_accessor, s );
	if ( true == bResult )
	{
		Acceptor = const_accessor->second;
	}
	const_accessor.release();
	return bResult;
}

void CAcceptorOfWMTypeMng::Insert( SOCKET s, CAcceptorOfWMType Acceptor )
{
	CMapAcceptor::accessor accesor;
	if( true == m_mapAcceptor.insert( accesor, s ) )
	{
		accesor->second = Acceptor;
	}
	accesor.release();
}

void CAcceptorOfWMTypeMng::Erase( SOCKET s )
{
	CMapAcceptor::accessor accessor;
	if ( true == m_mapAcceptor.find( accessor, s ) )
	{
		::closesocket( accessor->first );
		m_mapAcceptor.erase( accessor );
	}
}

void CAcceptorOfWMTypeMng::Clear()
{
	CMapAcceptorItor iter;
	for( iter = m_mapAcceptor.begin(); iter != m_mapAcceptor.end(); ++iter )
	{
		::closesocket( iter->first );
	}
	m_mapAcceptor.clear();
}