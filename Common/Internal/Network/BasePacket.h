#pragma once

class CBasePacket
{
public:
	CBasePacket() :dwPacketType(0), nSize(0){};
	virtual ~CBasePacket() {};

	DWORD dwPacketType;
	unsigned int nSize;
};

class CPacket_SC_Echo : public CBasePacket
{
public:
	CPacket_SC_Echo() : dwTick(0)
	{ 
		dwPacketType = PACKETTYPE_SC_ECHO;
		nSize = sizeof( *this );
	};
	~CPacket_SC_Echo() {};

	DWORD dwTick;
};

class CPacket_CS_Test1 : public CBasePacket
{
public:
	CPacket_CS_Test1() : dwTest1(0)
	{ 
		dwPacketType = PACKETTYPE_CS_TEST1;
		nSize = sizeof( *this );
	};
	~CPacket_CS_Test1() {};

	DWORD dwTest1;
};

class CPacket_SC_Test1 : public CBasePacket
{
public:
	CPacket_SC_Test1() : dwTest1(0), dwTest2(0), dwTest3(0)
	{ 
		dwPacketType = PACKETTYPE_CS_TEST1;
		nSize = sizeof( *this );
	};
	~CPacket_SC_Test1() {};

	DWORD dwTest1;
	DWORD dwTest2;
	DWORD dwTest3;

};

class CPacket_SC_AcceptError : public CBasePacket
{
public:
	CPacket_SC_AcceptError() 
	{ 
		dwPacketType = PACKETTYPE_SC_ACCEPT_ERROR;
		nSize = sizeof( *this );
	};
	~CPacket_SC_AcceptError() {};
};