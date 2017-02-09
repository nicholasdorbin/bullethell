#include "Engine/Net/Session/NetMessage.hpp"
#include "Engine/Utils/ByteUtils.hpp"
#include "Engine/Net/Session/NetSession.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>



NetMessage::NetMessage(const NetMessage& copyMessage)
{
	//m_messageBuffer, 0, MESSAGE_MTU, BIG_ENDIAN

	memcpy((unsigned char*)m_messageBuffer, copyMessage.m_messageBuffer, MESSAGE_MTU);
	m_buffer = m_messageBuffer;
	m_maxBufferSize = copyMessage.m_maxBufferSize;
	m_currentContentSize = copyMessage.m_currentContentSize;
	m_offset = copyMessage.m_offset;
	m_endianness = copyMessage.m_endianness;

	m_messageType = copyMessage.m_messageType;
	m_definition = copyMessage.m_definition;

	m_sequenceID = copyMessage.m_sequenceID;
	m_reliableID = copyMessage.m_reliableID;
	
}

void NetMessage::ResetOffset()
{
	m_offset = 0;
}


void NetMessage::AssembleMessage(const char* str)
{
	short len = (short)strlen(str) + 2; // + 2 bytes (1 for id 1 for null)
	Write<short>(&len);
	Write<byte_t>(&m_messageType);
	WriteString(str);
}

void NetMessage::AssembleMessage()
{
	if (m_definition == nullptr)
	{
		ERROR_AND_DIE("No defintion set before assembling.");
	}
	short len = 1; //Just ID Type
	Write<short>(&len);
	Write<byte_t>(&m_messageType);
	if (IsReliable())
	{
		Write<uint16_t>(&m_reliableID);
	}
	if (IsInOrder())
	{
		Write<uint16_t>(&m_sequenceID);
	}
}

void NetMessage::FinalizeMessage()
{
	WriteLength();
	WriteType();
	if (IsReliable())
	{
		WriteReliableID();
	}
	if (IsInOrder())
	{
		WriteSequenceID();
	}
}

void NetMessage::WriteLength()
{
	short len = (short)GetTypePayloadSize();
	ResetOffset();
 	OverWrite<short>(m_offset, &len);
	ResetOffset();
}

void NetMessage::WriteType()
{
	ResetOffset();
	m_offset += sizeof(short);
	OverWrite<uint8_t>(m_offset, &m_messageType);
	ResetOffset();
}

void NetMessage::WriteReliableID()
{
	ResetOffset();
	m_offset += sizeof(short) + sizeof(byte_t);
	OverWrite<uint16_t>(m_offset, &m_reliableID);
	ResetOffset();
}

void NetMessage::WriteSequenceID()
{
	ResetOffset();
	m_offset += sizeof(short) + sizeof(byte_t) + sizeof(short); //#FIXME May not work if we ever to InOrder Unreliable
	OverWrite<uint16_t>(m_offset, &m_sequenceID);
	ResetOffset();
}

size_t NetMessage::GetHeaderSize()
{
	size_t baseSize = (sizeof(short) + sizeof(byte_t));
	if (IsReliable())
	{
		baseSize += sizeof(uint16_t);
	}
	if (IsInOrder())
	{
		baseSize += sizeof(uint16_t);
	}

	return baseSize;
}

size_t NetMessage::GetPayloadSize()
{
	return m_currentContentSize - GetHeaderSize();
}

size_t NetMessage::GetTypePayloadSize()
{
	size_t baseSize = GetPayloadSize() + sizeof(byte_t);
	if (IsReliable())
	{
		baseSize += sizeof(uint16_t);
	}
	if (IsInOrder())
	{
		baseSize += sizeof(uint16_t);
	}
	return baseSize;
}

bool NetMessage::IsReliable() const
{
	return ((m_definition->option_flags) &  eOptionFlagID_RELIABLE) != 0;
}

bool NetMessage::IsInOrder() const
{
	return ((m_definition->option_flags) &  (eOptionFlagID_INORDER)) != 0;
}

void NetMessage::WriteStringToMessage(char* data)
{
	if (m_offset < GetHeaderSize())
	{
		m_offset = GetHeaderSize();
		m_currentContentSize = m_offset;
	}

	if (data == nullptr || data == NULL)
	{
		//Write<unsigned char>((unsigned char*)0xff);
	}
	else
	{
		size_t len = strlen(data) + 1;
		WriteForward(data, len);
	}

}