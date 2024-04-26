#pragma once

//rule of naming opcode
//server->client = SC
//client->server = CS
enum PacketType
{
	PACKETTYPE_ERROR			= 0,
	PACKETTYPE_SC_ACCEPT_ERROR,
	PACKETTYPE_SC_ECHO,
	PACKETTYPE_CS_TEST1,
	PACKETTYPE_SC_TEST2,
	PACKETTYPE_CS_ECHO,
};
