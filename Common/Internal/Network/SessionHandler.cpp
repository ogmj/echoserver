#include "stdafx.h"
#include "SessionHandler.h"
#include "MessageQueueMng.h"
#include "TypeMng.h"


#ifdef _DEBUG 
#undef THIS_FILE 
static char THIS_FILE[]=__FILE__; 
//#define new DEBUG_NEW  
#endif

CSessionHandler::CSessionHandler()
{
}


CSessionHandler::~CSessionHandler()
{

}


bool CSessionHandler::Startup( int nType, DWORD nMaxSession, DWORD dwKeepAliveTime, DWORD dwThreadPerProcessor )
{
	if ( IOCP_TYPE == nType)
	{
		if ( 0 == nMaxSession || 0 >= dwThreadPerProcessor ) 
		{
			//LOG( FORMAT( "Startup invalid parameter max session:%d thread per processor:%d", maxSession, threadPerProcessor));
			return false;
		}
	}
	else if ( WM_TYPE == nType)
	{
		if ( 0 == nMaxSession ) 
		{
			//LOG( FORMAT( "Stratup invalid parameter max session:%d", maxSession));
			return false;
		}
	}

	CSingleton< CTypeMng >::GetInstance()->SelectAInstance( nType );

	if( false == CSingleton< CTypeMng >::GetInstance()->Startup( nMaxSession, dwThreadPerProcessor ) )
	{
		return false;
	}

	return true;
}

bool CSessionHandler::Accept( DWORD dwMaxAccept, WORD wLocalPort, DWORD dwLocalIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity)
{
	return CSingleton< CTypeMng >::GetInstance()->Accept( dwMaxAccept, wLocalPort, dwLocalIP, dwSendBufCapacity, dwRecvBufCapacity );
}

bool CSessionHandler::Connect( WORD wRemotePort, DWORD dwRemoteIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity)
{
	return CSingleton< CTypeMng >::GetInstance()->Connect( wRemotePort, dwRemoteIP, dwSendBufCapacity, dwRecvBufCapacity );
}

bool CSessionHandler::Close( const MESSAGE_INFO& mi )
{
	return CSingleton< CTypeMng >::GetInstance()->Close( mi );
}

void CSessionHandler::Process()
{
	ProcessAsMsg();
	ProcessPacket();
}

void CSessionHandler::ProcessAsMsg()
{
	MESSAGE_INFO * pMsg = nullptr;
	while( 1 )
	{
		pMsg = CSingleton<CMessageQueueMng>::GetInstance()->Pop();
		if( nullptr != pMsg )
		{
			switch( pMsg->dwMessageEvent )
			{
			case MESSAGE_INFO::ACCEPT:   // an accept is made successfully
				{
					OnAccept( *pMsg );
					break;
				}
			case MESSAGE_INFO::CONNECT: // making a connection
				{
					OnConnect( *pMsg );
					break;
				}
			case MESSAGE_INFO::CONNECTFAIL: // making a connection failed.
				{
					OnConnectFail( *pMsg );
					break;
				}
			case MESSAGE_INFO::CLOSE:       // connection is closed 
				{
					OnClose( *pMsg );
					break;
				}
				//case MESSAGE_INFO::DB_RESULT:
				//	{
				//		pMsg->pDBResult->OnDBResult();
				//		break;
				//	}
			default:
				assert(0);
				break;
			}
			delete pMsg;
		}
		else
		{
			break;
		}
	}
}

void CSessionHandler::InsertPacketHandler( CBasePacketHandler* p )
{
	if ( p )
		m_packetHandlers.push_back(p);
}
void CSessionHandler::ProcessPacket()
{
	vector<CBasePacketHandler*>::iterator it  = m_packetHandlers.begin();
	vector<CBasePacketHandler*>::iterator end = m_packetHandlers.end();
	for( ; it != end; ++it )
	{
		CBasePacketHandler*  p = *it;
		if ( p )
			p->ProcessPacket();
	}
}


void CSessionHandler::TestType( int nType )
{
	//m_TypeMng.SelectAInstance( nType );
	CSingleton< CTypeMng >::GetInstance()->SelectAInstance( nType );
}
