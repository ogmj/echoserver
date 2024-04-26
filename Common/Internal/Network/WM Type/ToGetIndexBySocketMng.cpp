#include "stdafx.h"
#include "ToGetIndexBySocketMng.h"

CToGetIndexBySocketMng::CToGetIndexBySocketMng()
{

}

CToGetIndexBySocketMng::~CToGetIndexBySocketMng()
{

}

bool CToGetIndexBySocketMng::Find( SOCKET s, int & nIndex)
{
	bool bResult;
	CMapToGetIndexBySocketMng::accessor accessor;
	bResult = m_mapToGetIndexBySocketMng.find( accessor, s);
	if ( true == bResult )
	{
		nIndex = accessor->second;
	}
	accessor.release();
	return bResult;
}

void CToGetIndexBySocketMng::Insert( SOCKET s, int nIndex )
{
	CMapToGetIndexBySocketMng::accessor accesor;
	if( true == m_mapToGetIndexBySocketMng.insert( accesor, s ) )
	{
		accesor->second = nIndex;
	}
	accesor.release();
}

void CToGetIndexBySocketMng::Erase( SOCKET s )
{
	CMapToGetIndexBySocketMng::accessor accessor;
	if ( true == m_mapToGetIndexBySocketMng.find( accessor, s) )
	{
		m_mapToGetIndexBySocketMng.erase( accessor );
	}
}
