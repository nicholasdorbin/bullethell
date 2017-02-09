#include "Engine/Utils/BytePacker.hpp"
#include "Engine/Utils/ByteUtils.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


BytePacker::BytePacker()
{
	m_endianness = LITTLE_ENDIAN;
}


BytePacker::BytePacker(void* buffer, const size_t currentContentSize, const size_t writeSizeMax, eEndianMode endianess /*= GetLocalEndianess()*/)
{
	m_buffer = buffer;
	m_maxBufferSize = writeSizeMax;
	m_currentContentSize = currentContentSize;
	m_offset = 0u;
	m_endianness = endianess;
}

BytePacker::~BytePacker()
{

}



void BytePacker::WriteString(const char* str)
{
	if (str == nullptr || str == NULL)
	{
		//Write<unsigned char>((unsigned char*)0xff);
	}
	else
	{
		size_t len = strlen(str) + 1;
		WriteForward(str, len);
	}

}


char const* BytePacker::ReadString()
{
	if(GetReadableBytes() < 1)
	{
		return nullptr;
	}
	byte_t c;
	Read<byte_t>(&c);
	if(c == (byte_t)0xff)
	{
		return nullptr;
	}

	else
	{
		char* buffer = (char*) GetHead() - 1;//call in byte packet that returns a pointer to the buffer
		size_t max_size = GetReadableBytes() + 1;
		size_t len = 0;
		while(len < max_size && (c != NULL))
		{
			++len;
			Read<byte_t>(&c);
		}
		if(len <= max_size)
		{
			//AdvanceRead(len);
			return buffer;
		}
		else
		{
			return nullptr;
		}
	}
}



size_t BytePacker::AdvanceWrite(size_t size)
{
	m_offset += size;
	return m_offset;
}

size_t BytePacker::AdvanceRead(size_t size) const
{
	m_offset += size;
	return m_offset;
}

void* BytePacker::GetHead() const
{
	return ((unsigned char*)m_buffer + m_offset);
}

size_t BytePacker::GetReadableBytes() const
{
	return m_currentContentSize - m_offset;
}

size_t BytePacker::GetTotalReadableBytes() const
{
	return m_currentContentSize;
}

size_t BytePacker::GetWriteableBytes() const
{
	return GetTotalWriteableBytes() - m_offset;
}

size_t BytePacker::GetTotalWriteableBytes() const
{
	return m_maxBufferSize;
}

size_t BytePacker::WriteForward(const void* data, const size_t dataSize)
{
	memcpy((unsigned char*)m_buffer + m_offset, data, dataSize);
	AdvanceWrite(dataSize);
	m_currentContentSize += dataSize;
	return dataSize;
}

size_t BytePacker::WriteBackward(const void* data, const size_t dataSize)
{
	unsigned char* holder = (unsigned char*)m_buffer + m_offset;
	unsigned char* read = (unsigned char*)data + (dataSize - 1);

	for (size_t index = 0; index < dataSize; ++index)
	{
		*((unsigned char*)holder) = *((unsigned char*)read);
		++holder;
		--read;
	}

	m_currentContentSize += dataSize;
	AdvanceWrite(dataSize);
	return dataSize;
}

void BytePacker::RewindWrite(const size_t& amount)
{
	//m_offset -= amount;
	m_currentContentSize -= amount;
}

size_t BytePacker::ReadForward(void* outData, const size_t dataSize) const
{
	memcpy((unsigned char*)outData, (unsigned char*)m_buffer + m_offset, dataSize);
	AdvanceRead(dataSize);
	return dataSize;
}

size_t BytePacker::ReadBackwards(void* outData, const size_t dataSize) const
{
	unsigned char* holder = (unsigned char*)outData;
	unsigned char* read = (unsigned char*)m_buffer + m_offset + (dataSize - 1);

	for (size_t index = 0; index < dataSize; ++index)
	{
		*((unsigned char*)holder) = *((unsigned char*)read);
		++holder;
		--read;
	}

	
	AdvanceRead(dataSize);
	return dataSize;
}


