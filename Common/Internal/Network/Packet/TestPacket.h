#pragma once
#include "BasePacket.h"

class CPacket_SC_Test2 : public CBasePacket
{
public:
	CPacket_SC_Test2() : dwTest1(0), dwTest2(0), dwTest3(0)
	{
		dwPacketType = PACKETTYPE_SC_TEST2;
		nSize = sizeof( *this );
	}
	~CPacket_SC_Test2() {};

	DWORD dwTest1;
	DWORD dwTest2;
	DWORD dwTest3;
};

class CPacket_CS_Echo : public CBasePacket
{
public:
	CPacket_CS_Echo() : dwTick(0), string("")
	{
		dwPacketType = PACKETTYPE_CS_ECHO;
		nSize = sizeof( *this );
	}
	~CPacket_CS_Echo() {};

	//DWORD dwTime;
	DWORD dwTick;
	std::string string;

};
