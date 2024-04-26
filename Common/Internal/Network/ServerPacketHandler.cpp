
#include "stdafx.h"
#include "ServerPacketHandler.h"
#include "TypeMng.h"
#include "BasePacket.h"

void CServerPacketHandler::ProcessPacket()
{
	static char recvBuf[MAX_PACKET_SIZE];

	for( CMapID2MIMngItor it =  m_Id2MIMng.begin(); it != m_Id2MIMng.end() ; ++it )
	{
		MESSAGE_INFO& mi = it->second;

		for( ;; )
		{
			DWORD dataSize = 0;
			if( false == CSingleton< CTypeMng >::GetInstance()->Peek( mi, dataSize ) )
				break;

			if ( dataSize > MAX_DATA_SIZE)
			{
				break;
			}

			if (false == CSingleton< CTypeMng >::GetInstance()->Recv( mi, dataSize, recvBuf ) )
				break;
			CBasePacket & BasePacket = *( reinterpret_cast < CBasePacket * >( recvBuf ) );
			UserMessageHandler( BasePacket, mi.si.nId );
		}
	}
}

void CServerPacketHandler::OnAccept( const MESSAGE_INFO& mi )
{
	CMapID2MIMng::accessor accessor;
	if( true == m_Id2MIMng.insert( accessor, mi.si.nId ) )
	{
		accessor->second = mi;
	}
	accessor.release();
}

void CServerPacketHandler::OnClose( const MESSAGE_INFO& mi )
{
	CMapID2MIMng::accessor accesor;
	if( true == m_Id2MIMng.find( accesor, mi.si.nId ) )
	{
		m_Id2MIMng.erase( mi.si.nId );
	}
	accesor.release();
}

bool CServerPacketHandler::Send( CBasePacket * pPacket, int nFromId )
{
	bool bResult = FALSE;

	if ( NULL == pPacket || 0 == pPacket->nSize )
		return bResult;
	//	if ( DPID_ALLPLAYERS == dwFrom )
	if ( 0 == nFromId )
	{
		//return SendToAll( pBuffer, dwLen );
	}
	CMapID2MIMng::accessor accessor;
	if( true == m_Id2MIMng.find( accessor, nFromId ) )
	{
		bResult = CSingleton< CTypeMng >::GetInstance()->Send( accessor->second, pPacket->nSize, reinterpret_cast<char *>( pPacket ) );
	}

	accessor.release();
	return bResult;
}

bool CServerPacketHandler::SendToAll( char* pBuffer, DWORD dwLen )
{
	if ( 0 == dwLen || NULL == pBuffer)
		return FALSE;

	for( CMapID2MIMngItor it = m_Id2MIMng.begin(); it != m_Id2MIMng.end(); ++it )
	{
		CSingleton< CTypeMng >::GetInstance()->Send( it->second, dwLen, pBuffer );
	}
	return TRUE;
}

// <버퍼의 사이즈 (4byte) + 버퍼)> 를 전송한다. 
// pBuffer : 버퍼
// dwLen  : 버퍼 사이즈 
bool CServerPacketHandler::SendPacket( char* pBuffer, DWORD dwLen, int nFromId )
{
	bool bResult = FALSE;

	if ( 0 == dwLen || NULL == pBuffer )
		return FALSE;

	static char buffer[MAX_PACKET_SIZE];
	memcpy( buffer, &dwLen, sizeof(dwLen) );
	memcpy( buffer+sizeof(dwLen), pBuffer, dwLen );

	//if ( DPID_ALLPLAYERS == dwId)
	if ( 0 == nFromId )
	{
		return SendToAll( buffer, sizeof(dwLen)+dwLen );
	}

	CMapID2MIMng::const_accessor accessor;
	if( true == m_Id2MIMng.find( accessor, nFromId ) )
	{
		bResult = CSingleton< CTypeMng >::GetInstance()->Send( accessor->second, sizeof(dwLen)+dwLen, pBuffer );
	}
	accessor.release();
	return bResult;
}

bool CServerPacketHandler::Disconnect( int nFromId )
{
	//by ogmj: ? 디스커넥트인데 아이디를 지우지 않아도 되는지 테스트 필요
	bool bResult = FALSE;

	CMapID2MIMng::const_accessor accessor;
	if( true == m_Id2MIMng.find( accessor, nFromId ) )
	{
		bResult = CSingleton< CTypeMng >::GetInstance()->Close( accessor->second );
	}
	accessor.release();
	return bResult;
}

bool CServerPacketHandler::GetPlayerAddr( int nFromId, char* lpAddr )
{
	bool bResult = FALSE;

	if (NULL == lpAddr)
		return FALSE;

	//CMapID2MIMng::const_accessor accessor;
	//if( true == m_Id2MIMng.find( accessor, dwId ) )
	//{
	//	bResult = CSingleton< CTypeMng >::GetInstance()->ConvertIpAddress( (accessor->second).si.dwRemoteIP, lpAddr );
	//}
	//accessor.release();
	return bResult;
}