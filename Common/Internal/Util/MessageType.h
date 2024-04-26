#pragma once

#define DPSYS_CREATEPLAYERORGROUP   0x0003
#define DPSYS_DESTROYPLAYERORGROUP  0x0005
#define DPSYS_SESSIONLOST           0x0031

typedef struct
{
	DWORD	dwType;         // Message type
	int		nId; 
}DPMSG_CREATEPLAYERORGROUP, FAR *LPDPMSG_CREATEPLAYERORGROUP;

typedef struct
{
	DWORD	dwType;         // Message type
	int		nId;           // player ID being deleted
} DPMSG_DESTROYPLAYERORGROUP, FAR *LPDPMSG_DESTROYPLAYERORGROUP;

typedef struct
{
	DWORD       dwType;         // Message type
} DPMSG_GENERIC, FAR *LPDPMSG_GENERIC;


typedef struct _SESSION_INFO
{
	int		nId;
	DWORD   dwRemoteIP;
	WORD    wRemotePort;
	DWORD   dwLocalIP;
	WORD    wLocalPort;
}SESSION_INFO, *LPSESSION_INFO;

typedef struct _MESSAGE_INFO
{
	enum { ACCEPT, CONNECT, CONNECTFAIL, CLOSE, DB_RESULT };
	DWORD			dwMessageEvent;
	SESSION_INFO	si;
	int				nSessionErrorCode;
	DWORD			dwSystemErrorCode;
	_MESSAGE_INFO(){ ZeroMemory( this, sizeof(*this) ); }
}MESSAGE_INFO, *LPMESSAGE_INFO;

enum
{
	ASE_NONE,					// 정상
	ASE_SOCKET_ERROR,			// 윈속 api에 포함된 에러 dwSystemErrorCode 참조
	ASE_RECV_QUEUE_OVERFOLW,	// 받기 버퍼 오버 플로우
	ASE_SEND_QUEUE_OVERFLOW,	// 보내기 버퍼 오버 플로우
	ASE_NO_MEMORY,				// 메모리 부족
};

enum
{
	DEFAULT_DATA_SIZE = 8192,
	DEFAULT_PACKET_SIZE	= sizeof(DWORD) + DEFAULT_DATA_SIZE,// 4byte + default data size 
	MAX_DATA_SIZE = 131072,
	MAX_PACKET_SIZE	= sizeof(DWORD) + MAX_DATA_SIZE,		// 4byte + max data size 
};

enum
{
	WM_TYPE = 0,
	IOCP_TYPE,
};

enum
{
	WM_SOCKET = WM_USER + 1,
};

enum
{
	MAX_WSA_BUF = 1048576,	//네트웤 버퍼는 기본값을 1메가로 한다.
};