#include "stdafx.h"
#include "WMType.h"
#include "SessionIndexMng.h"
#include "ToGetIndexBySocketMng.h"

static  LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static HANDLE	m_hEvent = NULL;
static HWND		m_hWnd = NULL;


CWMType::CWMType()
{
	m_nMaxSession = -1;
}

CWMType::~CWMType()
{
	Cleanup();
}

CWMType * CWMType::GetInstance()
{
	return CSingleton<CWMType>::GetInstance();
}

bool CWMType::Startup( int nMaxSession, DWORD dwThreadPerProcessor )
{
	WSADATA wd;
	//소켓 시작
	int ret = ::WSAStartup(MAKEWORD(2,2), &wd);
	if ( 0 != ret) 
	{
		//LOG( FORMAT( "WSAStartup fail %d",ret));
		return false;
	}

	m_hEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL);
	if (NULL == m_hEvent)
	{
		//LOG( FORMAT("CreateEvent fail %d", ::GetLastError()));
		return false;
	}

	//네트웤 메시지 처리 스레드 생성
	m_hThreadHandle = (HANDLE)::_beginthreadex( NULL, 0, Work, reinterpret_cast< LPVOID >( this ), 0, reinterpret_cast< u_int* >( &m_dwThreadId ) );
	if (NULL == m_hThreadHandle)
	{
		DWORD err = ::GetLastError();
		//LOG( FORMAT("CreateThread fail %d", err));
		return false;
	}
	::WaitForSingleObject( m_hEvent, INFINITE );         // wait for window create
	::ResetEvent( m_hEvent );                            // for ending thread.


	//각 세션들을 미리 생성
	for ( int nId = nMaxSession; nId > 0; --nId ) 
	{
		CSessionOfWMType* p  = new CSessionOfWMType( nId );
		if (NULL == p) 
		{
			//LOG("new fail");
			Cleanup();
			return false;
		}
		m_mapSessionMng.Insert( nId, p );
		CSingleton<CSessionIndexMng>::GetInstance()->Push( nId );
	}

	m_nMaxSession = nMaxSession;
	return true;
}

bool CWMType::Cleanup()
{
	::SendMessage( m_hWnd, WM_DESTROY, 0, 0) ;               // send quit message to thread
	::WaitForSingleObject( m_hEvent, INFINITE );             // wait until thread exit

	// close listen sockets
	m_mapAcceptorMng.Clear();

	for( int nId = m_nMaxSession; nId > 0; --nId )
	{
		CSessionOfWMType* p = NULL;
		if( true == m_mapSessionMng.Find_Const( nId, p ) )
		{
			delete p;
		}
	}
	m_mapSessionMng.Clear();

	if ( NULL != m_hThreadHandle )
		::CloseHandle( m_hThreadHandle );
	if ( NULL != m_hEvent )
		::CloseHandle( m_hEvent );

	::WSACleanup();
	return true;
}

u_int CWMType::Work (LPVOID /*param*/)
{
	//CManager2* manager = (CManager2*) param;
	//assert(manager);


	HINSTANCE hInstance = (HINSTANCE) ::GetModuleHandle(NULL); // for creating window
	if (NULL == hInstance)
	{
		//LOG( FORMAT("GetModuleHandle fail %d", ::GetLastError()));
		return 2;
	}

	WNDCLASSEX wcex		= { 0 };
	//ZeroMemory(&wcex, sizeof(wcex));

	wcex.cbSize         = sizeof(WNDCLASSEX); 
	wcex.style			    = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	  = (WNDPROC)WndProc;                     // callback function. 
	wcex.hInstance	    = hInstance;
	wcex.lpszClassName	= "WINDOW_MESSAGE_CLASS";
	//wcex.cbClsExtra		= 0;
	//wcex.cbWndExtra		= 0;
	//wcex.hIcon			  = LoadIcon(hInstance, (LPCTSTR)IDI_CREATEWINDOW);
	//wcex.hCursor		  = LoadCursor(NULL, IDC_ARROW);
	//wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	//wcex.lpszMenuName	= (LPCTSTR)IDC_CREATEWINDOW;
	//wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	if (0 == ::RegisterClassEx(&wcex))
	{
		//LOG( FORMAT("RegisterClassEx fail %d", ::GetLastError()));
		return 3;
	}

	m_hWnd = CreateWindow(
		wcex.lpszClassName, 
		0/*szTitle*/,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 
		0, 
		CW_USEDEFAULT, 
		0, 
		NULL, 
		NULL, 
		hInstance, 
		NULL);

	if (NULL == m_hWnd)
	{
		//LOG( FORMAT( "RegisterClassEx fail %d", ::GetLastError()));
		return 4;
	}

	::SetEvent( m_hEvent);    // window is created. 
	// message loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{		
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	::SetEvent( m_hEvent);    // message loop ended and this thread will end.
	return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_SOCKET:
		{
			SOCKET s    = (SOCKET) wParam;
			int   event = WSAGETSELECTEVENT(lParam);
			int   error = WSAGETSELECTERROR(lParam);
			CWMType::GetInstance()->SocketProc( s, event, error); // toss socket event to manager
			return 0;
//			return ::DefWindowProc(hWnd, message, wParam, lParam);
		}
	case WM_DESTROY:
		::PostQuitMessage(0);                       // here comes terminator!
		return 0;
	default: 
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
}

// process socket event
void CWMType::SocketProc( SOCKET s, DWORD event, DWORD error)
{
	int nId;
	if( true == CSingleton<CToGetIndexBySocketMng>::GetInstance()->Find( s, nId ) )
	{
		// now session's socket event come
		assert( nId > 0);
		CSessionOfWMType * pSession = NULL;
		m_mapSessionMng.Find_Const( nId, pSession );
		if( NULL != pSession )
		{
			pSession->OnIoEvent( event, error );
		}
	}
	else 
	{
		CAcceptorOfWMType Acceptor;
		if( false != m_mapAcceptorMng.Find_Const( s, Acceptor ) )
		{
			// now acceptor's listen socket event come
			if ( FD_ACCEPT == event)
			{
				if ( 0 == error)
				{
					int nNewId = CSingleton<CSessionIndexMng>::GetInstance()->Pop();
					if( nNewId < 0 ) 
					{
						//LOG( "CManager2::Accept fail. not enough session");
						CSessionOfWMType SessionError;
						SessionError.AcceptError( s, m_hWnd );
						return;
					}
					CSessionOfWMType * pSession = NULL;
					m_mapSessionMng.Find_Const( nNewId, pSession );
					if( NULL != pSession )
					{
						pSession->Accept( s, m_hWnd );
					}
				}
				else
				{
					//LOG( FORMAT( "FD_ACCEPT error %d", error));
				}
			}
			else if ( FD_CLOSE == event)
			{
				//LOG( FORMAT( " listen socket fail %d", error));
			}
		}
	}
}

bool CWMType::Accept( int nMaxAccept, WORD wLocalPort, DWORD dwLocalIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity )
{
	SOCKET s = ::socket( AF_INET, SOCK_STREAM,IPPROTO_TCP );
	if (INVALID_SOCKET == s) 
	{
		//LOG( FORMAT( "socket fail %d", ::WSAGetLastError()));
		return false;
	}

	SOCKADDR_IN   addr = { 0 };
	addr.sin_family       = AF_INET;
	addr.sin_addr.s_addr  = dwLocalIP;
	addr.sin_port         = htons( wLocalPort );
	
	// bind
	int ret = ::bind( s, (PSOCKADDR)&addr, sizeof(addr));
	if (SOCKET_ERROR == ret) 
	{
		//LOG( FORMAT( "bind fail %d", ::WSAGetLastError()));
		::closesocket(s);
		return false;
	}

	// listen
	if(SOCKET_ERROR == ::listen( s, SOMAXCONN)) 
	{ 
		//LOG( FORMAT(  "listen fail %d", ::WSAGetLastError()));
		::closesocket(s);
		return false;
	}

	// use window message for checking socket event.
	if ( SOCKET_ERROR ==::WSAAsyncSelect( s, m_hWnd, WM_SOCKET, FD_ACCEPT|FD_CLOSE ) )
	{
		//LOG( FORMAT( "WSAAsyncSelect fail %d", ::WSAGetLastError()));
		::closesocket(s);
		return false;
	}

	CAcceptorOfWMType a;
	a.m_listener = s;
	a.m_sendBufCapacity = dwSendBufCapacity;
	a.m_recvBufCapacity = dwRecvBufCapacity;
	m_mapAcceptorMng.Insert( s, a );

	return true;
}

bool CWMType::Connect ( WORD wRemotePort, DWORD wRemoteIP, DWORD dwSendBufCapacity, DWORD dwRecvBufCapacity )
{
	int nId = CSingleton<CSessionIndexMng>::GetInstance()->Pop();
	bool bResult = false;
	if (nId < 0)
	{
		//LOG( FORMAT( "Connect fail because session is full %s, %d", ipAddr, remotePort));
		return bResult;
	}
	CSessionOfWMType * pSession = NULL;
	m_mapSessionMng.Find_Const( nId, pSession );
	if( NULL != pSession )
	{
		bResult = pSession->Connect( m_hWnd, wRemotePort, wRemoteIP, dwSendBufCapacity, dwRecvBufCapacity);
	}
	return bResult;
}

bool CWMType::Send( const MESSAGE_INFO& mi, DWORD dwLen, const char * src )
{
	bool bResult = false;
	if ( mi.si.nId > 0 && mi.si.nId <= m_nMaxSession)
	{
		CSessionOfWMType * pSession = NULL;
		m_mapSessionMng.Find_Const( mi.si.nId, pSession );
		if( NULL != pSession )
		{
			bResult = pSession->Send( mi.si.nId, dwLen, src );
		}	
	}
	return bResult;
}

bool CWMType::Close ( const MESSAGE_INFO& mi )
{
	bool bResult = false;
	if ( mi.si.nId > 0 || mi.si.nId <= m_nMaxSession )
	{
		CSessionOfWMType * pSession = NULL;
		m_mapSessionMng.Find_Const( mi.si.nId, pSession );
		if( NULL != pSession )
		{
			bResult = pSession->Close( mi );
		}
	}
	return bResult;
}

bool CWMType::Disconnect( const MESSAGE_INFO& mi )
{
	bool bResult = false;
	if ( mi.si.nId > 0 || mi.si.nId <= m_nMaxSession )
	{
		CSessionOfWMType * pSession = NULL;
		m_mapSessionMng.Find_Const( mi.si.nId, pSession );
		if( NULL != pSession )
		{
			bResult = pSession->Disconnect( mi );
		}
	}
	return bResult;
}

bool CWMType::Recv( const MESSAGE_INFO& mi, DWORD dwLen, char* dest )
{
	bool bResult = false;
	if( mi.si.nId > 0 || mi.si.nId <= m_nMaxSession )
	{
		CSessionOfWMType * pSession = NULL;
		m_mapSessionMng.Find_Const( mi.si.nId, pSession );
		if( NULL != pSession )
		{
			bResult = pSession->Recv( mi.si.nId, dwLen, dest );
		}
	}
	return bResult;
}

bool CWMType::Peek( const MESSAGE_INFO& mi, DWORD & dwLen )
{
	bool bResult = false;
	if( mi.si.nId > 0 || mi.si.nId <= m_nMaxSession )
	{
		CSessionOfWMType * pSession = NULL;
		m_mapSessionMng.Find_Const( mi.si.nId, pSession );
		if( NULL != pSession )
		{
			bResult = pSession->Peek( mi.si.nId, dwLen );
		}
	}
	return bResult;
}
