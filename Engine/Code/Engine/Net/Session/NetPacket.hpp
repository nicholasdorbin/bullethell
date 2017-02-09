#pragma once
#include "Engine/Utils/BytePacker.hpp"
#include "Engine/Core/IO/EndianUtils.hpp"
#include <cstdint>

#define PACKET_MTU 1232


class NetPacket : public BytePacker
{
public:
	unsigned char m_packetBuffer[PACKET_MTU];

	/*
	buffer layout
	struct {
		uint8_t from_conn_index; //This is our connection index, not the person we are sending to.
		uint8_t message_count;
	}

	*/

	struct Header
	{
		uint8_t connectionIndex;
		uint16_t ackID;
		uint16_t highestReceivedAckID;
		uint16_t prevHighestReceivedAcksBitfield;
	};


	Header m_header;
	double m_timeRecv;
	double m_rtt;


	NetPacket()
		: BytePacker(m_packetBuffer, 0, PACKET_MTU, BIG_ENDIAN)
	{};

	void SetContentSizeFromBuffer();
	//#TODO IsValid();
};