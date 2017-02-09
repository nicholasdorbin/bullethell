#include "Engine/Net/Session/NetPacket.hpp"

void NetPacket::SetContentSizeFromBuffer()
{
	m_currentContentSize = PACKET_MTU;
	m_offset = 0;

	uint8_t fromConnIndex;
	uint8_t numMessages;
	uint16_t nextSentAckID;
	uint16_t highestReceivedAckID;
	uint16_t prevHighestReceivedAcksBitfield;

	size_t totalSize = Read<uint8_t>(&fromConnIndex); 
	
	totalSize += Read<uint16_t>(&nextSentAckID);
	totalSize += Read<uint16_t>(&highestReceivedAckID);
	totalSize += Read<uint16_t>(&prevHighestReceivedAcksBitfield);


	totalSize += Read<uint8_t>(&numMessages);


	for (uint8_t i = 0; i < numMessages; i++)
	{
		//Read the next message's length
		short msgLen;
		totalSize += Read<short>(&msgLen);
		size_t msgLenSize = (size_t)msgLen;
		m_offset += msgLenSize;
		totalSize += msgLenSize;
	}

	m_currentContentSize = totalSize;
	m_offset = 0;
}
