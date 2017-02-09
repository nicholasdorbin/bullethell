#pragma once
#include "Engine/Utils/BytePacker.hpp"
#include "Engine/Core/IO/EndianUtils.hpp"
#include <cstdint>

struct NetMessageDefinition;
struct NetSender;

#define MESSAGE_MTU 1024
class NetMessage : public BytePacker
{
public:
	uint16_t m_reliableID = 0;
	uint16_t m_sequenceID = 0;
	unsigned char m_messageBuffer[MESSAGE_MTU];
	uint8_t m_messageType;

	NetMessageDefinition const *m_definition;
	// various other old stuff from previous assignments

	// == NEW ==
	// Reliable ID assigned
	

	// If reliable, time since this was last attempted to be sent
	double m_lastSentTimestampMS = 0;

	NetMessage()
		:BytePacker(m_messageBuffer, 0, MESSAGE_MTU, BIG_ENDIAN)
		, m_messageType(0)
	{};

	NetMessage(uint8_t messageType)
		: BytePacker(m_messageBuffer, 0, MESSAGE_MTU, BIG_ENDIAN)
		, m_messageType(messageType)
	{};

	NetMessage(const NetMessage& copyMessage);

	
	void ResetOffset();
	void AssembleMessage(); //Builds a message with nothing in the dataBuffer
	void AssembleMessage(const char* str); //Builds a message with the string in the dataBuffer
	void FinalizeMessage(); //Set the size bytes to how much stuff we have in the dataBuffer
	void WriteLength();
	void WriteType();
	void WriteReliableID();
	void WriteSequenceID();
	template <typename T>
	void WriteToMessage(void* data); //Writes at the end of our buffer
	template <typename T>
	void ReadFromMessage(void* outData);

	size_t GetHeaderSize();
	size_t GetPayloadSize();
	size_t GetTypePayloadSize();

	bool IsReliable() const;
	bool IsInOrder() const;
	void WriteStringToMessage(char* data);
};

template <typename T>
void NetMessage::WriteToMessage(void* data)
{
	if (m_offset < GetHeaderSize())
	{
		m_offset = GetHeaderSize();
		m_currentContentSize = m_offset;
	}
	Write<T>(data);

}

template <typename T>
void NetMessage::ReadFromMessage(void* outData)
{
	if (m_offset < GetHeaderSize())
	{
		m_offset = GetHeaderSize();
		
	}
	Read<T>(outData);

}


