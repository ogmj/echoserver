#pragma once

#include "Type.h"
#include "SessionOfIOCPType.h"
#include "AcceptorOfIOCPType.h"
using namespace tbb;

class CIOCPWorker;
class CAcceptorOfIOCPType;
typedef concurrent_vector < CIOCPWorker *> CVectorIOCPWorkerMng;
typedef concurrent_vector < CAcceptorOfIOCPType *> CVectorAcceptorOfIOCPTypeMng;


class CIOCPType : public CType
{
public:
	CIOCPType();
	virtual ~CIOCPType();

	bool Startup( int nMaxSession, DWORD dwThreadPerProcessor );
	bool Accept( int nMaxAccept, WORD wLocalPort, DWORD dwLocalIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity );
	bool Connect( WORD wRemotePort, DWORD wRemoteIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity );
	bool Send( const MESSAGE_INFO & mi, DWORD dwLen, const char * src );
	bool Peek( const MESSAGE_INFO & mi, DWORD & dwLen );
	bool Recv( const MESSAGE_INFO & mi, DWORD dwLen, char * dest );
	bool Close( const MESSAGE_INFO & mi );
	bool Disconnect( const MESSAGE_INFO& mi );

	bool Cleanup();

	static CIOCPType * GetInstance();
	HANDLE	GetCPHandle() const					{ return m_hIOCP; }

private:

	HANDLE m_hIOCP;
	CSessionOfIOCPTypeMng m_mapSessionMng;

	CVectorAcceptorOfIOCPTypeMng m_vecAcceptorMng;
	CVectorIOCPWorkerMng m_vecIOCPWorkerMng;
	int m_nMaxSession;
	DWORD m_dwWorkerCount;
};