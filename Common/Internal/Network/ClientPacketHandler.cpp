#include "stdafx.h"
#include "ClientPacketHandler.h"
#include "TypeMng.h"
#include "BasePacket.h"

CClientPacketHandler::CClientPacketHandler()
{
	::ZeroMemory( &m_mi, sizeof( m_mi ) );
}

CClientPacketHandler::~CClientPacketHandler()
{
	::ZeroMemory( &m_mi, sizeof( m_mi ) );
}

void CClientPacketHandler::OnConnect( const MESSAGE_INFO& mi )
{
	m_mi = mi;
}

void CClientPacketHandler::OnClose()
{
	::ZeroMemory( &m_mi, sizeof( m_mi ) );
}

void CClientPacketHandler::ProcessPacket()
{
	static char recvBuf[ MAX_PACKET_SIZE ];
//	if ( 0 == m_mi.si.nId)
//	{
//		return;
//	}

	for( ;; )
	{
		DWORD dwDataSize = 0;
		if ( false == CSingleton< CTypeMng >::GetInstance()->Peek( m_mi, dwDataSize ) )
		{
//			LOGTRACE("ProcessPacket error ::: Peek == false");
			break;
		}
		if ( dwDataSize > MAX_DATA_SIZE)
		{
//			LOGTRACE("ProcessPacket error ::: dataSize > MAX_DATA_SIZE");
			//AS::PostRecv( mi );
			break;
		}
		if ( false == CSingleton< CTypeMng >::GetInstance()->Recv( m_mi, dwDataSize, recvBuf ) )
		{
//			LOGTRACE("ProcessPacket error ::: Recv == false");
			break;
		}

		//by ogmj : 매개변수를 포인터로 넘기면, 하위 함수에서 계속 널 검사를 해야 하지만, 참조로 넘기면 널 검사를 할 필요가 없어진다.
		CBasePacket & BasePacket = *( reinterpret_cast < CBasePacket * >( recvBuf ) );

		UserMessageHandler( BasePacket, m_mi.si.nId );
	}
}

bool CClientPacketHandler::Send( CBasePacket * pPacket, int nFromId )
{
	bool bResult = FALSE;

	if ( nullptr == pPacket || 0 == pPacket->nSize )
		return bResult;
	if ( 0 == TO_ALL )
	{
		//return SendToAll( pBuffer, dwLen );
	}
	bResult = CSingleton< CTypeMng >::GetInstance()->Send( m_mi, pPacket->nSize, reinterpret_cast<char *>( pPacket ) );
	return bResult;
}

bool CClientPacketHandler::Disconnect()
{
	return CSingleton< CTypeMng >::GetInstance()->Disconnect( m_mi );
}
